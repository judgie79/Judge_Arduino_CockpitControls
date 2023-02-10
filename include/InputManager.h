#ifndef InputManager_h
#define InputManager_h

#include "Arduino.h"
#include <CommandStream.h>
#include <DebouncedButton.h>
#include <MotorManager.h>
#include <UniversalGamepad.h>
#include "CockpitControlCommands.h"

class InputManager
{
public:
    InputManager(MotorManager *motorManager,
                DebouncedButton *modeSelectButton, DebouncedButton *forwardButton, DebouncedButton *reverseButton,
                DebouncedButton *pos1Button, DebouncedButton *pos2Button);
    InputManager(MotorManager *motorManager, UniversalGamepad *gamepad,
                DebouncedButton *modeSelectButton, DebouncedButton *forwardButton, DebouncedButton *reverseButton,
                DebouncedButton *pos1Button, DebouncedButton *pos2Button);

    void Begin(CommandStream *serialCommand);
    void Read();
    uint8_t GetButtonMode();

    void HandleInputs();
private:
    void onPosButtonClick(uint8_t pos);
    void onForwardBegin();
    void onForward();
    void onForwardEnd();
    void onReverseBegin();
    void onReverse();
    void onReverseEnd();

    DebouncedButton *modeSelectButton = nullptr;
    DebouncedButton *forwardButton = nullptr;
    DebouncedButton *reverseButton = nullptr;
    DebouncedButton *pos1Button = nullptr;
    DebouncedButton *pos2Button = nullptr;
    UniversalGamepad *gamepad = nullptr;

    CommandStream *serialCmd = nullptr;
    MotorManager *motorManager = nullptr;
    uint8_t buttonMode = 1;
};

#endif