#include "../include/Context.h"

// ==========================================
// INSTANCIACIÓN REAL (La Fábrica)
// ==========================================

Adafruit_SSD1306 oledRef(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);
RtcDS3231<TwoWire> Rtc(Wire);

Bomba bomba(PIN_BOMBA);
BombaConfig configBomba; 
Boton botonManual(PIN_BOTON_MANUAL);
ConfigManager configManager(configBomba);
BombaManager bombaManager(bomba, configBomba, Rtc, botonManual); 
Boton botonBomba(PIN_BOTON_BOMBA);
Potenciometro pot(PIN_POT);
OLED oled(oledRef, 7000); 
Reloj reloj(Rtc, oled);

// Pasamos 'oled' al NetworkManager
NetworkManager network(oled, configManager);

MenuBomba menuBomba(oled, botonBomba, pot, configManager);
MenuReloj menuReloj(oled, botonBomba, pot, reloj); 
MenuPrincipal menuPrincipal(oled, botonBomba, menuBomba, menuReloj);

// ==========================================
// FUNCIÓN DE INICIALIZACIÓN
// ==========================================
void initSystem() {
    Serial.begin(115200);

    // 1. Hardware Básico
    bomba.iniciar();
    Wire.begin(PIN_SDA, PIN_SCL); 
    
    if(!oledRef.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) { 
        Serial.println(F("Fallo OLED"));
    }
    
    pot.iniciar();
    botonBomba.iniciar();
    botonManual.iniciar();
    Rtc.Begin();
    configManager.iniciar();
    analogReadResolution(10); 

    // 2. Iniciar Red (WiFiManager + MQTT)
    network.iniciar();

    // 3. Configurar RTC
    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    if (!Rtc.IsDateTimeValid()) Rtc.SetDateTime(compiled);
    if (!Rtc.GetIsRunning()) Rtc.SetIsRunning(true);
}