# SmartPlug
## Circuit Diagram
----
![Image](https://github.com/juzkiddin/SmartPlug/blob/main/Images/Schematic_Smart%20Plug_2022-04-18.png)
### Explanation
The diagram shows the connection that needs to be made to make the device work. The current sensor ( PZEM-004T ) needs to be connected in parallel with the mains and the Neutral wire needs to be passed through the inductor. 

![Image](https://github.com/juzkiddin/SmartPlug/blob/main/Images/Screenshot%202022-04-18%20at%2012.44.17%20PM.png)

The NodeMCU needs to be powered via the USB Port for the device to work because the power for the current sensor and relay is taken from the VU pin which is the Voltage of the USB pin (5V).

* Temperature sensor Data pin connects to D2 of NodeMCU
* Current Sensor Rx connects to D6 and Tx to D5
* For Relays we use pins 
   * D7 for Relay 1
   * D0 for Relay 2
   * D1 for Relay 3
   * D3 for Relay 4

## Setting Up Arduino
----
Download Arduino and Install it from [here](https://www.arduino.cc/en/software)
Once installed follow this step to add NodeMCU to Arduino IDE [Click Here!](https://www.instructables.com/Steps-to-Setup-Arduino-IDE-for-NODEMCU-ESP8266-WiF/)
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
* Here we are declaring the necessary variable that is required. Most of it is self-explanatory. The thing to note is that `S_Current` refers to the Safety upper limit of current for overcurrent protection. `S_Voltage` refers to the Safety upper limit of voltage for overvoltage protection. `Su_Voltage` refers to a Safety lower limit of voltage for under-voltage protection. and `t_temperature` refers to the Safety upper-temperature limit of voltage for high-temperature protection. The sensor variable is the address of the temperature sensor.

```
WiFiServer server(80);
```
* Setting up the NodeMCU as a web server so we can obtain and transmit data to it via its IP Address when connected to a WiFi Network

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
* The `setup()` function is used to initialise the NodeMCU and set all the Relay pins as output. Then the next step is to initialise the WiFi of NodeMCU. Here we use the WifiManager Library. Once the NodeMCU is turned on the WiFiManager gets activated. 
After you have uploaded the code open the serial monitor by pressing `CTRL+SHIFT+M` in the Arduino window you would get a popup like this 

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
* This is the `loop()` function that continuously runs once the NodeMCU is connected to the WiFi Network. It first calls the `Safety_Check()` function to check for fault detection. After that based on the request from the client, it performs the necessary tasks. The request from the client will be in this format. Remember the IP that you copied from the Serial Monitor. IP may or may not be in the form of `19.168.x.x`
   * `192.168.x.x/data` returns the all the data from the `tx_data()` function
   * `192.168.x.x/rxon` turns the 'x'th relay on using the `rx_on()` function
   * `192.168.x.x/rxof` turns the 'x'th relay off using the `rx_off()` function
   * `192.168.x.x/v_x` where 'x' is a float or integer value sets the safety upper voltage limit as 'x'
   * `192.168.x.x/c_x` where 'x' is a float or integer value sets the safety upper current limit as 'x'
   * `192.168.x.x/vu_x` where 'x' is a float or integer value sets the safety lower voltage limit as 'x'
   * `192.168.x.x/t_x` where 'x' is a float or integer value sets the safety upper temperature limit as 'x'

## Backend Code

Because we cannot directly update value from the hardware to the cloud as we do not use any third-party MQTT services such as Blynk or Cayenne, we need a way to somehow update this data to the cloud and visualise it in a mobile application and also control the devices via the cloud.
For this, we use the PFF approach (Python-Firebase-Flutter). 

A python script acts as our communication backend. It fetches the data from the hardware system and updates it to the cloud. By 'cloud' we mean a database where all the data is stored and fetched from, for this purpose we use Firebase Firestore database which is a real time database provided by Google. For the mobile application we use the Flutter framework that work flawlessly with Firestore database and also enable us to create cross-platform applications with ease.

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
It stores the state of the particular relay.
If `relay_x` is `false` it means that a particular relay is turned off and `true` means it is turned on.
`r_flag` is a check variable used in the python script that will be explained later.
![Image](https://github.com/juzkiddin/SmartPlug/blob/main/Images/Firebase_Relay.png)

### Variables
It stores all the safety parameters of that can be updated via the mobile app.
The `flag` is another check variable used by the python script that will be explained later
![Image](https://github.com/juzkiddin/SmartPlug/blob/main/Images/Firebase_Safety.png)


## Python
----

* First step is to install Python and `pip` [click here](https://www.youtube.com/watch?v=dYfKJMPNMDw) and follow the steps.
* After that open the `Command Prompt` and type 
`pip install firebase_admin`
* Install Visual Studio Code(VSC) ([Click Here](https://www.youtube.com/watch?v=9o4gDQvVkLU)) by following the steps.
* Download this [file](https://github.com/juzkiddin/SmartPlug/archive/refs/heads/main.zip) and extract it.
* Open VSC and click on `File` from the Top Bar. Click on `Open Folder` and select the extracted folder.
* On the left panel click on `FirebaseMainSmartPlug.py`
* Update the `root_url` in the code to the IP address you received from the serial monitor. Remember both the hardware and the computer in which this program is run needs to be connected to the same WiFi network.
* Change only the IP address part in the code i.e. `https:/xxx.xxx.xxx.xxx/` only the 'x' needs to be changed the rest should not be changed
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

```python
import firebase_admin
from firebase_admin import credentials,firestore
import hardware as hw
```
Here we import the `firebase_admin` library that we installed using `pip`. This library enables us to connect to the firestore database from the python code.
Then we are importing the hardware.py that we discussed before to connect to NodeMCU to read and write data from the hardware.

```python
cred = credentials.Certificate("smartplugauth.json")
firebase_admin.initialize_app(cred,{'storageBucket': 'farmhelptest2.appspot.com'})
db=firestore.client()
```
Here we are initialising the parameters to allow the python code to connnect to our database. All the credentials required for the connection is stored in in the `smartplugauth.json` file which is present in the extracted folder.

```python
root_url = "http://192.xxx.xxx.xxx/"
```
This is the IP address of the NodeMCU that you obtain from the Serial Monitor. Change this according to your IP. Note only change the 'x' don't change anything else including the `'/'`.

```python
def update(volt,curr,pow,ene,freq,pf,temp):
    db.collection('smart_plug').document('data').update({'voltage':str(volt)})
    db.collection('smart_plug').document('data').update({'current':str(curr)})
    db.collection('smart_plug').document('data').update({'power':str(pow)})
    db.collection('smart_plug').document('data').update({'energy':str(ene)})
    db.collection('smart_plug').document('data').update({'frequency':str(freq)})
    db.collection('smart_plug').document('data').update({'pf':str(pf)})
    db.collection('smart_plug').document('data').update({'temperature':str(temp)})
    print("updated")
```
This function updates the data obtained from the NodeMCU to the `data` section under each subsection to the firebase firestore database. And from there it is fetched by the Mobile Application and shown to the user.

![Image](https://github.com/juzkiddin/SmartPlug/blob/main/Images/Data_M_1.png)

![Image](https://github.com/juzkiddin/SmartPlug/blob/main/Images/Data_M_2.png)

![Image](https://github.com/juzkiddin/SmartPlug/blob/main/Images/Data_M_3.png)

![Image](https://github.com/juzkiddin/SmartPlug/blob/main/Images/Data_M_4.png)

```python
def safety():
```
This is the `safety()` function. The safety conditions are checked on a hardware level inside the NodeMCU for fast response and at a software level in this python script for added safety. This functions also updates the safety parameters set from the app directly to the NodeMCU.

```python
    result=db.collection('smart_plug').document('variables').get()
    relay_r = db.collection('smart_plug').document('relay').get()
    result_data = db.collection('smart_plug').document('data').get()
     if (result.exists and relay_r.exists and result_data.exists):
        result=result.to_dict()
        relay_r=relay_r.to_dict()
        result_data = result_data.to_dict()
        s_voltage = result.get('s_voltage')
        s_current = result.get('s_current')
        su_voltage = result.get('Su_voltage')
        t_temp = result.get('T_temperature')
        flag = result.get('flag')
        relay1 = relay_r.get('relay1')
        relay2 = relay_r.get('relay2')
        relay3 = relay_r.get('relay3')
        relay4 = relay_r.get('relay4')
        r_flag = relay_r.get('r_flag')
```
On the `update()` function we update data *to* the firebase database. Here we are reading data *from* firebase database and extracting it to perform functions.
`result` variable stores the data from the `Variables` field in the Firestore Database.
`relay_r` variable stores the data from the `Relay` field in the Firestore Database.
`result_data` variable stores the data from the `Data` field in the Firestore Database.

`flag` and `r_flag` are the check variable that is used by this python scripts for certain functions.

```python
if(flag=="true"):
            hw.trigger_data(root_url+"v_"+str(s_voltage))
            hw.trigger_data(root_url+"c_"+str(s_current))
            hw.trigger_data(root_url+"vu_"+str(su_voltage))
            hw.trigger_data(root_url+"t_"+str(t_temp))
            db.collection('smart_plug').document('variables').update({'flag':'false'})
            print("safety_updated")
```
This block of the code updates the safety parameters to NodeMCU only if the `flag` variable is `"true"` and set the `flag` variable is set to `"false"` once it is executed. The `flag` variable is set to `"true"` by the mobile app when the `Set Values` button in the `Set Limits` section in clicked

![Image](https://github.com/juzkiddin/SmartPlug/blob/main/Images/Set_M_1.png)

Click on the `Set Limits` buttion it opens up a screen where you can adjust the safety parameters.

![Image](https://github.com/juzkiddin/SmartPlug/blob/main/Images/Set_M_2.png)

When the `Set Values` button is clicked it updates the new safety values to the firebase database and also sets the `flag `as `"true"`. This executes the python block above and updates the values to the nodemcu.

```python
        if((float(result_data.get('voltage'))>=result.get('Su_voltage'))and(float(result_data.get('voltage'))<=result.get('s_voltage'))and(float(result_data.get('current'))<=result.get('s_current'))and(float(result_data.get('temperature'))<=result.get('T_temperature'))):
            if(relay1=="true"):
                hw.trigger_data(root_url+"r1on")
                print("on 1")
            if(relay2=="true"):
                hw.trigger_data(root_url+"r2on")
                print("on 2")
            if(relay3=="true"):
                hw.trigger_data(root_url+"r3on")
                print("on 3")
            if(relay4=="true"):
                hw.trigger_data(root_url+"r4on")
                print("on 4")
```
This block of the code check the safety parameters is met i.e Over Voltage, Under Voltage, Over Current and High Temperature conditions as set by the user is checked.
If the check passes i.e. no fault is detected it checks whether the `relay_x` variable in the relay section of the firebase is set to `"true"`. If so it uses the trigger function to turn on the x_th relay.
The `relay_x` variable is set to `"true"` by the Mobile app when the user turns on a plug.

![Image](https://github.com/juzkiddin/SmartPlug/blob/main/Images/M_ON.png)

When this button is clicked it sets the valure of `relay_x` in the firebase firestore database to `"true"`
  * Plug 1 corresponds to `relay_1`
  * Plug 2 corresponds to `relay_2`
  * Plug 3 corresponds to `relay_3`
  * Plug 4 corresponds to `relay_4`
If the safety conditions are met the python code executes the particular block of the code that turns on that relay at the hardware level.

```python
      if(r_flag=="true"):
            if(relay1=="false"):
                hw.trigger_data(root_url+"r1of")
                print("off 1")
            if(relay2=="false"):
                hw.trigger_data(root_url+"r2of")
                print("off 2")
            if(relay3=="false"):
                hw.trigger_data(root_url+"r3of")
                print("off 3")
            if(relay4=="false"):
                hw.trigger_data(root_url+"r4of")
                print("off 4")
            db.collection('smart_plug').document('relay').update({'r_flag':"false"})
```
This block of the code is turn off the relay when the used presses the OFF button in the Mobile Application. 

![Image](https://github.com/juzkiddin/SmartPlug/blob/main/Images/M_OFF.png)

Once the button is turned off it sets the `relay_x` to `"false"` and the `r_flag` to `"true"`. 
`r_flag` is turned to `"true"` when any of the plug is turned off.
`relay_x` is changed to `"false"` when `Plug X` is turned off.
The use of `r_flag` is, It acts as a check and  executes the OFF section of the python code only if the user turns of any one of the relay.
Without this check each relay will be turned off for every execution of the Python code which wastes time and affects performance.
Since the Python Code runs infinitely untill it stops this without this `r_flag` check there will be a huge wastage of time and preformance.
It set the `r_flag` variable is set to `"false"` once it is executed

```python
while(True):
    volt,curr,pow,ene,freq,pf,temp = hw.get_data(root_url)
    update(volt,curr,pow,ene,freq,pf,temp)
    safety()
```
This runs the python code in a infinte loop updating the data from the nodemcu to the firebase firestore database and executing the `safety()` function infinitely.

## Mobile Application

The mobile app is built using flutter and the application is attached in this repository and also can be found in the extracted folder under the name `Mobile_App.apk` Install it in an Android Device and run it.

The source code for the App is [here](https://github.com/abhijithmh/Smart_Plug.git)

