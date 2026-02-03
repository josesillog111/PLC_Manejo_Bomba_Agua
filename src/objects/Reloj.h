#pragma once
#include "../objects/OLED.h"
#include <RtcDS3231.h>

class Reloj {
    private:
        OLED& oled;
        RtcDS3231<TwoWire>& Rtc;
        
    public:
        Reloj(RtcDS3231<TwoWire>& rtc, OLED& oled);
        void mostrarHora();
        void setHora(int h, int m, int s = 0);
        void setFecha(int d, int m, int a);
};