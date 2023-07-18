/*
 * usartCharFunctions.c
 *
 *  Created on: 20 JUNE 2022
 *      Author: F_RZH
 */
#include "usartCharFunctions.h"
//#include "indication.h"

#ifdef REDEFINE_STDOUT
static FILE mystdout = FDEV_SETUP_STREAM(usart_putchar_printf, NULL, _FDEV_SETUP_WRITE);	//this statement for stdout redefine
#endif

static bool isTransmitting = false;

static queue_t queueTX;
static queue_t queueRX;

void usart_init( uint16_t ubrr) {


#ifdef REDEFINE_STDOUT
	stdout = &mystdout; //this statement for stdout redefine
#endif

#ifdef RS485
	RS485_DE_INIT();
#endif

	// Set baud rate
	UBRR0H = (uint8_t)(ubrr>>8);
	UBRR0L = (uint8_t)ubrr;
	// Enable receiver and transmitter
	UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1 << RXCIE0);
	// Set frame format: 8data, 1stop bit
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);

	queueTX.buffer      = malloc(128);
	queueTX.buffer_size = 128;
	queueTX.head        = 0;
	queueTX.tail        = 0;
	queueTX.bytes_avail = 0;

	queueRX.buffer      = malloc(128);
	queueRX.buffer_size = 128;
	queueRX.head        = 0;
	queueRX.tail        = 0;
	queueRX.bytes_avail = 0;
}

#ifdef USE_INTERRUPTS_FOR_PRINT
void usart_putchar(uint8_t data) {
	// Wait for empty transmit buffer
	//while ( !(UCSR0A & (_BV(UDRE0))) );
	// Start transmission
	//UDR0 = data;

#ifdef RS485
	SET_RS485_DE();
#endif
	//while(isTrancmitting);
	isTransmitting = true;
	put(&queueTX, &data, 1);
	if (UCSR0A & (_BV(UDRE0)))
	{
		uint8_t dat;
		get(&queueTX, &dat, 1);
		UDR0 = dat;
		UCSR0B &= ~(1 << RXEN0);
		UCSR0B |= (1 << TXCIE0) | (1 << UDRIE0); //Enable TX Complete and Data register empty interrupts
	}
}
#else
void usart_putchar(uint8_t data) {

    // Wait for empty transmit buffer
    while ( !(UCSR0A & (_BV(UDRE0))) ){};

#ifdef RS485
	SET_RS485_DE();
#endif
    // Start transmission
    UDR0 = data;
}
#endif

bool isTX()
{
	return isTransmitting;
}

int usart_putchar_printf(char var, FILE *stream)
{
    //if (var == '\n') usart_putchar('\r');
    usart_putchar((uint8_t)var);
    return 0;
}

char usart_getchar(void) {
	// Wait for incomming data
	// while ( !(UCSR0A & (_BV(RXC0))) );
	// Return the data
	// return UDR0;
	char ch;
	get(&queueRX, (uint8_t*)&ch, 1);
	return ch;
}

void usart_pstr(char *s) {
    // loop through entire string
	while (*s) {
        usart_putchar(*s);
        s++;
    }
}

/**
 * @brief Put string to USART using interrupts
 *
 * @param s
 */
/*void usart_pstr(char *s)
{
    // loop through entire string
	while (*s) {
		put(&queue, (uint8_t *)s, 1);
		++s;
    }

	uint8_t tempChar;
	get(&queue, &tempChar, 1);
	usart_putchar(tempChar);
}*/


unsigned char kbhit(void) {
	//return nonzero if char waiting  polled version
	unsigned char b;
	b=0;
	if(UCSR0A & (1<<RXC0)) b=1;
	return b;
}

static inline off_t min(off_t a, off_t b) {
	return a < b ? a : b;
}

bool put(queue_t *q, uint8_t *data, size_t size) {
	if(q->buffer_size - q->bytes_avail < size){
		return false;
	}
	const size_t part1 = min(size, q->buffer_size - q->tail);
	const size_t part2 = size - part1;

	memcpy(q->buffer + q->tail, data,         part1);
	memcpy(q->buffer,           data + part1, part2);

	q->tail = (q->tail + size) % q->buffer_size;
	q->bytes_avail += size;
	return true;
}

bool get(queue_t *q, uint8_t *data, size_t size) {
	if(q->bytes_avail < size){
		return false;
	}
	const size_t part1 = min(size, q->buffer_size - q->head);
	const size_t part2 = size - part1;

	memcpy(data,         q->buffer + q->head, part1);
	memcpy(data + part1, q->buffer,           part2);

	q->head = (q->head + size) % q->buffer_size;
	q->bytes_avail -= size;
	return true;
}

size_t getUARTTXBufBytesAvail()
{
	return queueTX.bytes_avail;
}

size_t getUARTRXBufBytesAvail()
{
	return queueRX.bytes_avail;
}

void resetRXQueue()
{
	queueRX.head        = 0;
	queueRX.tail        = 0;
	queueRX.bytes_avail = 0;
}

void resetTXQueue()
{
	queueTX.head        = 0;
	queueTX.tail        = 0;
	queueTX.bytes_avail = 0;
}

ISR(USART_UDRE_vect)
{
	uint8_t dat;
	if (get(&queueTX, &dat, 1))
	{
		UDR0 = dat;
	}else
	{
		UCSR0B &= ~(1 << UDRIE0); //reset Data register empty interrupt
	}
}

ISR(USART_TX_vect)
{
	UCSR0B &= ~(1 << TXCIE0); //reset TX Complete interrupt
	isTransmitting = false;
#ifdef RS485
	RESET_RS485_DE();
	UCSR0B |= (1 << RXEN0);
#endif
}

ISR(USART_RX_vect)
{
	uint8_t dat = UDR0;
	put(&queueRX, &dat, 1);
/*	TOGGLE_LED(1);
	TOGGLE_LED(2);*/
}



