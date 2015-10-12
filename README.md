# Talking-Thermometer

Foxonix Embedded Audio Platform
talking_thermometer.EZ
Developed by Metre Ideas and Design
www.foxonix.com

Program: Will Sakran
Created: 09.05.14
Public - Modify and share this program as you wish.

Modified 09.21.15
Moving inputs from ports P1 and P2 to ports P2 and P6. The Arduino's serial communication conflicts with Foxonix pins P0.0 and P0.1, so if we want to use the Serial Monitor to debug the Arduino, we need to make sure P1.0 and P1.1 are free. 

Program description
This program reads two input ports and stores the values in two registers. Those values are then decoded and used to call specific labels in the program. Two ports is 8 bits, so we can decode up to 256 different labels. This program contains audio to say temperatures between 40 and 109 degrees.

Hardware INPUTS
Port P2 - ones place temperature code from the Arduino
Port P6 - tens place temperature code from the Arduino
Pin P0.2 - this pin gets triggered when a temperature is ready.

Hardware OUTPUTS
Speaker attached to PWM output
