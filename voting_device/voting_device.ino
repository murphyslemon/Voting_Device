#include "config.h"
#include "power.h"
#include "button_interrupts.h"
#include "display.h"

// MQTT-Client erstellen
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

char MQTTmsg[255] = "";
bool MQTT_flag = 0;

// MQTT-Nachrichten verarbeiten
void callback(char* topic, byte* payload, unsigned int length) {
  //look into what topic is
  memcpy(MQTTmsg, payload, length);
  MQTTmsg[length] = '\0';
  MQTT_flag = 1;
}

//move this function to another folder
int checkBatteryLevel(){
  int adcValue = analogRead(BATTERY_PIN);
  float voltage = adcValue * REFERENCE_VOLTAGE / 1023;
  //int batteryLevel = map(constrain(voltage, 3.0, 9.0), 3.0, 9.0, 0, 100);
  int batteryLevel = map(voltage, 3.0, 9.0, 0, 100);
  return batteryLevel;
}

void setup() {
  //button initialization
  pinMode(BUTTON_PIN_1, INPUT_PULLUP);  // Taster 1 als Eingang mit Pull-up-Widerstand
  pinMode(BUTTON_PIN_2, INPUT_PULLUP);  // Taster 2 als Eingang mit Pull-up-Widerstand
  pinMode(BUTTON_PIN_3, INPUT_PULLUP);  // Taster 3 als Eingang mit Pull-up-Widerstand
  //LED initialization
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(RXPIN, INPUT_PULLUP);
  //Display initialization
  initDisplay();

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

void loop() {
    //powerOff(); // RXPIN dose not work as interrupt, So we put it in main as a function for power off
  int state = 0;
  int batteryPercentage = 20; //this may need to be global if we diplay screen in setup
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
        paintVoteScreen(voteTitle, batteryPercentage);
        strcat(pubTopicVoteResponse, pubPubVote);
        strcat(pubTopicVoteResponse, votingID);
        state = VOTE;
      }
      break;

    case VOTE:
      if (!digitalRead(12)) {
        delay(500);
        strcpy(response, "Yes");
        paintConfirmScreen(response, batteryPercentage);
        Serial.println("YES");
        state = CONFIRM;
      }
      else if (digitalRead(2)) {
        strcpy(response, "Pass");
        paintConfirmScreen(response, batteryPercentage);
        Serial.println("PASS");
        state = CONFIRM;
      }
      else if (digitalRead(12)) {
        strcpy(response, "No");
        paintConfirmScreen(response, batteryPercentage);
        Serial.println("NO");
        state = CONFIRM;
      }
      break;

    case CONFIRM:
      if (digitalRead(12)) {
        delay(500);
        mqttClient.publish(pubPubVote, response);
        state = CLOSE_VOTE;
      }
      else if (!digitalRead(0)){
        paintVoteScreen(voteTitle, batteryPercentage);
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
