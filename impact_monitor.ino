/*
 * Sistema de Monitoreo de Impactos para Carros de Transporte
 * Green Egg - Control de Calidad en Logística
 * 
 * Hardware: ESP32 + ADXL345 + MicroSD
 * 
 * Autor: Claude AI
 * Fecha: Febrero 2026
 */

#include <Wire.h>
#include <Adafruit_ADXL345_U.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <SD.h>
#include <SPI.h>
#include <time.h>

// ===== CONFIGURACIÓN WIFI =====
const char* ssid = "TU_WIFI_AQUI";           // Cambiar por tu red WiFi
const char* password = "TU_PASSWORD_AQUI";   // Cambiar por tu contraseña

// ===== CONFIGURACIÓN SERVIDOR =====
const char* serverURL = "http://TU_SERVIDOR_IP:3000/api/impacts";  // URL de tu servidor

// ===== CONFIGURACIÓN PINES =====
#define SD_CS 5              // Pin CS de la tarjeta SD
#define LED_BUILTIN 2        // LED incorporado del ESP32

// ===== CONFIGURACIÓN SENSOR =====
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

// ===== PARÁMETROS DE DETECCIÓN =====
const float THRESHOLD_LOW = 2.0;    // Golpe leve (G)
const float THRESHOLD_MED = 4.0;    // Golpe medio (G)
const float THRESHOLD_HIGH = 6.0;   // Golpe fuerte (G)
const int DEBOUNCE_TIME = 500;      // Tiempo entre golpes (ms)

// ===== VARIABLES GLOBALES =====
unsigned long lastImpactTime = 0;
int impactCountLow = 0;
int impactCountMed = 0;
int impactCountHigh = 0;
bool wifiConnected = false;
bool sdAvailable = false;

// ===== ESTRUCTURA PARA DATOS DE IMPACTO =====
struct Impact {
  float magnitude;
  float x, y, z;
  unsigned long timestamp;
  String severity;
};

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("=================================");
  Serial.println("Monitor de Impactos Green Egg");
  Serial.println("=================================");
  
  // Configurar LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  // Inicializar sensor ADXL345
  if(!accel.begin()) {
    Serial.println("❌ ERROR: No se detectó el ADXL345!");
    while(1) {
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      delay(200);
    }
  }
  Serial.println("✓ ADXL345 inicializado");
  
  // Configurar rango del sensor (±16g para detectar golpes fuertes)
  accel.setRange(ADXL345_RANGE_16_G);
  
  // Inicializar tarjeta SD
  if(!SD.begin(SD_CS)) {
    Serial.println("⚠ Advertencia: Tarjeta SD no disponible");
    sdAvailable = false;
  } else {
    Serial.println("✓ Tarjeta SD inicializada");
    sdAvailable = true;
    
    // Crear archivo de log si no existe
    File file = SD.open("/impacts.csv", FILE_WRITE);
    if(file) {
      if(file.size() == 0) {
        file.println("Timestamp,Magnitud,X,Y,Z,Severidad");
      }
      file.close();
    }
  }
  
  // Conectar a WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  
  int attempts = 0;
  while(WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if(WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✓ WiFi conectado");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    wifiConnected = true;
    
    // Configurar NTP para obtener tiempo real
    configTime(-6 * 3600, 0, "pool.ntp.org");  // GMT-6 (Monterrey)
  } else {
    Serial.println("\n⚠ WiFi no conectado - solo se guardará en SD");
    wifiConnected = false;
  }
  
  Serial.println("\n✓ Sistema listo - Monitoreando impactos...\n");
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  // Leer datos del acelerómetro
  sensors_event_t event;
  accel.getEvent(&event);
  
  // Calcular magnitud del vector de aceleración
  // Restamos 1G de gravedad (aproximadamente)
  float x = event.acceleration.x;
  float y = event.acceleration.y;
  float z = event.acceleration.z - 9.8;  // Compensar gravedad
  
  float magnitude = sqrt(x*x + y*y + z*z);
  
  // Detectar impacto
  if(magnitude > THRESHOLD_LOW) {
    unsigned long currentTime = millis();
    
    // Debounce - ignorar golpes muy cercanos en el tiempo
    if(currentTime - lastImpactTime > DEBOUNCE_TIME) {
      lastImpactTime = currentTime;
      
      // Crear objeto de impacto
      Impact impact;
      impact.magnitude = magnitude;
      impact.x = x;
      impact.y = y;
      impact.z = z;
      impact.timestamp = currentTime;
      
      // Clasificar severidad
      if(magnitude >= THRESHOLD_HIGH) {
        impact.severity = "ALTO";
        impactCountHigh++;
        Serial.println("🔴 IMPACTO ALTO!");
      } else if(magnitude >= THRESHOLD_MED) {
        impact.severity = "MEDIO";
        impactCountMed++;
        Serial.println("🟡 IMPACTO MEDIO");
      } else {
        impact.severity = "BAJO";
        impactCountLow++;
        Serial.println("🟢 IMPACTO BAJO");
      }
      
      // Mostrar datos del impacto
      Serial.printf("   Magnitud: %.2f G\n", magnitude);
      Serial.printf("   X: %.2f, Y: %.2f, Z: %.2f\n", x, y, z);
      Serial.printf("   Total: Bajos=%d, Medios=%d, Altos=%d\n\n", 
                    impactCountLow, impactCountMed, impactCountHigh);
      
      // Parpadear LED
      for(int i = 0; i < 3; i++) {
        digitalWrite(LED_BUILTIN, LOW);
        delay(50);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(50);
      }
      
      // Guardar datos
      saveToSD(impact);
      sendToServer(impact);
    }
  }
  
  delay(50);  // Lectura cada 50ms (20 Hz)
}

// ===== FUNCIÓN: GUARDAR EN TARJETA SD =====
void saveToSD(Impact impact) {
  if(!sdAvailable) return;
  
  File file = SD.open("/impacts.csv", FILE_APPEND);
  if(file) {
    // Obtener timestamp formateado
    time_t now;
    time(&now);
    char timeStr[30];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localtime(&now));
    
    // Escribir línea CSV
    file.printf("%s,%.2f,%.2f,%.2f,%.2f,%s\n", 
                timeStr, impact.magnitude, impact.x, impact.y, impact.z, 
                impact.severity.c_str());
    file.close();
    Serial.println("✓ Guardado en SD");
  } else {
    Serial.println("❌ Error al guardar en SD");
  }
}

// ===== FUNCIÓN: ENVIAR A SERVIDOR VIA WIFI =====
void sendToServer(Impact impact) {
  if(!wifiConnected) return;
  
  HTTPClient http;
  http.begin(serverURL);
  http.addHeader("Content-Type", "application/json");
  
  // Crear JSON
  time_t now;
  time(&now);
  char timeStr[30];
  strftime(timeStr, sizeof(timeStr), "%Y-%m-%dT%H:%M:%S", localtime(&now));
  
  String jsonData = "{";
  jsonData += "\"timestamp\":\"" + String(timeStr) + "\",";
  jsonData += "\"magnitude\":" + String(impact.magnitude, 2) + ",";
  jsonData += "\"x\":" + String(impact.x, 2) + ",";
  jsonData += "\"y\":" + String(impact.y, 2) + ",";
  jsonData += "\"z\":" + String(impact.z, 2) + ",";
  jsonData += "\"severity\":\"" + impact.severity + "\"";
  jsonData += "}";
  
  // Enviar POST
  int httpCode = http.POST(jsonData);
  
  if(httpCode > 0) {
    Serial.printf("✓ Enviado a servidor (código %d)\n", httpCode);
  } else {
    Serial.printf("❌ Error al enviar: %s\n", http.errorToString(httpCode).c_str());
  }
  
  http.end();
}
