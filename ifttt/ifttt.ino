/*
 * Rui Santos 
 * Complete Project Details http://randomnerdtutorials.com
 */
 
#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 26     // Digital pin connected to the DHT sensor 
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

#define DHTTYPE    DHT22

DHT_Unified dht(DHTPIN, DHTTYPE);

// Replace with your SSID and Password
const char* ssid     = "Hermes";
const char* password = "halo361163";

// Replace with your unique IFTTT URL resource
const char* resource = "/trigger/cevawow/with/key/oOEhaVb0mS8dDZtK_2AQEOuVthqb1nsTL_xIT0UK2jD";

// How your resource variable should look like, but with your own API KEY (that API KEY below is just an example):
//const char* resource = "/trigger/bme280_readings/with/key/nAZjOphL3d-ZO4N3k64-1A7gTlNSrxMJdmqy3";

// Maker Webhooks IFTTT
const char* server = "maker.ifttt.com";

// Time to sleep
uint64_t uS_TO_S_FACTOR = 1000000;  // Conversion factor for micro seconds to seconds
// sleep for 30 minutes = 1800 seconds
uint64_t TIME_TO_SLEEP = 600;

#define SEALEVELPRESSURE_HPA (1013.25)

uint32_t delayMS;

void setup() {
  Serial.begin(115200); 
  delay(2000);

  dht.begin();

  sensor_t sensor;
  delayMS = sensor.min_delay / 1000;

  initWifi();
  makeIFTTTRequest();
    
  #ifdef ESP32
    // enable timer deep sleep
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);    
    Serial.println("Going to sleep now");
    // start deep sleep for 3600 seconds (60 minutes)
    esp_deep_sleep_start();
  #else
    // Deep sleep mode for 3600 seconds (60 minutes)
    Serial.println("Going to sleep now");
    ESP.deepSleep(TIME_TO_SLEEP * uS_TO_S_FACTOR); 
  #endif
}

void loop() {
  // sleeping so wont get here 
}

// Establish a Wi-Fi connection with your router
void initWifi() {
  Serial.print("Connecting to: "); 
  Serial.print(ssid);
  WiFi.begin(ssid, password);  

  int timeout = 10 * 4; // 10 seconds
  while(WiFi.status() != WL_CONNECTED  && (timeout-- > 0)) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("");

  if(WiFi.status() != WL_CONNECTED) {
     Serial.println("Failed to connect, going back to sleep");
  }

  Serial.print("WiFi connected in: "); 
  Serial.print(millis());
  Serial.print(", IP address: "); 
  Serial.println(WiFi.localIP());
}

// Make an HTTP request to the IFTTT web service
void makeIFTTTRequest() {
  sensors_event_t event_temp, event_h;
  Serial.print("Connecting to "); 
  Serial.print(server);
  
  WiFiClient client;
  int retries = 5;
  while(!!!client.connect(server, 80) && (retries-- > 0)) {
    Serial.print(".");
  }
  Serial.println();
  if(!!!client.connected()) {
    Serial.println("Failed to connect...");
  }
  
  Serial.print("Request resource: "); 
  Serial.println(resource);

  dht.temperature().getEvent(&event_temp);
  dht.humidity().getEvent(&event_h);
  if(isnan(event_temp.temperature) || isnan(event_h.relative_humidity))
    Serial.println(F("Error reading..."));
  else
  {
    String jsonObject = String("{\"value1\":\"") + event_temp.temperature + "\",\"value2\":\"" + event_h.relative_humidity + "\"}";  
    client.println(String("POST ") + resource + " HTTP/1.1");
    client.println(String("Host: ") + server); 
    client.println("Connection: close\r\nContent-Type: application/json");
    client.print("Content-Length: ");
    client.println(jsonObject.length());
    client.println();
    client.println(jsonObject);
  }
  // Temperature in Celsius
        
  int timeout = 5 * 10; // 5 seconds             
  while(!!!client.available() && (timeout-- > 0)){
    delay(100);
  }
  if(!!!client.available()) {
    Serial.println("No response...");
  }
  while(client.available()){
    Serial.write(client.read());
  }
  
  Serial.println("\nclosing connection");
  client.stop(); 
}
