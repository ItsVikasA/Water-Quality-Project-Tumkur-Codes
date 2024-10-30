 #include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_ADS1015.h>
#include <DFRobot_ESP_EC.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h> // Include SoftwareSerial library
#include <BME63M001.h> // Include your BME63M001 sensor library here

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define ONE_WIRE_BUS 14 // this is the gpio pin 14 on esp32
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sen0sors(&oneWire);

DFRobot_ESP_EC ec;
Adafruit_ADS1115 ads;

float voltage, ecValue, temperature = 25;
float tdsValue; // Variable to store TDS value in ppm
String apiKey = "W0L54FD34XOTI2WL"; // Enter your Write API key from ThingSpeak

const char *ssid = "project"; // replace with your wifi ssid and wpa2 keyconst 
char *pass = "Vikas@2005";
const char *server = "api.thingspeak.com"; // Added the server address

WiFiClient client;

void setup() {
  Serial.begin(115200);
  EEPROM.begin(32); // needed EEPROM.begin to store calibration k in eeprom
  ec.begin();
  sen0sors.begin(); // Changed from sensors to sen0sors

  // Initialize your BME63M001 sensor
  // Example: bmeSensor.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  delay(2000);
  display.clearDisplay();

  Serial.println("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop() {
  voltage = analogRead(A0); // A0 is the gpio 36
  sen0sors.requestTemperatures(); // Changed from sensors to sen0sors
  temperature = sen0sors.getTempCByIndex(0); // Read your temperature sensor to execute temperature compensation
  ecValue = ec.readEC(voltage, temperature); // Convert voltage to EC with temperature compensation
  
  // Read TDS value
  // Example: tdsValue = tdsSensor.readTDS();

  // Example calculation to convert EC to TDS
  tdsValue = ecValue * 500; // Example conversion factor, adjust as per your sensor's specifications
  
  Serial.print("Temperature:");
  Serial.print(temperature, 2);
  Serial.println("ºC");

  Serial.print("EC:");
  Serial.println(ecValue, 2);

  Serial.print("TDS:");
  Serial.println(tdsValue, 0); // Print TDS value in whole numbers (ppm)

  display.setTextSize(2);
  display.setTextColor(WHITE);

  display.setCursor(0, 10);
  display.print("T:");
  display.print(temperature, 2);
  display.drawCircle(85, 10, 2, WHITE); // Put degree symbol ( ° )
  display.setCursor(90, 10);
  display.print("C");

  display.setCursor(0, 40);
  display.print("TDS:");
  display.print(tdsValue, 0); // Print TDS value on OLED display
  display.display();
  delay(1500);
  display.clearDisplay();
  ec.calibration(voltage, temperature); // Calibration process by Serial CMD

  if (client.connect(server, 80)) {
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(temperature, 2);
    postStr += "&field2=";
    postStr += String(ecValue, 2);
    postStr += "&field3=";
    postStr += String(tdsValue, 0); // Send TDS value to ThingSpeak
    postStr += "\r\n\r\n";
    delay(500);
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    delay(500);
  }
  client.stop();
}
