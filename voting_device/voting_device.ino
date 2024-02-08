#include "config.h"
#include "power.h"
#include "button_interrupts.h"
//#include "display.h"

#define btn1 0
#define btn2 2
#define btn3 12

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

void ICACHE_RAM_ATTR btnIRQ(){
  delay(30);
  if (!digitalRead(btn1)){
    Serial.println("btn1");
    while (!digitalRead(btn1));
  }
  if (!digitalRead(btn2)){
    Serial.println("btn2");
    while (!digitalRead(btn2));
  }
  if (!digitalRead(btn3)){
    Serial.println("btn3");
    while (!digitalRead(btn3));
  }
}

void setup() {
  //button initialization
  pinMode(btn1, INPUT_PULLUP);
  pinMode(btn2, INPUT_PULLUP);
  pinMode(btn3, INPUT_PULLUP);
  Serial.begin(115200);
  attachInterrupt(digitalPinToInterrupt(btn1), btnIRQ, FALLING);
  attachInterrupt(digitalPinToInterrupt(btn2), btnIRQ, FALLING);
  attachInterrupt(digitalPinToInterrupt(btn3), btnIRQ, FALLING);
  //LED initialization
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);
#ifndef DEBUG
  //pinMode(RXPIN, INPUT_PULLUP); //COMMMENT OUT THIS LINE IF YOU ARE USING SERIAL
#endif
  //Display initialization
  //initDisplay();

  //attachISR();

#ifdef DEBUG
  //Serial.begin(115200);
#endif
  delay(10);
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
    delay(500);
  }
#ifdef DEBUG
  Serial.println("Connection to MQTT server established");
  Serial.println((WiFi.macAddress()).c_str());
  //String rawMacAddress = String(WiFi.macAddress());
  String macAddress = "Mac : " + String(WiFi.macAddress());
  //String Nadim = "/registration/Server/"+ String(WiFi.macAddress());
  
Serial.println(macAddress);
#endif
    mqttClient.publish(pubInit.c_str(), macAddress.c_str());  //send mac address
    mqttClient.subscribe(subInit.c_str(), MQTTsubQos); //recieve voting ID
    //mqttClient.subscribe(subResync, MQTTsubQos);
    mqttClient.subscribe(subVoteSetup, MQTTsubQos); //recieve question
}

void loop() {
    //powerOff(); // RXPIN dose not work as interrupt, So we put it in main as a function for power off
  int state = 0;
  int batteryPercentage = checkBatteryLevel(); //this may need to be global if we diplay screen in setup
  char response[10] = "";
  char votingID[20] = "";
  char pubTopicVoteResponse[50] = "";
  char voteTitle[256] = "";
    
  switch (state) {
    case BOOT:
  //display start up screen
      if (MQTT_flag) {
        MQTT_flag = 0;
        strcpy(votingID, MQTTmsg);
        Serial.println(MQTTmsg);
        state = QUESTION; 
      }
      //battery status
      break;

    case QUESTION:
      if (MQTT_flag) {
        MQTT_flag = 0;
        Serial.println(MQTTmsg);
        strcpy(voteTitle, MQTTmsg);
        //paintVoteScreen(voteTitle, batteryPercentage);
        strcat(pubTopicVoteResponse, pubPubVote);
        strcat(pubTopicVoteResponse, votingID);
        state = VOTE;
      }
      break;

    case VOTE:
      if (!digitalRead(0)) {
        delay(500);
        strcpy(response, "Yes");
        //paintConfirmScreen(response, batteryPercentage);
        Serial.println("YES");
        state = CONFIRM;
      }
      else if (digitalRead(2)) {
        strcpy(response, "Pass");
        //paintConfirmScreen(response, batteryPercentage);
        Serial.println("PASS");
        state = CONFIRM;
      }
      else if (digitalRead(12)) {
        strcpy(response, "No");
        //paintConfirmScreen(response, batteryPercentage);
        Serial.println("NO");
        state = CONFIRM;
      }
      break;

    case CONFIRM:
      if (digitalRead(0)) {
        delay(500);
        mqttClient.publish(pubPubVote, response);
        state = CLOSE_VOTE;
      }
      else if (!digitalRead(12)){
        //paintVoteScreen(voteTitle, batteryPercentage);
        state = VOTE;
      }
      break;

    case CLOSE_VOTE:
      //display closing thank you
      Serial.println("voting ending");
      delay(2000);
      state = BOOT;
      break;
  }
  mqttClient.loop();
}
