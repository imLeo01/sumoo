# 🥋 **Sumo Robot (Dual Arduino - Sensor & Motor Control)**

This project is a **Sumo Robot** built using two Arduino boards that communicate over **UART**.  
One Arduino handles **sensor input and decision-making**, while the other controls the **motors**.  
The robot is designed to detect opponents and avoid the edge of the arena.

---

## 📦 **System Overview**

- **Sensor Arduino**:  
  Detects opponents using ultrasonic sensor and arena edges using IR sensors.  
  Makes decisions and sends commands via Serial to the Motor Arduino.

- **Motor Arduino**:  
  Receives commands via UART and controls the motors using an L293D H-bridge.

---

## 🔌 **Hardware Components**

### 🧠 Microcontrollers
- 2 × Arduino Uno (or compatible)

### 👀 Sensors
- 1 × Ultrasonic sensor (HC-SR04)  
- 3 × Infrared line sensors (left, center, right)

### ⚙️ Actuators & Power
- 2 or 4 × DC motors  
- 1 × L293D Motor Driver  
- 1 × Power switch (KCD1)  
- 1 × Battery pack (7.4V or similar)

---

## 🔄 **UART Communication Protocol**

Sensor Arduino sends **single-character commands** to Motor Arduino over UART:

| **Command** | **Action**            |
|-------------|------------------------|
| `F`         | Move Forward           |
| `B`         | Move Backward          |
| `L`         | Rotate Left            |
| `R`         | Rotate Right           |
| `S`         | Stop                   |
| `A`         | Ram Attack (Fast Fwd)  |

---

## 🧠 **Behavior Logic (Sensor Arduino)**

1. **Line Detection**  
   - If a line is detected (edge), the robot stops, backs away, and rotates.

2. **Opponent Detection**  
   - If opponent is detected nearby → `A` (Ram Attack)  
   - If no opponent → rotate to search (alternate `L` and `R`)

3. **Command Resending**  
   - Same command is resent periodically to maintain motor behavior

---

## 🔧 **How to Use**

1. Upload the **sensor code** to one Arduino.
2. Upload the **motor control code** to the second Arduino.
3. Connect **TX → RX** and **RX → TX** between the boards.
4. Wire sensors to the Sensor Arduino, motors to the Motor Arduino.
5. Power on using the **KCD1 switch**, and place the robot in the arena.

---

## 📂 **Code Structure**

- `sensor_arduino.ino` – Logic and UART communication  
- `motor_arduino.ino` – Handles motor commands

---

## ⚠️ **Notes**

- Both Arduinos must use the **same baud rate** (e.g., 9600).
- TX and RX must be cross-connected.
- Do **not** use the Serial Monitor when UART is in use.
- Ensure proper **grounding and power regulation**.

---

## 🚀 **Future Improvements**

- Add strategy selector (aggressive/defensive).
- Implement PID for smoother motor control.
- Add EEPROM save/load or remote override.

---
