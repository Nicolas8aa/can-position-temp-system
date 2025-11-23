interface TemperatureDisplayProps {
  temperature: number | null;
  isLoading: boolean;
}

/**
 * Component for displaying current temperature value
 * Single Responsibility: Display current temperature
 */
export const TemperatureDisplay = ({ temperature, isLoading }: TemperatureDisplayProps) => {
  if (isLoading) {
    return (
      <div className="temperature-display loading">
        <div className="temp-label">Loading...</div>
      </div>
    );
  }

  if (temperature === null) {
    return (
      <div className="temperature-display error">
        <div className="temp-label">No Data</div>
      </div>
    );
  }

  return (
    <div className="temperature-display">
      <div className="temp-label">Current Temperature</div>
      <div className="temp-value">
        {temperature.toFixed(2)}
        <span className="temp-unit">°C</span>
      </div>
    </div>
  );
};
