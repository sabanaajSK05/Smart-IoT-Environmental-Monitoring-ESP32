#include <WiFi.h>
#include "DHT.h"

// WiFi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// ThingSpeak
String apiKey = "80V2CHCVQ12O3X57";
const char* server = "api.thingspeak.com";

// Pins
#define DHTPIN 4
#define DHTTYPE DHT22
#define PIR_PIN 13
#define IR_PIN 12
#define LDR_PIN 34
#define LED_PIN 2
#define BUTTON_PIN 14
#define BUZZER_PIN 27

DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;

void setup() {
  Serial.begin(115200);

  pinMode(PIR_PIN, INPUT);
  pinMode(IR_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  dht.begin();

  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected!");
}

void loop() {

  float temp = dht.readTemperature();
  int pir = digitalRead(PIR_PIN);
  int ir = digitalRead(IR_PIN);
  int ldr = analogRead(LDR_PIN);
  int button = digitalRead(BUTTON_PIN);

  Serial.println("Temp: " + String(temp));
  Serial.println("PIR: " + String(pir));
  Serial.println("IR: " + String(ir));
  Serial.println("LDR: " + String(ldr));

  // Light control
  if (ldr < 500 || pir == HIGH) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  // Intrusion alert (IR)
  if (ir == LOW) {
    digitalWrite(BUZZER_PIN, HIGH);
    for (int i = 0; i < 3; i++) {
      digitalWrite(LED_PIN, HIGH);
      delay(200);
      digitalWrite(LED_PIN, LOW);
      delay(200);
    }
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }

  // Emergency button
  if (button == LOW) {
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
    delay(1000);
    digitalWrite(BUZZER_PIN, LOW);
  }

  // Send to ThingSpeak
  if (client.connect(server, 80)) {
    String url = "/update?api_key=" + apiKey +
                 "&field1=" + String(temp) +
                 "&field2=" + String(pir) +
                 "&field3=" + String(ldr);

    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + server + "\r\n" +
                 "Connection: close\r\n\r\n");
  }

  delay(15000);
}
