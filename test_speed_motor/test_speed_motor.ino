// TT Motor Speed Test

#include <TM1637Display.h>

// Пины модулей
#define BUTTON_PIN 2    // Кнопка сброса (сигнал S -> D2)
#define RELAY_PIN 4     // Реле (IN -> D4)
#define SENSOR_PIN 12   // Сенсор импульсов (сигнал -> D12)
#define LED_PIN 13      // Встроенный LED для индикации
#define CLK 7           // Дисплей CLK -> D7
#define DIO 8           // Дисплей DIO -> D8

// НАСТРОЙКА ВРЕМЕНИ РАБОТЫ РЕЛЕ (в миллисекундах)
#define RELAY_WORK_TIME 40000  // 3000 мс = 3 секунды

TM1637Display display(CLK, DIO);

// Таблица сегментов для цифр 0-9
const uint8_t digits[] = {
  0b00111111,  // 0
  0b00000110,  // 1
  0b01011011,  // 2
  0b01001111,  // 3
  0b01100110,  // 4
  0b01101101,  // 5
  0b01111101,  // 6
  0b00000111,  // 7
  0b01111111,  // 8
  0b01101111   // 9
};

// Пустой сегмент (все сегменты выключены)
const uint8_t SEGMENT_OFF = 0b00000000;

// Переменные состояния
int pulseCount = 0;
bool isCounting = false;
unsigned long startTime = 0;
bool lastButtonState = HIGH;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(SENSOR_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  
  display.setBrightness(7);
  display.clear();
  
  showNumber(pulseCount);
}

void loop() {
  bool currentButtonState = digitalRead(BUTTON_PIN);
  
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    startCounting();
  }
  lastButtonState = currentButtonState;
  
  if (isCounting) {
    static int lastSensorState = HIGH;
    int currentSensorState = digitalRead(SENSOR_PIN);
    
    if (lastSensorState == HIGH && currentSensorState == LOW) {
      pulseCount++;
      showNumber(pulseCount);
      
      digitalWrite(LED_PIN, HIGH);
      delay(10);
      digitalWrite(LED_PIN, LOW);
    }
    lastSensorState = currentSensorState;
    
    // Вот здесь используется настройка времени
    if (millis() - startTime >= RELAY_WORK_TIME) {
      stopCounting();
    }
  }
  
  delay(1);
}

void startCounting() {
  pulseCount = 0;
  showNumber(pulseCount);
  
  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(LED_PIN, HIGH);
  
  startTime = millis();
  isCounting = true;
}

void stopCounting() {
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  
  isCounting = false;
}

void showNumber(int num) {
  if (num > 9999) num = 9999;
  
  uint8_t data[4];
  
  if (num == 0) {
    data[0] = SEGMENT_OFF;
    data[1] = SEGMENT_OFF;
    data[2] = SEGMENT_OFF;
    data[3] = digits[0];
  } else {
    int thousands = (num / 1000) % 10;
    int hundreds = (num / 100) % 10;
    int tens = (num / 10) % 10;
    int ones = num % 10;
    
    bool started = false;
    
    if (thousands > 0 || started) {
      data[0] = digits[thousands];
      started = true;
    } else {
      data[0] = SEGMENT_OFF;
    }
    
    if (hundreds > 0 || started) {
      data[1] = digits[hundreds];
      started = true;
    } else {
      data[1] = SEGMENT_OFF;
    }
    
    if (tens > 0 || started) {
      data[2] = digits[tens];
      started = true;
    } else {
      data[2] = SEGMENT_OFF;
    }
    
    data[3] = digits[ones];
  }
  
  display.setSegments(data);
}