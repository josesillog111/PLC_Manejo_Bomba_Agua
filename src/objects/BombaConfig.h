#pragma once
#include <Arduino.h>

// Modo de operación de la bomba
enum ModoBomba : uint8_t { // Forzamos que el enum pese 1 byte, no 4
    POR_DIAS,
    POR_INTERVALO,
    POR_FECHA,
    APAGADO
};

// Empaquetado estricto para evitar problemas en EEPROM
#pragma pack(push, 1)

// Representación de una fecha simple
struct Fecha {
    uint8_t dia;   // 1-31
    uint8_t mes;   // 1-12
    uint16_t anio; // año completo (ej: 2025)
};

class BombaConfig {

    public:
        // --- CONFIGURACIÓN GENERAL ---
        bool habilitada;
        bool desactivarHoy;
        ModoBomba modo;

        // --- POR DÍAS ---
        uint8_t diasSemana;

        // --- POR INTERVALO ---
        uint8_t intervaloDias;
        Fecha fechaInicio;

        // --- HORARIOS ---
        uint8_t horaInicio;
        uint8_t minutoInicio;
        uint8_t horaFin;
        uint8_t minutoFin;

        // --- POR FECHA ---
        Fecha proximaFecha;

        // Constructor (Sin cambios, está perfecto)
        BombaConfig(bool habilitada = false,
            bool desactivarHoy = false,
            ModoBomba modo = APAGADO,
            uint8_t diasSemana = 0b0111110, 
            uint8_t intervaloDias = 1,
            Fecha fechaInicio = {1, 1, 2024},
            uint8_t horaInicio = 8,
            uint8_t minutoInicio = 0,
            uint8_t horaFin = 20,
            uint8_t minutoFin = 0,
            Fecha proximaFecha = {1, 1, 2024})
            : habilitada(habilitada),
            desactivarHoy(desactivarHoy),
            modo(modo),
            diasSemana(diasSemana),
            intervaloDias(intervaloDias),
            fechaInicio(fechaInicio),
            horaInicio(horaInicio),
            minutoInicio(minutoInicio),
            horaFin(horaFin),
            minutoFin(minutoFin),
            proximaFecha(proximaFecha) {}

        String toString() const {
            String resultado = "Modo: ";
            switch (modo) {
                case POR_DIAS: resultado += "Por Dias"; break;
                case POR_INTERVALO: resultado += "Por Intervalo"; break;
                case POR_FECHA: resultado += "Por Fecha"; break;
                case APAGADO: resultado += "Apagado"; break;
            }

            resultado += "\nDias Semana: " + String(diasSemana, BIN);
            resultado += "\nIntervalo Dias: " + String(intervaloDias);
            resultado += "\nFecha Inicio: " + String(fechaInicio.dia) + "/" + 
                            String(fechaInicio.mes) + "/" + String(fechaInicio.anio);
            resultado += "\nProxima Fecha: " + String(proximaFecha.dia) + "/" + 
                            String(proximaFecha.mes) + "/" + String(proximaFecha.anio);
            resultado += "\nHora Inicio: " + String(horaInicio) + ":" + 
                            (minutoInicio < 10 ? "0" : "") + String(minutoInicio);
            resultado += "\nHora Fin: " + String(horaFin) + ":" + 
                            (minutoFin < 10 ? "0" : "") + String(minutoFin);
            resultado += "\nDesactivar Hoy: " + String(desactivarHoy ? "Si" : "No");
            resultado += "\nHabilitada: " + String(habilitada ? "Si" : "No");

            return resultado;
        }
};

#pragma pack(pop) // Volvemos a la configuración normal de memoria