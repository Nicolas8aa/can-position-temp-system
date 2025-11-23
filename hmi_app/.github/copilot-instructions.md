# Temperature Monitoring HMI Application

This is a React TypeScript application built with Vite for real-time temperature monitoring from an ESP32 CAN bus system.

## Project Type
- Framework: Vite + React + TypeScript
- Visualization: Recharts
- Design Principles: SOLID, KISS
- Responsive Design: Mobile and Desktop

## Architecture
- API Service Layer for backend communication
- Component-based architecture following SOLID principles
- Real-time data visualization with Recharts

## Backend Integration
- API Endpoint: http://192.168.1.100:3000/api/temperature
- Data Source: ESP32 master transceiver receiving CAN bus temperature data
