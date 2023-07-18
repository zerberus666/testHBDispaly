/*
 * triggers.h
 *
 *  Created on: 19 JUNE 2023
 *      Author: F_RZH
 */

#ifndef TRIGGERS_H_
#define TRIGGERS_H_

#include <inttypes.h>
#include <stdbool.h>

typedef struct{
	bool in;
	bool prevState;
	bool out;
}trig_t;

typedef struct{
	uint8_t in;
	uint8_t stateStore;
	bool out;
}onChange8_t;

typedef struct{
	uint8_t in;
	uint8_t stateStore;
	bool out;
}onChange16_t;

bool rtrig(trig_t*);
bool ftrig(trig_t*);
bool frtrig(trig_t*);

bool onChange8(onChange8_t*);
bool onChange16(onChange16_t*);

#endif /* TRIGGERS_H_ */
