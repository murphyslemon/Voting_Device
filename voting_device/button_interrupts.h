#ifndef BUTTON_ISR
#define BUTTON_ISR
#include <Arduino.h>
#include "config.h"

ICACHE_RAM_ATTR void Isr_Btn_1(void);
ICACHE_RAM_ATTR void Isr_Btn_2(void);
ICACHE_RAM_ATTR void Isr_Btn_3(void);
ICACHE_RAM_ATTR void powerButtonInterrupt(void);

#endif