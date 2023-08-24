#include "RS485_Wind_Direction_Transmitter_V2.h"


/**
   @brief RS485_Wind_Direction_Transmitter_V2 constructor
   @param serial - serial ports for communication, supporting hard and soft serial ports
   @param rx - UART the pin for receiving data
   @param tx - UART the pin for transmitting data
*/
#if defined(ARDUINO_AVR_UNO)||defined(ESP8266)   // use soft serial port
SoftwareSerial softSerial(/*rx =*/4, /*tx =*/5);
RS485_Wind_Direction_Transmitter_V2 windDirection(/*softSerial =*/&softSerial);
#elif defined(ESP32)   // use the hard serial port with remapping pin : Serial1
RS485_Wind_Direction_Transmitter_V2 windDirection(/*hardSerial =*/&Serial1, /*rx =*/D4, /*tx =*/D5);
#else   // use the hard serial port : Serial1
RS485_Wind_Direction_Transmitter_V2 windDirection(/*hardSerial =*/&Serial1);
#endif

uint8_t Address = 0x02;

const char* Orientation[17] = {
  "North", "North-northeast", "Northeast", "East-northeast", "East", "East-southeast", "Southeast", "South-southeast", "South",
  "South-southwest", "Southwest", "West-southwest", "West", "West-northwest", "Northwest", "North-northwest", "North"
};

void setup()
{
  Serial.begin(115200);

  // Init the sensor
  while ( !( windDirection.begin() ) ) {
    Serial.println("Communication with device failed, please check connection");
    delay(3000);
  }
  Serial.println("Begin ok!");
//Change modbus slave address, and you can use 0x00 broadcast address to change it if you forgot the current address.
//  windDirection.SetSlaveAddress(/*the current modbus slave address*/0x00, /*the address after the change*/0x02);
//Get modbus slave address
 Address = windDirection.GetSlaveAddress();
}

void loop()
{
  Serial.println(Address);
  //Get 16 wind directions
  int Direction = windDirection.GetWindDirection(/*modbus slave address*/Address);
  //Get 360° wind direction
  float Angle = windDirection.GetWindAngle(/*modbus slave address*/Address);
  Serial.println(Orientation[Direction]);
  Serial.print(Angle); Serial.println("°");
  Serial.println();
  delay(1000);
}
