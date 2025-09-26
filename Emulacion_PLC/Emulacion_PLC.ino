#include <LiquidCrystal_I2C.h>
#include <RtcDS1302.h>

// --- Pines ---
// DS1302 CLK/SCLK --> 5
// DS1302 DAT/IO   --> 4
// DS1302 RST/CE   --> 2
// DS1302 VCC      --> 3.3v - 5v
// DS1302 GND      --> GND

#define POT_PIN A0   // Potenciómetro
#define BTN_PIN 12    // Botón
#define RELE_PIN 8   // Relé para bomba


// --- Variables de menú ---
const int totalMenus = 3;

// --- Configuraciones ---
int brillo = 100;
int horaOn = 6, minOn = 0;
int horaOff = 18, minOff = 0;
 
LiquidCrystal_I2C lcd(0x27, 16, 2);
ThreeWire myWire(4,5,2); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

// --- BOTÓN ---
unsigned long pressStart = 0;
bool pressActive = false;
unsigned long lastBounce = 0;
int buttonEvent = 0; // 0 = sin evento, 1 = corta, 2 = larga

const unsigned long DEBOUNCE_MS = 30;   // anti-rebote
const unsigned long SHORT_MS    = 500;  // 400 ms -> pulsación corta
const unsigned long LONG_MS     = 1500; // 1500 ms -> pulsación larga

int readButtonEvent() {
    int reading = digitalRead(BTN_PIN);
    static int stableState = HIGH; // estado estable actual del pin

    // Cambio de estado detectado -> aplicar debounce
    if (reading != stableState) {
        if (millis() - lastBounce > DEBOUNCE_MS) {
            stableState = reading;
            lastBounce = millis();

            if (stableState == LOW) {
                // Se ha PRESIONADO
                pressStart = millis();
                pressActive = true;
            } else {
                // Se ha SOLTADO
                if (pressActive) {
                    unsigned long dur = millis() - pressStart;
                    pressActive = false;

                    // Clasificar según duracion
                    if (dur >= LONG_MS) {
                        buttonEvent = 2; // larga
                    } else {
                        // dur < LONG_MS -> considerar corta (incluye <= SHORT_MS y mid)
                        // Si quieres distinguir mid-range, cambia aquí.
                        buttonEvent = 1; // corta
                    }

                    // debug opcional
                    // Serial.print("dur(ms): "); Serial.println(dur);
                }
            }
        }
    }

    if (buttonEvent != 0) {
        int ev = buttonEvent;
        buttonEvent = 0; // consumir el evento (solo se devuelve 1 vez)
        return ev;
    }
    return 0; // sin evento
}

// --- SETUP ---
void setup () {
    Serial.begin(57600);
    pinMode(BTN_PIN, INPUT_PULLUP);
    pinMode(RELE_PIN, OUTPUT);
    digitalWrite(RELE_PIN, LOW);

    lcd.init();
    lcd.backlight();

    lcd.setCursor(0,0);
    lcd.print("Iniciando Reloj...");
    delay(2000);
    
    Rtc.Begin();

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    if (!Rtc.IsDateTimeValid()) Rtc.SetDateTime(compiled);
    if (Rtc.GetIsWriteProtected()) Rtc.SetIsWriteProtected(false);
    if (!Rtc.GetIsRunning()) Rtc.SetIsRunning(true);

    RtcDateTime now = Rtc.GetDateTime();
    if (now < compiled) Rtc.SetDateTime(compiled);

    lcd.clear();
    lcd.setCursor(0,0); lcd.print("Mostrando hora");
    delay(2000);
}

// --- LOOP ---
#define LCD_TIMEOUT   10000   // 10 segundos sin actividad -> apaga LCD
#define TIMEOUT_MENU  7000   // 7 segundos sin actividad en menú -> salir

unsigned long lcdLastOn = 0;   // último momento con pantalla encendida
unsigned long lastAction = 0;  // última acción del usuario
bool lcdEncendido = false;
bool inMenu = false;
int menuIndex = 0;
bool desactivarHoy = false; // Flag para desactivar bomba hoy

// ----------------- LOOP -----------------
void loop() {
    int btn = readButtonEvent();
    static int lastPotVal = 0;
    int rawPot = analogRead(POT_PIN);
    int potVal = rawPot / 8;  // reducir resolución 0–127 para menos ruido
    unsigned long ahora = millis();

    bool actividad = false;

    // --- Detectar actividad (botón o potenciómetro) ---
    if (btn > 0 || abs(potVal - lastPotVal) >= 5) { // ignora ruido ±1
        Serial.println("activo");
        actividad = true;
        lastAction = ahora;
        lcdLastOn = ahora;
        if (!lcdEncendido) {
            lcdEncendido = true;
            lcd.backlight();
        }
        if (!inMenu) {
            mostrarHora(Rtc.GetDateTime()); // refrescar cuando "despierta"
        }
    }
    lastPotVal = potVal;

    // --- Apagar LCD por inactividad ---
    if (!inMenu && lcdEncendido && (ahora - lcdLastOn > LCD_TIMEOUT)) {
        Serial.println("inactivo");
        lcdEncendido = false;
        lcd.noBacklight();
        lcd.clear();
    }

    RtcDateTime now = Rtc.GetDateTime();

    // ============================================================
    //                 MODO RELOJ / ESTADO NORMAL
    // ============================================================
    if (!inMenu) {
        if (lcdEncendido) {
            mostrarHora(now);  // refresca cada segundo

            if (btn == 1) { // clic corto -> mostrar config bomba
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("Bomba ON: ");
                if(horaOn < 10) lcd.print("0"); lcd.print(horaOn);
                lcd.print(":");
                if(minOn < 10) lcd.print("0"); lcd.print(minOn);

                lcd.setCursor(0,1);
                lcd.print("OFF: ");
                if(horaOff < 10) lcd.print("0"); lcd.print(horaOff);
                lcd.print(":");
                if(minOff < 10) lcd.print("0"); lcd.print(minOff);
                delay(1200);
            }

            if (btn == 2) { // clic largo -> entrar a menú
                inMenu = true;
                menuIndex = 0;
                lastAction = ahora;
                mostrarMenu(menuIndex);
            }
        }

        // --- Control bomba ---
        int actualMin = now.Hour() * 60 + now.Minute();
        int onMin = horaOn * 60 + minOn;
        int offMin = horaOff * 60 + minOff;

        if (!desactivarHoy && actualMin >= onMin && actualMin < offMin) {
            digitalWrite(RELE_PIN, HIGH);
        } else {
            digitalWrite(RELE_PIN, LOW);
        }
    }

    // ============================================================
    //                        MODO MENÚ
    // ============================================================
    else {
        if (btn == 1) {
            lastAction = ahora;
            menuIndex = (menuIndex + 1) % totalMenus;
            mostrarMenu(menuIndex);
        } else if (btn == 2) {
            lastAction = ahora;
            switch(menuIndex) {
                case 0: configurarHoraRTC(); break;
                case 1: configurarBomba(); break;
                case 2: configurarBrillo(); break;
            }
            inMenu = false;
            lcdLastOn = ahora; 
            mostrarHora(now);
        }

        // --- Timeout del menú ---
        if ((ahora - lastAction > TIMEOUT_MENU)) {
            inMenu = false;
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Saliendo...");
            delay(500);
            mostrarHora(now);
            lcdLastOn = ahora;
        }
    }
}

// --- Mostrar hora ---
void mostrarHora(RtcDateTime now) {
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate < 1000) return; // solo refrescar cada 1s
    lastUpdate = millis();

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(now.Day()); lcd.print("/");
    lcd.print(now.Month()); lcd.print("/");
    lcd.print(now.Year());

    lcd.setCursor(0,1);
    if (now.Hour() < 10) lcd.print("0"); lcd.print(now.Hour());
    lcd.print(":");
    if (now.Minute() < 10) lcd.print("0"); lcd.print(now.Minute());
    lcd.print(":");
    if (now.Second() < 10) lcd.print("0"); lcd.print(now.Second());
}


// --- Mostrar menú ---
void mostrarMenu(int index) {
    lcd.clear();
    lcd.setCursor(0,0); lcd.print("Menu:");
    lcd.setCursor(0,1);

    switch(index) {
        case 0: lcd.print("> Config Hora"); break;
        case 1: lcd.print("> Config Bomba"); break;
        case 2: lcd.print("> Config Brillo"); break;
    }
}

// --- Configurar Brillo ---
void configurarBrillo() {
    int pot = analogRead(POT_PIN);
    brillo = map(pot, 0, 1023, 0, 255);

    lcd.clear();
    lcd.setCursor(0,0); lcd.print("Config Brillo");
    lcd.setCursor(0,1); lcd.print("Nivel: "); lcd.print(brillo);

    if (brillo < 128) lcd.noBacklight();
    else lcd.backlight();

    delay(500);
}

// --- Configurar Bomba ---
void configurarBomba() {
    enum SubMenu { HORA, DIAS, DESACTIVAR, SALIR };
    SubMenu subMenuIndex = HORA;

    bool salir = false;
    while (!salir) {
        // Leer potenciómetro y mapear a submenú
        int potVal = analogRead(POT_PIN);
        subMenuIndex = static_cast<SubMenu>(map(potVal, 0, 1023, 0, 3));

        // Mostrar menú
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Config Bomba:");
        lcd.setCursor(0,1);
        switch(subMenuIndex) {
            case HORA: lcd.print("> Hora ON/OFF"); break;
            case DIAS: lcd.print("> Dias / Intervalo"); break;
            case DESACTIVAR: lcd.print("> Desactivar Hoy"); break;
            case SALIR: lcd.print("> Salir"); break;
        }

        int btn = readButtonEvent();
        if (btn == 1) { // pulsación corta = seleccionar
            switch(subMenuIndex) {
                case HORA:
                    configurarHoraBomba();
                    break;
                case DIAS:
                    configurarDiasBomba();
                    break;
                case DESACTIVAR:
                    bombaDesactivarHoy();
                    break;
                case SALIR:
                    salir = true;
                    break;
            }
        }

        delay(150); // pequeño retardo para evitar parpadeos
    }

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Regresando...");
    delay(1000);
}

// ------------------ Submenú HORA ------------------
void configurarHoraBomba() {
    int lastVal = -1;

    auto leerPot = [](int minVal, int maxVal) -> int {
        int val = map(analogRead(POT_PIN), 0, 1023, minVal, maxVal + 1);
        if (val > maxVal) val = maxVal;
        return val;
    };

    // --- Hora ON ---
    while (true) {
        int nuevaHora = leerPot(0, 23);
        if (abs(nuevaHora - lastVal) > 0) {
            lcd.clear();
            lcd.setCursor(0,0); lcd.print("Hora Encendido");
            lcd.setCursor(0,1); lcd.print("Hora: "); if(nuevaHora<10) lcd.print("0"); lcd.print(nuevaHora);
            lastVal = nuevaHora;
        }
        int btn = readButtonEvent();
        if (btn == 2) { horaOn = nuevaHora; break; } // pulsación larga para guardar
        delay(50);
    }

    // --- Minuto ON ---
    lastVal = -1;
    while (true) {
        int nuevoMin = leerPot(0, 59);
        if (abs(nuevoMin - lastVal) > 0) {
            lcd.clear();
            lcd.setCursor(0,0); lcd.print("Min Encendido");
            lcd.setCursor(0,1); lcd.print("Min: "); if(nuevoMin<10) lcd.print("0"); lcd.print(nuevoMin);
            lastVal = nuevoMin;
        }
        int btn = readButtonEvent();
        if (btn == 2) { minOn = nuevoMin; break; }
        delay(50);
    }

    // --- Hora OFF ---
    lastVal = -1;
    while (true) {
        int nuevaHora = leerPot(0, 23);
        if (abs(nuevaHora - lastVal) > 0) {
            lcd.clear();
            lcd.setCursor(0,0); lcd.print("Hora Apagado");
            lcd.setCursor(0,1); lcd.print("Hora: "); if(nuevaHora<10) lcd.print("0"); lcd.print(nuevaHora);
            lastVal = nuevaHora;
        }
        int btn = readButtonEvent();
        if (btn == 2) { horaOff = nuevaHora; break; }
        delay(50);
    }

    // --- Minuto OFF ---
    lastVal = -1;
    while (true) {
        int nuevoMin = leerPot(0, 59);
        if (abs(nuevoMin - lastVal) > 0) {
            lcd.clear();
            lcd.setCursor(0,0); lcd.print("Min Apagado");
            lcd.setCursor(0,1); lcd.print("Min: "); if(nuevoMin<10) lcd.print("0"); lcd.print(nuevoMin);
            lastVal = nuevoMin;
        }
        int btn = readButtonEvent();
        if (btn == 2) { minOff = nuevoMin; break; }
        delay(50);
    }

    // --- Confirmación ---
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Bomba ON "); if(horaOn<10) lcd.print("0"); lcd.print(horaOn); lcd.print(":"); if(minOn<10) lcd.print("0"); lcd.print(minOn);
    lcd.setCursor(0,1);
    lcd.print("OFF "); if(horaOff<10) lcd.print("0"); lcd.print(horaOff); lcd.print(":"); if(minOff<10) lcd.print("0"); lcd.print(minOff);
    delay(2000);
}

// ------------------ Submenú DÍAS/INTERVALO ------------------
void configurarDiasBomba() {
    int lastVal = -1;
    static int diasSemana = 0b0111110; // bitmask: lun a dom

    while (true) {
        int val = map(analogRead(POT_PIN), 0, 1023, 0, 7); // 0: todos, 1-7 días
        if (val != lastVal) {
            lcd.clear();
            lcd.setCursor(0,0); lcd.print("Dias/Intervalo");
            lcd.setCursor(0,1);
            if(val==0) lcd.print("Todos los dias");
            else {
                lcd.print("Dia: ");
                switch(val) {
                    case 1: lcd.print("Lun"); break;
                    case 2: lcd.print("Mar"); break;
                    case 3: lcd.print("Mie"); break;
                    case 4: lcd.print("Jue"); break;
                    case 5: lcd.print("Vie"); break;
                    case 6: lcd.print("Sab"); break;
                    case 7: lcd.print("Dom"); break;
                }
            }
            lastVal = val;
        }

        int btn = readButtonEvent();
        if (btn == 2) { 
            // Guardar selección
            if(val==0) diasSemana = 0b0111111; // todos
            else diasSemana = 1 << (val-1);    // bitmask día
            lcd.clear();
            lcd.setCursor(0,0); lcd.print("Guardado!");
            delay(1000);
            break; 
        }
        delay(50);
    }
}

// ------------------ Submenú DESACTIVAR HOY ------------------
void bombaDesactivarHoy() {
    desactivarHoy = true;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Bomba desactivada");
    lcd.setCursor(0,1);
    lcd.print("Solo hoy");
    delay(1500);
}

// --- Configurar Hora RTC ---
void configurarHoraRTC() {
    int submenuIndex = 0;          // 0 = Hora, 1 = Fecha
    const int totalSubmenus = 2;
    bool seleccion = false;
    unsigned long ultimaAccion = millis();   // para timeout
    const unsigned long TIMEOUT = 15000;     // 15 segundos

    int lastSubmenuIndex = -1;  // <-- para evitar redibujar de más

    while (!seleccion) {
        unsigned long ahora = millis();
        // Timeout: volver al reloj si no hay interacción
        if (ahora - ultimaAccion > TIMEOUT) {
            break;
        }

        // Solo refrescar si cambió la opción
        if (submenuIndex != lastSubmenuIndex) {
            lcd.clear();
            lcd.setCursor(0,0); lcd.print("Configurar Reloj");
            lcd.setCursor(0,1);
            switch (submenuIndex) {
                case 0: lcd.print("> Hora"); break;
                case 1: lcd.print("> Fecha"); break;
            }
            lastSubmenuIndex = submenuIndex;
        }

        int btn = readButtonEvent();
        if (btn != 0) ultimaAccion = ahora; // actualizar último evento

        if (btn == 1) { // clic corto → siguiente opción
            submenuIndex = (submenuIndex + 1) % totalSubmenus;
        } else if (btn == 2) { // clic largo → seleccionar
            seleccion = true;
        }

        delay(50); // pequeño retardo para evitar rebotes visuales
    }

    // Abrir la opción seleccionada (con espera de liberación del botón)
    while (readButtonEvent() != 0) {
        delay(10); // esperar a que se libere antes de entrar
    }

    if (submenuIndex == 0) {
        configurarHora();   // tu función para ajustar hora
    } else if (submenuIndex == 1) {
        configurarFecha();  // tu función para ajustar fecha
    }
}


// ------------------------------
void configurarHora() {
    while (readButtonEvent() != 0) delay(10);

    int lastVal = -1;
    RtcDateTime now = Rtc.GetDateTime();
    int newHour = now.Hour(), newMinute = now.Minute(), newSecond = now.Second();
    unsigned long ultimaAccion = millis();

    enum Paso { HORA, MINUTO, SEGUNDO };
    Paso pasoActual = HORA;

    while (true) {
        unsigned long ahora = millis();
        if (ahora - ultimaAccion > 15000) return;

        int val;
        switch(pasoActual) {
            case HORA:    val = map(analogRead(POT_PIN), 0, 1023, 0, 23); break;
            case MINUTO:  val = map(analogRead(POT_PIN), 0, 1023, 0, 59); break;
            case SEGUNDO: val = map(analogRead(POT_PIN), 0, 1023, 0, 59); break;
        }

        if (val != lastVal) {
            ultimaAccion = ahora;   // ⚡ actividad del potenciómetro
            lcd.clear();
            lcd.setCursor(0,0);
            switch(pasoActual) {
                case HORA:    lcd.print("Config HORA"); break;
                case MINUTO:  lcd.print("Config MIN"); break;
                case SEGUNDO: lcd.print("Config SEG"); break;
            }
            lcd.setCursor(0,1);
            if (val < 10) lcd.print("0");
            lcd.print(val);
            lastVal = val;
        }

        int btn = readButtonEvent();
        if (btn != 0) ultimaAccion = ahora;

        if (btn == 1) {
            switch(pasoActual) {
                case HORA:    newHour   = val; pasoActual = MINUTO; lastVal = -1; break;
                case MINUTO:  newMinute = val; pasoActual = SEGUNDO; lastVal = -1; break;
                case SEGUNDO: newSecond = val;
                    RtcDateTime newDT(now.Year(), now.Month(), now.Day(), newHour, newMinute, newSecond);
                    Rtc.SetDateTime(newDT);
                    lcd.clear(); lcd.print("Hora actualizada");
                    delay(1500);
                    return;
            }
        } else if (btn == 2) return;

        delay(50);
    }
}


// ------------------------------
void configurarFecha() {
    while (readButtonEvent() != 0) delay(10);

    int lastVal = -1;
    RtcDateTime now = Rtc.GetDateTime();
    int newDay = now.Day(), newMonth = now.Month(), newYear = now.Year();
    unsigned long ultimaAccion = millis();

    enum Paso { DIA, MES, ANO };
    Paso pasoActual = DIA;

    while (true) {
        unsigned long ahora = millis();
        if (ahora - ultimaAccion > 15000) return;

        int val;
        switch(pasoActual) {
            case DIA: val = map(analogRead(POT_PIN), 0, 1023, 1, 31); break;
            case MES: val = map(analogRead(POT_PIN), 0, 1023, 1, 12); break;
            case ANO: val = map(analogRead(POT_PIN), 0, 1023, 2025, 2035); break;
        }

        if (val != lastVal) {
            ultimaAccion = ahora;   // actividad del potenciómetro
            lcd.clear();
            lcd.setCursor(0,0);
            switch(pasoActual) {
                case DIA: lcd.print("Config DIA"); lcd.setCursor(0,1); lcd.print("Dia: "); break;
                case MES: lcd.print("Config MES"); lcd.setCursor(0,1); lcd.print("Mes: "); break;
                case ANO: lcd.print("Config ANO"); lcd.setCursor(0,1); lcd.print("Ano: "); break;
            }
            if (val < 10 && pasoActual != ANO) lcd.print("0");
            lcd.print(val);
            lastVal = val;
        }

        int btn = readButtonEvent();
        if (btn != 0) ultimaAccion = ahora;

        if (btn == 1) {
            switch(pasoActual) {
                case DIA: newDay = val; pasoActual = MES; lastVal = -1; break;
                case MES: newMonth = val; pasoActual = ANO; lastVal = -1; break;
                case ANO: newYear = val;
                    RtcDateTime newDT(newYear, newMonth, newDay, now.Hour(), now.Minute(), now.Second());
                    Rtc.SetDateTime(newDT);
                    lcd.clear(); lcd.print("Fecha actualizada");
                    delay(1500);
                    return;
            }
        } else if (btn == 2) return;

        delay(50);
    }
}
