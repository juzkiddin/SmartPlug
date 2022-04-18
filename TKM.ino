#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <PZEM004Tv30.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS D2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
PZEM004Tv30 pzem(D5, D6);

#define RELAY_ONE D7
#define RELAY_TWO D0
#define RELAY_THR D1
#define RELAY_FOU D3


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

WiFiServer server(80);

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
