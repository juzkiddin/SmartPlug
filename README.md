# SmartPlug
## Circuit Diagram
----
![Image](https://github.com/juzkiddin/SmartPlug/blob/main/Images/Schematic_Smart%20Plug_2022-04-18.png)
### Explanation
The diagram shows the connection that needs to be made to make the device work. The current sensor ( PZEM-004T ) needs to be connected in parallel with the mains and the Neutral wire needs to be passed through the inductor. 

![Image](https://github.com/juzkiddin/SmartPlug/blob/main/Images/Screenshot%202022-04-18%20at%2012.44.17%20PM.png)

The NodeMCU needs to be be powered via the USB-Port for the device to work because the power for the current sensor and relay is taken from the VU pin which is the Voltage of the USB pin (5V). 

* Temperature sensor Data pin connect to D2 of NodeMCU
* Current Sensor Rx connects to D6 and Tx to D5
* For Relays we use pins 
   * D7 for Relay 1
   * D0 for Relay 2
   * D1 for Relay 3
   * D3 for Relay 4

## Setting Up Arduino
----
Download Arduino and Install it from [here](https://www.arduino.cc/en/software)
Once downloaded and installed follow this step to add NodeMCU to Arduino IDE [Click Here!](https://www.instructables.com/Steps-to-Setup-Arduino-IDE-for-NODEMCU-ESP8266-WiF/)
After that Install the necessary libraries that is required to compile the code.
### Steps
* Click on tools

![Image](https://github.com/juzkiddin/SmartPlug/blob/main/Images/Tools.png)

* Click on Manage Libraries

![Image](https://github.com/juzkiddin/SmartPlug/blob/main/Images/Manage%20Libraries.png)

* Install All these Libraries

![Image](https://github.com/juzkiddin/SmartPlug/blob/main/Images/WifiManager.png)
![Image](https://github.com/juzkiddin/SmartPlug/blob/main/Images/Pzem.png)
![Image](https://github.com/juzkiddin/SmartPlug/blob/main/Images/OneWire.png)
![Image](https://github.com/juzkiddin/SmartPlug/blob/main/Images/DallasTemperature.png)

## Code Explanation
----
The code is available in this Repository as TKM.ino


```
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <PZEM004Tv30.h>
#include <OneWire.h>
#include <DallasTemperature.h>
```
* This is to import the necessary Libraries

```
#define ONE_WIRE_BUS D2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
PZEM004Tv30 pzem(D5, D6);
```
* Here we are defining pin D2 as the pin where data pin of tempereature sensor needs to be connected, And DallasTemperature allows us to convert raw data from temperature sensor to readable temperature values. The PZEM004Tv30 library is used to define the D5 and D6 as Rx and Tx pins to connect to the current sensor.

```
#define RELAY_ONE D7
#define RELAY_TWO D0
#define RELAY_THR D1
#define RELAY_FOU D3
```
* Defining the relay pins

```
float temperature = 0;
float voltage = 0;
float current = 0;
float power = 0;
float energy = 0;
float frequency = 0;
float pf = 0;

float S_Current = 1;
float S_Voltage = 240;
float Su_Voltage = 120;
float t_temperature = 50;

String  ClientRequest;
String  tx_data;

int vflag = 0;
int cflag = 0;
int tflag = 0;

uint8_t sensor1[8] = { 0x28, 0xAA, 0xB2, 0x12, 0x58, 0x14, 0x01, 0xA9  };
```
* Here we are declaring the necessary variable that is required. Most of it is self explanatory. Thing to note is `S_Current` refers to Safety upper limit of current for over current protection. `S_Voltage` refers to Safety upper limit of voltage for over voltage protection. `Su_Voltage` refers to Safety lower limit of voltage for under voltage protection. `t_temperature` refers to Safety upper temperature limit of voltage for high temperature protection. Sensor variable is the address of temperature sensor.

```
WiFiServer server(80);
```
* Setting up the NodeMCU as webserver so we can obtain and transmit data to it via its IP Address when connected to a WiFi Network

```
void setup() 
{
  pinMode(RELAY_ONE,OUTPUT);
  digitalWrite(RELAY_ONE,HIGH);

  pinMode(RELAY_TWO,OUTPUT);
  digitalWrite(RELAY_TWO,HIGH);

  pinMode(RELAY_THR,OUTPUT);
  digitalWrite(RELAY_THR,HIGH);

  pinMode(RELAY_FOU,OUTPUT);
  digitalWrite(RELAY_FOU,HIGH);

  ClientRequest = "";

  Serial.begin(9600);
  Serial.println();

  WiFiManager wm;
  wm.resetSettings();
  bool res;
  res=wm.autoConnect("Smart_Plug","smart@tkmplug");

  if(!res){
    Serial.println("Failed to Connect");
  }

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

```
* The `setup()` function that is used to initialise the NodeMCU and set all the Relay pins as output. Then the next step is to initialise the WiFi of NodeMCU. Here we use the WifiManager Library. Once the NodeMCU is turned on the WiFiManager gets activated. 
After you have uploaded the code open the serial monitor by pressing `CTRL+SHIFT+M` in the arduino window you would get a popup like this 

![image](https://github.com/juzkiddin/SmartPlug/blob/main/Images/SerialMonitor1.png)

Go to your mobile phone or laptop and open the WiFi settings there you can a WiFi nework named Smart_Plug, connect to it. The password is smart@tkmplug. You can change these properties in the code. Once connected to it sometimes a popup may automatically open, if not got to you browser (Remeber to turn off mobile data if you are using a mobile to perform this task) on the address bar type in `192.168.4.1` you would get a website like this.

![Image](https://github.com/juzkiddin/SmartPlug/blob/main/Images/Wifim1.PNG)

Click on `Configure WiFi` and select the WiFi you want to connect to type in the password and Click on Save . 

![Image](https://github.com/juzkiddin/SmartPlug/blob/main/Images/Wifim2.PNG)

By the mean time in the serial monitor in Arduino IDE uncheck `Autoscroll` and scroll up you coud see an IP Address like this 

![image](https://github.com/juzkiddin/SmartPlug/blob/main/Images/SerialMonitor2.png)

Copy this IP and save it somewhere.
