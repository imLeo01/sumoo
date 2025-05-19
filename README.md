🥋 Sumo Robot (Dual Arduino - Sensor & Motor Control)
This project is a Sumo Robot built using two Arduino boards that communicate over UART. One Arduino handles sensor input and decision-making, while the other controls the motors and movement based on received commands. The robot is designed to detect opponents and avoid falling off the edge of the arena.

📦 System Overview
Sensor Arduino: Detects opponents using an ultrasonic sensor and arena edges using line sensors. It makes decisions and sends commands to the Motor Arduino.

Motor Arduino: Receives commands from the Sensor Arduino and controls the motors using an L293D H-bridge.

🔌 Hardware Components
Microcontrollers
2 × Arduino Uno (or compatible)

Sensors
1 × Ultrasonic sensor (HC-SR04)

3 × Infrared line sensors (left, center, right)

Actuators & Power
2 or 4 DC motors

1 × L293D Motor Driver

Power switch (KCD1)

Power source (battery pack)

🔄 UART Communication Protocol
The Sensor Arduino sends single-character commands over Serial to the Motor Arduino. Each command represents a specific movement:

Command	Action
F	Move Forward
B	Move Backward
L	Rotate Left
R	Rotate Right
S	Stop
A	Ram Attack (Fast Fwd)

🧠 Behavior Logic (Sensor Arduino)
Line Detection:

If a line is detected (edge of arena), the robot backs away and rotates to stay inside.

Opponent Detection:

If the opponent is within range, the robot charges forward (ram attack).

If no opponent is detected, the robot rotates left/right alternately to search.

Command Resending:

Commands are periodically resent to ensure consistent motor behavior.

🔧 How to Use
Upload the Sensor Code to one Arduino.

Upload the Motor Code to the second Arduino.

Connect Serial (TX/RX) between the two boards (cross-wired: TX → RX and RX → TX).

Wire Sensors and Motors properly to each Arduino.

Power on using the KCD1 switch and place the robot in the arena.

📂 Code Structure
sensor_arduino.ino – Main logic for decision-making and UART communication.

motor_arduino.ino – Handles motor control based on commands.

📌 Notes
Ensure both Arduinos use the same baud rate.

TX and RX lines must be cross-connected (TX to RX).

Add capacitors and proper grounding for stable performance.

Avoid using Serial Monitor while using UART for inter-Arduino communication.

🤖 Future Improvements
Add a strategy mode switch (aggressive vs defensive).

Use PID control for precise motor handling.

Implement EEPROM or remote control override for behavior switching.# sumoo
