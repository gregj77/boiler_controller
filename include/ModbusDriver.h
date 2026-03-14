#pragma once

#include <ModbusSerial.h>
#include <driver/uart.h>
#include "TemperatureDisplay.h"

class ModbusDriver final {
    private:
        static constexpr uint8_t RX_PIN = 16;
        static constexpr uint8_t TX_PIN = 17;
        static constexpr uint8_t DIR_PIN = 18;
        static constexpr uint8_t UART_PORT = UART_NUM_2;
        static constexpr uint8_t BAUD_RATE = 9600;

        ModbusSerial _serialPort;
        QueueHandle_t _serialQueue;
        SemaphoreHandle_t _dataLock;
        bool _isInitialized = false;
        
    public:
        explicit ModbusDriver(uint8_t slaveId) noexcept;
        ModbusDriver() = delete;
        ModbusDriver(const ModbusDriver&) = delete;
        ModbusDriver(ModbusDriver&&) = delete;
        ModbusDriver& operator=(const ModbusDriver&) = delete;
        ModbusDriver& operator=(ModbusDriver&&) = delete;

        void onNewTankTopTemperature(float temperatureReading, TempReading ledStatus);
        void onNewTankBottomTemperature(float temperatureReading, TempReading ledStatus);

        void initializeTaskLoop();

    private:
        void onSerialDataReceived();
        uint16_t packData(float temperatureReading, TempReading ledStatus) const;
};