#pragma once
#include "../objects/BombaConfig.h"

class ConfigManager {
    private:
        BombaConfig& bombaConfig;   // referencia al objeto activo
        
        static const int EEPROM_ADDR = 0;

        BombaConfig cargarConfig();
        // Métodos privados de persistencia
        void aplicarCambios();
        void guardarConfig(const BombaConfig& config);
        

    public:
        // Constructor: recibe la config activa
        ConfigManager(BombaConfig& config);

        // Inicializa cargando desde EEPROM
        void iniciar();

        // === Métodos de configuración ===
        void configurarPorDias(uint8_t diasSemana,uint8_t horaInicio, uint8_t minutoInicio, uint8_t horaFin, uint8_t minutoFin);
        void configurarPorIntervalo(uint8_t intervalo, const Fecha& inicio, uint8_t horaInicio, uint8_t minutoInicio, uint8_t horaFin, uint8_t minutoFin);
        void configurarPorFecha(Fecha fecha, uint8_t horaInicio, uint8_t minutoInicio, uint8_t horaFin, uint8_t minutoFin);
        void apagarBomba();
        void encenderBomba();
        String infoBomba();
        bool estadoBomba();
};