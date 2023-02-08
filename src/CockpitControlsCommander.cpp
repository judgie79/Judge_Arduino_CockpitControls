#include "CockpitControlsCommander.h"

extern String buildStepList(uint16_t *steps, uint16_t stepCount)
{
	String tValue = "";
	for (size_t i = 0; i < stepCount; i++)
	{
		uint16_t distance = steps[i];

		if (i < (stepCount - 1))
		{
			tValue += String(distance) + CommandValueSeparator;
		}
		else {
			tValue += String(distance);
		}
	}

	return tValue;
}

CockpitControlsCommander::CockpitControlsCommander(MotorManager *motorManager, InputManager *inputManager, SettingsManager* settingsManager) : Commander()
{
	this->motorManager = motorManager;
	this->inputManager = inputManager;
	this->settingsManager = settingsManager;
}

void CockpitControlsCommander::readCmd(char key, String data)
{        
	switch (key)
	{
	case (char)CockpitControlsCommandKey::MOTORSPEED:
		if (isConnected())
		{
			onMotorSpeedCommand(data);
		}
		break;

	case (char)CockpitControlsCommandKey::GETPOSITIONS:

		if (isConnected())
		{
			onGetPositionsCommand(data);
		}
		break;

	case (char)CockpitControlsCommandKey::SETPOSITIONS:
		if (isConnected())
		{
			onSetPositionsCommand(data);
		}
		break;

	case (char)CockpitControlsCommandKey::GOTOPOSITION:
		if (isConnected())
		{
			onGotoPositionCommand(data);
		}
		break;

	case (char)CockpitControlsCommandKey::DATA:
		if (isConnected())
		{
			onGetDataCommand(data);
		}
		break;

	case (char)CockpitControlsCommandKey::SETSETTINGS:
		if (isConnected())
		{
			onSetSettingsCommand(data);
		}
		break;

	case (char)CockpitControlsCommandKey::GETSETTINGS:
		if (isConnected())
		{
			onGetSettingsCommand(data);
		}
		break;


	case (char)CockpitControlsCommandKey::RESETSETTINGS:
		if (isConnected())
		{
			onResetSettingsCommand(data);
		}
		break;
	default:
		Commander::readCmd(key, data);
		break;
	}
}

void CockpitControlsCommander::onMotorSpeedCommand(String value)
{
	motorManager->setSpeed(value.toInt());
	serialCmd->writeCommand(CommandKey::RECEIVED, CommandToken);
}

void CockpitControlsCommander::onGetPositionsCommand(String value)
{

	String tValue = buildStepList(motorManager->getSteps(), motorManager->getStepCount());

	serialCmd->writeCommand((char)CockpitControlsCommandKey::GETPOSITIONS, tValue);
}

void CockpitControlsCommander::onGetDataCommand(String value)
{
	sendData();
}

void CockpitControlsCommander::sendData()
{
	String data =
		String(motorManager->currentDistance()) + String(CommandValueSeparator) +
		String((int8_t)motorManager->currentDirection()) + String(CommandValueSeparator) +
		String(motorManager->getSpeed()) + String(CommandValueSeparator) +
		String(inputManager->GetButtonMode());

	serialCmd->writeCommand((char)CockpitControlsCommandKey::DATA, data);
}

void CockpitControlsCommander::sendEvent(CockpitControlsEvents event, String eventData)
{
	String data = String((uint8_t)event, DEC) + String(CommandValueSeparator) + eventData;
	serialCmd->writeCommand((char)CockpitControlsCommandKey::EVENT, data);
}

void CockpitControlsCommander::onGotoPositionCommand(String value)
{
	motorManager->gotoPos(value.toInt());
	serialCmd->writeCommand(CommandKey::RECEIVED, CommandToken);
}

void CockpitControlsCommander::onSetPositionsCommand(String value)
{
	uint16_t newPosLength = getNewPositionLength(value);

	uint16_t *newDistances = new uint16_t[newPosLength];

	for (size_t i = 1; i <= newPosLength; i++)
	{
		newDistances[i - 1] = getValue(value, CommandValueSeparator, i).toInt();
	}

	CockpitControlSettings settings = this->settingsManager->loadSettings();
	settings.stepCount = newPosLength;
	settings.steps = newDistances;

	CockpitControlSettings storedSettings = this->settingsManager->writeSettings(settings);
	motorManager->setSteps(storedSettings.steps, storedSettings.stepCount);
	// write back
	String tValue = buildStepList(motorManager->getSteps(), motorManager->getStepCount());

	serialCmd->writeCommand((char)CockpitControlsCommandKey::GETPOSITIONS, tValue);
}

void CockpitControlsCommander::onSetSettingsCommand(String value)
{
	bool autoSendData = getValue(value, CommandValueSeparator, 0).toInt() == 1 ? true : false;
	uint8_t defaultPos = getValue(value, CommandValueSeparator, 1).toInt();

	CockpitControlSettings settings = this->settingsManager->loadSettings();
	settings.autoSendData = autoSendData;
	settings.defaultPos = defaultPos;
	
	CockpitControlSettings storedSettings = this->settingsManager->writeSettings(settings);
	motorManager->begin(storedSettings.steps, storedSettings.stepCount);
	// write back
	serialCmd->writeCommand(CommandKey::RECEIVED, CommandToken);
}

void CockpitControlsCommander::onGetSettingsCommand(String value)
{
	CockpitControlSettings settings = this->settingsManager->loadSettings();
	// write back
	serialCmd->writeCommand((char)CockpitControlsCommandKey::GETSETTINGS, String(settings.autoSendData) + String(CommandValueSeparator) + String(settings.defaultPos));
}

void CockpitControlsCommander::onResetSettingsCommand(String value)
{
	// reset settings
	CockpitControlSettings storedSettings = this->settingsManager->resetSettings();
	motorManager->begin(storedSettings.steps, storedSettings.stepCount);
	// write back
	serialCmd->writeCommand(CommandKey::RECEIVED, CommandToken);
}


uint8_t CockpitControlsCommander::getNewPositionLength(String value)
{
	return getValue(value, CommandValueSeparator, 0).toInt();
}
