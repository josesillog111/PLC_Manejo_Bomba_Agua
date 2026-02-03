#pragma once
#include <LiquidCrystal_I2C.h>

class Lcd {
    private:
        LiquidCrystal_I2C& lcd;
        unsigned long ultimaActividad; // último momento de uso
        unsigned long tiempoApagado;   // timeout ms
        bool encendido;                // Estado actual del LCD

    public:
        // Constructor con timeout por defecto (10s)
        Lcd(LiquidCrystal_I2C& lcdRef, unsigned long timeout = 10000);

        void iniciar();
        void mostrar(const char* msg, int col = 0, int row = 0);
        void limpiar();

        // Métodos para forzar encendido/apagado
        void encender();
        void apagar();
        bool estaEncendido();
};
