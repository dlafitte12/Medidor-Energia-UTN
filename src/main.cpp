/**
 * @file main.cpp
 * @author Cetraro, Fabricio - Lafitte, Didier
 * @brief Medidor de Energía - Electrónica II - UTN FRRo
 * @version 1.2
 */

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include "config.h" // Separación de configuración según apunte "Herramientas"
#include <Preferences.h> // Librería para persistencia en NVS

Preferences preferences; // Objeto para manejar el almacenamiento persistente

// --- OBJETOS ---
AsyncWebServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);

// --- VARIABLES DE MEDICIÓN (Sistemas Reactivos) ---
float v_rms = 0, i_rms = 0, freq = 0;
float p_activa = 0, q_reactiva = 0, s_aparente = 0, fdp = 0;
float consumo_proyectado = 0;
float hist_valvula = 0, hist_vfd = 0;
bool vfd_active = false;

// Prototipos de funciones (Práctica recomendada en C++)
void handleWiFi();
void handleMQTT();

// --- INTERFAZ WEB (PROGMEM) ---
// [Aquí va el código HTML que ya tenemos definido]
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Medidor de Energía</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: 'Segoe UI', sans-serif; text-align: center; background-color: #eceff1; color: #333; margin: 20px; }
    .card { background: #f5f7f9; padding: 20px; margin: 15px auto; max-width: 500px; border-radius: 12px; box-shadow: 0 4px 10px rgba(0,0,0,0.1); }
    
    /* Ajuste de desplazamiento para centrar el bloque completo */
    .grid { 
      display: grid; 
      grid-template-columns: 1fr 1fr; 
      gap: 10px 40px; /* Aumentamos el espacio central entre columnas */
      padding: 10px;
      justify-content: center; /* Centra el bloque de la grilla en la tarjeta */
      width: fit-content;
      margin: 0 auto; /* Centra el contenedor fit-content */
      text-align: left;
    }

    .value { font-size: 1.2rem; font-weight: bold; color: #1565c0; }
    .unit { font-size: 0.9rem; margin-left: 4px; color: #666; }
    
    .controls-container { display: flex; justify-content: space-around; align-items: center; margin-top: 20px; padding: 10px; border-top: 1px solid #ddd; }
    .control-item { display: flex; flex-direction: column; align-items: center; gap: 8px; font-weight: bold; font-size: 0.9rem; }
    
    .switch { position: relative; display: inline-block; width: 54px; height: 28px; }
    .switch input { opacity: 0; width: 0; height: 0; }
    .slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; transition: .4s; border-radius: 34px; }
    .slider:before { position: absolute; content: ""; height: 20px; width: 20px; left: 4px; bottom: 4px; background-color: white; transition: .4s; border-radius: 50%; }
    input:checked + .slider { background-color: #2196F3; }
    input:checked + .slider:before { transform: translateX(26px); }
    
    .btn-reset { background-color: #d32f2f; color: white; border: none; padding: 12px 25px; border-radius: 8px; cursor: pointer; transition: 0.3s; font-weight: bold; text-transform: uppercase; }
    .btn-reset:hover { background-color: #b71c1c; }
    
    .hist-box { border: 1px solid #b0bec5; background-color: #eceff1; padding: 12px; border-radius: 8px; margin: 10px 0; text-align: center; }
    .vfd-style { background-color: #f1f8e9; border-color: #c5e1a5; }
    
    h2 { color: #1a237e; margin-bottom: 5px; }
    h3 { margin-bottom: 15px; border-bottom: 2px solid #1a237e; padding-bottom: 5px; display: inline-block; }
    h4 { margin: 10px 0; color: #555; text-transform: uppercase; font-size: 0.8rem; letter-spacing: 1px; }
  </style>
  <meta charset="UTF-8">
</head><body>
  <h2>Analizador de consumo energético</h2>
  <p style="margin-top:0; color:#666; font-weight: bold;">Electrónica II - UTN FRRo</p>

  <div class="card">
    <h3>Parámetros Eléctricos</h3>
    <div class="grid">
      <div>V: <span id="v" class="value">0</span> <span class="unit">V</span></div>
      <div>I: <span id="i" class="value">0</span> <span class="unit">A</span></div>
      <div>P: <span id="p" class="value">0</span> <span class="unit">W</span></div>
      <div>Q: <span id="q" class="value">0</span> <span class="unit">VAR</span></div>
      <div>S: <span id="s" class="value">0</span> <span class="unit">VA</span></div>
      <div>f: <span id="f" class="value">0</span> <span class="unit">Hz</span></div>
      <div style="grid-column: span 2; text-align: center; margin-top: 5px;">fdp: <span id="fdp" class="value">0</span></div>
    </div>
  </div>

  <div class="card" style="border-top: 6px solid #1a237e;">
    <h3>Consumo Proyectado</h3>
    <div id="proy" style="font-size: 2.5rem; font-weight: bold; color: #1a237e; margin: 10px 0;">0,00</div>
    <span class="unit" style="font-size: 1.2rem;">kWh</span>

    <h4 style="margin-top:25px;">Consumos Históricos</h4>
    <div class="hist-box">
      <strong>Presión regulada por válvula</strong><br>
      <span id="h_valv" class="value">0,00</span> <span class="unit">kWh</span>
    </div>
    <div class="hist-box vfd-style">
      <strong>Presión regulada por velocidad</strong><br>
      <span id="h_vfd" class="value">0,00</span> <span class="unit">kWh</span>
    </div>

    <div class="controls-container">
      <div class="control-item">
        <span>Regulación por VDF</span>
        <label class="switch">
          <input type="checkbox" id="vfd_switch" onchange="toggleVFD()">
          <span class="slider"></span>
        </label>
      </div>
      <div class="control-item">
        <span>Reset históricos</span>
        <button class="btn-reset" onclick="resetData()">RESET</button>
      </div>
    </div>
  </div>

  <script>
    function format(num) { 
      return parseFloat(num).toLocaleString('es-AR', {minimumFractionDigits: 2, maximumFractionDigits: 2}); 
    }
    
    setInterval(function ( ) {
      fetch('/data').then(response => response.json()).then(data => {
        document.getElementById("v").innerHTML = format(data.v); 
        document.getElementById("i").innerHTML = format(data.i);
        document.getElementById("p").innerHTML = format(data.p); 
        document.getElementById("q").innerHTML = format(data.q);
        document.getElementById("s").innerHTML = format(data.s); 
        document.getElementById("f").innerHTML = format(data.f);
        document.getElementById("fdp").innerHTML = format(data.fdp); 
        document.getElementById("proy").innerHTML = format(data.proy);
        document.getElementById("h_valv").innerHTML = format(data.h_valv); 
        document.getElementById("h_vfd").innerHTML = format(data.h_vfd);
      });
    }, 1000);

    function toggleVFD() { 
      fetch('/vfd?state=' + (document.getElementById("vfd_switch").checked ? 1 : 0)); 
    }
    
    function resetData() { 
      if(confirm("¿Seguro desea borrar los históricos de consumo?")) fetch('/reset'); 
    }
  </script>
</body></html>)rawliteral";

/**
 * @brief Callback MQTT con gestión de memoria optimizada (ArduinoJson v7)
 */
void callback(char* topic, byte* payload, unsigned int length) {
    // Usamos StaticJsonDocument para sistemas restringidos o limitamos el scope
    JsonDocument doc; 
    DeserializationError error = deserializeJson(doc, payload, length);

    if (!error && String(topic) == "shellies/status/em1:0") { 
        v_rms = doc["voltage"];
        p_activa = doc["act_power"];
        i_rms = doc["current"];
        freq = doc["freq"];
        
        // El cálculo se realiza dentro del evento para mantener la reactividad
        s_aparente = v_rms * i_rms; 
        fdp = (s_aparente > 0) ? (p_activa / s_aparente) : 0;
        q_reactiva = (s_aparente > p_activa) ? sqrt(pow(s_aparente, 2) - pow(p_activa, 2)) : 0;
    }
}

/**
 * @brief Rutinas de reconexión automática (Funcionamiento sin interacción humana)
 */
void handleWiFi() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Reconectando WiFi...");
        WiFi.disconnect();
        WiFi.begin(WIFI_SSID, WIFI_PASS);
        long startAttemptTime = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
            delay(500);
        }
    }
}

void handleMQTT() {
    while (!client.connected()) {
        handleWiFi(); // Aseguramos WiFi antes de MQTT
        if (client.connect(DEVICE_ID)) {
            client.subscribe("shellies/status/em1:0");
        } else {
            delay(5000);
        }
    }
}

/**
 * @brief Configuración de rutas del servidor asíncrono
 * Maneja la visualización de datos, control de modos y persistencia en memoria flash.
 */
void setupServerRoutes() {
  // 1. RUTA PRINCIPAL: Entrega la interfaz web almacenada en Flash (PROGMEM)
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){ 
    request->send_P(200, "text/html", index_html); 
  });

  // 2. RUTA DE DATOS: Genera el JSON dinámico para el frontend
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
    // Actualizamos el proyectado al vuelo
    consumo_proyectado = p_activa / 1000.0; 
    
    JsonDocument dataDoc; // Usamos la gestión automática de ArduinoJson v7
    dataDoc["v"] = v_rms;
    dataDoc["i"] = i_rms;
    dataDoc["p"] = p_activa;
    dataDoc["proy"] = consumo_proyectado;
    dataDoc["h_valv"] = hist_valvula;
    dataDoc["h_vfd"] = hist_vfd;
    
    String json;
    serializeJson(dataDoc, json);
    request->send(200, "application/json", json);
  });

  // 3. RUTA DEL SWITCH: Controla el modo y persiste los históricos en NVS
  server.on("/vfd", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("state")) {
      
      // Abrimos el almacenamiento persistente en modo lectura/escritura (false)
      preferences.begin("consumos", false); 
      
      // Lógica de "Captura": Guardamos la foto del proyectado ANTES de cambiar
      if (vfd_active) {
        hist_vfd = p_activa / 1000.0;
        preferences.putFloat("h_vfd", hist_vfd); // Persistencia física del VDF
      } else {
        hist_valvula = p_activa / 1000.0;
        preferences.putFloat("h_valv", hist_valvula); // Persistencia física de la Válvula
      }
      
      preferences.end(); // Cerramos para asegurar la escritura y liberar recursos

      // Actualizamos el modo de trabajo (estado reactivo)
      vfd_active = (request->getParam("state")->value() == "1");
    }
    request->send(200, "text/plain", "OK");
  });

  // 4. RUTA DE RESET: Borra variables y limpia la memoria NVS
  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request){
    hist_valvula = 0; 
    hist_vfd = 0;
    
    preferences.begin("consumos", false);
    preferences.clear(); // Elimina todas las claves bajo el nombre "consumos"
    preferences.end();
    
    request->send(200, "text/plain", "OK");
  });
}

void setup() {
    Serial.begin(115200);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    
// --- RECUPERACIÓN DE PERSISTENCIA ---
    preferences.begin("consumos", true); // Abrimos en modo lectura (true)
    hist_valvula = preferences.getFloat("h_valv", 0.0); // Si no existe, devuelve 0.0
    hist_vfd = preferences.getFloat("h_vfd", 0.0);
    preferences.end();

    setupServerRoutes();
    server.begin();

    client.setServer(MQTT_BROKER, 1883);
    client.setCallback(callback);
}

void loop() {
    // Gestión reactiva de conexiones
    if (!client.connected()) {
        handleMQTT();
    }
    client.loop();

    // Tarea de fondo: Actualización de proyección
    consumo_proyectado = p_activa / 1000.0;
}