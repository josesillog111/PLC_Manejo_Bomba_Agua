#include "ConfigManager.h"
#include <EEPROM.h>

ConfigManager::ConfigManager(BombaConfig& config)
    : bombaConfig(config) {}

void ConfigManager::iniciar() {
    bombaConfig = cargarConfig();

    if (!bombaConfig.habilitada) {
        bombaConfig = BombaConfig();
        guardarConfig(bombaConfig);
    }
}

// =================== PRIVADOS ===================
BombaConfig ConfigManager::cargarConfig() {
    BombaConfig config;
    EEPROM.get(EEPROM_ADDR, config);

    bool invalida = false;

    // Validar fecha próxima
    if (config.proximaFecha.anio < 2024 || config.proximaFecha.anio > 2100 ||
        config.proximaFecha.mes < 1 || config.proximaFecha.mes > 12 ||
        config.proximaFecha.dia < 1 || config.proximaFecha.dia > 31) {
        invalida = true;
    }

    // Validar fecha inicio
    if (config.fechaInicio.anio < 2024 || config.fechaInicio.anio > 2100 ||
        config.fechaInicio.mes < 1 || config.fechaInicio.mes > 12 ||
        config.fechaInicio.dia < 1 || config.fechaInicio.dia > 31) {
        invalida = true;
    }

    // Validar horas y minutos
    if (config.horaInicio > 23 || config.horaFin > 23 ||
        config.minutoInicio > 59 || config.minutoFin > 59) {
        invalida = true;
    }

    // Validar intervalo
    if (config.intervaloDias == 0 || config.intervaloDias > 30) {
        invalida = true;
    }

    if (invalida) {
        return BombaConfig(); // <- devuelve un struct limpio
    }

    return config;
}


void ConfigManager::guardarConfig(const BombaConfig& config) {
    BombaConfig actual;
    EEPROM.get(EEPROM_ADDR, actual);

    if (memcmp(&actual, &config, sizeof(BombaConfig)) != 0) {
        EEPROM.put(EEPROM_ADDR, config);
    }
}


void ConfigManager::aplicarCambios() {
    guardarConfig(bombaConfig);
}


// =================== CONFIG ===================

void ConfigManager::configurarPorDias(uint8_t diasSemana, uint8_t horaInicio, uint8_t minutoInicio, uint8_t horaFin, uint8_t minutoFin) {
    bombaConfig.habilitada = true;
    bombaConfig.desactivarHoy = false; // resetear
    bombaConfig.modo = POR_DIAS;
    bombaConfig.diasSemana = diasSemana;
    bombaConfig.horaInicio = horaInicio;
    bombaConfig.minutoInicio = minutoInicio;
    bombaConfig.horaFin = horaFin;
    bombaConfig.minutoFin = minutoFin;
    aplicarCambios();

}

void ConfigManager::configurarPorIntervalo(uint8_t intervalo, const Fecha& inicio, uint8_t horaInicio, uint8_t minutoInicio, uint8_t horaFin, uint8_t minutoFin) {
    bombaConfig.habilitada = true;
    bombaConfig.desactivarHoy = false; // resetear
    bombaConfig.modo = POR_INTERVALO;
    bombaConfig.intervaloDias = intervalo;
    bombaConfig.fechaInicio = inicio;   // ancla fija
    bombaConfig.horaInicio = horaInicio;
    bombaConfig.minutoInicio = minutoInicio;
    bombaConfig.horaFin = horaFin;
    bombaConfig.minutoFin = minutoFin;
    aplicarCambios();
}

void ConfigManager::configurarPorFecha(Fecha fecha, uint8_t horaInicio, uint8_t minutoInicio, uint8_t horaFin, uint8_t minutoFin) {
    bombaConfig.habilitada = true;
    bombaConfig.desactivarHoy = false; // resetear
    bombaConfig.modo = POR_FECHA;
    bombaConfig.proximaFecha = fecha;
    bombaConfig.horaInicio = horaInicio;
    bombaConfig.minutoInicio = minutoInicio;
    bombaConfig.horaFin = horaFin;
    bombaConfig.minutoFin = minutoFin;
    aplicarCambios();
}


void ConfigManager::apagarBomba() {
    bombaConfig.habilitada = false;
    aplicarCambios();
}

void ConfigManager::encenderBomba() {
    bombaConfig.habilitada = true;
    aplicarCambios();
}

bool ConfigManager::estadoBomba() {
    return bombaConfig.habilitada;
}

String ConfigManager::infoBomba() {
    return bombaConfig.toString();
}