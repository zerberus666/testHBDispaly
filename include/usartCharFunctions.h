/*
 * usartCharFunctions.h
 *
 *  Created on: 20 JUNE 2022
 *      Author: F_RZH
 */

#ifndef INC_USARTCHARFUNCTIONS_H_
#define INC_USARTCHARFUNCTIONS_H_

#include "avr/io.h"
#include "avr/interrupt.h"
#include "stdio.h"
#include "stdbool.h"
#include "inttypes.h"
#include "sys/types.h"
#include "stdlib.h"
#include "string.h"

#define USE_INTERRUPTS_FOR_PRINT
#define REDEFINE_STDOUT

#define RS485

#ifdef RS485
#define RS485_DE_PIN PD2
#define RS485_DE_PINx PIND
#define RS485_DE_PORT PORTD
#define RS485_DE_DDR DDRD
#define RS485_DE_INIT()	do{RS485_DE_DDR |= 1 << RS485_DE_PIN;}while(0)
#define RESET_RS485_DE() do{RS485_DE_PORT &= ~(1 << RS485_DE_PIN);}while(0)
#define SET_RS485_DE() do{RS485_DE_PORT |= (1 << RS485_DE_PIN);}while(0)

#endif

#define BAUD 19200
#define MYUBRR F_CPU/16/BAUD-1

typedef struct {
    uint8_t *buffer;
    size_t   buffer_size;
    size_t   head;
    size_t   tail;
    size_t   bytes_avail;
} queue_t;

/********************************************************************************
                                usart Related
********************************************************************************/
void usart_init( uint16_t ubrr);

void usart_putchar(uint8_t data);

bool isTX();

int usart_putchar_printf(char var, FILE *stream);

char usart_getchar(void);

void usart_pstr(char *s);

unsigned char kbhit(void);

bool put(queue_t *q, uint8_t *data, size_t size);
bool get(queue_t *q, uint8_t *data, size_t size);

size_t getUARTTXBufBytesAvail();
size_t getUARTRXBufBytesAvail();
void resetTXQueue();
void resetRXQueue();

#endif /* INC_USARTCHARFUNCTIONS_H_ */
