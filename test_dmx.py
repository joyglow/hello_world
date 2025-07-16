#!/usr/bin/env python3
"""
DMX512 Test Script for STM32F103C8T6 Receiver
This script simulates a DMX512 controller to test the receiver functionality.
"""

import serial
import time
import struct

class DMX512Simulator:
    def __init__(self, port='/dev/ttyUSB0', baudrate=250000):
        """
        Initialize DMX512 simulator
        
        Args:
            port (str): Serial port (e.g., '/dev/ttyUSB0' on Linux, 'COM3' on Windows)
            baudrate (int): Baud rate (250000 for DMX512)
        """
        self.port = port
        self.baudrate = baudrate
        self.serial = None
        self.dmx_data = [0] * 512  # 512 channels, all initialized to 0
        
    def connect(self):
        """Connect to serial port"""
        try:
            self.serial = serial.Serial(
                port=self.port,
                baudrate=self.baudrate,
                bytesize=serial.EIGHTBITS,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_TWO,
                timeout=1
            )
            print(f"Connected to {self.port} at {self.baudrate} baud")
            return True
        except serial.SerialException as e:
            print(f"Failed to connect: {e}")
            return False
    
    def disconnect(self):
        """Disconnect from serial port"""
        if self.serial and self.serial.is_open:
            self.serial.close()
            print("Disconnected")
    
    def set_channel(self, channel, value):
        """
        Set a specific DMX channel value
        
        Args:
            channel (int): Channel number (1-512)
            value (int): Channel value (0-255)
        """
        if 1 <= channel <= 512 and 0 <= value <= 255:
            self.dmx_data[channel - 1] = value
        else:
            print(f"Invalid channel ({channel}) or value ({value})")
    
    def send_dmx_frame(self):
        """Send a complete DMX512 frame"""
        if not self.serial or not self.serial.is_open:
            print("Not connected")
            return False
        
        try:
            # DMX512 frame structure:
            # 1. Break signal (88 microseconds of low)
            # 2. Mark After Break (8 microseconds of high)
            # 3. Start code (0x00)
            # 4. 512 channel data bytes
            
            # Send break signal (88us low)
            self.serial.setBreak(True)
            time.sleep(0.000088)  # 88 microseconds
            self.serial.setBreak(False)
            
            # Mark after break (8us high)
            time.sleep(0.000008)  # 8 microseconds
            
            # Send start code
            self.serial.write(bytes([0x00]))
            
            # Send channel data
            self.serial.write(bytes(self.dmx_data))
            
            return True
            
        except Exception as e:
            print(f"Error sending DMX frame: {e}")
            return False
    
    def test_led_control(self):
        """Test LED control using channel 2"""
        print("Testing LED control with channel 2...")
        
        # Test pattern: fade up and down
        for brightness in range(0, 256, 10):
            self.set_channel(2, brightness)
            self.send_dmx_frame()
            print(f"Channel 2: {brightness}")
            time.sleep(0.1)
        
        for brightness in range(255, -1, -10):
            self.set_channel(2, brightness)
            self.send_dmx_frame()
            print(f"Channel 2: {brightness}")
            time.sleep(0.1)
    
    def test_all_channels(self):
        """Test all channels with a moving pattern"""
        print("Testing all channels with moving pattern...")
        
        for position in range(0, 512, 10):
            # Clear all channels
            self.dmx_data = [0] * 512
            
            # Set a moving light pattern
            for i in range(max(0, position-5), min(512, position+5)):
                if 0 <= i < 512:
                    self.dmx_data[i] = 255
            
            self.send_dmx_frame()
            print(f"Pattern at position {position}")
            time.sleep(0.1)
    
    def continuous_test(self, duration=30):
        """Run continuous test for specified duration"""
        print(f"Running continuous test for {duration} seconds...")
        
        start_time = time.time()
        frame_count = 0
        
        while time.time() - start_time < duration:
            # Create a moving wave pattern
            wave_position = int((time.time() - start_time) * 10) % 512
            
            # Clear all channels
            self.dmx_data = [0] * 512
            
            # Create wave effect
            for i in range(512):
                distance = abs(i - wave_position)
                if distance < 50:
                    brightness = int(255 * (1 - distance / 50))
                    self.dmx_data[i] = max(0, brightness)
            
            if self.send_dmx_frame():
                frame_count += 1
            
            time.sleep(0.02)  # 50 FPS
        
        print(f"Sent {frame_count} frames in {duration} seconds")

def main():
    """Main test function"""
    print("DMX512 Simulator for STM32F103C8T6 Receiver Test")
    print("=" * 50)
    
    # Configure your serial port here
    port = input("Enter serial port (e.g., /dev/ttyUSB0, COM3): ").strip()
    if not port:
        port = '/dev/ttyUSB0'  # Default for Linux
    
    simulator = DMX512Simulator(port=port)
    
    if not simulator.connect():
        return
    
    try:
        while True:
            print("\nSelect test:")
            print("1. LED Control Test (Channel 2)")
            print("2. All Channels Test")
            print("3. Continuous Test")
            print("4. Manual Channel Control")
            print("5. Exit")
            
            choice = input("Enter choice (1-5): ").strip()
            
            if choice == '1':
                simulator.test_led_control()
            elif choice == '2':
                simulator.test_all_channels()
            elif choice == '3':
                duration = input("Enter test duration in seconds (default 30): ").strip()
                try:
                    duration = int(duration) if duration else 30
                except ValueError:
                    duration = 30
                simulator.continuous_test(duration)
            elif choice == '4':
                channel = input("Enter channel number (1-512): ").strip()
                value = input("Enter value (0-255): ").strip()
                try:
                    channel = int(channel)
                    value = int(value)
                    simulator.set_channel(channel, value)
                    simulator.send_dmx_frame()
                    print(f"Set channel {channel} to {value}")
                except ValueError:
                    print("Invalid input")
            elif choice == '5':
                break
            else:
                print("Invalid choice")
    
    except KeyboardInterrupt:
        print("\nTest interrupted by user")
    
    finally:
        simulator.disconnect()

if __name__ == "__main__":
    main()