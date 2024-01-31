# Arduino LED Controller

This Arduino-based project allows you to control a 20-LED strip using a HC-05 Bluetooth module. It is designed to interface with the [ArduinoLedApp](https://github.com/stingsek/arduinoLedApp), an Android application developed in Jetpack Compose (Kotlin), for a seamless and interactive user experience.

## Features

- **Color Control**: Set the color of the LED strip through the app.
- **Brightness Adjustment**: Modify the brightness of the LEDs.
- **Light Effects**: Choose from various effects like blinking, smooth transitions, rainbow, pulse, etc.
- **Bluetooth Connectivity**: Uses a HC-05 module for easy connection with the Android app.

## Hardware Requirements

- Arduino (Uno, Mega, etc.)
- HC-05 Bluetooth module
- RGB LED strip (20 LEDs)
- Some wires and suitable power supply
- Using a capacitor between led strip and the power supply would be recommended

## Software Setup
   - Upload the provided code to your Arduino using for example the Arduino IDE.
   - Install the `ArduinoLedApp` on your Android device.

## Usage

1. Power on the Arduino setup.
2. Open the `ArduinoLedApp` on your Android device.
3. Connect to the Arduino via Bluetooth within the app.
4. Control the LED strip using the app's interface.

## Code Overview

The main components of the code include:

- **Adafruit NeoPixel Library**: For controlling the LED strip.
- **Bluetooth Data Handling**: To receive commands from the Android app.
- **LED Control Functions**: To change colors, brightness, and activate different light effects.

## Troubleshooting

- Ensure the LED strip and HC-05 are correctly connected to the Arduino.
- Check if the Bluetooth module is paired with your Android device. The Bluetooth module should blink twice with a few seconds delay.
- Confirm that the Arduino is powered and the code is uploaded correctly.
- Ensure that all components are supplied with the voltage and power specifications as recommended by their manufacturers.

