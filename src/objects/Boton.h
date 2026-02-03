#pragma once
#include <Arduino.h>

class Boton {
private:
    int pin;
    
    // ESTADO INTERNO (Cada botón debe tener el suyo propio)
    int stableState = HIGH;   // Estado consolidado (sin rebote)
    int lastReading = HIGH;   // Última lectura física (para debounce)
    
    // Variables de eventos
    int buttonEvent = 0;      // 0=Nada, 1=Corta, 2=Larga
    bool pressActive = false;
    bool longTriggered = false; 

    // Tiempos
    unsigned long pressStart = 0;
    unsigned long lastDebounceTime = 0;

    // Configuración
    const unsigned long DEBOUNCE_MS = 50;   // Aumentado un poco para estabilidad
    const unsigned long LONG_MS     = 1000; // 1 segundo para larga (más natural)

public:
    Boton(int pin);
    void iniciar();
    
    // Devuelve: 0 (nada), 1 (click corto), 2 (click largo)
    int leerEvento();
    
    // Métodos extra útiles para el BombaManager
    bool estaEncendido();     // Devuelve true si el botón está físicamente presionado
    bool cambioDetectado();   // Helper para detectar cambios simples
};