#include "Boton.h"

Boton::Boton(int pin) : pin(pin) {}

void Boton::iniciar() {
    pinMode(pin, INPUT_PULLUP);
    stableState = digitalRead(pin);
    lastReading = stableState;
}

int Boton::leerEvento() {
    int reading = digitalRead(pin);

    // 1. FILTRO DE REBOTE (DEBOUNCE)
    // Si la lectura física cambió (ruido o pulsación real), reseteamos el cronómetro
    if (reading != lastReading) {
        lastDebounceTime = millis();
    }
    lastReading = reading;

    // Si ha pasado suficiente tiempo estable, aceptamos el cambio de estado
    if ((millis() - lastDebounceTime) > DEBOUNCE_MS) {
        
        // Si el estado estable cambió respecto a lo que teníamos guardado...
        if (reading != stableState) {
            stableState = reading;

            // --- LÓGICA DE DETECCIÓN ---
            if (stableState == LOW) {
                // FLANCO DE BAJADA (Presionado)
                pressStart = millis();
                pressActive = true;
                longTriggered = false;
            } else {
                // FLANCO DE SUBIDA (Soltado)
                if (pressActive) {
                    pressActive = false;
                    unsigned long duration = millis() - pressStart;

                    // Si soltó RÁPIDO y no se había disparado el evento largo...
                    if (!longTriggered && duration < LONG_MS) {
                        buttonEvent = 1; // CLICK CORTO
                    }
                }
            }
        }
    }

    // 2. DETECCIÓN DE PULSACIÓN LARGA (Mientras se mantiene presionado)
    if (pressActive && !longTriggered) {
        if ((millis() - pressStart) >= LONG_MS) {
            buttonEvent = 2; // CLICK LARGO
            longTriggered = true; // Marcamos para no dispararlo múltiples veces
        }
    }

    // 3. RETORNAR Y LIMPIAR EVENTO
    if (buttonEvent != 0) {
        int ev = buttonEvent;
        buttonEvent = 0; // Limpiamos el evento para no leerlo dos veces
        return ev;
    }

    return 0; // Sin novedad
}

// Métodos auxiliares para consultar estado directo (útil para tu BombaManager)
bool Boton::estaEncendido() {
    return (stableState == LOW);
}

bool Boton::cambioDetectado() {
    // Esta función es un helper simple, pero para tu lógica avanzada 
    // es mejor usar leerEvento() que ya maneja todo.
    return false; 
}