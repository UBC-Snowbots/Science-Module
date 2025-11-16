#include <ModbusMaster.h>
#include <SoftwareSerial.h>

// RS485 control pins
#define DE_PIN 4    // TX Enable
#define RE_PIN 5    // RX Enable

// SoftwareSerial for RS485 communication
#define RX_PIN 2
#define TX_PIN 3
SoftwareSerial rs485Serial(RX_PIN, TX_PIN);

// Instantiate the ModbusMaster object
ModbusMaster node;

// Function to enable transmit mode
void preTransmission() {
  digitalWrite(DE_PIN, HIGH);
  digitalWrite(RE_PIN, HIGH);
}

// Function to enable receive mode
void postTransmission() {
  digitalWrite(DE_PIN, LOW);
  digitalWrite(RE_PIN, LOW);
}

void setup() {
  Serial.begin(9600);         // Debug Serial Monitor
  rs485Serial.begin(9600);    // RS485 communication baud rate

  // Set RS485 direction control pins as outputs
  pinMode(DE_PIN, OUTPUT);
  pinMode(RE_PIN, OUTPUT);
  digitalWrite(DE_PIN, LOW);  // Start in receive mode
  digitalWrite(RE_PIN, LOW);

  // Initialize Modbus communication with sensor (device address 0x01)
  node.begin(0x01, rs485Serial);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  Serial.println("Extended Modbus RTU read example started");
}

void loop() {
  uint8_t result;
  uint16_t data;

  // Read and display pH Value (Register 0x0006)
  result = node.readHoldingRegisters(0x0006, 1);
  if (result == node.ku8MBSuccess) {
    data = node.getResponseBuffer(0);
    Serial.print("pH Value: ");
    Serial.println(data / 100.0, 1);  // Convert to pH units
  } else {
    Serial.println("Failed to read pH");
  }

  // Read and display Soil Moisture (Register 0x0012)
  result = node.readHoldingRegisters(0x0012, 1);
  if (result == node.ku8MBSuccess) {
    data = node.getResponseBuffer(0);
    Serial.print("Soil Moisture: ");
    Serial.print(data / 10.0, 1);  // Convert to 0.1% RH
    Serial.println(" %");
  } else {
    Serial.println("Failed to read Soil Moisture");
  }

  // Read and display Soil Temperature (Register 0x0013)
  result = node.readHoldingRegisters(0x0013, 1);
  if (result == node.ku8MBSuccess) {
    data = node.getResponseBuffer(0);
    Serial.print("Soil Temperature: ");
    Serial.print(data / 10.0, 1);  // Convert to 0.1°C
    Serial.println(" °C");
  } else {
    Serial.println("Failed to read Soil Temperature");
  }

  // Read and display Soil Conductivity (Register 0x0015)
  result = node.readHoldingRegisters(0x0015, 1);
  if (result == node.ku8MBSuccess) {
    data = node.getResponseBuffer(0);
    Serial.print("Soil Conductivity: ");
    Serial.print(data);  // in μS/cm
    Serial.println(" μS/cm");
  } else {
    Serial.println("Failed to read Soil Conductivity");
  }

  // Read NPK values (Registers 0x001E to 0x0020)
  result = node.readHoldingRegisters(0x001E, 3);
  if (result == node.ku8MBSuccess) {
    uint16_t nitrogen   = node.getResponseBuffer(0);
    uint16_t phosphorus = node.getResponseBuffer(1);
    uint16_t potassium  = node.getResponseBuffer(2);

    Serial.print("Nitrogen: "); Serial.print(nitrogen); Serial.println(" mg/kg");
    Serial.print("Phosphorus: "); Serial.print(phosphorus); Serial.println(" mg/kg");
    Serial.print("Potassium: "); Serial.print(potassium); Serial.println(" mg/kg");
  } else {
    Serial.println("Failed to read NPK values");
  }

  Serial.println("------------------------------------");
  delay(5000); // Wait 5 seconds between readings
}
