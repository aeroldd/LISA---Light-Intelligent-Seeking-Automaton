# L.I.S.A – Light-Intelligent Sensing Arduino Robot

A light-following robot designed to detect light intensity using multiple LDRs (Light-Dependent Resistors) and adjust movement direction and speed accordingly.

Project overview

L.I.S.A is an Arduino-based mobile robot that:
- Uses **4 LDR sensors** (right, center, left, rear) to detect light levels.
- Calculates **light difference errors** to determine turning direction and speed adjustments.
- Controls **two motors** through a DRV8833 motor driver.
- Is capable of **smooth directional changes** and speed regulation depending on light gradients.

## 📡 Sensors and Components

| Component        | Quantity | Purpose                                                                 |
|------------------|----------|-------------------------------------------------------------------------|
| Arduino Uno      | 1        | Main controller                                                         |
| LDR (Photoresistor) | 5        | Detects light from various directions (left, center, right, rear)       |
| 4.7kΩ Resistors  | 5        | Pull-down resistors for LDR voltage divider circuits                    |
| DRV8833 Motor Driver | 1        | Drives the two DC motors                                               |
| DC Motors        | 2        | Movement and turning                                                    |
| Chassis          | 1        | Physical frame                                                          |
| 5V Power Source  | 1        | Power supply to Arduino and sensors                                    |

Sensor Characterization

LDR readings were collected at various distances from a light source to analyze response curves and set thresholds.

### Example Table:

| Distance (cm) | Right | Center | Left | Rear |
|---------------|--------|--------|------|------|
| 3             | 90     | 94     | 82   | 90   |
| 7             | 240    | 163    | 170  | 170  |
| 11            | 362    | 260    | 260  | 260  |
| 15            | 450    | 350    | 320  | 350  |
| 19            | 550    | 420    | 383  | 400  |

A multi-series scatter plot with trendlines was used to visualize sensor behavior and normalize readings.

Light following at a glance

- The Arduino reads analog values from each LDR.
- Based on **differences in light intensity**, the robot computes an **error value**.
- This error determines:
  - The **direction** to turn (left/right).
  - The **motor speed adjustment** (slow down for smoother turns).
- The center LDR value also affects speed — brighter light ahead slows down the robot for more precision.
## 🛠️ Setup Instructions

1. Wire LDR sensors in voltage divider configuration (LDR + 4.7kΩ resistor to GND).
2. Connect analog pins A0–A4 to the LDR outputs.
3. Connect motor driver inputs to Arduino digital pins.
4. Upload Arduino sketch and test with a flashlight or moving light source.

## 📝 Future Improvements

- PID control for smoother response.
- OLED display for live light readings.
- Wireless debugging or remote control.
- Obstacle avoidance in combination with light following.

---

**Created by:** Aerold Parcon  
**Institution:** Heriot-Watt University, Dubai  
**Date:** April 2025
