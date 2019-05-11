#include <TrafficLight.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <Timer.h>

// Module connection pins (Digital Pins)
#define DIO1 22
#define CLK1 23
#define DIO2 26
#define CLK2 27
#define DIO3 24
#define CLK3 25
#define DIO4 28
#define CLK4 29

//declare 4 traffic lights
TrafficLight light_a(1, 30, CLK1, DIO1); // TrafficLight(dir, deafult sec, CLK, DIO)
TrafficLight light_b(1, 30, CLK2, DIO2);
TrafficLight light_c(2, 20, CLK3, DIO3);
TrafficLight light_d(2, 20, CLK4, DIO4);

int operand, second;
int nowDirect; // 1->ab; 2->cd;
int lastUpdate;
const long interval = 1000;
String received;
String message;

boolean NODEMCU_DISCONNECT = 1;

Timer sendESP;

StaticJsonDocument<1024> data;
StaticJsonDocument<1024> esp_data;

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // 設定 LCD I2C 位址

void setup() {
  //初始化序列埠
  initSerial();

  //初始化 lcd
  initLCD();

  // 等待 nodemcu
  waitNodeMCU();

  //等待 arduino led controller
  waitLightController();

  //設定初始秒數 (皆為 a 的初始秒數)
  //設定初始紅燈方向為 1<--->2 (1)
  // (direct, second)
  setDirAndSec(1, light_a.defaultSec + 1);

  //每 3 秒傳送秒數資料給 NodeMCU
  sendESP.every(5000, sendJSON);
}

void loop() {
  //秒數扣1
  minus();

  //判斷是否需要切換方向
  checkSwitch();

  //顯示秒數 (紅燈方向才顯示)
  showSecond();

  //把當前秒數, 當前方向 放到 data 字典檔裡面
  data["now_second"] = light_a.nowSec;
  data["now_direct"] = nowDirect;

  // 傳送資料到 NodeMCU
  sendESP.update();

  // 監聽 NodeMCU 的訊息 (秒數變動)
  listenToNodeMCU();

  //延遲一秒
  delay(1000);
}

void initSerial() {
  Serial.begin(9600); //usb slot
  Serial1.begin(9600); //for TX/RX message on arduino 2 (light controller)
  Serial2.begin(9600); // for Transmit seconds to NodeMCU(TX only)
  Serial3.begin(9600); //for Receive data from NodeMCU(RX only)
  Serial.println("hi");
}

void initLCD() {
  lcd.begin(16, 2);      // 初始化 LCD，一行 16 的字元，共 2 行，預設開啟背光
  lcd.clear();
}

void waitNodeMCU() {
  lcd.setCursor(0, 0); // 設定游標位置在第一行行首
  lcd.print("Waiting for");
  lcd.setCursor(0, 1); // 設定游標位置在第二行行首
  lcd.print("NodeMCU WIFI..");
  delay(1000);

  //busy wait..
  while (NODEMCU_DISCONNECT) {
    if (Serial3.available() > 0) {
      if (Serial3.read() == 100) {
        NODEMCU_DISCONNECT = 0;
      }
    }
  }

  //Connected!
  lcd.clear();
  lcd.setCursor(0, 0); // 設定游標位置在第一行行首
  lcd.print("WIFI Connected!");
  delay(1000);
}

void waitLightController() {
  // 等待 Light Controller
  lcd.setCursor(0, 0);
  lcd.print("Waiting for");
  lcd.setCursor(0, 1);
  lcd.print("Light Controller");
  delay(1000);

  //busy wait..
  while (Serial1.available() == 0) {}

  lcd.clear();
  lcd.setCursor(0, 0); // 設定游標位置在第一行行首
  lcd.print("Connected!");
  delay(1000);

  //lcd 顯示號誌初始狀態
  lcd.clear();
  lcd.setCursor(0, 0); // 設定游標位置在第一行行首
  lcd.print("Red: 1-2");
  lcd.setCursor(0, 1); // 設定游標位置在第一行行首
  lcd.print("Status: Normal");
}

void setDirAndSec(int direct, int sec) {
  //告知切換方向
  nowDirect = direct;
  Serial1.print(nowDirect);

  //設定秒數
  light_a.init(sec);
  light_b.init(sec);
  light_c.init(sec);
  light_d.init(sec);
}

void sendJSON() {
  serializeJsonPretty(data, Serial2);
  if (nowDirect == 1) {
    Serial1.print(nowDirect);
  } else if (nowDirect == 2) {
    Serial1.print(nowDirect);
  }

  lcd.setCursor(0, 1); // 設定游標位置在第一行行首
  lcd.print("Status: Normal  ");
}

void minus() {
  //秒數扣1
  light_a.minus(1);
  light_b.minus(1);
  light_c.minus(1);
  light_d.minus(1);
}

void checkSwitch() {
  //當秒數為 0 時, 變更方向
  if (light_a.nowSec <= 0 || light_c.nowSec <= 0) {
    lcd.setCursor(0, 1); // 設定游標位置在第一行行首

    //如果 1 <---> 2 方向是紅燈
    if (nowDirect == 1) {
      //變更方向為 3 <----> 4
      nowDirect = 2;
      Serial1.print(nowDirect);
      light_a.setSec(light_c.variaSec);
      light_b.setSec(light_c.variaSec);
      light_c.setSec(light_c.variaSec);
      light_d.setSec(light_c.variaSec);
      lcd.setCursor(0, 0); // 設定游標位置在第一行行首
      lcd.print("Red: 3-4");

    } else { //如果 3 <---> 4 方向是紅燈
      //變更方向為 1 <----> 2
      nowDirect = 1;
      Serial1.print(nowDirect);
      light_a.setSec(light_a.variaSec);
      light_b.setSec(light_a.variaSec);
      light_c.setSec(light_a.variaSec);
      light_d.setSec(light_a.variaSec);

      lcd.setCursor(0, 0); // 設定游標位置在第一行行首
      lcd.print("Red: 1-2");
    }
  }
}

void showSecond() {
  if (nowDirect == 1) {
    light_a.show();
    light_b.show();
    light_c.clear();
    light_d.clear();
  } else if (nowDirect == 2) {
    light_c.show();
    light_d.show();
    light_a.clear();
    light_b.clear();
  }
}

void listenToNodeMCU() {
  if (Serial3.available() > 0) {
    //讀取訊息
    received = Serial3.readString();

    //訊息剖析 (Deserialize)
    DeserializationError error = deserializeJson(esp_data, received);
    if (error) {
      switch (error.code()) {
        case DeserializationError::Ok:
          Serial.print(F("Deserialization succeeded"));
          break;
        case DeserializationError::InvalidInput:
          Serial.print(F("Invalid input!"));
          break;
        case DeserializationError::NoMemory:
          Serial.print(F("Not enough memory"));
          break;
        default:
          Serial.print(F("Deserialization failed"));
          break;
      }
    } else {
      //訊息萃取 (To integer)
      second = esp_data["second"];
      operand = esp_data["operand"];

      //調整秒數 (switch case)
      adjustSecond(second, operand);

      //lcd print message
      lcd.setCursor(0, 1);
      lcd.print(message);
    }
  }
}

void adjustSecond(int sec, int oper) {
  //調整秒數 1->"+"; 2->"-"; 3->"*"; 4->"/"; 5->"="
  switch (oper) {
    case 1: //"+"
      message = "Add " + sec;
      light_a.add(sec);
      light_b.add(sec);
      light_c.add(sec);
      light_d.add(sec);
      break;
    case 2: //"-"
      message = "Minus " + sec;
      if ((light_c.nowSec - sec ) <= 0) {
        light_a.nowSec = 0;
        light_b.nowSec = 0;
        light_c.nowSec = 0;
        light_d.nowSec = 0;
      } else {
        light_a.minus(sec);
        light_b.minus(sec);
        light_c.minus(sec);
        light_d.minus(sec);
      }
      break;
    case 3: //"*"
      message = "Mutiply " + sec;
      light_a.multiply(sec);
      light_b.multiply(sec);
      light_c.multiply(sec);
      light_d.multiply(sec);
      break;
    case 4: //"/"
      message = "Divide " + sec;
      light_a.divide(sec);
      light_b.divide(sec);
      light_c.divide(sec);
      light_d.divide(sec);
      break;
    case 5: //"="
      message = "Divide " + sec;
      light_a.setSec(sec);
      light_b.setSec(sec);
      light_c.setSec(sec);
      light_d.setSec(sec);
      break;
  }
}
