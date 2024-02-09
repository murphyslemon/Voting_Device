#include "config.h"
#include "power.h"
#include "display.h"
#include <ArduinoJson.h>

//REMEMBER TO REMOVE ALL SERIAL RELATED STUFF AS IT AFFECTS RX PIN.

// MQTT-Client erstellen
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
const size_t JSON_CAPACITY = 256;
char MQTTmsg[STRINGSIZE] = "";
bool MQTT_flag = 0;
char MQTTVotingId [STRINGSIZE]= "";
char MQTTVotingTitle [STRINGSIZE]= "";

// MQTT Callback Function
void callback(char* topic, byte* payload, unsigned int length) {
  // Copy payload bytes to MQTTmsg
  memcpy(MQTTmsg, payload, length);
  MQTTmsg[length] = '\0'; // Ensure null-termination for string

  // Parse JSON
  StaticJsonDocument<JSON_CAPACITY> doc;
  DeserializationError error = deserializeJson(doc, MQTTmsg);
  
  // If parsing succeeds, extract the necessary fields
  if (!error) {
    if (doc.containsKey("VotingID")) {
      const char* votingID = doc["VotingID"];
      Serial.print("Received Voting ID: ");
      Serial.println(votingID);
      memcpy(MQTTVotingId, votingID, strlen(votingID) + 1);
      // Optionally, you can set a flag to indicate that a new ID has been received
      MQTT_flag = true;
    }
    
    if (doc.containsKey("VoteTitle")) {
      const char* voteTitle = doc["VoteTitle"];
      Serial.print("Received Vote Title: ");
      Serial.println(voteTitle);
      memcpy(MQTTVotingTitle, voteTitle, strlen(voteTitle) + 1);
      // Optionally, you can set a flag to indicate that a new title has been received
      MQTT_flag = true;
    }
    
    // Add more conditions to extract other fields as needed
  } else {
    Serial.print("Failed to parse JSON: ");
    Serial.println(error.c_str());
  }
}
/*
// MQTT Callback Function
void callback(char* topic, byte* payload, unsigned int length) {
  memcpy(MQTTmsg, payload, length);
  MQTTmsg[length] = '\0';
  MQTT_flag = 1;
}
*/

//move this function to another folder
int checkBatteryLevel(){
  int adcValue = analogRead(BATTERY_PIN);
  if (adcValue > 440) {
    return FULL_BATTERY;
  }
  return EMPTY_BATTERY;
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}
void connectToMQTT() {
  while (!mqttClient.connected()) {
    Serial.println("Connecting to the MQTT server");
    mqttClient.connect("ESP8266", mqtt_user, mqtt_password);
    delay(500);
  }
  Serial.println("Connection to MQTT server established");
}

void setup() {

  Serial.begin(115200);
  //button initialization
  pinMode(BUTTON_PIN_1, INPUT_PULLUP);  // Taster 1 als Eingang mit Pull-up-Widerstand
  pinMode(BUTTON_PIN_2, INPUT_PULLUP);  // Taster 2 als Eingang mit Pull-up-Widerstand
  pinMode(BUTTON_PIN_3, INPUT_PULLUP);  // Taster 3 als Eingang mit Pull-up-Widerstand
  
  //LED initialization
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);
  //pinMode(RXPIN, INPUT_PULLUP); //COMMMENT OUT THIS LINE IF YOU ARE USING SERIAL

  //Display initialization
  //initDisplay();

  //attachISR();
  WiFi.begin(ssid, password);
  connectToWiFi();
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(callback);
  connectToMQTT();
  Serial.println("Connection to MQTT server established");
  String macAddress = "{\"Mac\" : \"" + String(WiFi.macAddress()) + "\"}";
  
  Serial.println(macAddress);
  mqttClient.publish(pubInit.c_str(), macAddress.c_str());  //send mac address
  mqttClient.subscribe(subInit.c_str(), MQTTsubQos); //recieve voting ID
  Serial.println("#" + String(subInit.c_str()) + "#") ;
  //mqttClient.subscribe(subResync, MQTTsubQos);
  mqttClient.subscribe(subVoteSetup, MQTTsubQos); //recieve question
  // mqttClient.subscribe("/registration/esp/C8:C9:A3:1B:ED:5F", MQTTsubQos);
}

  int state = 0;
  int batteryPercentage = checkBatteryLevel(); //this may need to be global if we diplay screen in setup
  char response[STRINGSIZE] = "";
  char votingID[STRINGSIZE] = "";
  char pubTopicVoteResponse[STRINGSIZE] = "";
  char voteTitle[STRINGSIZE] = "";
   

void loop() {
  switch (state) {
    case BOOT:
  //display start up screen
      if (MQTT_flag) {
        MQTT_flag = 0;
        strcpy(votingID, MQTTVotingId);
        Serial.println(votingID);
        state = QUESTION; 
      }
      //battery status
      break;

    case QUESTION:
      if (MQTT_flag) {
        MQTT_flag = 0;
        strcpy(voteTitle, MQTTVotingTitle);
        Serial.println(voteTitle);
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
      else if (!digitalRead(2)) {
        strcpy(response, "Pass");
        //paintConfirmScreen(response, batteryPercentage);
        Serial.println("PASS");
        state = CONFIRM;
      }
      else if (!digitalRead(12)) {
        strcpy(response, "No");
        //paintConfirmScreen(response, batteryPercentage);
        Serial.println("NO");
        state = CONFIRM;
      }
      break;

    case CONFIRM:
      if (!digitalRead(0)) {
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
