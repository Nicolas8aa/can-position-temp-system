import { useState, useEffect, useCallback } from 'react';
import type { TemperatureData } from '../types/Temperature';
import { temperatureService } from '../services/TemperatureService';

interface UseTemperatureOptions {
  refreshInterval?: number;
  maxDataPoints?: number;
}

/**
 * Custom hook for managing temperature data
 * Single Responsibility: Handle temperature data state and updates
 */
export const useTemperature = (options: UseTemperatureOptions = {}) => {
  const { refreshInterval = 5000, maxDataPoints = 20 } = options;
  
  const [data, setData] = useState<TemperatureData[]>([]);
  const [currentTemp, setCurrentTemp] = useState<number | null>(null);
  const [isLoading, setIsLoading] = useState<boolean>(true);
  const [error, setError] = useState<Error | null>(null);

  const fetchData = useCallback(async () => {
    try {
      setError(null);
      const newData = await temperatureService.fetchTemperature();
      
      setCurrentTemp(newData.temperature);
      setData(prevData => {
        const updatedData = [...prevData, newData];
        // Keep only the last maxDataPoints
        return updatedData.slice(-maxDataPoints);
      });
      
      setIsLoading(false);
    } catch (err) {
      setError(err as Error);
      setIsLoading(false);
    }
  }, [maxDataPoints]);

  useEffect(() => {
    // Initial fetch
    fetchData();

    // Set up interval for periodic updates
    const intervalId = setInterval(fetchData, refreshInterval);

    // Cleanup
    return () => clearInterval(intervalId);
  }, [fetchData, refreshInterval]);

  return {
    data,
    currentTemp,
    isLoading,
    error,
    refresh: fetchData,
  };
};
