#include "button_interrupts.h"

ButtonVote ButtonYes;
ButtonVote ButtonNo;
ButtonVote ButtonAbstain;

//Definition of GPIOS for Buttons
//#define BUTTON_PIN_1 0   // GPIO 5 (entspricht D1) für Taster 1
//#define BUTTON_PIN_2 2   // GPIO 4 (entspricht D2) für Taster 2
//#define BUTTON_PIN_3 12  // GPIO 16 (entspricht D0) für Taster 3
//#define RXPIN        3


ICACHE_RAM_ATTR void Isr_Btn_1(void){
  delay(30);// Debounce
  ButtonYes.setState(1);
//todo send stuff if button 1 is pressed
}
ICACHE_RAM_ATTR void Isr_Btn_2(void){
  delay(30);// Debounce
  ButtonAbstain.setState(1);
//todo send stuff if button 2 is pressed
}
ICACHE_RAM_ATTR void Isr_Btn_3(void){
  delay(30);// Debounce
  ButtonNo.setState(1);
//todo send stuff if button 3 is pressed
}

