#include <SPI.h>
#include <mcp2515.h>
#include <max6675.h>

struct can_frame canMsg;
struct MCP2515 mcp2515(5); // CS pin is GPIO 5

// MAX6675 Thermocouple (separate SPI pins)
#define MAX6675_SCK 14
#define MAX6675_CS 15
#define MAX6675_MISO 12
MAX6675 thermocouple(MAX6675_SCK, MAX6675_CS, MAX6675_MISO);

#define CAN_ACK_ID 0x037       // CAN ID for acknowledgment
#define CAN_DATA_ID 0x036      // CAN ID for incoming data
#define CAN_TEMP_ID 0x038      // CAN ID for temperature transmission
#define DAC_PIN 25             // GPIO25 for DAC output
#define DAC_RESOLUTION 255     // 8-bit DAC (0-255)
#define DAC_MAX_VOLTAGE 3.3    // ESP32 DAC max voltage
#define TEMP_READ_INTERVAL 1000 // Read temperature every 1 second
#define MAX_RETRIES 3

// Global variables
unsigned long lastTempRead = 0;
float lastTemperature = -999.0; // Invalid value to ensure first reading is sent

void setup()
{
  Serial.begin(115200);
  
  // Configure DAC pin
  pinMode(DAC_PIN, OUTPUT);
  
  // Initialize CAN bus (default SPI pins for MCP2515)
  SPI.begin();
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();
  
  // MAX6675 uses separate SPI pins, initialized in constructor
  // Give it time to stabilize
  delay(500);
  
  Serial.println("Transceiver (Slave) initialized");
  Serial.println("- Receiving voltage via CAN -> DAC output");
  Serial.println("- Reading temperature via MAX6675 -> CAN transmission");
}

void loop()
{
  // RECEIVER: Poll for CAN messages (voltage from master)
  receiveVoltageData();
  
  // TRANSMITTER: Read and send temperature data periodically
  readAndSendTemperature();
}

/**
 * Receive voltage data via CAN and output to DAC
 */
void receiveVoltageData() {
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK)
  {
    if (canMsg.can_id == CAN_DATA_ID)
    {
      // Extract voltage value in millivolts
      int voltageMillivolts = (canMsg.data[0] << 8) | canMsg.data[1];
      
      // Convert millivolts to DAC value (0-255)
      int dacValue = convertMillivoltsToDac(voltageMillivolts);
      
      // Output to DAC
      dacWrite(DAC_PIN, dacValue);
      
      // Display received data
      float voltage = voltageMillivolts / 1000.0;
      Serial.print("Voltage RX: ");
      Serial.print(voltage, 3);
      Serial.print(" V -> DAC: ");
      Serial.println(dacValue);
      
      // Send acknowledgment
      sendAcknowledgment();
    }
  }
}

/**
 * Read temperature from MAX6675 and send via CAN
 */
void readAndSendTemperature() {
  unsigned long currentMillis = millis();
  
  // Check if it's time to read temperature
  if (currentMillis - lastTempRead >= TEMP_READ_INTERVAL) {
    lastTempRead = currentMillis;
    
    // Read temperature from MAX6675
    float temperatureC = thermocouple.readCelsius();
    
    // Check for valid reading
    if (isnan(temperatureC)) {
      Serial.println("Error reading temperature from MAX6675");
      return;
    }
    
    // Check if temperature has changed significantly (0.5°C threshold)
    if (abs(temperatureC - lastTemperature) >= 0.5) {
      // Convert to integer (multiply by 100 for 2 decimal precision)
      int tempInt = (int)(temperatureC * 100);
      
      // Send temperature via CAN
      if (sendTemperatureMessage(tempInt)) {
        lastTemperature = temperatureC;
        
        Serial.print("Temperature TX: ");
        Serial.print(temperatureC, 2);
        Serial.println(" °C");
      } else {
        Serial.println("Failed to send temperature after retries");
      }
    }
  }
}

/**
 * Convert millivolts to DAC value (0-255)
 * @param millivolts Input voltage in millivolts
 * @return DAC value constrained to 0-255
 */
int convertMillivoltsToDac(int millivolts) {
  // Convert mV to DAC value: DAC = (mV / 3300) * 255
  int dacValue = (millivolts * DAC_RESOLUTION) / (DAC_MAX_VOLTAGE * 1000);
  
  // Constrain to valid DAC range
  return constrain(dacValue, 0, DAC_RESOLUTION);
}

/**
 * Send CAN acknowledgment message
 */
void sendAcknowledgment() {
  canMsg.can_id = CAN_ACK_ID;
  canMsg.can_dlc = 0; // No data needed for ACK
  
  if (mcp2515.sendMessage(&canMsg) == MCP2515::ERROR_OK) {
    Serial.println("ACK sent");
  }
}

/**
 * Send temperature data via CAN with retry mechanism
 * @param tempValue Temperature in hundredths of degrees (e.g., 2550 = 25.50°C)
 * @return true if message sent successfully, false otherwise
 */
bool sendTemperatureMessage(int tempValue) {
  // Prepare CAN message
  canMsg.can_id = CAN_TEMP_ID;
  canMsg.can_dlc = 2;
  canMsg.data[0] = (tempValue >> 8) & 0xFF; // MSB
  canMsg.data[1] = tempValue & 0xFF;        // LSB
  
  // Retry mechanism
  for (int retries = 0; retries < MAX_RETRIES; retries++) {
    if (mcp2515.sendMessage(&canMsg) == MCP2515::ERROR_OK) {
      return true;
    }
    Serial.println("Error sending temperature, retrying...");
    delay(10);
  }
  
  return false;
}
