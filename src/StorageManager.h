/**
 * @file StorageManager.h
 * @brief NVS Management for historical energy data
 */

#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include <Preferences.h>

// Initialize NVS storage
void initStorage();

// Getters for historical data
float getHistValvula();
float getHistVFD();

// Setters to persist data in Flash
void saveHistValvula(float value);
void saveHistVFD(float value);

// Clear all historical data
void resetStorage();

#endif