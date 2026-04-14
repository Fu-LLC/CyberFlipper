# CYBER BUS — Hardware Research Cheat Sheet
Integrated logic for ESP32-Bus-Pirate and hardware debugging.

## Common AT Commands (GSM)
- `AT+CSQ` : Signal Quality
- `AT+COPS?` : Check Operator
- `AT+CREG?` : Registration Status
- `AT+CBC` : Battery Connection

## I2C Protocol (Bus Pirate Mode)
- `[` : I2C Start
- `]` : I2C Stop
- `{` : I2C Address Scan
- `R` : Read Byte
- `W` : Write Byte

## SPI Protocol (Bus Pirate Mode)
- `[` : Chip Select Low
- `]` : Chip Select High
- `r` : Read Byte
- `0xAB` : Send Hex Value

## UART Protocol (Bus Pirate Mode)
- `(1)` : Set Baudrate 9600
- `(2)` : Set Baudrate 115200
- `v` : Show Voltage Rails
