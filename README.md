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

Go to your mobile phone or laptop and open the WiFi settings there you can a WiFi nework named `Smart_Plug`, connect to it. The password is `smart@tkmplug`. You can change these properties in the code. Once connected to it sometimes a popup may automatically open, if not go to you browser (Remeber to turn off mobile data if you are using a mobile to perform this task) on the address bar type in `192.168.4.1` you would get a website like this.

![Image](https://github.com/juzkiddin/SmartPlug/blob/main/Images/Wifim1.PNG)

Click on `Configure WiFi` and select the WiFi you want to connect to type in the password and Click on Save . 

![Image](https://github.com/juzkiddin/SmartPlug/blob/main/Images/Wifim2.PNG)

By the mean time in the serial monitor in Arduino IDE uncheck `Autoscroll` and scroll up you coud see an IP Address like this 

![image](https://github.com/juzkiddin/SmartPlug/blob/main/Images/SerialMonitor2.png)

Copy this IP and save it somewhere.

```
void getData(){
    voltage = pzem.voltage();
    if( !isnan(voltage) ){
        Serial.print("Voltage: "); Serial.print(voltage); Serial.println("V");
    } else {
        voltage = 0;
        Serial.println("Error reading voltage");
    }
    current = pzem.current();
    if( !isnan(current) ){
        Serial.print("Current: "); Serial.print(current); Serial.println("A");
    } else {
        current = 0;
        Serial.println("Error reading current");
    }
    power = pzem.power();
    if( !isnan(power) ){
        Serial.print("Power: "); Serial.print(power); Serial.println("W");
    } else {
        power = 0;
        Serial.println("Error reading power");
    }
    energy = pzem.energy();
    if( !isnan(energy) ){
        Serial.print("Energy: "); Serial.print(energy,3); Serial.println("kWh");
    } else {
        energy = 0;
        Serial.println("Error reading energy");
    }
    frequency = pzem.frequency();
    if( !isnan(frequency) ){
        Serial.print("Frequency: "); Serial.print(frequency, 1); Serial.println("Hz");
    } else {
        frequency = 0;
        Serial.println("Error reading frequency");
    }
    pf = pzem.pf();
    if( !isnan(pf) ){
        Serial.print("PF: "); Serial.println(pf);
    } else {
        pf = 0;
        Serial.println("Error reading power factor");
    }
    sensors.requestTemperatures();
    temperature = sensors.getTempC(sensor1);
    if(!isnan(temperature)){
        Serial.print("Temperature: "); Serial.print(temperature); Serial.println("C");
    } else {
        temperature = 0;
        Serial.println("Error reading temperature");
    }
}
```
* This function is used by the `tx_data()` function to obtain the data from the sensors for transmitting it.

```
void s_getData(){
    voltage = pzem.voltage();
    if( !isnan(voltage) ){
        vflag=1;
    } else {
        voltage = 0;
        vflag=0;
    }
    current = pzem.current();
    if( !isnan(current) ){
        cflag=1;
    } else {
        current = 0;
        cflag=0;
    }
    sensors.requestTemperatures();
    temperature = sensors.getTempC(sensor1);
    if(!isnan(temperature)){
        tflag=1;
    } else {
        temperature = 0;
        tflag=0;
    }
}
```
* This function is used by the `Safety_Check()` function to get data from the sensors.

```
void Safety_Check(){
    s_getData();
  if(voltage>=S_Voltage){
    Serial.println("1 T");
    r1_off();
    r2_off();
    r3_off();
    r4_off();
  }
  Serial.println(S_Voltage);
  if(current>=S_Current){
    Serial.println("2 T");
    r1_off();
    r2_off();
    r3_off();
    r4_off();
  }
  Serial.println(S_Current);
  if((voltage<=Su_Voltage) && vflag==1){
    Serial.println("3 T");
    r1_off();
    r2_off();
    r3_off();
    r4_off();
  }
  Serial.println(Su_Voltage);
  if((temperature>=t_temperature) && tflag==1){
    Serial.println("5 T");
    r1_off();
    r2_off();
    r3_off();
    r4_off();
  }
  Serial.println(t_temperature);
  delay(100);
}
```
* This function checks for any safety failure such as Over Current, Over Voltage, Under Voltage and High temperature and turns of the relays if any one of the fault is detected. 

```
void tx_Data(){
    tx_data = "";
    getData();
    tx_data += "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    tx_data += String(voltage);
    tx_data += " ";
    tx_data += String(current);
    tx_data += " ";
    tx_data += String(power);
    tx_data += " ";
    tx_data += String(energy);
    tx_data += " ";
    tx_data += String(frequency);
    tx_data += " ";
    tx_data += String(pf);
    tx_data += " ";
    tx_data += String(temperature);
}
```
* This function packs the data into HTML text format so that it can be transmitted via the webserver.

```
void r1_on(){
  digitalWrite(RELAY_ONE,LOW);
}
void r1_off(){
  digitalWrite(RELAY_ONE,HIGH);
}

void r2_on(){
  digitalWrite(RELAY_TWO,LOW);
}
void r2_off(){
  digitalWrite(RELAY_TWO,HIGH);
}

void r3_on(){
  digitalWrite(RELAY_THR,LOW);
}
void r3_off(){
  digitalWrite(RELAY_THR,HIGH);
}

void r4_on(){
  digitalWrite(RELAY_FOU,LOW);
}
void r4_off(){
  digitalWrite(RELAY_FOU,HIGH);
}
```
* This is a set of functions that turn the relays on and off.

```
void loop(){
    Safety_Check();
    WiFiClient client = server.available();
    if (!client)
    {
        return;
    }
    ClientRequest = (client.readStringUntil('\r'));
    ClientRequest.remove(0, 5);
    ClientRequest.remove(ClientRequest.length()-9,9);
    Serial.println("Request");
    Serial.println(ClientRequest);
    if(ClientRequest=="data"){
        tx_Data();
        client.println(tx_data);
        } 
    else{
        if(ClientRequest=="r1on"){
            r1_on();
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("");
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("Relay ON");
            client.println("</html>");
        }
        else{
            if(ClientRequest=="r1of"){
                r1_off();
                client.println("HTTP/1.1 200 OK");
                client.println("Content-Type: text/html");
                client.println("");
                client.println("<!DOCTYPE HTML>");
                client.println("<html>");
                client.println("Relay OFF");
                client.println("</html>");

            }
            else{
                if(ClientRequest.substring(0,2)=="v_"){
                    S_Voltage = ClientRequest.substring(2).toFloat();
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("");
                    client.println("<!DOCTYPE HTML>");
                    client.println("<html>");
                    client.println("Voltage Value Updated");
                    client.println("</html>");
                }
                else{
                    if(ClientRequest.substring(0,2)=="c_"){
                        S_Current = ClientRequest.substring(2).toFloat();
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("");
                        client.println("<!DOCTYPE HTML>");
                        client.println("<html>");
                        client.println("Current Value Updated");
                        client.println("</html>");
                    }
                    else{
                        if(ClientRequest.substring(0,3)=="vu_"){
                            Su_Voltage = ClientRequest.substring(3).toFloat();
                            client.println("HTTP/1.1 200 OK");
                            client.println("Content-Type: text/html");
                            client.println("");
                            client.println("<!DOCTYPE HTML>");
                            client.println("<html>");
                            client.println("Under Voltage Value Updated");
                            client.println("</html>");
                        }
                        else{
                            if(ClientRequest.substring(0,2)=="t_"){
                                t_temperature = ClientRequest.substring(2).toFloat();
                                client.println("HTTP/1.1 200 OK");
                                client.println("Content-Type: text/html");
                                client.println("");
                                client.println("<!DOCTYPE HTML>");
                                client.println("<html>");
                                client.println("Temperature Threshold Value Updated");
                                client.println("</html>");
                            }
                            else{
                                if(ClientRequest=="r2on"){
                                    r2_on();
                                    client.println("HTTP/1.1 200 OK");
                                    client.println("Content-Type: text/html");
                                    client.println("");
                                    client.println("<!DOCTYPE HTML>");
                                    client.println("<html>");
                                    client.println("Relay ON");
                                    client.println("</html>");
                                }
                                else{
                                    if(ClientRequest=="r2of"){
                                        r2_off();
                                        client.println("HTTP/1.1 200 OK");
                                        client.println("Content-Type: text/html");
                                        client.println("");
                                        client.println("<!DOCTYPE HTML>");
                                        client.println("<html>");
                                        client.println("Relay OFF");
                                        client.println("</html>");
                        
                                    }
                                    else{
                                        if(ClientRequest=="r3on"){
                                            r3_on();
                                            client.println("HTTP/1.1 200 OK");
                                            client.println("Content-Type: text/html");
                                            client.println("");
                                            client.println("<!DOCTYPE HTML>");
                                            client.println("<html>");
                                            client.println("Relay ON");
                                            client.println("</html>");
                                        }
                                        else{
                                            if(ClientRequest=="r3of"){
                                                r3_off();
                                                client.println("HTTP/1.1 200 OK");
                                                client.println("Content-Type: text/html");
                                                client.println("");
                                                client.println("<!DOCTYPE HTML>");
                                                client.println("<html>");
                                                client.println("Relay OFF");
                                                client.println("</html>");
                                
                                            }
                                            else{
                                                if(ClientRequest=="r4on"){
                                                    r4_on();
                                                    client.println("HTTP/1.1 200 OK");
                                                    client.println("Content-Type: text/html");
                                                    client.println("");
                                                    client.println("<!DOCTYPE HTML>");
                                                    client.println("<html>");
                                                    client.println("Relay ON");
                                                    client.println("</html>");
                                                }
                                                else{
                                                    if(ClientRequest=="r4of"){
                                                        r4_off();
                                                        client.println("HTTP/1.1 200 OK");
                                                        client.println("Content-Type: text/html");
                                                        client.println("");
                                                        client.println("<!DOCTYPE HTML>");
                                                        client.println("<html>");
                                                        client.println("Relay OFF");
                                                        client.println("</html>");
                                        
                                                    }
                                                    else{
                                                        client.println("HTTP/1.1 200 OK");
                                                        client.println("Content-Type: text/html");
                                                        client.println("");
                                                        client.println("<!DOCTYPE HTML>");
                                                        client.println("<html>");
                                                        client.println(ClientRequest.substring(2));
                                                        client.println("</html>");
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    delay(100);
}
```
* This is the `loop()` function that continously runs once the NodeMCU is connected to the WiFi Network. It first calls the `Safety_Check()` function to check for fault detection. After that based on the request from the client it performs the necessary tasks. The request from client will be in this format. Remeber the IP that you copied from the Serial Monitor. IP may or may not be in the form of `19.168.x.x`
   * `192.168.x.x/data` returns the all the data from the `tx_data()` function
   * `192.168.x.x/rxon` turns the 'x'th relay on using the `rx_on()` function
   * `192.168.x.x/rxof` turns the 'x'th relay off using the `rx_off()` function
   * `192.168.x.x/v_x` where 'x' is a float or integer value sets the safety upper voltage limit as 'x'
   * `192.168.x.x/c_x` where 'x' is a float or integer value sets the safety upper current limit as 'x'
   * `192.168.x.x/vu_x` where 'x' is a float or integer value sets the safety lower voltage limit as 'x'
   * `192.168.x.x/t_x` where 'x' is a float or integer value sets the safety upper temperature limit as 'x'

## Backend Code

Because we cannot directy update value from the hardware to the cloud because we are not use any third party MQTT services such as Blynk or Cayenne we need a way to somehow update this data to the cloud and visualise it in a mobile application and also control the devices via the cloud.
For this we use the PFF approach (Python-Firebase-Flutter). 

A python script acts as our communication framework. It fetches the data from the hardware system and updates it to the cloud. By 'cloud' we mean a database where all the data is stored and fetched from, for this purpose we use Firebase Firestore database which is a real time database provided by google. For the mobile application we use the Flutter framework that work flawlessly with Firestore database and also enable us to create cross-platform applications with ease.

## Firebase
----
* Open the firebase console by [clicking here](https://firebase.google.com/?gclsrc=ds&gclsrc=ds)
* Click on `Sign In` and use the given credentials to Log In to the account after that click on `Go to Console`
* After that click on `Firestore Database` on the left panel.
* You can see a screen like this.
![Image](https://github.com/juzkiddin/SmartPlug/blob/main/Images/Firebase.png)

### Data
It stores all the realtime data that is updated from the hardware
![Image](https://github.com/juzkiddin/SmartPlug/blob/main/Images/Firebase_Data.png)

### Relay
It stores the state of the a particular relay.
If `relay_x` is `false` it means that particular relay is turned off and `true` means it is turned on.
`r_flag` is a check variable used in the python script that will be explained later.
![Image](https://github.com/juzkiddin/SmartPlug/blob/main/Images/Firebase_Relay.png)

### Variables
It stores all the safety parameters of that can be updated via the mobile app.
The `flag` is anothe check variable used by the python script that will be explained later
![Image](https://github.com/juzkiddin/SmartPlug/blob/main/Images/Firebase_Safety.png)


## Python
----

* First step is to install Python and `pip` [click here](https://www.youtube.com/watch?v=dYfKJMPNMDw) and follow the steps.
* After that open the `Command Prompt` and type 
`pip install firebase_admin`
* Install Visual Studio Code(VSC) ([Click Here](https://www.youtube.com/watch?v=9o4gDQvVkLU)) by following the steps.
* Download this [file](https://github.com/juzkiddin/SmartPlug/archive/refs/heads/main.zip) and extract it.
* Open VSC and click on `File` from the Top-Bar. Click on `Open Folder` select the extracted folder.
* On the left panel click on `FirebaseMainSmartPlug.py`
* Update the `root_url` in the code to the IP address you recieve from the serial monitor. Remember both the hardware and the computer in which this program is run needs to be connected to the same WiFi network.
* Change only the IP address part in the code i.e. `https:/xxx.xxx.xxx.xxx/` only the 'x' needs to be change the rest should not be changed
* Run the program only after starting the NodeMCU and connecting it to the WiFi otherwise it may result in an error.

### Explanation

__hardware.py__
----

This is just a function to connect to the NodeMCU and not the main program. The main program is `FirebaseMainSmartPlug.py`. This file can be found in the same extracted folder.

```python
import urllib.request
```
This imports the `urllib.request` library that allows us to connect and read data from the webserver.

```python
def get_data(url):
    data = urllib.request.urlopen(url+"data").read()
    data = data.decode("utf-8")
    adata = data.split()
    volt = float(adata[0])
    curr = float(adata[1])
    pow = float(adata[2])
    ene = float(adata[3])
    freq = float(adata[4])
    pf = float(adata[5])
    temp = float(adata[6])
    return volt,curr,pow,ene,freq,pf,temp
```
This function is used to get the data from the NodeMCU by accessing the `url` which is passed from the main program. It converts the data from HTML Text to float variabled and returns each data as its individual component.

```python
def trigger_data(url):
    urllib.request.urlopen(url)
    return 0
```
This funtion is used to pass data from the main program to the NodeMCU for turning the relays ON/OFF or for updating the Safety Values.

__FirebaseMainSmartPlug.py__
----

This is the main program that you run in order to update data from the NodeMCU to Firebase Firestore Database and vice versa.

