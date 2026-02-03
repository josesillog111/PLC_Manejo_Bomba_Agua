// Potenciometro.h
#pragma once

class Potenciometro {
private:
    int pin;        // Pin analógico conectado al potenciómetro
    int muestras;   // Número de muestras para promediar

public:
    Potenciometro(int pinEntrada, int numMuestras = 10);
    void iniciar();
    // Lectura promedio cruda (0-1023)
    int leer();

    // Lectura escalada a un rango [minVal, maxVal]
    int leerEscalado(int minVal, int maxVal);
};