# Guía de Solución de Problemas - Sistema Bomba de Agua

## Problemas Comunes y Soluciones

### 🔧 Problemas de Inicialización

#### El sistema no inicia / Pantalla en blanco
**Síntomas:**
- LCD completamente en blanco
- Sin respuesta a botones
- LED de Arduino no enciende

**Posibles causas y soluciones:**
1. **Problema de alimentación**
   - ✅ Verificar conexión USB o fuente externa
   - ✅ Comprobar voltaje: 5V en pin VCC del Arduino
   - ✅ Revisar fusibles si usa fuente externa

2. **Problema de carga del código**
   - ✅ Re-cargar el sketch principal
   - ✅ Verificar que se seleccionó la placa correcta
   - ✅ Probar con sketch de prueba `test_components.ino`

3. **Hardware defectuoso**
   - ✅ Probar con otro Arduino
   - ✅ Verificar cortocircuitos en conexiones

#### Error "RTC no encontrado"
**Síntomas:**
- Mensaje de error en LCD: "Error RTC"
- En monitor serial: "Error: No se encuentra el RTC"

**Soluciones:**
1. **Verificar conexiones I2C**
   ```
   DS3231    Arduino
   ------    -------
   SDA   →   A4
   SCL   →   A5
   VCC   →   5V
   GND   →   GND
   ```

2. **Probar comunicación I2C**
   ```cpp
   // Código de prueba I2C
   #include <Wire.h>
   
   void setup() {
     Wire.begin();
     Serial.begin(9600);
     Serial.println("Escaneando I2C...");
     
     for (byte i = 8; i < 120; i++) {
       Wire.beginTransmission(i);
       if (Wire.endTransmission() == 0) {
         Serial.print("Dispositivo encontrado en 0x");
         Serial.println(i, HEX);
       }
     }
   }
   
   void loop() {}
   ```
   - DS3231 debería aparecer en dirección 0x68

3. **Verificar módulo RTC**
   - ✅ Probar con otro módulo DS3231
   - ✅ Verificar que no sea DS1307 (diferente librería)
   - ✅ Comprobar soldaduras del módulo

### ⚡ Problemas con el Relé

#### La bomba no se enciende
**Síntomas:**
- Sistema indica "Bomba ON" pero no funciona
- No se escucha click del relé
- LED de estado parpadea pero bomba no arranca

**Diagnóstico paso a paso:**
1. **Verificar el relé**
   ```cpp
   // Código de prueba manual del relé
   void loop() {
     digitalWrite(8, HIGH);  // Encender relé
     delay(2000);
     digitalWrite(8, LOW);   // Apagar relé
     delay(2000);
   }
   ```
   - ✅ Debe escucharse click del relé cada 2 segundos

2. **Verificar conexiones del relé**
   ```
   Relé      Arduino
   ----      -------
   IN    →   Pin 8
   VCC   →   5V
   GND   →   GND
   ```

3. **Verificar cableado de potencia**
   - ⚠️ **PELIGRO: 220V - Desconectar antes de revisar**
   - ✅ COM conectado a fase de entrada
   - ✅ NO conectado a bomba
   - ✅ Neutro directo a bomba
   - ✅ Fusible en línea

4. **Problemas con bomba grande**
   - Si la bomba consume >10A, usar contactor adicional
   - El relé del Arduino controla la bobina del contactor
   - El contactor maneja la potencia de la bomba

#### Relé se activa pero bomba no arranca
**Posibles causas:**
1. **Bomba bloqueada o defectuosa**
   - ✅ Probar bomba directamente a 220V
   - ✅ Verificar que no esté obstruida

2. **Caída de tensión**
   - ✅ Medir voltaje en bornes de la bomba
   - ✅ Verificar calibre de cables
   - ✅ Comprobar conexiones flojas

3. **Protección térmica activa**
   - ✅ Esperar enfriamiento de la bomba
   - ✅ Verificar ventilación adecuada

### 📱 Problemas de Interfaz

#### LCD muestra caracteres extraños
**Síntomas:**
- Caracteres ilegibles o símbolos raros
- Texto desplazado
- Solo una línea funciona

**Soluciones:**
1. **Verificar conexiones LCD**
   ```
   LCD       Arduino
   ---       -------
   RS    →   Pin 7
   E     →   Pin 9
   D4    →   Pin 10
   D5    →   Pin 11
   D6    →   Pin 12
   D7    →   Pin A1
   ```

2. **Ajustar contraste**
   - ✅ Girar potenciómetro de contraste
   - ✅ Si no hay potenciómetro, conectar V0 a GND (máximo contraste)

3. **Verificar alimentación LCD**
   - ✅ VDD a 5V, VSS a GND
   - ✅ Backlight: A a 5V (con resistencia), K a GND

4. **Probar reinicio LCD**
   ```cpp
   // En setup(), después de lcd.begin(16, 2)
   lcd.clear();
   delay(100);
   lcd.home();
   ```

#### Botones no responden
**Síntomas:**
- No hay respuesta al presionar botones
- Menú no cambia
- Respuesta errática

**Diagnóstico:**
1. **Verificar conexiones**
   ```
   Botón     Arduino
   -----     -------
   UP    →   Pin 2
   DOWN  →   Pin 3
   SELECT→   Pin 4
   BACK  →   Pin 5
   MANUAL→   Pin 6
   ```

2. **Probar botones individualmente**
   ```cpp
   void loop() {
     Serial.print("Botones: ");
     Serial.print(digitalRead(2)); // UP
     Serial.print(digitalRead(3)); // DOWN
     Serial.print(digitalRead(4)); // SELECT
     Serial.print(digitalRead(5)); // BACK
     Serial.println(digitalRead(6)); // MANUAL
     delay(100);
   }
   ```
   - Valores normales: 1 (no presionado), 0 (presionado)

3. **Verificar pull-ups**
   - ✅ Usar `INPUT_PULLUP` en pinMode()
   - ✅ O conectar resistencias pull-up externas (10kΩ)

### ⏰ Problemas de Temporización

#### La bomba no se activa en los horarios configurados
**Síntomas:**
- Configuración parece correcta
- Hora del sistema correcta
- Bomba no arranca cuando debería

**Verificación paso a paso:**
1. **Comprobar fecha/hora actual**
   ```cpp
   // En loop(), temporalmente
   DateTime now = rtc.now();
   Serial.print(now.hour());
   Serial.print(":");
   Serial.println(now.minute());
   ```

2. **Verificar configuración**
   - ✅ Modo correcto (Intervalo/Días)
   - ✅ Días de la semana seleccionados
   - ✅ Rango horario correcto
   - ✅ Configuración guardada en EEPROM

3. **Revisar lógica de tiempo**
   - En modo días: verificar que día actual esté activo
   - En modo intervalo: verificar último bombeo vs intervalo
   - Considerar cambios de horario (verano/invierno)

#### Fecha/hora se resetea constantemente
**Síntomas:**
- Fecha vuelve a valores por defecto
- Hora se atrasa o adelanta
- Pérdida de configuración temporal

**Soluciones:**
1. **Problema de batería RTC**
   - ✅ Reemplazar batería del DS3231 (CR2032)
   - ✅ Verificar que la batería esté bien conectada

2. **Interferencia eléctrica**
   - ✅ Usar cables blindados para I2C
   - ✅ Mantener cables cortos (<30cm)
   - ✅ Agregar capacitores de desacople (100nF)

3. **Código incorrecto**
   - ✅ No llamar `rtc.adjust()` en cada inicio
   - ✅ Solo ajustar cuando `rtc.lostPower()` sea true

### 🚨 Problemas de Seguridad

#### Sensor de agua no funciona
**Síntomas:**
- Sistema ignora nivel de agua
- Valores del sensor no cambian
- Bomba funciona sin agua

**Diagnóstico:**
1. **Verificar conexión sensor**
   - ✅ Pin A0 conectado a señal del sensor
   - ✅ Alimentación 5V/GND correcta

2. **Probar rango del sensor**
   ```cpp
   void loop() {
     int valor = analogRead(A0);
     Serial.print("Sensor agua: ");
     Serial.println(valor);
     delay(500);
   }
   ```
   - Anotar valores con/sin agua
   - Configurar umbral entre estos valores

3. **Calibrar umbral**
   - Ir a Configuración → Sensor
   - Ajustar `minWaterLevel` según valores obtenidos

#### Bomba no para (funcionamiento continuo)
**⚠️ EMERGENCIA - Detener inmediatamente**

**Acciones inmediatas:**
1. Activar interruptor manual (debería apagarla)
2. Si no responde: desconectar alimentación del Arduino
3. Como último recurso: desconectar bomba de 220V

**Investigación posterior:**
1. **Problema de software**
   - ✅ Re-cargar código desde cero
   - ✅ Verificar lógica de temporización
   - ✅ Revisar condiciones de parada

2. **Relé pegado**
   - ✅ Reemplazar relé defectuoso
   - ✅ Verificar que NO del relé abre correctamente

3. **Interferencia eléctrica**
   - ✅ Usar fuente estabilizada
   - ✅ Separar cables de potencia y señal
   - ✅ Agregar filtros de ruido

### 🔍 Herramientas de Diagnóstico

#### Monitor Serial
**Configuración:**
- Velocidad: 9600 baud
- Final de línea: Nueva línea
- Codificación: UTF-8

**Comandos útiles para debugging:**
```cpp
// Mostrar estado completo
void debugStatus() {
  Serial.println("=== ESTADO DEL SISTEMA ===");
  Serial.print("Bomba: ");
  Serial.println(pumpRunning ? "ON" : "OFF");
  Serial.print("Modo: ");
  Serial.println(config.mode);
  DateTime now = rtc.now();
  Serial.print("Fecha/Hora: ");
  Serial.print(now.day());
  Serial.print("/");
  Serial.print(now.month());
  Serial.print(" ");
  Serial.print(now.hour());
  Serial.print(":");
  Serial.println(now.minute());
}
```

#### Uso del sketch de pruebas
```bash
# 1. Cargar test_components.ino
# 2. Abrir monitor serial
# 3. Probar cada componente individualmente
# 4. Anotar resultados para diagnóstico
```

#### Multímetro
**Mediciones importantes:**
- **5V entre VCC y GND** del Arduino
- **3.3V en pin 3.3V** (para verificar regulador)
- **0V/5V en pin del relé** (según estado)
- **220V en COM del relé** cuando esté energizado
- **Continuidad** en todas las conexiones

### 🔧 Mantenimiento Preventivo

#### Rutina Mensual
- [ ] Verificar funcionamiento automático
- [ ] Limpiar contactos del relé si es necesario
- [ ] Revisar conexiones por corrosión
- [ ] Comprobar logs en monitor serial
- [ ] Verificar respuesta de botones

#### Rutina Semestral
- [ ] Reemplazar batería RTC si es necesario
- [ ] Limpiar polvo de componentes
- [ ] Verificar calibración del sensor de agua
- [ ] Revisar apriete de borneras
- [ ] Backup de configuración

#### Rutina Anual
- [ ] Revisar desgaste del relé
- [ ] Comprobar aislación de cables
- [ ] Verificar protecciones eléctricas
- [ ] Actualizar código si hay mejoras
- [ ] Documentar cambios y reparaciones

### 📞 Cuándo Pedir Ayuda

**Contactar electricista si:**
- Problemas con cableado de 220V
- Bomba consume corriente excesiva
- Protecciones diferenciales saltan
- Dudas sobre instalación eléctrica

**Contactar técnico en electrónica si:**
- Múltiples componentes fallan
- Problemas intermitentes no identificados
- Se requieren modificaciones avanzadas
- Interferencias electromagnéticas

**Reemplazar sistema si:**
- Arduino dañado irreparablemente
- Múltiples fallas recurrentes
- Costo de reparación > 70% del nuevo
- Sistema no cumple expectativas de confiabilidad

### 📋 Lista de Verificación para Instalación

#### Pre-instalación
- [ ] Todos los componentes probados individualmente
- [ ] Código cargado y funcionando en protoboard
- [ ] Caja de protección adecuada disponible
- [ ] Herramientas eléctricas disponibles
- [ ] Permisos eléctricos (si requeridos)

#### Durante instalación
- [ ] Desconectar alimentación antes de cablear
- [ ] Verificar polaridad de conexiones DC
- [ ] Usar terminales apropiados para 220V
- [ ] Instalar fusibles de protección
- [ ] Cablear con codificación de colores estándar

#### Post-instalación
- [ ] Configurar fecha/hora inicial
- [ ] Calibrar sensor de agua (si aplica)
- [ ] Probar funcionamiento manual
- [ ] Configurar horarios/intervalos iniciales
- [ ] Documentar configuración final
- [ ] Entrenar al usuario final

---

## Códigos de Error

| Código | Descripción | Acción |
|--------|-------------|--------|
| E01 | RTC no responde | Verificar conexiones I2C |
| E02 | EEPROM corrupta | Resetear configuración |
| E03 | Sensor agua desconectado | Revisar pin A0 |
| E04 | Relé no responde | Verificar pin 8 y relé |
| E05 | Sobrecorriente detectada | Verificar bomba |

---

**⚠️ Recordatorio de Seguridad:** 
Siempre desconectar la alimentación antes de realizar cualquier trabajo en el sistema. Para trabajo con 220V, consultar con electricista calificado.

**📧 Soporte:** 
Para problemas no cubiertos en esta guía, crear un issue en el repositorio del proyecto con:
- Descripción detallada del problema
- Código de error (si aplica)
- Fotos del hardware
- Logs del monitor serial