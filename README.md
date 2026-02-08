# 🎯 SISTEMA DE MONITOREO DE IMPACTOS - GREEN EGG

## Control de Calidad en Transporte de Piezas Cerámicas

---

## 📋 TABLA DE CONTENIDOS

1. [Resumen del Proyecto](#resumen-del-proyecto)
2. [Lista de Materiales](#lista-de-materiales)
3. [Diagrama de Conexiones](#diagrama-de-conexiones)
4. [Instalación y Configuración](#instalación-y-configuración)
5. [Uso del Sistema](#uso-del-sistema)
6. [Solución de Problemas](#solución-de-problemas)
7. [Archivos del Proyecto](#archivos-del-proyecto)

---

## 🎯 RESUMEN DEL PROYECTO

### Objetivo

Implementar un sistema automatizado de monitoreo de impactos para detectar y registrar golpes durante el transporte de piezas cerámicas Green Egg en carros de múltiples niveles. El sistema permite identificar prácticas de manejo inadecuadas y reducir daños en el producto.

### Características Principales

- ✅ Detección de impactos en tiempo real con sensor acelerómetro
- ✅ Clasificación automática de severidad: bajo, medio y alto
- ✅ Almacenamiento dual: tarjeta SD local + servidor WiFi
- ✅ Dashboard web en tiempo real con estadísticas
- ✅ Exportación de datos en formato CSV para análisis

### Beneficios

- 📉 Reducción de daños en piezas cerámicas
- 🎯 Identificación de puntos críticos en el proceso
- 📊 Datos objetivos para capacitación del personal
- 🔄 Mejora continua en procesos logísticos

---

## 🛒 LISTA DE MATERIALES

### Componentes Electrónicos

| Componente | Descripción | Precio Aprox. | Dónde Comprar |
|-----------|-------------|---------------|---------------|
| **ESP32 DevKit** | Microcontrolador con WiFi integrado | $8-12 USD | Amazon, AliExpress, MercadoLibre |
| **ADXL345** | Acelerómetro de 3 ejes, rango ±16g | $5-8 USD | Amazon, AliExpress |
| **Módulo MicroSD** | Lector de tarjeta SD para ESP32 | $3-5 USD | Amazon, AliExpress |
| **Tarjeta MicroSD** | 16GB mínimo, clase 10 | $5-8 USD | Cualquier tienda de electrónica |
| **Batería LiPo** | 3.7V 2000mAh con protección | $8-12 USD | Amazon, tiendas de drones |
| **Carcasa plástica** | Resistente a golpes, ~10x8x5cm | $3-5 USD | Ferreterías, Amazon |
| **Cables y conectores** | Dupont, jumpers, velcro | $3-5 USD | Tiendas de electrónica |

**COSTO TOTAL APROXIMADO: $35-45 USD**

### Herramientas Necesarias

- Soldador y estaño
- Cables dupont macho-hembra
- Multímetro (opcional, para verificación)
- Computadora con Arduino IDE instalado
- Cable USB para programar el ESP32

---

## 🔌 DIAGRAMA DE CONEXIONES

### Conexiones ESP32 ↔ ADXL345

| ADXL345 | ESP32 | Función |
|---------|-------|---------|
| VCC | 3.3V | Alimentación |
| GND | GND | Tierra |
| SDA | GPIO 21 | Datos I2C |
| SCL | GPIO 22 | Reloj I2C |

### Conexiones ESP32 ↔ Módulo MicroSD

| Módulo SD | ESP32 | Función |
|-----------|-------|---------|
| CS | GPIO 5 | Chip Select |
| MOSI | GPIO 23 | Master Out Slave In |
| MISO | GPIO 19 | Master In Slave Out |
| SCK | GPIO 18 | Serial Clock |
| VCC | 3.3V | Alimentación |
| GND | GND | Tierra |

### ⚠️ IMPORTANTE

> Verifica que tu módulo SD sea compatible con 3.3V. Algunos módulos requieren 5V y necesitarás un convertidor de nivel lógico.

---

## 💻 INSTALACIÓN Y CONFIGURACIÓN

### 1. Configuración del Arduino IDE

1. Descargar e instalar Arduino IDE desde [arduino.cc](https://www.arduino.cc/en/software)

2. Agregar soporte para ESP32:
   - Ir a **Archivo > Preferencias**
   - En "URLs de gestor de tarjetas adicionales" agregar:
     ```
     https://dl.espressif.com/dl/package_esp32_index.json
     ```
   - Ir a **Herramientas > Placa > Gestor de tarjetas**
   - Buscar "ESP32" e instalar

### 2. Instalación de Librerías

Desde el Arduino IDE, ir a **Sketch > Incluir Biblioteca > Administrar Bibliotecas** e instalar:

- `Adafruit ADXL345`
- `Adafruit Unified Sensor`

### 3. Configuración del Código Arduino

Editar las siguientes líneas en `impact_monitor.ino`:

```cpp
// Configurar tu red WiFi
const char* ssid = "TU_WIFI_AQUI";           // Cambiar por tu red WiFi
const char* password = "TU_PASSWORD_AQUI";   // Cambiar por tu contraseña

// Configurar la IP de tu servidor
const char* serverURL = "http://TU_SERVIDOR_IP:3000/api/impacts";
```

### 4. Configuración del Servidor Web

1. Instalar Node.js desde [nodejs.org](https://nodejs.org)

2. Crear una carpeta para el proyecto, por ejemplo: `monitor-impactos`

3. Dentro de la carpeta, crear una subcarpeta llamada `public`

4. Copiar los archivos:
   - `server.js` → en la carpeta principal
   - `dashboard.html` → dentro de la carpeta `public`

5. Abrir terminal en la carpeta del proyecto y ejecutar:
   ```bash
   npm install express cors
   ```

6. Iniciar el servidor:
   ```bash
   node server.js
   ```

7. Deberías ver:
   ```
   =================================
   Servidor de Monitoreo de Impactos
   =================================
   ✓ Servidor corriendo en http://localhost:3000
   ✓ Dashboard: http://localhost:3000
   ✓ API: http://localhost:3000/api/impacts
   
   Esperando datos del ESP32...
   ```

### 5. Carga del Código al ESP32

1. Conectar el ESP32 a la computadora vía USB
2. En Arduino IDE, seleccionar **Herramientas > Placa > ESP32 Dev Module**
3. Seleccionar el puerto COM correcto
4. Hacer clic en el botón **Subir** (flecha →)
5. Esperar a que se complete la carga

### 6. Verificación del Sistema

1. Abrir el Monitor Serie en Arduino IDE (115200 baudios)
2. Deberías ver:
   ```
   =================================
   Monitor de Impactos Green Egg
   =================================
   ✓ ADXL345 inicializado
   ✓ Tarjeta SD inicializada
   ✓ WiFi conectado
   IP: 192.168.X.X
   
   ✓ Sistema listo - Monitoreando impactos...
   ```

---

## 🚀 USO DEL SISTEMA

### 1. Montaje en el Carro

- Colocar el dispositivo en un lugar seguro del carro
- Usar velcro industrial o amarres de plástico para fijarlo firmemente
- Asegurar que el sensor esté orientado correctamente (cara plana hacia arriba)
- Conectar la batería y encender

### 2. Monitoreo en Tiempo Real

1. Asegurarse de que el servidor esté corriendo
2. Abrir un navegador web
3. Ir a `http://localhost:3000` o `http://IP_SERVIDOR:3000`
4. El dashboard se actualizará automáticamente cada 5 segundos

### 3. Interpretación de Datos

| Severidad | Magnitud | Interpretación | Color |
|-----------|----------|----------------|-------|
| **BAJO** | 2.0 - 4.0 G | Movimiento normal del carro, sin riesgo de daño | 🟢 Verde |
| **MEDIO** | 4.0 - 6.0 G | Golpe moderado, revisar técnica de manejo | 🟡 Amarillo |
| **ALTO** | > 6.0 G | Impacto fuerte, riesgo de daño en piezas | 🔴 Rojo |

### 4. Exportación y Análisis

- Hacer clic en el botón **"📥 Exportar CSV"** en el dashboard
- El archivo se descargará con todos los impactos registrados
- Abrir el archivo CSV en Excel para análisis detallado
- Los datos incluyen: timestamp, magnitud, ejes X/Y/Z, y severidad

---

## 🔧 SOLUCIÓN DE PROBLEMAS

### Problemas Comunes y Soluciones

| Problema | Solución |
|----------|----------|
| **No se detecta el ADXL345** | Verificar conexiones I2C (SDA/SCL). Revisar que VCC esté en 3.3V |
| **No conecta a WiFi** | Verificar SSID y contraseña. Revisar señal WiFi en el área. El ESP32 solo soporta WiFi 2.4GHz |
| **Error al guardar en SD** | Formatear tarjeta SD en FAT32. Verificar conexiones SPI. Probar con otra tarjeta |
| **No aparece en el dashboard** | Verificar que el servidor esté corriendo. Revisar URL del servidor en el código Arduino |
| **Detecta demasiados impactos** | Ajustar el valor de `THRESHOLD_LOW` en el código (actualmente 2.0G) |
| **Batería se agota rápido** | Reducir frecuencia de transmisión WiFi o usar batería de mayor capacidad (3000mAh+) |
| **LED parpadea constantemente** | Error en el sensor ADXL345. Verificar todas las conexiones |
| **Dashboard no actualiza** | Verificar conexión WiFi del ESP32. Ver en Monitor Serie si hay errores de transmisión |

### Códigos de Error en Monitor Serie

- `❌ ERROR: No se detectó el ADXL345!` → Problema de conexión I2C
- `⚠ Advertencia: Tarjeta SD no disponible` → SD no insertada o mal formateada
- `⚠ WiFi no conectado` → Problema con credenciales o señal WiFi
- `❌ Error al enviar: ...` → Problema de comunicación con el servidor

---

## 📁 ARCHIVOS DEL PROYECTO

### Archivos Incluidos

1. **impact_monitor.ino** - Código principal para ESP32
2. **server.js** - Servidor Node.js para recibir datos
3. **dashboard.html** - Interfaz web para visualización
4. **README.md** - Este documento

### Estructura de Carpetas Recomendada

```
proyecto-monitor-impactos/
├── Arduino/
│   └── impact_monitor.ino
├── Servidor/
│   ├── server.js
│   ├── package.json
│   └── public/
│       └── dashboard.html
└── Documentacion/
    └── README.md
```

---

## 📊 AJUSTES AVANZADOS

### Modificar Umbrales de Detección

En el archivo `impact_monitor.ino`, líneas 24-26:

```cpp
const float THRESHOLD_LOW = 2.0;    // Golpe leve (G) - AJUSTAR SEGÚN NECESIDAD
const float THRESHOLD_MED = 4.0;    // Golpe medio (G)
const float THRESHOLD_HIGH = 6.0;   // Golpe fuerte (G)
```

### Cambiar Frecuencia de Lectura

En el archivo `impact_monitor.ino`, última línea del `loop()`:

```cpp
delay(50);  // Lectura cada 50ms (20 Hz) - REDUCIR para más sensibilidad
```

### Configurar Zona Horaria

En el archivo `impact_monitor.ino`, línea 99:

```cpp
configTime(-6 * 3600, 0, "pool.ntp.org");  // GMT-6 (Monterrey)
// Cambiar -6 por tu zona horaria
```

---

## 🔐 SEGURIDAD Y MEJORES PRÁCTICAS

1. **Protección de la Batería**: Usar siempre baterías LiPo con circuito de protección
2. **Montaje Seguro**: Verificar que el dispositivo esté bien sujeto antes de cada uso
3. **Respaldo de Datos**: La SD guarda datos incluso sin WiFi - revisar periódicamente
4. **Mantenimiento**: Limpiar el dispositivo semanalmente de polvo y suciedad
5. **Calibración**: Probar el sistema con impactos conocidos para verificar precisión

---

## 📞 SOPORTE Y CONTACTO

Para asistencia técnica adicional o mejoras al sistema:

1. Documentar el problema con capturas de pantalla
2. Incluir los mensajes del Monitor Serie
3. Describir qué estabas haciendo cuando ocurrió el problema
4. Especificar la configuración de tu hardware

---

## 📝 NOTAS ADICIONALES

### Autonomía de la Batería

Con una batería de 2000mAh:
- **Modo WiFi activo**: ~8-12 horas
- **Solo SD (sin WiFi)**: ~24-36 horas

### Capacidad de la Tarjeta SD

Una tarjeta de 16GB puede almacenar aproximadamente:
- 500,000+ registros de impactos
- Equivalente a varios meses de operación continua

### Alcance WiFi

El ESP32 tiene un alcance típico de:
- **Interior**: 30-50 metros
- **Exterior**: hasta 100 metros (línea de vista)

---

## ✅ CHECKLIST PRE-OPERACIÓN

Antes de cada uso, verificar:

- [ ] Batería completamente cargada
- [ ] Tarjeta SD insertada y formateada
- [ ] Todas las conexiones firmes
- [ ] LED de encendido iluminado
- [ ] WiFi conectado (verificar en Monitor Serie)
- [ ] Servidor web corriendo
- [ ] Dashboard accesible desde navegador
- [ ] Dispositivo bien montado en el carro

---

**Versión del Documento**: 1.0  
**Fecha**: Febrero 2026  
**Autor**: Sistema de Documentación Técnica

---

## 🎉 ¡LISTO PARA USAR!

Tu sistema de monitoreo de impactos está completo. Sigue las instrucciones paso a paso y en poco tiempo estarás recolectando datos valiosos para mejorar tus procesos de transporte.

**¡Buena suerte con tu proyecto!** 🚀
