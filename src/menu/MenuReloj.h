#pragma once

#include "../objects/OLED.h"
#include "../objects/Boton.h"
#include "../objects/Potenciometro.h"
#include "../objects/Reloj.h"

class MenuReloj{
    private:
        OLED& oled;
        Boton& boton;
        Potenciometro& pot;
        Reloj& reloj;

        int opcionActual = 0;
        unsigned long ultimaInteraccion;

        enum EstadoMenu {
            NO_MENU,
            MENU_AJUSTE_HORA,
            MENU_AJUSTE_FECHA,
        } estado;

        const char* opciones[3] = {
            "1) Ajuste Hora",
            "2) Ajuste Fecha",
            "3) Salir"
        };
        const int numOpciones = sizeof(opciones) / sizeof(opciones[0]);
        void procesarEventoBoton();
        int selectorGenerico(const char* titulo, int minVal, int maxVal, const char* sufijo);
        
    public:
        MenuReloj(OLED& oled, Boton& boton, Potenciometro& pot, Reloj& reloj);
        void mostrarMenu();
};