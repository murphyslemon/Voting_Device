#include <Arduino.h>
#include <ESP8266WiFi.h>  // default wifi library
#include <PubSubClient.h> //Mqtt library by Nick O'Leary
#include <string.h>
//#include <Wifi.h>

#define STRINGSIZE 256
#define BATTERY_PIN A0
#define REFERENCE_VOLTAGE 3.3

//Definition of GPIOS for Buttons
#define BUTTON_PIN_1 0   // GPIO 5 (entspricht D1) für Taster 1
#define BUTTON_PIN_2 2   // GPIO 4 (entspricht D2) für Taster 2
#define BUTTON_PIN_3 12  // GPIO 16 (entspricht D0) für Taster 3

#define YES     0
#define ABSTAIN 1
#define NO      2

#define BOOT        0
#define QUESTION    1
#define VOTE        2
#define CONFIRM     3
#define CLOSE_VOTE  4


// WLAN-Settings
const char* ssid = "Nadim";
const char* password = "nadimahmed";

// MQTT-Server Settings
const char* mqtt_server = "194.110.231.227";
const int mqtt_port = 1883;
const char* mqtt_user = "";
const char* mqtt_password = "";
#define MQTTpubQos 1                  
#define MQTTsubQos 1

//Mqtt topics to publish
const String pubInit = "/registration/Server/"+ String(WiFi.macAddress());
const char* pubPubVote = "/vote/";

// MQTT topics to subscribe
const String subInit = "/registration/esp/"+ String(WiFi.macAddress()); // /registration/esp/
const char* subVoteSetup = "/setupVote/Setup";
const char* subResync = "/setupVote/Resync";

//configuration of functionality
//#define ENCRYPTION

//Includes according to config
#ifdef ENCRYPTION
#include <encryptionlibrary.h>  //todo find encryption library
#endif

//Battery
#define FULL_BATTERY    25
#define EMPTY_BATTERY   0

