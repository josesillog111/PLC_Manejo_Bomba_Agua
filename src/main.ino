#include <Arduino.h>
#include "../include/Context.h" // Aquí viven todas tus instancias (bombaManager, network, etc.)

// ==========================================
// VARIABLES LOCALES DE INTERFAZ
// ==========================================
// Solo dejamos aquí lo estrictamente necesario para la UI del Loop
unsigned long lastMsgMQTT = 0;
const int UMBRAL_POT = 5;
int lastPotVal = -1;

// Variables para gestión de pantalla OLED
unsigned long ultimaInteraccion = 0;
unsigned long ultimaActualizacionReloj = 0;
const unsigned long TIEMPO_ENCENDIDO_PANTALLA = 10000; // 10 segundos

// ==========================================
// SETUP
// ==========================================
void setup() {
    initSystem(); 
}

// ==========================================
// LOOP
// ==========================================
void loop() {
    unsigned long ahora = millis();

    // 1. MANTENIMIENTO DE RED (WiFi & MQTT)
    // Se encarga de reconectar y procesar mensajes entrantes ("ON", "OFF")
    network.update();

    // 2. LECTURA DE INTERFAZ HUMANA (Menú y Potenciómetro)
    // Nota: El botón de la BOMBA (manual) se lee dentro de bombaManager.Evaluar()
    int btnMenu = botonBomba.leerEvento(); // Botón del Menú (Pin 16)
    
    int rawPot = pot.leer();       
    int potVal = rawPot / 128; // Escala 0-8 para menús

    // 3. DETECTAR ACTIVIDAD (Para encender pantalla)
    if ((btnMenu != 0) || abs(potVal - lastPotVal) > UMBRAL_POT) {
        oled.encender();
        ultimaInteraccion = ahora; 
        
        // Si pulsó el botón de Menú (Click Largo = 2)
        if (btnMenu == 2) { 
            menuPrincipal.mostrarMenu();
            oled.limpiar();
            ultimaInteraccion = ahora; // Renovar tiempo de pantalla
        }
    }
    lastPotVal = potVal;

    // 4. GESTIÓN VISUAL (Qué mostrar en la OLED)
    if (ahora - ultimaInteraccion < TIEMPO_ENCENDIDO_PANTALLA) {
        // Refrescar cada 1 segundo para no parpadear demasiado
        if (ahora - ultimaActualizacionReloj >= 1000) { 
            ultimaActualizacionReloj = ahora;
            
            // Si no estás dentro del menú (asumimos que menuPrincipal bloquea si está activo, 
            // o si es simple, mostramos estado base):
            
            oled.limpiar();
            
            // Mitad del tiempo mostramos Estado, mitad Reloj (o ambos si caben)
            if ((ahora - ultimaInteraccion) < TIEMPO_ENCENDIDO_PANTALLA / 2) {
                // FASE 1: ESTADO BOMBA
                bool hardwareOn = bomba.estaEncendida(); 
                String estadoTxt = hardwareOn ? "Bomba: ON" : "Bomba: OFF";
                oled.mostrar(estadoTxt.c_str(), 0, 0);
                // Mostrar estado Cloud
                if(network.isConnected()) oled.mostrar("Cloud: OK", 0, 1);
                else oled.mostrar("Cloud: ...", 0, 1);
            } else {
                // FASE 2: HORA
                reloj.mostrarHora();
            }
        }
    } else {
        oled.apagar(); // Ahorro de energía
    }

    // 5. CEREBRO DE RIEGO (Lógica + Botón Manual)
    // Evalúa horarios Y lee el botón físico de la bomba (Pin 17)
    bombaManager.Evaluar(Rtc.GetDateTime());

    // 6. REPORTE DE ESTADO MQTT (Solo si cambia)
    static bool ultimoEstadoReportado = false; 
    bool estadoRealBomba = bomba.estaEncendida(); // O bomba.estaEncendida()

    if (estadoRealBomba != ultimoEstadoReportado) {
        // Hubo cambio (ON->OFF o OFF->ON)
        network.publishStatus(estadoRealBomba);
        ultimoEstadoReportado = estadoRealBomba;
        
        Serial.print("Cambio estado -> MQTT: ");
        Serial.println(estadoRealBomba ? "ON" : "OFF");
        
        // Forzamos encender pantalla para que el usuario vea que pasó algo
        oled.encender();
        ultimaInteraccion = ahora;
    }

    // Pequeño respiro para estabilidad
    delay(10); 
}