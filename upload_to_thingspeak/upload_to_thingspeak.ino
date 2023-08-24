
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <SPI.h>


#define RXD2 16
#define TXD2 17

#define mqttPort  1883
#define channelID 1957949


WiFiClient client; //client object

String data_received,Data[30];
int StringCount = 0;


const char* server = "mqtt3.thingspeak.com"; //name of thinkspeak broker
int status = WL_IDLE_STATUS; 
float lastPublishMillis = 0; //initialize time
int updateInterval = 5; //rate at which i want data to be published to thingspeak channel
PubSubClient mqttClient( client ); //wifi client set as mqtt client


const char* ssid     = "P";
const char* password = "";

//update client id and password created by thingspeak while setting up an mqtt device there
const char mqttUserName[] = "DBINMDY6MgMKBiQ2FTctLi0"; 
const char clientID[] = "DBINMDY6MgMKBiQ2FTctLi0";
const char mqttPass[] = "hQJhK7LkBedGjy/kWy7R7t3r";

// Function to handle messages from MQTT subscription.
void mqttSubscriptionCallback( char* topic, byte* payload, unsigned int length ) {
  // Print the details of the message that was received to the serial monitor.
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");
}

//connect to mqtt broker provided by thingspeak
void mqttConnect(){
 while ( !mqttClient.connected() ){
    // Connect to the MQTT broker.
    if ( mqttClient.connect( clientID, mqttUserName, mqttPass ) ) {
      Serial.print( "MQTT to " );
      Serial.print( server );
      Serial.print (" at port ");
      Serial.print( mqttPort );
      Serial.println( " successful." );
    }
    else {
      Serial.print( "MQTT connection failed, rc = " );
      // See https://pubsubclient.knolleary.net/api.html#state for the failure code explanation.
      Serial.print( mqttClient.state() );
      Serial.println( " Will try again in a few seconds" );
      delay(3000);
    }
  }
 }

 void setup()
{
      Serial.begin(9600);
      Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
    
     Serial.begin(115200);//baud rate (not 9600) high freq required
     delay(3000);//some time to establish wifi connection
    
    // connecting to a WiFi network
      wificonnect();
   
     // Configure the MQTT client
      mqttClient.setServer( server, mqttPort ); //established server(broker at selected port
      // Set the MQTT message handler function.
      mqttClient.setCallback( mqttSubscriptionCallback );
      // Set the buffer to handle the returned JSON. NOTE: A buffer overflow of the message buffer will result in your callback not being invoked.
      mqttClient.setBufferSize( 2048 );
}
 

 

 void mqttpublish(long channelId) {
    delay(2000); //delay to take sensor reading
    
    //receive sensor data
    data_received = Serial2.readString();
    while (data_received.length() > 0)
  {
    int index = data_received.indexOf(' ');
    if (index == -1) // No space found
    {
      Data[StringCount++] = data_received;
      break;
    }
    else
    {
      Data[StringCount++] = data_received.substring(0, index);
      data_received = data_received.substring(index+1);
    }
   
  }
 
    // Create a data string to send data to ThingSpeak.
    // Use these lines to publish to a channel feed, which allows multiple fields to be updated simultaneously.
     String datas = String("field1=" + Data[0] + "&field2=" + Data[1]
                           + "&field3=" + Data[2] + "&field4=" + Data[3]);

     String topic = String("channels/"+String(channelId)+ "/publish");
    
    mqttClient.publish( topic.c_str(), datas.c_str() );
}

  //establish connection to wifi
 void wificonnect(){
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password); //enable wifi connection

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  } 
  



void loop() {
  // Reconnect to WiFi if it gets disconnected.
  if (WiFi.status() != WL_CONNECTED) {
      wificonnect();
  }
  
  // Connect if MQTT client is not connected 
  if (!mqttClient.connected()) {
     mqttConnect(); 
  }
  
  // Call the loop to maintain connection to the server.
  mqttClient.loop(); 
  
  // Update ThingSpeak channel periodically. The update results in the message to the subscriber.
  if ( abs(millis() - lastPublishMillis) > updateInterval*1000) {
    mqttpublish(channelID);
    lastPublishMillis = millis();//after 15 sec each update time
  }
}
