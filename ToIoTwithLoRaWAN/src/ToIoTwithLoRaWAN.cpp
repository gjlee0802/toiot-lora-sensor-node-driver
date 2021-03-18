#include "ToIoTwithLoRaWAN.h"

ToIoTwithLoRaWAN::ToIoTwithLoRaWAN()
{
}

void ToIoTwithLoRaWAN::setupToIoTwithLoRaWAN(char* nodeI, const unsigned long intertimer, unsigned int qos)
{
    //Serial.begin(9600);
    Serial.begin(115200);
    if(!lora.init()){
        Serial.println("RFM95 not detected");
        delay(5000);
        return;
    }
    nodeId = nodeI;
    interval = intertimer;
    QOS = qos;
    sender_lock = false;
    snprintf(topic, 26, "data/%s", nodeId);
}

void ToIoTwithLoRaWAN::actuator_servo(struct Actuator* actptr, Servo* servoptr, int pin)
{
    if((actptr->run == true) && (actptr->running_index < actptr->values_len)) {
        if(millis() - actptr->previousMillis > actptr->interval[actptr->running_index]) {
            Serial.println("IF TEST");
            actptr->previousMillis = millis();
            servoptr->attach(pin);
            servoptr->write(actptr->value[actptr->running_index]);
            
            actptr->running_index++;
            if (actptr->running_index >= actptr->values_len)
            {
                actptr->run = false;
            }
        }
    }
}

void ToIoTwithLoRaWAN::actuator_L9110(struct Actuator* actptr, int pin1, int pin2)
{
    if((actptr->run == true) && (actptr->running_index < actptr->values_len)) {
        if(millis() - actptr->previousMillis > actptr->interval[actptr->running_index]) {
            Serial.println("IF TEST");
            actptr->previousMillis = millis();
            if (actptr->value[actptr->running_index] == 1){
                digitalWrite(pin1, HIGH);
                digitalWrite(pin2, LOW);
            }
            else{
                digitalWrite(pin1, LOW);
                digitalWrite(pin2, LOW);
            }
            
            actptr->running_index++;
            if (actptr->running_index >= actptr->values_len)
            {
                actptr->run = false;
            }
        }
    }
}

void ToIoTwithLoRaWAN::set_target_actuator(struct Actuator *actptr)
{
    target_actuator = actptr;
}

void ToIoTwithLoRaWAN::pub(char* sensorId, int cnt, ...)
{

    if(millis() - previousMillis > interval) {
        va_list ap;
        va_start(ap, cnt);
        memset(msg, 0, 50);
        sprintf(msg, "%s:%s,",topic,sensorId);

        for(int i=0; i<cnt; i++)
        {
            arg = va_arg(ap, double);
            if(i == cnt-1)
            {
                sprintf(msg, "%s%lf",msg,arg);
            }
            else
            {
                sprintf(msg, "%s%lf,",msg,arg);
            }
        }
        va_end(ap);
        previousMillis = millis(); 
        if(QOS){
            if ((uplink_counter > 0) && !sender_lock){
                lora.sendUplink(msg, strlen(msg), 1, 1);
                Serial.print("[Pub] ");
                Serial.println(msg);
            }
            else if(uplink_counter == 0){
                lora.sendUplink(msg, strlen(msg), 1, 1);
                Serial.print("[Pub] ");
                Serial.println(msg);
            }
            else{
                lora.sendUplink(last_msg, strlen(last_msg), 1, 1);
            }
            strcpy(last_msg, msg);
            sender_lock = true;
        }
        else{
            lora.sendUplink(msg, strlen(msg), 0, 1);
            Serial.print("[Pub] ");
            Serial.println(msg);
            uplink_counter++;
        }
    }
    
    lora.update();
    recvStatus = lora.readData(outStr);
    if(recvStatus) {
        Serial.print("[Recv] ");
        Serial.println(outStr);
        Serial.print("SNR Value: ");
        Serial.println(lora.get_pkt_snr());
        if(QOS && lora.readAck()){
            Serial.println("ACK received!!");
            sender_lock = false;
            uplink_counter++;
        }
        if(lora.readMac()){
            Serial.println("MAC Command received!");
            lora.handle_mac_cmd_req(outStr,&uplink_counter, target_actuator);
        }
    }
    
}