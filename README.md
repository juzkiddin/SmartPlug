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
..1. D7 for Relay 1
..2. D0 for Relay 2
..3. D1 for Relay 3
..4. D3 for Relay 4


