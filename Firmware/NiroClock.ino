#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

// Display SPI Pins
#define TFT_SCLK 9   // SCL on screen
#define TFT_MOSI 10  // SDA on screen
#define TFT_RST  -1
#define TFT_DC   5
#define TFT_CS   6
#define TFT_BL   8


#define BUZZER   7

#define BT5 0
#define BT4 1
#define BT3 2
#define BT2 3
#define BT1 4

const unsigned long DEBOUNCE_TIME = 150; // ms
unsigned long clockTick = 0;
unsigned long buzzerTick = 0;
bool buzzerState = false;


int hours = 12, minutes = 0, seconds = 0;
int alarmHours = 0, alarmMinutes = 0;
bool alarmEnabled = false;

int secondmeter[3] = {0, 0, 0}; // H, M, S
bool secondmeterEnabled = false;


bool lastBt1 = HIGH;
bool lastBt2 = HIGH;
bool lastBt3 = HIGH;
bool lastBt4 = HIGH;
bool lastBt5 = HIGH;


class MyST7789 : public Adafruit_ST7789 {
public:
  MyST7789(int8_t cs, int8_t dc, int8_t mosi, int8_t sclk, int8_t rst)
    : Adafruit_ST7789(cs, dc, mosi, sclk, rst) {}
  void setOffsets(uint8_t col, uint8_t row) {
    _colstart = _colstart2 = col;
    _rowstart = _rowstart2 = row;
  }
};

MyST7789 tft(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

enum Mode { State_Clock, State_Alarm, State_SetClock, State_SetAlarm, State_Secondmeter };
enum ClockMode { HMS, HM };

Mode currentState = State_Clock;
ClockMode currentClockMode = HMS;

// Setting buffers
int editTime[3] = {0, 0, 0}; // [H, M, S]
int editPointer = 0;


bool checkPressed(int pin, bool &lastState) {
  int currentPressed = digitalRead(pin);
  if (currentPressed == LOW && lastState == HIGH) {
    lastState = LOW;
    delay(20); 
    return true;
  }
  if (currentPressed == HIGH && lastState == LOW) {
    lastState = HIGH;
  }
  return false;
}


void drawClock() {
  tft.setCursor(20, 20);
  tft.setTextSize(4);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);

  char timeBuffer[12];
  if (currentClockMode == HMS) {
    snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d:%02d", hours, minutes, seconds);
  } else {
    snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d   ", hours, minutes);
  }
  tft.print(timeBuffer);

  // Status line
  tft.setCursor(20, 60);
  tft.setTextSize(2);
  tft.printf("Alarm: %s [%02d:%02d]", alarmEnabled ? "ON " : "OFF", alarmHours, alarmMinutes);
}

void drawSetClock() {
  tft.setCursor(20, 15);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
  tft.print("Set Time (H:M:S)   ");

  tft.setCursor(20, 40);
  tft.setTextSize(4);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  char buffer[12];
  snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", editTime[0], editTime[1], editTime[2]);
  tft.print(buffer);

  
  tft.setCursor(20, 75);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_CYAN, ST77XX_BLACK);
  if (editPointer == 0)      tft.print("Selected: HOURS  ");
  else if (editPointer == 1) tft.print("Selected: MINUTES");
  else                       tft.print("Selected: SECONDS");
}

void drawSetAlarm() {
  tft.setCursor(20, 15);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
  tft.print("Set Alarm (H:M)    ");

  tft.setCursor(20, 40);
  tft.setTextSize(4);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  char buffer[10];
  snprintf(buffer, sizeof(buffer), "%02d:%02d   ", editTime[0], editTime[1]);
  tft.print(buffer);

  tft.setCursor(20, 75);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_CYAN, ST77XX_BLACK);
  if (editPointer == 0) tft.print("Selected: HOURS  ");
  else                  tft.print("Selected: MINUTES");
}

void drawSecondmeter() {
  tft.setCursor(20, 15);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
  tft.print("Stopwatch          ");

  tft.setCursor(20, 40);
  tft.setTextSize(4);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  char buffer[12];
  snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", secondmeter[0], secondmeter[1], secondmeter[2]);
  tft.print(buffer);
}

void drawAlarm() {
  tft.setCursor(20, 15);
  tft.setTextSize(3);
  tft.setTextColor(ST77XX_RED, ST77XX_BLACK);
  tft.print("WAKE UP!   ");

  tft.setCursor(20, 50);
  tft.setTextSize(4);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  char timeBuffer[10];
  snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d", alarmHours, alarmMinutes);
  tft.print(timeBuffer);
}


void handleBuzzer() {
  if (currentState != State_Alarm) {
    digitalWrite(BUZZER, LOW);
    buzzerState = false;
    return;
  }

  // every 300s beep 
  if (millis() - buzzerTick >= 300) {
    buzzerTick = millis();
    buzzerState = !buzzerState;
    digitalWrite(BUZZER, buzzerState ? HIGH : LOW);
  }
}



void ClockPageButtons() {
  drawClock();

  if (checkPressed(BT1, lastBt1)) {
    currentClockMode = (currentClockMode == HMS) ? HM : HMS;
    tft.fillScreen(ST77XX_BLACK);
  }
  if (checkPressed(BT2, lastBt2)) {
    tft.fillScreen(ST77XX_BLACK);
    currentState = State_Secondmeter;
  }
  if (checkPressed(BT4, lastBt4)) {
    editTime[0] = hours;
    editTime[1] = minutes;
    editTime[2] = seconds;
    editPointer = 0;
    tft.fillScreen(ST77XX_BLACK);
    currentState = State_SetClock;
  }
  if (checkPressed(BT5, lastBt5)) {
    editTime[0] = alarmHours;
    editTime[1] = alarmMinutes;
    editPointer = 0;
    tft.fillScreen(ST77XX_BLACK);
    currentState = State_SetAlarm;
  }
}

void SetClockPageButtons() {
  drawSetClock();
 // Left
  if (checkPressed(BT1, lastBt1)) {
    editPointer = (editPointer <= 0) ? 2 : editPointer - 1;
  }
  // Right
  if (checkPressed(BT2, lastBt2)) { 
    editPointer = (editPointer >= 2) ? 0 : editPointer + 1;
  }
  if (checkPressed(BT3, lastBt3)) { // Up
    int maxVal = (editPointer == 0) ? 23 : 59;
    editTime[editPointer] = (editTime[editPointer] + 1) > maxVal ? 0 : editTime[editPointer] + 1;
  }
  if (checkPressed(BT4, lastBt4)) { // Down
    int maxVal = (editPointer == 0) ? 23 : 59;
    editTime[editPointer] = (editTime[editPointer] - 1) < 0 ? maxVal : editTime[editPointer] - 1;
  }
  if (checkPressed(BT5, lastBt5)) { // Save & Exit
    hours = editTime[0];
    minutes = editTime[1];
    seconds = editTime[2];
    tft.fillScreen(ST77XX_BLACK);
    currentState = State_Clock;
  }
}

void SetAlarmPageButtons() {
  drawSetAlarm();

  if (checkPressed(BT1, lastBt1)) { // Left
    editPointer = (editPointer <= 0) ? 1 : editPointer - 1;
  }
  if (checkPressed(BT2, lastBt2)) { // Right
    editPointer = (editPointer >= 1) ? 0 : editPointer + 1;
  }
  if (checkPressed(BT3, lastBt3)) { // Up
    int maxVal = (editPointer == 0) ? 23 : 59;
    editTime[editPointer] = (editTime[editPointer] + 1) > maxVal ? 0 : editTime[editPointer] + 1;
  }
  if (checkPressed(BT4, lastBt4)) { // Down
    int maxVal = (editPointer == 0) ? 23 : 59;
    editTime[editPointer] = (editTime[editPointer] - 1) < 0 ? maxVal : editTime[editPointer] - 1;
  }
  if (checkPressed(BT5, lastBt5)) { // Save & Exit
    alarmHours = editTime[0];
    alarmMinutes = editTime[1];
    alarmEnabled = true;
    tft.fillScreen(ST77XX_BLACK);
    currentState = State_Clock;
  }
}

void SecondmeterPageButtons() {
  drawSecondmeter();

  if (checkPressed(BT1, lastBt1)) {
    secondmeterEnabled = !secondmeterEnabled;
  }
  if (checkPressed(BT3, lastBt3)) { // Reset
    secondmeterEnabled = false;
    secondmeter[0] = 0;
    secondmeter[1] = 0;
    secondmeter[2] = 0;
  }
  if (checkPressed(BT4, lastBt4) || checkPressed(BT5, lastBt5)) { // Back
    tft.fillScreen(ST77XX_BLACK);
    currentState = State_Clock;
  }
}

void AlarmPageButtons() {
  drawAlarm();

  
  if (checkPressed(BT1, lastBt1) || checkPressed(BT3, lastBt3) ||
      checkPressed(BT4, lastBt4) || checkPressed(BT5, lastBt5)) {
    alarmEnabled = false;
    digitalWrite(BUZZER, LOW);
    tft.fillScreen(ST77XX_BLACK);
    currentState = State_Clock;
  }
  // Snooze for 5 minutes
  if (checkPressed(BT2, lastBt2)) {
    alarmMinutes += 5;
    alarmHours += alarmMinutes / 60;
    alarmMinutes %= 60;
    alarmHours %= 24;
    digitalWrite(BUZZER, LOW);
    tft.fillScreen(ST77XX_BLACK);
    currentState = State_Clock;
  }
}



void setup() {
  Serial.begin(115200);

  
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH); // Turn backlight ON
// Buzzer
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  // Buttons
  pinMode(BT1, INPUT_PULLUP);
  pinMode(BT2, INPUT_PULLUP);
  pinMode(BT3, INPUT_PULLUP);
  pinMode(BT4, INPUT_PULLUP);
  pinMode(BT5, INPUT_PULLUP);

  // Display init
  tft.init(76, 284);
  tft.setOffsets(82, 18);
  tft.invertDisplay(false);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);
}

void loop() {
  
  if (millis() - clockTick >= 1000) {
    clockTick = millis();

    seconds++;
    if (seconds >= 60) {
      seconds = 0;
      minutes++;
      if (minutes >= 60) {
        minutes = 0;
        hours = (hours + 1) % 24;
      }
    }

    if (secondmeterEnabled) {
      secondmeter[2]++;
      if (secondmeter[2] >= 60) {
        secondmeter[2] = 0;
        secondmeter[1]++;
        if (secondmeter[1] >= 60) {
          secondmeter[1] = 0;
          secondmeter[0] = (secondmeter[0] + 1) % 24;
        }
      }
    }

    
    if (alarmEnabled && hours == alarmHours && minutes == alarmMinutes && seconds == 0) {
      currentState = State_Alarm;
      tft.fillScreen(ST77XX_BLACK);
    }
  }

 // pages and handling them properly with control functions
  switch (currentState) {
    case State_Clock:       ClockPageButtons();       break;
    case State_SetClock:    SetClockPageButtons();    break;
    case State_SetAlarm:    SetAlarmPageButtons();    break;
    case State_Secondmeter: SecondmeterPageButtons(); break;
    case State_Alarm:       AlarmPageButtons();       break;
  }

  
  handleBuzzer();
}


// 5 buttons role in each state
/*
Page - 1 Clock State
btn1 - change display mode (HH:MM:SS) or (HH:MM) 
btn2 - setSecondmeter
btn3 -  nothing ? Mute
btn4 - setClock
btn5 - setAlarm

Page - 2 setClock
btn1 - Moveleft 
btn2 - MoveRight
btn3 - MoveUp
btn4 - MoveDown
btn5 - Confirm Leave

Page - 3 set alarm
btn1 - Moveleft 
btn2 - MoveRight
btn3 - MoveUp
btn4 - MoveDown
btn5 - Confirm Leave

Page - 3 start Secondmeter
btn1 - pause/continue
btn2 - flag
btn3 - reset
btn4 - return home ( do not stop)
btn5 - return home (reset)

page - 4 AlarmRinging

btn1 - Stop
btn2 - Snooze
btn3 - Stop
btn4 - Stop
btn5 - Stop

*/