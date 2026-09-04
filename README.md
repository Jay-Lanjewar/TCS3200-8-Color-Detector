# TCS3200 8-Color Detector

An Arduino UNO based color detection system using a TCS3200 color sensor and a servo motor.

The system detects eight different colors and moves the servo to a predefined position corresponding to the detected color.

## Colors Detected

- Black
- White
- Red
- Green
- Blue
- Yellow
- Purple
- Orange

## Hardware Used

- Arduino UNO
- TCS3200 Color Sensor
- Servo Motor
- Color palette / color wheel
- Jumper wires
- Cardboard mounting structure

## Pin Connections

| TCS3200 | Arduino UNO |
|---------|-------------|
| S0      | D4          |
| S1      | D5          |
| S2      | D6          |
| S3      | D7          |
| OUT     | D8          |
| VCC     | 5V          |
| GND     | GND         |

**Servo signal → D9**

## How It Works

The TCS3200 color sensor uses photodiodes and different color filters to measure the intensity of red, green, and blue light.

The sensor produces a frequency output based on the intensity of the selected color. The Arduino measures the output pulse duration using `pulseIn()` and obtains readings for the red, green, and blue channels.

Instead of relying on fixed color thresholds, this project uses calibration profiles for each color. The Arduino compares a new sensor reading with the stored profiles and identifies the closest calibrated color.

The servo then moves to the predefined angle for that color.

## Automatic Calibration

The project includes an EEPROM-based calibration system.

During calibration:

1. A color is placed in front of the TCS3200.
2. The Arduino takes multiple sensor readings.
3. The readings are averaged to create a color profile.
4. The profile is stored in the Arduino UNO's EEPROM.
5. The process is repeated for all eight colors.

Because the calibration data is stored in EEPROM, it remains available even after the Arduino is powered off.

Calibration should be performed using the final physical setup because sensor distance, lighting, surface material, and positioning can affect the readings.

## Calibration Commands

The Serial Monitor can be used to control calibration:

| Command | Function |
|---------|----------|
| `C` | Start calibration |
| `S` | Show stored calibration |
| `R` | Reset stored calibration |

The Serial Monitor should be set to **9600 baud**.

## Servo Positions

The servo uses predefined positions for the detected colors:

| Color | Servo Angle |
|-------|-------------|
| Black | 170° |
| White | 145° |
| Red | 125° |
| Green | 105° |
| Blue | 80° |
| Yellow | 60° |
| Purple | 35° |
| Orange | 10° |

## Project Status

**Working prototype successfully tested on physical hardware.**

The system was calibrated and tested with all eight colors using the physical setup.

## Demo

A demonstration video of the working prototype can be linked here.

_Add the YouTube video link once it is uploaded._

## Author

**Jay Lanjewar**
