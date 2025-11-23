import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, ResponsiveContainer, Legend } from 'recharts';
import type { TemperatureData } from '../types/Temperature';

interface TemperatureChartProps {
  data: TemperatureData[];
}

/**
 * Component for displaying temperature data as a line chart
 * Single Responsibility: Visualize temperature data
 */
export const TemperatureChart = ({ data }: TemperatureChartProps) => {
  // Format timestamp for display
  const formatTime = (timestamp: number) => {
    const date = new Date(timestamp);
    return date.toLocaleTimeString('en-US', { 
      hour: '2-digit', 
      minute: '2-digit',
      second: '2-digit' 
    });
  };

  // Transform data for Recharts
  const chartData = data.map(item => ({
    time: formatTime(item.timestamp),
    temperature: item.temperature,
  }));

  return (
    <div className="temperature-chart">
      <ResponsiveContainer width="100%" height={400}>
        <LineChart
          data={chartData}
          margin={{ top: 20, right: 30, left: 20, bottom: 20 }}
        >
          <CartesianGrid strokeDasharray="3 3" stroke="#e0e0e0" />
          <XAxis 
            dataKey="time" 
            stroke="#666"
            tick={{ fill: '#666' }}
            label={{ value: 'Time', position: 'insideBottom', offset: -10, fill: '#666' }}
          />
          <YAxis 
            stroke="#666"
            tick={{ fill: '#666' }}
            label={{ value: 'Temperature (°C)', angle: -90, position: 'insideLeft', fill: '#666' }}
            domain={['dataMin - 5', 'dataMax + 5']}
          />
          <Tooltip 
            contentStyle={{ 
              backgroundColor: '#fff', 
              border: '1px solid #ccc',
              borderRadius: '4px'
            }}
            formatter={(value: number) => [`${value.toFixed(2)}°C`, 'Temperature']}
          />
          <Legend />
          <Line 
            type="monotone" 
            dataKey="temperature" 
            stroke="#ff6b6b" 
            strokeWidth={2}
            dot={{ fill: '#ff6b6b', r: 4 }}
            activeDot={{ r: 6 }}
            name="Temperature"
          />
        </LineChart>
      </ResponsiveContainer>
    </div>
  );
};
