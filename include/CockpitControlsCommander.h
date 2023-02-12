// CockpitControlsCommander.h

#ifndef _COCKPITCONTROLSCOMMANDER_h
#define _COCKPITCONTROLSCOMMANDER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif
#include <Commander.h>
#include <CommandStream.h>
#include <Commands.h>
#include <MotorManager.h>
#include "CockpitControlCommands.h"
#include "InputManager.h"
#include "SettingsManager.h"

class CockpitControlsCommander : public Commander
{
public:

	CockpitControlsCommander(MotorManager* motorManager, InputManager* inputManager, SettingsManager* settingsManager);
	void sendData();
	void sendEvent(CockpitControlsEvents event, const char * eventData);
protected:

	void readCmd(char key, const char * data, uint8_t valueLength);
private:
	MotorManager* motorManager;
	InputManager* inputManager;
	SettingsManager* settingsManager;

	void onMotorSpeedCommand(const char * value, uint8_t valueLength);

	void onGetPositionsCommand(const char * value, uint8_t valueLength);

	void onGotoPositionCommand(const char * value, uint8_t valueLength);

	void onSetPositionsCommand(const char * value, uint8_t valueLength);

	void onSetSettingsCommand(const char * value, uint8_t valueLength);

	void onGetSettingsCommand(const char * value, uint8_t valueLength);

	void onResetSettingsCommand(const char * value, uint8_t valueLength);

	void onGetDataCommand(const char * value, uint8_t valueLength);

	uint8_t getNewPositionLength(const char * value, uint8_t valueLength);
};

#endif

