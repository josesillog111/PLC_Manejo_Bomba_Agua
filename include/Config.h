#ifndef CONFIG_H
#define CONFIG_H

// ==========================================
// PINES DEL HARDWARE
// ==========================================
#define PIN_BOTON_BOMBA     16 
#define PIN_POT             34 
#define PIN_BOMBA           2
#define PIN_BOTON_MANUAL    17 
#define PIN_SDA             21
#define PIN_SCL             22

// ==========================================
// CONFIGURACIÓN DE PANTALLA
// ==========================================
#define OLED_ADDR     0x3C
#define OLED_WIDTH    128
#define OLED_HEIGHT   64

// ==========================================
// MAPA DE MEMORIA EEPROM
// ==========================================
// Dejamos los primeros 200 bytes libres para tu ConfigManager
#define EEPROM_SIZE         512
#define EEPROM_ADDR_MQTT    200  // Inicio bloque MQTT
#define EEPROM_ADDR_PORT    280  
#define EEPROM_ADDR_USER    290  
#define EEPROM_ADDR_PASS    330  

// ==========================================
// VALORES POR DEFECTO (HiveMQ)
// ==========================================
#define DEFAULT_MQTT_SERVER "b507c8ea9f9d4aa4b0953468eb8a3d05.s1.eu.hivemq.cloud"
#define DEFAULT_MQTT_PORT   "8883"
#define DEFAULT_MQTT_USER   "esp32"
#define DEFAULT_MQTT_PASS   "L8U8Zg7AA4PhRyV"

#endif