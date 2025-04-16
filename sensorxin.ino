/*
 * Code Arduino Sensor cho Robot Sumo - Đã tối ưu
 * Thêm chiến thuật tông-lùi-tông và zig-zag
 */

// Định nghĩa chân cảm biến siêu âm
const int trigPin = 10;
const int echoPin = 9;

// Định nghĩa chân cảm biến dò line
#define SENSOR_LEFT 2   // Cảm biến line bên TRÁI
#define SENSOR_RIGHT 3  // Cảm biến line bên PHẢI

// Thông số cấu hình
const int SEARCH_DISTANCE = 30;  // Khoảng cách tìm kiếm (cm)
const int MAX_DISTANCE = 200;    // Khoảng cách tối đa hợp lệ (cm)
const int CLOSE_ATTACK_DISTANCE = 15; // Khoảng cách rất gần để thực hiện tông-lùi

// Định nghĩa command codes
#define CMD_FORWARD 'F'      // Tiến
#define CMD_BACKWARD 'B'     // Lùi
#define CMD_ROTATE_LEFT 'L'  // Quay trái
#define CMD_ROTATE_RIGHT 'R' // Quay phải
#define CMD_STOP 'S'         // Dừng
#define CMD_RAM_ATTACK 'X'   // Tông mạnh (chiến thuật tông-lùi-tông)
#define CMD_ZIGZAG_LEFT 'Z'  // Zig-zag sang trái
#define CMD_ZIGZAG_RIGHT 'Y' // Zig-zag sang phải

// Biến trạng thái
bool leftLineDetected = false;  // Phát hiện line bên trái
bool rightLineDetected = false; // Phát hiện line bên phải
bool searching = true;          // Đang tìm kiếm đối thủ
unsigned long lastChangeTime = 0; // Thời điểm đổi hướng quay cuối cùng
bool rotateDirection = true;    // true: quay trái, false: quay phải
char lastSentCommand = 0;       // Lệnh cuối cùng đã gửi
unsigned long lastSendTime = 0; // Thời gian gửi lệnh cuối cùng
bool inRamAttackMode = false;   // Đang trong chế độ tông-lùi
unsigned long ramAttackStartTime = 0; // Thời gian bắt đầu tông-lùi
int ramAttackPhase = 0;         // Giai đoạn của tông-lùi (0: tông, 1: lùi, 2: tông)
bool inZigzagMode = false;      // Đang trong chế độ zig-zag
unsigned long zigzagStartTime = 0; // Thời gian bắt đầu zig-zag
bool zigzagDirection = true;    // Hướng zig-zag (true: trái, false: phải)

// Khai báo trước các hàm để tránh lỗi
void sendCommand(char command);
void handleZigzagStrategy();

// Đọc khoảng cách từ cảm biến siêu âm (cm)
long readDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 15000); // Timeout 15ms
  
  if (duration == 0) {
    return MAX_DISTANCE;
  }
  
  long distance = duration * 0.034 / 2;
  if (distance > MAX_DISTANCE || distance <= 0) {
    return MAX_DISTANCE;
  }
  
  return distance;
}

// Đọc giá trị từ cảm biến dò line
void readLineSensors() {
  int leftValue = digitalRead(SENSOR_LEFT);
  int rightValue = digitalRead(SENSOR_RIGHT);
  
  // Với sàn đấu màu đen và viền trắng:
  // - LOW = viền trắng
  // - HIGH = sàn đen
  leftLineDetected = (leftValue == LOW);   // Phát hiện viền trắng bên trái
  rightLineDetected = (rightValue == LOW); // Phát hiện viền trắng bên phải
}

// Gửi lệnh đến Arduino Motor qua UART
void sendCommand(char command) {
  // Gửi lệnh nhiều lần để đảm bảo nhận được
  for (int i = 0; i < 5; i++) {
    Serial.write(command);
    delay(2); // Đợi một chút giữa các lần gửi
  }
  
  lastSentCommand = command;
  lastSendTime = millis();
}

// Xử lý chiến thuật zig-zag
void handleZigzagStrategy() {
  // Bắt đầu chế độ zig-zag
  if (!inZigzagMode) {
    inZigzagMode = true;
    zigzagStartTime = millis();
    zigzagDirection = true; // Bắt đầu với hướng sang trái
  }
  
  // Đổi hướng zig-zag mỗi 300ms
  if (millis() - zigzagStartTime > 300) {
    zigzagDirection = !zigzagDirection;
    zigzagStartTime = millis();
  }
  
  // Gửi lệnh zig-zag phù hợp
  if (zigzagDirection) {
    sendCommand(CMD_ZIGZAG_LEFT);
  } else {
    sendCommand(CMD_ZIGZAG_RIGHT);
  }
}

// Xử lý chiến thuật tông-lùi-tông
void handleRamAttackStrategy(long distance) {
  // Nếu không trong chế độ tông-lùi và đối thủ rất gần, bắt đầu chiến thuật
  if (!inRamAttackMode && distance < CLOSE_ATTACK_DISTANCE) {
    inRamAttackMode = true;
    ramAttackStartTime = millis();
    ramAttackPhase = 0;
  }
  
  // Nếu đang trong chế độ tông-lùi, thực hiện các bước
  if (inRamAttackMode) {
    unsigned long currentTime = millis();
    unsigned long elapsed = currentTime - ramAttackStartTime;
    
    if (ramAttackPhase == 0 && elapsed < 500) {
      // Giai đoạn 1: Tông mạnh trong 500ms
      sendCommand(CMD_RAM_ATTACK);
    }
    else if (ramAttackPhase == 0) {
      // Chuyển sang giai đoạn lùi
      ramAttackPhase = 1;
      ramAttackStartTime = currentTime;
    }
    else if (ramAttackPhase == 1 && elapsed < 300) {
      // Giai đoạn 2: Lùi trong 300ms
      sendCommand(CMD_BACKWARD);
    }
    else if (ramAttackPhase == 1) {
      // Chuyển sang giai đoạn tông lại
      ramAttackPhase = 2;
      ramAttackStartTime = currentTime;
    }
    else if (ramAttackPhase == 2 && elapsed < 800) {
      // Giai đoạn 3: Tông mạnh trong 800ms
      sendCommand(CMD_RAM_ATTACK);
    }
    else {
      // Kết thúc chiến thuật, quay lại chế độ bình thường
      inRamAttackMode = false;
    }
    
    // Đang trong chế độ tông-lùi, không thực hiện các chiến thuật khác
    return;
  }
}

// Cải tiến xử lý phát hiện line trắng
bool handleLineDetection() {
  if (leftLineDetected || rightLineDetected) {
    if (leftLineDetected && rightLineDetected) {
      // Cả hai cảm biến cùng phát hiện viền trắng
      // => Tăng tốc tối đa về phía trước để ra khỏi viền
      sendCommand(CMD_FORWARD);
    }
    else if (leftLineDetected) {
      // Chỉ cảm biến trái phát hiện viền trắng
      // => Lùi và quay về phía trái
      sendCommand(CMD_BACKWARD);
      delay(200);
      sendCommand(CMD_ROTATE_LEFT);
      delay(300);
      // Cố gắng phát hiện khi cả hai cảm biến thấy viền
      readLineSensors();
      if (leftLineDetected && rightLineDetected) {
        sendCommand(CMD_FORWARD);
      }
    }
    else if (rightLineDetected) {
      // Chỉ cảm biến phải phát hiện viền trắng
      // => Lùi và quay về phía phải
      sendCommand(CMD_BACKWARD);
      delay(200);
      sendCommand(CMD_ROTATE_RIGHT);
      delay(300);
      // Cố gắng phát hiện khi cả hai cảm biến thấy viền
      readLineSensors();
      if (leftLineDetected && rightLineDetected) {
        sendCommand(CMD_FORWARD);
      }
    }
    
    // Đã xử lý line, không thực hiện các chiến thuật khác
    return true;
  }
  
  return false;
}

void setup() {
  // Khởi tạo giao tiếp UART với baud rate cao
  Serial.begin(115200);
  
  // Cài đặt chân siêu âm
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // Cài đặt chân cảm biến dò line
  pinMode(SENSOR_LEFT, INPUT);
  pinMode(SENSOR_RIGHT, INPUT);
  
  // Đợi mọi thứ ổn định và gửi lệnh dừng ban đầu
  delay(1000);
  sendCommand(CMD_STOP);
  
  // Bắt đầu với chế độ zig-zag
  handleZigzagStrategy();
}

void loop() {
  // 1. Đọc cảm biến dò line (ưu tiên cao nhất)
  readLineSensors();
  
  // 2. Xử lý phát hiện line trắng (viền sàn đấu)
  if (handleLineDetection()) {
    // Đã xử lý line, bỏ qua các chiến thuật khác
    return;
  }
  
  // 3. Đọc khoảng cách và xử lý chiến thuật
  long distance = readDistance();
  
  // 4. Xử lý chiến thuật tông-lùi-tông nếu đối thủ rất gần
  if (distance < CLOSE_ATTACK_DISTANCE) {
    handleRamAttackStrategy(distance);
    return;
  }
  
  // 5. Xử lý các chiến thuật khác
  if (distance < SEARCH_DISTANCE) {
    // Đối thủ trong tầm tấn công nhưng không quá gần
    searching = false;
    inZigzagMode = false; // Thoát chế độ zig-zag
    sendCommand(CMD_FORWARD);
  }
  else {
    // Không phát hiện đối thủ - thực hiện chiến thuật zig-zag
    searching = true;
    handleZigzagStrategy();
  }
  
  // Gửi lại lệnh hiện tại mỗi 300ms để đảm bảo robot không bị mất lệnh
  if (millis() - lastSendTime > 300) {
    sendCommand(lastSentCommand);
  }
  
  // Delay ngắn để tối ưu
  delay(20);
}