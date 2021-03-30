#include "ToIoTwithLoRaWAN.h"
#include "config.h"
//#include <avr/wdt.h>

const sRFM_pins RFM_pins = {
  .CS = 15,
  .RST = D4, //16
  .DIO0 = D2,
  .DIO1 = D0,
  .DIO2 = -1,
  .DIO5 = -1,
};

int IN1=D3; // A-IA연결  A
int IN2=D1; // A-IB연결 /A
int pin_A0 = A0;

ToIoTwithLoRaWAN t;
float value = 0.0;

struct Actuator a1;

void setup() {
  t.setupToIoTwithLoRaWAN(nodeId, interval, 0);

  pinMode(pin_A0, INPUT);

  // Actuator Setting
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
  a1.actuatorId = 1;
  for(int i=0; i < a1.values_len; i++)
    a1.value[i] = 0;

  // Set LoRaWAN Class change CLASS_A or CLASS_C
  lora.setDeviceClass(CLASS_C);

  // Set Data Rate
  lora.setDataRate(SF8BW125);

  // set channel to random
  lora.setChannel(0);
  
  // Put OTAA Key and DevAddress here
  lora.setDevEUI(devEui);
  lora.setAppEUI(appEui);
  lora.setAppKey(appKey);

  // Join procedure
  bool isJoined;
  do {
    Serial.println(F("Joining..."));
    isJoined = lora.join();
    Serial.println(isJoined);
    //wait for 3s to try again
    delay(3000);
  }while(!isJoined);
  delay(1);
  Serial.println(F("Joined to network"));
}

void loop() {
  value = analogRead(A0);
  t.pub("7", 1, value);
  t.rcv();
  t.set_target_actuator(&a1);
  t.actuator_L9110(&a1, IN1, IN2);
  wdt_reset();
}