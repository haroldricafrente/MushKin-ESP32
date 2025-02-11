#define BLYNK_TEMPLATE_ID "TMPL6n0oBGr3j"
#define BLYNK_TEMPLATE_NAME "MushKin_v1"
#define BLYNK_AUTH_TOKEN "nPfV80LD8I3KSUIwDv1pV4yVNa03QYpb"

char auth[] = BLYNK_AUTH_TOKEN;
//char ssid[] = "OT7";  // type your wifi name
//char pass[] = "dIk0aLAm";  // type your wifi password
char ssid[] = "BawalAngHappy";  // type your wifi name
char pass[] = "0987654321";  // type your wifi password

bool fetch_blynk_state = true;  //true or false

   //#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <AceButton.h>
using namespace ace_button;
// define the GPIO connected with Relays and switches
#define RelayPin1 26  //light
#define RelayPin2 27  //fan
#define RelayPin3 14  //humidifier

#define SwitchPin1 19  //D13
#define SwitchPin2 18  //D13
#define SwitchPin3 12  //D13  

#define wifiLed   2   //D2

#define moistPIN 34
#define lightPIN 35
// define the gpio connected to DHT
#define DHTPIN 23 
#define DHTTYPE DHT22  
DHT dht(DHTPIN, DHTTYPE);

//Change the virtual pins according the rooms
#define VPIN_BUTTON_1    V5
#define VPIN_BUTTON_2    V6
#define VPIN_BUTTON_3    V7


// Relay State
bool toggleState_1 = LOW; //Define integer to remember the toggle state for relay 1
bool toggleState_2 = LOW; //Define integer to remember the toggle state for relay 2
bool toggleState_3 = LOW; //Define integer to remember the toggle state for relay 2

int wifiFlag = 0;

ButtonConfig config1;
AceButton button1(&config1);
ButtonConfig config2;
AceButton button2(&config2);
ButtonConfig config3;
AceButton button3(&config3);

void handleEvent1(AceButton*, uint8_t, uint8_t);
void handleEvent2(AceButton*, uint8_t, uint8_t);
void handleEvent3(AceButton*, uint8_t, uint8_t);

BlynkTimer timer;

// When App button is pushed - switch the state

BLYNK_WRITE(VPIN_BUTTON_1) {
  toggleState_1 = param.asInt();
  digitalWrite(RelayPin1, !toggleState_1);
}
BLYNK_WRITE(VPIN_BUTTON_2) {
  toggleState_2 = param.asInt();
  digitalWrite(RelayPin2, !toggleState_2);
}
BLYNK_WRITE(VPIN_BUTTON_3) {
  toggleState_3 = param.asInt();
  digitalWrite(RelayPin3, !toggleState_3);
}

void checkBlynkStatus() { // called every 3 seconds by SimpleTimer
  bool isconnected = Blynk.connected();
  if (isconnected == false) {
    wifiFlag = 1;
    Serial.println("Blynk Not Connected");
    digitalWrite(wifiLed, LOW);
  }
  if (isconnected == true) {
    wifiFlag = 0;
    if (!fetch_blynk_state){
    Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);
    Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2);
    Blynk.virtualWrite(VPIN_BUTTON_3, toggleState_3);
    }
    digitalWrite(wifiLed, HIGH);
  }
}

BLYNK_CONNECTED() {
  // Request the latest state from the server
  if (fetch_blynk_state){
    Blynk.syncVirtual(VPIN_BUTTON_1);
    Blynk.syncVirtual(VPIN_BUTTON_2);
    Blynk.syncVirtual(VPIN_BUTTON_3);
  }
}

void sendSensor()
{
  /*int soilmoisturevalue = analogRead(A0);
   soilmoisturevalue = map(soilmoisturevalue, 0, 1023, 0, 100);*/
   int value = analogRead(moistPIN);
  value = map(value,400,1023,100,0);
  int moist = value + 493;
  int lightInt = analogRead(lightPIN);
  float h = dht.readHumidity();
  float t = dht.readTemperature();


  // You can send any value at any time.
  // Please don't send more that 10 values per second.
    Blynk.virtualWrite(V0, moist);
    Blynk.virtualWrite(V1, t);
    Blynk.virtualWrite(V2, h);
    Blynk.virtualWrite(V3, lightInt);
    Serial.print("Light Intensity : ");
    Serial.print(lightInt);
    Serial.print("    Soil Moisture : ");
    Serial.print(moist);
    Serial.print("    Temperature : ");
    Serial.print(t);
    Serial.print("    Humidity : ");
    Serial.println(h);
}

void setup()
{
  Serial.begin(115200);

  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);
  pinMode(RelayPin3, OUTPUT);

  pinMode(wifiLed, OUTPUT);

  pinMode(SwitchPin1, INPUT_PULLUP);
  pinMode(SwitchPin2, INPUT_PULLUP);
  pinMode(SwitchPin3, INPUT_PULLUP);

  //During Starting all Relays should TURN OFF
  digitalWrite(RelayPin1, !toggleState_1);
  digitalWrite(RelayPin2, !toggleState_2);
  digitalWrite(RelayPin3, !toggleState_3);

  digitalWrite(wifiLed, LOW);

  config1.setEventHandler(button1Handler);
  config1.setEventHandler(button2Handler);
  config1.setEventHandler(button3Handler);

  button1.init(SwitchPin1);
  button2.init(SwitchPin2);
  button3.init(SwitchPin3);

  //Blynk.begin(auth, ssid, pass);
  WiFi.begin(ssid, pass);
  dht.begin();
  timer.setInterval(2000L, checkBlynkStatus); // check if Blynk server is connected every 2 seconds
  timer.setInterval(1000L, sendSensor);
  Blynk.config(auth);
  delay(1000);
  
  if (!fetch_blynk_state){
    Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);
    Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2);
    Blynk.virtualWrite(VPIN_BUTTON_3, toggleState_3);
  }
}

void loop()
{  
  Blynk.run();
  timer.run(); // Initiates SimpleTimer

  button1.check();
  button2.check();
  button3.check();
}

void button1Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT1");
  switch (eventType) {
    case AceButton::kEventReleased:
 
      digitalWrite(RelayPin1, toggleState_1);
      toggleState_1 = !toggleState_1;
      Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);
      break;
  }
}
void button2Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT2");
  switch (eventType) {
    case AceButton::kEventReleased:
      Serial.println("kEventReleased");
      digitalWrite(RelayPin2, toggleState_2);
      toggleState_2 = !toggleState_2;
      Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2);
      break;
  }
}
void button3Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT3");
  switch (eventType) {
    case AceButton::kEventReleased:
      Serial.println("kEventReleased");
      digitalWrite(RelayPin3, toggleState_3);
      toggleState_3 = !toggleState_3;
      Blynk.virtualWrite(VPIN_BUTTON_3, toggleState_3);
      break;
  }
}