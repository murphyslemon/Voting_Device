#include "config.h"
#include "power.h"
#include "display.h"
#include "button_interrupts.h"
#include <ArduinoJson.h>

//#################################################################################################
//### REMEMBER TO COMMENT OUT: Serial.begin(115200); FROM SETUP FUNCTION, AS IT AFFECTS RX PIN. ###
//#################################################################################################

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

int batteryPercentage = checkBatteryLevel();

void setup() {
  Serial.begin(115200);
  //button initialization
  pinMode(BUTTON_PIN_1, INPUT_PULLUP);  // Taster 1 als Eingang mit Pull-up-Widerstand
  pinMode(BUTTON_PIN_2, INPUT_PULLUP);  // Taster 2 als Eingang mit Pull-up-Widerstand
  pinMode(PWR_PIN, OUTPUT);
  pinMode(RX_PIN, INPUT_PULLUP);
  digitalWrite(PWR_PIN, HIGH); //power on device
  
  //Display initialization
  initDisplay();
  startupScreen(batteryPercentage);

  WiFi.begin(ssid, password);
  connectToWiFi();
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(callback);
  connectToMQTT();

  String macAddress = "{\"Mac\" : \"" + String(WiFi.macAddress()) + "\"}";
  Serial.println(macAddress);
  mqttClient.publish(pubInit.c_str(), macAddress.c_str());  //send mac address
  mqttClient.subscribe(subInit.c_str(), MQTTsubQos); //recieve voting ID
  Serial.println("#" + String(subInit.c_str()) + "#") ;
  //mqttClient.subscribe(subResync, MQTTsubQos);
  mqttClient.subscribe(subVoteSetup, MQTTsubQos); //recieve question
}

  int state = 0;
  //int batteryPercentage = checkBatteryLevel(); //this may need to be global if we diplay screen in setup
  //int batteryPercentage = 20;
  char response[STRINGSIZE] = "";
  char votingID[STRINGSIZE] = "";
  char pubTopicVoteResponse[STRINGSIZE] = "";
  char voteTitle[STRINGSIZE] = "";
  char responseBuffer[STRINGSIZE];
  
void loop() {
  powerOff();
  switch (state) {
    case BOOT:
  //display start up screen
      if (MQTT_flag) {
        MQTT_flag = 0;
        strcpy(votingID, MQTTVotingId);
        Serial.println(votingID);
        state = QUESTION; 
      }
      break;

    case QUESTION:
      if (MQTT_flag) {
        MQTT_flag = 0;
        strcpy(voteTitle, MQTTVotingTitle);
        Serial.println(voteTitle);
        paintVoteScreen(voteTitle);
        strcat(pubTopicVoteResponse, pubPubVote);
        strcat(pubTopicVoteResponse, votingID);
        state = VOTE;
      }
      break;

    case VOTE:
      if (!digitalRead(BUTTON_PIN_1)) {
        delay(30);
        if (!digitalRead(BUTTON_PIN_1)){
          Serial.println("I am in Button1: Vote");
          snprintf(responseBuffer, sizeof(responseBuffer), "{\"vote\":\"Yes\", \"VoteTitle\":\"%s\"}", MQTTVotingTitle);
          strcpy(response, responseBuffer);
          paintConfirmScreen("YES");
          Serial.println("YES");
          state = CONFIRM;
          while (!digitalRead(BUTTON_PIN_1)){
            delay(100);
          }
        }      
      }
      else if (!digitalRead(BUTTON_PIN_2)) {
        delay(30);
        if (!digitalRead(BUTTON_PIN_2)){
          snprintf(responseBuffer, sizeof(responseBuffer), "{\"vote\":\"Abstain\", \"VoteTitle\":\"%s\"}", MQTTVotingTitle);
          strcpy(response, responseBuffer);
          paintConfirmScreen("ABSTAIN");
          Serial.println("ABSTAIN");
          state = CONFIRM;
          while (!digitalRead(BUTTON_PIN_2)){
            delay(100);
          }
        }
      }
      else if (!digitalRead(BUTTON_PIN_3)) {
        delay(30);
        if (!digitalRead(BUTTON_PIN_3)){
          snprintf(responseBuffer, sizeof(responseBuffer), "{\"vote\":\"No\", \"VoteTitle\":\"%s\"}", MQTTVotingTitle);
          strcpy(response, responseBuffer);
          paintConfirmScreen("NO");
          Serial.println("NO");
          state = CONFIRM;
          while (!digitalRead(BUTTON_PIN_3)){
            delay(100);
          }
        }
      }
      break;

    case CONFIRM:
    // ButtonYes.getState()
      if (!digitalRead(BUTTON_PIN_1)) {
        delay(30);
        if (!digitalRead(BUTTON_PIN_1)){
          Serial.println("I am in Button1: Confirm");
          mqttClient.publish(pubPubVote, response);
          state = CLOSE_VOTE;
          while (!digitalRead(BUTTON_PIN_1)){
            delay(100);
          }
        }
      }
      else if (!digitalRead(BUTTON_PIN_3)) {
        delay(30);
        if (!digitalRead(BUTTON_PIN_3)){
          paintVoteScreen(voteTitle);
          state = VOTE;
          while (!digitalRead(BUTTON_PIN_3)){
            delay(100);
          }
        }
      }
      break;

    case CLOSE_VOTE:
      delay(2000);
      paintClosingScreen();
      state = QUESTION;
      break;
  }

 mqttClient.loop();
}
