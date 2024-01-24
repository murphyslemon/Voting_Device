 #include "power.h"


 uint32_t startTime;
 bool powerSW = false;

 void powerOff(){
   if (!digitalRead(RXPIN)) {
      if (!powerSW) {
        startTime = millis();
        powerSW = true;
      }
      if (millis() - startTime > 3000) {
        digitalWrite(5, LOW);  
        digitalWrite(LED_BUILTIN, HIGH);
        while (!digitalRead(RXPIN)); 
      }
    } 
    else {
    powerSW = false;
  }
 }