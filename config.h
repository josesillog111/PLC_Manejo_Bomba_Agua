/*
 * config.h - Archivo de configuración para Sistema de Control de Bomba de Agua
 * 
 * Este archivo contiene todas las configuraciones personalizables del sistema.
 * Modificar estos valores según las necesidades específicas de la instalación.
 */

#ifndef CONFIG_H
#define CONFIG_H

// =============================================================================
// CONFIGURACIÓN DE HARDWARE
// =============================================================================

// Pines de control
#define PUMP_RELAY_PIN 8        // Pin del relé de la bomba
#define WATER_SENSOR_PIN A0     // Sensor de nivel de agua (analógico)
#define STATUS_LED_PIN 13       // LED de estado del sistema

// Pines de interfaz de usuario
#define BUTTON_UP_PIN 2         // Botón navegación arriba
#define BUTTON_DOWN_PIN 3       // Botón navegación abajo  
#define BUTTON_SELECT_PIN 4     // Botón seleccionar/confirmar
#define BUTTON_BACK_PIN 5       // Botón atrás/cancelar
#define MANUAL_OVERRIDE_PIN 6   // Interruptor manual de emergencia

// Configuración del LCD (RS, E, D4, D5, D6, D7)
#define LCD_RS_PIN 7
#define LCD_E_PIN 9
#define LCD_D4_PIN 10
#define LCD_D5_PIN 11
#define LCD_D6_PIN 12
#define LCD_D7_PIN A1

// =============================================================================
// CONFIGURACIÓN DE TEMPORIZACIÓN
// =============================================================================

// Intervalos de tiempo (en minutos)
#define DEFAULT_INTERVAL_MINUTES 60     // Intervalo por defecto: cada hora
#define DEFAULT_DURATION_MINUTES 10     // Duración por defecto: 10 minutos
#define MIN_INTERVAL_MINUTES 15         // Intervalo mínimo permitido
#define MIN_DURATION_MINUTES 5          // Duración mínima permitida
#define MAX_INTERVAL_MINUTES 1440       // Intervalo máximo: 24 horas
#define MAX_DURATION_MINUTES 120        // Duración máxima: 2 horas

// Horarios por defecto para modo días específicos
#define DEFAULT_START_HOUR 6            // Hora de inicio: 6:00 AM
#define DEFAULT_START_MINUTE 0
#define DEFAULT_END_HOUR 22             // Hora de fin: 10:00 PM  
#define DEFAULT_END_MINUTE 0

// Timeouts de interfaz (en milisegundos)
#define MENU_TIMEOUT_MS 30000           // 30 segundos hasta volver a modo automático
#define BUTTON_DEBOUNCE_MS 50           // Antirrebote de botones
#define DISPLAY_UPDATE_MS 100           // Actualización del display

// =============================================================================
// CONFIGURACIÓN DEL SENSOR DE AGUA
// =============================================================================

#define DEFAULT_MIN_WATER_LEVEL 300     // Nivel mínimo de agua (0-1023)
#define WATER_SENSOR_ENABLED false      // Sensor habilitado por defecto
#define WATER_CHECK_INTERVAL_MS 5000    // Verificar nivel cada 5 segundos

// =============================================================================
// CONFIGURACIÓN DE COMUNICACIÓN SERIAL
// =============================================================================

#define SERIAL_BAUD_RATE 9600           // Velocidad del puerto serial
#define ENABLE_SERIAL_LOGGING true      // Habilitar logging por serial

// =============================================================================
// CONFIGURACIÓN DE EEPROM
// =============================================================================

#define EEPROM_CONFIG_ADDR 0            // Dirección base de configuración
#define EEPROM_MAGIC_ADDR 100           // Dirección del valor mágico
#define EEPROM_MAGIC_VALUE 0xAB         // Valor para verificar EEPROM válida

// =============================================================================
// CONFIGURACIÓN DE SEGURIDAD
// =============================================================================

// Protecciones del sistema
#define ENABLE_DRY_RUN_PROTECTION true  // Protección contra funcionamiento en seco
#define ENABLE_OVERCURRENT_PROTECTION false // Protección contra sobrecorriente (requiere sensor)
#define MAX_CONTINUOUS_RUN_HOURS 4      // Máximo tiempo continuo de funcionamiento

// Reinicio automático
#define ENABLE_WATCHDOG false           // Habilitar watchdog timer
#define WATCHDOG_TIMEOUT_MS 8000        // Timeout del watchdog

// =============================================================================
// CONFIGURACIÓN DE DÍAS DE LA SEMANA
// =============================================================================

// Días activos por defecto (true = activo)
// Índices: 0=Lunes, 1=Martes, 2=Miércoles, 3=Jueves, 4=Viernes, 5=Sábado, 6=Domingo
#define DEFAULT_MONDAY true
#define DEFAULT_TUESDAY true  
#define DEFAULT_WEDNESDAY true
#define DEFAULT_THURSDAY true
#define DEFAULT_FRIDAY true
#define DEFAULT_SATURDAY false
#define DEFAULT_SUNDAY false

// =============================================================================
// CONFIGURACIÓN DE DISPLAY Y INTERFAZ
// =============================================================================

// Configuración del LCD
#define LCD_COLUMNS 16                  // Número de columnas del LCD
#define LCD_ROWS 2                      // Número de filas del LCD
#define LCD_BACKLIGHT_PIN -1            // Pin de backlight (-1 si no se usa)

// Mensajes de interfaz (para personalización de idioma)
#define MSG_SYSTEM_STARTING "Iniciando Sistema"
#define MSG_PUMP_ON "Bomba ENCENDIDA"
#define MSG_PUMP_OFF "Bomba APAGADA"  
#define MSG_MANUAL_MODE "MODO MANUAL"
#define MSG_AUTO_MODE "MODO AUTOMATICO"
#define MSG_LOW_WATER "Nivel agua bajo"
#define MSG_CONFIGURATION "Configuracion"

// =============================================================================
// CONFIGURACIÓN AVANZADA
// =============================================================================

// Configuración de interrupciones
#define ENABLE_BUTTON_INTERRUPTS false // Usar interrupciones para botones
#define ENABLE_RTC_ALARM false          // Usar alarma del RTC

// Configuración de diagnóstico
#define ENABLE_DIAGNOSTICS true        // Habilitar modo diagnóstico
#define DIAGNOSTIC_PIN A2               // Pin para activar diagnóstico
#define ENABLE_MEMORY_MONITORING false // Monitorear uso de memoria

// Configuración de expansión
#define ENABLE_MULTIPLE_PUMPS false    // Soporte para múltiples bombas
#define MAX_PUMPS 4                     // Número máximo de bombas
#define ENABLE_MODBUS false             // Comunicación Modbus
#define ENABLE_WIFI false               // Conectividad WiFi

// =============================================================================
// MACROS DE UTILIDAD
// =============================================================================

// Conversión de tiempo
#define MINUTES_TO_MS(min) ((unsigned long)(min) * 60000UL)
#define HOURS_TO_MS(hrs) ((unsigned long)(hrs) * 3600000UL)
#define SECONDS_TO_MS(sec) ((unsigned long)(sec) * 1000UL)

// Macros de depuración
#if ENABLE_SERIAL_LOGGING
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

// Validación de configuración
#if MIN_INTERVAL_MINUTES >= MAX_INTERVAL_MINUTES
  #error "MIN_INTERVAL_MINUTES debe ser menor que MAX_INTERVAL_MINUTES"
#endif

#if MIN_DURATION_MINUTES >= MAX_DURATION_MINUTES
  #error "MIN_DURATION_MINUTES debe ser menor que MAX_DURATION_MINUTES"
#endif

#if DEFAULT_INTERVAL_MINUTES < MIN_INTERVAL_MINUTES
  #error "DEFAULT_INTERVAL_MINUTES debe ser mayor o igual a MIN_INTERVAL_MINUTES"
#endif

#endif // CONFIG_H