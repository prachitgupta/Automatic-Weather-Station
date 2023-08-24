# Automatic-Weather-Station
The following repo contains Arduino C code for integrating sensor data via i2c, spi and Modbus protocols, uploading data to Thingspeak, and real-time data logging
Code for testing individual sensor data, using various communication protocols has been provided
Flashing Final_code_with_real_time_logging will enable you to log temperature, humidity, wind speed, and rainfall accumulation every 15 minutes on SD card with a timestamp. Daily data will be recorded in separate files
The code for individual sensors contains the names of library to be included and sensor pin connections
Upload to ThingSpeak dir contains code snippets for data acquisition and real-time data monitoring being uploaded every 15 seconds on open open-source web server  ThingSpeak by Mathworks

Most of the sensors are readily market-available with open-source libraries and example code
A relatively new Optical Rain Sensor Rg15 module has been tested in the above project

link to its website https://rainsensors.com/products/rg-15/
