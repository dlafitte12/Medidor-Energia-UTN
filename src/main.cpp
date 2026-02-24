/**
 * @file main.cpp
 * @author Cetraro, Fabricio - Lafitte, Didier
 * @brief Energy Meter - Final Modular Version
 * @details Electronics II - UTN FRRo. This main file orchestrates 
 * the NVS storage, MQTT communication, and the Asynchronous Web Server.
 */

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>

// Project Modules
#include "config.h"
#include "index_h.h"
#include "StorageManager.h"
#include "MqttHandler.h"

// --- NETWORK OBJECTS ---
AsyncWebServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);

// --- LOCAL STATE VARIABLES ---
float hist_valvula = 0, hist_vfd = 0;
float consumo_proyectado = 0;
bool vfd_active = false;

// Function Prototypes
void handleWiFi();

/**
 * @brief Web Server Route Definitions
 * Encapsulates the HMI logic and interaction with StorageManager and MqttHandler.
 */
void setupServerRoutes() {
    // Main Landing Page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){ 
        request->send_P(200, "text/html", index_html); 
    });

    // Data Endpoint: Requests fresh data from the MqttHandler module
    server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
        consumo_proyectado = getPactiva() / 1000.0;
        
        JsonDocument dataDoc;
        dataDoc["v"] = getVrms();
        dataDoc["i"] = getIrms();
        dataDoc["p"] = getPactiva();
        dataDoc["f"] = getFreq();
        dataDoc["s"] = getSapa();
        dataDoc["q"] = getQreac();
        dataDoc["fdp"] = getFDP();
        dataDoc["proy"] = consumo_proyectado;
        dataDoc["h_valv"] = hist_valvula;
        dataDoc["h_vfd"] = hist_vfd;
        
        String json;
        serializeJson(dataDoc, json);
        request->send(200, "application/json", json);
    });

    // Control Endpoint: Updates mode and persists data in NVS via StorageManager
    server.on("/vfd", HTTP_GET, [](AsyncWebServerRequest *request){
        if (request->hasParam("state")) {
            if (vfd_active) {
                hist_vfd = getPactiva() / 1000.0;
                saveHistVFD(hist_vfd); 
            } else {
                hist_valvula = getPactiva() / 1000.0;
                saveHistValvula(hist_valvula);
            }
            vfd_active = (request->getParam("state")->value() == "1");
        }
        request->send(200, "text/plain", "OK");
    });

    // Reset Endpoint: Wipes variables and NVS storage
    server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request){
        hist_valvula = 0; hist_vfd = 0;
        resetStorage();
        request->send(200, "text/plain", "OK");
    });
}

void handleWiFi() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi Link Down. Reconnecting...");
        WiFi.disconnect();
        WiFi.begin(WIFI_SSID, WIFI_PASS);
        unsigned long start = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
            delay(500);
        }
    }
}

void setup() {
    Serial.begin(115200);
    
    // 1. Initialize NVS and Recover Historical Data
    initStorage();
    hist_valvula = getHistValvula();
    hist_vfd = getHistVFD();

    // 2. Setup Network and Web HMI
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    setupServerRoutes();
    server.begin();

    // 3. Initialize MQTT via Module
    initMQTT(client);
    
    Serial.println("System Ready - Listening for Sensor Data");
}

void loop() {
    // Maintain connections in a non-blocking way
    if (!client.connected()) {
        handleWiFi();
        handleMQTTConnection(client);
    }
    client.loop();
    
    // Periodic background calculations
    consumo_proyectado = getPactiva() / 1000.0;
}