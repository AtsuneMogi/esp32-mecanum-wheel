#include <Arduino.h>
#include <AsyncUDP.h>
#include <stdlib.h>
#include <vector>
#include <WiFi.h>

#define sensorFL 39
#define sensorFR 36
#define sensorL 35
#define sensorR 34
#define sensorBL 5
#define sensorBR 4

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
char ssid[] = "M5StickC-Controller";
char pass[] = "controller";
AsyncUDP udp;
unsigned int port = 8888; // local port to listen on


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
    pinMode(sensorFL, INPUT);
    pinMode(sensorFR, INPUT);
    pinMode(sensorL, INPUT);
    pinMode(sensorR, INPUT);
    pinMode(sensorBL, INPUT);
    pinMode(sensorBR, INPUT);

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
            if (!digitalRead(sensorFL) + !digitalRead(sensorFR)) {
                stop();
            } else {
                forward();
            }
            break;
        case 'A':
            if (!digitalRead(sensorFL) + !digitalRead(sensorFR)) {
                stop();
            } else {
                forward();
            }
            break;
        case 'b':
            if (!digitalRead(sensorBL) + !digitalRead(sensorBR)) {
                stop();
            } else {
                back();
            }
            break;
        case 'B':
            if (!digitalRead(sensorBL) + !digitalRead(sensorBR)) {
                stop();
            } else {
                back();
            }
            break;
        case 'c':
            if (!digitalRead(sensorFL) + !digitalRead(sensorL)) {
                stop();
            } else {
                left_forward();
            }
            break;
        case 'C':
            if (!digitalRead(sensorFL) + !digitalRead(sensorL)) {
                stop();
            } else {
                left_forward();
            }
            break;
        case 'd':
            if (!digitalRead(sensorR) + !digitalRead(sensorFR)) {
                stop();
            } else {
                right_forward();
            }
            break;
        case 'D':
            if (!digitalRead(sensorR) + !digitalRead(sensorFR)) {
                stop();
            } else {
                right_forward();
            }
            break;
        case 'e':
            if (!digitalRead(sensorL) + !digitalRead(sensorBL)) {
                stop();
            } else {
                left_back();
            }
            break;
        case 'E':
            if (!digitalRead(sensorL) + !digitalRead(sensorBL)) {
                stop();
            } else {
                left_back();
            }
            break;
        case 'f':
            if (!digitalRead(sensorR) + !digitalRead(sensorBR)) {
                stop();
            } else {
                right_back();
            }
            break;
        case 'F':
            if (!digitalRead(sensorR) + !digitalRead(sensorBR)) {
                stop();
            } else {
                right_back();
            }
            break;
        case 'g':
            if (!digitalRead(sensorFL) + !digitalRead(sensorL) + !digitalRead(sensorBL)) {
                stop();
            } else {
                left();
            }
            break;
        case 'G':
            if (!digitalRead(sensorFL) + !digitalRead(sensorL) + !digitalRead(sensorBL)) {
                stop();
            } else {
                left();
            }
            break;
        case 'h':
            if (!digitalRead(sensorFR) + !digitalRead(sensorR) + !digitalRead(sensorBR)) {
                stop();
            } else {
                right();
            }
            break;
        case 'H':
            if (!digitalRead(sensorFR) + !digitalRead(sensorR) + !digitalRead(sensorBR)) {
                stop();
            } else {
                right();
            }
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

