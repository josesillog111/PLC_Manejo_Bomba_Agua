#include "OLED.h"
#include "Lcd.h"
#include <Arduino.h>

// Constructor
Lcd::Lcd(LiquidCrystal_I2C& lcdRef, unsigned long timeout)
    : lcd(lcdRef), tiempoApagado(timeout), encendido(true) {
    ultimaActividad = millis();
}

// Inicialización del LCD
void Lcd::iniciar() {
    lcd.init();
    lcd.backlight();
    encendido = true;
    ultimaActividad = millis();
}

// Mostrar un mensaje en el LCD
void Lcd::mostrar(const char* msg, int col, int row) {
    if (!encendido) {
        encender(); // si estaba apagado, enciéndelo antes de mostrar
    }
    lcd.setCursor(col, row);
    lcd.print(msg);
    ultimaActividad = millis(); // reinicia temporizador de apagado
}

// Limpiar la pantalla
void Lcd::limpiar() {
    lcd.clear();
    ultimaActividad = millis();
}

// Forzar encendido
void Lcd::encender() {
    lcd.backlight();
    encendido = true;
    ultimaActividad = millis();
}

// Forzar apagado
void Lcd::apagar() {
    lcd.noBacklight();
    lcd.clear();
    encendido = false;
}

bool Lcd::estaEncendido() {
    return encendido;
}
