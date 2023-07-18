/*
 * soft_timer.h
 *
 *  Created on: 19 JUNE 2023
 *      Author: F_RZH
 */

#ifndef SOFT_TIMER_H_
#define SOFT_TIMER_H_
#include "inttypes.h"
#include "stdbool.h"
#include "triggers.h"

#define CLICK_TIME_MS 3
#define PRESS_TIME_MS 1000
#define DOUBLE_CLICK_TIME_MS 300


	//setup timer 1
	//F_CPU = 16 000 000
	//prescaler x/64, 1ms = 250
	//Time mS
#define INIT_SOFT_TIMERS() \
		do{\
			TIMSK0 |= (1 << OCIE0A);\
			TCCR0A |= (1 << WGM01);  		/*CTC mode*/\
			TCCR0B |= (1 << CS00) | (1 << CS01);\
			OCR0A = 250;}while(0) //1ms = 250


/*!
 * @brief Structure defines timer
 *
 */
typedef struct {
	uint32_t timeStopS;		///Time in seconds when timer stops for internal use
	uint16_t timeStopMs;	///Time in milliseconds when timer stops for internal use
	bool start;				///Parameter trigs timer start
	uint16_t timeMs;		///Parameter time in millisecons when timer works
	uint8_t isRunning;		///Output TRUE if timer counts
	trig_t trig;			///rTrig's Structure for internal use
	bool out;				///Output True for one program cycle when timer stops
}Timer_t;

typedef struct{
	uint32_t timeStopClickS;
	uint16_t timeStopClickMs;
	uint32_t timeStopPressS;
	uint16_t timeStopPressMs;
	uint8_t clickCount;
	bool isRunning;
	trig_t trig;
	Timer_t doubleCLickTimer;
	bool isPressed;
	bool isClicked;
	bool isDoubleClicked;
	bool pushButton;
	bool isPressedOut;
	bool isClickedOut;
	bool isDoubleClickedOut;
}FbButton_t;

typedef struct{
	uint32_t timeStopClickS;
	uint16_t timeStopClickMs;
	uint32_t timeStopPressS;
	uint16_t timeStopPressMs;
	uint8_t isRunning;
	trig_t trig;
	bool isPressed;
	bool isClicked;
	bool pushButton;
	bool isDoubleClicked;
	bool isPressedOut;
	bool isClickedOut;
	bool isDoubleClickedOut;
}FbButtonDoubleClick_t;

volatile static uint16_t timeMs;
volatile static uint32_t timeS;

//uint8_t fbTimer(uint16_t sTimeMs, uint8_t bStart, Timer_t* tim);
bool fbTimer(Timer_t *tim);
bool fbTimerContinue(Timer_t *tim);
void resetFbTimer(Timer_t *tim);
uint8_t fbPulses(uint16_t periodMs, uint8_t enable, Timer_t* tim);
uint8_t countTimeDown(uint8_t* mins, uint8_t* secs, uint8_t* secsIndicator, uint8_t enable, Timer_t* tim);
void fbButton(FbButton_t* tim);
void fbButtonDoubleClick(FbButtonDoubleClick_t* button);
unsigned long mills();

#endif /* SOFT_TIMER_H_ */
