#include "SettingsManager.h"

SettingsManager::SettingsManager()
: dataStore()
{
}

void SettingsManager::settingsChangeCallback(SettingsChanged settingsChanged)
  {
    this->settingsChanged = settingsChanged;
  }


CockpitControlSettings SettingsManager::resetSettings()
{
    CockpitControlSettings defaultSettings;
    this->writeSettings(defaultSettings);

    if (this->settingsChanged != nullptr)
    {
        this->settingsChanged(defaultSettings);
    }

    return defaultSettings;
}

CockpitControlSettings SettingsManager::writeSettings(CockpitControlSettings settings)
{

    dataStore.putBool(settings.autoSendDataAddress, settings.autoSendData);
    dataStore.putUInt(settings.defaultPosAddress, settings.defaultPos);
    dataStore.putUInt(settings.stepCountAddress, settings.stepCount);

    for (size_t i = 0; i < settings.stepCount; i++)
     {
        const char* temp = String(settings.stepsStartAddress + String(i)).c_str();
         dataStore.putUInt(temp, settings.steps[i]);
     }

     if (this->settingsChanged != nullptr)
     {
         this->settingsChanged(settings);
     }

    return settings;
}

CockpitControlSettings SettingsManager::loadSettings()
{
    
    CockpitControlSettings settings;
  //  LOGD_INFO("SettingsManager::loadSettings1");
    settings.settingsStored = dataStore.getBool(settings.settingsStoredAddress);

    if (!settings.settingsStored)
    {
        writeSettings(settings);
        dataStore.putBool(settings.settingsStoredAddress, true);
        settings.settingsStored = true;
    } else {
        settings.autoSendData = dataStore.getBool(settings.autoSendDataAddress);
        settings.defaultPos = dataStore.getUInt(settings.defaultPosAddress);
        settings.stepCount = dataStore.getUInt(settings.stepCountAddress);

        settings.steps = new uint16_t[settings.stepCount]();
        for (size_t i = 0; i < settings.stepCount; i++)
        {
            const char* temp = String(settings.stepsStartAddress + String(i)).c_str();
            settings.steps[i] = dataStore.getUInt(temp);
        }
        
    }

  //  LOGD_INFO("SettingsManager::loadSettings2");
    if (this->settingsChanged != nullptr)
    {
        this->settingsChanged(settings);
    }
 //   LOGD_INFO("SettingsManager::loadSettings3");
    
    return settings;
}