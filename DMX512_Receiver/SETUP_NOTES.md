# Additional Files Required

To complete this project, you'll need to add the following files from STM32CubeMX or STM32CubeF1 package:

## 1. Startup File
Download and place in the project root:
- `startup_stm32f103xb.s` - Assembly startup code for STM32F103C8T6

## 2. STM32 HAL Drivers
Create a `Drivers` directory and add:
- `Drivers/STM32F1xx_HAL_Driver/` - Complete HAL driver library
- `Drivers/CMSIS/` - CMSIS library files

These can be obtained from:
1. STM32CubeMX - Generate a basic project and copy the Drivers folder
2. STM32CubeF1 package - Download from ST website
3. GitHub: https://github.com/STMicroelectronics/STM32CubeF1

## Directory Structure After Adding Files
```
DMX512_Receiver/
├── Core/
│   ├── Inc/
│   │   ├── main.h
│   │   ├── dmx512.h
│   │   ├── stm32f1xx_it.h
│   │   └── stm32f1xx_hal_conf.h
│   └── Src/
│       ├── main.c
│       ├── dmx512.c
│       ├── stm32f1xx_it.c
│       ├── stm32f1xx_hal_msp.c
│       └── system_stm32f1xx.c
├── Drivers/
│   ├── STM32F1xx_HAL_Driver/
│   │   ├── Inc/
│   │   └── Src/
│   └── CMSIS/
│       ├── Device/
│       └── Include/
├── build/
├── .project
├── Makefile
├── STM32F103C8Tx_FLASH.ld
├── startup_stm32f103xb.s
├── README.md
└── SETUP_NOTES.md
```

## Quick Setup Using STM32CubeMX

1. Open STM32CubeMX
2. Select STM32F103C8T6
3. Configure:
   - USART1: Asynchronous mode
   - TIM2: Channel 1 PWM Generation
   - System Clock: 72MHz (HSE with PLL)
4. Generate code
5. Copy the `Drivers` folder and `startup_stm32f103xb.s` to this project

## Building Without STM32CubeIDE

If using command-line Make:
1. Install ARM GCC toolchain: `arm-none-eabi-gcc`
2. Ensure all driver files are in place
3. Run `make` in the project directory

The project is configured to handle DMX512 protocol specifics that STM32CubeMX cannot generate automatically.