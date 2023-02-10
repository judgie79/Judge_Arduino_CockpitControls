#include "InputManager.h"

InputManager::InputManager(MotorManager *motorManager,
    DebouncedButton *modeSelectButton, DebouncedButton *forwardButton, DebouncedButton *reverseButton, 
    DebouncedButton *pos1Button, DebouncedButton *pos2Button)
{  
    this->motorManager = motorManager;
    this->modeSelectButton = modeSelectButton;
    this->forwardButton = forwardButton;
    this->reverseButton = reverseButton;
    this->pos1Button = pos1Button;
    this->pos2Button = pos2Button;
}

InputManager::InputManager(MotorManager *motorManager, UniversalGamepad *gamepad,
    DebouncedButton *modeSelectButton, DebouncedButton *forwardButton, DebouncedButton *reverseButton, 
    DebouncedButton *pos1Button, DebouncedButton *pos2Button)
    : InputManager(motorManager, modeSelectButton, forwardButton, reverseButton, pos1Button, pos2Button)
{
    this->gamepad = gamepad;
}

void InputManager::Begin(CommandStream *serialCommand)
{
    this->serialCmd = serialCommand;
}

void InputManager::HandleInputs()
{
    if (this->gamepad == nullptr)
    return;

  if (motorManager->currentDirection() == Direction::FORWARD)
  {
    gamepad->press(1);
    gamepad->release(0);
   }
   else if (motorManager->currentDirection() == Direction::BACKWARD)
   {
    gamepad->press(1);
    gamepad->release(0);
   }
   else
  {
        gamepad->press(0);
        gamepad->release(0);

        if (pos1Button->pressDownStarted()) {
          gamepad->press(2);
      } else if (pos1Button->wasReleased()) {
          gamepad->release(2);
      }

      if (pos2Button->pressDownStarted()) {
          gamepad->press(3);
      } else if (pos2Button->wasReleased()) {
          gamepad->release(3);
      }
  }
}

uint8_t InputManager::GetButtonMode()
{
    return buttonMode;
}

void InputManager::Read()
{
    // check for current mode selection
    if (modeSelectButton != nullptr) {
        modeSelectButton->read(true);
        if (modeSelectButton->isPressed() || modeSelectButton->pressDownStarted())
        {
            buttonMode = 0;
        }
        else
        {
            buttonMode = 1;
        }
    }
    else {
        buttonMode = 0;
    }

    // check for manual controls, manual overrides
    if (forwardButton != nullptr) {
        forwardButton->read(false);
        if (forwardButton->pressDownStarted())
        {
            onForwardBegin();
        }
        else if (forwardButton->isPressed())
        {
            onForward();
        }
        else if (forwardButton->wasReleased())
        {
            onForwardEnd();
        }
    }

    if (reverseButton != nullptr) {
       reverseButton->read(false);
        if (reverseButton->pressDownStarted())
        {
            onReverseBegin();
        }
        else if (reverseButton->isPressed())
        {
            onReverse();
        }
        else if (reverseButton->wasReleased())
        {
            onReverseEnd();
        }
    }
    if (pos1Button != nullptr) {

        //pos1Button->read(false);
        if (pos1Button->wasReleased())
        {
            onPosButtonClick(0);
            String data = String((uint8_t)CockpitControlsEvents::POS_CLICKED, DEC) + String(0);
            this->serialCmd->writeCommand((char)CockpitControlsCommandKey::EVENT, data);
        }
    }
    if (pos2Button != nullptr) {

      //  pos2Button->read(false);
        if (pos2Button->wasReleased())
        {
            onPosButtonClick(1);
            String data = String((uint8_t)CockpitControlsEvents::POS_CLICKED, DEC) + String(1);
            this->serialCmd->writeCommand((char)CockpitControlsCommandKey::EVENT, data);
        }
    }
}

void InputManager::onPosButtonClick(uint8_t pos)
{
  motorManager->gotoPos(pos);
}
void InputManager::onForwardBegin()
{
  this->serialCmd->debugPrintln("start forward");
  motorManager->forward(true);
}
void InputManager::onForward()
{
  motorManager->forward(true);
}
void InputManager::onForwardEnd()
{
  this->serialCmd->debugPrintln("stop forward");
  motorManager->stop();
}
void InputManager::onReverseBegin()
{
  this->serialCmd->debugPrintln("start Reverse");
  motorManager->reverse(true);
}
void InputManager::onReverse()
{
  motorManager->reverse(true);
}
void InputManager::onReverseEnd()
{
  this->serialCmd->debugPrintln("stop Reverse");
  motorManager->stop();
}