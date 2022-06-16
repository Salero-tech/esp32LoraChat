#include <Arduino.h>

//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>

//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//wifi
#include <WiFi.h>
//web server
#include <WebServer.h>
#include "SPIFFS.h"

//define the pins used by the LoRa transceiver module
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

//433E6 for Asia
//866E6 for Europe
//915E6 for North America
#define BAND 866E6

//OLED pins
#define OLED_SDA 4
#define OLED_SCL 15 
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels


//wifi
// SSID & Password
const char* ssid = "Elektroniker_E3_2.4GHz";  // Enter your SSID here
const char* password = "%!MSW33!%";  //Enter your Password here

//packet packet_counter
int packet_counter = 0;

//Display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

//webserver
WebServer server(80);  // Object of WebServer(HTTP port, 80 is defult)

//lora
String loraData;

String readFile(String name)
{
  File file = SPIFFS.open(name);
  if(!file) return "";

  String content = "";
  //read
  while(file.available())
  {
    //Serial.println(file.read());
    content += char(file.read());
  }

  file.close();

  return content;
}

String getLoraData()
{
  String LoRa_Data;
  //check if packet available
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    //received a packet

    //read packet
    while (LoRa.available()) {
      LoRa_Data = LoRa.readString();
    }
    return LoRa_Data;
  }
  return "";
}

void handle_root()
{
  server.send(200, "text/html", readFile("/index.html"));
}

void handle_style()
{
  server.send(200, "text/css", readFile("/style.css"));
}

void handle_js()
{
  server.send(200, "text/javascript", readFile("/main.js"));
}

void handle_api_loraSend()
{
  String jsonString = server.arg(0);

  //Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print(jsonString);
  LoRa.endPacket();
  server.send(200);
}

void handle_api_loraRec()
{
  //headers
  server.sendHeader("Accept", "*");
  server.sendHeader("Access-Control-Allow-Headers", "application/json");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  //send
  server.send(200, "text/json", loraData);
  loraData = "";
}

void setup() {
  //serial
  //Serial.begin(115200);

  //file system
  if(!SPIFFS.begin(true)){
  return;
}

  //reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    //FAIL
    for(;;); // Don't proceed, loop forever
  }
  
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("LORA SENDER");
  display.display();
  


  //wlan
  //connect
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  //ip
  display.setCursor(0,10);
  display.print("IP: ");
  display.setCursor(20,10);
  display.print(WiFi.localIP());  //Show ESP32 IP on serial
  display.display();

  //web server
  server.on("/", handle_root);
  server.on("/style.css", handle_style);
  server.on("/main.js", handle_js);
  server.on("/api/lorasend", HTTP_POST, handle_api_loraSend);
  server.on("/api/lorarec", handle_api_loraRec);
  server.begin();


  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);
  
  display.setCursor(0,20);
  if (!LoRa.begin(BAND)) {
    display.print("Starting LoRa failed!");
    display.display();
    while (1);
  }
  display.print("LoRa Initializing OK!");
  display.display();
  delay(2000);
}

String temp;

void loop() {
  server.handleClient();

  temp = getLoraData();
  if (temp != "")
  {
    loraData = temp;
  }

}