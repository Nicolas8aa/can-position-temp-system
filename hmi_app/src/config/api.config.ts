/**
 * API Configuration
 * Change ESP32_IP to match your ESP32's IP address
 */
export const API_CONFIG = {
  ESP32_IP: '172.20.10.2',
  get TEMPERATURE_ENDPOINT() {
    return `http://${this.ESP32_IP}/api/temperature`;
  }
} as const;
