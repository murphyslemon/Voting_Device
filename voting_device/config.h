#include <Arduino.h>
#include <ESP8266WiFi.h>  // default wifi library
#include <PubSubClient.h> //Mqtt library by Nick O'Leary
//#include <Wifi.h>


//Definition of GPIOS for Buttons
#define BUTTON_PIN_1 5   // GPIO 5 (entspricht D1) für Taster 1
#define BUTTON_PIN_2 4   // GPIO 4 (entspricht D2) für Taster 2
#define BUTTON_PIN_3 16  // GPIO 16 (entspricht D0) für Taster 3




// WLAN-Settings
const char* ssid = "RasPi-Netzwerk";
const char* password = "";

// MQTT-Server Settings
const char* mqtt_server = "10.42.0.1";
const int mqtt_port = 1883;
const char* mqtt_user = "";
const char* mqtt_password = "";
#define MQTTpubQos 1                  
#define MQTTsubQos 1

// MQTT topics to subscribe

const char* subInit = ("/registration/esp/"+WiFi.macAddress()).c_str();
const char* subVoteSetup = "/setupVote/Setup";
const char* subResync = "/setupVote/Resync";
//Mqtt topics to publish
const char* pubInit = ("/registration/Server/"+WiFi.macAddress()).c_str();
const char* pubPubVote = "/vote/VotingID";


//configuration of functionality
//#define ENCRYPTION
#define STATUS_LEDS
//#define E_PAPER
#define DEBUG
#define ISRS_FOR_BUTTONS

//Includes according to config

#ifdef ENCRYPTION
#include <encryptionlibrary.h>  //todo find encryption library
#endif

#ifdef STATUS_LEDS
#define LED_PIN_1  14 // GPIO 14 (entspricht D5) für LED 1
#define LED_PIN_2  12 // GPIO 12 (entspricht D6) für LED 2
#define LED_PIN_3  13 // GPIO 13 (entspricht D7) für LED 3
#endif

#ifdef E_PAPER
#include <E_Paper_Library.h> //todo find epaper library
#endif

#ifdef ISRS_FOR_BUTTONS
#include "button_interrupts.h"
void attachISR(void){
attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_1), Isr_Btn_1, FALLING);
attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_2), Isr_Btn_2, FALLING);
attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_3), Isr_Btn_3, FALLING);
}
#endif