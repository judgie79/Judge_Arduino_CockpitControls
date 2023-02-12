#include "CockpitControlsCommander.h"

extern const char * buildStepList(uint8_t *steps, uint8_t stepCount)
{
	String tValue = "";
	for (size_t i = 0; i < stepCount; i++)
	{
		String distance = String(steps[i]);

		if (i < (uint8_t)(stepCount - (uint8_t)i))
		{
			tValue += distance + CommandValueSeparator;
		}
		else {
			tValue += distance;
		}
	}

	return tValue.c_str();
}

CockpitControlsCommander::CockpitControlsCommander(MotorManager *motorManager, InputManager *inputManager, SettingsManager* settingsManager) : Commander()
{
	this->motorManager = motorManager;
	this->inputManager = inputManager;
	this->settingsManager = settingsManager;
}

void CockpitControlsCommander::readCmd(char key, const char * data, uint8_t valueLength)
{        
	switch (key)
	{
	case (char)CockpitControlsCommandKey::MOTORSPEED:
		if (isConnected())
		{
			onMotorSpeedCommand(data, valueLength);
		}
		break;

	case (char)CockpitControlsCommandKey::GETPOSITIONS:

		if (isConnected())
		{
			onGetPositionsCommand(data, valueLength);
		}
		break;

	case (char)CockpitControlsCommandKey::SETPOSITIONS:
		if (isConnected())
		{
			onSetPositionsCommand(data, valueLength);
		}
		break;

	case (char)CockpitControlsCommandKey::GOTOPOSITION:
		if (isConnected())
		{
			onGotoPositionCommand(data, valueLength);
		}
		break;

	case (char)CockpitControlsCommandKey::DATA:
		if (isConnected())
		{
			onGetDataCommand(data, valueLength);
		}
		break;

	case (char)CockpitControlsCommandKey::SETSETTINGS:
		if (isConnected())
		{
			onSetSettingsCommand(data, valueLength);
		}
		break;

	case (char)CockpitControlsCommandKey::GETSETTINGS:
		if (isConnected())
		{
			onGetSettingsCommand(data, valueLength);
		}
		break;


	case (char)CockpitControlsCommandKey::RESETSETTINGS:
		if (isConnected())
		{
			onResetSettingsCommand(data, valueLength);
		}
		break;
	default:
		Commander::readCmd(key, data);
		break;
	}
}

void CockpitControlsCommander::onMotorSpeedCommand(const char * value, uint8_t valueLength)
{
	motorManager->setSpeed(static_cast<unsigned int>( *value ));
	serialCmd->writeCommand(CommandKey::RECEIVED, CommandToken.c_str());
}

void CockpitControlsCommander::onGetPositionsCommand(const char * value, uint8_t valueLength)
{
	serialCmd->writeCommand((char)CockpitControlsCommandKey::GETPOSITIONS, buildStepList(motorManager->getSteps(), motorManager->getStepCount()));
}

void CockpitControlsCommander::onGetDataCommand(const char * value, uint8_t valueLength)
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

	serialCmd->writeCommand((char)CockpitControlsCommandKey::DATA, data.c_str());
}

void CockpitControlsCommander::sendEvent(CockpitControlsEvents event, const char * eventData)
{
	String data = String((uint8_t)event, DEC) + String(CommandValueSeparator) + eventData;
	serialCmd->writeCommand((char)CockpitControlsCommandKey::EVENT, data.c_str());
}

void CockpitControlsCommander::onGotoPositionCommand(const char *  value, uint8_t valueLength)
{
	motorManager->gotoPos(static_cast<unsigned int>( *value ));
	serialCmd->writeCommand(CommandKey::RECEIVED, CommandToken.c_str());
}

void CockpitControlsCommander::onSetPositionsCommand(const char *  value, uint8_t valueLength)
{
	uint8_t newPosLength = getNewPositionLength(value, valueLength);

	uint8_t *newDistances = new uint8_t[newPosLength];

	for (size_t i = 1; i <= newPosLength; i++)
	{
		newDistances[i - 1] = getValue(value, valueLength, CommandValueSeparator, i).toInt();
	}

	CockpitControlSettings settings = this->settingsManager->loadSettings();
	settings.stepCount = newPosLength;
	settings.steps = newDistances;

	CockpitControlSettings storedSettings = this->settingsManager->writeSettings(settings);
	motorManager->setSteps(storedSettings.steps, storedSettings.stepCount);

	serialCmd->writeCommand((char)CockpitControlsCommandKey::GETPOSITIONS, buildStepList(motorManager->getSteps(), motorManager->getStepCount()));
}

void CockpitControlsCommander::onSetSettingsCommand(const char *  value, uint8_t valueLength)
{
	bool autoSendData = getValue(value, valueLength, CommandValueSeparator, 0).toInt() == 1 ? true : false;
	uint8_t defaultPos = getValue(value, valueLength,  CommandValueSeparator, 1).toInt();

	CockpitControlSettings settings = this->settingsManager->loadSettings();
	settings.autoSendData = autoSendData;
	settings.defaultPos = defaultPos;
	
	CockpitControlSettings storedSettings = this->settingsManager->writeSettings(settings);
	motorManager->begin(storedSettings.steps, storedSettings.stepCount);
	// write back
	serialCmd->writeCommand(CommandKey::RECEIVED, CommandToken.c_str());
}

void CockpitControlsCommander::onGetSettingsCommand(const char *  value, uint8_t valueLength)
{
	CockpitControlSettings settings = this->settingsManager->loadSettings();
	// write back
	char temp[3] = "";
	temp[0] = settings.autoSendData;
	temp[1] = CommandValueSeparator;
	temp[2] = settings.defaultPos;
	serialCmd->writeCommand((char)CockpitControlsCommandKey::GETSETTINGS, temp);
}

void CockpitControlsCommander::onResetSettingsCommand(const char *  value, uint8_t valueLength)
{
	// reset settings
	CockpitControlSettings storedSettings = this->settingsManager->resetSettings();
	motorManager->begin(storedSettings.steps, storedSettings.stepCount);
	// write back
	serialCmd->writeCommand(CommandKey::RECEIVED, CommandToken.c_str());
}


uint8_t CockpitControlsCommander::getNewPositionLength(const char *  value, uint8_t valueLength)
{
	return getValue(value, valueLength, CommandValueSeparator, 0).toInt();
}
