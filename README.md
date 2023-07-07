# thingSpeakTemperature
Explore interface between Dallas DS18B20 sensor and thingSpeak platform. <br>
Based on 
* Example described in https://github.com/nothans/thingspeak-esp-examples/tree/master <br>
* Corresponding tutorial: http://nothans.com/measure-wi-fi-signal-levels-with-the-esp8266-and-thingspeak
* DS18B20 code snippet

## DS18B20 code snippet

```
#include <OneWire.h> 
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 4 // Data wire is plugged into pin 4 on the Arduino 

DeviceAddress gv_Thermometer;

OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with any OneWire devices  

DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature. 
  
void setup(void) 
{ 
  sensors.begin(); 
  delay(25);
} 

void loop(void) 
{ 
  sensors.requestTemperatures(); // Send the command to get temperature readings from all devices on the bus

  for (int i = 0; i < sensors.getDeviceCount(); i++) {
    Serial.print("Sensor "); Serial.print(i + 1); Serial.print(" : ");
    double tempC = sensors.getTempCByIndex(i);
    Serial.print(tempC); 
    Serial.print((char)176);  //shows degrees character
    Serial.print("C");
  }
  delay(2000); 
} 
```
## Wemos D1 pin usage
Not all pins of the Wemos D1 can be used for peripherals. See this link for more information:<br>
https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/
