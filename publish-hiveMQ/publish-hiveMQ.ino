#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#include <string>
#include <bits/stdc++.h>

// Thông tin về wifi
#define ssid "LTA"
#define password "hanoi@2021"
#define mqtt_server "broker.hivemq.com"
const uint16_t mqtt_port = 1883;
#define DHTPIN D1
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

float readHumi() {
  return dht.readHumidity();
}

float readTemp(bool check) {
  if (!check)
    return dht.readTemperature();
  // F
  return dht.readTemperature(true);
}

float computeHeatIndex(bool check) {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  if (!check)
    // Compute heat index in Celsius (isFahreheit = false)
    // C
    return dht.computeHeatIndex(t, h, false);
  // F
  return dht.computeHeatIndex(f, h);
}

void setup()
{
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

// Hàm kết nối wifi
void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Hàm call back để nhận dữ liệu
void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Topic: ");
  Serial.println(topic);
  for (int i = 0; i < length; i++)
    Serial.print((char)payload[i]);
  Serial.println();
}

// Hàm reconnect thực hiện kết nối lại khi mất kết nối với MQTT Broker
void reconnect()
{
  while (!client.connected()) // Chờ tới khi kết nối
  {
    // Thực hiện kết nối với mqtt user và pass
    if (client.connect("ESP8266_id1", "ESP_offline", 0, 0, "ESP8266_id1_offline")) //kết nối vào broker
    {
      Serial.println("Đã kết nối!!!");
      //      client.subscribe("Humidity");
      //      client.subscribe("Temperature");
      //      client.subscribe("HeatIndex");
    }
    else
    {
      Serial.print("Lỗi: rc = ");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Đợi 5s
      delay(5000);
    }
  }
}

void loop()
{
  if (!client.connected())// Kiểm tra kết nối
    reconnect();
  client.loop();

  delay(2000);
  // Serial.println("Đưa dữ liệu đo được lên các topic");
  client.publish("Humidity", std::to_string(readHumi()).c_str());
  client.publish("Temperature", std::to_string(readTemp(false)).c_str());
  client.publish("HeatIndex", std::to_string(computeHeatIndex(false)).c_str());
}
