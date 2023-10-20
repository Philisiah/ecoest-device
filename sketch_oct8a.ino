#include <DHT.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

// Replace these with your WiFi and MQTT credentials
const char* ssid = "Seline";
const char* password = "Seline21";
const char* mqtt_server = "91.121.93.94";
const int mqtt_port = 1883;

const char* mqtt_topic_temperature = "DT/temperature";
const char* mqtt_topic_humidity = "DT/humidity";
const char* mqtt_topic_gas = "MQ/gas";

const int MQ2_PIN = A0; // Connect the MQ2 sensor analog output to A0
const int DHT_PIN = D2; // Connect DHT11 data pin to D2

DHT dht(DHT_PIN, DHT11);
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  delay(10);
  
  pinMode(DHT_PIN, INPUT);
  dht.begin();

  connectToWiFi();
  client.setServer(mqtt_server, mqtt_port);
}

void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect("arduino-client")) {
      Serial.println("Connected to MQTT server");
    } else {
      Serial.print("Failed to connect to MQTT server, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read data from DHT sensor");
    delay(2000);
    return;
  }

  Serial.print("Temperature: ");
  Serial.println(temperature);
  Serial.print("Humidity: ");
  Serial.println(humidity);

  char temperatureStr[8];
  char humidityStr[8];
  dtostrf(temperature, 6, 2, temperatureStr);
  dtostrf(humidity, 6, 2, humidityStr);

  client.publish(mqtt_topic_temperature, temperatureStr);
  client.publish(mqtt_topic_humidity, humidityStr);

  delay(5000); // Publish data every 5 seconds
{
  if (!client.connected()) {
    reconnect();
  }

  int gasValue = analogRead(MQ2_PIN);

  Serial.print("Gas Value: ");
  Serial.println(gasValue);

  char gasStr[8];
  itoa(gasValue, gasStr, 10);

  client.publish(mqtt_topic_gas, gasStr);

  delay(5000); // Publish data every 5 seconds
}
}