#define BLYNK_TEMPLATE_ID "TMPLtaRLfNfT"
#define BLYNK_DEVICE_NAME "Health monitor"
#define BLYNK_AUTH_TOKEN "fM2xLkOtBN-h8GQ4AK3x-ogmplWHTFYd"

#include <time.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include <ESP8266HTTPClient.h>
  #include <WiFiClient.h>

#include <Wire.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "MAX30100_PulseOximeter.h"
// #include <SimpleTimer.h>
// #include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

char auth[] = "fM2xLkOtBN-h8GQ4AK3x-ogmplWHTFYd";             // You should get Auth Token in the Blynk App.
char ssid[] = "A";                                     // Your WiFi credentials.
char pass[] = "3115999Wild";

// REPLACE with your Domain name and URL path or IP address with path
const char* serverName = "http://healthmonitor1690.000webhostapp.com/post-esp-data.php";

// Keep this API Key value to be compatible with the PHP code provided in the project page. 
// If you change the apiKeyValue value, the PHP file /post-esp-data.php also needs to have the same key 
String apiKeyValue = "tPmAT5Ab3j7F9";

String sensorName = "MAX30100";
String sensorLocation = "Class";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

#define REPORTING_PERIOD_MS     3000
SimpleTimer timer;

PulseOximeter pox;
uint32_t tsLastReport = 0;

void onBeatDetected()
{
  ;
}

void setup()
{
  Serial.begin(9600);
  Wire.begin(D2, D1);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("Welcome To");
  lcd.setCursor(0, 1);
  lcd.print("JustDoElectronic");
  delay(3000);
  lcd.clear();
  lcd.setCursor(6, 0);
  lcd.print("IoT");
  lcd.setCursor(1, 1);
  lcd.print("Pulse Oximeter");
  delay(3000);
  lcd.clear();
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  Serial.print("Initializing pulse oximeter..");
  


  // Initialize the PulseOximeter instance
  // Failures are generally due to an improper I2C wiring, missing power supply
  // or wrong target chip
  if (!pox.begin()) {
    Serial.println("FAILED");
    for (;;);
  } else {
    Serial.println("SUCCESS");
    digitalWrite(1, HIGH);
  }
  pox.setIRLedCurrent(MAX30100_LED_CURR_24MA);

  // Register a callback for the beat detection
  pox.setOnBeatDetectedCallback(onBeatDetected);

  timer.setInterval(1000L, getSendData);

  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

   timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(25200);
}

void loop()
{
  
  
  timeClient.update();

  time_t epochTime = timeClient.getEpochTime();

  timer.run(); // Initiates SimpleTimer
  Blynk.run();
  // Make sure to call update as fast as possible
  pox.update();
  
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {


    // to computer serial monitor
    Serial.print("BPM: ");
    Serial.print(pox.getHeartRate());
    //blue.println("\n");

    Serial.print("    SpO2: ");
    Serial.print(pox.getSpO2());
    Serial.print("%");
    Serial.println("\n");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("BPM: ");
    lcd.setCursor(10, 0);
    lcd.print(pox.getHeartRate());
    lcd.setCursor(0, 1);
    lcd.print("SpO2:");
    lcd.setCursor(10, 1);
    lcd.print(pox.getSpO2());
    lcd.setCursor(14, 1);
    lcd.print("%");

    Blynk.virtualWrite(V0, pox.getHeartRate() );
    Blynk.virtualWrite(V1, pox.getSpO2());
    Blynk.virtualWrite(V2, timeClient.getHours());
    Blynk.virtualWrite(V3, timeClient.getMinutes());
    //unsigned long epochTime = timeClient.getEpochTime();
    struct tm *ptm = gmtime ((time_t *)&epochTime);
    Blynk.virtualWrite(V4, ptm->tm_mday);
    Blynk.virtualWrite(V5, ptm->tm_mon+1);
    Blynk.virtualWrite(V6, ptm->tm_year+1900);
    Blynk.virtualWrite(V7, timeClient.getSeconds());

    tsLastReport = millis();

  if(WiFi.status()== WL_CONNECTED){
    WiFiClient client;
    HTTPClient http;
    
    // Your Domain name with URL path or IP address with path
    http.begin(client, serverName);
    
    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    // Prepare your HTTP POST request data
    String httpRequestData = "api_key=" + apiKeyValue + "&sensor=" + sensorName
                          + "&location=" + sensorLocation + "&value1=" + String(pox.getHeartRate())
                          + "&value2=" + String(pox.getSpO2()) +  "";
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);
    
    // You can comment the httpRequestData variable above
    // then, use the httpRequestData variable below (for testing purposes without the BME280 sensor)
    //String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=BME280&location=Office&value1=24.75&value2=49.54&value3=1005.14";

    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);
     
    // If you need an HTTP request with a content type: text/plain
    //http.addHeader("Content-Type", "text/plain");
    //int httpResponseCode = http.POST("Hello, World!");
    
    // If you need an HTTP request with a content type: application/json, use the following:
    //http.addHeader("Content-Type", "application/json");
    //int httpResponseCode = http.POST("{\"value1\":\"19\",\"value2\":\"67\",\"value3\":\"78\"}");
        
    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
  //Send an HTTP POST request every 30 seconds
  //delay(5000);  
  }

  
}

void getSendData()
{
  ;
}
