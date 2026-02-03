#include "Reloj.h"
#include <Arduino.h>

// Constructor
Reloj::Reloj(RtcDS3231<TwoWire>& rtc, OLED& oledRef) : oled(oledRef), Rtc(rtc){
}

void Reloj::mostrarHora() {
    RtcDateTime now = Rtc.GetDateTime();
    if (!now.IsValid()) {
        oled.mostrar("RTC no valido", 0, 0);
        oled.mostrar(" ", 0, 1); // limpiar segunda línea
        return;
    }

    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate < 1000) return; // refrescar cada 1s
    lastUpdate = millis();

    char buffer[21];

    // --- Fecha en la primera línea ---
    snprintf(buffer, sizeof(buffer), "%02d/%02d/%04d", now.Day(), now.Month(), now.Year());
    oled.mostrar(buffer, 0, 0);

    // --- Hora en la segunda línea ---
    snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", now.Hour(), now.Minute(), now.Second());
    oled.mostrar(buffer, 0, 1);
}


void Reloj::setHora(int h, int m, int s) {
    if (h < 0 || h > 23 || m < 0 || m > 59) {
        return; // Hora inválida
    }
    RtcDateTime now = Rtc.GetDateTime();
    RtcDateTime newTime(now.Year(), now.Month(), now.Day(), h, m, s);
    Rtc.SetDateTime(newTime);
}

void Reloj::setFecha(int d, int m, int a) {
    if (a < 2000 || m < 1 || m > 12 || d < 1 || d > 31) {
        return; // Fecha inválida
    }
    RtcDateTime now = Rtc.GetDateTime();
    RtcDateTime newDate(a, m, d, now.Hour(), now.Minute(), now.Second());
    Rtc.SetDateTime(newDate);
}
