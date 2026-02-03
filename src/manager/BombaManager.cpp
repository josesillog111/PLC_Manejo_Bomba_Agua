#include "BombaManager.h"

// Constructor
BombaManager::BombaManager(Bomba& bomba, BombaConfig& configBomba, RtcDS3231<TwoWire>& rtc, Boton& btnManual)
    : bomba(bomba), configBomba(configBomba), Rtc(rtc), btnManual(btnManual) {}

// ======================================================
// EVALUAR (CEREBRO CENTRAL)
// ======================================================
void BombaManager::Evaluar(const RtcDateTime& now) {

    // 1. ¿QUÉ DICE EL HORARIO? (Cálculo puro)
    bool deberiaEstarEncendido = calcularSiDebeEstarEncendido(now);

    // 2. ¿QUÉ DICE EL BOTÓN FÍSICO?
    int click = btnManual.leerEvento(); 

    if (click == 1) { // CLICK CORTO -> TOGGLE (Natural)
        Serial.println("Boton Manual: Click detectado");
        
        if (bomba.estaEncendida()) {
            // Si está encendida (por horario o manual) -> APAGAR
            estadoOverride = MANUAL_OFF;
            Serial.println("-> Accion: Forzar APAGADO");
        } else {
            // Si está apagada -> ENCENDER
            estadoOverride = MANUAL_ON;
            inicioManual = millis();
            Serial.println("-> Accion: Forzar ENCENDIDO");
        }
    } 
    else if (click == 2) { // CLICK LARGO -> RESET TOTAL
        Serial.println("Boton Manual: Click Largo -> RESET A AUTO");
        estadoOverride = AUTO;
        configBomba.desactivarHoy = false; // Reactivamos si estaba bloqueado
    }

    // 3. SEGURIDAD (Timeout)
    if (estadoOverride == MANUAL_ON) {
        if (millis() - inicioManual > TIEMPO_MAXIMO_MANUAL) {
            Serial.println("Tiempo manual excedido -> Vuelta a Auto");
            estadoOverride = AUTO; 
        }
    }
    
    // 4. RESET INTELIGENTE DE 'MANUAL OFF'
    // Si forzaste apagado, pero el horario ya terminó, volvemos a AUTO
    // para que mañana funcione normal sin que tengas que tocar nada.
    if (estadoOverride == MANUAL_OFF && !deberiaEstarEncendido) {
        estadoOverride = AUTO;
    }

    // 5. EJECUCIÓN FINAL (Solo aquí tocamos el hardware)
    switch (estadoOverride) {
        case MANUAL_ON:
            bomba.ActivarBomba();
            break;
            
        case MANUAL_OFF:
            bomba.ApagarBomba();
            break;
            
        case AUTO:
            // En auto, respetamos el "desactivarHoy" (emergencia)
            if (configBomba.desactivarHoy) {
                bomba.ApagarBomba();
            } else if (deberiaEstarEncendido) {
                bomba.ActivarBomba();
            } else {
                bomba.ApagarBomba();
            }
            break;
    }
}

// ======================================================
// CONTROLES EXTERNOS (Para MQTT)
// ======================================================
void BombaManager::forzarManual(bool encender) {
    if (encender) {
        estadoOverride = MANUAL_ON;
        inicioManual = millis();
    } else {
        estadoOverride = MANUAL_OFF;
    }
}

void BombaManager::resetAutomator() {
    estadoOverride = AUTO;
}

void BombaManager::ActualizarConfigBomba(const BombaConfig& nuevaConfig) {
    configBomba = nuevaConfig;
}

// ======================================================
// LÓGICA DE CÁLCULO DE HORARIOS (Devuelven bool)
// ======================================================
bool BombaManager::calcularSiDebeEstarEncendido(const RtcDateTime& now) {
    if (!configBomba.habilitada) return false;

    switch (configBomba.modo) {
        case POR_DIAS:      return checkPorDias(now);
        case POR_INTERVALO: return checkPorIntervalo(now);
        case POR_FECHA:     return checkPorFecha(now);
        default:            return false;
    }
}

bool BombaManager::checkPorDias(const RtcDateTime& now) {
    bool activoHoy = configBomba.diasSemana & (1 << now.DayOfWeek());
    if (!activoHoy) return false;
    return estaEnHorario(now);
}

bool BombaManager::checkPorIntervalo(const RtcDateTime& now) {
    RtcDateTime inicio(configBomba.fechaInicio.anio, configBomba.fechaInicio.mes, configBomba.fechaInicio.dia, configBomba.horaInicio, configBomba.minutoInicio, 0);
    int64_t inicioSecs = (int64_t)inicio.TotalSeconds();
    int64_t nowSecs = (int64_t)now.TotalSeconds();
    int64_t diasPasados = (nowSecs - inicioSecs) / 86400LL;

    if (diasPasados < 0) return false;
    
    if ((diasPasados % configBomba.intervaloDias) == 0) {
        return estaEnHorario(now);
    }
    return false;
}

bool BombaManager::checkPorFecha(const RtcDateTime& now) {
    if (now.Year() == configBomba.proximaFecha.anio &&
        now.Month() == configBomba.proximaFecha.mes &&
        now.Day() == configBomba.proximaFecha.dia) {
        return estaEnHorario(now);
    }
    return false;
}

bool BombaManager::estaEnHorario(const RtcDateTime& now) {
    int actualMin = now.Hour() * 60 + now.Minute();
    int inicioMin = configBomba.horaInicio * 60 + configBomba.minutoInicio;
    int finMin    = configBomba.horaFin * 60 + configBomba.minutoFin;
    return (actualMin >= inicioMin && actualMin < finMin);
}