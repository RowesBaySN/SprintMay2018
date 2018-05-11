/*
  * drafted by Jeremy VanDerWal ( jjvanderwal@gmail.com ... www.jjvanderwal.com )
  * co-drafted by Dylan VanDerWal ( dylanjvanderwal@gmail.com )
  * GNU General Public License .. feel free to use / distribute ... no warranties
 */

#include <ESP8266WiFi.h>
#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>
 
#define DHTPIN_0 D1     // what pin we're connected to
#define DHTTYPE_0 DHT22   // DHT 22  (AM2302)
#define ONE_WIRE_BUS D2

DHT dht_0(DHTPIN_0, DHTTYPE_0);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer;
 
//WiFi information
const char* ssid = "TCC-Sensors";
const char* password = "5Birds+Feathers";

// ThingSpeak Settings
String APIKey = "79JN0SIKD96V64SE";             // enter your channel's Write API Key
const int updateThingSpeakInterval = 20 * 1000; // 20 second interval at which to update ThingSpeak

// function prototypes
void connectWiFi();

// setup the start
void setup() {
  Serial.begin(115200);
  delay(10);
  
  dht_0.begin();
  sensors.begin();
 
}

//the looping script
void loop() {
  //confirm the wifi is still connected
  if (WiFi.status() != WL_CONNECTED) { 
    connectWiFi();
  } 
  
  //define the strings of sensor values
  float h0 = dht_0.readHumidity();
  float t0 = dht_0.readTemperature();
  float Temp = sensors.getTempCByIndex(0);
  sensors.requestTemperatures();
 
  // Check if any reads failed and exit early (to try again).
  if (isnan(h0) || isnan(t0)) {
    Serial.println("Failed to read from DHT sensor 1");
    return;
  }
  
  String S1 = String(h0);
  String S2 = String(t0);
  String S3 = String(Temp);
  String S4 = String(analogRead(A0));

  String postData = ("field1=" + S1 + "&field2=" + S2 + "&field3=" + S3 + "&field4" + S4);
  Serial.println(postData);
  
  // Update ThingSpeak
  WiFiClient client; // use WiFiClient class to create TCP connections
  if (client.connect("api.thingspeak.com", 80)) {
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + APIKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postData.length());
    client.print("\n\n");
    client.print(postData);
    String line = client.readStringUntil('\r');   // read all the lines of the reply from server and print them to Serial
    Serial.println(line);
  }
  client.stop();

  delay(updateThingSpeakInterval); // delay by the interval defined
   
}

//section to start the WIFI
void connectWiFi() {
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
  Serial.println();
}
