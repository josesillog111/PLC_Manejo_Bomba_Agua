#pragma once
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

class OLED {
    private:
        Adafruit_SSD1306& display;
        unsigned long ultimaActividad; // último momento de uso
        unsigned long tiempoApagado;   // timeout ms
        bool encendido;                // Estado actual del OLED

    public:
        // Constructor con timeout por defecto (10s)
        OLED(Adafruit_SSD1306& displayRef, unsigned long timeout = 10000);

        void iniciar();
        void mostrar(const char* msg, int col = 0, int row = 0);
        void limpiar();

        // Métodos para forzar encendido/apagado
        void encender();
        void apagar();
        bool estaEncendido();
};