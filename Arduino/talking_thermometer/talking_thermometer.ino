// talking_thermometer.ino
// Foxonix / Metre Ideas and Design

// Program: Will Sakran
// Created: 09.21.15
// Public - Modify and share this program as you wish.

/*
  This program reads an internal or external temperature sensor and
  converts that to a Fahrenheit value. That value is then encoded into
  two numbers (a tens place value and a ones place value) that are sent
  out to a Foxonix board. The Foxonix board then interprets those values
  and says the corresponding temperature.
*/

// GENERAL VARIABLES
int  temperatureF = 0;

int tens3 = 11;    // to Foxonix pin P6.3
int tens2 = 10;    // to Foxonix pin P6.2
int tens1 = 9;     // to Foxonix pin P6.1
int tens0 = 8;     // to Foxonix pin P6.0

int ones3 = 7;     // to Foxonix pin P2.3
int ones2 = 6;     // to Foxonix pin P2.2
int ones1 = 5;     // to Foxonix pin P2.1
int ones0 = 4;     // to Foxonix pin P2.0

int trigger = 2;   // to Foxonix pin P1.2

int sensorPin = 0; // the external temperature sensor is connected to ADC pin0

// The temperature sensor used in this project is TMP36. If not using an
// external sensor, then the Arduino's internal chip temp. sensor can be used.
// The internal sensor is very inaccurate (+/- 10 degrees C).
#define  EXTERNAL_TEMP     // define INTERNAL_TEMP sensor or EXTERNAL_TEMP sensor
#define  EXT_CORRECTION 1  // external temperature correction degrees F
#define  INT_CORRECTION 8  // internal temperature correction degrees F

// The setup() method runs once, when the sketch starts

void setup()   {

  byte  io_index = 0;

  Serial.begin(9600);      // start serial for output
  
  // initialize the digital pins as outputs
  for(io_index = 2; io_index < 11; io_index++)
  {
    pinMode(io_index, OUTPUT);
  }

#ifdef INTERNAL_TEMP                // if sensing internal temp, set internal voltage ref.
  ADMUX = _BV(REFS1) | _BV(REFS0);  // Set internal 1.1V reference
  ADMUX |= _BV(MUX3);               // set ADC MUX to read internal temperature
#else
  // using and external temp. sensor, so
  // disable the pullup resistor on ADC0 (ADC0 = pin14)
  digitalWrite(14, LOW);
#endif
}


void loop()                     
{
#ifdef INTERNAL_TEMP
  get_chip_temperature();
#else
  get_external_temperature();
#endif

  encode_temp();
  
  // The program will only run through once and then sit in this while loop.
  // Press RESET on the Arduino to run the program again
  while(1);
}


// This function reads the Arduino chip's internal temperature sensor
int get_chip_temperature(void) {
  
  int  reading;
  float  reading_mV;
  int  temperatureC = 0;
  
  ADCSRA &= ~(_BV(ADATE) |_BV(ADIE));      // Clear auto trigger and interrupt enable
  
  // get one conversion and then discard
  ADCSRA |= _BV(ADEN) | _BV(ADSC);         // Enable AD and start conversion
  while((ADCSRA & _BV(ADSC)));             // Wait until conversion is finished
  
  // do another conversion and we'll keep this one
  ADCSRA |= _BV(ADEN) | _BV(ADSC);         // Enable AD and start conversion
  while((ADCSRA & _BV(ADSC)));             // Wait until conversion is finished

  reading = (ADCL | (ADCH << 8));          // Get the raw ADC value
  Serial.print("ADC Reading: ");
  Serial.println(reading);
  
  // convert the ADC reading to an equivalent voltage (REF = 1.1 V)
  // 10-bit resolution = 1024 A/D counts
  reading_mV = ((reading * 1.1) / 1024) * 1000;  // temperature translated to mV
  Serial.print("ADC Reading in mV: ");
  Serial.println(reading_mV);
  
  temperatureC = (reading_mV * 25) / 314;
  Serial.print("Temp. C: ");
  Serial.println(temperatureC);
  
  temperatureF = ((temperatureC * 9) / 5) + 32;
  Serial.print("Temp. F: ");
  Serial.println(temperatureF);

  temperatureF -= INT_CORRECTION;  // apply a temperature correction 
  Serial.print("Corrected Temp. F.: ");
  Serial.println(temperatureF);
}


// This function reads the external temperature sensor connected to the ADC0 pin
void get_external_temperature()
{
  int reading;
  float voltage, temperatureC;

  // get a reading from the temperature sensor
  reading = analogRead(sensorPin);  
  Serial.print("ADC Reading: ");
  Serial.println(reading);
  
  // convert the ADC reading to an equivalent voltage
  // Vcc nominal = 5.0 V, but using measured Vcc value is more accurate
  // 10-bit resolution = 1024 A/D counts
  voltage = reading * 4.85 / 1024;  // 4.85V is measured value.
   
  Serial.print("ADC Reading in V: ");
  Serial.println(voltage);
  
  // the TMP36 has a 0.5V offset, so subtract that from the voltage.
  // the output of the sensor is scaled at 10mV per degree C
  // divide by 10mV to get degrees C (same as multiply by 100)
  temperatureC = (voltage - 0.5) * 100 ;
  
  Serial.print("Temp. C: ");
  Serial.println(temperatureC);
  
  // convert to Fahrenheit
  temperatureF = (temperatureC * 1.8) + 32;
  Serial.print("Temp. F: ");
  Serial.println(temperatureF);
  
  temperatureF -= EXT_CORRECTION;  // apply a temperature correction
  Serial.print("Corrected Temp. F.: ");
  Serial.println(temperatureF);
}


/*
  This function takes the temperature value in degrees F, applies a
  temperature correction factor, and then breaks the value down into
  a "tens" value and a "ones" value. These values are applied to the
  Arduino outputs. The Foxonix chip reads these values and interprets them
  the say the correct temperature.
*/
int encode_temp(void) {
  int  tens;
  int  ones;
  
  tens = temperatureF / 10;    // get the first digit of the temperature
  ones = temperatureF % 10;    // get the second digit of the temperature
  
  Serial.print("Tens: ");
  Serial.println(tens);
 
  Serial.print("Ones: ");
  Serial.println(ones);
  Serial.print("\n");
  
  PORTB &= 0xF0;          // clear the lower four bits of Port B
  PORTB |= tens;          // write the tens values to Port B
  PORTD &= 0x0F;          // clear the high four bits of Port D
  PORTD |= (ones << 4);   // write the values out to Port D
  
  delay(200);
  digitalWrite(trigger, HIGH); // set pin high to trigger Foxonix board (rising edge)
  delay(50);
  digitalWrite(trigger, LOW); // return trigger pin to zero
}

