#include "button_interrupts.h"
#include "config.h"


uint32_t startTime;
volatile bool powerSW = false;
ICACHE_RAM_ATTR void Isr_Btn_1(void){
//todo send stuff if button 1 is pressed
}
ICACHE_RAM_ATTR void Isr_Btn_2(void){
//todo send stuff if button 2 is pressed
}
ICACHE_RAM_ATTR void Isr_Btn_3(void){
//todo send stuff if button 3 is pressed
}
ICACHE_RAM_ATTR void powerButtonInterrupt(void){
  delay(30);  // Debounce
  if (!digitalRead(RXPIN)) {
    if (!powerSW) {
      startTime = millis();
      powerSW = true;
    }
    if (millis() - startTime > 3000) {
      digitalWrite(BUTTON_PIN_1, LOW);  
      digitalWrite(LED_BUILTIN, LOW);
      while (!digitalRead(RXPIN)); 
    }
  } else {
    powerSW = false;
  }

}
