/*
 * Header for RemoteDebugger
 *
 * Copyright (C) 2018  Joao Lopes https://github.com/JoaoLopesF/RemoteDebuggger
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * This header file describes the public API for sending debug messages to a telnet client.
 *
 */

#ifndef REMOTEDEBUGGER_H
#define REMOTEDEBUGGER_H

//////// Simple software debugger (based on SerialDebug)

// Is for SerialDebug (TODO: in future will some code for both library)

//#define DEBUGGER_FOR_SERIALDEBUG true

// Is for RemoteDebug

#define DEBUGGER_FOR_REMOTEDEBUG true

//////// Includes

#include "Arduino.h"

// Library

#ifdef DEBUGGER_FOR_SERIALDEBUG

#include "SerialDebug.h"
#define DEBUGGER_VAR_TYPE

#else // RemoteDebug

#include "RemoteDebug.h"
#define DEBUGGER_VAR_TYPE static

#endif

// Boards

#ifdef DEBUGGER_FOR_SERIALDEBUG
#include "utility/Boards.h"
#else // RemoteDebug
#define BOARD_ENOUGH_MEMORY true // WiFi boards have a lot of memory
#endif

//////
// Debugger options
// It is here too, due Arduino IDE not working for project local defines
/////

// Debug watch global variables - need stay here, to not cause errors in DEBUG_DISABLED empty macros

typedef enum {										// Type of operator
	DEBUG_WATCH_CHANGED,							// Changed value ?
	DEBUG_WATCH_EQUAL,								// Equal (==)
	DEBUG_WATCH_DIFF,								// Different (!=)
	DEBUG_WATCH_LESS,								// Less (<=)
	DEBUG_WATCH_GREAT,								// Greater (>)
	DEBUG_WATCH_LESS_EQ,							// Less or equal (<=)
	DEBUG_WATCH_GREAT_EQ							// Greater or equal (>=)
} debugEnumWatch_t;

// Disable debug - good for release (production)
// Put below define after the include SerialDebug/RemoteDebug in your project to disable all debug
// as nothing of SerialDebug/RemoteDebug is compiled, zero overhead :-)
//#define DEBUG_DISABLED true

#ifdef DEBUG_DISABLED

	// No debugs

#ifdef DEBUGGER_FOR_SERIALDEBUG
	#define debugSilence(...)
#endif

	#define DEBUG_DISABLE_DEBUGGER true

#endif

// Disable debugger ? No more commands and features as functions and globals
// Uncomment this to disable it (SerialDebug will not do reads from Serial, good if already have this)
//#define DEBUG_DISABLE_DEBUGGER true

#ifndef DEBUG_DISABLE_DEBUGGER	// Debugger enabled

// Enable Flash variables support - F()
// Used internally in SerialDebug and in public API
// If is a low memory board, like AVR, all strings in SerialDebug is using flash memory
// If have RAM memory, this is more fast than flash
//#define DEBUG_USE_FLASH_F true

// For Espressif boards, default is not flash support for printf,
// due it have a lot of memory and Serial.printf is not compatible with it
// If you need more memory, can force it:
//#define DEBUG_USE_FLASH_F true

//////// Defines

// Internal printf ?

#if (defined ESP8266 || defined ESP32) && !(defined DEBUG_USE_FLASH_F)  // For Espressif boards, have Serial.printf native (but not for Flash F)
											 							// TODO: see if another boards have it, Arduinos AVR not have it)
	#define DEBUG_USE_NATIVE_PRINTF true	// Enable native
	#define DEBUG_NOT_USE_FLASH_F true    	// Disable all printf with __FlashStringHelper - If you need, just comment it
#endif

// Uses flash ?

#if (defined ESP8266 || defined ESP32) && !(defined DEBUG_USE_FLASH_F)  // For Espressif boards, have Serial.printf native (but not for Flash F)
																		// TODO: see if another boards have it, Arduinos AVR not have it)
	#define DEBUG_NOT_USE_FLASH_F true    	// Disable all printf with __FlashStringHelper - If you need, just comment it
#endif

// Size for commands

#define DEBUG_MAX_SIZE_COMMANDS 10			// Maximum size of commands - can be changed
#define DEBUG_MAX_SIZE_CMD_OPTIONS 64		// Maximum size of commands options - can be changed

// Max size for compare char arrays or string

#define DEBUG_MAX_CMP_STRING 48

// Timeout for debugBreak - put 0 if not want timeout

#ifndef DEBUG_BREAK_TIMEOUT
	#define DEBUG_BREAK_TIMEOUT 5000
#endif

// Timeout for watch triggered - put 0 if not want timeout

#ifndef DEBUG_BREAK_WATCH
	#define DEBUG_BREAK_WATCH 10000
#endif

// Minimum time to process handle event

#define DEBUG_MIN_TIME_EVENT 850

//////// Prototypes - public

#ifdef DEBUGGER_FOR_SERIALDEBUG
void debugInitDebugger (HardwareSerial *serial, void (*callbackShowHelp)(), void (*callbackProcessCmd)(),
						boolean *debuggerActive); // TODO: in future do both lib share this same code
#else // RemoteDebug
void debugInitDebugger (RemoteDebug *Debug);
#endif

void debugPrintInfo(const char level, const char* function);

void debugProcessCmdDebugger();
String debugGetHelpDebugger();
boolean debugGetDebuggerEnabled();
void debugSetDebuggerEnabled(boolean enabled);
void debugHandleEvent(boolean calledByHandleEvent);
String debugBreak();
#ifdef DEBUG_USE_FLASH_F
String debugBreak(const __FlashStringHelper * str, uint32_t timeout = DEBUG_BREAK_TIMEOUT, boolean byWatch = false);
#endif
String debugBreak(const char* str, uint32_t timeout = DEBUG_BREAK_TIMEOUT, boolean byWatch = false);

#ifndef BOARD_LOW_MEMORY // Not for low memory boards
	String debugBreak(String& str, uint32_t timeout = DEBUG_BREAK_TIMEOUT);
#endif

#ifdef DEBUGGER_FOR_SERIALDEBUG
void debugSilence(boolean activate, boolean showMessage, boolean fromBreak = false);
#endif

// Debugger

// For functions // TODO: make more types

int8_t debugAddFunctionVoid(const char* name, void (*callback)());
int8_t debugAddFunctionStr(const char* name, void (*callback)(String));
int8_t debugAddFunctionChar(const char* name, void (*callback)(char));
int8_t debugAddFunctionInt(const char* name, void (*callback)(int));

void debugSetLastFunctionDescription(const char *description);

// For globals

#ifndef BOARD_LOW_MEMORY // Not for low memory boards

	int8_t debugAddGlobalBoolean (const char* name, boolean* pointer);
	int8_t debugAddGlobalChar (const char* name, char* pointer);
	int8_t debugAddGlobalByte (const char* name, byte* pointer);
	int8_t debugAddGlobalInt (const char* name, int* pointer);
	int8_t debugAddGlobalUInt (const char* name, unsigned int* pointer);
	int8_t debugAddGlobalLong (const char* name, long* pointer);
	int8_t debugAddGlobalULong (const char* name, unsigned long* pointer);
	int8_t debugAddGlobalFloat (const char* name, float* pointer);
	int8_t debugAddGlobalDouble (const char* name, double* pointer);
	int8_t debugAddGlobalInt8_t (const char* name, int8_t* pointer);
	int8_t debugAddGlobalInt16_t (const char* name, int16_t* pointer);
	int8_t debugAddGlobalInt32_t (const char* name, int32_t* pointer);
//		#ifdef ESP32
	//int8_t debugAddGlobalInt64_t (const char* name, int64_t* pointer);
//		#endif
	int8_t debugAddGlobalUInt8_t (const char* name, uint8_t* pointer);
	int8_t debugAddGlobalUInt16_t (const char* name, uint16_t* pointer);
	int8_t debugAddGlobalUInt32_t (const char* name, uint32_t* pointer);
//		#ifdef ESP32
	//int8_t debugAddGlobalUInt64_t (const char* name, uint64_t* pointer);
//		#endif
	int8_t debugAddGlobalCharArray (const char* name, char* pointer);
	int8_t debugAddGlobalCharArray (const char* name, char* pointer, uint8_t showLength);
	int8_t debugAddGlobalString (const char* name, String* pointer);
	int8_t debugAddGlobalString (const char* name, String* pointer, uint8_t showLength);

	void debugSetLastGlobalDescription(const char *description);

	// For flash F

	int8_t debugAddFunctionVoid(const __FlashStringHelper* name, void (*callback)());
	int8_t debugAddFunctionStr(const __FlashStringHelper* name, void (*callback)(String));
	int8_t debugAddFunctionChar(const __FlashStringHelper* name, void (*callback)(char));
	int8_t debugAddFunctionInt(const __FlashStringHelper* name, void (*callback)(int));

	void debugSetLastFunctionDescription(const __FlashStringHelper *description);

	int8_t debugAddGlobalBoolean (const __FlashStringHelper* name, boolean* pointer);
	int8_t debugAddGlobalChar (const __FlashStringHelper* name, char* pointer);
	int8_t debugAddGlobalByte (const __FlashStringHelper* name, byte* pointer);
	int8_t debugAddGlobalInt (const __FlashStringHelper* name, int* pointer);
	int8_t debugAddGlobalUInt (const __FlashStringHelper* name, unsigned int* pointer);
	int8_t debugAddGlobalLong (const __FlashStringHelper* name, long* pointer);
	int8_t debugAddGlobalULong (const __FlashStringHelper* name, unsigned long* pointer);
	int8_t debugAddGlobalFloat (const __FlashStringHelper* name, float* pointer);
	int8_t debugAddGlobalDouble (const __FlashStringHelper* name, double* pointer);
	int8_t debugAddGlobalInt8_t (const __FlashStringHelper* name, int8_t* pointer);
	int8_t debugAddGlobalInt16_t (const __FlashStringHelper* name, int16_t* pointer);
	int8_t debugAddGlobalInt32_t (const __FlashStringHelper* name, int32_t* pointer);
//			#ifdef ESP32
	//int8_t debugAddGlobalInt64_t (const __FlashStringHelper* name, int64_t* pointer);
//			#endif
	int8_t debugAddGlobalUInt8_t (const __FlashStringHelper* name, uint8_t* pointer);
	int8_t debugAddGlobalUInt16_t (const __FlashStringHelper* name, uint16_t* pointer);
	int8_t debugAddGlobalUInt32_t (const __FlashStringHelper* name, uint32_t* pointer);
//			#ifdef ESP32
	//int8_t debugAddGlobalUInt64_t (const __FlashStringHelper* name, uint64_t* pointer);
//			#endif
	int8_t debugAddGlobalCharArray (const __FlashStringHelper* name, char* pointer);
	int8_t debugAddGlobalCharArray (const __FlashStringHelper* name, char* pointer, uint8_t showLength);
	int8_t debugAddGlobalString (const __FlashStringHelper* name, String* pointer);
	int8_t debugAddGlobalString (const __FlashStringHelper* name, String* pointer, uint8_t showLength);

	void debugSetLastGlobalDescription(const __FlashStringHelper *description);

	// Watches

	int8_t debugAddWatchBoolean (uint8_t globalNum, uint8_t operation, boolean value, boolean allwaysStop = false);
	int8_t debugAddWatchChar (uint8_t globalNum, uint8_t operation, char value, boolean allwaysStop = false);
	int8_t debugAddWatchByte (uint8_t globalNum, uint8_t operation, byte value, boolean allwaysStop = false);
	int8_t debugAddWatchInt (uint8_t globalNum, uint8_t operation, int value, boolean allwaysStop = false);
	int8_t debugAddWatchUInt (uint8_t globalNum, uint8_t operation, unsigned int value, boolean allwaysStop = false);
	int8_t debugAddWatchLong (uint8_t globalNum, uint8_t operation, long value, boolean allwaysStop = false);
	int8_t debugAddWatchULong (uint8_t globalNum, uint8_t operation, unsigned long value, boolean allwaysStop = false);
	int8_t debugAddWatchFloat (uint8_t globalNum, uint8_t operation, float value, boolean allwaysStop = false);
	int8_t debugAddWatchDouble (uint8_t globalNum, uint8_t operation, double value, boolean allwaysStop = false);
	int8_t debugAddWatchInt8_t (uint8_t globalNum, uint8_t operation, int8_t value, boolean allwaysStop = false);
	int8_t debugAddWatchInt16_t (uint8_t globalNum, uint8_t operation, int16_t value, boolean allwaysStop = false);
	int8_t debugAddWatchInt32_t (uint8_t globalNum, uint8_t operation, int32_t value, boolean allwaysStop = false);
	int8_t debugAddWatchUInt8_t (uint8_t globalNum, uint8_t operation, uint8_t value, boolean allwaysStop = false);
	int8_t debugAddWatchUInt16_t (uint8_t globalNum, uint8_t operation, uint16_t value, boolean allwaysStop = false);
	int8_t debugAddWatchUInt32_t (uint8_t globalNum, uint8_t operation, uint32_t value, boolean allwaysStop = false);

	int8_t debugAddWatchCharArray (uint8_t globalNum, uint8_t operation, const char* value, boolean allwaysStop = false);
	int8_t debugAddWatchString (uint8_t globalNum, uint8_t operation, String value, boolean allwaysStop = false);

	int8_t debugAddWatchBoolean  (const char* globalName, uint8_t operation, boolean value, boolean allwaysStop = false);
	int8_t debugAddWatchChar  (const char* globalName, uint8_t operation, char value, boolean allwaysStop = false);
	int8_t debugAddWatchByte  (const char* globalName, uint8_t operation, byte value, boolean allwaysStop = false);
	int8_t debugAddWatchInt  (const char* globalName, uint8_t operation, int value, boolean allwaysStop = false);
	int8_t debugAddWatchUInt  (const char* globalName, uint8_t operation, unsigned int value, boolean allwaysStop = false);
	int8_t debugAddWatchLong  (const char* globalName, uint8_t operation, long value, boolean allwaysStop = false);
	int8_t debugAddWatchULong  (const char* globalName, uint8_t operation, unsigned long value, boolean allwaysStop = false);
	int8_t debugAddWatchFloat  (const char* globalName, uint8_t operation, float value, boolean allwaysStop = false);
	int8_t debugAddWatchDouble  (const char* globalName, uint8_t operation, double value, boolean allwaysStop = false);
	int8_t debugAddWatchInt8_t  (const char* globalName, uint8_t operation, int8_t value, boolean allwaysStop = false);
	int8_t debugAddWatchInt16_t  (const char* globalName, uint8_t operation, int16_t value, boolean allwaysStop = false);
	int8_t debugAddWatchInt32_t  (const char* globalName, uint8_t operation, int32_t value, boolean allwaysStop = false);
	int8_t debugAddWatchUInt8_t  (const char* globalName, uint8_t operation, uint8_t value, boolean allwaysStop = false);
	int8_t debugAddWatchUInt16_t  (const char* globalName, uint8_t operation, uint16_t value, boolean allwaysStop = false);
	int8_t debugAddWatchUInt32_t  (const char* globalName, uint8_t operation, uint32_t value, boolean allwaysStop = false);

	int8_t debugAddWatchCharArray (const char* globalName, uint8_t operation, const char* value, boolean allwaysStop = false);
	int8_t debugAddWatchString (const char* globalName, uint8_t operation, String value, boolean allwaysStop = false);

	int8_t debugAddWatchCross(uint8_t globalNum, uint8_t operation, uint8_t anotherGlobalNum, boolean allwaysStop = false);
	int8_t debugAddWatchCross(const char* globalName, uint8_t operation, const char* anotherGlobalName, boolean allwaysStop = false);

	// For Flash F

	int8_t debugAddWatchBoolean (const __FlashStringHelper* globalName, uint8_t operation, boolean value, boolean allwaysStop = false);
	int8_t debugAddWatchChar (const __FlashStringHelper* globalName, uint8_t operation, char value, boolean allwaysStop = false);
	int8_t debugAddWatchByte (const __FlashStringHelper* globalName, uint8_t operation, byte value, boolean allwaysStop = false);
	int8_t debugAddWatchInt (const __FlashStringHelper* globalName, uint8_t operation, int value, boolean allwaysStop = false);
	int8_t debugAddWatchUInt (const __FlashStringHelper* globalName, uint8_t operation, unsigned int value, boolean allwaysStop = false);
	int8_t debugAddWatchLong (const __FlashStringHelper* globalName, uint8_t operation, long value, boolean allwaysStop = false);
	int8_t debugAddWatchULong (const __FlashStringHelper* globalName, uint8_t operation, unsigned long value, boolean allwaysStop = false);
	int8_t debugAddWatchFloat (const __FlashStringHelper* globalName, uint8_t operation, float value, boolean allwaysStop = false);
	int8_t debugAddWatchDouble (const __FlashStringHelper* globalName, uint8_t operation, double value, boolean allwaysStop = false);
	int8_t debugAddWatchInt8_t (const __FlashStringHelper* globalName, uint8_t operation, int8_t value, boolean allwaysStop = false);
	int8_t debugAddWatchInt16_t (const __FlashStringHelper* globalName, uint8_t operation, int16_t value, boolean allwaysStop = false);
	int8_t debugAddWatchInt32_t (const __FlashStringHelper* globalName, uint8_t operation, int32_t value, boolean allwaysStop = false);
	int8_t debugAddWatchUInt8_t (const __FlashStringHelper* globalName, uint8_t operation, uint8_t value, boolean allwaysStop = false);
	int8_t debugAddWatchUInt16_t (const __FlashStringHelper* globalName, uint8_t operation, uint16_t value, boolean allwaysStop = false);
	int8_t debugAddWatchUInt32_t (const __FlashStringHelper* globalName, uint8_t operation, uint32_t value, boolean allwaysStop = false);

	int8_t debugAddWatchCharArray (const __FlashStringHelper* globalName, uint8_t operation, const char* value, boolean allwaysStop = false);
	int8_t debugAddWatchString (const __FlashStringHelper* globalName, uint8_t operation, String value, boolean allwaysStop = false);

	int8_t debugAddWatchCross(const __FlashStringHelper* globalName, uint8_t operation, const __FlashStringHelper* anotherGlobalName, boolean allwaysStop = false);

#else // Low memory boards -> reduced number of functions

	// For flash F

	int8_t debugAddFunctionVoid(const __FlashStringHelper* name, void (*callback)());
	int8_t debugAddFunctionStr(const __FlashStringHelper* name, void (*callback)(String));
	int8_t debugAddFunctionChar(const __FlashStringHelper* name, void (*callback)(char));
	int8_t debugAddFunctionInt(const __FlashStringHelper* name, void (*callback)(int));

	#define debugSetLastFunctionDescription(str) // Not compile this

	int8_t debugAddGlobalBoolean (const __FlashStringHelper* name, boolean* pointer);
	int8_t debugAddGlobalChar (const __FlashStringHelper* name, char* pointer);
	int8_t debugAddGlobalInt (const __FlashStringHelper* name, int* pointer);
	int8_t debugAddGlobalULong (const __FlashStringHelper* name, unsigned long* pointer);
	int8_t debugAddGlobalString (const __FlashStringHelper* name, String* pointer);
	int8_t debugAddGlobalString (const __FlashStringHelper* name, String* pointer, uint8_t showLength);

	#define debugSetLastGlobalDescription(str) // Not compile this

	// No watches

#endif // Low memory

// Handle debugger

void debugHandleDebugger (const boolean calledByHandleEvent);

//////// External variables (need to use macros)

#ifdef DEBUGGER_FOR_SERIALDEBUG
extern boolean _debugActive;		   			// Debug is only active after receive first data from Serial
extern bool _debugSilence;						// Silent mode ?

extern uint8_t _debugFunctionsAdded;			// Number of functions added
extern uint8_t _debugGlobalsAdded;				// Number of globals added
extern uint8_t _debugWatchesAdded;				// Number of watches added
extern boolean _debugWatchesEnabled;			// Watches is enabled (only after add any)?
extern boolean _debugDebuggerEnabled;			// Simple Software Debugger enabled ?
#endif

//////// Defines and macros

// Macro to handle (better performance)

// Macros for debugs
// Note: not used F() for formats, due it is small and Flash can be slow

#ifndef BOARD_LOW_MEMORY // Not for low memory boards
	#define DEBUG_HANDLE_DEBUGGER() \
		if (_debugDebuggerEnabled && _debugGlobalsAdded > 0) { \
			debugHandleDebugger(false); \
		}
#else
	#define DEBUG_HANDLE_DEBUGGER()
#endif

#ifndef DEBUG_USE_FLASH_F // Only if not using flash

#ifndef FPSTR // Thanks a lot Espressif -> https://arduino-esp8266.readthedocs.io/en/latest/PROGMEM.html
	#define FPSTR(pstr_pointer) (reinterpret_cast<const __FlashStringHelper *>(pstr_pointer))
#endif

#endif

#endif // DEBUG_DISABLE_DEBUGGER

// Debugger disabled ?

#ifdef DEBUG_DISABLE_DEBUGGER

	#define debugAddGlobalBoolean (...)
	#define debugAddGlobalChar (...)
	#define debugAddGlobalByte (...)
	#define debugAddGlobalInt (...)
	#define debugAddGlobalUInt (...)
	#define debugAddGlobalLong (...)
	#define debugAddGlobalULong (...)
	#define debugAddGlobalFloat (...)
	#define debugAddGlobalDouble (...)
	#define debugAddGlobalInt8_t (...)
	#define debugAddGlobalInt16_t (...)
	#define debugAddGlobalInt32_t (...)
	//#ifdef ESP32
	//#define debugAddGlobalInt64_t (...)
	//#endif
	#define debugAddGlobalUInt8_t (...)
	#define debugAddGlobalUInt16_t (...)
	#define debugAddGlobalUInt32_t (...)
	//#ifdef ESP32
	//#define debugAddGlobalUInt64_t (...)
	//#endif
	#define debugAddGlobalCharArray (...)
	#define debugAddGlobalString (...)

	#define debugSetLastGlobalDescription(...)

	#define debugAddWatchBoolean (...)
	#define debugAddWatchChar (...)
	#define debugAddWatchByte (...)
	#define debugAddWatchInt (...)
	#define debugAddWatchUInt (...)
	#define debugAddWatchLong (...)
	#define debugAddWatchULong (...)
	#define debugAddWatchFloat (...)
	#define debugAddWatchDouble (...)
	#define debugAddWatchInt8_t (...)
	#define debugAddWatchInt16_t (...)
	#define debugAddWatchInt32_t (...)
	#define debugAddWatchUInt8_t (...)
	#define debugAddWatchUInt16_t (...)
	#define debugAddWatchUInt32_t (...)

	#define debugAddWatchCharArray (...)
	#define debugAddWatchString (...)

	#define debugAddWatchCross(...)

	#define debugHandleDebugger(...)

#endif // DEBUG_DISABLE_DEBUGGER

#endif // H

//////// End
