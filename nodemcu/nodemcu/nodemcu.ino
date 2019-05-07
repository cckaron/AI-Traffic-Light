#include <ArduinoJson.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>


const char* ssid = "ASUS_Aaron_2G";
const char* password = "0981429828";

WiFiServer wifiServer(2345);
ESP8266WebServer server(80);

HTTPClient http;

StaticJsonDocument<200> doc;

StaticJsonDocument<200> doc1;
JsonObject data = doc1.to<JsonObject>();

String received = "";
String url = "";
String page, page2, page3;
int now;
int past;
int httpCode;
int value;
int sending;

long second, operand, now_direct, now_second;

void setup() {
  Serial.begin(9600); // for receiving seconds from Arduino
  Serial1.begin(9600);// for transmitting operator and operands

  //waiting for wifi connected
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    //    Serial.println("Connecting..");
  }

  //  Serial.print("Conntected to Wifi. IP:");
  //  Serial.println(WiFi.localIP());

  wifiServer.begin();
  //  Serial.println("port 2345 opened");

  //  Serial.println(WiFi.macAddress());

  //web server
  page = "<h1>NodeMCU Web Server</h1><p><a href=\"minus\"><button>Minus</button></a></p>";

  server.on("/", []() {
    server.send(200, "text/html", page);
  });

  server.on("/change", []() {
    operand = server.arg(0).toInt();
    second = server.arg(1).toInt();
    page2 = "<h1>Operand:"+ operand;
    page2 += "</h1><h1><Data:";
    page2 += second;
    page2 += "></h1>";
    server.send(200, "text/html", page2);

    data["operand"] = operand;
    data["second"] = second;

    //send message to Arduino
    serializeJsonPretty(data, Serial1);
  });

  server.begin();

  Serial1.write(100); //init message
}

void loop() {
  while (Serial.available() > 0) {
    received = Serial.readString();
    Serial.println(received);
    DeserializationError error = deserializeJson(doc, received);
    JsonObject data = doc.as<JsonObject>();
    now_second = data["now_second"];
    now_direct = data["now_direct"];
    
    url = "http://192.168.50.46/arduino/" + String(now_direct) + "/" + String(now_second);

    http.begin(url);
    httpCode = http.GET();
    http.end();
  }

  server.handleClient();
}
