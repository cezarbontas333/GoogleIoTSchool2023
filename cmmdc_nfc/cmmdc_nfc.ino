#include <WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include "NfcAdapter.h"
#include <ArduinoJson.h>

#define CS_PIN 21

MFRC522 mfrc522(CS_PIN, UINT8_MAX); // Create MFRC522 instance

NfcAdapter nfc = NfcAdapter(&mfrc522);

const char* ssid = "UPB-Guest";
const char* password = "";
const char* mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

StaticJsonDocument<200> doc;

int numbers[3] = {0}, r1, r2, res = 0;

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
  const char* source = doc["source"];
  if(source[1] == '1')
    numbers[0] = doc["value"];
  else if(source[1] == '2')
    numbers[1] = doc["value"];
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
      client.publish("google_iot_proj_d3", "M-am conectat");
      // ... and resubscribe
      client.subscribe("google_iot_proj_d1");
      client.subscribe("google_iot_proj_d2");
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
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522
  nfc.begin();
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if(nfc.tagPresent())
  {
    NfcTag tag = nfc.read();
    if (tag.hasNdefMessage())
    {
      StaticJsonDocument<200> doc_send;
      NdefMessage message = tag.getNdefMessage();
      NdefRecord record = message.getRecord(0);
      int payloadLength = record.getPayloadLength();
      const byte *payload = record.getPayload();
      String payloadAsString = "";
      for (int c = 3; c < payloadLength; c++) {
        payloadAsString += (char)payload[c];
      }
      char msg[100];
      Serial.println(payloadAsString);
      doc_send["event"] = "measure";
      doc_send["source"] = "d3";
      doc_send["value"] = payloadAsString.toInt();
      numbers[2] = doc_send["value"];
      serializeJson(doc_send, msg);
      client.publish("google_iot_proj_d3", msg);
      Serial.println(msg);
      delay(500);
    }
  }
  bool ver = true;
  for(int i = 0; i < 3; i++)
  {
    //Serial.printf("%d, ", numbers[i]);
    if(numbers[i] == 0)
    {
      ver = false;
    }
    //Serial.printf("\n");
  }
  if(ver)
  {
    r1 = numbers[0] % numbers[1];
    while(r1 != 0)
    {
      numbers[0] = numbers[1];
      numbers[1] = r1;
      r1 = numbers[0] % numbers[1];
    }
    r2 = numbers[2] % numbers[0];
    while(r2 != 0)
    {
      numbers[2] = numbers[0];
      numbers[0] = r2;
      r2 = numbers[2] % numbers[0];
    }
    res = numbers[0];
  }
  if(res)
  {
    Serial.println(res);
    for(int cont = 0; cont < res;)
    {
      digitalWrite(13, HIGH);
      delay(1000);
      digitalWrite(13, LOW);
      delay(1000);
      cont++;
    }
    res = 0;
    numbers[2] = 0;
  }
}
