# BluePill adalight controller

This is a project for driving RGB LEDs using the adalight protocol (for use with [OpenRGB](https://openrgb.org/) ) and the FastLED library. It runs on a Blue Pill microcontroller and is built with PlatformIO.

## Features

- Built with PlatformIO.
- Implements the adalight protocol for controlling RGB LED strips.
- Uses the FastLED library.
- Runs on a Blue Pill microcontroller.
- Uses built-in USB port on the BluePill.

## Requirements

### Hardware

- Blue Pill Board (STM32F103C8)
- RGB LED Strip (WS2812, WS2811, or similar)

### Software

- PlatformIO ([installation guide](https://docs.platformio.org/en/latest/core/installation.html))
- FastLED Library (installed via PlatformIO automatically)

## Wiring

| Blue Pill Pin | RGB LED Strip | Notes              |
| ------------- | ------------- | ------------------ |
| PA7           | Data In       | Signal line to LED |
| GND           | GND           | Common ground      |
| 5V            | VCC           | Power for the LEDs |

## Customization

The project uses three constants that you can adjust to customize the behavior of the LED strip:

- `DATA_PIN` (default: `PA7`): Defines the pin used to send data to the LED strip.
- `NUM_LEDS` (default: `128`): Defines the number of LEDs in the strip.
- `BRIGHTNESS` (default: `255`): Sets the global brightness level of the LEDs (0-255).

You can modify these constants in `src/main.cpp` to suit your hardware or preferences.

## Credits and Contributions

This project is based on [Adalight-FastLED](https://github.com/dmadison/Adalight-FastLED) by Dave Madison.

## License

Bluepill-adalight is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

Bluepill-adalight is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with bluepill-adalight. If not, see <http://www.gnu.org/licenses/>.