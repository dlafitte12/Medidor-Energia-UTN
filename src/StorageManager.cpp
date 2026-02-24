/**
 * @file StorageManager.cpp
 * @brief Implementation of NVS persistence logic
 */

#include "StorageManager.h"

Preferences prefs;

void initStorage() {
    // Open "consumos" namespace in read-write mode (false)
    prefs.begin("consumos", false);
    prefs.end();
}

float getHistValvula() {
    prefs.begin("consumos", true); // Read-only
    float val = prefs.getFloat("h_valv", 0.0);
    prefs.end();
    return val;
}

float getHistVFD() {
    prefs.begin("consumos", true);
    float val = prefs.getFloat("h_vfd", 0.0);
    prefs.end();
    return val;
}

void saveHistValvula(float value) {
    prefs.begin("consumos", false);
    prefs.putFloat("h_valv", value);
    prefs.end();
}

void saveHistVFD(float value) {
    prefs.begin("consumos", false);
    prefs.putFloat("h_vfd", value);
    prefs.end();
}

void resetStorage() {
    prefs.begin("consumos", false);
    prefs.clear();
    prefs.end();
}