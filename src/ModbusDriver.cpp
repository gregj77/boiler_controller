#include <Arduino.h>
#include "ModbusDriver.h"

#define TOP_TANK_TEMP_REGISTER 0
#define BOTTOM_TANK_TEMP_REGISTER 1

ModbusDriver::ModbusDriver(uint8_t slaveId) noexcept
: _serialPort(Serial2, slaveId)
, _dataLock(xSemaphoreCreateMutex()) {}

void ModbusDriver::initializeTaskLoop() {
    if (_isInitialized) {
        return;
    }

    _isInitialized = true;

    Serial2.begin(BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN);
    _serialPort.config(BAUD_RATE);
    uart_driver_delete(UART_PORT);
    uart_config_t uart_config = {
        .baud_rate = BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_XTAL,
    };
    uart_driver_install(UART_PORT, 1024, 0, 20, &_serialQueue, 0);
    uart_param_config(UART_PORT, &uart_config);
    uart_set_pin(UART_PORT, TX_PIN, RX_PIN, DIR_PIN, UART_PIN_NO_CHANGE);
    uart_set_mode(UART_PORT, UART_MODE_RS485_HALF_DUPLEX);

    _serialPort.addIreg(TOP_TANK_TEMP_REGISTER); 
    _serialPort.addIreg(BOTTOM_TANK_TEMP_REGISTER);

    xTaskCreate([](void *ptr) {
        static_cast<ModbusDriver*>(ptr)->onSerialDataReceived();
    }, "ModbusTask", 5120, this, 10, NULL);
}

void ModbusDriver::onSerialDataReceived() {
    uart_event_t event;
    while (true) {
        // Czekaj na zdarzenie z kolejki UART (odpowiednik przerwania)
        if (xQueueReceive(_serialQueue, static_cast<void*>(&event), portMAX_DELAY)) {
            if (event.type == UART_DATA) {
                if (xSemaphoreTake(_dataLock, pdMS_TO_TICKS(50)) == pdTRUE) {                      
                    _serialPort.task();
                    xSemaphoreGive(_dataLock);
                }
            } else if (event.type == UART_FIFO_OVF || event.type == UART_BUFFER_FULL) {
                uart_flush_input(UART_PORT);
                xQueueReset(_serialQueue);
            }
        }
    }
}

void ModbusDriver::onNewTankTopTemperature(float temperatureReading, TempReading ledStatus ) {
    uint16_t data = packData(temperatureReading, ledStatus);

    if (xSemaphoreTake(_dataLock, pdMS_TO_TICKS(50)) == pdTRUE) {
        _serialPort.Ireg(TOP_TANK_TEMP_REGISTER, data);
        xSemaphoreGive(_dataLock);
    }
}

void ModbusDriver::onNewTankBottomTemperature(float temperatureReading, TempReading ledStatus) {
    uint16_t data = packData(temperatureReading, ledStatus);
    if (xSemaphoreTake(_dataLock, pdMS_TO_TICKS(50)) == pdTRUE) {
        _serialPort.Ireg(BOTTOM_TANK_TEMP_REGISTER, data);
        xSemaphoreGive(_dataLock);
    }
}

uint16_t ModbusDriver::packData(float temperatureReading, TempReading ledStatus) const {
    uint16_t rawTemp = static_cast<uint16_t>(temperatureReading * 10) & 0x03FF;
    uint16_t rawLed = static_cast<uint16_t>(ledStatus) * 10000;

    return rawTemp + rawLed;
}