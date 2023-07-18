/*
 * triggers.c
 *
 *  Created on: 19 JUNE 2023
 *      Author: F_RZH
 */

#include "triggers.h"

bool rtrig(trig_t *trig)
{
	if (trig->in && !trig->prevState)
	{
		trig->prevState = true;
		trig->out = true;
	} else if (trig->in && trig->prevState)
	{
		trig->out = false;
	} else
	{
		trig->prevState = false;
		trig->out = false;
	}
	return trig->out;
}

bool ftrig(trig_t *trig)
{
	if (!trig->in && !trig->prevState)
	{
		trig->prevState = false;
		trig->out = false;
		return false;
	} else if (trig->in && !trig->prevState)
	{
		trig->prevState = true;
		trig->out = false;
		return false;
	} else if (trig->in && trig->prevState)
	{
		trig->prevState = true;
		trig->out = false;
		return false;
	} else if (!trig->in && trig->prevState)
	{
		trig->prevState = false;
		trig->out = true;
		return true;
	}
	return 0;
}

bool frtrig(trig_t *trig)
{
	return (ftrig(trig) || rtrig(trig));
}

bool onChange8(onChange8_t* onChange)
{

	if (onChange->in != onChange->stateStore)
	{
		onChange->stateStore = onChange->in;
		onChange->out = true;
	} else
	{
		onChange->out = false;
	}
	return onChange->out;
}

bool onChange16(onChange16_t* onChange)
{

	if (onChange->in != onChange->stateStore)
	{
		onChange->stateStore = onChange->in;
		onChange->out = true;
	} else
	{
		onChange->out = false;
	}
	return onChange->out;
}



