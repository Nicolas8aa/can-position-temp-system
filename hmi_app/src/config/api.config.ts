/**
 * API Configuration
 * Change ESP32_IP to match your ESP32's IP address
 */
export const API_CONFIG = {
  ESP32_IP: '192.168.1.100',
  get TEMPERATURE_ENDPOINT() {
    return `http://${this.ESP32_IP}/api/temperature`;
  }
} as const;
