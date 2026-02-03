#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include "../manager/ConfigManager.h"
#include "../include/Config.h"
#include "../objects/OLED.h" // Necesitamos acceso a la pantalla para mostrar mensajes

class NetworkManager {
private:
    WiFiClientSecure espClient;
    PubSubClient client;
    ConfigManager& configManager;
    OLED& oled; // Referencia a la pantalla principal


    // Variables para guardar credenciales en RAM
    char mqtt_server[80];
    char mqtt_port[6];
    char mqtt_user[32];
    char mqtt_pass[32];

    void loadCredentials();
    void saveCredentials();

public:
    NetworkManager(OLED& display, ConfigManager& configManager);
    void iniciar();
    void update();
    bool isConnected();
    void reconnect();
    void publishStatus(bool estadoBomba);
    void publishInfo();
    void configurarPorDias(uint8_t diasSemana,uint8_t horaInicio, uint8_t minutoInicio, uint8_t horaFin, uint8_t minutoFin);
    void configurarPorIntervalo(uint8_t intervalo, const Fecha& inicio, uint8_t horaInicio, uint8_t minutoInicio, uint8_t horaFin, uint8_t minutoFin);
    void configurarPorFecha(Fecha fecha, uint8_t horaInicio, uint8_t minutoInicio, uint8_t horaFin, uint8_t minutoFin);
};

#endif