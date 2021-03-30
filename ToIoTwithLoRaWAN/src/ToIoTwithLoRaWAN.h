#ifndef ToIoTwithLoRaWAN_H
#define ToIoTwithLoRaWAN_H

#include <lorawan.h>
#include "Servo.h"
#include "Actuator.h"


class ToIoTwithLoRaWAN
{
    public:
        ToIoTwithLoRaWAN();
        void setupToIoTwithLoRaWAN(char* nodeI, const unsigned long intertimer, unsigned int QOS);
        void pub(char* sensorId, int cnt, ...);
        void rcv();
        void actuator_servo(struct Actuator* actptr, Servo* servoptr, int pin);
        void actuator_L9110(struct Actuator* actptr, int pin1, int pin2);
        void actuator_LED(struct Actuator* actptr, int pin);
        void set_target_actuator(struct Actuator *actptr);

    private:
        char topic[26];
        char* nodeId;
        double arg=0;
        char msg[50];
        char last_msg[50];
        char outStr[50];
        byte recvStatus = 0;
        
        unsigned long interval = 10000;    // 10 s interval to send message
        unsigned long previousMillis = 0;  // will store last time message sent

        unsigned int uplink_counter = 0;     // message counter

        unsigned int QOS;
        bool sender_lock;

        struct Actuator* target_actuator;    // Maximum number of actuator is 5
};

#endif