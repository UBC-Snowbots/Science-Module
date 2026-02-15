#include <Servo.h>

Servo myServo;

const int SERVO_PIN = 9;

// Calibrate these for your servo
const int SERVO_US_MIN = 500;
const int SERVO_US_MAX = 2500;

const int NUM_POSITIONS = 8;

// Speed limit
const float MAX_US_PER_SEC = 250.0;   // derived above
const unsigned long UPDATE_INTERVAL = 20; // ms
const float MAX_US_PER_UPDATE = MAX_US_PER_SEC * (UPDATE_INTERVAL / 1000.0);

// Derived
const float US_STEP = (SERVO_US_MAX - SERVO_US_MIN) / (float)(NUM_POSITIONS - 1);

int currentPulse = 1500;
int targetPulse = 1500;

unsigned long lastUpdate = 0;

void setup() {
  Serial.begin(9600);
  myServo.attach(SERVO_PIN);

  currentPulse = 1500;
  targetPulse = 1500;
  myServo.writeMicroseconds(currentPulse);

  Serial.println("Send 0–7 to move (speed limited to 45 deg/sec output).");
}

void loop() {

  // ----- SERIAL INPUT -----
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.length() > 0) {
      int index = input.toInt();

      if (index >= 0 && index < NUM_POSITIONS) {
        targetPulse = SERVO_US_MIN + index * US_STEP;

        Serial.print("New target index ");
        Serial.print(index);
        Serial.print(" | Target pulse ");
        Serial.println(targetPulse);
      } else {
        Serial.println("Invalid input (use 0–7)");
      }
    }
  }

  // ----- SPEED-LIMITED MOTION -----
  unsigned long now = millis();
  if (now - lastUpdate >= UPDATE_INTERVAL) {
    lastUpdate = now;

    int error = targetPulse - currentPulse;

    if (abs(error) > MAX_US_PER_UPDATE) {
      if (error > 0)
        currentPulse += MAX_US_PER_UPDATE;
      else
        currentPulse -= MAX_US_PER_UPDATE;
    } else {
      currentPulse = targetPulse;
    }

    myServo.writeMicroseconds(currentPulse);
  }
}
