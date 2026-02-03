#include "Potenciometro.h"
#include <Arduino.h>

Potenciometro::Potenciometro(int pinEntrada, int numMuestras)
    : pin(pinEntrada), muestras(numMuestras) {
        pinMode(pin, INPUT);
    }

    void Potenciometro::iniciar() {
        pinMode(pin, INPUT);
    }

    int Potenciometro::leer() {
        long suma = 0;
        for (int i = 0; i < muestras; i++) {
            suma += analogRead(pin);
            delay(2); // pequeño delay para estabilidad
        }
        return suma / muestras; // valor promedio (0–1023)
    }

    int Potenciometro::leerEscalado(int minVal, int maxVal) {
        int valorCrudo = leer(); // promedio crudo
        return map(valorCrudo, 0, 1023, minVal, maxVal);
}