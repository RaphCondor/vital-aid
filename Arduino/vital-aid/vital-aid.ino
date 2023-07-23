#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
 
#define REPORTING_PERIOD_MS     1000
 
// PulseOximeter is the higher level interface to the sensor
// it offers:
//  * beat detection reporting
//  * heart rate calculation
//  * SpO2 (oxidation level) calculation
PulseOximeter pox;
float glucose, temperature;
int analog_temp, analog_glucose;
float volts_temp, volts_glucose;
 
uint32_t tsLastReport = 0;
 
// Callback (registered below) fired when a pulse is detected
void onBeatDetected()
{
    //Serial.println("Beat!");
}

void setup()
{
    Serial.begin(115200);
 
    Serial.print("Initializing pulse oximeter..");
 
    // Initialize the PulseOximeter instance
    // Failures are generally due to an improper I2C wiring, missing power supply
    // or wrong target chip
    if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
    } 
 
    // The default current for the IR LED is 50mA and it could be changed
    //   by uncommenting the following line. Check MAX30100_Registers.h for all the
    //   available options.
    pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
 
    // Register a callback for the beat detection
//    pox.setOnBeatDetectedCallback(onBeatDetected);

      pinMode(A1, INPUT);
      pinMode(A0, INPUT);
}
 
void loop()
{
    analog_temp = analogRead(A0); // analog value of temperature sensor
    analog_glucose = analogRead(A1); // analog value of glucose sensor
    volts_temp  = analog_temp * (5000/1024.0); // voltage output of temperature sensor (mV)
    volts_glucose  = analog_glucose * (5.00/1024.0); // voltage output of glucose sensor (V)
    glucose = (volts_glucose*70.577) + 34.093; // using calibration curve y = mx+b
    // calibration curve for glucose sensor might be different. Refer to Buda et al 2014 DOI:10.1109/IECBES.2014.7047655
    temperature = (volts_temp / 10); // conversion factor from millivolts to temp in degrees Celcius
    
    // Make sure to call update as fast as possible
    pox.update();
 
    // Asynchronously dump heart rate and oxidation levels to the serial
    // For both, a value of 0 means "invalid"
  
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
      
        Serial.print("Heart Rate: ");
        Serial.print(pox.getHeartRate());
        Serial.print(" ");
        Serial.print("Blood Oxygen Saturation: ");
        Serial.print(pox.getSpO2());
        Serial.print(" ");
        Serial.print("Body Teperature: ");
        Serial.print(temperature);
        Serial.print(" ");
        Serial.print("Blood Glucose: ");
        Serial.println(glucose);
        tsLastReport = millis();
    }
}
