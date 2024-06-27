#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

#define DHTPIN 19 // D19 
#define DHTTYPE DHT11 
DHT dht11(DHTPIN, DHTTYPE);

#define SOIL_SENSOR_PIN 34 // Replace with the GPIO number for the soil sensor on the ESP32
#define RELAY_PIN 5        // Pin connected to the relay

const char* ssid = "titituit"; 
const char* password = "uwauuwau"; 

String dht11URL = "http://192.168.43.189/dht11_project/connect.php";
String soilURL = "http://192.168.43.189/consoil/koneksi.php";

int temperature = 0;
int humidity = 0;
int soilMoisture = 0;
String soilStatus = "";

void setup() {
  Serial.begin(115200);
  pinMode(SOIL_SENSOR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  
  dht11.begin(); 
  connectWiFi();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  Load_DHT11_Data();
  Load_SoilMoisture_Data();

  // Send DHT11 data
  String dht11PostData = "temperature=" + String(temperature) + "&humidity=" + String(humidity);
  sendData(dht11URL, dht11PostData);

  // Send Soil Moisture data
  String soilPostData = "kelmbapan=" + String(soilMoisture) + "&status=" + soilStatus;
  sendData(soilURL, soilPostData);

  delay(10000); // Add delay between readings and HTTP requests
}

void Load_DHT11_Data() {
  temperature = dht11.readTemperature(); // Celsius
  humidity = dht11.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    temperature = 0;
    humidity = 0;
  }

  Serial.printf("Temperature: %d °C\n", temperature);
  Serial.printf("Humidity: %d %%\n", humidity);
}

void Load_SoilMoisture_Data() {
  int output_value = analogRead(SOIL_SENSOR_PIN);
  // Adjust the values below according to your sensor's specifications
  // Assuming 0 is wet and 4095 is dry for ESP32's 12-bit ADC resolution
  output_value = map(output_value, 0, 4095, 100, 0); // Reverse map analog reading to percentage
  soilMoisture = constrain(output_value, 0, 100); // Ensure the value is between 0 and 100

  Serial.print("Moisture: ");
  Serial.print(soilMoisture);
  Serial.println("%");

  if (soilMoisture < 30) {
    digitalWrite(RELAY_PIN, LOW);
    soilStatus = "Tanah Kering";
    Serial.println(soilStatus);
  } else if (soilMoisture >= 30 && soilMoisture < 60) {
    digitalWrite(RELAY_PIN, HIGH);
    soilStatus = "Tanah Basah";
    Serial.println(soilStatus);
  } else {
    digitalWrite(RELAY_PIN, HIGH);
    soilStatus = "Tanah Kelebihan Air";
    Serial.println(soilStatus);
  }
}

void connectWiFi() {
  WiFi.mode(WIFI_OFF);
  delay(1000);
  WiFi.mode(WIFI_STA);
  
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
    
  Serial.print("Connected to: "); Serial.println(ssid);
  Serial.print("IP address: "); Serial.println(WiFi.localIP());
}

void sendData(String url, String postData) {
  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  int httpCode = http.POST(postData);
  String payload = "";

  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      payload = http.getString();
      Serial.println(payload);
    } else {
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);
    }
  } else {
    Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  
  http.end();  // Close connection

  Serial.print("URL: "); Serial.println(url); 
  Serial.print("Data: "); Serial.println(postData);
  Serial.print("httpCode: "); Serial.println(httpCode);
  Serial.print("payload: "); Serial.println(payload);
  Serial.println("--------------------------------------------------");
}
