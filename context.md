# Project Context: esp32_test02

## Hardware Overview
- **MCU:** ESP32-S3 (WROOM-1 / DevKitC-1)
- **Flash:** 16MB (DIO mode, 80MHz)
- **Core Config:** Arduino runs on Core 1; Events run on Core 1.

## Pin Mapping
### Sensors (Analog)
- **NTC Tank Top:** GPIO 5
- **NTC Tank Bottom:** GPIO 4

### Actuators & Control
- **Relay (Boiler):** GPIO 38 (Output Open Drain)
- **SPI Digital Pot (MCP41010):**
  - **MOSI:** GPIO 40
  - **SCK:** GPIO 41
  - **CS:** GPIO 42
  - *Note:* MISO is unused (-1).

### Communication (RS485 / Modbus)
- **UART Port:** UART_NUM_2
- **TX:** GPIO 17
- **RX:** GPIO 16
- **DIR (RTS):** GPIO 18
- **Modbus Slave ID:** 77

### UI / LEDs
- **Top Tank LEDs:** Cold (37), Med (36), High (35)
- **Bottom Tank LEDs:** Cold (12), Med (13), High (14)

## Software Architecture
1. **`ModbusDriver`**: Manages the RS485 interface using the `Modbus-Serial` library. It operates in a dedicated FreeRTOS task and uses a Mutex (`_dataLock`) to ensure thread-safe register updates.
2. **`TemperatureSensor`**: Handles NTC thermistor logic. Uses the Steinhart-Hart equation for conversion and an Alpha filter for smoothing. It runs in a background task (`TempReading`) on Core 1.
3. **`TemperatureDisplay`**: Functional logic for updating physical LEDs based on temperature thresholds.
4. **`BoilerController`**: Controls the boiler via a digital potentiometer (SPI) and a relay. It uses a callback mechanism (`BoilerStatusCallback`) to notify the system of state changes.
5. **`RuleEngine`**: Evaluates logical rules enabled/disabled via Modbus Holding Registers.

## Modbus Register Map (Input Registers)
| Register | Name | Data Description |
| :--- | :--- | :--- |
| 0 | TOP_TANK_TEMP_REGISTER | Packed: (Status * 10000) + (Temp * 10) |
| 1 | BOTTOM_TANK_TEMP_REGISTER | Packed: (Status * 10000) + (Temp * 10) |
| 2 | BOILER_CONTROL_REGISTER | Packed: (RelayStatus * 10000) + (OutputTemp * 10) |

## Modbus Register Map (Holding Registers)
| Register | Name | Data Description |
| :--- | :--- | :--- |
| 100 | RULE_ENABLE_MASK | Bitmask: Bit 0 = Overheat, Bit 1 = Delta-T, etc. |
| 101 | TEMP_TARGET_HREG | Target temperature used by the rules (Target * 10) |

## Dependencies
- `epsilonrt/Modbus-Serial@^2.0.5`
- `thijse/ArduinoLog@^1.1.1`

## Current Status
- The project is in `debug` mode with `esp32_exception_decoder` enabled.
- Logging is configured to `LOG_LEVEL_VERBOSE`.
- Main loop is disabled via `vTaskDelete(NULL)` to allow background tasks to handle all logic.

*Last Updated: 2024-05-22*