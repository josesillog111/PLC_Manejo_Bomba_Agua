
#include "NetworkManager.h"
#include "../include/Context.h"

// Variable auxiliar para el callback de WiFiManager
bool shouldSaveConfig = false;
void saveConfigCallback() {
    Serial.println("GUARDAR CONFIGURACION DETECTADO");
    shouldSaveConfig = true;
}

NetworkManager::NetworkManager(OLED& display, ConfigManager& configManager) : oled(display), configManager(configManager), client(espClient) {
    // Constructor: Copiamos valores por defecto a las variables
    strcpy(mqtt_server, DEFAULT_MQTT_SERVER);
    strcpy(mqtt_port, DEFAULT_MQTT_PORT);
    strcpy(mqtt_user, DEFAULT_MQTT_USER);
    strcpy(mqtt_pass, DEFAULT_MQTT_PASS);
}

void NetworkManager::loadCredentials() {
    if (EEPROM.read(EEPROM_ADDR_MQTT) != 0xFF) {
        EEPROM.get(EEPROM_ADDR_MQTT, mqtt_server);
        Serial.println("Credenciales cargadas de EEPROM");
    }
}

void NetworkManager::saveCredentials() {
    Serial.println("Guardando credenciales en EEPROM...");
    EEPROM.put(EEPROM_ADDR_MQTT, mqtt_server);
    EEPROM.put(EEPROM_ADDR_USER, mqtt_user);
    EEPROM.commit();
}

void NetworkManager::iniciar() {
    // ... (todo tu código de EEPROM y WiFiManager igual) ...

    // Configuración MQTT
    espClient.setInsecure();
    int port = atoi(mqtt_port);
    client.setServer(mqtt_server, port);
    
    // ==========================================
    // CALLBACK INTELIGENTE (JSON + COMANDOS)
    // ==========================================
    client.setCallback([this](char* topic, byte* payload, unsigned int length) {
        String mensaje = "";
        for (int i = 0; i < length; i++) mensaje += (char)payload[i];
        
        Serial.print("MQTT Recibido: ");
        Serial.println(mensaje);

        // CASO 1: COMANDOS SIMPLES (Manual)
        if (mensaje == "ON") {
            bombaManager.forzarManual(true);
        } 
        else if (mensaje == "OFF") {
            bombaManager.forzarManual(false);
        }
        
        // CASO 2: CONFIGURACIÓN AVANZADA (JSON)
        // Si el mensaje empieza con '{', asumimos que es un JSON de configuración
        else if (mensaje.startsWith("{")) {
            Serial.println("-> Procesando Configuración JSON...");
            
            // Buffer para el JSON (512 bytes suele ser suficiente)
            JsonDocument doc; 
            DeserializationError error = deserializeJson(doc, mensaje);

            if (error) {
                Serial.print("Error JSON: "); Serial.println(error.c_str());
                return;
            }

            // Leemos el "modo" para saber qué configurar
            const char* modo = doc["modo"]; // "dias", "intervalo", "fecha"

            if (strcmp(modo, "dias") == 0) {
                uint8_t dias = doc["diasSemana"];
                uint8_t hI = doc["horaInicio"];
                uint8_t mI = doc["minutoInicio"];
                uint8_t hF = doc["horaFin"];
                uint8_t mF = doc["minutoFin"];
                
                // Aplicamos la configuración y confirmamos a la nube
                this->configurarPorDias(dias, hI, mI, hF, mF);
                Serial.println("Configuración POR DIAS actualizada desde Nube");
            }
            else if (strcmp(modo, "intervalo") == 0) {
                uint8_t interv = doc["intervaloDias"];
                
                // Parseamos la fecha (asumiendo formato "2024-01-20" o campos separados)
                // Para facilitar, sugiero que envíes campos separados desde HiveMQ:
                Fecha inicio;
                inicio.anio = doc["anioInicio"];
                inicio.mes  = doc["mesInicio"];
                inicio.dia  = doc["diaInicio"];
                
                uint8_t hI = doc["horaInicio"];
                uint8_t mI = doc["minutoInicio"];
                uint8_t hF = doc["horaFin"];
                uint8_t mF = doc["minutoFin"];

                this->configurarPorIntervalo(interv, inicio, hI, mI, hF, mF);
                Serial.println("Configuración POR INTERVALO actualizada desde Nube");
            }
            else if (strcmp(modo, "fecha") == 0) {
                Fecha prox;
                prox.anio = doc["anio"];
                prox.mes  = doc["mes"];
                prox.dia  = doc["dia"];

                uint8_t hI = doc["horaInicio"];
                uint8_t mI = doc["minutoInicio"];
                uint8_t hF = doc["horaFin"];
                uint8_t mF = doc["minutoFin"];

                this->configurarPorFecha(prox, hI, mI, hF, mF);
                Serial.println("Configuración POR FECHA actualizada desde Nube");
            }
        }
    });
}

void NetworkManager::reconnect() {
    if (!client.connected()) {
        Serial.print("Reconectando MQTT...");
        String clientId = "ESP32Riego-" + String(random(0xffff), HEX);

        if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
            Serial.println("Conectado!");
            
            client.subscribe("casa/jardin/bomba/comando");
            Serial.println("Suscrito a .../comando");

            publishInfo();

        } else {
            Serial.print("Fallo, rc=");
            Serial.print(client.state());
        }
    }
}

void NetworkManager::update() {
    if (WiFi.status() == WL_CONNECTED) {
        if (!client.connected()) {
            static unsigned long lastReconnect = 0;
            if (millis() - lastReconnect > 5000) {
                lastReconnect = millis();
                reconnect();
            }
        }
        client.loop();
    }
}

bool NetworkManager::isConnected() { return client.connected(); }

void NetworkManager::publishStatus(bool estadoBomba) {
    if (client.connected()) {
        String payload = "{\"bomba\": " + String(estadoBomba) + "}";
        client.publish("casa/jardin/bomba/estado", payload.c_str());
    }
}

void NetworkManager::publishInfo() {
    if (client.connected()) {
        // Esta variable 'payload' la creas pero NO la usas abajo:
        String payload = "{\"info\": \"Sistema de riego activo\"}"; 
        
        // Aquí estás enviando directamente el String de configManager:
        client.publish("casa/jardin/bomba/info", configManager.infoBomba().c_str());
    }
}



/*  
    Por días 
    
    EJEMPLO JSON:
    {
        "modo": "dias",
        "diasSemana": 62,
        "horaInicio": 8,
        "minutoInicio": 0,
        "horaFin": 20,
        "minutoFin": 0
    }
*/
void NetworkManager::configurarPorDias(uint8_t diasSemana,uint8_t horaInicio, uint8_t minutoInicio, uint8_t horaFin, uint8_t minutoFin) {
    if (client.connected()) {
        configManager.configurarPorDias(diasSemana, horaInicio, minutoInicio, horaFin, minutoFin);
        String payload = "{\"modo\":\"dias\",\"diasSemana\":" + String(diasSemana) + ",\"horaInicio\":" + String(horaInicio) + ",\"minutoInicio\":" + String(minutoInicio) + ",\"horaFin\":" + String(horaFin) + ",\"minutoFin\":" + String(minutoFin) + "}";
        client.publish("casa/jardin/bomba/configuracion", payload.c_str());
        publishInfo();
    }

    
}

/* 
    Intervalo 
    EJEMPLO JSON:
    {
        "modo": "intervalo",
        "intervaloDias": 3,
        "anioInicio": 2024,
        "mesInicio": 1,
        "diaInicio": 20,
        "horaInicio": 8,
        "minutoInicio": 0,
        "horaFin": 20,
        "minutoFin": 0
    }
*/
void NetworkManager::configurarPorIntervalo(uint8_t intervalo, const Fecha& inicio, uint8_t horaInicio, uint8_t minutoInicio, uint8_t horaFin, uint8_t minutoFin) {
    if (client.connected()) {
        configManager.configurarPorIntervalo(intervalo, inicio, horaInicio, minutoInicio, horaFin, minutoFin);
        
        String fechaInicio = String(inicio.anio) + "-" + 
                            (inicio.mes < 10 ? "0" : "") + String(inicio.mes) + "-" + 
                            (inicio.dia < 10 ? "0" : "") + String(inicio.dia);
                                
        String payload = "{\"modo\":\"intervalo\",\"intervaloDias\":" + String(intervalo) + 
                        ",\"fechaInicio\":\"" + fechaInicio + "\",\"horaInicio\":" +
                        String(horaInicio) + ",\"minutoInicio\":" + String(minutoInicio) +
                        ",\"horaFin\":" + String(horaFin) + ",\"minutoFin\":" + String(minutoFin) + "}";

        client.publish("casa/jardin/bomba/configuracion", payload.c_str());
        publishInfo();
    }
}

/*  
    Por fecha 
    EJEMPLO JSON:
    {
        "modo": "fecha",
        "anio": 2024,
        "mes": 12,
        "dia": 25,
        "horaInicio": 9,
        "minutoInicio": 0,
        "horaFin": 18,
        "minutoFin": 0
    }
*/
void NetworkManager::configurarPorFecha(Fecha fecha, uint8_t horaInicio, uint8_t minutoInicio, uint8_t horaFin, uint8_t minutoFin) {
    if (client.connected()) {
        configManager.configurarPorFecha(fecha, horaInicio, minutoInicio, horaFin, minutoFin);
        
        String proximaFecha = String(fecha.anio) + "-" + 
                            (fecha.mes < 10 ? "0" : "") + String(fecha.mes) + "-" + 
                            (fecha.dia < 10 ? "0" : "") + String(fecha.dia);
                                
        String payload = "{\"modo\":\"fecha\",\"proximaFecha\":\"" + proximaFecha + 
                        "\",\"horaInicio\":" + String(horaInicio) + ",\"minutoInicio\":" +
                        String(minutoInicio) + ",\"horaFin\":" + String(horaFin) +
                        ",\"minutoFin\":" + String(minutoFin) + "}";

        client.publish("casa/jardin/bomba/configuracion", payload.c_str());
        publishInfo();
    }
}
