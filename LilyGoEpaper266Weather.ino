#include <GxEPD.h>
#include <GxDEPG0266BN/GxDEPG0266BN.h>    // 2.66" b/w   form DKE GROUP

// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define EPD_SCLK  18  // SPI Clock (SCK)
#define EPD_MISO  19  // Master In Slave Out (MISO) - Not needed for most e-paper displays
#define EPD_MOSI  23  // Master Out Slave In (MOSI)
#define EPD_CS    5   // Chip Select (CS)
#define EPD_DC    17  // Data/Command (DC)
#define EPD_RST   16  // Reset (RST)
#define EPD_BUSY  4   // Busy (BUSY)

GxIO_Class io(SPI,  EPD_CS, EPD_DC,  EPD_RST);
GxEPD_Class display(io, EPD_RST, EPD_BUSY);

const char* ssid = "YOUR_WIFI_NAME";       // Replace with your WiFi name
const char* password = "YOUR_WIFI_PASSWORD"; // Replace with your WiFi password
const char* apiUrl = "https://api.open-meteo.com/v1/forecast?latitude=41.4141&longitude=-3.7373&timezone=Europe%2FBerlin&current=temperature_2m,wind_speed_10m,relative_humidity_2m&forecast_days=1";

String formattedTime;
float temperature;
float windSpeed;
int humidity;

void setup(void)
{
    Serial.begin(115200);
    Serial.println();
    Serial.println("setup");

    //connect to Wifi
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nConnected to WiFi!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    fetchWeatherData();
    updateDisplay();

    // Sleep for 600,000,000 microseconds (600 seconds) = 10 minutes
    esp_sleep_enable_timer_wakeup(600000000);  
    esp_deep_sleep_start();  // Start deep sleep mode
}

void loop()
{}

void fetchWeatherData() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(apiUrl);
        int httpCode = http.GET();

        if (httpCode > 0) { // Check if request was successful
            String payload = http.getString();
            Serial.println("Received JSON:");
            Serial.println(payload);

            // Parse JSON
            StaticJsonDocument<512> doc;
            DeserializationError error = deserializeJson(doc, payload);

            if (!error) {
                temperature = doc["current"]["temperature_2m"];
                windSpeed = doc["current"]["wind_speed_10m"];
                humidity = doc["current"]["relative_humidity_2m"];
                formattedTime = String(doc["current"]["time"]);
                formattedTime.replace("T", " "); 

                Serial.print("Temperature: ");
                Serial.print(temperature);
                Serial.println(" °C");

                Serial.print("Wind Speed: ");
                Serial.print(windSpeed);
                Serial.println(" km/h");

                Serial.print("Humidity: ");
                Serial.print(humidity);
                Serial.println(" %");
            } else {
                Serial.println("JSON parsing failed!");
            }
        } else {
            Serial.print("HTTP request failed. Error code: ");
            Serial.println(httpCode);
        }

        http.end();
    } else {
        Serial.println("WiFi not connected!");
    }
}

void updateDisplay(){
      SPI.begin(EPD_SCLK, EPD_MISO, EPD_MOSI);
    display.init(); // enable diagnostic output on Serial

    display.setRotation(1);
    display.fillScreen(GxEPD_WHITE);
    display.setTextColor(GxEPD_BLACK);
    
    // Temperature
    display.setFont(&FreeMonoBold18pt7b);
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(0, 60);
    display.print("Temp. ");
    display.print(temperature, 1); // 1 decimal place
    display.println(" C");

    // Humidity
    display.setFont(&FreeMonoBold12pt7b);
    display.print("Hum.  ");
    display.print(humidity);
    display.println(" %");

    // Wind Speed
    display.print("Vie.  ");
    display.print(windSpeed, 1);  // 1 decimal place
    display.println(" km/h");
    display.setFont(&FreeMonoBold9pt7b);

    // Last Actualization Time
    display.print("Act: ");
    display.println(formattedTime);  
    display.update();
}



