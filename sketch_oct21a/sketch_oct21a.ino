#define BLYNK_TEMPLATE_ID "TMPLtaRLfNfT"
#define BLYNK_DEVICE_NAME "Health monitor"
#define BLYNK_AUTH_TOKEN "fM2xLkOtBN-h8GQ4AK3x-ogmplWHTFYd"

constexpr uint8_t RST_PIN = D3; // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = D4;  // Configurable, see typical pin layout above

#include <SPI.h>
#include <MFRC522.h>

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

char auth[] = "fM2xLkOtBN-h8GQ4AK3x-ogmplWHTFYd"; // You should get Auth Token in the Blynk App.
char ssid[] = "A";                                // Your WiFi credentials.
char pass[] = "3115999Wild";

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;

String tag;

// REPLACE with your Domain name and URL path or IP address with path
const char *serverName = "http://healthmonitor1690.000webhostapp.com/post-esp-data.php";

// Keep this API Key value to be compatible with the PHP code provided in the project page.
// If you change the apiKeyValue value, the PHP file /post-esp-data.php also needs to have the same key
String apiKeyValue = "tPmAT5Ab3j7F9";

String status = "";

String sensorName = "MAX30100";
String sensorLocation = "Class";

// declare variable for average reading of heart rate and spo2
float avg_pulse = 0;
float avg_spo2 = 0;

// delacre array variable of 10 element to store the value of heart rate and spo2
float pulse_data[10];
float spo2_data[10];

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

#define REPORTING_PERIOD_MS 3000
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
  SPI.begin();     // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522

  // Initialize the PulseOximeter instance
  // Failures are generally due to an improper I2C wiring, missing power supply
  // or wrong target chip
  if (!pox.begin())
  {
    Serial.println("FAILED");
    for (;;)
      ;
  }
  else
  {
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
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  //  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  // timeClient.setTimeOffset(25200);
}

void hw_wdt_disable(){
  *((volatile uint32_t*) 0x60000900) &= ~(1); // Hardware WDT OFF
}

int count = -2;
String student_id = "";

void loop()
{
  pox.update();
  // timeClient.update();
  // time_t epochTime = timeClient.getEpochTime();
  timer.run(); // Initiates SimpleTimer
  if (count == -2){ 
  if (!rfid.PICC_IsNewCardPresent())
  {
    // write to LCD "Please scan your card"
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Please scan your card");

    return;
  }
  // Blynk.run();
  if (rfid.PICC_ReadCardSerial())
  {
    for (byte i = 0; i < 4; i++)
    {
      tag += rfid.uid.uidByte[i];
    }
    student_id = tag;
    count = 9;
    promptAfterScan();

  }

  }
   // use millis to loop displayAndSend() for 10 seconds
  //  for(int i; i < 4; i++) {
    // unsigned long startMillis = millis();
    // while (millis() - startMillis < 10000)
    // {
      // pox.update();
      if (count > -1){
      
      displayAndSend();
      } else if (count == -1) {
        send();
        displayStatus();
        count = -2;
        student_id = "";
      }
      
    // }
  // }

  tag = ""; // clear the tag variable
}

void displayStatus() {
  // display status on the LCD screen
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Status: ");
  lcd.print(status);
  lcd.setCursor(0, 1);
  // if the status is good, display "You are healthy"
  if (status == "Good") {
    lcd.print("You are healthy");
  } else {
    // if the status is bad, display "You are not healthy"
    lcd.print("You are not healthy");
  }

  delay(3000);
}

void promptAfterScan()
{
  Serial.println(tag);

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  // display on LCD "Welcome, tag number, please put your finger on the sensor"
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Welcome, student");
  lcd.setCursor(0, 1);
  lcd.print(tag);
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Please put finger");
  lcd.setCursor(0, 1);
  lcd.print("on the sensor");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
}

void getSendData()
{
  ;
}

void displayAndSend()
{
  if (millis() - tsLastReport > REPORTING_PERIOD_MS)
  {
    pox.update();

    // to computer serial monitor
    Serial.print("BPM: ");
    Serial.print(pox.getHeartRate());
    // blue.println("\n");
     pulse_data[count] = pox.getHeartRate();

    Serial.print("    SpO2: ");
    Serial.print(pox.getSpO2());
    Serial.print("%");
    Serial.println("\n");
    spo2_data[count] = pox.getSpO2();

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

    

    // Blynk.virtualWrite(V0, pox.getHeartRate() );
    // Blynk.virtualWrite(V1, pox.getSpO2());
    // Blynk.virtualWrite(V2, timeClient.getHours());
    // Blynk.virtualWrite(V3, timeClient.getMinutes());
    // unsigned long epochTime = timeClient.getEpochTime();
    // // struct tm *ptm = gmtime ((time_t *)&epochTime);
    // Blynk.virtualWrite(V4, ptm->tm_mday);
    // Blynk.virtualWrite(V5, ptm->tm_mon+1);
    // Blynk.virtualWrite(V6, ptm->tm_year+1900);
    // Blynk.virtualWrite(V7, timeClient.getSeconds());

    tsLastReport = millis();

    // delay(3000);

    
    // Send an HTTP POST request every 30 seconds
    // delay(5000);
    count--;
  }
}

// calculate average of an array and return the average
float average(float array[], int length)
{
  float sum = 0;
  for (int i = 0; i < length; i++)
  {
    sum += array[i];
  }
  return sum / length;
}

void send(){
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
      avg_pulse = average(pulse_data, 10);
      avg_spo2 = average(spo2_data, 10);

      if (avg_pulse >= 160 || avg_pulse <= 50 || avg_spo2 <= 70)
    {
      status = "Bad";
    }
    else if (avg_pulse >= 130 || avg_pulse <= 60 || avg_spo2 <= 85)
    {
      status = "Warning";
    }
    else
    {
      status = "Good";
    }

      // Your Domain name with URL path or IP address with path
      http.begin(client, serverName);

      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");

      // Prepare your HTTP POST request data
      String httpRequestData = "api_key=" + apiKeyValue + "&sensor=" + sensorName
                            + "&location=" + sensorLocation + "&pulse=" + String(avg_pulse)
                            + "&spo2=" + String(avg_spo2) + "&studentStatus=" + status + "&student_id=" + student_id + "";
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
}
