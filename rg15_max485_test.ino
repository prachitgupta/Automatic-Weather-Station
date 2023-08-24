#include <SoftwareSerial.h> 
#define esp_trans 32
#define rg15_trans 33
float acc;
String response;

SoftwareSerial mod(26, 27); // RX=26 , TX =27
void setup() {
  Serial.begin(115200);
  mod.begin(9600);
  pinMode(esp_trans, OUTPUT);
  pinMode(rg15_trans, OUTPUT);
}


void loop() {

  digitalWrite(esp_trans,HIGH);
  digitalWrite(rg15_trans,LOW);
  mod.print('r');
  mod.print('\n');
  
  digitalWrite(esp_trans,LOW);
  digitalWrite(rg15_trans,HIGH);
  
 String response = mod.readStringUntil('\n');

  char acc[7], eventAcc[7], totalAcc[7], rInt[7], unit[4];
  sscanf (response.c_str(),"%*s %s %[^,] , %*s %s %*s %*s %s %*s %*s %s", &acc, &unit, &eventAcc, &totalAcc, &rInt);

  Serial.println(response);
  /*
  Serial.print("Accumulation: ");
  Serial.print(atof (acc),3);  
  Serial.println(unit);
  Serial.print("Event Accumulation: ");
  Serial.print(atof (eventAcc),3);  
  Serial.println(unit);
  Serial.print("Total Accumulation: ");
  Serial.print(atof (totalAcc),3);  
  Serial.println(unit);
  Serial.print("IPH: ");
  Serial.print(atof (rInt), 3);
  Serial.println(" IPH\n");
  */
  delay(1000);
}
