# Stop Watch System
## Overview
This project implements a Stop Watch system using the ATmega32 Microcontroller with a 1MHz frequency. It utilizes Timer1 in CTC mode for precise time counting and features a display system using six Common Anode 7-segments through multiplexing with a 7447 decoder and NPN BJT transistors.

## Hardware Setup
- Connect the 7447 decoder to the first 4 pins in PORTC.
- Use the first 6 pins in PORTA as enable/disable pins for the six 7-segments.
- Implement multiplexing technique for displaying time on the 7-segment displays.

## Stopwatch Controls
- Start the stopwatch automatically upon connecting power to the MCU.
- Connect a push button with an internal pull-up resistor to trigger External Interrupt INT0 (falling edge) for resetting the stopwatch.
- Connect a push button with an external pull-down resistor to trigger External Interrupt INT1 (rising edge) for pausing the stopwatch.
- Connect a push button with an internal pull-up resistor to trigger External Interrupt INT2 (falling edge) for resuming the stopwatch.

## Timer Configuration
Configure Timer1 in CTC mode for accurate timekeeping.

## Notes
- The project is designed for the ATmega32 Microcontroller with a 1MHz frequency.
- Utilize the provided circuit diagram for proper connection of 7-segments and control elements.
