#include <Arduino.h>
#include <AsyncUDP.h>
#include <stdlib.h>
#include <vector>
#include <WiFi.h>

#define FRONT_RIGHT_MOTOR 0
#define FRONT_LEFT_MOTOR 1
#define BACK_RIGHT_MOTOR 2
#define BACK_LEFT_MOTOR 3

#define FORWARD 1
#define BACKWARD -1
#define STOP 0

struct MOTOR_PINS {
    int pinIN1;
    int pinIN2;    
};
std::vector<MOTOR_PINS> motorPins = {
    {26, 25}, // Front Right Motor direction pin 1, 2 to Model-Y M_B IN1, IN2
    {17, 16}, // Front Left Motor direction pin 1, 2 to Model-Y M_B IN3, IN4
    {27, 14}, // Rear Right Motor direction pin 1, 2 to Model-Y M_A IN1, IN2
    {12, 13}, // Rear Left Motor direction pin 1, 2 to Model-Y M_A IN3, IN4 
};
// use a ring buffer to increase speed and reduce memory allocation
char buf[1];
char ssid[] = "M5StickC-Plus-Controller";
char pass[] = "controller";
AsyncUDP udp;
unsigned int port = 8888;  // local port to listen on


void rotateMotor(int motorNumber, int motorDirection) {
    if (motorDirection == FORWARD) {
        digitalWrite(motorPins[motorNumber].pinIN1, HIGH);
        digitalWrite(motorPins[motorNumber].pinIN2, LOW);    
    } else if (motorDirection == BACKWARD) {
        digitalWrite(motorPins[motorNumber].pinIN1, LOW);
        digitalWrite(motorPins[motorNumber].pinIN2, HIGH);     
    } else {
        digitalWrite(motorPins[motorNumber].pinIN1, LOW);
        digitalWrite(motorPins[motorNumber].pinIN2, LOW);       
    }
}


void forward() {
    rotateMotor(FRONT_RIGHT_MOTOR, FORWARD);
    rotateMotor(BACK_RIGHT_MOTOR, FORWARD);
    rotateMotor(FRONT_LEFT_MOTOR, FORWARD);
    rotateMotor(BACK_LEFT_MOTOR, FORWARD);  
}


void back() {
    rotateMotor(FRONT_RIGHT_MOTOR, BACKWARD);
    rotateMotor(BACK_RIGHT_MOTOR, BACKWARD);
    rotateMotor(FRONT_LEFT_MOTOR, BACKWARD);
    rotateMotor(BACK_LEFT_MOTOR, BACKWARD);
}


void right() {
    rotateMotor(FRONT_RIGHT_MOTOR, BACKWARD);
    rotateMotor(BACK_RIGHT_MOTOR, FORWARD);
    rotateMotor(FRONT_LEFT_MOTOR, FORWARD);
    rotateMotor(BACK_LEFT_MOTOR, BACKWARD);
}


void left() {
    rotateMotor(FRONT_RIGHT_MOTOR, FORWARD);
    rotateMotor(BACK_RIGHT_MOTOR, BACKWARD);
    rotateMotor(FRONT_LEFT_MOTOR, BACKWARD);
    rotateMotor(BACK_LEFT_MOTOR, FORWARD);
}


void right_forward() {
    rotateMotor(FRONT_RIGHT_MOTOR, STOP);
    rotateMotor(BACK_RIGHT_MOTOR, FORWARD);
    rotateMotor(FRONT_LEFT_MOTOR, FORWARD);
    rotateMotor(BACK_LEFT_MOTOR, STOP);
}


void left_forward() {
    rotateMotor(FRONT_RIGHT_MOTOR, FORWARD);
    rotateMotor(BACK_RIGHT_MOTOR, STOP);
    rotateMotor(FRONT_LEFT_MOTOR, STOP);
    rotateMotor(BACK_LEFT_MOTOR, FORWARD);
}


void right_back() {
    rotateMotor(FRONT_RIGHT_MOTOR, BACKWARD);
    rotateMotor(BACK_RIGHT_MOTOR, STOP);
    rotateMotor(FRONT_LEFT_MOTOR, STOP);
    rotateMotor(BACK_LEFT_MOTOR, BACKWARD);
}


void left_back() {
    rotateMotor(FRONT_RIGHT_MOTOR, STOP);
    rotateMotor(BACK_RIGHT_MOTOR, BACKWARD);
    rotateMotor(FRONT_LEFT_MOTOR, BACKWARD);
    rotateMotor(BACK_LEFT_MOTOR, STOP);
}


void right_turn() {
    rotateMotor(FRONT_RIGHT_MOTOR, BACKWARD);
    rotateMotor(BACK_RIGHT_MOTOR, BACKWARD);
    rotateMotor(FRONT_LEFT_MOTOR, FORWARD);
    rotateMotor(BACK_LEFT_MOTOR, FORWARD);  
}


void left_turn() {
    rotateMotor(FRONT_RIGHT_MOTOR, FORWARD);
    rotateMotor(BACK_RIGHT_MOTOR, FORWARD);
    rotateMotor(FRONT_LEFT_MOTOR, BACKWARD);
    rotateMotor(BACK_LEFT_MOTOR, BACKWARD);
}


void stop() {
    rotateMotor(FRONT_RIGHT_MOTOR, STOP);
    rotateMotor(BACK_RIGHT_MOTOR, STOP);
    rotateMotor(FRONT_LEFT_MOTOR, STOP);
    rotateMotor(BACK_LEFT_MOTOR, STOP);
}

// pins initialization
void initGPIO() {
    for (int i = 0; i < 4; i++) {
        pinMode(motorPins[i].pinIN1, OUTPUT);
        pinMode(motorPins[i].pinIN2, OUTPUT);
    }

    stop();
}


void setup() {
    initGPIO();
    Serial.begin(9600);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    delay(100);
    
    Serial.print("Connecting...");
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("");

    if (udp.listen(port)) {
        udp.onPacket([](AsyncUDPPacket packet) {
            buf[0]= (char)*(packet.data());
            Serial.println(buf[0]);
        });
    }
}


void loop() {
    switch (buf[0]) {
        case 'a':
            forward();
            break;
        case 'A':
            forward();
            break;
        case 'b':
            back();
            break;
        case 'B':
            back();
            break;
        case 'c':
            left_forward();
            break;
        case 'C':
            left_forward();
            break;
        case 'd':
            right_forward();
            break;
        case 'D':
            right_forward();
            break;
        case 'e':
            left_back();
            break;
        case 'E':
            left_back();
            break;
        case 'f':
            right_back();
            break;
        case 'F':
            right_back();
            break;
        case 'g':
            left();
            break;
        case 'G':
            left();
            break;
        case 'h':
            right();
            break;
        case 'H':
            right();
            break;
        case 'I':
            right_turn();
            break;
        case 'J':
            left_turn();
            break;
        case 'K':
            stop();
            break;
        default:
            stop();
            break;
    }
}
