#include <Arduino.h>
#include "rg15arduino.h"
#include<SoftwareSerial.h>
#include "DHT.h"
SoftwareSerial mySerial(2, 3); // RX, TX

RG15Arduino rg15;

#define DHTPIN2 4
#define DHTTYPE2 DHT22

DHT dht2(DHTPIN2,DHTTYPE2);

float temp_dht22=0,humidity_dht22 = 0,rainfall=0;
const int sensorPin = A2
; //Defines the pin that the anemometer output is connected to
int sensorValue = 0; //Variable stores the value direct from the analog pin
float sensorVoltage = 0; //Variable that stores the voltage (in Volts) from the anemometer being sent to the analog pin
float windSpeed = 0; // Wind speed in meters per second (m/s)
 
float voltageConversionConstant = .004882814; //This constant maps the value provided from the analog read function, which ranges from 0 to 1023, to actual voltage, which ranges from 0V to 5V
int sensorDelay = 250; //Delay between sensor readings, measured in milliseconds (ms)
 
//Anemometer Technical Variables
//The following variables correspond to the anemometer sold by Adafruit, but could be modified to fit other anemometers.
 
float voltageMin = .4; // Mininum output voltage from anemometer in mV.
float windSpeedMin = 0; // Wind speed in meters/sec corresponding to minimum voltage
 
float voltageMax = 2.0; // Maximum output voltage from anemometer in mV.
float windSpeedMax = 32; // Wind speed in meters/sec corresponding to maximum voltage

void setup(){
    Serial.begin(9600);
    Serial.println("setup"); 
    mySerial.begin(9600);
    rg15.setStream(&mySerial);
      
     // dht1.begin();
    dht2.begin();
}

void loop(){
   if(rg15.poll()){
    rainfall = rg15.totalAcc;
    }
    
    temp_dht22 = dht2.readTemperature();
    humidity_dht22 = dht2.readHumidity();
    sensorValue = analogRead(sensorPin); //Get a value between 0 and 1023 from the analog pin connected to the anemometer
 
    sensorVoltage = sensorValue * voltageConversionConstant; //Convert sensor value to actual voltage
     
    //Convert voltage value to wind speed using range of max and min voltages and wind speed for the anemometer
    if (sensorVoltage < voltageMin){
     windSpeed = 0; //Check if voltage is below minimum value. If so, set wind speed to zero.
    }else {
      windSpeed = (sensorVoltage - voltageMin)*windSpeedMax/(voltageMax - voltageMin);
    }

    Serial.print(String(temp_dht22) + " " + String(humidity_dht22) + " " + String(rainfall) +" " + String(windSpeed*18/5) +" ");
    delay(1500); 
}
