
#define VERSION "1"

#define DEBUGLOG_DEFAULT_LOG_LEVEL_TRACE
#define LOGD_PRINT 1

#include <DebugLog.h>
#include <DebugLogEnable.h>
#include <DebugOut.h>

#include <Wire.h>

#include <DebouncedButton.h>
#include <JoystickButton.h>
#include <TriggerSensor.h>
#include <DirectionButtonSensor.h>
#include <DistanceSensor.h>
#include <MotorManager.h>
#include <SonarDistanceDevice.h>
#include <CommandStream.h>
#include <Commander.h>
#include "CockpitControlCommands.h"
#include "CockpitControlsCommander.h"
//#include "InputManager.h"
#include "SettingsManager.h"
#include "CockpitControlSettings.h"

/*
    BUTTONS
*/
#define forwardButtonPin 2    // the number of the forward pushbutton pin
#define reverseButtonPin 3    // the number of the reverse pushbutton pin
#define modeSelectButtonPin 4 // the number of the modeselect pin

#define pos1ButtonPin 5 // the number of the pos1 pushbutton pin
#define pos2ButtonPin 6 // the number of the pos2 pushbutton pin

bool motorInitialized = false;
bool handleInputManager = false;

DebouncedButton modeSelectButton(modeSelectButtonPin, INPUT_PULLUP);
DebouncedButton forwardButton(forwardButtonPin, INPUT_PULLUP);
DebouncedButton reverseButton(reverseButtonPin, INPUT_PULLUP);

DebouncedButton pos1Button(pos1ButtonPin, INPUT_PULLUP);
DebouncedButton pos2Button(pos2ButtonPin, INPUT_PULLUP);

/*
    DISTANCE MEASURE
*/
#define distanceEchoPin 12    // the number of the echo pin connected to the distance sensor
#define distanceTriggerPin 13 // the number of the trigger pin connected to the distance sensor
SonarDistanceDevice sonar(distanceTriggerPin, distanceEchoPin, 30);

/*
    SENSORS
*/
#define backEndSensorPin 11 // the number of the pushbutton pin of the backend sensor
#define frontEndSensorPin 7 // the number of the pushbutton pin of the front end sensor
DirectionButtonSensor backEndSensor(0, "START", SensorType::Trigger, SensorTriggerType::Force, SensorTriggerDirection::Backward, backEndSensorPin, INPUT_PULLUP);
DirectionButtonSensor frontEndSensor(1, "END", SensorType::Trigger, SensorTriggerType::Force, SensorTriggerDirection::Forward, frontEndSensorPin, INPUT_PULLUP);

DirectionTriggerSensor **defaultSensors = new DirectionTriggerSensor *[2]
{
  &backEndSensor, &frontEndSensor
};

#define R_EN 17  // the number of the R_EN pin connected to the motor
#define L_EN 18  // the number of the L_EN pin connected to the motor

#define R_PWM 36  // the number of the R_PWM pin connected to the motor
#define L_PWM 37 // the number of the L_PWM pin connected to the motor

//SensorManager sManager(defaultSensors, 3, &sonar);
MotorManager mManager(L_EN, R_EN, L_PWM, R_PWM, defaultSensors, 2, &sonar);

//uint16_t steps[2] = { 7, 15 };
//const int defaultPos = 0;
SettingsManager settingsManager;
CockpitControlSettings settings;

/*
    DIRECT INPUT CONTROLS
*/
InputManager inputManager(&mManager, &modeSelectButton, &forwardButton, &reverseButton, &pos1Button, &pos2Button);

CommandStream cmdStream(new char[14]{
                      (char)CommandKey::HELLO,
                      (char)CommandKey::ALREADY_CONNECTED,
                      (char)CommandKey::DEBUG,
                      (char)CommandKey::ERROR,
                      (char)CommandKey::RECEIVED,
                      (char)CommandKey::BYE,
                      (char)CockpitControlsCommandKey::DATA,
                      (char)CockpitControlsCommandKey::GETPOSITIONS,
                      (char)CockpitControlsCommandKey::SETPOSITIONS,
                      (char)CockpitControlsCommandKey::GOTOPOSITION,
                      (char)CockpitControlsCommandKey::MOTORSPEED,
                      (char)CockpitControlsCommandKey::RESETSETTINGS,
                      (char)CockpitControlsCommandKey::GETSETTINGS,
                      (char)CockpitControlsCommandKey::SETSETTINGS
                      },
                  14);
/*
    COMMANDS FROM SERIAL BUS
*/
CockpitControlsCommander commander(&mManager, &inputManager, &settingsManager);

#define DATA_HZ 1000 / 60

long lastDataSend = 0;

void onPositionReached(uint16_t index);
void onSensorReached(DirectionTriggerSensor *sensor);


void initPedalPosition()
{
  
  mManager.read();
  if (!mManager.initPositions())
  {
    return;
  }
  else
  {
    if (!motorInitialized)
    {
      motorInitialized = true;
      LOGD_INFO("motor initialized");
    }
  }
}

void onModeSelect(bool isSelected)
{
  handleInputManager = !isSelected;
}

bool autoSendData = false;
void onSettingsChange(CockpitControlSettings settings)
{
  autoSendData = settings.autoSendData;
}

void setup()
{
  Serial.begin(115200);
  delay(2000);
  settings = settingsManager.loadSettings();
  mManager.begin(settings.steps, settings.stepCount);

  while(!motorInitialized)
  {
    initPedalPosition();
  }
  cmdStream.setOutput(&Serial);
   inputManager.Begin(&cmdStream);
   commander.begin(&cmdStream);
   mManager.addPositionListener(&onPositionReached);
   mManager.addSensorListener(&onSensorReached);

   modeSelectButton.setChangeCallback(&onModeSelect);

  settingsManager.settingsChangeCallback(&onSettingsChange);
  LOGD_INFO("ready");
}

void onPositionReached(uint16_t index)
{
  LOGD_INFO("pos reached");
  commander.sendEvent(CockpitControlsEvents::POS_REACHED, String(index, DEC));
}

void onSensorReached(DirectionTriggerSensor *sensor)
{
  if (sensor->getType() == SensorType::Trigger)
  {
    if (sensor->getTriggerType() == SensorTriggerType::Force)
    {
      if (sensor->getTriggerDirection() == SensorTriggerDirection::Forward)
      {
        commander.sendEvent(CockpitControlsEvents::FRONT_REACHED, String(sensor->getId(), DEC));
      } else if (sensor->getTriggerDirection() == SensorTriggerDirection::Backward)
      {
        commander.sendEvent(CockpitControlsEvents::BACK_REACHED, String(sensor->getId(), DEC));
      }
    }
  }
}

void loop()
{
    commander.read();
  mManager.read();
  inputManager.Read();

  if (handleInputManager)
  {
    inputManager.HandleInputs();
  }
  else
  {
     mManager.move();
  }
  if (commander.isConnected())
  {
    if ((millis() - lastDataSend >= DATA_HZ) && autoSendData)
     {
       commander.sendData();

       lastDataSend = millis();
     }
   }
}







// // /*
// //     DATASTORE AND SETTINGS
// // */
// //EEPROMDataStore dataStore;







// #define DATA_HZ 1000 / 60

// long lastDataSend = 0;

// void setup()
// {
//    Serial.begin(115200);
//     delay(2000);
//     LOGD_INFO("Starting");
//     //DebugOut.setLogLevel(LogLevel::Debug);
//     //DebugOut.setOutput(&Serial);
//     //DebugOut.enable();
//     // put your setup code here, to run once:
//   //  dataStore.begin();
//     // modeSelectButton.setChangeCallback(&onModeSelect);

//   settingsManager.settingsChangeCallback(&onSettingsChange);

//   CockpitControlSettings loadedSettings = settingsManager.loadSettings();
//   mManager.begin(loadedSettings.steps, loadedSettings.stepCount);
//   //  cmdStream.setOutput(&Serial);
//   //  inputManager.Begin(&cmdStream);
//   //  commander.begin(&cmdStream);
//     LOGD_INFO("Started");
// }

// void loop()
// {
//   if (!motorInitialized)
//   {
//     initPedalPosition();
//     return;
//   }

//   // commander.read();
//   mManager.read();
//   // inputManager.Read();

//   // if (handleInputManager)
//   // {
//   //   inputManager.HandleInputs();
//   // }
//   // else
//   // {
//      mManager.move();
//   // }
//   // if (commander.isConnected() && autoSendData)
//   // {
//   //   if (millis() - lastDataSend >= DATA_HZ)
//   //   {
//   //     // send status
//   //     String data =
//   //         String(CommandValueSeparator) +
//   //         String(CommandValueSeparator) +
//   //         String(CommandValueSeparator) +
//   //         String(inputManager.GetButtonMode());

//   //     cmdStream.writeCommand((char)CockpitControlsCommandKey::DATA, data);

//   //     lastDataSend = millis();
//   //   }
//   // }
// }


