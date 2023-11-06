#include <ESP32Servo.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <string.h>

int APin = 23;
ESP32PWM pwm;
int freq = 100;

#define DHTPIN 26
#define DHTTYPE DHT22

#define THRESHOLD_TEMP 28

float temp = 0, hum = 0;
bool active = false, system_status = false;

const char* ssid = "UPB-Guest";
const char* password = "";
const char* mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(100)
char msg[MSG_BUFFER_SIZE];
int value = 0;

DHT dht(DHTPIN, DHTTYPE);
int delayTemp;

StaticJsonDocument<250> doc;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  DeserializationError error = deserializeJson(doc, payload);
  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  const char* action = doc["action"];
  const char* target = doc["target"];
  bool data = doc["data"];

  if(!(strcmp(action, "set_state")))
    if(!(strcmp(target, "all")) || !(strcmp(target, "fan"))) {
      Serial.println(data);
      system_status = data;
    }
  
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "EdgeLord";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("google_iot_proj_input", "M-am conectat");
      // ... and resubscribe
      client.subscribe("google_iot_proj_output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

	// Allow allocation of all timers
	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
	pwm.attachPin(APin, freq, 10); // 1KHz 8 bit

  dht.begin();
}

unsigned long lastAct = 0;
void loop() {
  StaticJsonDocument<200> doc_send;
  if (!client.connected()) {
    reconnect();
  }

  client.loop();
  if(system_status)
  {
    // Delay between measurements.
    delay(1000);
    // Get temperature event and print its value.
    float temp = dht.readTemperature();
    if (isnan(temp)) {
      Serial.println(F("Error reading!"));
    }
    else{
      if(!active)
      {
        // am depasit thresholdul -> pronim motorul
        if (temp >= THRESHOLD_TEMP + 0.4)
        {
          active = true;
        }
      } else {
        if(temp > THRESHOLD_TEMP - 0.5)
        {
          for (float brightness = 0; brightness <= 0.5; brightness += 0.001) {
            // Write a unit vector value from 0.0 to 1.0
            pwm.writeScaled(brightness);
            delay(2);
          }
          //delay(1000);
          for (float brightness = 0.5; brightness >= 0; brightness -= 0.001) {
            pwm.writeScaled(brightness);
            delay(2);
          }
          delay(200);
        } else {
          active = false;
        }
      }
    }

    if(millis() - lastAct > 2000) {
      lastAct = millis();
      char msg[100];
      doc_send["event"] = "measure";
      doc_send["source"] = "fan";
      doc_send["value"] = active;
      serializeJson(doc_send, msg);
      client.publish("google_iot_proj_input", msg);
      Serial.println(msg);
      Serial.println(temp);
    }
  }
}
