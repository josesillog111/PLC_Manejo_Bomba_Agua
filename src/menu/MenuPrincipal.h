#pragma once

#include "../objects/OLED.h"
#include "../objects/Boton.h"
#include "../objects/Potenciometro.h"
#include "../objects/Reloj.h"
#include "../manager/ConfigManager.h"
#include "../manager/BombaManager.h"
#include "MenuBomba.h"
#include "MenuReloj.h"

class MenuPrincipal {
private:
    OLED& oled;
    Boton& boton;

    // Referencias a menús secundarios
    MenuBomba& menuBomba;
    MenuReloj& menuReloj;

    int opcionActual = 0;
    unsigned long ultimaInteraccion;

    enum EstadoMenu {
        NO_MENU,
        MENU_CONFIG_BOMBA,
        MENU_CONFIG_RELOJ,
    } estado;

    const char* opciones[3] = {
        "1) Config Bomba",
        "2) Config Reloj",
        "3) Salir"
    };
    const int numOpciones = sizeof(opciones) / sizeof(opciones[0]);
    void procesarEventoBoton();

public:
    MenuPrincipal(OLED& oled, Boton& boton, MenuBomba& menuBomba, MenuReloj& menuReloj);
    void mostrarMenu();
};
