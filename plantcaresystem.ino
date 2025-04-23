#define BLYNK_TEMPLATE_ID "TMPL2Tx4zB9xg" 
#define BLYNK_TEMPLATE_NAME "plant care" 
#define BLYNK_AUTH_TOKEN "liWUzhbq1hlRQcgmF0flsBj-a9oRegMz" 
 
#define BLYNK_PRINT Serial 
#include <WiFi.h> 
#include <BlynkSimpleEsp32.h>  
#include"DHT.h" 
 
#define SOIL_MOISTURE_PIN 34   // Analog pin for soil moisture sensor 
#define DHT_PIN 14 
#define THRESHOLD_MOISTURE 100 // Adjust this value based on your soil moisture sensor readings 
#define MAX_TEMP 40 
#define PUMP_PIN 25  // Digital pin for controlling the pump 
#define FAN_PIN 4 
#define PUMP_SWITCH V1  // Virtual pin for controlling the pump manually 
#define FAN_SWITCH V4 
 
char auth[] = BLYNK_AUTH_TOKEN;  // Replace with your Blynk auth token 
char ssid[] = "Ayman";   // Replace with your WiFi credentials 
char pass[] = "ayman123"; 
 
BlynkTimer timer; 
 
bool isPumpOn = false;  // Variable to track pump status 
bool isFanOn = false; 
 
DHT dht(DHT_PIN,DHT11); 
 
void sendSensorData() 
{ 
  int soilMoisture = analogRead(SOIL_MOISTURE_PIN); 
  Serial.print("Soil Moisture "); 
  Serial.println(soilMoisture); 
  // Map the analog sensor values to a percentage (0-100)  
  int soilMoisturePercentage = map(soilMoisture, 3500, 4095, 100, 0); 
  Serial.print("Soil Moisture Percentage "); 
  Serial.println(soilMoisturePercentage); 
  // Send soil moisture data to Blynk 
  Blynk.virtualWrite(V0, soilMoisturePercentage); 
 
  float temp= dht.readTemperature() ; 
  float humidity = dht.readHumidity(); 
  Serial.print("Temperature ");  Serial.println(temp); 
  Serial.print("Humidity ");  Serial.println(humidity); 
  Blynk.virtualWrite(V2,temp); 
  Blynk.virtualWrite(V3,humidity); 
 
  // Check if the pump should be on based on manual switch or soil moisture level 
  if (isPumpOn || soilMoisturePercentage < THRESHOLD_MOISTURE) 
  { 
    // Turn on the pump 
    digitalWrite(PUMP_PIN, HIGH); 
    // Check if the pump is turned on automatically (not manually) 
    if (!isPumpOn) { 
      // Send alert notification to Blynk app if the pump is turned on automatically 
      Blynk.logEvent("moisture_alert","Soil moisture is below the threshold!"); 
      Serial.println("Soil moisture is below the threshold!"); 
    } 
  } 
  else 
  { 
    // Turn off the pump only if it was not turned on manually 
    if (!isPumpOn) { 
      digitalWrite(PUMP_PIN, LOW); 
    } 
  } 
 
  if (isFanOn || temp > MAX_TEMP) 
  { 
    digitalWrite(FAN_PIN, HIGH); 
    // Check if the pump is turned on automatically (not manually) 
    if (!isFanOn) { 
      // Send alert notification to Blynk app if the pump is turned on automatically 
      Blynk.logEvent("heat_alert","The temperature is too high!"); 
      Serial.println("The temperature is too high!"); 
    } 
  } 
  else 
  { 
    // Turn off the pump only if it was not turned on manually 
    if (!isFanOn) { 
      digitalWrite(FAN_PIN, LOW); 
    } 
  } 
} 
 
BLYNK_WRITE(PUMP_SWITCH) 
{ 
  isPumpOn = param.asInt(); 
  if (isPumpOn) { 
    Serial.println("Pump manually turned ON"); 
  } else { 
    Serial.println("Pump manually turned OFF"); 
  } 
} 
 
BLYNK_WRITE(FAN_SWITCH) 
{ 
  isFanOn = param.asInt(); 
  if (isFanOn) { 
    Serial.println("Fan manually turned ON"); 
  } else { 
    Serial.println("Fan manually turned OFF"); 
  } 
} 
 
void setup() 
{ 
  Serial.begin(9600); 
  pinMode(PUMP_PIN, OUTPUT); // Set pump pin as an output 
  pinMode(DHT_PIN,INPUT); 
  pinMode(FAN_PIN,OUTPUT); 
  Blynk.begin(auth, ssid, pass); 
  dht.begin(); 
 
  timer.setInterval(3000L, sendSensorData); // Set the interval for checking soil moisture (every 3 seconds) 
 
  // Setup switch widget 
  Blynk.virtualWrite(PUMP_SWITCH, isPumpOn); 
  Blynk.syncVirtual(PUMP_SWITCH); 
  Blynk.virtualWrite(FAN_SWITCH, isFanOn); 
  Blynk.syncVirtual(FAN_SWITCH); 
} 
 
void loop() 
{ 
  Blynk.run(); 
  timer.run(); 
}