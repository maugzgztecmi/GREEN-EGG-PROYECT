/*
 * Servidor Web para Monitoreo de Impactos
 * Recibe datos del ESP32 y los muestra en un dashboard web
 * 
 * Instalación:
 * npm install express cors
 * 
 * Uso:
 * node server.js
 */

const express = require('express');
const cors = require('cors');
const fs = require('fs');
const path = require('path');

const app = express();
const PORT = 3000;

// Middleware
app.use(cors());
app.use(express.json());
app.use(express.static('public'));

// Almacenamiento en memoria
let impacts = [];
const MAX_IMPACTS = 1000; // Mantener últimos 1000 impactos

// ===== ENDPOINT: RECIBIR IMPACTOS =====
app.post('/api/impacts', (req, res) => {
  const impact = {
    ...req.body,
    receivedAt: new Date().toISOString()
  };
  
  console.log('📊 Nuevo impacto recibido:');
  console.log(`   Severidad: ${impact.severity}`);
  console.log(`   Magnitud: ${impact.magnitude} G`);
  console.log(`   Timestamp: ${impact.timestamp}`);
  
  // Agregar a la lista
  impacts.push(impact);
  
  // Mantener solo los últimos MAX_IMPACTS
  if(impacts.length > MAX_IMPACTS) {
    impacts = impacts.slice(-MAX_IMPACTS);
  }
  
  // Guardar en archivo CSV
  saveToFile(impact);
  
  res.json({ success: true, message: 'Impacto registrado' });
});

// ===== ENDPOINT: OBTENER IMPACTOS =====
app.get('/api/impacts', (req, res) => {
  const limit = parseInt(req.query.limit) || 100;
  const severity = req.query.severity;
  
  let filtered = impacts;
  
  if(severity) {
    filtered = impacts.filter(i => i.severity === severity);
  }
  
  res.json({
    total: filtered.length,
    impacts: filtered.slice(-limit)
  });
});

// ===== ENDPOINT: ESTADÍSTICAS =====
app.get('/api/stats', (req, res) => {
  const stats = {
    total: impacts.length,
    low: impacts.filter(i => i.severity === 'BAJO').length,
    medium: impacts.filter(i => i.severity === 'MEDIO').length,
    high: impacts.filter(i => i.severity === 'ALTO').length,
    avgMagnitude: impacts.length > 0 
      ? (impacts.reduce((sum, i) => sum + parseFloat(i.magnitude), 0) / impacts.length).toFixed(2)
      : 0
  };
  
  res.json(stats);
});

// ===== ENDPOINT: LIMPIAR DATOS =====
app.delete('/api/impacts', (req, res) => {
  impacts = [];
  console.log('🗑️  Datos limpiados');
  res.json({ success: true, message: 'Datos limpiados' });
});

// ===== FUNCIÓN: GUARDAR EN ARCHIVO =====
function saveToFile(impact) {
  const filename = 'impacts_log.csv';
  const csvLine = `${impact.timestamp},${impact.magnitude},${impact.x},${impact.y},${impact.z},${impact.severity}\n`;
  
  // Crear archivo con encabezado si no existe
  if(!fs.existsSync(filename)) {
    fs.writeFileSync(filename, 'Timestamp,Magnitud,X,Y,Z,Severidad\n');
  }
  
  // Agregar línea
  fs.appendFileSync(filename, csvLine);
}

// ===== INICIAR SERVIDOR =====
app.listen(PORT, '0.0.0.0', () => {
  console.log('=================================');
  console.log('Servidor de Monitoreo de Impactos');
  console.log('=================================');
  console.log(`✓ Servidor corriendo en http://localhost:${PORT}`);
  console.log(`✓ Dashboard: http://localhost:${PORT}`);
  console.log(`✓ API: http://localhost:${PORT}/api/impacts`);
  console.log('\nEsperando datos del ESP32...\n');
});
