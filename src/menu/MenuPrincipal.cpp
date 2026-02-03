#include "MenuPrincipal.h"

MenuPrincipal::MenuPrincipal(OLED& oled, Boton& boton, MenuBomba& menuBomba, MenuReloj& menuReloj)
    : oled(oled), boton(boton), menuBomba(menuBomba), menuReloj(menuReloj)  {}

// maquina de estados simple
void MenuPrincipal::procesarEventoBoton() {
    switch (estado) {
        case MENU_CONFIG_BOMBA:
            oled.limpiar();
            menuBomba.mostrarMenu();
            delay(1500);
            break;

        case MENU_CONFIG_RELOJ:
            oled.limpiar();
            menuReloj.mostrarMenu();
            delay(1500);
            break;
        case NO_MENU:
        default:
            delay(50);
            oled.limpiar();
            break;

    }
    return;
}

// Mostrar menú principal y manejar navegación
void MenuPrincipal::mostrarMenu() {
    bool salir = false;

    ultimaInteraccion = millis();
    
    // Mostrar por primera vez al entrar
    oled.limpiar();
    oled.mostrar("Menu Principal", 0, 0);
    oled.mostrar(opciones[opcionActual], 0, 1);

    while (!salir) {
        int evento = boton.leerEvento();

        // Reset timeout si hubo interacción
        if (evento != 0) {
            ultimaInteraccion = millis();
        }

        // Timeout o LCD apagado → salir
        if (millis() - ultimaInteraccion > 15000 || !oled.estaEncendido()) {
            ultimaInteraccion = millis();
            oled.limpiar();
            return;
        }

        if (evento == 1) {
            opcionActual = (opcionActual + 1) % numOpciones;

            oled.limpiar();
            oled.mostrar("Menu Principal", 0, 0);
            oled.mostrar(opciones[opcionActual], 0, 1);
        } else if (evento == 2) {  
            if (opcionActual == 2) {   // "Salir"
                oled.limpiar();
                estado = NO_MENU;      // <- salir al estado reposo
                salir = true;
            } else {
                estado = (opcionActual == 0) ? MENU_CONFIG_BOMBA : MENU_CONFIG_RELOJ;
                procesarEventoBoton();
                salir = true;
            }
        }
        delay(50); // pequeño respiro, no debounce fuerte
    }
}
