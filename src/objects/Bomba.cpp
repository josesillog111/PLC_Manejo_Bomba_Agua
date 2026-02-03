#include "Bomba.h"
#include <Arduino.h>

// Constructor
Bomba::Bomba(int pinControl) : estado(false), pinControl(pinControl) {
}   

void Bomba::iniciar() {
    pinMode(pinControl, OUTPUT);
    ApagarBomba(); // Asegurarse de que la bomba esté apagada al iniciar
    
}

// Encender bomba
void Bomba::ActivarBomba() {
    digitalWrite(pinControl, HIGH);
    estado = true;
}

// Apagar bomba
void Bomba::ApagarBomba() {
    digitalWrite(pinControl, LOW);
    estado = false;
}

bool Bomba::estaEncendida() {
    return estado;
}

