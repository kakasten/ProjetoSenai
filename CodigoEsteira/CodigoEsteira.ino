#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WiFiClientSecure.h>
#include <FirebaseESP8266.h>

#define WIFI_SSID "Teste"
#define WIFI_PASSWORD "12345678"
#define FIREBASE_HOST "https://projectx-967f0-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "AIzaSyDSEo23WOOlr_qoNzjlqoxOcNGSZW3zV7E"

FirebaseData firebaseData;

#define BOTAO 12
#define BOTAOINTERRUPT D6
#define SAIDA 13

float TensaoSensor;
float corrente;

bool botaoemergencia = false;
bool onOff = false;

WiFiUDP udp;
NTPClient ntp(udp, "a.st1.ntp.br", -3 * 3600, 60000);

String HoraComeco;
String HoraFinal;
int MostraHora;
String PassouUmDia; 

int calcula1;
int calcula2;
int calcula3;

void setup() {
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

  ntp.begin();
  ntp.forceUpdate();

  pinMode(BOTAO, INPUT_PULLUP);
  pinMode(BOTAOINTERRUPT, INPUT_PULLUP);
  pinMode(SAIDA, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(BOTAOINTERRUPT), botao_emergencia_acionado, LOW );

}

void loop() {
  botaoemergencia = false;
  Firebase.setBool(firebaseData, "/projectX/botaoemergencia", botaoemergencia);

  TensaoSensor = analogRead(A0) * (1.1 / 1023.0);
  corrente = TensaoSensor * 20;
  Firebase.setFloat(firebaseData, "/projectX/correntemotor", corrente);


  if (digitalRead(BOTAO) == 0){
    onOff = !onOff;
    digitalWrite(SAIDA, onOff);
    if(onOff){
      Firebase.setString(firebaseData, "/projectX/hora", MostraHora);
      HoraComeco =ntp.getFormattedTime();
      HoraComeco.replace(":","");
      calcula1 = HoraComeco.toInt();

    }else{
      HoraFinal = ntp.getFormattedTime();
      HoraFinal.replace(":","");
      calcula2 = HoraFinal.toInt();
      MostraHora = calcula2 - calcula1;
      Firebase.setString(firebaseData, "projectX/hora", String(MostraHora));
    }
  }
  
  PassouUmDia = ntp.getFormattedTime();
  PassouUmDia.replace(":","");
  calcula3 = PassouUmDia.toInt();
  if(PassouUmDia == "00:00:00"){
    HoraComeco = calcula1 - calcula3;
  }
}

void botao_emergencia_acionado(){
  botaoemergencia = true;
  Firebase.setBool(firebaseData, "/projectX/botaoemergencia", botaoemergencia);
}
