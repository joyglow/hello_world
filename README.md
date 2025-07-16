# DMX512 Receiver for STM32F103C8T6

A complete DMX512 receiver implementation for STM32F103C8T6 microcontroller that receives DMX channel 2 and controls an LED with PWM output.

## Features

- **DMX512 Protocol Support**: Full DMX512 receiver implementation with proper break detection and frame parsing
- **Channel-specific Control**: Monitors DMX channel 2 and maps its value to LED brightness
- **PWM LED Control**: Uses TIM2 Channel 1 (PA0) for smooth PWM control of LED brightness
- **Error Handling**: Robust error handling with timeout detection for DMX signal loss
- **STM32CubeIDE Compatible**: Complete project structure for STM32CubeIDE
- **Interrupt-driven**: Efficient UART interrupt-based DMX reception

## Hardware Requirements

### Microcontroller
- STM32F103C8T6 (Blue Pill board or equivalent)
- 8MHz external crystal oscillator
- System clock configured to 72MHz

### Pin Configuration
| Pin | Function | Description |
|-----|----------|-------------|
| PA0 | TIM2_CH1 | PWM output for LED control |
| PA9 | USART1_TX | UART transmit (not used in receiver) |
| PA10 | USART1_RX | DMX512 data input |
| PA13 | SWD_IO | Debug interface |
| PA14 | SWD_CLK | Debug interface |

### DMX512 Interface Circuit
For proper DMX512 reception, you'll need an RS485 to TTL converter:
- Use a MAX485 or SN75176 IC
- Connect DE and RE pins to ground (receive mode)
- Connect A and B to DMX+ and DMX- respectively
- Connect the output to PA10 (USART1_RX)

### LED Connection
- Connect LED with appropriate current-limiting resistor to PA0
- LED brightness will be controlled by PWM signal (0-255 levels)

## Software Configuration

### UART Settings
- **Baud Rate**: 250,000 bps (DMX512 standard)
- **Data Bits**: 8
- **Stop Bits**: 2
- **Parity**: None
- **Mode**: Receive only

### Timer Settings (TIM2)
- **Prescaler**: 281 (for ~1kHz PWM frequency at 72MHz)
- **Period**: 255 (8-bit resolution)
- **Channel 1**: PWM output on PA0

### DMX512 Parameters
- **Monitored Channel**: Channel 2
- **Start Code**: 0x00 (standard DMX)
- **Break Detection**: Framing error detection
- **Timeout**: 5 seconds for signal loss detection

## Project Structure

```
DMX512_Receiver_STM32F103/
├── Core/
│   ├── Inc/
│   │   ├── main.h
│   │   ├── dmx512.h
│   │   ├── stm32f1xx_hal_conf.h
│   │   └── stm32f1xx_it.h
│   └── Src/
│       ├── main.c
│       ├── dmx512.c
│       ├── stm32f1xx_hal_msp.c
│       ├── stm32f1xx_it.c
│       └── system_stm32f1xx.c
├── Drivers/
│   ├── STM32F1xx_HAL_Driver/
│   └── CMSIS/
├── Makefile
├── STM32F103C8Tx_FLASH.ld
├── startup_stm32f103xb.s
├── DMX512_Receiver_STM32F103.ioc
└── README.md
```

## Building the Project

### Using STM32CubeIDE
1. Import the project into STM32CubeIDE
2. Build the project using the IDE's build system
3. Flash the generated `.elf` file to your STM32F103C8T6

### Using Command Line (Make)
Ensure you have the ARM GCC toolchain installed:

```bash
# Build the project
make

# Clean build files
make clean

# Flash using OpenOCD (if configured)
openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program build/DMX512_Receiver_STM32F103.elf verify reset exit"
```

## Usage

1. **Power up the system** with the STM32F103C8T6 properly connected
2. **Connect DMX512 signal** to the RS485 interface
3. **Send DMX data** with channel 2 containing the desired LED brightness value (0-255)
4. **Observe LED brightness** changing according to DMX channel 2 value

### DMX Channel Mapping
- **Channel 1**: Not used (reserved for dimmer/intensity in typical DMX fixtures)
- **Channel 2**: LED brightness control (0 = off, 255 = full brightness)
- **Channels 3-512**: Available for future expansion

## Code Overview

### DMX512 Driver (`dmx512.c`)
- **State Machine**: Implements DMX protocol state machine (IDLE, BREAK, START_CODE, DATA)
- **Break Detection**: Uses UART framing errors to detect DMX break conditions
- **Interrupt Handling**: Processes incoming data via UART interrupts
- **Buffer Management**: Stores up to 512 DMX channels in memory

### Main Application (`main.c`)
- **Initialization**: Sets up peripherals and starts DMX reception
- **Main Loop**: Monitors for complete DMX frames and updates LED accordingly
- **PWM Control**: Maps DMX values to PWM duty cycle for smooth LED control
- **Timeout Handling**: Turns off LED if no DMX signal for 5 seconds

## Customization

### Changing DMX Channel
To monitor a different DMX channel, modify the `DMX_CHANNEL_NUMBER` definition in `main.h`:
```c
#define DMX_CHANNEL_NUMBER  3   // Change to desired channel (1-512)
```

### Adjusting PWM Frequency
To change PWM frequency, modify the timer prescaler in `MX_TIM2_Init()`:
```c
htim2.Init.Prescaler = 281;  // Adjust for different frequency
```

### Adding Multiple Channels
The DMX driver supports all 512 channels. You can easily extend the main application to control multiple outputs:
```c
uint8_t red_value = DMX_GetChannelData(&hdmx, 1);
uint8_t green_value = DMX_GetChannelData(&hdmx, 2);
uint8_t blue_value = DMX_GetChannelData(&hdmx, 3);
```

## Troubleshooting

### No LED Response
1. Check DMX512 wiring and RS485 converter
2. Verify DMX transmitter is sending data on channel 2
3. Use a logic analyzer to verify UART data reception
4. Check LED and current-limiting resistor connection

### Intermittent Operation
1. Verify stable power supply
2. Check DMX cable quality and termination
3. Ensure proper grounding between DMX and microcontroller
4. Verify external crystal oscillator operation

### LED Always On/Off
1. Check PWM output with oscilloscope
2. Verify TIM2 configuration and clock settings
3. Test with different DMX channel values
4. Check for proper LED polarity

## License

This project is provided as-is for educational and development purposes. STM32 HAL drivers are subject to STMicroelectronics license terms.

## Support

For technical questions or issues, please refer to:
- STM32F103 Reference Manual
- DMX512 Protocol Specification (USITT DMX512-A)
- STM32CubeIDE Documentation
