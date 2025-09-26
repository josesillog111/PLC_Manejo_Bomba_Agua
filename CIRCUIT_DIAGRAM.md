# Diagrama de Circuito - Sistema de Control de Bomba de Agua

## Esquema de Conexiones

```
                        +----------------------------------+
                        |         ARDUINO UNO/NANO        |
                        |                                  |
    RTC DS3231          |  A5 (SCL) ◄──────────────────── SCL
       │                |  A4 (SDA) ◄──────────────────── SDA
       │                |  5V ────────────────────────── VCC
       │                |  GND ───────────────────────── GND
       │                |                                  |
    LCD 16x2            |  D7 ─────────────────────────── A1
       │                |  D12 ────────────────────────── D6
       │                |  D11 ────────────────────────── D5  
       │                |  D10 ────────────────────────── D4
       │                |  D9 ─────────────────────────── E
       │                |  D7 ─────────────────────────── RS
       │                |  5V ─────────────────────────── VCC
       │                |  GND ────────────────────────── GND
       │                |                                  |
    Relé Bomba          |  D8 ─────────────────────────── IN
       │                |  5V ─────────────────────────── VCC
       │                |  GND ────────────────────────── GND
       │                |                                  |
    Botones             |  D2 ─────────────────────────── UP
       │                |  D3 ─────────────────────────── DOWN
       │                |  D4 ─────────────────────────── SELECT
       │                |  D5 ─────────────────────────── BACK
       │                |  D6 ─────────────────────────── MANUAL
       │                |                                  |
    Sensor Agua         |  A0 ─────────────────────────── SIGNAL
       │                |  5V ─────────────────────────── VCC
       │                |  GND ────────────────────────── GND
       │                |                                  |
    LED Estado          |  D13 ────────────────────────── ANODE
       │                |  GND ────────[220Ω]─────────── CATHODE
       │                |                                  |
                        +----------------------------------+
```

## Detalles de Conexión por Componente

### 1. Módulo RTC DS3231

```
DS3231    Arduino
------    -------
VCC   →   5V
GND   →   GND  
SDA   →   A4 (SDA)
SCL   →   A5 (SCL)
```

**Notas:**
- El DS3231 incluye batería de respaldo para mantener la hora
- Usar cables cortos para evitar interferencias
- El módulo funciona con 3.3V o 5V

### 2. Display LCD 16x2

```
LCD       Arduino
-----     -------
VSS   →   GND
VDD   →   5V
V0    →   Potenciómetro 10kΩ (contraste)
RS    →   D7
E     →   D9
D4    →   D10
D5    →   D11
D6    →   D12
D7    →   A1
A     →   5V (con resistencia 220Ω)
K     →   GND
```

**Notas:**
- El potenciómetro de 10kΩ controla el contraste
- La resistencia en el backlight evita sobrecorriente
- Si no hay backlight, conectar A y K según especificaciones

### 3. Módulo Relé

```
Relé      Arduino    Bomba 220V
------    -------    ----------
IN    →   D8
VCC   →   5V
GND   →   GND
COM   →              Fase entrada
NO    →              Fase a bomba
NC    →              (no usado)
```

**⚠️ IMPORTANTE - SEGURIDAD ELÉCTRICA:**
- Usar relé apropiado para la corriente de la bomba
- Instalar fusible en serie con la bomba
- La conexión de 220V debe ser realizada por electricista calificado
- Usar contactores adicionales para bombas de alta potencia
- Implementar protección diferencial

### 4. Botones de Control

```
                   +5V
                    │
                    │
    ┌─────────────┐ │
    │   BUTTON    │ │
    │     UP      │ │ 10kΩ
    └─────────────┘ │
            │       │
            └───────┼─────► D2 (Arduino)
                    │
                   GND
```

**Configuración para todos los botones:**
- Botón UP: D2
- Botón DOWN: D3  
- Botón SELECT: D4
- Botón BACK: D5
- Override MANUAL: D6

**Notas:**
- Usar resistencias pull-up internas del Arduino
- Los botones conectan el pin a GND cuando se presionan
- Añadir capacitor 100nF en paralelo para mejor antirrebote (opcional)

### 5. Sensor de Nivel de Agua (Opcional)

```
Sensor    Arduino
-------   -------
VCC   →   5V
GND   →   GND
SIG   →   A0
```

**Tipos de sensores compatibles:**
- **Sensor capacitivo**: Para detección sin contacto
- **Sensor ultrasónico**: Para medición de distancia al agua
- **Float switch**: Interruptor mecánico simple
- **Sensor resistivo**: Medición por conductividad

### 6. LED Indicador de Estado

```
                 +5V
                  │
                  │
    ┌─────────────┐
    │    220Ω     │
    └─────────────┘
                  │
                  │ ──► D13 (Arduino)
    ┌─────────────┐
    │     LED     │
    │   (ANODE)   │
    └─────────────┘
                  │
                 GND
```

**Patrones de parpadeo:**
- **Apagado**: Sistema en modo manual o error
- **Parpadeo lento (2s)**: Modo automático, bomba apagada  
- **Parpadeo rápido (0.5s)**: Bomba funcionando

## Alimentación del Sistema

### Opción 1: Alimentación USB (Desarrollo/Pruebas)
```
PC/Cargador USB → Arduino → Componentes 5V
```

### Opción 2: Fuente Externa (Instalación Permanente)
```
Fuente 12V → Regulador 7805 → Arduino → Componentes 5V
              │
              └─────────────► Relé (si requiere 12V)
```

**Especificaciones de la fuente:**
- Voltaje: 7-12V DC
- Corriente: Mínimo 1A (recomendado 2A)
- Regulación: Estabilizada
- Protección: Fusible 2A

## Consideraciones de Instalación

### Caja de Protección
- **Grado IP**: IP65 mínimo para exteriores
- **Material**: ABS o policarbonato resistente a UV
- **Ventilación**: Rejillas para evitar condensación
- **Dimensiones**: Suficiente espacio para componentes y cableado

### Cableado
- **Cables de señal**: AWG 22-24, máximo 3 metros
- **Cables de potencia**: AWG 12-14 según corriente de bomba
- **Protección**: Conduit o manguera corrugada
- **Conectores**: Borneras o conectores impermeables

### Puesta a Tierra
- Conectar chassis de la caja a tierra
- Usar cable de tierra independiente para la bomba
- Instalar protección diferencial (30mA)

## Lista de Materiales (BOM)

### Componentes Electrónicos
| Cantidad | Componente | Especificación | Código/Referencia |
|----------|------------|----------------|-------------------|
| 1 | Arduino Uno/Nano | Original o compatible | A000066 |
| 1 | RTC DS3231 | Con batería | DS3231SN |
| 1 | LCD 16x2 | HD44780 compatible | HD44780 |
| 1 | Relé 5V | 10A, SPDT | SRD-05VDC-SL-C |
| 4 | Botón pulsador | 6x6mm, tactil | B3F-4055 |
| 1 | Interruptor | SPST, palanca | MTS-101 |
| 1 | LED | 5mm, rojo | LED-5MM-RED |
| 1 | Resistencia | 220Ω, 1/4W | CF14JT220R |
| 1 | Potenciómetro | 10kΩ, lineal | 3386P-103 |

### Materiales Adicionales
| Cantidad | Material | Especificación |
|----------|-----------|----------------|
| 1 | Protoboard/PCB | Para conexiones |
| 1 | Caja protección | IP65, 200x150x100mm |
| 20m | Cable multipar | AWG 22, 8 conductores |
| 1 | Fuente alimentación | 12V, 2A |
| 1 | Fusible | 2A, tipo F |
| 1 | Portafusible | Para riel DIN |

## Diagrama de Flujo del Sistema

```
        ┌─────────────┐
        │   INICIO    │
        └──────┬──────┘
               │
        ┌──────▼──────┐
        │ Inicializar │
        │ Hardware    │
        └──────┬──────┘
               │
        ┌──────▼──────┐
        │ Cargar      │
        │ Configuración│
        └──────┬──────┘
               │
        ┌──────▼──────┐     NO   ┌──────────────┐
        │ Modo        │◄─────────┤ Override     │
        │ Automático? │          │ Manual?      │
        └──────┬──────┘          └──────────────┘
               │ SI
        ┌──────▼──────┐
        │ Verificar   │
        │ Condiciones │
        └──────┬──────┘
               │
        ┌──────▼──────┐     SI   ┌──────────────┐
        │ Activar     │◄─────────┤ Condiciones  │
        │ Bomba?      │          │ Cumplidas?   │
        └──────┬──────┘          └──────────────┘
               │ NO
        ┌──────▼──────┐
        │ Actualizar  │
        │ Display     │
        └──────┬──────┘
               │
        ┌──────▼──────┐
        │ Procesar    │
        │ Botones     │
        └──────┬──────┘
               │
               └──────┘ (Loop)
```

## Pruebas y Validación

### Test de Componentes Individuales
1. **RTC**: Verificar fecha/hora correcta
2. **LCD**: Mostrar texto en ambas líneas
3. **Botones**: Respuesta correcta de cada botón
4. **Relé**: Activación/desactivación audible
5. **LED**: Patrones de parpadeo correctos

### Test de Sistema Integrado
1. **Configuración**: Navegación completa por menús
2. **Modo Intervalo**: Activación según tiempo configurado
3. **Modo Días**: Activación en días/horarios específicos
4. **Override Manual**: Funcionamiento independiente
5. **Protecciones**: Sensor de agua y timeouts

### Herramientas de Diagnóstico
- **Monitor Serial**: Para logs y debugging
- **Multímetro**: Verificar voltajes y continuidad
- **Osciloscopio**: Analizar señales (opcional)

---

**Nota Final**: Este diagrama es una guía general. Verificar siempre las especificaciones de los componentes específicos utilizados y consultar con un electricista calificado para instalaciones que involucren voltajes peligrosos.