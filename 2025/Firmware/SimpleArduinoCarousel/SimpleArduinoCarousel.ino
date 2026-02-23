#include <Servo.h>

Servo myServo;

const int SERVO_PIN = 9;

// Calibrate these for your specific 270-degree servo
const int SERVO_US_MIN = 500;
const int SERVO_US_MAX = 2500;

const int NUM_POSITIONS = 8;
const int INDEX_SHIFT = 4;  // Your 3-index offset

// Speed limit settings (45 deg/sec roughly)
const float MAX_US_PER_SEC = 250.0; 
const unsigned long UPDATE_INTERVAL = 20; // ms
const float MAX_US_PER_UPDATE = MAX_US_PER_SEC * (UPDATE_INTERVAL / 1000.0);

// Logic: To get 8 points, we divide the range into 7 equal gaps
const float US_STEP = (SERVO_US_MAX - SERVO_US_MIN) / (float)(NUM_POSITIONS - 1 + 1);

const int OFFSET_US = 200; // Your mechanical calibration offset

float currentPulse = 1500;
int targetPulse = 1500;

unsigned long lastUpdate = 0;

void setup() {
  Serial.begin(9600);
  myServo.attach(SERVO_PIN);

  // Initialize at the center or a safe start
  currentPulse = 1500;
  targetPulse = 1500;
  myServo.writeMicroseconds((int)currentPulse);

  Serial.println("--- 270 Degree Servo Controller ---");
  Serial.println("Input 0-7. Result will shift by 3 and wrap around.");
}

void loop() {

  // ----- SERIAL INPUT -----
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.length() > 0) {
      int rawIndex = input.toInt();

      if (rawIndex >= 0 && rawIndex < NUM_POSITIONS) {
        
        // Apply the 3-index shift with Wrap Around (Modulo)
        int shiftedIndex = (rawIndex + INDEX_SHIFT) % NUM_POSITIONS;

        // Calculate final pulse
        targetPulse = (SERVO_US_MIN + OFFSET_US) + (shiftedIndex * US_STEP);

        // Safety check to ensure we don't overdrive the motor hardware
        targetPulse = constrain(targetPulse, 500, 2500);

        Serial.print("Input: "); Serial.print(rawIndex);
        Serial.print(" -> Shifted: "); Serial.print(shiftedIndex);
        Serial.print(" | Target Pulse: "); Serial.println(targetPulse);
      } else {
        Serial.println("Invalid input! Use 0-7.");
      }
    }
  }

  // ----- SPEED-LIMITED MOTION (Non-Blocking) -----
  unsigned long now = millis();
  if (now - lastUpdate >= UPDATE_INTERVAL) {
    lastUpdate = now;

    float error = targetPulse - currentPulse;

    if (abs(error) > MAX_US_PER_UPDATE) {
      if (error > 0)
        currentPulse += MAX_US_PER_UPDATE;
      else
        currentPulse -= MAX_US_PER_UPDATE;
    } else {
      currentPulse = targetPulse;
    }

    myServo.writeMicroseconds((int)currentPulse);
  }
}