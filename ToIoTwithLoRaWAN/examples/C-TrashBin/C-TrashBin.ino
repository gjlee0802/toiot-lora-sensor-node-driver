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
int trigPin = D3;
int echoPin = D1;

ToIoTwithLoRaWAN t;

void setup() {
  t.setupToIoTwithLoRaWAN(nodeId, interval, 0);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

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
    Serial.println("Joining...");
    isJoined = lora.join();
    Serial.println(isJoined);
    //wait for 3s to try again
    delay(3000);
  }while(!isJoined);
  delay(1);
  Serial.println("Joined to network");
}

double MyPreviousMillis;
double MyInterval = 10;
double distance;
double MAX_distance=50.0;

void ultrasonic() {
  float duration;
  
  if(millis() - MyPreviousMillis > MyInterval) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // echoPin 이 HIGH를 유지한 시간을 저장 한다.
    duration = pulseIn(echoPin, HIGH);
    // HIGH 였을 때 시간(초음파가 보냈다가 다시 들어온 시간)을 가지고 거리를 계산 한다.
    // 340은 초당 초음파(소리)의 속도, 10000은 밀리세컨드를 세컨드로, 왕복거리이므로 2로 나눠준다.
    distance = ((double)(340 * duration) / 10000) / 2;
   
    //시리얼모니터에 Echo가 HIGH인 시간 및 거리를 표시해준다.
    //Serial.print("Duration:");
    //Serial.print(duration);
    Serial.print("\nDIstance:");
    Serial.print(distance);
    Serial.println("cm\n");
  
    MyPreviousMillis = millis();
  }
}


double value;
void loop() {
  ultrasonic();
  value = (distance/MAX_distance)*100;
  t.pub("8", 1, value);
  t.rcv();
  wdt_reset();
}