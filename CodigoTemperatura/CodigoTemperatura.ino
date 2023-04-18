#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseESP8266.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define WIFI_SSID "Teste"
#define WIFI_PASSWORD "12345678"
#define FIREBASE_HOST "https://projectx-967f0-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "AIzaSyDSEo23WOOlr_qoNzjlqoxOcNGSZW3zV7E"

#define TEMPERATURE_PIN D2

FirebaseData firebaseData;

float temperature;
float Corrente;
bool device_on = true;

OneWire oneWire(TEMPERATURE_PIN);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(9600);

  pinMode(LED_BUILTIN,OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  sensors.begin();

  Firebase.setBool(firebaseData, "/projectX/device_on", device_on);

}

void loop() {
  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0);


  Firebase.setFloat(firebaseData, "/projectX/temperature", temperature);


  Corrente = analogRead(A0) * (1.1 /1023.00);
  Firebase.setFloat(firebaseData, "/projectX/correntear", Corrente );

  if (Firebase.get(firebaseData, "/projectX/device_on")) {
    String tratando = firebaseData.stringData();
    tratando.replace("""","");
    device_on = tratando.toInt();
  }

  if (!device_on) {
    digitalWrite(LED_BUILTIN, HIGH);
  }else{
    digitalWrite(LED_BUILTIN, LOW);
  }

  delay(100);
}