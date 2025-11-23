# ESP32 Temperature Monitor - Setup Guide

## Architecture Overview

```
┌─────────────┐       CAN Bus      ┌─────────────┐
│   Slave     │ ◄──────────────────►│   Master    │
│   ESP32     │   (Temperature)     │   ESP32     │
└─────────────┘                     └──────┬──────┘
                                           │
                                         WiFi
                                           │
                                      ┌────▼─────┐
                                      │   HMI    │
                                      │   App    │
                                      │ (Browser)│
                                      └──────────┘
```

**Key Points:**
- **No separate backend needed** - ESP32 Master acts as the HTTP server
- HMI app connects directly to ESP32 via WiFi
- Temperature data flows: Slave → CAN → Master → HTTP → Browser

## Step-by-Step Setup

### 1. ESP32 Master Configuration

1. Open `transceiver_master/transceiver_master.ino`
2. Configure your WiFi credentials:
   ```cpp
   #define WIFI_SSID "YOUR_WIFI_NAME"
   #define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
   ```
3. Upload sketch to ESP32
4. Open Serial Monitor (115200 baud)
5. **Note the IP address** displayed (e.g., `192.168.1.100`)

### 2. HMI App Configuration

1. Navigate to the HMI app directory:
   ```bash
   cd hmi_app
   ```

2. Install dependencies:
   ```bash
   npm install
   ```

3. Update ESP32 IP address in `src/config/api.config.ts`:
   ```typescript
   export const API_CONFIG = {
     ESP32_IP: '192.168.1.100',  // ← Change to your ESP32's IP
     ...
   }
   ```

4. Start the development server:
   ```bash
   npm run dev
   ```

5. Open browser at `http://localhost:5173`

### 3. Verify Connection

1. Check ESP32 Serial Monitor - should show "HTTP server started"
2. Test ESP32 endpoint directly in browser:
   - Visit: `http://YOUR_ESP32_IP/api/temperature`
   - Should return JSON: `{"temperature":25.50,"timestamp":12345}`
3. HMI app should display temperature and update every 5 seconds

## Troubleshooting

### HMI can't connect to ESP32
- ✓ Both devices on same WiFi network?
- ✓ ESP32 IP address correct in `api.config.ts`?
- ✓ Firewall blocking connection?
- ✓ Test ESP32 endpoint directly in browser first

### Temperature shows 0.00
- ✓ Is the Slave ESP32 running and sending CAN messages?
- ✓ Check CAN bus wiring (H, L, GND)
- ✓ Verify CAN bitrate matches on both devices

### CORS errors in browser console
- ESP32 code includes CORS headers - should work
- If issues persist, check browser console for specific error

## API Endpoints

### GET `/api/temperature`
Returns current temperature data from CAN bus

**Response:**
```json
{
  "temperature": 25.50,
  "timestamp": 12345
}
```

### GET `/`
Health check endpoint

**Response:**
```
ESP32 Temperature Server - Use /api/temperature
```

## Design Principles Applied

### KISS (Keep It Simple, Stupid)
- Direct ESP32 → Browser communication
- No unnecessary middleware or backend servers
- Simple HTTP GET requests
- JSON data format

### SOLID Principles (HMI App)
- **Single Responsibility**: Each component/service has one job
- **Open/Closed**: Easy to extend (add new sensors) without modifying core
- **Dependency Inversion**: Services injected via hooks, not hardcoded

## Performance Notes

- **Refresh Rate**: 5 seconds (configurable in `useTemperature` hook)
- **Data Points**: Last 20 readings kept in memory
- **Network**: Lightweight JSON payloads (~50 bytes)
- **ESP32 Load**: Minimal - serves static JSON on demand
