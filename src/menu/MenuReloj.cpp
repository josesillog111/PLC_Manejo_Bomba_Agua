#include "MenuReloj.h"

MenuReloj::MenuReloj(OLED& oled, Boton& boton, Potenciometro& pot, Reloj& reloj)
    : oled(oled), boton(boton), pot(pot), reloj(reloj), estado(NO_MENU) {}


void MenuReloj::procesarEventoBoton() {
    switch (estado) {
        case MENU_AJUSTE_HORA: {
            oled.limpiar();
            int hora = selectorGenerico("Hora", 0, 23, "h");
            int min = selectorGenerico("Minuto", 0, 59, "m");
            reloj.setHora(hora, min, 0);
            delay(1500);
            break;
        }
        case MENU_AJUSTE_FECHA: {
            oled.limpiar();
            int dia = selectorGenerico("Dia", 1, 31, "d");
            int mes = selectorGenerico("Mes", 1, 12, "m");
            int year = selectorGenerico("Anio", 2024, 2035, "a");
            reloj.setFecha(dia, mes, year);
            delay(1500);
            break;
        }   
        case NO_MENU:
        default:
            delay(50);
            oled.limpiar();
            break;
    }
    return;
}

void MenuReloj::mostrarMenu() {
    bool salir = false;

    // Mostrar por primera vez al entrar
    oled.limpiar();
    oled.mostrar("Menu Reloj", 0, 0);
    oled.mostrar(opciones[opcionActual], 0, 1);

    while (!salir) {
        
        int evento = boton.leerEvento();

        // Reset timeout si hubo interacción
        if (evento != 0) {
            ultimaInteraccion = millis();
        }

        // Timeout o oled apagado → salir
        if (millis() - ultimaInteraccion > 15000 || !oled.estaEncendido()) {
            ultimaInteraccion = millis();
            oled.limpiar();
            return;
        }

        if (evento == 1) {
            opcionActual = (opcionActual + 1) % numOpciones;
            oled.limpiar();
            oled.mostrar("Menu Reloj", 0, 0);
            oled.mostrar(opciones[opcionActual], 0, 1);
        } else if (evento == 2) {  
            if (opcionActual == 2) {   // "Salir"
                oled.limpiar();
                estado = NO_MENU;      // <- salir al estado reposo
                salir = true;
            } else {
                estado = (opcionActual == 0) ?  MENU_AJUSTE_HORA : MENU_AJUSTE_FECHA;
                procesarEventoBoton();
                salir = true;
            }
        }
        delay(50); // pequeño respiro, no debounce fuerte
    }
}

int MenuReloj::selectorGenerico(const char* titulo, int minVal, int maxVal, const char* sufijo) {
    bool salir = false;
    ultimaInteraccion = millis();
    int valor = minVal;
    int valorAnterior = -1;

    while (!salir) {
        int evento = boton.leerEvento();
        int nuevoValor = pot.leerEscalado(minVal, maxVal);
        // Reset del timeout con el potenciómetro
        if (evento != 0 || nuevoValor != valorAnterior) {
            ultimaInteraccion = millis();
        }

        if (millis() - ultimaInteraccion > 15000 || !oled.estaEncendido()) {
            oled.limpiar();
            return minVal;
        }

        if (nuevoValor != valorAnterior) {
            valor = nuevoValor;
            valorAnterior = nuevoValor;

            oled.limpiar();
            oled.mostrar(titulo, 0, 0);

            char buffer[17];
            snprintf(buffer, sizeof(buffer), "%d %s", valor, sufijo);
            oled.mostrar(buffer, 0, 1);
        }

        if (evento == 1) {
            salir = true;
        }

        delay(50);
    }

    return valor;
}
