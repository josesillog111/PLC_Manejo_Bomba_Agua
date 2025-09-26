/*
 * PLC Manejo Bomba de Agua - Automatic Water Pump Controller
 * 
 * Este programa controla automáticamente una bomba de agua basado en:
 * - Intervalos de tiempo configurables
 * - Días específicos de la semana
 * - Control manual de emergencia
 * 
 * Características:
 * - RTC para mantener tiempo preciso
 * - Configuración mediante botones y display
 * - Monitoreo de estado y logs
 * - Protección contra funcionamiento en seco
 * - Interfaz serial para configuración avanzada
 * 
 * Hardware requerido:
 * - Arduino Uno/Nano
 * - Módulo RTC DS3231
 * - Relé para control de bomba
 * - Display LCD 16x2
 * - Botones de configuración
 * - Sensor de nivel de agua (opcional)
 * 
 * Autor: Sistema de Control de Bomba de Agua
 * Versión: 1.0
 */

#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
// #include "config.h"  // Descomenta para usar configuración personalizada

// Configuración de pines
#define PUMP_RELAY_PIN 8        // Pin del relé de la bomba
#define WATER_SENSOR_PIN A0     // Sensor de nivel de agua
#define BUTTON_UP_PIN 2         // Botón subir/siguiente
#define BUTTON_DOWN_PIN 3       // Botón bajar/anterior
#define BUTTON_SELECT_PIN 4     // Botón seleccionar/confirmar
#define BUTTON_BACK_PIN 5       // Botón atrás/cancelar
#define MANUAL_OVERRIDE_PIN 6   // Interruptor manual de emergencia
#define STATUS_LED_PIN 13       // LED de estado

// Configuración del LCD (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(7, 9, 10, 11, 12, A1);

// Configuración del RTC
RTC_DS3231 rtc;

// Estados del sistema
enum SystemState {
  STATE_MAIN_MENU,
  STATE_CONFIG_MODE,
  STATE_CONFIG_INTERVAL,
  STATE_CONFIG_DAYS,
  STATE_CONFIG_TIME,
  STATE_RUNNING,
  STATE_MANUAL_MODE
};

// Modos de operación
enum OperationMode {
  MODE_INTERVAL,    // Por intervalos de tiempo
  MODE_WEEKDAYS,    // Días específicos de la semana
  MODE_MANUAL       // Control manual
};

// Estructura de configuración
struct Config {
  OperationMode mode;
  // Configuración de intervalos
  uint16_t intervalMinutes;     // Intervalo en minutos
  uint16_t pumpDurationMinutes; // Duración del bombeo en minutos
  // Configuración de días específicos
  bool activeDays[7];           // Lun=0, Mar=1, ..., Dom=6
  uint8_t startHour;            // Hora de inicio
  uint8_t startMinute;          // Minuto de inicio
  uint8_t endHour;              // Hora de fin
  uint8_t endMinute;            // Minuto de fin
  // Configuración general
  bool waterSensorEnabled;      // Usar sensor de agua
  uint16_t minWaterLevel;       // Nivel mínimo de agua
  bool autoMode;                // Modo automático habilitado
} config;

// Variables de estado
SystemState currentState = STATE_MAIN_MENU;
unsigned long lastPumpStart = 0;
unsigned long lastPumpEnd = 0;
bool pumpRunning = false;
bool manualOverride = false;
unsigned long lastButtonPress = 0;
int menuIndex = 0;
bool configChanged = false;

// Direcciones EEPROM
#define EEPROM_CONFIG_ADDR 0
#define EEPROM_MAGIC_ADDR 100
#define EEPROM_MAGIC_VALUE 0xAB

void setup() {
  Serial.begin(9600);
  Serial.println(F("Iniciando Sistema de Control de Bomba de Agua"));
  
  // Inicializar pines
  pinMode(PUMP_RELAY_PIN, OUTPUT);
  pinMode(WATER_SENSOR_PIN, INPUT);
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
  pinMode(BUTTON_SELECT_PIN, INPUT_PULLUP);
  pinMode(BUTTON_BACK_PIN, INPUT_PULLUP);
  pinMode(MANUAL_OVERRIDE_PIN, INPUT_PULLUP);
  pinMode(STATUS_LED_PIN, OUTPUT);
  
  // Asegurar que la bomba esté apagada al inicio
  digitalWrite(PUMP_RELAY_PIN, LOW);
  digitalWrite(STATUS_LED_PIN, LOW);
  
  // Inicializar LCD
  lcd.begin(16, 2);
  lcd.print(F("Iniciando..."));
  lcd.setCursor(0, 1);
  lcd.print(F("Bomba de Agua"));
  delay(2000);
  
  // Inicializar RTC
  if (!rtc.begin()) {
    Serial.println(F("Error: No se encuentra el RTC"));
    lcd.clear();
    lcd.print(F("Error RTC"));
    lcd.setCursor(0, 1);
    lcd.print(F("Verificar conexion"));
    while (1);
  }
  
  // Si el RTC perdió energía, configurar fecha/hora actual
  if (rtc.lostPower()) {
    Serial.println(F("RTC sin energía, configurando fecha/hora"));
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
  // Cargar configuración desde EEPROM
  loadConfig();
  
  // Mostrar información inicial
  displayWelcomeMessage();
  
  Serial.println(F("Sistema inicializado correctamente"));
  logEvent("Sistema iniciado");
}

void loop() {
  // Verificar override manual
  checkManualOverride();
  
  // Procesar botones
  processButtons();
  
  // Actualizar estado del sistema
  updateSystemState();
  
  // Actualizar display
  updateDisplay();
  
  // Verificar y ejecutar operaciones de bombeo
  if (currentState == STATE_RUNNING) {
    checkPumpOperation();
  }
  
  // Guardar configuración si cambió
  if (configChanged) {
    saveConfig();
    configChanged = false;
  }
  
  delay(100); // Pequeña pausa para estabilidad
}

void checkManualOverride() {
  bool manualPressed = !digitalRead(MANUAL_OVERRIDE_PIN);
  
  if (manualPressed != manualOverride) {
    manualOverride = manualPressed;
    
    if (manualOverride) {
      Serial.println(F("Modo manual activado"));
      logEvent("Modo manual ON");
      currentState = STATE_MANUAL_MODE;
      turnPumpOn();
    } else {
      Serial.println(F("Modo manual desactivado"));
      logEvent("Modo manual OFF");
      currentState = STATE_RUNNING;
      turnPumpOff();
    }
  }
}

void processButtons() {
  static unsigned long lastDebounce = 0;
  const unsigned long debounceDelay = 50;
  
  if (millis() - lastDebounce < debounceDelay) {
    return;
  }
  
  bool upPressed = !digitalRead(BUTTON_UP_PIN);
  bool downPressed = !digitalRead(BUTTON_DOWN_PIN);
  bool selectPressed = !digitalRead(BUTTON_SELECT_PIN);
  bool backPressed = !digitalRead(BUTTON_BACK_PIN);
  
  if (upPressed || downPressed || selectPressed || backPressed) {
    lastDebounce = millis();
    lastButtonPress = millis();
    
    switch (currentState) {
      case STATE_MAIN_MENU:
        handleMainMenuButtons(upPressed, downPressed, selectPressed, backPressed);
        break;
      case STATE_CONFIG_MODE:
        handleConfigModeButtons(upPressed, downPressed, selectPressed, backPressed);
        break;
      case STATE_CONFIG_INTERVAL:
        handleIntervalConfigButtons(upPressed, downPressed, selectPressed, backPressed);
        break;
      case STATE_CONFIG_DAYS:
        handleDaysConfigButtons(upPressed, downPressed, selectPressed, backPressed);
        break;
      case STATE_CONFIG_TIME:
        handleTimeConfigButtons(upPressed, downPressed, selectPressed, backPressed);
        break;
      default:
        if (backPressed) {
          currentState = STATE_MAIN_MENU;
          menuIndex = 0;
        }
        break;
    }
  }
  
  // Timeout del menú - volver al estado de funcionamiento
  if (currentState != STATE_RUNNING && currentState != STATE_MANUAL_MODE) {
    if (millis() - lastButtonPress > 30000) { // 30 segundos
      currentState = STATE_RUNNING;
      menuIndex = 0;
    }
  }
}

void handleMainMenuButtons(bool up, bool down, bool select, bool back) {
  if (up && menuIndex > 0) {
    menuIndex--;
  } else if (down && menuIndex < 3) {
    menuIndex++;
  } else if (select) {
    switch (menuIndex) {
      case 0: // Configuración
        currentState = STATE_CONFIG_MODE;
        menuIndex = 0;
        break;
      case 1: // Estado del sistema
        // Mostrar estado detallado
        break;
      case 2: // Inicio manual
        if (!pumpRunning) {
          turnPumpOn();
          logEvent("Inicio manual");
        }
        break;
      case 3: // Volver al modo automático
        currentState = STATE_RUNNING;
        break;
    }
  }
}

void handleConfigModeButtons(bool up, bool down, bool select, bool back) {
  if (back) {
    currentState = STATE_MAIN_MENU;
    menuIndex = 0;
    return;
  }
  
  if (up && menuIndex > 0) {
    menuIndex--;
  } else if (down && menuIndex < 4) {
    menuIndex++;
  } else if (select) {
    switch (menuIndex) {
      case 0: // Modo de operación
        config.mode = (OperationMode)((config.mode + 1) % 3);
        configChanged = true;
        break;
      case 1: // Configurar intervalos
        if (config.mode == MODE_INTERVAL) {
          currentState = STATE_CONFIG_INTERVAL;
          menuIndex = 0;
        }
        break;
      case 2: // Configurar días
        if (config.mode == MODE_WEEKDAYS) {
          currentState = STATE_CONFIG_DAYS;
          menuIndex = 0;
        }
        break;
      case 3: // Configurar hora actual
        currentState = STATE_CONFIG_TIME;
        menuIndex = 0;
        break;
      case 4: // Sensor de agua
        config.waterSensorEnabled = !config.waterSensorEnabled;
        configChanged = true;
        break;
    }
  }
}

void handleIntervalConfigButtons(bool up, bool down, bool select, bool back) {
  if (back) {
    currentState = STATE_CONFIG_MODE;
    menuIndex = 1;
    return;
  }
  
  if (up && menuIndex > 0) {
    menuIndex--;
  } else if (down && menuIndex < 1) {
    menuIndex++;
  } else if (select) {
    configChanged = true;
    if (menuIndex == 0) { // Intervalo
      if (up) config.intervalMinutes += 15;
      if (down && config.intervalMinutes > 15) config.intervalMinutes -= 15;
    } else if (menuIndex == 1) { // Duración
      if (up) config.pumpDurationMinutes += 5;
      if (down && config.pumpDurationMinutes > 5) config.pumpDurationMinutes -= 5;
    }
  }
}

void handleDaysConfigButtons(bool up, bool down, bool select, bool back) {
  if (back) {
    currentState = STATE_CONFIG_MODE;
    menuIndex = 2;
    return;
  }
  
  if (up && menuIndex > 0) {
    menuIndex--;
  } else if (down && menuIndex < 8) {
    menuIndex++;
  } else if (select) {
    configChanged = true;
    if (menuIndex < 7) { // Días de la semana
      config.activeDays[menuIndex] = !config.activeDays[menuIndex];
    } else if (menuIndex == 7) { // Hora de inicio
      // Incrementar hora de inicio
      config.startHour = (config.startHour + 1) % 24;
    } else if (menuIndex == 8) { // Hora de fin
      // Incrementar hora de fin
      config.endHour = (config.endHour + 1) % 24;
    }
  }
}

void handleTimeConfigButtons(bool up, bool down, bool select, bool back) {
  if (back) {
    currentState = STATE_CONFIG_MODE;
    menuIndex = 3;
    return;
  }
  
  DateTime now = rtc.now();
  int year = now.year();
  int month = now.month();
  int day = now.day();
  int hour = now.hour();
  int minute = now.minute();
  
  if (up && menuIndex > 0) {
    menuIndex--;
  } else if (down && menuIndex < 4) {
    menuIndex++;
  } else if (select) {
    switch (menuIndex) {
      case 0: // Año
        year = up ? year + 1 : (year > 2020 ? year - 1 : year);
        break;
      case 1: // Mes
        month = up ? (month < 12 ? month + 1 : 1) : (month > 1 ? month - 1 : 12);
        break;
      case 2: // Día
        day = up ? (day < 31 ? day + 1 : 1) : (day > 1 ? day - 1 : 31);
        break;
      case 3: // Hora
        hour = up ? (hour < 23 ? hour + 1 : 0) : (hour > 0 ? hour - 1 : 23);
        break;
      case 4: // Minuto
        minute = up ? (minute < 59 ? minute + 1 : 0) : (minute > 0 ? minute - 1 : 59);
        break;
    }
    rtc.adjust(DateTime(year, month, day, hour, minute, 0));
  }
}

void updateSystemState() {
  // Actualizar LED de estado
  if (pumpRunning) {
    digitalWrite(STATUS_LED_PIN, (millis() / 500) % 2); // Parpadeo rápido
  } else if (config.autoMode) {
    digitalWrite(STATUS_LED_PIN, (millis() / 2000) % 2); // Parpadeo lento
  } else {
    digitalWrite(STATUS_LED_PIN, LOW); // Apagado
  }
}

void checkPumpOperation() {
  if (manualOverride) return; // No operar en modo manual
  
  DateTime now = rtc.now();
  bool shouldRun = false;
  
  switch (config.mode) {
    case MODE_INTERVAL:
      shouldRun = checkIntervalMode(now);
      break;
    case MODE_WEEKDAYS:
      shouldRun = checkWeekdayMode(now);
      break;
    case MODE_MANUAL:
      // No operación automática en modo manual
      break;
  }
  
  // Verificar sensor de agua si está habilitado
  if (shouldRun && config.waterSensorEnabled) {
    int waterLevel = analogRead(WATER_SENSOR_PIN);
    if (waterLevel < config.minWaterLevel) {
      shouldRun = false;
      Serial.println(F("Bombeo cancelado: Nivel de agua bajo"));
      logEvent("Nivel agua bajo");
    }
  }
  
  // Actualizar estado de la bomba
  if (shouldRun && !pumpRunning) {
    turnPumpOn();
  } else if (!shouldRun && pumpRunning) {
    turnPumpOff();
  }
}

bool checkIntervalMode(DateTime now) {
  unsigned long currentMillis = millis();
  unsigned long intervalMs = (unsigned long)config.intervalMinutes * 60000UL;
  unsigned long durationMs = (unsigned long)config.pumpDurationMinutes * 60000UL;
  
  if (!pumpRunning) {
    // Verificar si es tiempo de iniciar
    if (lastPumpEnd == 0 || (currentMillis - lastPumpEnd) >= intervalMs) {
      return true;
    }
  } else {
    // Verificar si es tiempo de parar
    if ((currentMillis - lastPumpStart) >= durationMs) {
      return false;
    }
  }
  
  return pumpRunning; // Mantener estado actual
}

bool checkWeekdayMode(DateTime now) {
  int dayOfWeek = now.dayOfTheWeek(); // 0=Domingo, 1=Lunes, etc.
  int currentHour = now.hour();
  int currentMinute = now.minute();
  
  // Ajustar índice para que Lunes=0
  int adjustedDay = (dayOfWeek + 6) % 7;
  
  // Verificar si hoy es un día activo
  if (!config.activeDays[adjustedDay]) {
    return false;
  }
  
  // Verificar si estamos en el rango de tiempo
  int currentTimeMinutes = currentHour * 60 + currentMinute;
  int startTimeMinutes = config.startHour * 60 + config.startMinute;
  int endTimeMinutes = config.endHour * 60 + config.endMinute;
  
  if (startTimeMinutes <= endTimeMinutes) {
    // Rango normal (no cruza medianoche)
    return (currentTimeMinutes >= startTimeMinutes && currentTimeMinutes < endTimeMinutes);
  } else {
    // Rango que cruza medianoche
    return (currentTimeMinutes >= startTimeMinutes || currentTimeMinutes < endTimeMinutes);
  }
}

void turnPumpOn() {
  if (!pumpRunning) {
    digitalWrite(PUMP_RELAY_PIN, HIGH);
    pumpRunning = true;
    lastPumpStart = millis();
    Serial.println(F("Bomba ENCENDIDA"));
    logEvent("Bomba ON");
  }
}

void turnPumpOff() {
  if (pumpRunning) {
    digitalWrite(PUMP_RELAY_PIN, LOW);
    pumpRunning = false;
    lastPumpEnd = millis();
    Serial.println(F("Bomba APAGADA"));
    logEvent("Bomba OFF");
  }
}

void updateDisplay() {
  lcd.clear();
  
  switch (currentState) {
    case STATE_MAIN_MENU:
      displayMainMenu();
      break;
    case STATE_CONFIG_MODE:
      displayConfigMenu();
      break;
    case STATE_CONFIG_INTERVAL:
      displayIntervalConfig();
      break;
    case STATE_CONFIG_DAYS:
      displayDaysConfig();
      break;
    case STATE_CONFIG_TIME:
      displayTimeConfig();
      break;
    case STATE_RUNNING:
      displayRunningStatus();
      break;
    case STATE_MANUAL_MODE:
      displayManualMode();
      break;
  }
}

void displayMainMenu() {
  const char* menuItems[] = {
    "Configuracion",
    "Estado Sistema",
    "Inicio Manual",
    "Modo Automatico"
  };
  
  lcd.setCursor(0, 0);
  lcd.print(F("Menu Principal"));
  lcd.setCursor(0, 1);
  if (menuIndex < 4) {
    lcd.print(menuIndex == 0 ? ">" : " ");
    lcd.print(menuItems[menuIndex]);
  }
}

void displayConfigMenu() {
  lcd.setCursor(0, 0);
  lcd.print(F("Configuracion"));
  lcd.setCursor(0, 1);
  
  switch (menuIndex) {
    case 0:
      lcd.print(F(">Modo: "));
      lcd.print(config.mode == MODE_INTERVAL ? F("Intervalo") : 
                config.mode == MODE_WEEKDAYS ? F("Dias") : F("Manual"));
      break;
    case 1:
      lcd.print(F(">Config Intervalo"));
      break;
    case 2:
      lcd.print(F(">Config Dias"));
      break;
    case 3:
      lcd.print(F(">Ajustar Hora"));
      break;
    case 4:
      lcd.print(F(">Sensor: "));
      lcd.print(config.waterSensorEnabled ? F("ON") : F("OFF"));
      break;
  }
}

void displayIntervalConfig() {
  lcd.setCursor(0, 0);
  lcd.print(F("Config Intervalo"));
  lcd.setCursor(0, 1);
  
  if (menuIndex == 0) {
    lcd.print(F(">Cada: "));
    lcd.print(config.intervalMinutes);
    lcd.print(F("min"));
  } else {
    lcd.print(F(">Duracion: "));
    lcd.print(config.pumpDurationMinutes);
    lcd.print(F("m"));
  }
}

void displayDaysConfig() {
  const char* dayNames[] = {"L", "M", "X", "J", "V", "S", "D"};
  
  lcd.setCursor(0, 0);
  lcd.print(F("Config Dias"));
  lcd.setCursor(0, 1);
  
  if (menuIndex < 7) {
    lcd.print(F(">"));
    lcd.print(dayNames[menuIndex]);
    lcd.print(F(": "));
    lcd.print(config.activeDays[menuIndex] ? F("ON") : F("OFF"));
  } else if (menuIndex == 7) {
    lcd.print(F(">Inicio: "));
    if (config.startHour < 10) lcd.print(F("0"));
    lcd.print(config.startHour);
    lcd.print(F(":"));
    if (config.startMinute < 10) lcd.print(F("0"));
    lcd.print(config.startMinute);
  } else {
    lcd.print(F(">Fin: "));
    if (config.endHour < 10) lcd.print(F("0"));
    lcd.print(config.endHour);
    lcd.print(F(":"));
    if (config.endMinute < 10) lcd.print(F("0"));
    lcd.print(config.endMinute);
  }
}

void displayTimeConfig() {
  DateTime now = rtc.now();
  lcd.setCursor(0, 0);
  lcd.print(F("Ajustar Hora"));
  lcd.setCursor(0, 1);
  
  switch (menuIndex) {
    case 0:
      lcd.print(F(">Año: "));
      lcd.print(now.year());
      break;
    case 1:
      lcd.print(F(">Mes: "));
      lcd.print(now.month());
      break;
    case 2:
      lcd.print(F(">Dia: "));
      lcd.print(now.day());
      break;
    case 3:
      lcd.print(F(">Hora: "));
      lcd.print(now.hour());
      break;
    case 4:
      lcd.print(F(">Min: "));
      lcd.print(now.minute());
      break;
  }
}

void displayRunningStatus() {
  DateTime now = rtc.now();
  
  // Primera línea: fecha y hora
  lcd.setCursor(0, 0);
  if (now.day() < 10) lcd.print(F("0"));
  lcd.print(now.day());
  lcd.print(F("/"));
  if (now.month() < 10) lcd.print(F("0"));
  lcd.print(now.month());
  lcd.print(F(" "));
  if (now.hour() < 10) lcd.print(F("0"));
  lcd.print(now.hour());
  lcd.print(F(":"));
  if (now.minute() < 10) lcd.print(F("0"));
  lcd.print(now.minute());
  
  // Segunda línea: estado de la bomba
  lcd.setCursor(0, 1);
  lcd.print(F("Bomba: "));
  lcd.print(pumpRunning ? F("ON ") : F("OFF"));
  
  // Modo actual
  lcd.print(F(" M:"));
  lcd.print(config.mode == MODE_INTERVAL ? F("I") : 
            config.mode == MODE_WEEKDAYS ? F("D") : F("M"));
}

void displayManualMode() {
  lcd.setCursor(0, 0);
  lcd.print(F("MODO MANUAL"));
  lcd.setCursor(0, 1);
  lcd.print(F("Bomba: "));
  lcd.print(pumpRunning ? F("ENCENDIDA") : F("APAGADA"));
}

void displayWelcomeMessage() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Sistema Bomba"));
  lcd.setCursor(0, 1);
  lcd.print(F("de Agua v1.0"));
  delay(2000);
  
  DateTime now = rtc.now();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Fecha/Hora:"));
  lcd.setCursor(0, 1);
  lcd.print(now.day());
  lcd.print(F("/"));
  lcd.print(now.month());
  lcd.print(F(" "));
  lcd.print(now.hour());
  lcd.print(F(":"));
  lcd.print(now.minute());
  delay(2000);
  
  currentState = STATE_RUNNING;
}

void loadConfig() {
  // Verificar si hay configuración válida en EEPROM
  if (EEPROM.read(EEPROM_MAGIC_ADDR) == EEPROM_MAGIC_VALUE) {
    EEPROM.get(EEPROM_CONFIG_ADDR, config);
    Serial.println(F("Configuración cargada desde EEPROM"));
  } else {
    // Configuración por defecto
    config.mode = MODE_INTERVAL;
    config.intervalMinutes = 60;        // Cada hora
    config.pumpDurationMinutes = 10;    // 10 minutos
    for (int i = 0; i < 7; i++) {
      config.activeDays[i] = (i < 5);   // Lun-Vie por defecto
    }
    config.startHour = 6;               // 6:00 AM
    config.startMinute = 0;
    config.endHour = 22;                // 10:00 PM
    config.endMinute = 0;
    config.waterSensorEnabled = false;
    config.minWaterLevel = 300;
    config.autoMode = true;
    
    Serial.println(F("Configuración por defecto cargada"));
    configChanged = true;
  }
}

void saveConfig() {
  EEPROM.put(EEPROM_CONFIG_ADDR, config);
  EEPROM.write(EEPROM_MAGIC_ADDR, EEPROM_MAGIC_VALUE);
  Serial.println(F("Configuración guardada en EEPROM"));
}

void logEvent(const char* event) {
  DateTime now = rtc.now();
  
  Serial.print(now.year());
  Serial.print(F("/"));
  Serial.print(now.month());
  Serial.print(F("/"));
  Serial.print(now.day());
  Serial.print(F(" "));
  Serial.print(now.hour());
  Serial.print(F(":"));
  Serial.print(now.minute());
  Serial.print(F(":"));
  Serial.print(now.second());
  Serial.print(F(" - "));
  Serial.println(event);
}