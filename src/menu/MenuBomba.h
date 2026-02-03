#pragma once
#include "../objects/Bomba.h"
#include "../objects/Boton.h"
#include "../objects/Potenciometro.h"
#include "../objects/OLED.h"
#include "../manager/ConfigManager.h"

class MenuBomba {
    private:
        OLED& oled;
        Boton& boton;
        Potenciometro& pot;
        ConfigManager& configManager;

        int opcionActual = 0;
        unsigned long ultimaInteraccion;

        enum EstadoMenu {
            POR_DIAS,
            POR_INTERVALO,
            POR_FECHA,
            APAGADO,
            NO_MENU
        } estado;

        const char* opciones[5] = {
            "1) Por Intervalo",
            "2) Por Dias",
            "3) Por Fecha",
            configManager.estadoBomba() ? "4) Apagar Bomba" : "4) Encender Bomba",
            "5) Salir"
        };
        const int numOpciones = sizeof(opciones) / sizeof(opciones[0]);


        void procesarEventoBoton();
        uint8_t selectorDias();
        void selectorFecha();
        int selectorIntervalo();
        int selectorGenerico(const char* titulo, int minVal, int maxVal, const char* sufijo);
    

        

    public:
        MenuBomba(OLED& oled, Boton& boton, Potenciometro& pot, ConfigManager& cfg);
        void mostrarMenu();
};