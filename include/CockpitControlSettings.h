#ifndef __COCKPITCONTROLSETTINGS_h
#define __COCKPITCONTROLSETTINGS_h

class CockpitControlSettings
{
    public:
        CockpitControlSettings()
        {
        }

        bool settingsStored = false;
        bool autoSendData = false;
        uint8_t defaultPos = 0;
        uint8_t stepCount = 2;

        uint8_t *steps = new uint8_t[stepCount]{7, 15};

        const char*   settingsStoredAddress = "cockpit";
        const char*   autoSendDataAddress = "autoSendData";
        const char* defaultPosAddress = "defaultPos";
        const char* stepCountAddress = "stepCount";
        const char* stepsStartAddress = "stepsStart";
};

#endif