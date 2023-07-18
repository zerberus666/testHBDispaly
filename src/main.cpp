#include <Arduino.h>
#include <usartCharFunctions.h>
#include "soft_timer.h"
#include <stdbool.h>
//#include "usartCharFunctions.h"

// put function declarations here:
Timer_t tim;



void setup() {
  // put your setup code here, to run once:
  //int result = myFunction(2, 3);
  //usart_init(MYUBRR);  //usart_init(12);
  INIT_SOFT_TIMERS();
  tim.timeMs = 1000;
  tim.start = true;
}

void loop() {
  // put your main code here, to run repeatedly:
  fbTimer(&tim);
  
}

      