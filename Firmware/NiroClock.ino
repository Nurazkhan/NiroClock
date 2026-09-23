#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <string>
#include <iostream>


//display
#define TFT_SCLK 9 // labeled SCL on the screen
#define TFT_MOSI 10// labeled SDA on the screen
#define TFT_RST -1
#define TFT_DC 5
#define TFT_CS 6
#define TFT_BL 8


//buzzer
#define BUZZER 7

//butons
#define BT5 0
#define BT4 1
#define BT3 2
#define BT2 3
#define BT1 4


//time
unsigned long tick = 0;
int hours = 12, minutes = 0, seconds =0;
int alarmHours = 0, alarmMinutes =0;
bool alarmEnabled = False;

// debounce for btns
bool LastBt1 = HIGH;
bool LastBt2 = HIGH;
bool LastBt3 = HIGH;
bool LastBt4 = HIGH;
bool LastBt5 = HIGH;

//debounce time
const unsigned long = 150; //ms


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

enum Mode {State_Clock,State_Alarm, State_Settings, State_SetClock,State_SetAlarm, State_Secondmeter};
enum ClockMode {HMS, HM};

Mode currentState = State_Clock;
ClockMode CurrentClockMode = HMS;

void drawClock(){
  if(currentState != State_Clock){
    return; 
  }
  tft.setCursor(20,20);
  tft.setTextSize(6);
  if (CurrentClockMode == HMS){
    char timeBuffer[9]; //HH:MM:SS 9 symbols
    snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d:%02d", hours, minutes, seconds);
  }else{
    char timeBuffer[5]; // HH:MM 5 symbol
    snprintf(timeBuffer,sizeof(timeBuffer), "%02d:%02d", hours, minutes)
  }
  
  tft.print(timebuffer);
}
void drawSetClock(int[] snapshotTime){
  if(currentState != State_SetClock){
    return; 
  }
  tft.setCursor(20,20);
  tft.setTextSize(6);
 
    char timeBuffer[9]; //HH:MM:SS 9 symbols
    snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d:%02d", snapshotTime[0], snapshotTime[1], snapshotTime[2]);
  
  
  tft.print(timebuffer);
};

bool checkPressed(int pin, bool lastState){
  int currentPressed = digitalRead(pin);
  if (currentPressed == LOW && lastState == HIGH ){
    lastState = LOW;
    delay(20);
    return true;
  }
  lastState = currentPressed;
  return false;
};


void ClockPageButtons(){
  if(currentState != State_Clock){
    return;
  };
  if(checkPressed(BT1, lastBt1)){
    CurrentClockMode = CurrentClockMode == HMS ? HM:HMS;
  };
  if(checkPressed(BT2, lastBt2)){
    currentState = State_Secondmeter;
  };
  if(checkPressed(BT4, lastBt4)){
    currentState = State_SetClock;
  };
  if(checkPressed(BT5, lastBt5)){
    currentState = State_SetAlarm;
  }
};

void SetClockPageButtons(){
  if(currentState != State_SetClock){
    return;
  };
  int pointer = 0; //max value 2
  /*  H:M:S
      ^   
      |  */
  int[] snapshotTime = [hours,minutes,seconds]; 
  drawSetClock(snapshotTime);
    // move left
   if(checkPressed(BT1, lastBt1)){
    pointer -=1;
    pointer= pointer < 0 ? 2:pointer;
  };
  //move right
  if(checkPressed(BT2, lastBt2)){
    pointer +=1;
    pointer= pointer > 2 ? 0:pointer;
  };
  //move up
  if(checkPressed(BT3, lastBt3)){
    snapshotTime[pointer] += 1;
    if(pointer == 0 && snapshotTime[pointer] > 23){
      snapshotTime[pointer] = 0
    }else if ((pointer == 1 || pointer == 2) && snapshotTime[pointer] > 59)
    {
      snapshotTime[pointer] = 0
    }};
    // move down
    if(checkPressed(BT4, lastBt4)){
    snapshotTime[pointer] -= 1;
    if(pointer == 0 && snapshotTime[pointer] < 0){
      snapshotTime[pointer] = 23
    }else if ((pointer == 1 || pointer == 2) && snapshotTime[pointer] < 0)
    {
      snapshotTime[pointer] = 59
    };};
    // Save and leave
    if(checkPressed(BT5, lastBt5)){
      hours = snapshotTime[0];
      minutes = snapshotTime[1];
      seconds = snapshotTime[2];
      currentState = State_SetClock;
    };
    
  };

void SetAlarmPageButtons(){

}



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(TFT_BL, OUTPUT); 
  digitalWrite(TFT_BL, LOW);

  //btns
pinMode(BT1, INPUT_PULLUP);
pinMode(BT2, INPUT_PULLUP);
pinMode(BT3, INPUT_PULLUP);
pinMode(BT4, INPUT_PULLUP);
pinMode(BT5, INPUT_PULLUP);


//display setup
tft.init(76, 284); // Our panel size (portrait)
tft.setOffsets(82, 18); // Offsets for the weird resolution
tft.invertDisplay(false); // Invert the colors (This display is flipped from normal)
tft.setRotation(1); // Landscape, if it's upside down use 3!
tft.fillScreen(ST77XX_BLACK); // clear the screen
Serial.println("TFT Initialized!");
tft.setCursor(0,0); 

//buzzer
pinMode(BUZZER, OUTPUT);


  

}

void loop() {
  // put your main code here, to run repeatedly:
  if(millis() - tick >= 1){
    tick = millis();
    seconds +=1;
    if (seconds >59){
      seconds = 0;
      minutes +=1;
      if(minutes > 59){
        minutes = 0;
        hours +=1;
        if(hours > 23){
          hours = 0
        }
      }
    }
    
    drawClock();
  };
  
  
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