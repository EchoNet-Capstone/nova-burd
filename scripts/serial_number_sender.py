#!/usr/bin/env python3
"""
Serial Number Oneshot Sender
A utility to send a single serial number to a device over serial connection.
"""

import argparse
import sys
import time
import serial
import os

def send_serial_number(port, baud_rate, timeout, serial_number, wait_for_ready, wait_for_response):
    """Send a serial number to the specified port and optionally wait for response."""
    try:
        # Open serial connection
        with serial.Serial(port, baud_rate, timeout=timeout) as ser:
            # Wait for port to be ready
            time.sleep(1)
            
            # Wait for device to be ready if requested
            if wait_for_ready:
                print("Waiting for device to be ready...")
                start_time = time.time()
                ready_found = False
                
                while time.time() - start_time < timeout and not ready_found:
                    if ser.in_waiting:
                        response = ser.readline().decode().strip()
                        print(f"Device: {response}")
                        if "waiting for" in response.lower() and "serial number" in response.lower():
                            print("Device is ready")
                            ready_found = True
                    time.sleep(0.1)
                
                if not ready_found:
                    print("Warning: Device ready signal not detected")
            
            # Send the serial number followed by a newline
            ser.write(f"{serial_number}\n".encode())
            
            print(f"Sent serial number: {serial_number}")
            
            # Optionally wait for a response
            if wait_for_response:
                print("Waiting for confirmation...")
                start_time = time.time()
                while time.time() - start_time < timeout:
                    if ser.in_waiting:
                        response = ser.readline().decode().strip()
                        print(f"Received: {response}")
                        if "confirmed" in response.lower() or "success" in response.lower():
                            print("Success confirmation received")
                            return True
                    time.sleep(0.1)
                print("Warning: No confirmation received within timeout period")
            return True
            
    except serial.SerialException as e:
        print(f"Error: Failed to open serial port: {e}", file=sys.stderr)
        return False
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        return False

def get_serial_number():
    """Get serial number from stdin or prompt user if stdin is a terminal."""
    # Check if stdin is a terminal (interactive) or being piped/redirected
    if os.isatty(sys.stdin.fileno()):
        # Interactive mode - prompt user
        try:
            serial_number = input("Enter serial number: ").strip()
            return serial_number
        except (KeyboardInterrupt, EOFError):
            print("\nInput cancelled by user")
            sys.exit(1)
    else:
        # Non-interactive mode - read from stdin (pipe or redirection)
        serial_number = sys.stdin.readline().strip()
        return serial_number

def main():
    parser = argparse.ArgumentParser(description='Send a serial number to a device')
    parser.add_argument('-p', '--port', required=True, help='Serial port to use')
    parser.add_argument('-b', '--baud', type=int, default=115200, help='Baud rate (default: 115200)')
    parser.add_argument('-t', '--timeout', type=float, default=10.0, help='Timeout in seconds (default: 10.0)')
    parser.add_argument('-w', '--wait', action='store_true', help='Wait for confirmation response')
    parser.add_argument('-r', '--ready', action='store_true', help='Wait for device ready signal before sending')
    
    args = parser.parse_args()
    
    # Get serial number from stdin or by prompting user
    serial_number = get_serial_number()
    
    if not serial_number:
        print("Error: No serial number provided", file=sys.stderr)
        sys.exit(1)
    
    success = send_serial_number(
        args.port, 
        args.baud, 
        args.timeout, 
        serial_number, 
        args.ready,  # Wait for ready signal
        args.wait    # Wait for confirmation
    )
    
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()