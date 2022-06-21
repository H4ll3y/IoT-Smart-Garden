#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <bits/stdc++.h>

// Thông tin về wifi
#define ssid "LTA"
#define password "hanoi@2021"
#define mqtt_server "broker.hivemq.com"
const uint16_t mqtt_port = 1883;

#define led D3

WiFiClient espClient;
PubSubClient client(espClient);

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
  pinMode(led, OUTPUT);

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
  std::string humi = "";
  Serial.print("Topic: ");
  Serial.println(topic);
  for (int i = 0; i < length; i++) {
    humi += (char)payload[i];
    Serial.print((char)payload[i]);
  }
  Serial.println();
  if (strcmp(topic, "Humidity") == 0 && std::stof(humi) > 30) {
//    digitalWrite(led, HIGH);
//    delay(1000);
//    digitalWrite(led, LOW);
//    delay(1000);
    led_light(led, 1000);
  }
}

// Hàm reconnect thực hiện kết nối lại khi mất kết nối với MQTT Broker
void reconnect()
{
  while (!client.connected()) // Chờ tới khi kết nối
  {
    // Thực hiện kết nối với mqtt user và pass
    if (client.connect("ESP8266_id2", "ESP_offline", 0, 0, "ESP8266_id2_offline")) //kết nối vào broker
    {
      Serial.println("Đã kết nối!!!");
      client.subscribe("Humidity");
      client.subscribe("Temperature");
      client.subscribe("HeatIndex");
    }
    else
    {
      Serial.print("Lỗi: rc= ");
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
}

void led_light(int led, int ms){
  digitalWrite(led, HIGH);
  delay(ms);
  digitalWrite(led, LOW);
  delay(ms);
}
