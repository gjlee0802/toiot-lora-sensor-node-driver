#include "ToIoTwithLoRaWAN.h"
#include "config.h"

const sRFM_pins RFM_pins = {
  .CS = 15,
  .RST = 16,
  .DIO0 = 4,
  .DIO1 = 5,
  .DIO2 = -1,
  .DIO5 = -1,
};

ToIoTwithLoRaWAN t;
double value = 0.0; 

void setup() {
  t.setupToIoTwithLoRaWAN(nodeId, interval, 0);

  // Set LoRaWAN Class change CLASS_A or CLASS_C
  lora.setDeviceClass(CLASS_C);

  // Set Data Rate
  lora.setDataRate(SF8BW125);

  // set channel to random
  lora.setChannel(0);
  
  // Put ABP Key and DevAddress here
  lora.setNwkSKey(nwkSKey);
  lora.setAppSKey(appSKey);
  lora.setDevAddr(devAddr);
}

void loop() {
  t.pub("sensor-uuid-1", 1,value);
  t.rcv();
  wdt_reset();
}