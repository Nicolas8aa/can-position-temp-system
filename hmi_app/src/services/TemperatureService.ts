import type { TemperatureData } from '../types/Temperature';
import { API_CONFIG } from '../config/api.config';

/**
 * Service for fetching temperature data from ESP32
 * Single Responsibility: Handle all ESP32 API communication
 */
export class TemperatureService {
  private readonly apiUrl: string;

  constructor(apiUrl: string = API_CONFIG.TEMPERATURE_ENDPOINT) {
    this.apiUrl = apiUrl;
  }

  /**
   * Fetch current temperature from ESP32
   * @returns Promise with temperature data
   */
  async fetchTemperature(): Promise<TemperatureData> {
    try {
      const response = await fetch(this.apiUrl);
      
      if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`);
      }
      
      const data = await response.json();
      return {
        temperature: data.temperature,
        timestamp: data.timestamp || Date.now(),
      };
    } catch (error) {
      console.error('Error fetching temperature from ESP32:', error);
      throw error;
    }
  }
}

// Export singleton instance
export const temperatureService = new TemperatureService();
