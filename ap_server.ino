/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

// Import required libraries
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C//3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// Replace with your network credentials
const char* ssid = "esp8266_a_point";
const char* password = "123456789";

// Set LED GPIO
const int ledPin = 2;
// Stores LED state
String ledState;
float tekanan;
int data3;
const char* teks="nama"; 
String DataTeks;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String getTemperature() {
  float temperature = 32.8;    //bme.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float temperature = 1.8 * bme.readTemperature() + 32;
  Serial.println(temperature);
  return String(temperature);
}
  
String getHumidity() {
  float humidity = 50.5;  //bme.readHumidity();
  Serial.println(humidity);
  return String(humidity);
}

String getPressure() {
  float pressure = 4.2;   //bme.readPressure()/ 100.0F;
  Serial.println(pressure);
  return String(pressure);
}

String AngkaAcak() {
  float pressure = tekanan;//random(1000);   //bme.readPressure()/ 100.0F;
  //tampilAngcak(pressure);
  Serial.println(pressure);
  return String(pressure);
}

// Replaces placeholder with LED state value
String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
    if(digitalRead(ledPin)){
      ledState = "OFF";
    }
    else{
      ledState = "ON";
    }
    Serial.print(ledState);
    return ledState;
  }
  else if (var == "TEMPERATURE"){
    return getTemperature();
  }
  else if (var == "HUMIDITY"){
    return getHumidity();
  }
  else if (var == "PRESSURE"){
    return AngkaAcak();   //getPressure();
  }  
  return "ada yang error";
}

void testdrawstyles(void) {
  display.clearDisplay();
  display.setTextSize(2);             // Draw 2X-scale text
  display.setCursor(0,1);    
  display.setTextColor(SSD1306_WHITE);
  display.println(F("BALAMANTRA   NODE")); //0xDEADBEEF, HEX);
  display.display();
  delay(2000);
}

void tampilAngcak(float angka){
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0); 
  display.print(F("Tekanan Batin:")); 
  
  display.setTextSize(2);             // Draw 2X-scale text
  display.setCursor(0,12);    
  display.setTextColor(SSD1306_WHITE);
  //display.print(F("0x")); 
  
 // String tes=String (angka);
  display.println(String (angka)); //0xDEADBEEF, HEX);
  display.setCursor(70,12);
  display.print(F("Bar")); 

  display.setTextSize(1);
  display.setCursor(0,32); 
  display.print(F("Pesan Teks:")); 

  display.setCursor(0,44); 
  display.print(String(data3));
  display.setCursor(20,44);
  display.print(String(DataTeks));
//
  display.display();
  delay(1000);
}

 
void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  //display.display();
  testdrawstyles();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

//  // Connect to Wi-Fi
//  WiFi.begin(ssid, password);
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(1000);
//    Serial.println("Connecting to WiFi..");
//  }
//
//  // Print ESP32 Local IP Address
//  Serial.println(WiFi.localIP());

 // Setting the ESP as an access point
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  //mencetak acces point
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
//  // Route to load style.css file
//  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
//    request->send(SPIFFS, "/style.css", "text/css");
//  });

  // Route to set GPIO to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin, LOW);  
    data3=data3+1;   
    if (data3>100){
      data3=0;
    }  
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to set GPIO to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin, HIGH); 
    data3=data3+1;   
    if (data3>100){
      data3=0;
    }
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", getTemperature().c_str());
  });
  
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", getHumidity().c_str());
  });
  
  server.on("/pressure", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", AngkaAcak().c_str()); 
  });

  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request){
    String inputMessage;
    String inputParam;
    if (request->hasParam(teks)) {
      inputMessage = request->getParam(teks)->value();
      inputParam = teks;
    }
    Serial.println(teks);
    DataTeks=inputMessage;
   request->send(SPIFFS, "/index.html", String(), false, processor);
   
  });
  
  server.serveStatic("/",SPIFFS,"/");
  
  // Start server
  server.begin();
  //server.handleClient();
}
 
void loop(){
  tampilAngcak(tekanan);
  //testdrawstyles();
  int data1 = random(1, 10000); // generate the integers
  float data2 = data1; // generate the numbers after the decimal point
  tekanan = data2 / 100; // sum the resulting values
  //tekanan = random(1000)/100;
  
}
