#include <SPI.h>
#include <mcp2515.h>

struct can_frame canMsg;
struct MCP2515 mcp2515(5); // CS pin is GPIO 5

#define MAX_RETRIES 3
#define CAN_ACK_ID 0x037       // CAN ID for acknowledgment
#define POTENTIOMETER_PIN 13   // GPIO pin for potentiometer
#define ADC_RESOLUTION 4095    // 12-bit ADC (0-4095)
#define VOLTAGE_REFERENCE 3.3  // ESP32 reference voltage
#define CHANGE_THRESHOLD 10    // Minimum ADC change to trigger transmission

// Global variables
int lastAdcValue = -1; // Initialize to invalid value to ensure first reading is sent

void setup() {
  Serial.begin(115200);
  
  // Configure ADC pin
  pinMode(POTENTIOMETER_PIN, INPUT);
  analogReadResolution(12); // Set to 12-bit resolution
  
  // Initialize CAN bus
  SPI.begin();
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();
  
  Serial.println("Transmitter initialized");
}

void loop() {
  // Read ADC value from potentiometer
  int currentAdcValue = analogRead(POTENTIOMETER_PIN);
  
  // Check if value has changed significantly
  if (abs(currentAdcValue - lastAdcValue) >= CHANGE_THRESHOLD) {
    // Calculate voltage in millivolts for transmission
    int voltageMillivolts = (currentAdcValue * VOLTAGE_REFERENCE * 1000) / ADC_RESOLUTION;
    
    // Send CAN message with retry mechanism
    if (sendCanMessage(voltageMillivolts)) {
      lastAdcValue = currentAdcValue;
      
      // Display voltage information
      float voltage = voltageMillivolts / 1000.0;
      Serial.print("Voltage sent: ");
      Serial.print(voltage, 3);
      Serial.print(" V (ADC: ");
      Serial.print(currentAdcValue);
      Serial.println(")");
    } else {
      Serial.println("Failed to send message after retries");
    }
  }
  
  delay(10); // Small delay for stability
}

/**
 * Send CAN message with acknowledgment and retry logic
 * @param value The voltage value in millivolts to send
 * @return true if message was sent and acknowledged, false otherwise
 */
bool sendCanMessage(int value) {
  // Prepare CAN message
  canMsg.can_id  = 0x036;  // CAN ID
  canMsg.can_dlc = 2;      // Data length code (2 bytes)
  canMsg.data[0] = (value >> 8) & 0xFF; // MSB
  canMsg.data[1] = value & 0xFF;        // LSB
  
  // Retry mechanism
  for (int retries = 0; retries < MAX_RETRIES; retries++) {
    if (mcp2515.sendMessage(&canMsg) != MCP2515::ERROR_OK) {
      Serial.println("Error sending message, retrying...");
      continue;
    }
    
    // Wait for acknowledgment
    if (waitForAcknowledgment()) {
      Serial.println("ACK received");
      return true;
    }
    
    Serial.println("ACK not received, retrying...");
  }
  
  return false;
}

/**
 * Wait for CAN acknowledgment message
 * @return true if ACK received within timeout, false otherwise
 */
bool waitForAcknowledgment() {
  unsigned long startTime = millis();
  const unsigned long ACK_TIMEOUT = 500; // 500ms timeout
  
  while (millis() - startTime < ACK_TIMEOUT) {
    if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
      if (canMsg.can_id == CAN_ACK_ID) {
        return true;
      }
    }
  }
  
  return false;
}