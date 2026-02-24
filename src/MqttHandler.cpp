/**
 * @file MqttHandler.cpp
 * @brief Implementation of MQTT logic and power calculations
 */

#include "MqttHandler.h"
#include "config.h"
#include <Arduino.h>

// Internal variables
float v_rms = 0, i_rms = 0, p_activa = 0, freq = 0;
float s_aparente = 0, q_reactiva = 0, fdp = 0;

// Internal callback to process JSON from Shelly EM
void mqttCallback(char* topic, byte* payload, unsigned int length) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload, length);

    if (!error && String(topic) == "shellies/status/em1:0") {
        v_rms = doc["voltage"];
        p_activa = doc["act_power"];
        i_rms = doc["current"];
        freq = doc["freq"];

        // Power math calculations
        s_aparente = v_rms * i_rms;
        fdp = (s_aparente > 0) ? (p_activa / s_aparente) : 0;
        q_reactiva = (s_aparente > p_activa) ? sqrt(pow(s_aparente, 2) - pow(p_activa, 2)) : 0;
    }
}

void initMQTT(PubSubClient& client) {
    client.setServer(MQTT_BROKER, 1883);
    client.setCallback(mqttCallback);
}

void handleMQTTConnection(PubSubClient& client) {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (client.connect(DEVICE_ID)) {
            Serial.println("connected");
            client.subscribe("shellies/status/em1:0");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            delay(5000);
        }
    }
}

// Implementation of Getters
float getVrms() { return v_rms; }
float getIrms() { return i_rms; }
float getPactiva() { return p_activa; }
float getFreq() { return freq; }
float getSapa() { return s_aparente; }
float getQreac() { return q_reactiva; }
float getFDP() { return fdp; }