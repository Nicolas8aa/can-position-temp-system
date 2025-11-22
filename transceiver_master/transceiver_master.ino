#include <SPI.h>
#include <mcp2515.h>
#include <WiFi.h>
#include <HTTPClient.h>

struct can_frame canMsg;
struct MCP2515 mcp2515(5); // CS pin is GPIO 5

// WiFi Configuration
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define WEB_SERVER_URL "http://192.168.1.100:3000/api/temperature" // Update with your server

// CAN IDs
#define MAX_RETRIES 3
#define CAN_ACK_ID 0x037       // CAN ID for acknowledgment
#define CAN_VOLTAGE_ID 0x036   // CAN ID for voltage data
#define CAN_TEMP_ID 0x038      // CAN ID for temperature data

// ADC Configuration
#define POTENTIOMETER_PIN 13   // GPIO pin for potentiometer
#define ADC_RESOLUTION 4095    // 12-bit ADC (0-4095)
#define VOLTAGE_REFERENCE 3.3  // ESP32 reference voltage
#define CHANGE_THRESHOLD 10    // Minimum ADC change to trigger transmission

// Global variables
int lastAdcValue = -1; // Initialize to invalid value to ensure first reading is sent
float currentTemperature = 0.0; // Current temperature from slave
unsigned long lastWebUpdate = 0;
#define WEB_UPDATE_INTERVAL 5000 // Send to web server every 5 seconds

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
  
  // Initialize WiFi
  initWiFi();
  
  Serial.println("Transceiver (Master) initialized");
  Serial.println("- Sending voltage via CAN");
  Serial.println("- Receiving temperature via CAN");
  Serial.println("- Forwarding temperature to web server");
}

void loop() {
  // TRANSMITTER: Read and send potentiometer voltage
  readAndSendVoltage();
  
  // RECEIVER: Poll for temperature data from slave
  receiveTemperatureData();
  
  // WEB SERVER: Send temperature data periodically
  sendTemperatureToWebServer();
  
  delay(10); // Small delay for stability
}

/**
 * Read potentiometer and send voltage via CAN
 */
void readAndSendVoltage() {
  int currentAdcValue = analogRead(POTENTIOMETER_PIN);
  
  // Check if value has changed significantly
  if (abs(currentAdcValue - lastAdcValue) >= CHANGE_THRESHOLD) {
    // Calculate voltage in millivolts for transmission
    int voltageMillivolts = (currentAdcValue * VOLTAGE_REFERENCE * 1000) / ADC_RESOLUTION;
    
    // Send CAN message with retry mechanism
    if (sendVoltageMessage(voltageMillivolts)) {
      lastAdcValue = currentAdcValue;
      
      // Display voltage information
      float voltage = voltageMillivolts / 1000.0;
      Serial.print("Voltage TX: ");
      Serial.print(voltage, 3);
      Serial.print(" V (ADC: ");
      Serial.print(currentAdcValue);
      Serial.println(")");
    } else {
      Serial.println("Failed to send voltage after retries");
    }
  }
}

/**
 * Receive temperature data from slave via CAN
 */
void receiveTemperatureData() {
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
    if (canMsg.can_id == CAN_TEMP_ID) {
      // Extract temperature value (in hundredths of degrees)
      int tempInt = (canMsg.data[0] << 8) | canMsg.data[1];
      currentTemperature = tempInt / 100.0;
      
      Serial.print("Temperature RX: ");
      Serial.print(currentTemperature, 2);
      Serial.println(" °C");
    }
  }
}

/**
 * Send temperature data to web server periodically
 */
void sendTemperatureToWebServer() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - lastWebUpdate >= WEB_UPDATE_INTERVAL) {
    lastWebUpdate = currentMillis;
    
    // Only send if WiFi is connected
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      
      http.begin(WEB_SERVER_URL);
      http.addHeader("Content-Type", "application/json");
      
      // Create JSON payload
      String jsonPayload = "{\"temperature\":";
      jsonPayload += String(currentTemperature, 2);
      jsonPayload += ",\"timestamp\":";
      jsonPayload += String(millis());
      jsonPayload += "}";
      
      int httpResponseCode = http.POST(jsonPayload);
      
      if (httpResponseCode > 0) {
        Serial.print("Web Server Response: ");
        Serial.println(httpResponseCode);
        String response = http.getString();
        Serial.println(response);
      } else {
        Serial.print("Error sending to web server: ");
        Serial.println(httpResponseCode);
      }
      
      http.end();
    } else {
      Serial.println("WiFi not connected");
      // Try to reconnect
      WiFi.reconnect();
    }
  }
}

/**
 * Initialize WiFi connection
 */
void initWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi connection failed - continuing without web server");
  }
}

/**
 * Send voltage message via CAN with acknowledgment and retry logic
 * @param value The voltage value in millivolts to send
 * @return true if message was sent and acknowledged, false otherwise
 */
bool sendVoltageMessage(int value) {
  // Prepare CAN message
  canMsg.can_id  = CAN_VOLTAGE_ID;
  canMsg.can_dlc = 2;      // Data length code (2 bytes)
  canMsg.data[0] = (value >> 8) & 0xFF; // MSB
  canMsg.data[1] = value & 0xFF;        // LSB
  
  // Retry mechanism
  for (int retries = 0; retries < MAX_RETRIES; retries++) {
    if (mcp2515.sendMessage(&canMsg) != MCP2515::ERROR_OK) {
      Serial.println("Error sending voltage, retrying...");
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