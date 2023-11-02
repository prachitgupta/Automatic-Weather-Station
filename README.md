# Automatic-Weather-Station

- The following repo contains Arduino C code for integrating sensor data via i2c, spi, and Modbus protocols, uploading data to Thingspeak, and real-time data logging
- Code for testing individual sensor data, using various communication protocols has been provided
- Flashing Final_code_with_real_time_logging will enable you to log temperature, humidity, wind speed, and rainfall accumulation every 15 minutes on an SD card with a timestamp. Daily data will be recorded in separate files
- The code for individual sensors contains the names of the library to be included and sensor pin connections
- Upload to ThingSpeak dir contains code snippets for data acquisition and real-time data monitoring being uploaded every 15 seconds on open open-source web server ThingSpeak by Mathworks

- -To deploy the weather station with given components go to -> isurp_phase1 dir  _. upload rtc.ino to your esp32 board  this will initiate the time of your rtc clock to that of your system at the time of compiling code i.e compiled time
  
- then simply upload gdrive.ino and you are good to go
-   
- change ssid and pasword to that of your network the weather station once powered will continousily log data in sd card with real time clocking and as soon as wifi facility is available , all the unuploaded daily files will get uploaded to AWS folder of gdrive and one can remotely ccess the data without extracting sd card

- Most of the sensors are readily market-available with open-source libraries and example code
- A relatively new Optical Rain Sensor Rg15 module has been tested in the above project

- Link to its website: [https://rainsensors.com/products/rg-15/](https://rainsensors.com/products/rg-15/)
