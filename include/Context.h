#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RtcDS3231.h>

// Configuración Global
#include "Config.h"

// TUS CLASES
#include "objects/Bomba.h"
#include "objects/BombaConfig.h"
#include "objects/Boton.h"
#include "objects/OLED.h"
#include "objects/Potenciometro.h"
#include "objects/Reloj.h"
#include "menu/MenuBomba.h"
#include "menu/MenuReloj.h"
#include "menu/MenuPrincipal.h"
#include "manager/NetworkManager.h"
#include "manager/ConfigManager.h"
#include "manager/BombaManager.h"

// ==========================================
// DECLARACIÓN EXTERNA (El Catálogo)
// ==========================================
// La palabra 'extern' dice: "Esto existe, búscalo en el .cpp"

extern Adafruit_SSD1306 oledRef;
extern RtcDS3231<TwoWire> Rtc;

extern Bomba bomba;
extern BombaConfig configBomba; 
extern ConfigManager configManager;
extern BombaManager bombaManager; 
extern Boton botonBomba;
extern Boton botonManual;
extern Potenciometro pot;
extern OLED oled; 
extern Reloj reloj;

extern NetworkManager network;

extern MenuBomba menuBomba;
extern MenuReloj menuReloj; 
extern MenuPrincipal menuPrincipal;

// Función maestra para iniciarlo todo
void initSystem();