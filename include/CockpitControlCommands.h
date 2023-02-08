// CockpitControlCommand.h

#ifndef _COCKPITCONTROLCOMMAND_h
#define _COCKPITCONTROLCOMMAND_h
#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

enum class CockpitControlsCommandKey : char {
	MOTORSPEED = 'm',
	DATA = 'd',
	EVENT = 't',
	GETPOSITIONS = 'g',
	SETPOSITIONS = 'u',
	GOTOPOSITION = 'i',
	SETSETTINGS = 's',
	GETSETTINGS = 'o',
	RESETSETTINGS = 'n'
};
/*

enum class CommandKey: char {
  HELLO = 'h',              // Hello order to initiate communication with the Arduino
  ALREADY_CONNECTED = 'c',  // Already connected to the Arduino
  ERROR = 'e',
  RECEIVED = 'r',  // Aknowlegment message
  DEBUG = 'l', // debug message
};

*/


enum class CockpitControlsEvents : uint8_t {
	POS_CLICKED = 0,
	POS_REACHED = 1,
	BACK_REACHED = 2,
	FRONT_REACHED = 3
};

#endif

