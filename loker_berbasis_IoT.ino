#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>
#include <SPI.h>
#include <ThingSpeak.h>
#include <WiFiClient.h>

char Id[]="Bylnk_ID";
char Wifi[]="SSID";
char Pass[]="PASS_SSID";

// Thingspeak
char* apiTulis = "WRITE_API_THINGSPEAK";
char* apiBaca = "READ_API_THINGSPEAK";
const long channelID = 2005298;

Servo myservo;
WiFiClient client;

int led_kuning = D6;
int led_hijau = D7;
int led_merah = D5;
int buzzer = D8;
int sensor = 0;
int virtualValue = 0;


long bacaJarak(int trigPin, int echoPin) 
{
  pinMode(trigPin, OUTPUT); //Set trigger as outpu
  digitalWrite(trigPin, LOW); //generate frekuense set pin LOW
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(100);
  digitalWrite(trigPin, LOW);
  pinMode(echoPin, INPUT);
  return pulseIn(echoPin, HIGH); //reads the echo PIN
}

BLYNK_WRITE(V2) // virtual pin 2 on blynk app
{
    int pinValue = param.asInt();
    virtualValue = pinValue;
    if (pinValue == 1) {//if button on 
      myservo.write(0);// move servo to 58•
      digitalWrite(led_merah, LOW);
      digitalWrite(led_hijau, HIGH);
      tone(buzzer, 13500);
      delay(500);
      tone(buzzer, 0);
    } else {
        myservo.write(180); // move servo to 0•
        digitalWrite(led_merah, HIGH);
        digitalWrite(led_hijau, LOW);
        tone(buzzer, 13500);
        delay(500);
        tone(buzzer, 0);
        delay(1000);
    }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(led_hijau, OUTPUT);
  pinMode(led_merah, OUTPUT);
  pinMode(led_kuning, OUTPUT);
  pinMode(buzzer, OUTPUT);
  myservo.attach(D9);
  myservo.write(180);
  WiFi.begin(Wifi, Pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Tidak connect....., try again!");
  }
  Serial.println("Connecting to Wifi");
  Serial.print("Connected, IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("OK MULAI");
  delay(1000);
  Serial.println("LABEL,Data");
  Blynk.begin(Id,Wifi,Pass,"iot.amikom.ac.id",8080);
}

void loop() {
  // put your main code here, to run repeatedly:
  Blynk.run();
  int cm=0;
  int temp=0;
  cm = (0.03446 * bacaJarak(D15, D14))/2;
  temp=cm;
  sensor=temp;

  if (sensor <= 3 && virtualValue == 1) {
    delay(5000);
    myservo.write(180); // move servo to 180•
    digitalWrite(led_hijau, LOW);
    tone(buzzer, 13500);
    delay(1000);
    tone(buzzer, 0);
    Serial.print("DATA,");
    Serial.print(sensor);
    Serial.print(" cm");
    Serial.println(" ");
    delay(5000);
  } else if (sensor >= 4 && virtualValue == 1) {
    delay(1000);
    myservo.write(0);
    digitalWrite(led_hijau, HIGH);
  }
  Serial.print("DATA,");
  Serial.print(sensor);
  Serial.print(" cm");
  Serial.println(" ");
  ThingSpeak.begin(client);
  ThingSpeak.writeField(channelID,1,sensor,apiTulis);
  delay(500);
  //Serial.println(sensor);
}
