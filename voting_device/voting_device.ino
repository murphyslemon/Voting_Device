#include "config.h"
#include "power.h"
#include "button_interrupts.h"
#include "display.h"

int message_count = 0;

// MQTT-Client erstellen
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// MQTT-Nachrichten verarbeiten
void callback(char* topic, byte* payload, unsigned int length) {
  // Nachricht ausgeben
  Serial.print("Message received: ");
  Serial.print(topic);
  Serial.print(" : ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
 // mqttClient.publish("e", payload, length);
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
  paintVoteScreen(question2);

#ifdef ISRS_FOR_BUTTONS
  attachISR();
#endif

#ifdef DEBUG
  Serial.begin(115200);
#endif
  delay(10);

// Verbindung zum WLAN herstellen
#ifdef DEBUG
  Serial.println("Connect to WiFi");
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

  // Verbindung zum MQTT-Server herstellen
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(callback);

  while (!mqttClient.connected()) {
#ifdef DEBUG
    Serial.println("Connect to the MQTT server");
#endif
    mqttClient.connect("ESP8266", mqtt_user, mqtt_password);
    delay(500);
  }
  mqttClient.publish("e", "payload");
#ifdef DEBUG
  Serial.println("Connection to MQTT server established");
#endif
    mqttClient.subscribe(subInit, MQTTsubQos);
    mqttClient.subscribe(subResync, MQTTsubQos);
    mqttClient.subscribe(subVoteSetup, MQTTsubQos);
}

int state = 0;
int response;

bool username = true; // move to right location
bool question = true; //move to right location

void loop() { //working progress, need to define pressed function and buttons
    //powerOff(); // RXPIN dose not work as interrupt, So we put it in main as a function for power off
    mqttClient.loop();
    /*
  switch (state) {
    
    case BOOT:
      //display start up screen
      
      //check connection
      if (WiFi.status() != WL_CONNECTED) {
        //error msg
        //try connect again
      } 
      //receive user name and vote question
      if (!username) {
        //error msg
        //request user name
      }
      if (!question) {
        //error msg
        //request question
      }
      //battery status
      
      state = VOTE;
    case VOTE:
      //display question
      if (ButtonYes.getState()) {
        Serial.println("YES");
        response = YES;
        state = CONFIRM;
      }
      else if (ButtonAbstain.getState()) {
        Serial.println("ABSTAIN");
        response = ABSTAIN;
        state = CONFIRM;
      }
      else if (ButtonNo.getState()) {
        Serial.println("NO");
        response = NO;
        state = CONFIRM;
      }
    case CONFIRM:
      if (ButtonYes.getState()) {
        Serial.println("YES");
        response = YES;
        state = CLOSE_VOTE;
      }
      else if (ButtonNo.getState()){
        Serial.println("NO");
        response = NO;
        state = VOTE;
      }
    case CLOSE_VOTE:
      //display closing thank you
      delay(5000);
  }*/
}
