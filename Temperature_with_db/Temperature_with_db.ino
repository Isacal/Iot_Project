#include <ESP8266WiFi.h>
#include <LiquidCrystal.h>
#include <DHT.h>
#include <ESP8266HTTPClient.h>

// Pin definitions
#define DHTPIN D7          // Pin connected to the DHT sensor
#define DHTTYPE DHT11      // DHT 11 sensor

#define FAN_PIN D8         // Pin connected to the base of the transistor
#define TEMP_THRESHOLD 30  // Temperature threshold in Celsius
#define HYSTERESIS 2       // Hysteresis in Celsius
#define HYSTERESIS_OFF 1   // Hysteresis for turning off the fan in Celsius

DHT dht(DHTPIN, DHTTYPE);

// Initialize the library with the numbers of the interface pins
LiquidCrystal lcd(D1, D2, D3, D4, D5, D6);

// WiFi credentials
const char* ssid = "RAFIKI";
const char* password = "Rafiki!!";

// Server URL (Replace with your computer's IP address)
const char* serverUrl = "http://192.168.137.2/Iot_Project/index.php";  // Replace with your server IP

bool fanState = false; // To keep track of the fan's state

void setup() {
  Serial.begin(9600);
  dht.begin();
  lcd.begin(16, 2);       // Set up the LCD's number of columns and rows
  lcd.print("Temp: ");    // Print a message to the LCD

  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW); // Ensure fan is off at startup

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  // Read temperature as Celsius
  float temperature = dht.readTemperature();

  // Check if any reads failed and exit early (to try again)
  if (isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    lcd.setCursor(0, 1);
    lcd.print("Error         ");  // Print "Error" on the LCD
    delay(2000);                  // Wait for 2 seconds before trying again
    return;
  }

  // Display temperature on the LCD
  lcd.setCursor(6, 0);
  lcd.print(temperature);
  lcd.print(" C  ");

  // Control the fan based on temperature
  if (temperature >= TEMP_THRESHOLD && !fanState) {
    digitalWrite(FAN_PIN, HIGH);  // Turn fan on
    fanState = true;
  } else if (temperature <= TEMP_THRESHOLD - HYSTERESIS_OFF && fanState) {
    digitalWrite(FAN_PIN, LOW);   // Turn fan off
    fanState = false;
  }

  // Print temperature and fan state to Serial Monitor (for debugging)
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" C");
  Serial.print("Fan State: ");
  Serial.println(fanState ? "ON" : "OFF");

  // Send temperature and fan state data to the server
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient wifiClient;
    http.begin(wifiClient, serverUrl);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String postData = "temperature=" + String(temperature) + "&fanState=" + String(fanState ? "ON" : "OFF");
    int httpResponseCode = http.POST(postData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      Serial.print("Response: ");
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
  delay(5000); // Wait for 5 seconds between readings
}
