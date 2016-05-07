# Ableton-Session-Controller-v1.0
Arduino and Chuck programs for a customisable hardware MIDI controller

This code runs on an Arduino Mega in a hardware unit I built consisting of potentiometers and buttons.

The Arduino code sets up the inputs and outputs and detects changes made to the analog inputs (buttons, pots)

It continuously prints the values to serial, in a format specified in the Chuck code.

The Chuck program picks up the data from serial and compares it to the previous values received. If values have changed a MIDI message is created
and sent through a virtual MIDI port to be received in Ableton (or any other MIDI receiving program)

Ableton has a simple MIDI map function to assign controls from the hardware to parameters in the software.

The code is simple to customise to allow more or less controls on the hardware. 

I plan to remove the chuck program and output the MIDI signal from Arduino at some stage.
