#include "SettingsManager.h"


uint16_t settingsMap(const char * key)
{
    if (strcmp(key, "cockpit") == 0)
    {
        return 0;
    } else if (strcmp(key,  "autoSendData") == 0)
    {
        return 1;
    } else if (strcmp(key, "defaultPos") == 0)
    {
        return 2;
    } else if (strcmp(key,  "stepCount") == 0)
    {
        return 3;
    } else if (strcmp(key, "stepsStart") == 0)
    {
        return 4;
    }

    return -1;
}
#if defined(ESP32)
SettingsManager::SettingsManager()
: dataStore("Wind")
{
    
}
#else
SettingsManager::SettingsManager()
: dataStore(&settingsMap)
{
    
}
#endif

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
    dataStore.begin(false);
    dataStore.writeBool(settings.autoSendDataAddress, settings.autoSendData);
    dataStore.writeBool(settings.defaultPosAddress, settings.defaultPos);
    dataStore.writeBool(settings.stepCountAddress, settings.stepCount);

    settings.autoSendData = dataStore.readBool(settings.autoSendDataAddress);
    settings.defaultPos = dataStore.readBool(settings.defaultPosAddress); 
    settings.stepCount = dataStore.readBool(settings.stepCountAddress); 

    for (size_t i = 0; i < settings.stepCount; i++)
     {
        const char* temp = String(settings.stepsStartAddress + String(i)).c_str();
         dataStore.writeUShort(temp, settings.steps[i]);
     }
     dataStore.end();

     if (this->settingsChanged != nullptr)
     {
         this->settingsChanged(settings);
     }

    return settings;
}

CockpitControlSettings SettingsManager::loadSettings()
{
    
    CockpitControlSettings settings;

    dataStore.begin(true);
    settings.settingsStored = dataStore.readBool(settings.settingsStoredAddress);
    dataStore.end();

    if (!settings.settingsStored)
    {
        writeSettings(settings);
        dataStore.begin(false);
        dataStore.writeBool(settings.settingsStoredAddress, true);
        settings.settingsStored = true;
        dataStore.end();
    } else {
            dataStore.begin(true);
        settings.autoSendData = dataStore.readBool(settings.autoSendDataAddress);
        settings.defaultPos = dataStore.readInt(settings.defaultPosAddress);
        settings.stepCount = dataStore.readUInt(settings.stepCountAddress);

        settings.steps = new uint8_t[settings.stepCount]();
        for (size_t i = 0; i < settings.stepCount; i++)
        {
            settings.steps[i] = dataStore.readUInt(settings.stepsStartAddress + i);
        }
        dataStore.end();
        
    }

  //  LOGD_INFO("SettingsManager::loadSettings2");
    if (this->settingsChanged != nullptr)
    {
        this->settingsChanged(settings);
    }
 //   LOGD_INFO("SettingsManager::loadSettings3");
    
    return settings;
}