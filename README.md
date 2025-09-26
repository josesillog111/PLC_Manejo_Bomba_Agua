# PLC Manejo Bomba de Agua - Sistema de Control Automático

## Descripción

Este proyecto implementa un sistema de control automático para bombas de agua usando Arduino. El sistema permite configurar el funcionamiento de la bomba mediante dos métodos principales:

1. **Por intervalos de tiempo**: La bomba se activa cada X minutos durante Y minutos
2. **Por días específicos**: La bomba funciona en días seleccionados dentro de un rango horario

## Características Principales

- ✅ Control automático basado en tiempo real (RTC)
- ✅ Dos modos de configuración: intervalos o días específicos
- ✅ Interfaz de usuario con LCD y botones
- ✅ Control manual de emergencia
- ✅ Protección contra funcionamiento en seco (sensor de agua opcional)
- ✅ Almacenamiento de configuración en EEPROM
- ✅ Monitoreo y logging por puerto serial
- ✅ Indicadores LED de estado
- ✅ Menú de configuración intuitivo

## Hardware Requerido

### Componentes Principales
- **Arduino Uno/Nano** - Controlador principal
- **Módulo RTC DS3231** - Reloj de tiempo real para precisión
- **Relé 5V/10A** - Control de la bomba (compatible con bomba de 220V)
- **Display LCD 16x2** - Interfaz de usuario
- **4 Botones pulsadores** - Navegación del menú
- **Interruptor** - Override manual
- **LED** - Indicador de estado

### Componentes Opcionales
- **Sensor de nivel de agua** - Protección contra funcionamiento en seco
- **Fuente de alimentación 12V** - Para sistemas con bombas de mayor potencia
- **Caja de protección IP65** - Para instalación en exteriores

## Conexiones Eléctricas

### Pines de Arduino

| Componente | Pin Arduino | Descripción |
|------------|-------------|-------------|
| Relé de bomba | Pin 8 | Control ON/OFF de la bomba |
| Sensor de agua | Pin A0 | Lectura analógica del nivel |
| Botón UP | Pin 2 | Navegación hacia arriba |
| Botón DOWN | Pin 3 | Navegación hacia abajo |
| Botón SELECT | Pin 4 | Confirmar selección |
| Botón BACK | Pin 5 | Volver/Cancelar |
| Override manual | Pin 6 | Interruptor de control manual |
| LCD RS | Pin 7 | Control del LCD |
| LCD Enable | Pin 9 | Control del LCD |
| LCD D4-D7 | Pines 10-12, A1 | Datos del LCD |
| LED estado | Pin 13 | Indicador visual |

### Conexión RTC DS3231
- **VCC** → 5V Arduino
- **GND** → GND Arduino  
- **SDA** → Pin A4 Arduino
- **SCL** → Pin A5 Arduino

### Conexión del Relé
⚠️ **IMPORTANTE**: Para bombas de 220V, usar relé adecuado y seguir normas de seguridad eléctrica.

```
Arduino Pin 8 → IN del relé
5V Arduino → VCC del relé
GND Arduino → GND del relé
COM del relé → Fase de entrada 220V
NO del relé → Fase hacia la bomba
```

## Instalación y Configuración

### 1. Preparación del IDE Arduino
```bash
# Instalar librerías necesarias desde el Gestor de Librerías:
- RTClib by Adafruit
- LiquidCrystal (incluida por defecto)
```

### 2. Carga del Código
1. Conectar Arduino a la computadora
2. Abrir `PLC_Manejo_Bomba_Agua.ino` en Arduino IDE
3. Seleccionar la placa correcta (Arduino Uno/Nano)
4. Seleccionar el puerto COM correspondiente
5. Compilar y cargar el código

### 3. Primer Uso
1. El sistema iniciará con configuración por defecto
2. Configurar fecha/hora actual a través del menú
3. Seleccionar el modo de operación deseado
4. Configurar intervalos o días según el modo elegido

## Modo de Uso

### Navegación del Menú
- **Botón UP**: Navegar hacia arriba en las opciones
- **Botón DOWN**: Navegar hacia abajo en las opciones  
- **Botón SELECT**: Confirmar selección o cambiar valor
- **Botón BACK**: Volver al menú anterior

### Configuración por Intervalos
1. Ir a "Configuración" → "Config Intervalo"
2. Configurar "Cada X minutos" (intervalo entre activaciones)
3. Configurar "Duración Y minutos" (tiempo que permanece encendida)

**Ejemplo**: Cada 60 minutos durante 10 minutos
- La bomba se encenderá cada hora durante 10 minutos

### Configuración por Días Específicos
1. Ir a "Configuración" → "Config Días"
2. Seleccionar días activos (L-D para Lunes a Domingo)
3. Configurar hora de inicio (formato 24h)
4. Configurar hora de fin (formato 24h)

**Ejemplo**: Lunes a Viernes de 6:00 a 22:00
- La bomba funcionará solo en días laborables en horario diurno

### Control Manual
- Activar el interruptor de "Override Manual"
- La bomba se encenderá inmediatamente
- El modo automático se suspende mientras esté activo
- Desactivar para volver al modo automático

## Características de Seguridad

### Protección contra Funcionamiento en Seco
Si se habilita el sensor de agua:
- El sistema verificará el nivel antes de cada activación
- Si el nivel es insuficiente, cancelará el bombeo
- Se registrará el evento en el log

### Almacenamiento de Configuración
- Toda la configuración se guarda automáticamente en EEPROM
- Los ajustes se conservan aunque se corte la energía
- El RTC mantiene la hora con su batería de respaldo

### Logging y Monitoreo
- Todos los eventos se registran por puerto serial (9600 baud)
- Timestamps precisos de cada operación
- Útil para diagnóstico y mantenimiento

## Códigos de Estado LED

| Patrón LED | Estado |
|------------|--------|
| Apagado | Sistema en modo manual o error |
| Parpadeo lento (2s) | Modo automático, bomba apagada |
| Parpadeo rápido (0.5s) | Bomba funcionando |

## Solución de Problemas

### La bomba no se enciende
1. Verificar configuración del modo y horarios
2. Revisar conexiones del relé
3. Comprobar nivel de agua (si sensor habilitado)
4. Verificar que no esté en modo manual

### Fecha/hora incorrecta
1. Ir a "Configuración" → "Ajustar Hora"
2. Configurar año, mes, día, hora y minutos
3. Si persiste el problema, verificar batería del RTC

### LCD no muestra información
1. Verificar conexiones del LCD
2. Ajustar contraste del LCD si es necesario
3. Comprobar alimentación 5V

### Configuración se resetea
1. Problema en EEPROM - puede requerir reemplazo de Arduino
2. Interferencia eléctrica - verificar alimentación estable

## Personalización

### Modificar Intervalos Mínimos
En el código, cambiar las constantes:
```cpp
// Intervalo mínimo de 15 minutos
if (down && config.intervalMinutes > 15) config.intervalMinutes -= 15;

// Duración mínima de 5 minutos  
if (down && config.pumpDurationMinutes > 5) config.pumpDurationMinutes -= 5;
```

### Agregar Más Botones
Modificar las definiciones de pines y agregar lógica en `processButtons()`

### Cambiar Mensajes del LCD
Modificar las cadenas de texto en las funciones `display*()`

## Mantenimiento

### Rutina Mensual
- Verificar funcionamiento del sistema
- Limpiar contactos del relé si es necesario
- Revisar nivel de agua en depósito
- Comprobar logs por puerto serial

### Rutina Anual  
- Reemplazar batería del RTC si es necesario
- Revisar todas las conexiones eléctricas
- Calibrar sensor de agua (si se usa)
- Backup de configuración

## Expansiones Futuras

- 📡 Conectividad WiFi para monitoreo remoto
- 📱 App móvil para control y configuración
- 🌡️ Sensores de temperatura y humedad
- 💧 Múltiples bombas y zonas de riego
- 📊 Estadísticas de consumo y eficiencia
- 🚨 Alertas por SMS/email en caso de fallas

## Contribuciones

Este proyecto está abierto a contribuciones. Para reportar bugs o sugerir mejoras:

1. Crear un issue describiendo el problema o mejora
2. Fork del repositorio
3. Crear branch para la funcionalidad
4. Enviar pull request con descripción detallada

## Licencia

Este proyecto está bajo licencia MIT. Ver archivo LICENSE para más detalles.

## Contacto

Para soporte técnico o consultas sobre el proyecto, crear un issue en el repositorio.

---

**⚠️ Advertencia de Seguridad**: Este sistema maneja voltajes peligrosos (220V). La instalación debe ser realizada por personal calificado siguiendo las normas locales de seguridad eléctrica. El autor no se hace responsable por daños causados por instalación o uso inadecuado del sistema.