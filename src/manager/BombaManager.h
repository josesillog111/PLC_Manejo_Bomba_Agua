#pragma once
#include "../objects/Bomba.h"
#include "../objects/BombaConfig.h"
#include "../objects/Boton.h" // Usamos Botón, no Switch
#include <Wire.h>
#include <RtcDS3231.h>

// Estados de prioridad
enum EstadoOverride {
    AUTO,       // Manda el horario
    MANUAL_ON,  // Forzado ON (Ignora horario)
    MANUAL_OFF  // Forzado OFF (Ignora horario)
};

class BombaManager {
private:
    Bomba& bomba;
    BombaConfig& configBomba;
    RtcDS3231<TwoWire>& Rtc;
    Boton& btnManual; // Referencia al botón físico (Pin 17)

    // Variables de Control Manual
    EstadoOverride estadoOverride = AUTO;
    unsigned long inicioManual = 0;
    const unsigned long TIEMPO_MAXIMO_MANUAL = 3600000; // 1 Hora seguridad

    // Métodos auxiliares que solo CALCULAN (retornan bool), no actúan
    bool calcularSiDebeEstarEncendido(const RtcDateTime& now);
    bool checkPorDias(const RtcDateTime& now);
    bool checkPorIntervalo(const RtcDateTime& now);
    bool checkPorFecha(const RtcDateTime& now);
    bool estaEnHorario(const RtcDateTime& now);

public:
    // Constructor recibe Boton
    BombaManager(Bomba& bomba, BombaConfig& configBomba, RtcDS3231<TwoWire>& rtc, Boton& btnManual);
    
    void Evaluar(const RtcDateTime& now);

    // Métodos para MQTT
    void forzarManual(bool encender);
    void resetAutomator(); 

    void ActualizarConfigBomba(const BombaConfig& nuevaConfig);
};