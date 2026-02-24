/**
 * @file MqttHandler.h
 * @brief MQTT communication and JSON parsing module
 */

#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <PubSubClient.h>
#include <ArduinoJson.h>

// Initialize MQTT client and set callback
void initMQTT(PubSubClient& client);

// Handle connection and subscription logic
void handleMQTTConnection(PubSubClient& client);

// Getters for processed electrical data
float getVrms();
float getIrms();
float getPactiva();
float getFreq();
float getSapa();
float getQreac();
float getFDP();

#endif