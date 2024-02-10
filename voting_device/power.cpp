#include "power.h"
#include "display.h"

uint32_t startTime;
bool powerSW = false;

void powerOff(){
  if (!digitalRead(RX_PIN)){
    delay(30);  //debounce
    if (!digitalRead(RX_PIN)){
      if (!powerSW){
      startTime = millis();
      powerSW = true;
      }
      if (millis() - startTime > 3000){
        digitalWrite(PWR_PIN, LOW);  //turn device off
        //write your code here to clear the epaper to notify the user know to release the button
        epd.LDirInit();
        epd.Clear(); // clears whole screen to white
        while (!digitalRead(RX_PIN)){  //wait for button released
          delay(100);
        }
      }
    }
  }
  else{
    powerSW = false;
  }
}