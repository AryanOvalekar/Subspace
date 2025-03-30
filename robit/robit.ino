#include <RH_ASK.h>
#include <SPI.h>

// these are non-pwm pins
#define LMOTOR_FORWARD 8
#define LMOTOR_BACKWARD 12
#define RMOTOR_FORWARD 4
#define RMOTOR_BACKWARD 7

RH_ASK driver;

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

enum Routine {
  RT_WAITING = 0,
  RT_DEPOSIT,
  RT_WITHDRAW,
};

int currentRoutine = 0;
uint8_t targetX = 0, targetY = 0;
uint8_t recvBuf[4];

// this routine gets called when we're waiting for a message from the terminal
void rt_waiting() {
  memset(waitingBuf, 0, 4)
  uint8_t buflen = 4;
  while (!driver.recv(waitingBuf, &buflen)) {
    delay(100);
  }

  // Message with a good checksum received, dump it.
  Serial.print("Message: ");
  Serial.println((char*)buf);

  char* type = recvBuf[0];
  targetX = (uint8_t)recvBuf[1];
  targetY = (uint8_t)recvBuf[2];
  if(type == 'd') {
    // deposit
    currentRoutine = RT_DEPOSIT;
  } else if (type == 'w') {
    // withdraw
    currentRoutine = RT_WITHDRAW;
  }
}

void rt_deposit() {

}

void rt_withdraw() {

}

void (*routines[3])() = {
    rt_waiting,
    rt_deposit,
    rt_withdraw
};


void loop() {
    routines[currentRoutine]();
}
