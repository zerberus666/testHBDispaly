/*
 * soft_timer.c
 *
 *  Created on: 19 JUNE 2023
 *      Author: F_RZH
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include "soft_timer.h"
#include "triggers.h"

/*!
 * @brief Timers function block. Must be executed every program cycle
 *
 * @param tim timer defines structure
 * @return	true for one program cycle if timer count ends
 */

bool fbTimer(Timer_t *tim)
{
	tim->trig.in = tim->start;
	if (rtrig(&tim->trig))
	{

		if (!tim->isRunning)
		{
			tim->isRunning = true;
			tim->timeStopMs = tim->timeMs % 1000 + timeMs;
			tim->timeStopS = tim->timeMs / 1000 + timeS;

			if (tim->timeStopMs > 999)
			{
				tim->timeStopS += tim->timeStopMs / 1000;
				tim->timeStopMs = tim->timeStopMs % 1000;
			}
		}
	}

	tim->start = false;

	if (tim->isRunning && timeS >= tim->timeStopS && timeMs >= tim->timeStopMs)
	{
		tim->isRunning = false;
		tim->out = true;
		return tim->out;
	}

	tim->out = false;
	return tim->out;
}

/*!
 * @brief Function continues running timer or do nothing
 *
 * @param tim Timer defines structure
 * @return true, if timer was running and successfully continued
 */
bool fbTimerContinue(Timer_t *tim)
{
	if(tim->isRunning)
	{
		tim->timeStopMs = tim->timeMs % 1000 + timeMs;
		tim->timeStopS = tim->timeMs / 1000 + timeS;

		if (tim->timeStopMs > 999)
		{
			tim->timeStopS += tim->timeStopMs / 1000;
			tim->timeStopMs = tim->timeStopMs % 1000;
		}
		return true;
	}
	return false;
}

/*!
 * @brief Resets and stops timer defined in parameter tim
 *
 * @param tim
 */
void resetFbTimer(Timer_t *tim)
{
	tim->start = false;
	tim->isRunning = false;
}

void fbButton(FbButton_t* but)
{
	but->trig.in = but->pushButton;
	fbTimer(&but->doubleCLickTimer);


	if (rtrig(&but->trig))
	{
		if (!but->isRunning)
		{
			but->isRunning = true;
			but->timeStopClickMs = CLICK_TIME_MS % 1000 + timeMs;
			but->timeStopClickS = CLICK_TIME_MS / 1000 + timeS;
			if (but->timeStopClickMs > 999)
			{
				but->timeStopClickS += but->timeStopClickMs / 1000;
				but->timeStopClickMs = but->timeStopClickMs % 1000;
			}
			but->timeStopPressMs = PRESS_TIME_MS % 1000 + timeMs;
			but->timeStopPressS = PRESS_TIME_MS / 1000 + timeS;
			if (but->timeStopPressMs > 999)
			{
				but->timeStopPressS += but->timeStopPressMs / 1000;
				but->timeStopPressMs = but->timeStopPressMs % 1000;
			}
		}
		if (!but->doubleCLickTimer.isRunning)
		{
			but->doubleCLickTimer.timeMs = DOUBLE_CLICK_TIME_MS;
			but->doubleCLickTimer.start = true;
		}
	}

	if (but->pushButton && !(but->isPressed) && !(but->isClicked) && but->isRunning && timeS >= but->timeStopClickS && timeMs >= but->timeStopClickMs)
	{
		but->isClicked = true;

	}


	if (but->pushButton && !(but->isPressed) && but->isRunning && timeS >= but->timeStopPressS && timeMs >= but->timeStopPressMs)
	{
		but->isPressed = true;
		but->isPressedOut = true;
		but->isClicked = false;
	}

	if(!but->pushButton)
	{
		if (but->doubleCLickTimer.isRunning)
		{
			if (but->isRunning)
			{
				but->isRunning = false;
				if (but->isClicked)
				{
					but->isClicked = false;
					but->clickCount++;
				}
			}
		}else
		{
			switch (but->clickCount)
			{
			case 0:
				but->isClickedOut = false;
				but->isDoubleClickedOut = false;
				break;
			case 1:
				but->isClickedOut = true;
				but->isDoubleClickedOut = false;
				break;
			case 2:
				but->isClickedOut = false;
				but->isDoubleClickedOut = true;
				break;
			}

			but->isRunning = false;
			but->isPressedOut = false;
			but->isPressed = false;
			but->clickCount = 0;
			resetFbTimer(&but->doubleCLickTimer);
		}

		if(but->isPressed && but->isRunning)
		{
			but->isPressed = false;
			but->isRunning = false;
		}
	}else
	{

	}
}

void fbButtonDoubleClick(FbButtonDoubleClick_t* button)
{

}

uint8_t fbPulses(uint16_t periodMs, uint8_t enable, Timer_t* tim)
{
	if (!enable)
	{
		resetFbTimer(tim);
	}
	tim->timeMs = periodMs;
	tim->start = (!tim->out) && enable;
	return fbTimer(tim);
}

uint8_t countTimeDown(uint8_t* mins, uint8_t* secs, uint8_t* secsIndicator, uint8_t enable, Timer_t* tim)
{
	uint16_t secsElapsed = *mins * 60 + *secs;

	if (fbPulses(1000, enable, tim))
	{
		if (secsElapsed > 0)
		{
			*mins = (--secsElapsed) / 60;
			*secs = (secsElapsed) % 60;
			*secsIndicator = !*secsIndicator;
		}
	}
	if (secsElapsed > 0)
	{
		return 1;
	}else
	{
		return 0;
	}
}

unsigned long mills()
{
	return (timeS * 1000 + timeMs);
}

ISR(TIMER0_COMPA_vect)
{
	timeMs++;
	if (timeMs >= 1000)
	{
		timeMs = 0;
		timeS++;
	}
}
