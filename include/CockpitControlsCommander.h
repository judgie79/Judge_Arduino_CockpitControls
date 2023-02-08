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
	void sendEvent(CockpitControlsEvents event, String eventData);
protected:

	void readCmd(char key, String data);
private:
	MotorManager* motorManager;
	InputManager* inputManager;
	SettingsManager* settingsManager;

	void onMotorSpeedCommand(String value);

	void onGetPositionsCommand(String value);

	void onGotoPositionCommand(String value);

	void onSetPositionsCommand(String value);

	void onSetSettingsCommand(String value);

	void onGetSettingsCommand(String value);

	void onResetSettingsCommand(String value);

	void onGetDataCommand(String value);

	uint8_t getNewPositionLength(String value);
};

#endif

