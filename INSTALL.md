# Guía de Instalación - Sistema de Control de Bomba de Agua

## Instalación Paso a Paso

### 1. Preparación del Entorno de Desarrollo

#### 1.1 Instalar Arduino IDE
1. Descargar desde [arduino.cc](https://www.arduino.cc/en/software)
2. Instalar siguiendo las instrucciones del sistema operativo
3. Conectar Arduino y verificar que se detecte correctamente

#### 1.2 Instalar Librerías Necesarias
Ir a **Herramientas** → **Gestor de Librerías** e instalar:

- **RTClib** by Adafruit (versión 2.1.1 o superior)
  ```
  Autor: Adafruit
  Descripción: Real time clock library
  ```

- **LiquidCrystal** (incluida por defecto en Arduino IDE)
  ```
  Ubicación: Sketch → Incluir Librería → LiquidCrystal
  ```

### 2. Preparación del Hardware

#### 2.1 Lista de Componentes
- [ ] Arduino Uno o Nano
- [ ] Módulo RTC DS3231 con batería
- [ ] Display LCD 16x2 (HD44780)
- [ ] Módulo relé 5V/10A
- [ ] 4 botones pulsadores
- [ ] 1 interruptor para override manual
- [ ] 1 LED 5mm + resistencia 220Ω
- [ ] 1 potenciómetro 10kΩ (para contraste LCD)
- [ ] Protoboard o PCB para conexiones
- [ ] Cables jumper
- [ ] Caja de protección IP65

#### 2.2 Ensamblaje del Circuito
Seguir el diagrama en `CIRCUIT_DIAGRAM.md`:

1. **Conexiones de alimentación**
   ```
   Arduino 5V  → VCC de todos los módulos
   Arduino GND → GND de todos los módulos
   ```

2. **Conexiones I2C (RTC)**
   ```
   Arduino A4 → SDA del DS3231
   Arduino A5 → SCL del DS3231
   ```

3. **Conexiones LCD**
   ```
   Arduino D7  → RS del LCD
   Arduino D9  → E del LCD
   Arduino D10 → D4 del LCD
   Arduino D11 → D5 del LCD
   Arduino D12 → D6 del LCD
   Arduino A1  → D7 del LCD
   ```

4. **Conexiones de control**
   ```
   Arduino D8  → IN del relé
   Arduino D2  → Botón UP
   Arduino D3  → Botón DOWN
   Arduino D4  → Botón SELECT
   Arduino D5  → Botón BACK
   Arduino D6  → Interruptor MANUAL
   Arduino D13 → LED de estado (con resistencia)
   Arduino A0  → Sensor de agua (opcional)
   ```

### 3. Instalación del Software

#### 3.1 Descarga del Código
```bash
# Opción 1: Clonar repositorio
git clone https://github.com/josesillog111/PLC_Manejo_Bomba_Agua.git

# Opción 2: Descargar ZIP desde GitHub
```

#### 3.2 Carga del Código de Prueba
1. Abrir `test_components.ino` en Arduino IDE
2. Seleccionar placa: **Herramientas** → **Placa** → **Arduino Uno**
3. Seleccionar puerto: **Herramientas** → **Puerto** → [Puerto correspondiente]
4. Compilar: **Sketch** → **Verificar/Compilar**
5. Cargar: **Sketch** → **Subir**

#### 3.3 Verificación de Componentes
1. Abrir monitor serial (9600 baud)
2. Probar cada componente usando los menús del sketch de prueba
3. Verificar que todos los componentes respondan correctamente

#### 3.4 Carga del Programa Principal
1. Una vez verificados todos los componentes, abrir `PLC_Manejo_Bomba_Agua.ino`
2. Compilar y cargar siguiendo los mismos pasos anteriores
3. Verificar que el sistema inicie correctamente

### 4. Configuración Inicial

#### 4.1 Primera Configuración
1. **Configurar fecha/hora:**
   - Navegar a: Menú Principal → Configuración → Ajustar Hora
   - Configurar año, mes, día, hora y minutos actuales

2. **Seleccionar modo de operación:**
   - Configuración → Modo → Seleccionar entre:
     - **Intervalo**: Para funcionamiento por tiempo
     - **Días**: Para funcionamiento en días específicos
     - **Manual**: Solo control manual

3. **Configurar parámetros según el modo:**

   **Para Modo Intervalo:**
   - Configuración → Config Intervalo
   - Cada X minutos: Intervalo entre activaciones
   - Duración Y minutos: Tiempo de funcionamiento

   **Para Modo Días:**
   - Configuración → Config Días
   - Seleccionar días activos (L-M-X-J-V-S-D)
   - Configurar hora de inicio y fin

#### 4.2 Configuración del Sensor de Agua (Opcional)
1. Si se instaló sensor de agua:
   - Configuración → Sensor → ON
   - Ir a modo prueba para calibrar el umbral
   - Anotar valores con agua/sin agua
   - Configurar umbral intermedio

### 5. Instalación Eléctrica

⚠️ **IMPORTANTE**: Esta sección debe ser realizada por un electricista calificado.

#### 5.1 Preparación
- Desconectar alimentación general
- Verificar ausencia de tensión con multímetro
- Usar herramientas aisladas
- Trabajar con luz natural o iluminación auxiliar

#### 5.2 Conexión del Relé a la Bomba
```
Instalación típica:
                                    
Tablero Principal    Relé Arduino      Bomba
     │                    │              │
   Fase ──────────────── COM          │
     │                    │              │
   Neutro ─────────────────────────── Neutro
     │                    │              │
   Tierra ─────────────────────────── Tierra
                          │
                        NO ────────── Fase
```

#### 5.3 Protecciones Eléctricas
- Instalar interruptor termomagnético (según potencia de bomba)
- Instalar protección diferencial (30mA)
- Fusible en línea del relé
- Conexión a tierra independiente

#### 5.4 Caja de Protección
- Instalar todos los componentes electrónicos en caja IP65
- Separar sección de control (5V) de potencia (220V)
- Usar prensacables para entrada/salida
- Instalar desiccante para evitar humedad

### 6. Puesta en Marcha

#### 6.1 Verificaciones Previas
- [ ] Todas las conexiones verificadas
- [ ] Protecciones eléctricas instaladas
- [ ] Sistema configurado correctamente
- [ ] Bomba desconectada para pruebas iniciales

#### 6.2 Prueba del Sistema
1. **Conectar alimentación del Arduino**
2. **Verificar funcionamiento del relé:**
   - Configurar intervalo corto (5 minutos)
   - Escuchar clicks del relé según programación
3. **Conectar bomba y probar:**
   - Verificar que la bomba arranca cuando el relé se activa
   - Comprobar que para correctamente

#### 6.3 Ajuste Final
- Configurar intervalos/horarios definitivos
- Habilitar sensor de agua si aplica
- Documentar configuración final
- Capacitar al usuario en el uso del sistema

### 7. Mantenimiento Post-Instalación

#### Primera Semana
- Verificar funcionamiento diario
- Revisar logs por monitor serial
- Ajustar configuración según necesidades

#### Primer Mes
- Verificar precisión del RTC
- Comprobar desgaste de contactos del relé
- Revisar que no haya infiltraciones de agua

#### Rutina Mensual
- Limpieza general del sistema
- Verificación de conexiones
- Backup de configuración

### 8. Documentación Final

#### 8.1 Registro de Instalación
Completar la siguiente información:

```
=== REGISTRO DE INSTALACIÓN ===
Fecha de instalación: _______________
Técnico instalador: _________________
Ubicación: _________________________

CONFIGURACIÓN:
Modo de operación: _________________
Intervalo/Horarios: ________________
Sensor de agua: ___________________
Potencia de bomba: ________________

COMPONENTES:
Arduino: __________________________
RTC: ______________________________
Relé: _____________________________
Bomba: ____________________________

OBSERVACIONES:
___________________________________
___________________________________
```

#### 8.2 Manual de Usuario
Crear un manual simple para el usuario final con:
- Operación básica del menú
- Cómo cambiar horarios
- Qué hacer en caso de falla
- Contactos para soporte técnico

### 9. Checklist de Instalación Completa

#### Hardware
- [ ] Circuito armado según diagrama
- [ ] Todas las conexiones verificadas con multímetro
- [ ] Componentes montados en caja de protección
- [ ] Cableado de potencia realizado por electricista
- [ ] Protecciones eléctricas instaladas

#### Software
- [ ] Código de prueba ejecutado exitosamente
- [ ] Programa principal cargado
- [ ] Sistema inicia correctamente
- [ ] Fecha/hora configurada
- [ ] Modo de operación seleccionado
- [ ] Parámetros de tiempo configurados

#### Pruebas
- [ ] Funcionamiento del relé verificado
- [ ] Bomba arranca y para correctamente
- [ ] Interfaz de usuario responde
- [ ] Override manual funciona
- [ ] Sistema guarda configuración

#### Documentación
- [ ] Manual de usuario entregado
- [ ] Registro de instalación completado
- [ ] Información de contacto proporcionada
- [ ] Usuario capacitado en operación básica

---

## Solución de Problemas Durante la Instalación

### Error de compilación
```
error: 'RTClib.h' file not found
```
**Solución:** Instalar librería RTClib desde el Gestor de Librerías.

### Arduino no se conecta
```
avrdude: ser_open(): can't open device
```
**Solución:** 
- Verificar que el puerto COM sea correcto
- Instalar drivers USB del Arduino si es necesario
- Probar con otro cable USB

### RTC no responde
```
Error: No se encuentra el RTC
```
**Solución:** Verificar conexiones I2C (SDA/SCL) y alimentación.

### LCD muestra caracteres extraños
**Solución:** Ajustar potenciómetro de contraste o verificar conexiones de datos.

---

¡Instalación completada! El sistema debería estar funcionando correctamente. Para soporte adicional, consultar `TROUBLESHOOTING.md` o crear un issue en el repositorio del proyecto.