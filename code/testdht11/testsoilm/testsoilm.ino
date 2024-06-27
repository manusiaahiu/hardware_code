#include <WiFi.h>
#include <HTTPClient.h>

#define SOIL_SENSOR_PIN 18 // Replace with the GPIO number for the soil sensor on the ESP32
#define RELAY_PIN 4        // Pin connected to the relay

const char* ssid = "coconut";       // Your Wi-Fi SSID
const char* password = "natadecoco"; // Your Wi-Fi password

const char* serverName = "http://192.168.43.189/consoil/koneksi.php"; // URL to the PHP script

void setup() {
  Serial.begin(115200); // Start the Serial communication with baud rate 115200
  pinMode(SOIL_SENSOR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi");
}

void loop() {
  // Display soil moisture value on the Serial Monitor
  int output_value = analogRead(SOIL_SENSOR_PIN);
  output_value = map(output_value, 550, 10, 0, 100); // Map analog reading to percentage
  Serial.print("Moisture: ");
  Serial.print(output_value);
  Serial.println("%");

  String status;

  // Logic to control the relay based on soil moisture
  if (output_value < 30) {
    digitalWrite(RELAY_PIN, LOW);
    status = "Tanah Kering";
    Serial.println(status);
  }
  else if (output_value >= 30 && output_value < 60) {
    digitalWrite(RELAY_PIN, HIGH);
    status = "Tanah Basah";
    Serial.println(status);
  }
  else {
    digitalWrite(RELAY_PIN, HIGH);
    status = "Tanah Kelebihan Air";
    Serial.println(status);
  }

  // Send data to the server
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String httpRequestData = "kelmbapan=" + String(output_value) + "&status=" + status;
    int httpResponseCode = http.POST(httpRequestData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }

  delay(10000); // Add delay between readings and HTTP requests
}
