#include "ToIoTwithLoRaWAN.h"
#include "config.h"

const sRFM_pins RFM_pins = {
  .CS = 15,
  .RST = D4, //16
  .DIO0 = D2,
  .DIO1 = D0,
  .DIO2 = -1,
  .DIO5 = -1,
};

ToIoTwithLoRaWAN t;
double value = 0.0; 
// Actuator 2
struct Actuator a2;
Servo myservo;
// PIR
int pirPin = D1; //D1    // 센서 신호핀
int pirState = LOW;   // 센서 초기상태는 움직임이 없음을 가정
int pirVal = 0;          // 센서 신호의 판별을 위한 변수
double pirCnt = 0;   

//cds
int cdsPin1 = A0;
int cdsPin2 = A1;
double cdsVal1 = 0;
double cdsVal2 = 0;
// LED
int ledPin = D3;

void setup() {
  pinMode(pirPin, INPUT);
  pinMode(cdsPin1, INPUT);
  pinMode(cdsPin2, INPUT);
  pinMode(ledPin, OUTPUT);
  
  t.setupToIoTwithLoRaWAN(nodeId, interval, 0);

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

  // Actuator setting
  a2.actuatorId = 2;  // LED actuator

  // Join procedure
  bool isJoined;
  do {
    Serial.println("Joining...");
    isJoined = lora.join();
    
    //wait for 3s to try again
    delay(3000);
  }while(!isJoined);
  Serial.println("Joined to network");
}

void loop() {
  cdsVal1 = analogRead(cdsPin1);
  cdsVal2 = analogRead(cdsPin2);

  pirVal = digitalRead(pirPin);

  if (pirVal == HIGH) {
    if (pirState == LOW){
           Serial.println("Welcome!");    // 시리얼 모니터 출력
           pirCnt=pirCnt+1.0;
           pirState = HIGH;
    }
  } 
  else {           
    if (pirState == HIGH){
          Serial.println("Good Bye~");   // 시리얼 모니터 출력
          pirState = LOW;
    }
  }
  t.pub("5,6,9\0", 3, cdsVal1, pirCnt, cdsVal2); 
  t.rcv();

  t.set_target_actuator(&a2);
  t.actuator_LED(&a2, ledPin);

  wdt_reset();
}