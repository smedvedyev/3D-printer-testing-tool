#include <Wire.h>
#include <SPI.h>
#include <WEMOS_SHT3X.h>
#include "MPU9250.h"
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#if defined(ESP8266)
#include <ESP8266WebServer.h>
#else
#include <WebServer.h>
#endif
#include <PubSubClient.h>

const char *mqtt_server = "test.mosquitto.org";
const char *ssid = "12connect";
const char *password = "";

WiFiClient espClient;
PubSubClient client(espClient);

struct SensorData
{
  float temperature;
  float humidity;
  float roll;
  float pitch;
  float gyroX;
  float gyroY;
  float gyroZ;
};

const int INTERVAL = 200;
const int RECONNECT_INTERVAL = 5000;
const int WIFI_CONNECT_INTERVAL = 5000;

// initialize thresholds
const float TEMPERATURE_THRESHOLD = 1.0;
const float HUMIDITY_THRESHOLD = 5.0;
const int LEVELING_THRESHOLD = 5;
const float SIGNIFICANCE_THRESHOLD = 2.0;

// moduldo variables for the bed leveling
const int NUM_SAMPLES = 10;
float rollSamples[NUM_SAMPLES];
float pitchSamples[NUM_SAMPLES];
int sampleIndex = 0;

SensorData bedLevelData;

bool isSignificantChange(SensorData &currentData, SensorData &previousData, float threshold)
{
  return (abs(currentData.roll - previousData.roll) > threshold ||
          abs(currentData.pitch - previousData.pitch) > threshold);
}

unsigned long previousTime = 0;
float elapsedTime, elapsedTimeTotal = 0;

const unsigned long SENSOR_UPDATE_INTERVAL = 100;

SHT3X sht(0x45);
MPU9250 IMU(Wire, 0x68);
void callback(char *topic, byte *payload, unsigned int length)
{ // callback includes topic and payload ( from which (topic) the payload is comming)
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] \n");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  if ((char)payload[0] == '1' && (char)payload[1] == 'N') // on
  {
  }
  Serial.println();
}

void reconnect()
{
  static unsigned long lastReconnectAttempt = 0;
  static unsigned long lastWiFiConnectAttempt = 0;

  if (!client.connected())
  {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > RECONNECT_INTERVAL)
    {
      lastReconnectAttempt = now;

      if (!WiFi.isConnected())
      {
        if (now - lastWiFiConnectAttempt > WIFI_CONNECT_INTERVAL)
        {
          lastWiFiConnectAttempt = now;
          Serial.println("Connecting to WiFi...");
          WiFi.begin(ssid, password);
        }
      }
      else
      {
        Serial.println("Attempting MQTT connection...");
        if (client.connect("ESP32_clientID"))
        {
          Serial.println("connected");
          client.publish("studentinc/acme/environment", "Nodemcu connected to MQTT");
          client.subscribe("studentinc/acme/environment");
        }
        else
        {
          Serial.print("failed, rc=");
          Serial.print(client.state());
          Serial.println(" try again in 5 seconds");
        }
      }
    }
  }
}

void connectmqtt()
{
  client.connect("ESP32_clientID"); // ESP will connect to mqtt broker with clientID

  Serial.println("connected to MQTT");
  // Once connected, publish an announcement...

  // ... and resubscribe
  client.subscribe("studentinc/acme/environment"); // topic=Demo

  if (!client.connected())
  {
    reconnect();
  }
}

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  WiFi.begin(ssid, password);
  int status;
  unsigned long startTime = millis();
  unsigned long timeout = 5000; // 5 seconds

  while ((status = IMU.begin()) < 0)
  {
    if (millis() - startTime > timeout)
    {
      Serial.println("IMU initialization unsuccessful");
      Serial.println("Please check the connections!");
      // Block indefinitely in case of failure
      while (1)
      {
      }
    }
  }

  Serial.println("Connecting to WiFi...");
  unsigned long start = millis();
  while (!WiFi.isConnected() && millis() - start < WIFI_CONNECT_INTERVAL)
  {
    // Wait for a WiFi connection or a timeout
    delay(100);
  }

  if (WiFi.isConnected())
  {
    Serial.println("connected");
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
    connectmqtt();
  }
  else
  {
    Serial.println("WiFi connection failed.");
  }
}

void readIMUSensorData()
{
  IMU.readSensor();

  float ax = IMU.getAccelX_mss();
  float ay = IMU.getAccelY_mss();
  float az = IMU.getAccelZ_mss();

  // roll and pitch angles are calculated from the accelerometer readings
  float newRoll = atan2(ay, az) * 180.0 / PI;
  float newPitch = atan2(-ax, sqrt(ay * ay + az * az)) * 180.0 / PI;

  // Add new roll and pitch angles to samples array
  rollSamples[sampleIndex] = newRoll;
  pitchSamples[sampleIndex] = newPitch;

  // Update sample index (Modulo technique)
  sampleIndex = (sampleIndex + 1) % NUM_SAMPLES;

  // Calculate moving average of roll and pitch angles
  bedLevelData.roll = 0;
  bedLevelData.pitch = 0;
  for (int i = 0; i < NUM_SAMPLES; i++)
  {
    bedLevelData.roll += rollSamples[i];
    bedLevelData.pitch += pitchSamples[i];
  }
  bedLevelData.roll /= NUM_SAMPLES;
  bedLevelData.pitch /= NUM_SAMPLES;

  bedLevelData.gyroX = IMU.getGyroX_rads();
  bedLevelData.gyroY = IMU.getGyroY_rads();
  bedLevelData.gyroZ = IMU.getGyroZ_rads();
}

void updateAngles(unsigned long currentTime)
{
  elapsedTime = (currentTime - previousTime) / 1000.0;
  previousTime = currentTime;

  bedLevelData.roll += bedLevelData.gyroX * elapsedTime;
  bedLevelData.pitch += bedLevelData.gyroY * elapsedTime;
}

void printSensorData()
{
  client.publish("studentinc/acme/environment/pitch", String(bedLevelData.pitch).c_str());
  client.publish("studentinc/acme/environment/roll", String(bedLevelData.roll).c_str());
  Serial.print("Roll: ");
  Serial.print(bedLevelData.roll);
  
  Serial.print("\tPitch: ");
  Serial.print(bedLevelData.pitch);
  Serial.print("\tGyro X: ");
  Serial.print(bedLevelData.gyroX);
  Serial.print("\tGyro Y: ");
  Serial.print(bedLevelData.gyroY);
  Serial.print("\tGyro Z: ");
  Serial.println(bedLevelData.gyroZ);
}

void checkBedLevel()
{
  if (abs(bedLevelData.roll) < LEVELING_THRESHOLD && abs(bedLevelData.pitch) < LEVELING_THRESHOLD)
  {
    Serial.println("The bed is level.");
  }
  else if (bedLevelData.roll > LEVELING_THRESHOLD || bedLevelData.roll < -LEVELING_THRESHOLD)
  {
    Serial.println("The bed is tilted along the X-axis.");
  }
  else if (bedLevelData.pitch > LEVELING_THRESHOLD || bedLevelData.pitch < -LEVELING_THRESHOLD)
  {
    Serial.println("The bed is tilted along the Y-axis.");
  }
}

SensorData readSensorData()
{
  byte error = sht.get();
  SensorData tempHumidityData;

  unsigned long startTime = millis();
  while (error != 0)
  {
    Serial.println("Error reading from SHT3X");
    if (millis() - startTime > INTERVAL)
    {
      Serial.println("Sensor reading error, please check the connections!");
      tempHumidityData.temperature = -1.0f; // Invalid temperature value
      tempHumidityData.humidity = -1.0f;    // Invalid humidity value
      return tempHumidityData;
    }
    error = sht.get();
  }

  tempHumidityData.temperature = sht.cTemp;
  tempHumidityData.humidity = sht.humidity;

  return tempHumidityData;
}

// Display sensor data
void displaySensorData(SensorData &data, float &prevTemp, float &prevHum)
{
  // Display the temperature and humidity values only if there is a difference of more than 1.0
  if (abs(data.temperature - prevTemp) > TEMPERATURE_THRESHOLD || abs(data.humidity - prevHum) > HUMIDITY_THRESHOLD)
  {
    Serial.print("Temperature: ");
    Serial.print(data.temperature);
    Serial.print(" °C, Humidity: ");
    Serial.print(data.humidity);
    Serial.println(" %");
    client.publish("studentinc/acme/environment/temp", String(data.temperature).c_str());
    client.publish("studentinc/acme/environment/hum", String(data.humidity).c_str());

    prevTemp = data.temperature;
    prevHum = data.humidity;
  }
}

void loop()
{
  reconnect();
  unsigned long currentTime = millis();
  static unsigned long previousTime = 0;
  static float prevTemp = 0.0f;
  static float prevHum = 0.0f;
  static SensorData previousBedLevelData;

  if (currentTime - previousTime >= INTERVAL)
  {
    previousTime = currentTime;
    readIMUSensorData();
    updateAngles(currentTime);
    SensorData data = readSensorData();
    displaySensorData(data, prevTemp, prevHum);

    if (isSignificantChange(bedLevelData, previousBedLevelData, SIGNIFICANCE_THRESHOLD))
    {

      printSensorData();
      checkBedLevel();

      previousBedLevelData = bedLevelData;
    }
  }
}