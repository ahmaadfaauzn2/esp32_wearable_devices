#include <Wire.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include "MS5611.h"
#include <WiFi.h> // For ESP32
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <time.h> // Make sure to include this for time manipulation functions
#include <HTTPClient.h>
#include <ArduinoJson.h>

// MS5611 Configuration
#define MS5611_ADDRESS 0x77
MS5611 ms5611(MS5611_ADDRESS);

// Function Declarations
String getCityName(); // Declaration of getCityName function
String uv_index(); // Declaration of uv_index function




// // WiFi network details
const char *ssid     = "Wifi rumah";
const char *password = "jokam354";

// // NTP Server details
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000); // Time offset (in seconds), update interval (in milliseconds)

// OpenWeatherMap API details
// const char *api_key = "f4f1a7372a6beff49da94f8249a3a339";
// // const char *city_name = "Bandung";
// const char *api_endpoint = "http://api.openweathermap.org/data/2.5/weather";

// U8G2 Display Configuration
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// Bitmaps for the display
static const unsigned char image_weather_temperature_bits[] PROGMEM = {
  0x38,0x00,0x44,0x40,0xd4,0xa0,0x54,0x40,0xd4,0x1c,0x54,0x06,0xd4,0x02,0x54,0x02,0x54,0x06,
  0x92,0x1c,0x39,0x01,0x75,0x01,0x7d,0x01,0x39,0x01,0x82,0x00,0x7c,0x00
};
static const unsigned char image_display_brightness_bits[] PROGMEM = {
  0x80,0x00,0x84,0x10,0x08,0x08,0xc0,0x01,0x31,0x46,0x12,0x24,0x08,0x08,0x08,0x08,0x08,0x08,
  0x12,0x24,0x31,0x46,0xc0,0x01,0x08,0x08,0x84,0x10,0x80,0x00,0x00,0x00
};

#define DEFAULT_SEA_LEVEL_PRESSURE 1010 // Example: Sarijadi, Bandung

String uv_index(); // Declaration of uv_index function

String UV = "0"; // Initialize UV variable

void initializeSensors();



// void uv_index () {
//   float sensorVoltage;
//   float sensorValue;
//   int UV_index;
//   String quality = "";
//   sensorValue = analogRead(2);
//   sensorVoltage = sensorValue * (3.3 / 1024.0);
//   UV_index = sensorVoltage/0.1;

//   // Print the sensor value and voltage to the serial monitor
//   Serial.print("Sensor Value: ");
//   Serial.print(sensorValue);
//   Serial.print(" - Sensor Voltage: ");
//   Serial.print(sensorVoltage);

//   // Calculate the UV index
//   if (UV_index < 2) {
//     quality = "Low";
//   } else if (UV_index < 5) {
//     quality = "Moderate";
//   } else if (UV_index < 7) {
//     quality = "High";
//   } else if (UV_index < 10) {
//     quality = "Very High";
//   } else {
//     quality = "Extreme";
//   }
  

//   //return quality;
//   Serial.print(" - UV Index: ");
//   Serial.println(quality);
  
  

// }

String readSensor() 
{
  String UVIndex = "00";
  int sensorValue = 0;

  sensorValue = analogRead(2); // Read the analog input from the UV sensor
  // Voltage in millivolts
  int voltage = (sensorValue * (3.3 / 1024.0))*1000;

  //Print the sensor value and voltage to the serial monitor
  Serial.print("Sensor Value: ");
  Serial.print(sensorValue);

  // Print the millivoltage value
  Serial.print(" - Sensor Voltage: ");
  Serial.print(voltage);

  // Calculate the UV index
  if (voltage < 50) 
  {
    UVIndex = "00";
  }
  else if (voltage >50 && voltage<=277)
  {
    UVIndex = "00";
  } 
  else if (voltage >227 && voltage<=318)
  {
    UVIndex = "01";
  }
  else if (voltage >318 && voltage<=408)
  {
    UVIndex = "02";
  }
  else if (voltage >408 && voltage<=503)
  {
    UVIndex = "03";
  }
  else if (voltage >503 && voltage<=606)
  {
    UVIndex = "04";
  }
  else if (voltage >606 && voltage<=696)
  {
    UVIndex = "05";
  }
  else if (voltage >696 && voltage<=795)
  {
    UVIndex = "06";
  }
  else if (voltage >795 && voltage<=881)
  {
    UVIndex = "07";
  }
  else if (voltage >881 && voltage<=976)
  {
    UVIndex = "08";
  }
  else if (voltage >976 && voltage<=1079)
  {
    UVIndex = "09";
  }
  else if (voltage >1079 && voltage<=1170)
  {
    UVIndex = "10";
  }
  else if (voltage >1170)
  {
    UVIndex = "11";
  }
  // Print the UV index to the serial monitor
  Serial.print(" - UV Index: ");
  Serial.println(UVIndex);

  return UVIndex;
}

void setup() {
  Serial.begin(9600);
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  
  // Initialize NTP client
  timeClient.begin();

  Wire.begin(8, 9);
  
  // Initialize MS5611 and Display
  initializeSensors();
}

void loop() {
  timeClient.update(); // Update time from NTP server

  // uv_index();

 
  // Call the uv_index function and store its result
  UV = readSensor();

  // // Adjust for timezone UTC+7:00 (25200 seconds)
  long epochTime = timeClient.getEpochTime() + 25200; // Add 7 hours to UTC
  struct tm *ptm = localtime(&epochTime); // Convert adjusted epoch time to local time structure

  // // Buffers for date and time strings
  char dateStr[30];
  char timeStr[20];

  // // Fetch day of the week and month as strings
  const char* daysOfWeek[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  const char* monthsOfYear[12] = {"January", "February", "March", "April", "May", "June", 
                                  "July", "August", "September", "October", "November", "December"};
  
  // // Construct the date string manually
  snprintf(dateStr, sizeof(dateStr), "%s, %s %02d %04d", 
           daysOfWeek[ptm->tm_wday], 
           monthsOfYear[ptm->tm_mon], 
           ptm->tm_mday, 
           1900 + ptm->tm_year);

  // Format time string with AM/PM
  strftime(timeStr, sizeof(timeStr), "%I:%M:%S %p", ptm); // e.g., "09:58:58 PM"

  // Read sensors
  ms5611.read();
  float temperature = ms5611.getTemperature();
  float pressure = ms5611.getPressure();
  float altitude = 44330 * (1.0 - pow(pressure / DEFAULT_SEA_LEVEL_PRESSURE, 0.1903));

  // Get city name
  // String cityName = getCityName();

  // Prepare the display buffer
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x12_tr);

  // // Displaying the manually formatted date and time
  u8g2.drawStr(0, 15, dateStr); // Display formatted date
  u8g2.drawStr(0, 28, timeStr); // Display formatted time

  // Display city name
  // u8g2.drawStr(0, 40, cityName.c_str()); // Display city name

  // Additional display updates for sensors, etc.
  char buffer[32]; // Buffer for dynamic text
  sprintf(buffer, "%.2fC", temperature);
  u8g2.drawStr(22, 59, buffer);

  sprintf(buffer, "%.2f hPa", pressure);
  u8g2.drawStr(59, 51, buffer);

  sprintf(buffer, "%.2f m", altitude);
  u8g2.drawStr(61, 61, buffer);

  // Display UV index
  // char buffer[32]; // Buffer for dynamic text
  sprintf(buffer, "UVI %s", UV.c_str());
  u8g2.drawStr(63, 37, buffer); // Ensure this matches your desired position on the OLED

  // Draw icons for temperature and brightness
  u8g2.drawXBM(3, 46, 16, 16, image_weather_temperature_bits);
  u8g2.drawXBM(95, 13, 15, 16, image_display_brightness_bits);
  u8g2.drawFrame(1, 41, 126, 1);

  u8g2.sendBuffer(); // Send the buffer to the display

  delay(1000); // Refresh every second
}

void initializeSensors() {
  if (!ms5611.begin()) {
    Serial.println("MS5611 not found. Halt.");
    while (1); // Stop execution
  }
  Serial.println("MS5611 found.");
  
  u8g2.begin();
}






  // Convert the voltage to milivolts
  
  






// String getCityName()
// {
//   // Construct the URL for the API request
//   String url = String(api_endpoint) + "?q=" + String(city_name) + "&appid=" + String(api_key);

//   // Make HTTP GET request to OpenWeatherMap API
//   HTTPClient http;
//   http.begin(url);
//   int httpCode = http.GET();

//   if (httpCode == HTTP_CODE_OK)
//   {
//     // Parse JSON response
//     DynamicJsonDocument doc(1024);
//     DeserializationError error = deserializeJson(doc, http.getString());

//     if (!error)
//     {
//       String cityName = doc["name"].as<String>(); // Extract city name
//       return cityName;
//     }
//     else
//     {
//       Serial.println("Failed to parse JSON response");
//       return "City Not Found";
//     }
//   }
//   else
//   {
//     Serial.println("Failed to connect to OpenWeatherMap API");
//     return "Connection Error";
//   }
// }
