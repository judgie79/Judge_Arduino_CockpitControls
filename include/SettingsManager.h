#ifndef __SETTINGSMANAGER_H
#define __SETTINGSMANAGER_H
#include "Arduino.h"
#include <DebugOut.h>
#include <Preferences.h>
#include "CockpitControlSettings.h"



typedef void(*SettingsChanged) (CockpitControlSettings);

class SettingsManager
{
private:
    /* data */
    Preferences dataStore;
    SettingsChanged settingsChanged;
public:
    SettingsManager();

    CockpitControlSettings loadSettings();

    CockpitControlSettings writeSettings(CockpitControlSettings settings);

    CockpitControlSettings resetSettings();

    void settingsChangeCallback(SettingsChanged changedcallback);
};

#endif