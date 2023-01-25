#include <Servo.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <ThingSpeak.h>
#include <WiFiClient.h>

#define BLYNK_PRINT Serial

char Id[]="5Gj7Rt9VmZac10WsSD580ofVWEXjPast";
char Wifi[]="Iki";
char Pass[]="pecelenak";

// Thingspeak
char* apiTulis = "PEG4MYXQ7ATMSXTR";
char* apiBaca = "8LUV1XASOY1CG7JS";
const long channelID = 2006798;

int button1 = D2;
int buttonOn = 0;
int ledMerah = D5;
int ledIjo = D7;
int buzzer = D8;
int virtualValue = 0;   //Digunakan untuk menyimpan nilai Virtual PIN pada Blynk

Servo myservo;
WiFiClient client;

void setup() {

  pinMode(button1, INPUT);
  pinMode (ledMerah, OUTPUT);
  pinMode (ledIjo, OUTPUT);
  pinMode(buzzer, OUTPUT);

  myservo.attach(D9);
  myservo.write(180);

  Blynk.begin(Id,Wifi,Pass,"iot.amikom.ac.id",8080);
  Serial.begin(9600);
  WiFi.begin(Wifi, Pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Tidak connect....., try again!");
  }
  Serial.println("Connecting to Wifi");
  Serial.print("Connected, IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("OK MULAI");
  
}

void loop() {
  Blynk.run();  // Menjalankan Blynk
  int temp;     // variable untuk menyimpan nilai integer
  buttonOn = digitalRead(button1);  // membaca INPUT button1/Limit Switch dan menyimpannya ke variable buttonOn

//  Serial.print("button on: ");
//  Serial.println(buttonOn);

  if (virtualValue == 0) {  // Ketika virtualValue yang didapat dari BLYNK(V2) bernilai 0
    temp = virtualValue;    // temp akan menyimpan nilai virtualValue
    digitalWrite(ledMerah, HIGH);
    digitalWrite(ledIjo, LOW);
    delay(100);
    ThingSpeak.begin(client); //Inisialisasi ThingSpeak
    ThingSpeak.writeField(channelID,1,temp,apiTulis);   //Mengirim nilai temp ke thingSpeak channelID monitoring grafik 1
    Serial.println("Loker locked");
    myservo.write(180); 
    toneBunyi();  // Memanggil fungsi toneBunyi() untuk menyalakan buzzer
  }

  if (buttonOn == 0 && virtualValue == 1){
    temp = buttonOn;    // temp akan menyimpan nilai buttonOn
    digitalWrite (ledMerah, HIGH);
    digitalWrite (ledIjo, LOW);
    delay(100);
    Serial.println("Loker locked"); 
    toneBunyi();  // memanggil fungsi toneBunyi() untuk menyalakan buzzer
    myservo.write(180);
    ThingSpeak.begin(client);
    ThingSpeak.writeField(channelID,1,temp,apiTulis); //Mengirim nilai temp ke thingSpeak channelID monitoring grafik 1
    delay(100);
  } else if (buttonOn == 1 && virtualValue == 1) {
    temp = buttonOn;  // temp akan menyimpan nilai buttonOn
    digitalWrite (ledMerah, LOW);
    digitalWrite (ledIjo, HIGH);
    myservo.write(0);
    delay(100);
    Serial.println("Loker unlocked");  
    ThingSpeak.begin(client);
    ThingSpeak.writeField(channelID,1,temp,apiTulis); //Mengirim nilai temp ke thingSpeak channelID monitoring grafik 1
    delay(100);  
  }
  delay(1000);
}

// Fungsi TONE
void toneBunyi() {
    tone(buzzer,  13500);   // buzzer menyala dengan frekuensi 13500
    delay(1000);
    noTone(buzzer);         // buzzer mati
    delay(1000);
}

BLYNK_WRITE(V2) 
{
  virtualValue = param.asInt();   // Menyimpan nilai dari V2 yang dirubah menjadi INT ke dalam variable virtualValue
  if (virtualValue == 1) {
    toneBunyi();    // Untuk membunyikan buzzer, ketika loker terbuka/ virtualValue bernilai 1
  }
}
