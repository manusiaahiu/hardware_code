#include <dht.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(7,8,9,10,11,12);

dht DHT;

#define DHT11_PIN 5

int sensor_pin = A0; // Soil Sensor input at Analog PIN A0
int output_value ;
int relayPin = 6;

void setup(){
  Serial.begin(9600); // Memulai komunikasi serial dengan baud rate 9600
  lcd.begin(16, 2);
  pinMode(sensor_pin, INPUT);
  pinMode(relayPin, OUTPUT);
}

void loop(){
  int chk = DHT.read11(DHT11_PIN);
  
  // Menampilkan suhu dan kelembapan ke Serial Monitor
  Serial.print("Temperature: ");
  Serial.print(DHT.temperature);
  Serial.print(" °C, Humidity: ");
  Serial.print(DHT.humidity);
  Serial.println("%");

  // Menampilkan suhu dan kelembaban di LCD
  lcd.setCursor(0,0); 
  lcd.print("Temp: ");
  lcd.print(DHT.temperature);
  lcd.print((char)223);  
  
  lcd.setCursor(0,1); 
  lcd.print("Humidity: ");
  lcd.print(DHT.humidity);
  lcd.print("%");
  delay(2000);
  
  // Menampilkan nilai kelembaban tanah di Serial Monitor
  output_value= analogRead(sensor_pin);
  output_value = map(output_value,550,10,0,100);
  Serial.print("Moisture: ");
  Serial.print(output_value);
  Serial.println("%");

  // Menampilkan nilai kelembaban tanah di LCD
  lcd.setCursor(23,0);
  lcd.autoscroll();
  lcd.print("Moisture: ");
  lcd.print(output_value);
  lcd.print("%");
  
  lcd.setCursor(23,1);
  if(output_value < 30){
    digitalWrite(relayPin, LOW);
    lcd.print("Motor ON");
    Serial.println("Tanah Kering");
  }
  else if(output_value >= 30 && output_value < 60){
    digitalWrite(relayPin, HIGH);
    lcd.print("Motor OFF");
    Serial.println("Tanah Basah");
  }
  else {
    digitalWrite(relayPin, HIGH);
    lcd.print("Motor OFF"); 
    Serial.println("Tanah Kelebihan Air");
  }
  
  delay(100); // Menambahkan jeda antara pembacaan
  lcd.noAutoscroll();
  lcd.clear();
}

