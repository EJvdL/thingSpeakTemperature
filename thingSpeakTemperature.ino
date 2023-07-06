/*
 ESP8266 --> ThingSpeak Channel
 
 This sketch sends the value of Analog Input (A0) to a ThingSpeak channel
 using the ThingSpeak API (https://www.mathworks.com/help/thingspeak).
 
 Requirements:
 
   * ESP8266 Wi-Fi Device
   * Arduino 1.8.8+ IDE
   * Additional Boards URL: http://arduino.esp8266.com/stable/package_esp8266com_index.json
   * Library: esp8266 by ESP8266 Community
   * Library: ThingSpeak by MathWorks
 
 ThingSpeak Setup:
 
   * Sign Up for New User Account - https://thingspeak.com/users/sign_up
   * Create a new Channel by selecting Channels, My Channels, and then New Channel
   * Enable one field
   * Enter SECRET_CH_ID in "secrets.h"
   * Enter SECRET_WRITE_APIKEY in "secrets.h"

 Setup Wi-Fi:
  * Enter SECRET_SSID in "secrets.h"
  * Enter SECRET_PASS in "secrets.h"
  
 Tutorial: http://nothans.com/measure-wi-fi-signal-levels-with-the-esp8266-and-thingspeak
   
 Created: Feb 3, 2017 by Hans Scharler (http://nothans.com)

  To connect the Dallas thermometers
  https://lastminuteengineers.com/multiple-ds18b20-arduino-tutorial/
 
*/

//#define DEBUG

#include "ThingSpeak.h"
#include "MySecrets.h"
#include <ESP8266WiFi.h>

#include <OneWire.h> 
#include <DallasTemperature.h>

unsigned long myChannelNumber = SECRET_CH_ID;
const    char * myWriteAPIKey = SECRET_WRITE_APIKEY;


char ssid[] = SECRET_SSID;   // your network SSID (name)
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key index number (needed only for WEP)
WiFiClient  client;

const char* statuses[] =  { "WL_IDLE_STATUS=0", "WL_NO_SSID_AVAIL=1", "WL_SCAN_COMPLETED=2", "WL_CONNECTED=3", "WL_CONNECT_FAILED=4", "WL_CONNECTION_LOST=5", "WL_DISCONNECTED=6"};

#define ONE_WIRE_BUS 4            // Data wire is plugged into pin 4 = D2 on the Arduino 
OneWire           mvOneWire(ONE_WIRE_BUS); 
DallasTemperature mvSensors(&mvOneWire);

/* -----
 * wifiReconnect
 * Checks the current connection state of the WiFi.
 * If not connected, try to re-connect.
 * -----
 */
void wifiReconnect() {
  if (WiFi.status() != WL_CONNECTED) {
    #ifdef DEBUG
      Serial.print("Attempting to connect to SSID: ");
      Serial.println(SECRET_SSID);
    #endif
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      delay(5000);
    #ifdef DEBUG
      Serial.print(".");
    #endif      
    }
    #ifdef DEBUG
      Serial.println("\nConnected.");
    #endif  
  } else {
    Serial.print("*");  //Check how often the WiFi connection is broken
  }
}

/* -----
 * monitorWiFi
 * Outputs WiFi status and IP@ to the serial output
 * -----
 */
void monitorWiFi()
{
  Serial.println();
  Serial.print("wlstatus:");
  Serial.println(statuses[WiFi.status()]);
  Serial.print("WiFi.localIP():");
  Serial.println(WiFi.localIP().toString());
  Serial.print("IP.isSet: ");
  Serial.println(WiFi.localIP().isSet());
}

void setup() {
  /* -----
   * Setup Serial
   * -----
   */
  Serial.begin(115200);
  delay(100);
  Serial.println();
  
  /* -----
   * Setup WiFi to a fixed IP address
   * -----
   */
  WiFi.mode(WIFI_STA);
  
  IPAddress ip(192,168,2,96);
  IPAddress dns(192, 168, 2, 254);
  IPAddress gateway(192, 168, 2, 254);
  IPAddress subnet(255, 255, 255, 0);  

  bool lvResult = WiFi.config(ip, dns, gateway, subnet);
 
  #ifdef DEBUG
    if (!lvResult) {
      Serial.println("STA Failed to configure");
    }
    monitorWiFi();
  #endif  
  
  /* -----
   * Start a client to connect to the ThingSpeak framework
   * Sensor data is going to be logged to ThingSpeak channels
   * -----
   */
  ThingSpeak.begin(client);

  /* -----
   * Start the communication to the Dallas temperature sensors. 
   * Check if at least two temperature sensors are found.
   * If no or only one sensor found stop execution but give up control to the ESP with the yield() function 
   * -----
   */  
  mvSensors.begin();
  if (mvSensors.getDeviceCount() == 0) {
    Serial.println("Only one sensor is found, at least two sensors are expected");
    while (true) {
      yield();
    }
  }
}

void loop() {
  float lvTemp;
  int   lvHttpCode;
  
  /* -----
   * Connect or reconnect to WiFi
   * -----
   */
   wifiReconnect();

  /* -----
   * Send a command on the I2C bus to all Temp sensors to take a temperature measurement
   * -----
   */
  mvSensors.requestTemperatures();  

  /* -----
   * Read the degree Celcius temperature from the sensor(s) and
   * log it to my ThingSpeak Channel on Field 1 and Field 2
   * Note: you can only send data to ThinSpeak once each 15 seconds, so a delay 20 second is added
   * -----
   */
  for (int i = 0; i <= 1; i++) {
    lvTemp = mvSensors.getTempCByIndex(i);
    lvHttpCode = ThingSpeak.writeField(myChannelNumber, i + 1, lvTemp, myWriteAPIKey);
    
    #ifdef DEBUG
      Serial.print("ChannelNumber: ");
      Serial.println(myChannelNumber);
      Serial.print("Channel: ");
      Serial.println(i + 1);
      Serial.print("Value: ");
      Serial.println(lvTemp);
      if (lvHttpCode == 200) {
          Serial.println("Channel write successful.");
        }
        else {
          Serial.println("Problem writing to channel. HTTP error code " + String(lvHttpCode));
        }
    #endif

    delay(20000);
  }
}
