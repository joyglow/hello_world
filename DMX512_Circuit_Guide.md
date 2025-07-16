# DMX512 Receiver Circuit Connection Guide

This document provides detailed circuit connections for implementing the DMX512 receiver with STM32F103C8T6.

## Circuit Overview

The complete system consists of:
1. STM32F103C8T6 microcontroller (Blue Pill board)
2. RS485 transceiver for DMX512 signal conversion
3. LED output circuit
4. Power supply connections

## Component Requirements

### Essential Components
- **STM32F103C8T6** (Blue Pill development board)
- **MAX485** or **SN75176** RS485 transceiver IC
- **LED** (any color, 3mm or 5mm)
- **Resistors**: 220Ω (LED current limiting), 10kΩ (pull-up/down)
- **Capacitors**: 100nF ceramic (decoupling)
- **XLR-3 Female Connector** (for DMX input)
- **Breadboard** or **PCB** for prototyping

### Optional Components
- **120Ω Resistor** (DMX termination, if this is the last device)
- **LEDs** for status indication (Power, DMX activity)
- **Filtering capacitors** (10µF electrolytic for power)

## Pin Connections

### STM32F103C8T6 Connections

| STM32 Pin | Function | Connection |
|-----------|----------|------------|
| PA0 | TIM2_CH1 (PWM) | LED Anode (via 220Ω resistor) |
| PA9 | USART1_TX | Not connected (receiver only) |
| PA10 | USART1_RX | MAX485 RO (Receiver Output) |
| PA13 | SWDIO | ST-Link debugger |
| PA14 | SWCLK | ST-Link debugger |
| 3.3V | Power | System power supply |
| GND | Ground | System ground |

### MAX485 RS485 Transceiver Connections

| MAX485 Pin | Pin Name | Connection |
|------------|----------|------------|
| 1 | RO | STM32 PA10 (USART1_RX) |
| 2 | RE | GND (receive enable - always enabled) |
| 3 | DE | GND (driver enable - always disabled) |
| 4 | DI | Not connected (transmit not used) |
| 5 | GND | System ground |
| 6 | A | DMX+ (XLR Pin 3) |
| 7 | B | DMX- (XLR Pin 2) |
| 8 | VCC | +5V or +3.3V (check datasheet) |

### XLR-3 DMX512 Connector

| XLR Pin | Signal | Connection |
|---------|--------|------------|
| 1 | Shield/Ground | Chassis ground (optional) |
| 2 | DMX- | MAX485 Pin 7 (B) |
| 3 | DMX+ | MAX485 Pin 6 (A) |

### LED Output Circuit

```
PA0 (STM32) ──── 220Ω Resistor ──── LED Anode
                                      │
                                    LED Cathode
                                      │
                                    GND
```

## Complete Circuit Diagram (Text Format)

```
           DMX512 Input                     STM32F103C8T6              LED Output
              (XLR-3)                       (Blue Pill)
                │                              │
    ┌───────────┴───────────┐              ┌───┴────┐               ┌─────┐
    │ Pin 2 (DMX-)  Pin 3   │              │        │    PA0 ──220Ω─┤ LED ├─ GND
    │     │       (DMX+)    │              │        │               └─────┘
    │     │          │      │              │        │
    │   ┌─┴─┐      ┌─┴─┐    │              │        │
    │   │ B │      │ A │    │              │        │
    │   └─┬─┘      └─┬─┘    │              │        │
    └─────┼──────────┼──────┘              │        │
          │          │                     │        │
    ┌─────┴──────────┴─────┐              │        │
    │      MAX485          │              │        │
    │  ┌─┐ ┌─┐ ┌─┐ ┌─┐     │              │        │
    │  │8│ │7│ │6│ │5│     │              │        │
    │  └┬┘ └┬┘ └┬┘ └┬┘     │              │        │
    │   │   │   │   │      │              │        │
    │ VCC   B   A  GND     │              │        │
    └───┬───────────┬──────┘              │        │
        │           │                     │        │
       +5V         GND                    │        │
                                          │        │
    ┌─────────────────────────────────────┼────────┼──┐
    │  ┌─┐ ┌─┐ ┌─┐ ┌─┐                   │        │  │
    │  │1│ │2│ │3│ │4│                   │        │  │
    │  └┬┘ └┬┘ └┬┘ └┬┘                   │        │  │
    │   │   │   │   │                    │        │  │
    │  RO  RE  DE  DI                    │        │  │
    └───┬───┬───┬───────────────────────────────────┬──┘
        │   │   │                               │
      PA10 GND GND                            GND
```

## Power Supply Options

### Option 1: USB Power (Recommended for Development)
- Connect USB cable to Blue Pill board
- Provides +5V and +3.3V regulated power
- Sufficient current for MAX485 and LED

### Option 2: External 5V Supply
- Connect +5V to Blue Pill 5V pin
- Connect ground to Blue Pill GND pin
- Ensure minimum 100mA current capability

### Option 3: 3.3V Only (Check MAX485 Datasheet)
- Some MAX485 variants work with 3.3V
- Connect +3.3V to Blue Pill 3.3V pin
- Lower power consumption

## Construction Steps

### Step 1: Prepare the Base
1. Insert Blue Pill board into breadboard
2. Connect power rails (+5V, +3.3V, GND)
3. Add decoupling capacitor (100nF) near STM32

### Step 2: Install RS485 Transceiver
1. Insert MAX485 IC into breadboard
2. Connect VCC to appropriate power rail
3. Connect GND to ground rail
4. Wire RE and DE pins to ground

### Step 3: Connect DMX Interface
1. Solder wires to XLR connector
2. Connect DMX+ to MAX485 A terminal
3. Connect DMX- to MAX485 B terminal
4. Connect shield to chassis ground (optional)

### Step 4: Connect STM32 Interface
1. Wire MAX485 RO to STM32 PA10
2. Ensure common ground connection
3. Add pull-up resistor on RO line (optional, 10kΩ)

### Step 5: Install LED Circuit
1. Insert LED into breadboard
2. Connect 220Ω resistor in series with anode
3. Connect resistor to STM32 PA0
4. Connect LED cathode to ground

### Step 6: Final Connections
1. Double-check all connections
2. Verify power supply voltages
3. Connect ST-Link debugger for programming

## Testing Procedure

### Initial Power-On Test
1. Connect power to circuit
2. Verify LED turns on briefly (power-on indication)
3. Check voltage levels with multimeter
4. Program the STM32 with DMX receiver code

### DMX Signal Test
1. Connect DMX transmitter or console
2. Set DMX channel 2 to different values
3. Observe LED brightness changes
4. Test with values: 0, 64, 128, 192, 255

### Debug Connections
1. Connect ST-Link to SWDIO/SWCLK pins
2. Use STM32CubeIDE for debugging
3. Monitor UART reception in debugger
4. Check DMX data buffer contents

## Troubleshooting

### No LED Response
- Check power supply connections
- Verify MAX485 VCC voltage
- Test with known good DMX source
- Check LED polarity and current limiting resistor

### Erratic LED Behavior
- Add 100nF capacitor across DMX+/DMX- (EMI filtering)
- Check DMX cable quality and connectors
- Verify 120Ω termination if last device in chain
- Ensure stable power supply

### Programming Issues
- Check ST-Link connections
- Verify 3.3V power to STM32
- Reset STM32 before programming
- Check for proper ground connections

## Safety Considerations

### Electrical Safety
- Always disconnect power when making connections
- Use appropriate wire gauges for current levels
- Insulate all connections properly
- Ground chassis if using metal enclosure

### DMX Network Safety
- Never connect power supplies between DMX devices
- Use proper DMX cables (120Ω characteristic impedance)
- Terminate the last device in chain with 120Ω resistor
- Keep DMX cable runs under maximum specified length (500m)

## Performance Specifications

### Timing Requirements
- DMX break: minimum 88µs
- Mark after break: minimum 8µs
- Character time: 44µs (11 bits at 250kbps)
- Frame rate: typically 44Hz (complete 512-channel frame)

### Signal Levels
- DMX differential voltage: ±2V to ±6V
- Common mode voltage: -1V to +6V
- Input impedance: 120Ω (when terminated)

This circuit provides a robust and reliable DMX512 receiver implementation suitable for professional lighting control applications.