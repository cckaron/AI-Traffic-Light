#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <TrafficLight.h>
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
TrafficLight light_a(1, 30, CLK1, DIO1); // TrafficLight(dir, sec, CLK, DIO)
TrafficLight light_b(1, 30, CLK2, DIO2);
TrafficLight light_c(2, 20, CLK3, DIO3);
TrafficLight light_d(2, 20, CLK4, DIO4);

int operand, second;
int nowDirect; // 1->ab; 2->cd;
String received;

boolean NODEMCU_DISCONNECT = 1;

Timer sendESP;

StaticJsonDocument<200> data;
StaticJsonDocument<200> esp_data;

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // 設定 LCD I2C 位址

void setup() {
  Serial1.begin(9600); //for NodeMCU (Wifi)
  Serial2.begin(9600); //for arduino 2 (led controller)

  lcd.begin(16, 2);      // 初始化 LCD，一行 16 的字元，共 2 行，預設開啟背光


  // 等待 nodemcu
  lcd.clear();
  lcd.setCursor(0, 0); // 設定游標位置在第一行行首
  lcd.print("Waiting for");
  lcd.setCursor(0, 1); // 設定游標位置在第二行行首
  lcd.print("NodeMCU WIFI..");
  delay(1000);

  //busy wait..
  while (NODEMCU_DISCONNECT) {
    if (Serial1.available() > 0) {
      if (Serial1.read() == 100) {
        NODEMCU_DISCONNECT = 0;
      }
    }
  }

  lcd.clear();
  lcd.setCursor(0, 0); // 設定游標位置在第一行行首
  lcd.print("WIFI Connected!");
  delay(1000);


  // 等待 Light Controller
  lcd.setCursor(0, 0);
  lcd.print("Waiting for");
  lcd.setCursor(0, 1);
  lcd.print("Light Controller");
  delay(1000);

  //busy wait..
  while (Serial2.available() == 0) {}

  lcd.clear();
  lcd.setCursor(0, 0); // 設定游標位置在第一行行首
  lcd.print("Connected!");
  delay(1000);


  lcd.clear();
  lcd.setCursor(0, 0); // 設定游標位置在第一行行首
  lcd.print("Red: 1-2");
  lcd.setCursor(0, 1); // 設定游標位置在第一行行首
  lcd.print("Status: Normal");

  //set direct to 1
  nowDirect = 1;
  Serial2.write(nowDirect);

  //set the data to light a's data
  light_a.init(light_a.defaultSec + 1);
  light_b.init(light_a.defaultSec + 1);
  light_c.init(light_a.defaultSec + 1);
  light_d.init(light_a.defaultSec + 1);



  //send data to laravel for every 3 data
  sendESP.every(5000, sendJSON);
}

void loop() {
  //adjust data of traffic light
  light_a.minus(1);
  light_b.minus(1);
  light_c.minus(1);
  light_d.minus(1);

  if (light_a.nowSec <= 0 || light_c.nowSec <= 0) {
    lcd.setCursor(0, 1); // 設定游標位置在第一行行首
    if (nowDirect == 1) {
      nowDirect = 2;
      Serial2.write(nowDirect);
      light_a.setSec(light_c.variaSec);
      light_b.setSec(light_c.variaSec);
      light_c.setSec(light_c.variaSec);
      light_d.setSec(light_c.variaSec);
      lcd.setCursor(0, 0); // 設定游標位置在第一行行首
      lcd.print("Red: 3-4");

      
    } else {
      nowDirect = 1;
      Serial2.write(nowDirect);
      light_a.setSec(light_a.variaSec);
      light_b.setSec(light_a.variaSec);
      light_c.setSec(light_a.variaSec);
      light_d.setSec(light_a.variaSec);

      lcd.setCursor(0, 0); // 設定游標位置在第一行行首
      lcd.print("Red: 1-2");

      
    }
  }

  //show the data on screen
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

  data["sec_a"] = light_a.nowSec;
  data["now_direct"] = nowDirect;
//  data["variaSec_a"] = light_a.variaSec;
//  data["variaSec_c"] = light_c.variaSec;

  // waiting for messages from ESP8266
  if (Serial1.available() > 0) {

    //read the message
    received = Serial1.readString();

    //transform the message from text to JSON format
    DeserializationError error = deserializeJson(esp_data, received);

    // extract the data from JSON
    second = esp_data["second"];
    operand = esp_data["operand"];

    //如果扣秒數的方向 等於 當前紅燈方向
    //if (direct == nowDirect) {
    
    if (operand == 1) { //加秒數
      //扣預設秒數 //不用扣了
//        light_a.changeVariaSec(second);
//        light_b.changeVariaSec(second);

      //扣當前秒數
      if ((light_a.nowSec - second ) <= 0) {
        light_a.nowSec = 0;
        light_b.nowSec = 0;
        light_c.nowSec = 0;
        light_d.nowSec = 0;
      } else {
        light_a.add(second);
        light_b.add(second);
        light_c.add(second);
        light_d.add(second);
      }
    }

    if (operand == 2) { //減少秒數
      //扣預設秒數 //不用扣了
//        light_c.changeVariaSec(second);
//        light_d.changeVariaSec(second);

      //扣當前秒數
      if ((light_c.nowSec - second ) <= 0) {
        light_a.nowSec = 0;
        light_b.nowSec = 0;
        light_c.nowSec = 0;
        light_d.nowSec = 0;
      } else {
        light_a.minus(second);
        light_b.minus(second);
        light_c.minus(second);
        light_d.minus(second);
      }


    }

    lcd.setCursor(0, 1);
    lcd.print("Status: Minus ");
    //}
  }

  sendESP.update();
  delay(1000);
}

void sendJSON() {
  serializeJsonPretty(data, Serial1);
  if (nowDirect == 1) {
    Serial2.write(nowDirect);
  } else if (nowDirect == 2) {
    Serial2.write(nowDirect);
  }

  lcd.setCursor(0, 1); // 設定游標位置在第一行行首
  lcd.print("Status: Normal  ");
}
