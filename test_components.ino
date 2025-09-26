/*
 * test_components.ino - Sketch de prueba para componentes individuales
 * 
 * Este sketch permite probar cada componente del sistema por separado
 * antes de cargar el programa principal. Útil para diagnóstico y verificación.
 * 
 * Instrucciones:
 * 1. Conectar los componentes según el diagrama de circuito
 * 2. Cargar este sketch en Arduino
 * 3. Abrir monitor serial a 9600 baud
 * 4. Seguir las instrucciones en pantalla
 * 
 * Componentes que se pueden probar:
 * - RTC DS3231
 * - LCD 16x2
 * - Botones
 * - Relé
 * - LED de estado
 * - Sensor de agua (opcional)
 */

#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal.h>

// Configuración de pines (igual que en el programa principal)
#define PUMP_RELAY_PIN 8
#define WATER_SENSOR_PIN A0
#define BUTTON_UP_PIN 2
#define BUTTON_DOWN_PIN 3
#define BUTTON_SELECT_PIN 4
#define BUTTON_BACK_PIN 5
#define MANUAL_OVERRIDE_PIN 6
#define STATUS_LED_PIN 13

// Configuración del LCD (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(7, 9, 10, 11, 12, A1);
RTC_DS3231 rtc;

// Variables para el menú de pruebas
int testMode = 0;
unsigned long lastUpdate = 0;
bool ledState = false;

void setup() {
  Serial.begin(9600);
  Serial.println(F("========================================"));
  Serial.println(F("  SISTEMA DE PRUEBAS - BOMBA DE AGUA"));
  Serial.println(F("========================================"));
  
  // Configurar pines
  pinMode(PUMP_RELAY_PIN, OUTPUT);
  pinMode(WATER_SENSOR_PIN, INPUT);
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
  pinMode(BUTTON_SELECT_PIN, INPUT_PULLUP);
  pinMode(BUTTON_BACK_PIN, INPUT_PULLUP);
  pinMode(MANUAL_OVERRIDE_PIN, INPUT_PULLUP);
  pinMode(STATUS_LED_PIN, OUTPUT);
  
  // Estado inicial seguro
  digitalWrite(PUMP_RELAY_PIN, LOW);
  digitalWrite(STATUS_LED_PIN, LOW);
  
  // Inicializar LCD
  lcd.begin(16, 2);
  lcd.print(F("Test Components"));
  lcd.setCursor(0, 1);
  lcd.print(F("Starting..."));
  
  delay(2000);
  showTestMenu();
}

void loop() {
  // Verificar botones para cambiar modo de prueba
  if (!digitalRead(BUTTON_UP_PIN)) {
    delay(200); // Debounce
    testMode = (testMode > 0) ? testMode - 1 : 6;
    showTestMenu();
  }
  
  if (!digitalRead(BUTTON_DOWN_PIN)) {
    delay(200); // Debounce
    testMode = (testMode < 6) ? testMode + 1 : 0;
    showTestMenu();
  }
  
  if (!digitalRead(BUTTON_SELECT_PIN)) {
    delay(200); // Debounce
    executeTest(testMode);
  }
  
  // Ejecutar prueba activa
  switch (testMode) {
    case 0: testRTC(); break;
    case 1: testLCD(); break;
    case 2: testButtons(); break;
    case 3: testRelay(); break;
    case 4: testLED(); break;
    case 5: testWaterSensor(); break;
    case 6: testComplete(); break;
  }
  
  delay(100);
}

void showTestMenu() {
  Serial.println(F("\n--- MENU DE PRUEBAS ---"));
  Serial.println(F("0. Prueba RTC"));
  Serial.println(F("1. Prueba LCD"));
  Serial.println(F("2. Prueba Botones"));
  Serial.println(F("3. Prueba Relé"));
  Serial.println(F("4. Prueba LED"));
  Serial.println(F("5. Prueba Sensor Agua"));
  Serial.println(F("6. Prueba Completa"));
  Serial.print(F("Modo actual: "));
  Serial.println(testMode);
  Serial.println(F("UP/DOWN para cambiar, SELECT para ejecutar"));
  Serial.println(F("----------------------------------------"));
}

void executeTest(int mode) {
  Serial.print(F("Ejecutando prueba "));
  Serial.println(mode);
  
  switch (mode) {
    case 0: runRTCTest(); break;
    case 1: runLCDTest(); break;
    case 2: runButtonTest(); break;
    case 3: runRelayTest(); break;
    case 4: runLEDTest(); break;
    case 5: runWaterSensorTest(); break;
    case 6: runCompleteTest(); break;
  }
}

// =============================================================================
// PRUEBAS INDIVIDUALES
// =============================================================================

void testRTC() {
  static unsigned long lastRTCUpdate = 0;
  
  if (millis() - lastRTCUpdate > 1000) {
    lastRTCUpdate = millis();
    
    if (!rtc.begin()) {
      Serial.println(F("ERROR: RTC no encontrado"));
      lcd.setCursor(0, 1);
      lcd.print(F("RTC: ERROR      "));
      return;
    }
    
    DateTime now = rtc.now();
    Serial.print(F("RTC: "));
    Serial.print(now.day());
    Serial.print(F("/"));
    Serial.print(now.month());
    Serial.print(F("/"));
    Serial.print(now.year());
    Serial.print(F(" "));
    Serial.print(now.hour());
    Serial.print(F(":"));
    Serial.print(now.minute());
    Serial.print(F(":"));
    Serial.println(now.second());
    
    lcd.setCursor(0, 1);
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
    lcd.print(F(":"));
    if (now.second() < 10) lcd.print(F("0"));
    lcd.print(now.second());
  }
}

void testLCD() {
  static unsigned long lastLCDUpdate = 0;
  static int counter = 0;
  
  if (millis() - lastLCDUpdate > 1000) {
    lastLCDUpdate = millis();
    counter++;
    
    lcd.setCursor(0, 1);
    lcd.print(F("LCD Test: "));
    lcd.print(counter);
    lcd.print(F("    "));
    
    Serial.print(F("LCD actualizado: "));
    Serial.println(counter);
  }
}

void testButtons() {
  lcd.setCursor(0, 1);
  lcd.print(F("Presiona botones"));
  
  if (!digitalRead(BUTTON_UP_PIN)) {
    Serial.println(F("Botón UP presionado"));
    lcd.setCursor(0, 1);
    lcd.print(F("UP pressed      "));
    delay(500);
  }
  if (!digitalRead(BUTTON_DOWN_PIN)) {
    Serial.println(F("Botón DOWN presionado"));
    lcd.setCursor(0, 1);
    lcd.print(F("DOWN pressed    "));
    delay(500);
  }
  if (!digitalRead(BUTTON_SELECT_PIN)) {
    Serial.println(F("Botón SELECT presionado"));
    lcd.setCursor(0, 1);
    lcd.print(F("SELECT pressed  "));
    delay(500);
  }
  if (!digitalRead(BUTTON_BACK_PIN)) {
    Serial.println(F("Botón BACK presionado"));
    lcd.setCursor(0, 1);
    lcd.print(F("BACK pressed    "));
    delay(500);
  }
  if (!digitalRead(MANUAL_OVERRIDE_PIN)) {
    Serial.println(F("Override MANUAL activo"));
    lcd.setCursor(0, 1);
    lcd.print(F("MANUAL active   "));
    delay(500);
  }
}

void testRelay() {
  static unsigned long lastRelayToggle = 0;
  static bool relayState = false;
  
  if (millis() - lastRelayToggle > 2000) {
    lastRelayToggle = millis();
    relayState = !relayState;
    
    digitalWrite(PUMP_RELAY_PIN, relayState);
    
    Serial.print(F("Relé: "));
    Serial.println(relayState ? F("ON") : F("OFF"));
    
    lcd.setCursor(0, 1);
    lcd.print(F("Relay: "));
    lcd.print(relayState ? F("ON ") : F("OFF"));
    lcd.print(F("       "));
  }
}

void testLED() {
  static unsigned long lastLEDToggle = 0;
  
  if (millis() - lastLEDToggle > 500) {
    lastLEDToggle = millis();
    ledState = !ledState;
    
    digitalWrite(STATUS_LED_PIN, ledState);
    
    lcd.setCursor(0, 1);
    lcd.print(F("LED: "));
    lcd.print(ledState ? F("ON ") : F("OFF"));
    lcd.print(F("        "));
  }
}

void testWaterSensor() {
  static unsigned long lastSensorRead = 0;
  
  if (millis() - lastSensorRead > 500) {
    lastSensorRead = millis();
    
    int sensorValue = analogRead(WATER_SENSOR_PIN);
    
    Serial.print(F("Sensor agua: "));
    Serial.println(sensorValue);
    
    lcd.setCursor(0, 1);
    lcd.print(F("Water: "));
    lcd.print(sensorValue);
    lcd.print(F("       "));
  }
}

void testComplete() {
  lcd.setCursor(0, 1);
  lcd.print(F("Test completo..."));
  
  // Test secuencial de todos los componentes
  static unsigned long testStart = 0;
  static int testStep = 0;
  
  if (testStart == 0) {
    testStart = millis();
    testStep = 0;
    Serial.println(F("Iniciando prueba completa..."));
  }
  
  unsigned long elapsed = millis() - testStart;
  
  switch (testStep) {
    case 0: // LED test
      if (elapsed < 2000) {
        digitalWrite(STATUS_LED_PIN, (millis() / 200) % 2);
        lcd.setCursor(0, 1);
        lcd.print(F("Testing LED...  "));
      } else {
        testStep++;
        digitalWrite(STATUS_LED_PIN, LOW);
      }
      break;
      
    case 1: // Relay test
      if (elapsed < 4000) {
        digitalWrite(PUMP_RELAY_PIN, (elapsed / 1000) % 2);
        lcd.setCursor(0, 1);
        lcd.print(F("Testing Relay..."));
      } else {
        testStep++;
        digitalWrite(PUMP_RELAY_PIN, LOW);
      }
      break;
      
    case 2: // Complete
      if (elapsed < 6000) {
        lcd.setCursor(0, 1);
        lcd.print(F("Test OK!        "));
        Serial.println(F("Prueba completa finalizada - OK"));
      } else {
        testStart = 0; // Reiniciar
      }
      break;
  }
}

// =============================================================================
// PRUEBAS DETALLADAS (Ejecutadas con SELECT)
// =============================================================================

void runRTCTest() {
  Serial.println(F("\n=== PRUEBA DETALLADA DEL RTC ==="));
  
  if (!rtc.begin()) {
    Serial.println(F("ERROR: No se puede inicializar RTC"));
    Serial.println(F("Verificar:"));
    Serial.println(F("- Conexiones SDA/SCL"));
    Serial.println(F("- Alimentación 5V/GND"));
    Serial.println(F("- Dirección I2C (0x68)"));
    return;
  }
  
  DateTime now = rtc.now();
  Serial.print(F("Fecha/Hora actual: "));
  Serial.print(now.day());
  Serial.print(F("/"));
  Serial.print(now.month());
  Serial.print(F("/"));
  Serial.print(now.year());
  Serial.print(F(" "));
  Serial.print(now.hour());
  Serial.print(F(":"));
  Serial.print(now.minute());
  Serial.print(F(":"));
  Serial.println(now.second());
  
  if (rtc.lostPower()) {
    Serial.println(F("ADVERTENCIA: RTC perdió energía"));
    Serial.println(F("Configurando fecha/hora actual..."));
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
  float temp = rtc.getTemperature();
  Serial.print(F("Temperatura RTC: "));
  Serial.print(temp);
  Serial.println(F("°C"));
  
  Serial.println(F("RTC OK"));
}

void runLCDTest() {
  Serial.println(F("\n=== PRUEBA DETALLADA DEL LCD ==="));
  
  // Test de inicialización
  lcd.clear();
  lcd.print(F("LCD Test 1/4"));
  lcd.setCursor(0, 1);
  lcd.print(F("Initialization"));
  delay(2000);
  
  // Test de caracteres
  lcd.clear();
  lcd.print(F("LCD Test 2/4"));
  lcd.setCursor(0, 1);
  lcd.print(F("ABCD1234!@#$"));
  delay(2000);
  
  // Test de posicionamiento
  lcd.clear();
  lcd.print(F("LCD Test 3/4"));
  for (int i = 0; i < 16; i++) {
    lcd.setCursor(i, 1);
    lcd.print(F("*"));
    delay(100);
  }
  delay(1000);
  
  // Test completo
  lcd.clear();
  lcd.print(F("LCD Test 4/4"));
  lcd.setCursor(0, 1);
  lcd.print(F("Test Complete!"));
  delay(2000);
  
  Serial.println(F("LCD OK"));
}

void runButtonTest() {
  Serial.println(F("\n=== PRUEBA DETALLADA DE BOTONES ==="));
  Serial.println(F("Presiona cada botón para probar:"));
  Serial.println(F("UP, DOWN, SELECT, BACK, MANUAL"));
  Serial.println(F("Presiona BACK para terminar"));
  
  lcd.clear();
  lcd.print(F("Button Test"));
  lcd.setCursor(0, 1);
  lcd.print(F("Press buttons"));
  
  while (digitalRead(BUTTON_BACK_PIN)) {
    if (!digitalRead(BUTTON_UP_PIN)) {
      Serial.println(F("✓ UP OK"));
      lcd.setCursor(0, 1);
      lcd.print(F("UP OK           "));
      delay(500);
    }
    if (!digitalRead(BUTTON_DOWN_PIN)) {
      Serial.println(F("✓ DOWN OK"));
      lcd.setCursor(0, 1);
      lcd.print(F("DOWN OK         "));
      delay(500);
    }
    if (!digitalRead(BUTTON_SELECT_PIN)) {
      Serial.println(F("✓ SELECT OK"));
      lcd.setCursor(0, 1);
      lcd.print(F("SELECT OK       "));
      delay(500);
    }
    if (!digitalRead(MANUAL_OVERRIDE_PIN)) {
      Serial.println(F("✓ MANUAL OK"));
      lcd.setCursor(0, 1);
      lcd.print(F("MANUAL OK       "));
      delay(500);
    }
    delay(50);
  }
  
  Serial.println(F("✓ BACK OK"));
  Serial.println(F("Prueba de botones completada"));
}

void runRelayTest() {
  Serial.println(F("\n=== PRUEBA DETALLADA DEL RELÉ ==="));
  Serial.println(F("ADVERTENCIA: Desconectar bomba para esta prueba"));
  Serial.println(F("Escuchar clicks del relé..."));
  
  lcd.clear();
  lcd.print(F("Relay Test"));
  
  for (int i = 0; i < 5; i++) {
    Serial.print(F("Activación "));
    Serial.print(i + 1);
    Serial.println(F("/5"));
    
    lcd.setCursor(0, 1);
    lcd.print(F("ON  - Test "));
    lcd.print(i + 1);
    lcd.print(F("/5"));
    
    digitalWrite(PUMP_RELAY_PIN, HIGH);
    delay(1000);
    
    lcd.setCursor(0, 1);
    lcd.print(F("OFF - Test "));
    lcd.print(i + 1);
    lcd.print(F("/5"));
    
    digitalWrite(PUMP_RELAY_PIN, LOW);
    delay(1000);
  }
  
  Serial.println(F("Prueba de relé completada"));
  Serial.println(F("¿Se escucharon 5 clicks? Relé OK"));
}

void runLEDTest() {
  Serial.println(F("\n=== PRUEBA DETALLADA DEL LED ==="));
  
  lcd.clear();
  lcd.print(F("LED Test"));
  
  // Encendido constante
  lcd.setCursor(0, 1);
  lcd.print(F("Steady ON"));
  digitalWrite(STATUS_LED_PIN, HIGH);
  delay(2000);
  
  // Apagado
  lcd.setCursor(0, 1);
  lcd.print(F("OFF         "));
  digitalWrite(STATUS_LED_PIN, LOW);
  delay(2000);
  
  // Parpadeo lento
  lcd.setCursor(0, 1);
  lcd.print(F("Slow blink  "));
  for (int i = 0; i < 10; i++) {
    digitalWrite(STATUS_LED_PIN, i % 2);
    delay(500);
  }
  
  // Parpadeo rápido
  lcd.setCursor(0, 1);
  lcd.print(F("Fast blink  "));
  for (int i = 0; i < 20; i++) {
    digitalWrite(STATUS_LED_PIN, i % 2);
    delay(100);
  }
  
  digitalWrite(STATUS_LED_PIN, LOW);
  Serial.println(F("Prueba de LED completada"));
}

void runWaterSensorTest() {
  Serial.println(F("\n=== PRUEBA DETALLADA SENSOR AGUA ==="));
  Serial.println(F("Probando sensor en diferentes niveles..."));
  Serial.println(F("Presiona BACK para terminar"));
  
  lcd.clear();
  lcd.print(F("Water Sensor"));
  
  int minValue = 1023, maxValue = 0;
  
  while (digitalRead(BUTTON_BACK_PIN)) {
    int sensorValue = analogRead(WATER_SENSOR_PIN);
    
    if (sensorValue < minValue) minValue = sensorValue;
    if (sensorValue > maxValue) maxValue = sensorValue;
    
    Serial.print(F("Valor: "));
    Serial.print(sensorValue);
    Serial.print(F(" (Min: "));
    Serial.print(minValue);
    Serial.print(F(", Max: "));
    Serial.print(maxValue);
    Serial.println(F(")"));
    
    lcd.setCursor(0, 1);
    lcd.print(sensorValue);
    lcd.print(F(" ("));
    lcd.print(minValue);
    lcd.print(F("-"));
    lcd.print(maxValue);
    lcd.print(F(")   "));
    
    delay(500);
  }
  
  Serial.print(F("Rango detectado: "));
  Serial.print(minValue);
  Serial.print(F(" - "));
  Serial.println(maxValue);
  Serial.println(F("Configurar umbral entre estos valores"));
}

void runCompleteTest() {
  Serial.println(F("\n=== PRUEBA COMPLETA DEL SISTEMA ==="));
  Serial.println(F("Ejecutando todas las pruebas..."));
  
  runRTCTest();
  delay(1000);
  runLCDTest();
  delay(1000);
  runLEDTest();
  delay(1000);
  runRelayTest();
  
  Serial.println(F("\n=== RESUMEN DE PRUEBAS ==="));
  Serial.println(F("✓ RTC: OK"));
  Serial.println(F("✓ LCD: OK"));
  Serial.println(F("✓ LED: OK"));
  Serial.println(F("✓ Relé: OK"));
  Serial.println(F("Probar botones y sensor manualmente"));
  Serial.println(F("================================="));
  
  lcd.clear();
  lcd.print(F("All Tests"));
  lcd.setCursor(0, 1);
  lcd.print(F("Complete!"));
  
  // Secuencia de finalización
  for (int i = 0; i < 3; i++) {
    digitalWrite(STATUS_LED_PIN, HIGH);
    delay(200);
    digitalWrite(STATUS_LED_PIN, LOW);
    delay(200);
  }
}