# Temperature Monitoring HMI Application

A modern, responsive web application for real-time temperature monitoring from an ESP32 CAN bus system. Built with React, TypeScript, and Vite, following SOLID and KISS principles.

## 🎯 Features

- **Real-time Temperature Monitoring**: Displays current temperature from ESP32 via CAN bus
- **Interactive Charts**: Visual temperature history using Recharts
- **Responsive Design**: Works seamlessly on mobile, tablet, and desktop devices
- **Dark Mode Support**: Automatic dark mode based on system preferences
- **Clean Architecture**: Component-based design following SOLID principles
- **Type-Safe**: Full TypeScript implementation

## 🏗️ Architecture

The application follows a clean, modular architecture:

### Components (`src/components/`)
- **TemperatureChart**: Displays temperature data as a line chart (Recharts)
- **TemperatureDisplay**: Shows current temperature value with large, readable display

### Services (`src/services/`)
- **TemperatureService**: Handles all API communication with the ESP32
  - Endpoint: `http://192.168.1.100/api/temperature` (configurable in `src/config/api.config.ts`)

### Hooks (`src/hooks/`)
- **useTemperature**: Custom hook managing temperature data state and periodic updates
  - Auto-refresh every 5 seconds
  - Maintains last 20 data points

### Types (`src/types/`)
- **Temperature**: TypeScript interfaces for type safety

## 🚀 Getting Started

### Prerequisites
- Node.js (v18 or higher)
- npm or yarn
- ESP32 running as HTTP server (configured in transceiver_master sketch)

### Configuration

1. **ESP32 Setup**: 
   - Upload the `transceiver_master.ino` sketch to your ESP32
   - Note the IP address shown in the Serial Monitor after WiFi connection
   
2. **Update HMI App Configuration**:
   - Edit `src/config/api.config.ts`
   - Set `ESP32_IP` to match your ESP32's IP address

### Installation

1. Clone the repository and navigate to the project directory:
```bash
cd hmi_app
```

2. Install dependencies:
```bash
npm install
```

3. Install Recharts (if not already installed):
```bash
npm install recharts
```

### Running the Application

#### Development Mode
```bash
npm run dev
```
The application will be available at `http://localhost:5173`

#### Build for Production
```bash
npm run build
```

#### Preview Production Build
```bash
npm run preview
```

## 📡 Backend Integration

The application connects to an ESP32 master transceiver that:
1. Receives temperature data via CAN bus from slave devices
2. Hosts a REST API endpoint at `http://192.168.1.100:3000/api/temperature`
3. Returns JSON data in format:
```json
{
  "temperature": 25.50,
  "timestamp": 1234567890
}
```

### Configuration

To change the backend URL, modify `src/services/TemperatureService.ts`:
```typescript
const apiUrl = 'http://YOUR_ESP32_IP:3000/api/temperature';
```

## 📱 Responsive Design

The application adapts to different screen sizes:

- **Mobile (<768px)**: Single column layout with stacked components
- **Tablet (768px-1024px)**: Optimized spacing and font sizes
- **Desktop (>1024px)**: Grid layout with temperature display and chart side-by-side
- **Large Desktop (>1440px)**: Maximum spacing and readability

## 🎨 Design Principles

### SOLID Principles
- **Single Responsibility**: Each component has one clear purpose
  - `TemperatureService`: API communication only
  - `TemperatureChart`: Data visualization only
  - `TemperatureDisplay`: Current value display only
  
- **Open/Closed**: Components are open for extension, closed for modification
- **Interface Segregation**: Props interfaces are minimal and specific
- **Dependency Inversion**: Components depend on abstractions (interfaces/types)

### KISS (Keep It Simple, Stupid)
- Clean, readable code
- Minimal dependencies
- Straightforward component structure
- Simple state management with hooks

## 🛠️ Technology Stack

- **Framework**: React 19.2.0
- **Build Tool**: Vite 7.2.4
- **Language**: TypeScript 5.9.3
- **Charts**: Recharts 2.15.0
- **Styling**: CSS3 with CSS Variables
- **Linting**: ESLint 9.39.1

## 📂 Project Structure

```
hmi_app/
├── public/                 # Static assets
├── src/
│   ├── components/        # React components
│   │   ├── TemperatureChart.tsx
│   │   └── TemperatureDisplay.tsx
│   ├── hooks/            # Custom React hooks
│   │   └── useTemperature.ts
│   ├── services/         # API services
│   │   └── TemperatureService.ts
│   ├── types/            # TypeScript type definitions
│   │   └── Temperature.ts
│   ├── App.tsx           # Main application component
│   ├── App.css           # Application styles
│   ├── main.tsx          # Entry point
│   └── index.css         # Global styles
├── .github/
│   └── copilot-instructions.md
├── index.html
├── package.json
├── tsconfig.json
├── vite.config.ts
└── README.md
```

## 🔧 Development

### Code Quality
```bash
npm run lint         # Run ESLint
```

### Type Checking
```bash
npm run build        # TypeScript compilation + build
```

## 📊 Features in Detail

### Temperature Chart
- Shows last 20 temperature readings
- Time-based X-axis with formatted timestamps
- Auto-scaling Y-axis
- Responsive container
- Interactive tooltips
- Legend display

### Current Temperature Display
- Large, easy-to-read font
- Loading state animation
- Error state indication
- Color-coded values
- Celsius unit display

### Error Handling
- Connection error messages
- Graceful degradation
- User-friendly error displays
- Automatic retry on reconnection

## 🌙 Dark Mode

The application automatically adapts to your system's color scheme preferences:
- Light mode: Clean, bright interface
- Dark mode: Comfortable, reduced eye strain

## 📱 Accessibility

- Reduced motion support for users with vestibular disorders
- Semantic HTML structure
- Proper heading hierarchy
- ARIA labels where needed
- Keyboard navigation support

## 📄 License

This project is part of an Industrial Electronics course project.

## 👥 Authors

Industrial Electronics - Semester 12

## 🔗 Related Projects

- ESP32 Master Transceiver (CAN bus receiver)
- Temperature sensor nodes (CAN bus transmitters)

---

**Note**: Ensure your ESP32 backend is running and accessible at the configured IP address before starting the application.

