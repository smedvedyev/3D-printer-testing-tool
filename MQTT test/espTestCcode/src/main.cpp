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

const char* mqtt_server = "test.mosquitto.org"; //mqtt server
const char* ssid = "Xiaomi 12";
const char* password = "qwerty123456!?1";

WiFiClient espClient;
PubSubClient client(espClient); //lib required for mqtt
int YELLOWLED = GPIO_NUM_27;
int REDLED = GPIO_NUM_26;

void callback(char* topic, byte* payload, unsigned int length) {   //callback includes topic and payload ( from which (topic) the payload is comming)
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] \n");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  if ((char)payload[0] == '1' && (char)payload[1] == 'N') //on
  {
    digitalWrite(YELLOWLED, HIGH);
    Serial.println("on");
    client.publish("outTopic", "YELLOWLED turned ON");
  }
  else if ((char)payload[0] == '1' && (char)payload[1] == 'F' && (char)payload[2] == 'F') //off
  {
    digitalWrite(YELLOWLED, LOW);
    Serial.println(" off");
    client.publish("outTopic", "YELLOWLED turned OFF");
  }
  else if ((char)payload[0] == '2' && (char)payload[1] == 'N') //on
  {
    digitalWrite(REDLED, HIGH);
    Serial.println("on");
    client.publish("outTopic", "Red turned ON");
  }
  else if ((char)payload[0] == '2' && (char)payload[1] == 'F' && (char)payload[2] == 'F') //off
  {
    digitalWrite(REDLED, LOW);
    Serial.println(" off");
    client.publish("outTopic", "Red turned OFF");
  }
  Serial.println();
}

void reconnect() {
  while (!client.connected()) {
    while(WiFi.isConnected() == false)
    {
      WiFi.begin(ssid, password);
      Serial.println("Connecting to WiFi..");
      delay(5000);
    }
    Serial.println("Attempting MQTT connection...");
    if (client.connect("ESP32_clientID")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "Nodemcu connected to MQTT");
      // ... and resubscribe
      client.subscribe("inTopic");

    } else {
      Serial.print("faiYELLOWLED, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
      reconnect();
    }
  }
}


void connectmqtt()
{
  client.connect("ESP32_clientID");  // ESP will connect to mqtt broker with clientID
  {
    Serial.println("connected to MQTT");
    // Once connected, publish an announcement...

    // ... and resubscribe
    client.subscribe("studentinc/acme/xyzManipulator"); //topic=Demo
    client.publish("SM",  "connected to MQTT");

    if (!client.connected())
    {
      reconnect();
    }
  }
}

void setup()
{
  Serial.begin(9600);
  pinMode(YELLOWLED, OUTPUT);
  pinMode(REDLED, OUTPUT);
  digitalWrite(YELLOWLED, LOW);
  WiFi.begin(ssid, password);
  Serial.println("connected");
  client.setServer(mqtt_server, 1883);//connecting to mqtt server
  client.setCallback(callback);
  delay(5000);
  connectmqtt();
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
    
  }

  client.loop();
}

