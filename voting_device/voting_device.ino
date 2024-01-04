#include "config.h"

int message_count = 0;

// MQTT-Client erstellen
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// MQTT-Nachrichten verarbeiten
void callback(char* topic, byte* payload, unsigned int length) {
  // Nachricht ausgeben
  Serial.print("Nachricht empfangen: ");
  Serial.print(topic);
  Serial.print(" : ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  // Nachricht weiterleiten
  //mqttClient.publish("e", payload, length);
}

void setup() {
  pinMode(BUTTON_PIN_1, INPUT_PULLUP);  // Taster 1 als Eingang mit Pull-up-Widerstand
  pinMode(BUTTON_PIN_2, INPUT_PULLUP);  // Taster 2 als Eingang mit Pull-up-Widerstand
  pinMode(BUTTON_PIN_3, INPUT_PULLUP);  // Taster 3 als Eingang mit Pull-up-Widerstand

#ifdef ISRS_FOR_BUTTONS
  attachISR();
#endif

#ifdef STATUS_LEDS
  pinMode(LED_PIN_1, OUTPUT);  // LED 1 als Ausgang
  pinMode(LED_PIN_2, OUTPUT);  // LED 2 als Ausgang
  pinMode(LED_PIN_3, OUTPUT);  // LED 3 als Ausgang
#endif

#ifdef DEBUG
  Serial.begin(115200);
#endif
  delay(10);

// Verbindung zum WLAN herstellen
#ifdef DEBUG
  Serial.println("Verbindung zum WLAN herstellen");
#endif
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
#ifdef DEBUG
    Serial.print(".");
#endif
  }
#ifdef DEBUG
  Serial.println("Verbindung zum WLAN hergestellt");
#endif

  // Verbindung zum MQTT-Server herstellen
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(callback);
  mqttClient.subscribe(subInit, MQTTsubQos);
  mqttClient.subscribe(subResync, MQTTsubQos);
  mqttClient.subscribe(subVoteSetup, MQTTsubQos);

  while (!mqttClient.connected()) {
#ifdef DEBUG
    Serial.println("Verbindung zum MQTT-Server herstellen");
#endif
    mqttClient.connect("ESP8266", mqtt_user, mqtt_password);
    delay(500);
  }
#ifdef DEBUG
  Serial.println("Verbindung zum MQTT-Server hergestellt");
#endif
}

int state = 0;
int response;

bool username = true; // move to right location
bool question = true; //move to right location

void loop() {/* working progress, need to define pressed function and buttons
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
      state = 1;
    case VOTE:
      //display question
      if (button_A) {
        response = YES
        state = 2;
      }
      else if (button_B) {
        response = ABSTAIN
        state = 2;
      }
      else if (button_C) {
        response = NO
        state = 2;
      }
    case CONFIRM:
      if (button_A) {
        response = YES
        state = 3;
      }
      else if (button_C){
        response = NO
        state = 1;
      }
    case CLOSE_VOTE:
      //display closing thank you
      delay(5000);
  }*/
}
