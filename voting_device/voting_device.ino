#include "config.h"
#include "power.h"
#include "button_interrupts.h"
#include "display.h"

//REMEMBER TO REMOVE ALL SERIAL RELATED STUFF AS IT AFFECTS RX PIN.

// MQTT-Client erstellen
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

char MQTTmsg[255] = "";
bool MQTT_flag = 0;

// MQTT-Nachrichten verarbeiten
void callback(char* topic, byte* payload, unsigned int length) {
  //look into what topic is
  //add specific topic flags here
  memcpy(MQTTmsg, payload, length);
  MQTTmsg[length] = '\0';
  MQTT_flag = 1;
}

//move this function to another folder
int checkBatteryLevel(){
  int adcValue = analogRead(BATTERY_PIN);
  if (adcValue > 440) {
    return FULL_BATTERY;
  }
  return EMPTY_BATTERY;
}

bool debounce(int pin) {
  static uint16_t state = 0;
  state = (state<<1) | digitalRead(pin) | 0xfe00;
  return (state == 0xff00);
}

void setup() {
  //button initialization
  pinMode(BUTTON_PIN_1, INPUT_PULLUP);
  pinMode(BUTTON_PIN_2, INPUT_PULLUP);
  pinMode(BUTTON_PIN_3, INPUT_PULLUP);
#ifdef DEBUG
  Serial.begin(115200);
#endif
  //why delay
  delay(10);
  //LED initialization
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);
  //Display initialization
  initDisplay();
#ifndef DEBUG
  pinMode(RXPIN, INPUT_PULLUP); //COMMMENT OUT THIS LINE IF YOU ARE USING SERIAL
#endif

#ifdef DEBUG
  Serial.println("Connecting to WiFi");
#endif
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
#ifdef DEBUG
    Serial.print(".");
#endif
  }
#ifdef DEBUG
  Serial.println("Connected to WiFi established");
#endif

  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(callback);

  while (!mqttClient.connected()) {
#ifdef DEBUG
    Serial.println("Connecting to the MQTT server");
#endif
    mqttClient.connect("ESP8266", mqtt_user, mqtt_password);
    delay(500); //why is this here
  }
  String macAddress = "Mac : " + String(WiFi.macAddress());
#ifdef DEBUG
  Serial.println("Connection to MQTT server established");
  Serial.println((WiFi.macAddress()).c_str());
  //String rawMacAddress = String(WiFi.macAddress());
  //String Nadim = "/registration/Server/"+ String(WiFi.macAddress());
  Serial.println(macAddress);
#endif
    mqttClient.publish(pubInit.c_str(), macAddress.c_str());  //send mac address
    mqttClient.subscribe(subInit.c_str(), MQTTsubQos); //recieve voting ID
    //mqttClient.subscribe(subResync, MQTTsubQos);
    mqttClient.subscribe(subVoteSetup, MQTTsubQos); //recieve question
}

  int state = 0;
  int batteryPercentage = checkBatteryLevel(); //this may need to be global if we diplay screen in setup
  char response[10] = "";
  char votingID[20] = "";
  char pubTopicVoteResponse[50] = "";
  char voteTitle[256] = "";

void loop() {
#ifndef DEBUG
    powerOff(); // RXPIN dose not work as interrupt, So we put it in main as a function for power off
#endif
  switch (state) {
    case BOOT:
  //display start up screen
      if (MQTT_flag) {
        MQTT_flag = 0;
        strcpy(votingID, MQTTmsg);
#ifdef DEBUG
        Serial.println(MQTTmsg);
#endif
        state = QUESTION; 
      }
      break;

    case QUESTION:
      if (MQTT_flag) {
        MQTT_flag = 0;
#ifdef DEBUG
        Serial.println(MQTTmsg);
#endif
        strcpy(voteTitle, MQTTmsg);
        paintVoteScreen(voteTitle, batteryPercentage);
        strcat(pubTopicVoteResponse, pubPubVote);
        strcat(pubTopicVoteResponse, votingID);
        state = VOTE;
      }
      break;

    case VOTE:
      if (debounce(!BUTTON_PIN_1)) {
        delay(500);
        strcpy(response, "Yes");
        paintConfirmScreen(response, batteryPercentage);
#ifdef DEBUG
        Serial.println("YES");
#endif
        state = CONFIRM;
        while (!debounce(BUTTON_PIN_1));
      }
      else if (!debounce(BUTTON_PIN_2)) {
        strcpy(response, "Pass");
        paintConfirmScreen(response, batteryPercentage);
#ifdef DEBUG
        Serial.println("PASS");
#endif
        state = CONFIRM;
        while (!debounce(BUTTON_PIN_2));
      }
      else if (debounce(BUTTON_PIN_3)) {
        strcpy(response, "No");
        paintConfirmScreen(response, batteryPercentage);
#ifdef DEBUG
        Serial.println("NO");
#endif
        state = CONFIRM;
        while (debounce(BUTTON_PIN_1));
      }
      break;

    case CONFIRM:
      if (!debounce(BUTTON_PIN_1)) {
        delay(500);
        mqttClient.publish(pubPubVote, response);
        state = CLOSE_VOTE;
        while (!debounce(BUTTON_PIN_1));
      }
      else if (!debounce(BUTTON_PIN_3)){
        paintVoteScreen(voteTitle, batteryPercentage);
        state = VOTE;
        while (!debounce(BUTTON_PIN_3));
      }
      break;

    case CLOSE_VOTE:
      //display closing thank you
#ifdef DEBUG
      Serial.println("voting ending");
#endif
      delay(2000);
      state = BOOT;
      break;
  }
  mqttClient.loop();
}
