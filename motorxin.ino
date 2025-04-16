/*
 * Code Arduino Motor cho Robot Sumo - Đã tối ưu
 * Thêm chiến thuật tông-lùi-tông và zig-zag
 */

// H-bridge 1 pins (Left side motors)
// Motor A (Left motor #1)
const int H1_A_EN = 6;   // PWM speed control for motor A
const int H1_A_IN1 = 7;  // Direction pin 1 for motor A
const int H1_A_IN2 = 5;  // Direction pin 2 for motor A

// Motor B (Left motor #2)
const int H1_B_EN = 3;   // PWM speed control for motor B
const int H1_B_IN3 = 4;  // Direction pin 1 for motor B
const int H1_B_IN4 = 2;  // Direction pin 2 for motor B

// H-bridge 2 pins (Right side motors)
// Motor C (Right motor #1)
const int H2_C_EN = 9;   // PWM speed control for motor C
const int H2_C_IN1 = 8;  // Direction pin 1 for motor C
const int H2_C_IN2 = 10; // Direction pin 2 for motor C

// Motor D (Right motor #2)
const int H2_D_EN = 11;   // PWM speed control for motor D
const int H2_D_IN3 = 12;  // Direction pin 1 for motor D
const int H2_D_IN4 = 13;  // Direction pin 2 for motor D

// Định nghĩa command codes
#define CMD_FORWARD 'F'      // Tiến
#define CMD_BACKWARD 'B'     // Lùi
#define CMD_ROTATE_LEFT 'L'  // Quay trái
#define CMD_ROTATE_RIGHT 'R' // Quay phải
#define CMD_STOP 'S'         // Dừng
#define CMD_RAM_ATTACK 'X'   // Tông mạnh (chiến thuật tông-lùi-tông)
#define CMD_ZIGZAG_LEFT 'Z'  // Zig-zag sang trái
#define CMD_ZIGZAG_RIGHT 'Y' // Zig-zag sang phải

// Biến lưu tốc độ
const int NORMAL_SPEED = 200;     // Tốc độ bình thường (0-255)
const int ATTACK_SPEED = 255;     // Tốc độ tấn công tối đa
const int ROTATION_SPEED = 180;   // Tốc độ quay tìm kiếm
const int RAM_SPEED = 255;        // Tốc độ tông mạnh (tối đa)
const int ZIGZAG_SPEED = 220;     // Tốc độ zig-zag

// Biến lưu trạng thái hiện tại
char currentCommand = CMD_STOP;   // Lệnh hiện tại đang thực hiện

// Điều khiển động cơ bên trái
void setLeftMotors(int speed) {
  int absSpeed = abs(speed);
  absSpeed = constrain(absSpeed, 0, 255);
  
  // Motor A (Left motor #1)
  digitalWrite(H1_A_IN1, speed > 0 ? HIGH : LOW);
  digitalWrite(H1_A_IN2, speed <= 0 ? HIGH : LOW);
  analogWrite(H1_A_EN, absSpeed);
  
  // Motor B (Left motor #2)
  digitalWrite(H1_B_IN3, speed > 0 ? HIGH : LOW);
  digitalWrite(H1_B_IN4, speed <= 0 ? HIGH : LOW);
  analogWrite(H1_B_EN, absSpeed);
}

// Điều khiển động cơ bên phải
void setRightMotors(int speed) {
  int absSpeed = abs(speed);
  absSpeed = constrain(absSpeed, 0, 255);
  
  // Motor C (Right motor #1)
  digitalWrite(H2_C_IN1, speed > 0 ? HIGH : LOW);
  digitalWrite(H2_C_IN2, speed <= 0 ? HIGH : LOW);
  analogWrite(H2_C_EN, absSpeed);
  
  // Motor D (Right motor #2)
  digitalWrite(H2_D_IN3, speed > 0 ? HIGH : LOW);
  digitalWrite(H2_D_IN4, speed <= 0 ? HIGH : LOW);
  analogWrite(H2_D_EN, absSpeed);
}

// Di chuyển tiến
void driveForward(int speed) {
  setLeftMotors(speed);
  setRightMotors(speed);
}

// Di chuyển lùi
void driveBackward(int speed) {
  setLeftMotors(-speed);
  setRightMotors(-speed);
}

// Quay trái
void rotateLeft(int speed) {
  setLeftMotors(-speed);
  setRightMotors(speed);
}

// Quay phải
void rotateRight(int speed) {
  setLeftMotors(speed);
  setRightMotors(-speed);
}

// Dừng động cơ
void stopMotors() {
  setLeftMotors(0);
  setRightMotors(0);
}

// Thực hiện di chuyển zig-zag sang trái
void zigzagLeft() {
  setLeftMotors(ZIGZAG_SPEED * 0.4);  // Giảm tốc bên trái
  setRightMotors(ZIGZAG_SPEED);       // Tốc độ đầy đủ bên phải
}

// Thực hiện di chuyển zig-zag sang phải
void zigzagRight() {
  setLeftMotors(ZIGZAG_SPEED);        // Tốc độ đầy đủ bên trái
  setRightMotors(ZIGZAG_SPEED * 0.4); // Giảm tốc bên phải
}

// Thực hiện tông mạnh (tốc độ tối đa)
void ramAttack() {
  driveForward(RAM_SPEED);
}

// Xử lý lệnh nhận được
void handleCommand(char command) {
  // Cập nhật lệnh hiện tại
  currentCommand = command;
  
  switch (command) {
    case CMD_FORWARD:
      driveForward(ATTACK_SPEED);
      break;
      
    case CMD_BACKWARD:
      driveBackward(NORMAL_SPEED);
      break;
      
    case CMD_ROTATE_LEFT:
      rotateLeft(ROTATION_SPEED);
      break;
      
    case CMD_ROTATE_RIGHT:
      rotateRight(ROTATION_SPEED);
      break;
      
    case CMD_STOP:
      stopMotors();
      break;
      
    case CMD_RAM_ATTACK:
      ramAttack();
      break;
      
    case CMD_ZIGZAG_LEFT:
      zigzagLeft();
      break;
      
    case CMD_ZIGZAG_RIGHT:
      zigzagRight();
      break;
      
    default:
      // Giữ nguyên trạng thái hiện tại nếu lệnh không hợp lệ
      break;
  }
}

void setup() {
  // Khởi tạo giao tiếp UART với baud rate cao
  Serial.begin(115200);
  
  // Khởi tạo chân cho H-bridge 1 (động cơ bên trái)
  pinMode(H1_A_EN, OUTPUT);
  pinMode(H1_A_IN1, OUTPUT);
  pinMode(H1_A_IN2, OUTPUT);
  
  pinMode(H1_B_EN, OUTPUT);
  pinMode(H1_B_IN3, OUTPUT);
  pinMode(H1_B_IN4, OUTPUT);
  
  // Khởi tạo chân cho H-bridge 2 (động cơ bên phải)
  pinMode(H2_C_EN, OUTPUT);
  pinMode(H2_C_IN1, OUTPUT);
  pinMode(H2_C_IN2, OUTPUT);
  
  pinMode(H2_D_EN, OUTPUT);
  pinMode(H2_D_IN3, OUTPUT);
  pinMode(H2_D_IN4, OUTPUT);
  
  // Dừng động cơ khi khởi động
  stopMotors();
  
  // Kiểm tra động cơ - nhấp nháy để xác nhận
  driveForward(100);
  delay(200);
  stopMotors();
  delay(200);
  driveForward(100);
  delay(200);
  stopMotors();
}

void loop() {
  // Kiểm tra xem có lệnh mới từ Arduino Sensor không
  while (Serial.available()) {
    char command = Serial.read();
    handleCommand(command);
  }
  
  // Không có timeout - robot sẽ tiếp tục thực hiện lệnh cuối cùng nhận được
  
  // Delay ngắn để giảm tải CPU
  delay(5);
}