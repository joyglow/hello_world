# DMX512 Receiver for STM32F103C8T6

A complete DMX512 receiver solution for the STM32F103C8T6 microcontroller that receives DMX512 signals and controls an LED using PWM output on Timer2.

## Features

- **DMX512 Reception**: Receives standard DMX512 signals at 250,000 baud
- **Channel 2 Control**: Specifically monitors DMX channel 2 for LED control
- **PWM Output**: Uses Timer2 to generate PWM signal for LED brightness control
- **Status LED**: Built-in status LED on PC13 for debugging
- **Interrupt-based**: Efficient interrupt-driven DMX reception

## Hardware Requirements

- STM32F103C8T6 Blue Pill or compatible board
- 8MHz crystal oscillator (external)
- LED with current-limiting resistor (220Ω recommended)
- DMX512 signal source (DMX controller, lighting console, etc.)
- DMX512 to RS485 converter (if needed)

## Pin Connections

| STM32 Pin | Function | Description |
|-----------|----------|-------------|
| PA10      | USART1_RX | DMX512 signal input |
| PA1       | TIM2_CH1 | PWM output for LED control |
| PC13      | GPIO_Output | Status LED |
| PA9       | USART1_TX | (Optional) Debug output |

## DMX512 Protocol Implementation

The receiver implements the standard DMX512 protocol:
- **Baud Rate**: 250,000 bps
- **Data Bits**: 8
- **Stop Bits**: 2
- **Parity**: None
- **Start Code**: 0x00 (standard DMX start code)

## PWM Configuration

- **Timer**: TIM2
- **Channel**: Channel 1 (PA1)
- **Frequency**: 1kHz (72MHz / 72 / 1000)
- **Resolution**: 10-bit (0-1000)
- **Duty Cycle**: 0-100% based on DMX channel 2 value

## Software Architecture

### Main Components

1. **UART Reception**: Interrupt-driven DMX512 data reception
2. **DMX Parser**: Processes incoming DMX frames and extracts channel data
3. **PWM Controller**: Updates LED brightness based on DMX channel 2
4. **Status Monitoring**: Visual feedback via status LED

### Key Functions

- `HAL_UART_RxCpltCallback()`: Handles received DMX data
- `HAL_UART_ErrorCallback()`: Error recovery for UART issues
- Main loop: Updates PWM duty cycle based on received data

## Setup Instructions

### 1. STM32CubeIDE Setup

1. Open STM32CubeIDE
2. Import the project using the `.ioc` file
3. Configure the project settings:
   - Target: STM32F103C8T6
   - Toolchain: STM32CubeIDE
   - Debug: SWD

### 2. Hardware Setup

1. Connect 8MHz crystal to PH0/PH1
2. Connect LED to PA1 through 220Ω resistor
3. Connect DMX signal to PA10
4. Connect status LED to PC13 (optional)

### 3. DMX512 Signal Connection

For proper DMX512 reception, you may need:
- **RS485 to TTL converter** if your DMX source uses RS485
- **Signal level conversion** if needed (DMX typically uses 0-5V levels)
- **Proper grounding** between DMX source and receiver

### 4. Compilation and Flashing

1. Build the project in STM32CubeIDE
2. Connect ST-Link or compatible programmer
3. Flash the binary to the STM32F103C8T6

## Usage

1. Power on the STM32 board
2. Connect DMX512 signal to PA10
3. Send DMX data with channel 2 controlling LED brightness
4. LED brightness will vary from 0-100% based on channel 2 value (0-255)

## DMX Channel Mapping

- **Channel 1**: Not used (reserved for future expansion)
- **Channel 2**: LED brightness control (0-255)
- **Channels 3-512**: Received but not processed

## Troubleshooting

### Common Issues

1. **No LED Response**:
   - Check DMX signal connection
   - Verify DMX controller is sending data
   - Check LED and resistor connections

2. **Erratic Behavior**:
   - Verify proper grounding
   - Check signal levels (should be 0-3.3V for STM32)
   - Ensure DMX baud rate is 250,000

3. **Compilation Errors**:
   - Ensure STM32F1 HAL library is installed
   - Check STM32CubeIDE version compatibility

### Debug Features

- Status LED on PC13 provides visual feedback
- UART TX on PA9 can be used for debug output (requires additional code)

## Customization

### Changing DMX Channel

To control LED from a different DMX channel, modify the `LED_CHANNEL` define in `main.c`:

```c
#define LED_CHANNEL 5  // Use DMX channel 5 instead of 2
```

### PWM Frequency Adjustment

To change PWM frequency, modify Timer2 configuration:

```c
htim2.Init.Prescaler = 71;  // Adjust for different frequency
htim2.Init.Period = 999;    // Adjust for different resolution
```

### Multiple LED Control

To control multiple LEDs, add additional Timer channels and modify the main loop accordingly.

## License

This project is provided as-is for educational and development purposes.

## Contributing

Feel free to submit issues, feature requests, or pull requests to improve this DMX512 receiver implementation.
