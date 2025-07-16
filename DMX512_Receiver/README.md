# DMX512 Receiver for STM32F103C8T6

This project implements a DMX512 receiver using STM32F103C8T6 microcontroller that reads channel 2 data and controls an LED using PWM.

## Features

- DMX512 protocol receiver implementation
- Reads DMX channel 2 (configurable)
- Controls LED brightness using PWM on TIM2 Channel 1 (PA0)
- Break signal detection using UART framing error
- Efficient state machine implementation
- 250kbaud UART communication (DMX512 standard)

## Hardware Requirements

- STM32F103C8T6 (Blue Pill board)
- DMX512 interface circuit (MAX485 or similar RS-485 transceiver)
- LED with current limiting resistor
- Power supply (3.3V for MCU, 5V for DMX interface)

## Pin Connections

| STM32 Pin | Function | Connection |
|-----------|----------|------------|
| PA10 | USART1_RX | DMX512 receive (from RS-485 transceiver) |
| PA0 | TIM2_CH1 | LED PWM output |
| PC13 | GPIO | Status/Error LED (built-in on Blue Pill) |

## DMX512 Interface Circuit

Connect a MAX485 or similar RS-485 transceiver:
- DMX+ to A pin of MAX485
- DMX- to B pin of MAX485
- RO (Receiver Output) to PA10 (USART1_RX)
- RE (Receiver Enable) to GND (always enabled)
- DE (Driver Enable) to GND (transmitter disabled)
- VCC to 5V
- GND to GND

## Building the Project

### Using STM32CubeIDE
1. Import the project into STM32CubeIDE
2. Build the project (Ctrl+B)
3. Flash to the MCU using ST-Link

### Using Command Line (Make)
```bash
cd DMX512_Receiver
make
```

The output files will be in the `build` directory:
- `DMX512_Receiver.elf` - Debug file
- `DMX512_Receiver.hex` - Intel HEX format
- `DMX512_Receiver.bin` - Binary format

## Flashing the Firmware

Using ST-Link and st-flash:
```bash
st-flash write build/DMX512_Receiver.bin 0x8000000
```

Using OpenOCD:
```bash
openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program build/DMX512_Receiver.elf verify reset exit"
```

## Configuration

To change the DMX channel to read, modify in `main.c`:
```c
#define DMX_CHANNEL_TO_READ 2  // Change to desired channel (1-512)
```

To change the PWM output pin, modify:
```c
#define LED_PWM_CHANNEL TIM_CHANNEL_1  // TIM2 channel selection
```

## DMX512 Protocol Details

The implementation handles:
- Break detection (>88μs low signal) using UART framing error
- Mark After Break (MAB) timing
- Start code validation (0x00 for standard DMX)
- 512 channel data reception
- Frame timeout detection

## LED Control

- DMX value 0 = LED off
- DMX value 255 = LED full brightness
- Linear mapping from DMX (0-255) to PWM duty cycle (0-100%)
- 1kHz PWM frequency

## Troubleshooting

1. **No DMX reception**: 
   - Check wiring to RS-485 transceiver
   - Verify DMX signal is present
   - Check UART configuration (250kbaud, 8N2)

2. **LED not responding**:
   - Verify PA0 connection
   - Check LED polarity
   - Ensure proper current limiting resistor

3. **Error LED blinking** (PC13):
   - Indicates Error_Handler() was called
   - Check for hardware faults or configuration issues

## License

This project is provided as-is for educational and commercial use.