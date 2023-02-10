#ifndef __SETTINGSMANAGER_H
#define __SETTINGSMANAGER_H
#include "Arduino.h"
#include <DebugOut.h>
#if defined(ESP32)
#include "PreferencesDataStore.h"
#else
#include "EEPROMDataStore.h"
#endif
#include "CockpitControlSettings.h"

typedef void(*SettingsChanged) (CockpitControlSettings);

class SettingsManager
{
private:
    /* data */
#if defined(ESP32)
    PreferencesDataStore dataStore;
#else
    EEPROMDataStore dataStore;
#endif
    SettingsChanged settingsChanged;
public:
    SettingsManager();

    CockpitControlSettings loadSettings();

    CockpitControlSettings writeSettings(CockpitControlSettings settings);

    CockpitControlSettings resetSettings();

    void settingsChangeCallback(SettingsChanged changedcallback);
};

#endif