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
  mqttClient.subscribe("#");

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

void loop() {
  if (digitalRead(BUTTON_PIN_1) == LOW) {
    digitalWrite(LED_PIN_1, HIGH);  // Wenn Taster 1 gedrückt wird, schalte LED 1 ein
    if (message_count == 0) {
      mqttClient.publish("send", "Taster 1!");
      message_count++;
    }
  } else {
    message_count = 0;
    digitalWrite(LED_PIN_1, LOW);  // Andernfalls schalte LED 1 aus
  }

  if (digitalRead(BUTTON_PIN_2) == LOW) {
    digitalWrite(LED_PIN_2, HIGH);  // Wenn Taster 2 gedrückt wird, schalte LED 2 ein
  } else {
    digitalWrite(LED_PIN_2, LOW);  // Andernfalls schalte LED 2 aus
  }

  if (digitalRead(BUTTON_PIN_3) == LOW) {
    digitalWrite(LED_PIN_3, HIGH);  // Wenn Taster 3 gedrückt wird, schalte LED 3 ein
  } else {
    digitalWrite(LED_PIN_3, LOW);  // Andernfalls schalte LED 3 aus
  }
  // MQTT-Nachrichten abrufen
  if (!mqttClient.loop()) {
#ifdef DEBUG
    Serial.println("Verbindung zum MQTT-Server abgebrochen");
#endif
  }
}
