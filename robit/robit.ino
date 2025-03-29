#include <RH_ASK.h>
#include <SPI.h>

// these are non-pwm pins
#define LMOTOR_FORWARD 8
#define LMOTOR_BACKWARD 12
#define RMOTOR_FORWARD 4
#define RMOTOR_BACKWARD 7

RH_ASK driver(2000);

bool reverse = false;

void setup() {
    // pinMode(leftMotor, OUTPUT);
    // pinMode(leftMotorReverse, OUTPUT);
    Serial.begin(9600);	  // Debugging only
    if (!driver.init())
         Serial.println("init failed"); 

    pinMode(LMOTOR_FORWARD, OUTPUT);
    pinMode(LMOTOR_BACKWARD, OUTPUT);
}

void left_motor(int direction) {
  if(direction > 0) {
    digitalWrite(LMOTOR_FORWARD, HIGH);
    digitalWrite(LMOTOR_BACKWARD, LOW);
  } else if (direction < 0)  {
    digitalWrite(LMOTOR_FORWARD, LOW);
    digitalWrite(LMOTOR_BACKWARD, HIGH);
  } else {
    digitalWrite(LMOTOR_FORWARD, LOW);
    digitalWrite(LMOTOR_BACKWARD, LOW);    
  }
}

void right_motor(int direction) {
  if(direction > 0) {
    digitalWrite(RMOTOR_FORWARD, HIGH);
    digitalWrite(RMOTOR_BACKWARD, LOW);
  } else if (direction < 0)  {
    digitalWrite(RMOTOR_FORWARD, LOW);
    digitalWrite(RMOTOR_BACKWARD, HIGH);
  } else {
    digitalWrite(RMOTOR_FORWARD, LOW);
    digitalWrite(RMOTOR_BACKWARD, LOW);    
  }
}

void loop() {

    // uint8_t buf[4];
    // uint8_t buflen = sizeof(buf);
    // if (driver.recv(buf, &buflen)) // Non-blocking
    // {
    //   int i;
    //   // Message with a good checksum received, dump it.
    //   Serial.print("Message: ");
    //   Serial.println((char*)buf);         
    // }
}
