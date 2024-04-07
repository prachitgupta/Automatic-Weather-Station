 
#include <SoftwareSerial.h> 
#include <ThreeWire.h>  
#include <RtcDS1302.h>
#include "DHT.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <SD.h>
#include <SPI.h>

const char* ssid     = "P";   // Your network SSID
const char* password = "12345678";   // Your network password
const char* myDomain = "script.google.com";
String scriptUrl = "https://script.google.com/macros/s/AKfycbygK0deC06Zrsv-CNippQP_MHy77UnAjWIunhoNpxOjiY0KLhhZVk_4xTVftaknNcQr/exec"; // Replace with the web app URL
String mimeType = "&mimetype=text/plain"; // Change the MIME type if 
int waitingTime = 30000; // 


SoftwareSerial rg(26, 27); // RX=26 , TX =27
#define DHTPIN2 25
#define DHTTYPE2 DHT22
#define esp_trans 16
#define rg15_trans 17
float temp,humidity,rainfall = 0;
String dateTime,dataString,date;
const int sensorPin = 33;
float sensorValue = 0; //Variable stores the value direct from the analog pin
float sensorVoltage = 0; //Variable that stores the voltage (in Volts) from the anemometer being sent to the analog pin
float windSpeed = 0; // Wind speed in meters per second (m/s)
File dataFile;
//float voltageConversionConstant = 0.00322265625; //This constant maps the value provided from the analog read function, which ranges from 0 to 1023, to actual voltage, which ranges from 0V to 5V
//float voltageConversionConstant = 0.004882814; //This constant maps the value provided from the analog read function, which ranges from 0 to 1023, to actual voltage, which ranges from 0V to 5V
float voltageConversionConstant = 0.000805664; 
int sensorDelay = 250; //Delay between sensor readings, measured in milliseconds (ms)
int month, day, year, hour, minute, second = 0;
 
//Anemometer Technical Variables
//The following variables correspond to the anemometer sold by Adafruit, but could be modified to fit other anemometers.
 
float voltageMin = .40; // Mininum output voltage from anemometer in mV.
float windSpeedMin = 0; // Wind speed in meters/sec corresponding to minimum voltage
 
float voltageMax = 2.0; // Maximum output voltage from anemometer in mV.
float windSpeedMax = 32.4; // Wind speed in meters/sec corresponding to maximum voltage

unsigned long previous = second;
unsigned long previous_day = 0;
unsigned long previous_minute = 0;
unsigned long interval = 2
;  // 30s minutes in milliseconds
//unsigned long interval = 5;  
#define MAX_QUEUE_SIZE 366 // Define the maximum storage
String Unuploaded[MAX_QUEUE_SIZE]; // Define a queue of strings
int front = 0; // Front of the queue
int rear = -1; // Rear of the queue
int itemCount = 0; // Counter to track the number of elements in the queue

ThreeWire myWire(15,21,2); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

DHT dht2(DHTPIN2,DHTTYPE2);


void setup() {
  
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);

  Serial.println("");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password); 

  //rg15 setup
  rg.begin(9600);

  //dht22 setup
  dht2.begin();

  //rtc setup plus debug
    Serial.print("compiled: ");
    Serial.print(__DATE__);
    Serial.println(__TIME__);

    Rtc.Begin();

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    getDateTime(compiled);
    Serial.println();

    if (!Rtc.IsDateTimeValid()) 
    {
        // Common Causes:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing

        Serial.println("RTC lost confidence in the DateTime!");
        Rtc.SetDateTime(compiled);
    }

    if (Rtc.GetIsWriteProtected())
    {
        Serial.println("RTC was write protected, enabling writing now");
        Rtc.SetIsWriteProtected(false);
    }

    if (!Rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (now < compiled) 
    {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
    }
    else if (now > compiled) 
    {
        Serial.println("RTC is newer than compile time. (this is expected)");
    }
    else if (now == compiled) 
    {
        Serial.println("RTC is the same as compile time! (not expected but all is fine)");
    }

      // SD Card Initialization
   if (!SD.begin(5)) {
    Serial.println("SD card initialization failed!");
    return;
  }

  // Create a new file for logging
  String filename = getFileName();
 File dataFile = SD.open(filename,FILE_WRITE);
  if(!dataFile) {
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    writeFile(SD, filename.c_str(), "Date   Hour   Temperature   Rainfall   Windspeed \r\n");
  }
  else {
    Serial.println("File already exists");  
  }
  dataFile.close();
  
}

//custom queue , it has come to that because of memory issues
void enqueue(String element) {
  if (itemCount < MAX_QUEUE_SIZE) {
    rear = (rear + 1) % MAX_QUEUE_SIZE;
   Unuploaded[rear] = element;
    itemCount++;
  } else {
    Serial.println("Queue is full");
  }
}

// Function to remove an element from the queue
String dequeue() {
  if (itemCount > 0) {
    String element =  Unuploaded[front];
    front = (front + 1) % MAX_QUEUE_SIZE;
    itemCount--;
    return element;
  } else {
    Serial.println("Queue is empty");
    return "";
  }
}

// Function to check if the queue is empty
bool isEmpty() {
  return itemCount == 0;
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File dataFile = fs.open(path, FILE_APPEND);
    if(!dataFile){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(dataFile.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    dataFile.close();
}

float annemometer(){
    sensorValue = analogRead(sensorPin); //Get a value between 0 and 1023 from the analog pin connected to the anemometer
    sensorVoltage = sensorValue * voltageConversionConstant; //Convert sensor value to actual voltage
    //Convert voltage value to wind speed using range of max and min voltages and wind speed for the anemometer
    if (sensorVoltage < voltageMin){
     windSpeed = 0; //Check if voltage is below minimum value. If so, set wind speed to zero.
    }else {
      windSpeed = (sensorVoltage - voltageMin)*windSpeedMax/(voltageMax - voltageMin);
    }
    //Serial.println(sensorValue);
    return windSpeed;
}

float rg15_data(){//esp transmitting and rg receiving
  digitalWrite(esp_trans,HIGH);
  digitalWrite(rg15_trans,LOW);
  rg.print('r');
  rg.print('\n');
  
  //esp receiving and rg transmitting
  digitalWrite(esp_trans,LOW);
  digitalWrite(rg15_trans,HIGH);
  
  String response = rg.readStringUntil('\n');

  char acc[7], eventAcc[7], totalAcc[7], rInt[7], unit[4];
  sscanf (response.c_str(),"%*s %s %[^,] , %*s %s %*s %*s %s %*s %*s %s", &acc, &unit, &eventAcc, &totalAcc, &rInt);

  rainfall = atof (totalAcc);
  Serial.println(rainfall);
  
  return rainfall;
}
String getFileName() {
  String fileName;
  int index = dateTime.indexOf(' ');
  date = dateTime.substring(0, index);
  date = date.substring(0,2) + "-" + date.substring(3,5) + "-" + date.substring(6,10);
  fileName += "/" + date;
  fileName += ".txt";
  return fileName;
}

void saveTestFileToGoogleDrive(String myFilename) {
  Serial.println("Connect to " + String(myDomain));
  WiFiClientSecure client;
  client.setInsecure();
 if (!client.connect("script.google.com", 443)) {
    Serial.println("Connection to Google Script failed!");
    return;
  }

  File file = SD.open(myFilename, FILE_READ); // Replace with the name of the file
  if (!file) {
    Serial.println("File not found!");
    return;
  }

  String headers = "POST " + scriptUrl + " HTTP/1.1\r\n";
  headers += "Host: script.google.com\r\n";
  headers += "Content-Length: " + String(file.size()) + "\r\n";
  headers += "Content-Type: application/octet-stream\r\n\r\n";

  // Upload the file
  client.println(headers);
  while (file.available()) {
    client.write(file.read());
  }

  // Read and print the response
  while (client.available()) {
    String line = client.readStringUntil('\n');
    Serial.println(line);
  }

  file.close();
  client.stop();
}

void savefile(){
  
    previous_day = day;
    //previous_minute = minute;
    String filename = getFileName();
    dataFile.close();
    Serial.println("File saved and closed.");
    //push into queue fifo
    enqueue(filename.c_str());
   // Serial.print("Condition met: Removing element from queue - ");
  }
  
 void upload(){
  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("no wifi");
    delay(500);
  }
  
  else if (WiFi.status() == WL_CONNECTED){
    Serial.println("wifi facility available");
    //String filename = getFileName();
    while(!isEmpty()){ 
    //Serial.println(dequeue());
    String name = dequeue();
    saveTestFileToGoogleDrive(name);
    }
    Serial.println("UPLOADED ALL YET");
    delay(500);   
}
}


void saveData(){
  // Append data to the file
  RtcDateTime now = Rtc.GetDateTime();
  
  //unsigned long currentMillis = millis();
  if (second - previous >= interval) {
    previous = second;
    String filename = getFileName();
   appendFile(SD, filename.c_str(), dataString.c_str());  
  }
}

void updateDateTime(const RtcDateTime& dt)
{
    month  =   dt.Month();
    day    =   dt.Day();
    year   =   dt.Year();
    hour   =   dt.Hour();
    minute =   dt.Minute();
    second =   dt.Second();
    
}

String getDateTime(const RtcDateTime& dt){
  char datestring[20];
    
    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    return datestring;
}
    

void loop() {
  //temp rainfall humidity and wind speed
  rainfall = rg15_data();
  temp = dht2.readTemperature();
  humidity = dht2.readHumidity();
  windSpeed = annemometer();
 

  RtcDateTime now = Rtc.GetDateTime();

  dateTime = getDateTime(now);
  updateDateTime(now);
  //day changed goes to savedata

  if (!now.IsValid())
  {
      // Common Causes:
      //    1) the battery on the device is low or even missing and the power line was disconnected
      Serial.println("RTC lost confidence in the DateTime!");
  }
  //integrate
  dataString = dateTime + " " + String(temp) + " " + String(humidity) + " " + String(rainfall) + " " + String(windSpeed) + " " + String(sensorVoltage)  + " " + String(sensorValue)+ "\n";
  //dataString = dateTime + " " + String(temp) + " " + String(humidity) + " " + String(rainfall) + " " + String(windSpeed) + "\n";
  if( day != previous_day){
      Serial.println("day changed");
      savefile();
  }

//if( minute != previous_minute){
//      Serial.println("day changed");
//      savefile();
  
  else{
    saveData();
    upload();
  }
 // delay(5000); // five seconds
}
 
