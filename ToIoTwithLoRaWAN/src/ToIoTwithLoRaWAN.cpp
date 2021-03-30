#include "ToIoTwithLoRaWAN.h"

#define BAUDRATE 115200

ToIoTwithLoRaWAN::ToIoTwithLoRaWAN()
{
}

void ToIoTwithLoRaWAN::setupToIoTwithLoRaWAN(char* nodeI, const unsigned long intertimer, unsigned int qos)
{
    //Serial.begin(9600);
    Serial.begin(BAUDRATE);
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
                Serial.println("MOTOR ON");
                digitalWrite(pin1, HIGH);
                digitalWrite(pin2, LOW);
            }
            else{
                Serial.println("MOTOR OFF");
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

void ToIoTwithLoRaWAN::actuator_LED(struct Actuator* actptr, int pin)
{
    if((actptr->run == true) && (actptr->running_index < actptr->values_len)) {
        if(millis() - actptr->previousMillis > actptr->interval[actptr->running_index]) {
            Serial.println("IF TEST");
            actptr->previousMillis = millis();
            analogWrite(pin , actptr->value[actptr->running_index]);
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

void ToIoTwithLoRaWAN::pub(char* sensorIdList, int cnt, ...)
{
    if(millis() - previousMillis > interval) {
        previousMillis = millis(); 
        int i=0;
        char *strBuff;
        char *sensorArr[50] = {NULL,};
        va_list ap;
        va_start(ap, cnt);
        memset(msg, 0, 50);
        
        strBuff = (char*)malloc(strlen(sensorIdList));
        strcpy(strBuff, sensorIdList);

        //sprintf(msg, "%s:%s,",topic,sensorId);
        sprintf(msg, "%s:", topic);

        char *ptr = strtok(strBuff, ",");
        i = 0;
        while (ptr != NULL)            // 자른 문자열이 나오지 않을 때까지 반복
        {
            sensorArr[i] = ptr;             // 문자열을 자른 뒤 메모리 주소를 문자열 포인터 배열에 저장
            i++;                       // 인덱스 증가

            ptr = strtok(NULL, ",");   // 다음 문자열을 잘라서 포인터를 반환
        }

        for(i=0; i<cnt; i++)
        {
            
            arg = va_arg(ap, double);
            if(i == cnt-1)
            {
                sprintf(msg, "%s%s,%lf", msg, sensorArr[i], arg);
            }
            else
            {
                sprintf(msg, "%s%s,%lf;", msg, sensorArr[i], arg);
            }
            //arg = va_arg(ap, double);
        }
        va_end(ap);
        
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
}

void ToIoTwithLoRaWAN::rcv()
{
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