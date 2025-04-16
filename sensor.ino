/*
 * Code Arduino Sensor cho Robot Sumo
 * Sử dụng cổng TX/RX phần cứng để giao tiếp
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

// Định nghĩa command codes
#define CMD_FORWARD 'F'      // Tiến
#define CMD_BACKWARD 'B'     // Lùi
#define CMD_ROTATE_LEFT 'L'  // Quay trái
#define CMD_ROTATE_RIGHT 'R' // Quay phải
#define CMD_STOP 'S'         // Dừng

// Biến trạng thái
bool leftLineDetected = false;  // Phát hiện line bên trái
bool rightLineDetected = false; // Phát hiện line bên phải
bool searching = true;          // Đang tìm kiếm đối thủ
unsigned long lastChangeTime = 0; // Thời điểm đổi hướng quay cuối cùng
bool rotateDirection = true;    // true: quay trái, false: quay phải
char lastSentCommand = 0;       // Lệnh cuối cùng đã gửi
unsigned long lastSendTime = 0; // Thời gian gửi lệnh cuối cùng

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
    delay(5); // Đợi một chút giữa các lần gửi
  }
  
  lastSentCommand = command;
  lastSendTime = millis();
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
}

void loop() {
  // 1. Đọc cảm biến dò line (ưu tiên cao nhất)
  readLineSensors();
  
  // 2. Xử lý phát hiện line trắng (viền sàn đấu)
  if (leftLineDetected || rightLineDetected) {
    if (leftLineDetected && rightLineDetected) {
      // Cả hai cảm biến cùng phát hiện viền trắng
      // => Tăng tốc tối đa về phía trước để tránh rơi khỏi sàn
      sendCommand(CMD_FORWARD);
    }
    else if (leftLineDetected) {
      // Chỉ cảm biến trái phát hiện viền trắng
      // => Rẽ nhẹ sang trái để cả hai cảm biến cùng phát hiện
      sendCommand(CMD_ROTATE_LEFT);
    }
    else if (rightLineDetected) {
      // Chỉ cảm biến phải phát hiện viền trắng
      // => Rẽ nhẹ sang phải để cả hai cảm biến cùng phát hiện
      sendCommand(CMD_ROTATE_RIGHT);
    }
  }
  else {
    // 3. Không phát hiện line - tìm kiếm hoặc tấn công đối thủ
    long distance = readDistance();
    
    if (distance < SEARCH_DISTANCE) {
      // Đối thủ trong tầm tấn công
      searching = false;
      sendCommand(CMD_FORWARD);
    }
    else {
      // Không phát hiện đối thủ - tìm kiếm
      searching = true;
      
      // Chuyển đổi hướng quay mỗi 2 giây
      if (millis() - lastChangeTime > 2000) {
        rotateDirection = !rotateDirection;
        lastChangeTime = millis();
      }
      
      if (rotateDirection) {
        sendCommand(CMD_ROTATE_LEFT);
      } else {
        sendCommand(CMD_ROTATE_RIGHT);
      }
    }
  }
  
  // Gửi lại lệnh hiện tại mỗi 300ms để đảm bảo robot không bị mất lệnh
  if (millis() - lastSendTime > 300) {
    sendCommand(lastSentCommand);
  }
  
  // Delay ngắn để tối ưu
  delay(20);
}