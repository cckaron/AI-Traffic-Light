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

int second, operand;
long now_direct;
long light_a, light_b, light_c, light_d, variaSec_a, variaSec_b, variaSec_c, variaSec_d;

void setup() {
  Serial.begin(9600);

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
  page2 = "<h1>Has been added</h1>";
  page3 = "<h1>Has been minused</h1>";

  server.on("/", []() {
    server.send(200, "text/html", page);
  });

  server.on("/add", []() {
    server.send(200, "text/html", page2);

    second = server.arg(0).toInt();

    data["operand"] = 1;
    data["second"] = second;

    //send message to Arduino
    serializeJsonPretty(data, Serial);
  });
  
  server.on("/minus", []() {
    server.send(200, "text/html", page3);

    second = server.arg(0).toInt();

    data["operand"] = 2;
    data["second"] = second;

    //send message to Arduino
    serializeJsonPretty(data, Serial);
  });

  server.begin();

  Serial.write(100); //init message
}

void loop() {
  while (Serial.available() > 0) {
    received = Serial.readString();

    DeserializationError error = deserializeJson(doc, received);
    JsonObject data = doc.as<JsonObject>();
    light_a = data["sec_a"];
    now_direct = data["now_direct"];
//    variaSec_a = data["variaSec_a"];
//    variaSec_c = data["variaSec_c"];

    url = "http://192.168.50.149/arduino/" + String(light_a) + "/" + String(now_direct);

    http.begin(url);
    httpCode = http.GET();
    http.end();
  }

  server.handleClient();
}
