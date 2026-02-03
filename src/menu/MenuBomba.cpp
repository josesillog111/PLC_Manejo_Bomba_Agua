#include "MenuBomba.h"

MenuBomba::MenuBomba(OLED& oled, Boton& boton, Potenciometro& pot, ConfigManager& cfg)
    : oled(oled), boton(boton), pot(pot), configManager(cfg), estado(NO_MENU) {}


void MenuBomba::procesarEventoBoton() {
    switch (estado) {
        case POR_DIAS: {
            oled.limpiar();
            uint8_t dias = selectorDias();
            int hInicio = selectorGenerico("Hora Inicio", 0, 23, "h");
            int mInicio = selectorGenerico("Min Inicio", 0, 59, "m");
            int hFin = selectorGenerico("Hora Fin", 0, 23, "h");
            int mFin = selectorGenerico("Min Fin", 0, 59, "m");

            configManager.configurarPorDias(dias, hInicio, mInicio, hFin, mFin);

            oled.limpiar();
            oled.mostrar("Config Guardada", 0, 0);
            delay(1500);
            break;
        }
        case POR_INTERVALO: {
            oled.limpiar();
            int intervalo = selectorIntervalo();
            Fecha inicio;
            inicio.dia  = selectorGenerico("Dia Inicio", 1, 31, "d");
            inicio.mes  = selectorGenerico("Mes Inicio", 1, 12, "m");
            inicio.anio = selectorGenerico("Anio Inicio", 2025, 2035, "a");

            int hInicio = selectorGenerico("Hora Inicio", 0, 23, "h");
            int mInicio = selectorGenerico("Min Inicio", 0, 59, "m");
            int hFin    = selectorGenerico("Hora Fin", 0, 23, "h");
            int mFin    = selectorGenerico("Min Fin", 0, 59, "m");

            configManager.configurarPorIntervalo(intervalo, inicio, hInicio, mInicio, hFin, mFin);

            oled.limpiar();
            oled.mostrar("Config Guardada", 0, 0);
            delay(1500);
            break;
        }
        case POR_FECHA: {
            oled.limpiar();

            int dia = selectorGenerico("Dia", 1, 31, "d");
            int mes = selectorGenerico("Mes", 1, 12, "m");
            int anio = selectorGenerico("Anio", 2024, 2035, "a");

            int hInicio = selectorGenerico("Hora Inicio", 0, 23, "h");
            int mInicio = selectorGenerico("Min Inicio", 0, 59, "m");
            int hFin = selectorGenerico("Hora Fin", 0, 23, "h");
            int mFin = selectorGenerico("Min Fin", 0, 59, "m");

            configManager.configurarPorFecha({(uint8_t)dia, (uint8_t)mes, (uint16_t)anio}, hInicio, mInicio, hFin, mFin);

            oled.limpiar();
            oled.mostrar("Config Guardada", 0, 0);
            delay(1500);
            oled.limpiar();
            break;
        }
        case APAGADO: {
            if (configManager.estadoBomba()) {
                configManager.apagarBomba();
                oled.limpiar();
                oled.mostrar("Bomba Apagada", 0, 0);
            } else {
                configManager.encenderBomba();
                oled.limpiar();
                oled.mostrar("Bomba Encendida", 0, 0);
            }
            break;
        }
        case NO_MENU:
        default:
            delay(50);
            oled.limpiar();
            break;
    }
}

void MenuBomba::mostrarMenu() {
    bool salir = false;

    ultimaInteraccion = millis();
    oled.limpiar();
    oled.mostrar("Config Bomba", 0, 0);
    oled.mostrar(opciones[opcionActual], 0, 1);

    while (!salir) {
        int evento = boton.leerEvento();

        if (evento != 0) {
            ultimaInteraccion = millis();
        }

        if (millis() - ultimaInteraccion > 15000 || !oled.estaEncendido()) {
            ultimaInteraccion = millis();
            oled.limpiar();
            return;
        }

        if (evento == 1) {
            opcionActual = (opcionActual + 1) % numOpciones;

            oled.limpiar();
            oled.mostrar("Config Bomba", 0, 0);
            oled.mostrar(opciones[opcionActual], 0, 1);
        }
        else if (evento == 2) {  
            if (opcionActual == 4) {   // "Salir"
                oled.limpiar();
                estado = NO_MENU;
                salir = true;
            } else {
                estado =    (opcionActual == 0) ? POR_INTERVALO :
                            (opcionActual == 1) ? POR_DIAS :
                            (opcionActual == 2) ? POR_FECHA :
                            (opcionActual == 3) ? APAGADO :
                            APAGADO;
                procesarEventoBoton();
                salir = true;
            }
        }
        delay(50);
    }
}

int MenuBomba::selectorGenerico(const char* titulo, int minVal, int maxVal, const char* sufijo) {
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

uint8_t MenuBomba::selectorDias() {
    bool salir = false;
    ultimaInteraccion = millis();
    uint8_t mask = 0;
    int diaActual = -1;

    static const char* nombres[] = {
        "Dom", "Lun", "Mar", "Mie", "Jue", "Vie", "Sab"
    };

    while (!salir) {
        int evento = boton.leerEvento();
        int nuevoDia = pot.leerEscalado(0, 6);

        if (evento != 0 || nuevoDia != diaActual) {
            ultimaInteraccion = millis();
        }

        if (millis() - ultimaInteraccion > 15000 || !oled.estaEncendido()) {
            oled.limpiar();
            return mask;
        }

        
        if (nuevoDia != diaActual) {
            diaActual = nuevoDia;

            oled.limpiar();
            oled.mostrar("Dias Semana", 0, 0);

            char buffer[17];
            snprintf(buffer, sizeof(buffer), "%s %s", nombres[diaActual],
                        (mask & (1 << diaActual)) ? "[X]" : "[ ]");
            oled.mostrar(buffer, 0, 1);
        }

        if (evento == 1) {
            mask ^= (1 << diaActual); // toggle
        }
        else if (evento == 2) {
            salir = true;
        }

        delay(50);
    }

    return mask;
}

int MenuBomba::selectorIntervalo() {
    return selectorGenerico("Intervalo", 1, 30, "dias");
}
