import { useTemperature } from './hooks/useTemperature';
import { TemperatureDisplay } from './components/TemperatureDisplay';
import { TemperatureChart } from './components/TemperatureChart';
import { API_CONFIG } from './config/api.config';
import './App.css';

/**
 * Main Application Component
 * Orchestrates the temperature monitoring display
 */
function App() {
  const { data, currentTemp, isLoading, error } = useTemperature({
    refreshInterval: 5000, // Update every 5 seconds
    maxDataPoints: 20,     // Keep last 20 data points
  });

  return (
    <div className="app">
      <header className="app-header">
        <h1>Temperature Monitoring System</h1>
        <p className="app-subtitle">ESP32 Direct Connection · {API_CONFIG.ESP32_IP}</p>
      </header>

      <main className="app-main">
        {error && (
          <div className="error-message">
            <strong>Connection Error:</strong> Cannot reach ESP32 at {API_CONFIG.ESP32_IP}. 
            Check that ESP32 is powered on, connected to WiFi, and the IP is correct in api.config.ts.
          </div>
        )}

        <div className="dashboard">
          <TemperatureDisplay temperature={currentTemp} isLoading={isLoading} />
          
          {data.length > 0 && (
            <div className="chart-container">
              <h2>Temperature History</h2>
              <TemperatureChart data={data} />
            </div>
          )}

          {!isLoading && data.length === 0 && !error && (
            <div className="no-data-message">
              Waiting for temperature data...
            </div>
          )}
        </div>
      </main>

      <footer className="app-footer">
        <p>Industrial Electronics Project - Temperature Monitoring HMI</p>
      </footer>
    </div>
  );
}

export default App;

