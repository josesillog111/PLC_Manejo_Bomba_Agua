#include "OLED.h"
#include <Arduino.h>

// Constructor
OLED::OLED(Adafruit_SSD1306& displayRef, unsigned long timeout)
    : display(displayRef), tiempoApagado(timeout), encendido(true) {
    ultimaActividad = millis();
}

// Inicialización del OLED
void OLED::iniciar() {
    display.clearDisplay();
    display.setTextSize(1);      // Tamaño normal
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    
    display.display();
    encendido = true;
    ultimaActividad = millis();
}

// Mostrar un mensaje en el OLED
void OLED::mostrar(const char* msg, int col, int row) {
    if (!encendido) {
        encender(); 
    }
    
    display.setTextColor(SSD1306_WHITE); 
    display.setCursor(col, row * 10); 
    display.print(msg);
    display.display();
    ultimaActividad = millis(); 
}

// Limpiar la pantalla
void OLED::limpiar() {
    display.clearDisplay();   
    ultimaActividad = millis();
}

// Forzar encendido (Despertar hardware)
void OLED::encender() {
    display.ssd1306_command(SSD1306_DISPLAYON); 
    encendido = true;
    ultimaActividad = millis();
}

// Forzar apagado (Ahorro de energía real)
void OLED::apagar() {
    display.ssd1306_command(SSD1306_DISPLAYOFF); 
    encendido = false;
}

bool OLED::estaEncendido() {
    return encendido;
}