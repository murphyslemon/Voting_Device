#include "config.h"
#include "power.h"
#include "button_interrupts.h"
#include "display.h"

int message_count = 0;

// MQTT-Client erstellen
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

char MQTTmsg[] = "This is a test string for payload";
bool flag = 0;

// MQTT-Nachrichten verarbeiten
void callback(char* topic, byte* payload, unsigned int length) {
  memcpy(MQTTmsg, payload, length);
  MQTTmsg[length] = '\0';
  flag = 1;
}

int checkBatteryLevel(){
  int adcValue = analogRead(BATTERY_PIN);
  float voltage = adcValue * REFERENCE_VOLTAGE / 1023;
  //int batteryLevel = map(constrain(voltage, 3.0, 9.0), 3.0, 9.0, 0, 100);
  int batteryLevel = map(voltage, 3.0, 9.0, 0, 100);
  return batteryLevel;
}

void setup() {
  
  pinMode(BUTTON_PIN_1, INPUT_PULLUP);  // Taster 1 als Eingang mit Pull-up-Widerstand
  pinMode(BUTTON_PIN_2, INPUT_PULLUP);  // Taster 2 als Eingang mit Pull-up-Widerstand
  pinMode(BUTTON_PIN_3, INPUT_PULLUP);  // Taster 3 als Eingang mit Pull-up-Widerstand
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(RXPIN, INPUT_PULLUP);

  initDisplay();
  char question[100] = "How many characters can E-paper fit across?";
  char question2[] = "In this example, exampleString is a character array containing the string Hello, World!. The strlen function is then u Hello, World!.";
  //attachISR();

#ifdef DEBUG
  Serial.begin(115200);
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
int state = 0;
char response[10] = "";
char votingID[20] = "";
char pubTopicVoteResponse[50] = "";
char voteTitle[256] = "";

//bool username = true; // move to right location
//bool question = true; //move to right location

void loop() { //working progress, need to define pressed function and buttons
    //powerOff(); // RXPIN dose not work as interrupt, So we put it in main as a function for power off
  switch (state) {
    case BOOT:
  //display start up screen
      if (flag) {
        flag = 0;
        strcpy(votingID, MQTTmsg);
        Serial.println(MQTTmsg);
        state = QUESTION; 
      }
      //if (!question) {
        //error msg
        //request question
      //}
      //battery status
      break;
    case QUESTION:
      if (flag) {
        flag = 0;
        Serial.println(MQTTmsg);
        strcpy(voteTitle, MQTTmsg);
        paintVoteScreen(voteTitle);
        strcat(pubTopicVoteResponse, pubPubVote);
        strcat(pubTopicVoteResponse, votingID);
        state = VOTE;
      }
      break;
    case VOTE:
      //display question
      //strcpy(response, "Yes");
      //state = CONFIRM;
      if (!digitalRead(12)) {
        delay(500);
        strcpy(response, "Yes");
        Serial.println("YES");
        state = CONFIRM;
      }/*
      else if (digitalRead(2)) {
        strcpy(response, "Pass");
        Serial.println("PASS");
        state = CONFIRM;
      }
      else if (digitalRead(12)) {
        strcpy(response, "No");
        Serial.println("NO");
        state = CONFIRM;
      }*/
      break;
    case CONFIRM:
      if (digitalRead(12)) {
        delay(500);
        mqttClient.publish(pubPubVote, response);
        state = CLOSE_VOTE;
      }
      else if (!digitalRead(0)){
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
