#include <Arduino.h>
#include <AsyncUDP.h>
#include <stdlib.h>
#include <WiFi.h>

#define SPEED 150
#define TURN_SPEED 100

#define speedPinR 9   //  Front Wheel PWM pin connect Model-Y M_B ENA 
#define RightMotorDirPin1  22    //Front Right Motor direction pin 1 to Model-Y M_B IN1  (K1)
#define RightMotorDirPin2  24   //Front Right Motor direction pin 2 to Model-Y M_B IN2   (K1)                                 
#define LeftMotorDirPin1  26    //Front Left Motor direction pin 1 to Model-Y M_B IN3 (K3)
#define LeftMotorDirPin2  28   //Front Left Motor direction pin 2 to Model-Y M_B IN4 (K3)
#define speedPinL 10   //  Front Wheel PWM pin connect Model-Y M_B ENB

#define speedPinRB 11   //  Rear Wheel PWM pin connect Left Model-Y M_A ENA 
#define RightMotorDirPin1B  5    //Rear Right Motor direction pin 1 to Model-Y M_A IN1 ( K1)
#define RightMotorDirPin2B 6    //Rear Right Motor direction pin 2 to Model-Y M_A IN2 ( K1) 
#define LeftMotorDirPin1B 7    //Rear Left Motor direction pin 1 to Model-Y M_A IN3  (K3)
#define LeftMotorDirPin2B 8  //Rear Left Motor direction pin 2 to Model-Y M_A IN4 (K3)
#define speedPinLB 12    //  Rear Wheel PWM pin connect Model-Y M_A ENB

char ssid[] = "M5StickC-Plus-Controller";
char pass[] = "controller"; 
// use a ring buffer to increase speed and reduce memory allocation
char buf[1]; 
AsyncUDP udp;
unsigned int port = 8888;  // local port to listen on

// front-right wheel forward turn
void FR_bck(int speed) {
    digitalWrite(RightMotorDirPin1, LOW);
    digitalWrite(RightMotorDirPin2,HIGH); 
    analogWrite(speedPinR,speed);
}

// front-right wheel backward turn
void FR_fwd(int speed) {
    digitalWrite(RightMotorDirPin1,HIGH);
    digitalWrite(RightMotorDirPin2,LOW); 
    analogWrite(speedPinR,speed);
}

// front-left wheel forward turn
void FL_bck(int speed) {
    digitalWrite(LeftMotorDirPin1,LOW);
    digitalWrite(LeftMotorDirPin2,HIGH);
    analogWrite(speedPinL,speed);
}

// front-left wheel backward turn
void FL_fwd(int speed) {
    digitalWrite(LeftMotorDirPin1,HIGH);
    digitalWrite(LeftMotorDirPin2,LOW);
    analogWrite(speedPinL,speed);
}

// rear-right wheel forward turn
void RR_bck(int speed) {
    digitalWrite(RightMotorDirPin1B, LOW);
    digitalWrite(RightMotorDirPin2B,HIGH); 
    analogWrite(speedPinRB,speed);
}

// rear-right wheel backward turn
void RR_fwd(int speed) {
    digitalWrite(RightMotorDirPin1B, HIGH);
    digitalWrite(RightMotorDirPin2B,LOW); 
    analogWrite(speedPinRB,speed);
}

// rear-left wheel forward turn
void RL_bck(int speed) {
    digitalWrite(LeftMotorDirPin1B,LOW);
    digitalWrite(LeftMotorDirPin2B,HIGH);
    analogWrite(speedPinLB,speed);
}

// rear-left wheel backward turn
void RL_fwd(int speed) {
    digitalWrite(LeftMotorDirPin1B,HIGH);
    digitalWrite(LeftMotorDirPin2B,LOW);
    analogWrite(speedPinLB,speed);
}
 
// Stop
void stop() {
    analogWrite(speedPinLB,0);
    analogWrite(speedPinRB,0);
    analogWrite(speedPinL,0);
    analogWrite(speedPinR,0);
}

/* motor control */
void right_shift(
    int speed_fl_fwd,
    int speed_rl_bck,
    int speed_rr_fwd,
    int speed_fr_bck
) {
    FL_fwd(speed_fl_fwd); 
    RL_bck(speed_rl_bck); 
    FR_bck(speed_fr_bck);
    RR_fwd(speed_rr_fwd);
}


void left_shift(
    int speed_fl_bck,
    int speed_rl_fwd,
    int speed_rr_bck,
    int speed_fr_fwd
) {
    FL_bck(speed_fl_bck);
    RL_fwd(speed_rl_fwd);
    FR_fwd(speed_fr_fwd);
    RR_bck(speed_rr_bck); 
}


void forward(int speed) {
    RL_fwd(speed);
    RR_fwd(speed);
    FR_fwd(speed);
    FL_fwd(speed); 
}


void back(int speed) {
    RL_bck(speed);
    RR_bck(speed);
    FR_bck(speed);
    FL_bck(speed); 
}


void left_turn(int speed) {
    RL_bck(speed);
    RR_fwd(speed);
    FR_fwd(speed);
    FL_bck(speed); 
}


void right_turn(int speed) {
    RL_fwd(speed);
    RR_bck(speed);
    FR_bck(speed);
    FL_fwd(speed); 
}


void left_back(int speed) {
    RL_fwd(0);
    RR_bck(speed);
    FR_bck(speed);
    FL_fwd(0); 
}


void right_back(int speed) {
    RL_bck(speed);
    RR_fwd(0);
    FR_fwd(0);
    FL_bck(speed); 
}


void clockwise(int speed) {
    RL_fwd(speed);
    RR_bck(speed);
    FR_bck(speed);
    FL_fwd(speed); 
}


void countclockwise(int speed) {
    RL_bck(speed);
    RR_fwd(speed);
    FR_fwd(speed);
    FL_bck(speed); 
}

// pins initialization
void initGPIO() {
    pinMode(RightMotorDirPin1, OUTPUT); 
    pinMode(RightMotorDirPin2, OUTPUT); 
    pinMode(speedPinL, OUTPUT);  
    
    pinMode(LeftMotorDirPin1, OUTPUT);
    pinMode(LeftMotorDirPin2, OUTPUT); 
    pinMode(speedPinR, OUTPUT);
    pinMode(RightMotorDirPin1B, OUTPUT); 
    pinMode(RightMotorDirPin2B, OUTPUT); 
    pinMode(speedPinLB, OUTPUT);  
    
    pinMode(LeftMotorDirPin1B, OUTPUT);
    pinMode(LeftMotorDirPin2B, OUTPUT); 
    pinMode(speedPinRB, OUTPUT);
    
    stop();
}


void setup() {
    initGPIO();
    Serial.begin(9600);
    /*
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
    */
}


void loop() {
    Serial.println("Unko!!!!!!"); delay(2000);
    /*int packetSize = Udp.parsePacket();
    
    switch (buf[0]) {
        case 'a':
            forward(SPEED);
            break;
        case 'A':
            forward(2*SPEED);
            break;
        case 'b':
            back(speed);
            break
        case 'B':
            back(2*SPEED);
            break;
        case 'c':
            left_shift(0, 200, 0, 200);
            break;
        case 'C':
            left_shift(0, 400, 0, 400);
            break;
        case 'd':
            right_shift(200, 0, 200, 0);
            break;
        case 'D':
            right_shift(400, 0, 400, 0);
            break;
        case 'e':
            left_shift(200, 0, 200, 0);
            break;
        case 'E':
            left_shift(400, 0, 400, 0);
            break;
        case 'f':
            right_shift(0, 200, 0, 200);
            break;
        case 'F':
            right_shift(0, 400, 0, 400);
            break;
        case 'g':
            left_shift(200, 200, 200, 200);
            break;
        case 'G':
            left_shift(400, 400, 400, 400);
            break;
        case 'h':
            right_shift(200, 200, 200, 200);
            break;
        case 'H':
            right_shift(400, 400, 400, 400);
            break;
        case 'I':
            right_turn(TURN_SPEED);
            break;
        case 'J':
            left_turn(TURN_SPEED);
            break;
        case 'K':
            stop();
            break;
        default:
            break;
    }
    */
}
