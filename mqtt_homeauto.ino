/*
 MQTT home automation
 this is a general template for my home automation
 
  - connects to an MQTT server
  - subscribes to the topic "inTopic"
  - publish status to outTopic
  
  using callback to respond commands
  version: 0.1
  status: working code
  todo : - debouncing is still one-by-one
         - change protocol to vscp
*/
#include <Bounce.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
byte server[] = { 192, 168, 35, 200 };
//byte ip[]     = { 172, 16, 0, 100 };



// Callback function header
void callback(char* topic, byte* payload, unsigned int length);

EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);
unsigned long time;

int totalAppl = 4;
char* roomName = "RuangKaca";
char* myAppliance[] ={"RuangKaca1","RuangKaca2","RuangKaca3","RuangKaca4"};
int ardPinRelay[]={6,7,8,9};      
int statusAppl[]={0,0,0,0};        //Status appliance sewaktu startup
                                       //di arduino bisa membaca digitalread untuk analog Pin, untuk Uno Pin
                                       //analog di hitung mulai dari pin 14 sd 19
int mappedAnalogInput[]={16,17,18,19}; //Analog Input yang digunakan sebagai Button harus map dengan myAppliance

long lastDebounceTime = 0;
long debounceDelay = 50;
int lastButtonState[] = {0,0,0,0}; 
long blink_timer = millis();  
  
int LED_INFO = 3; // pin digital 2
boolean client_connect;
int ledState = 0;
char message_buff[200];
String sstatus;
// defines and variable for sensor/control mode
#define MODE_OFF    0  // not sensing light, LED off
#define MODE_ON     1  // not sensing light, LED on
#define MODE_SENSE  2  // sensing light, LED controlled by software
#define MODE_READ  3

Bounce bouncer1 = Bounce(A2,15);
Bounce bouncer2 = Bounce(A3,15);
Bounce bouncer3 = Bounce(A4,15);
Bounce bouncer4 = Bounce(A5,15);
Bounce bouncer5 = Bounce(A0,15);
Bounce bouncer6 = Bounce(A1,15);

// Callback function
void callback(char* topic, byte* payload, unsigned int length) {
  // In order to republish this payload, a copy must be made
  // as the orignal payload buffer will be overwritten whilst
  // constructing the PUBLISH packet.
  
  // Allocate the correct amount of memory for the payload copy
  ///byte* p = (byte*)malloc(length);
  // Copy the payload to the new buffer
  ///memcpy(p,payload,length);
  ///client.publish("outTopic", p, length);
  // Free the memory
  ///free(p);
  

  int i = 0;

  //Serial.println("Message arrived:  topic: " + String(topic));
  //Serial.println("Length: " + String(length,DEC));
  
  // create character buffer with ending null terminator (string)
  for(i=0; i<length; i++) {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';
  
  String msgString = String(message_buff);
  
  //Serial.println("Payload: " + msgString);

  for (int thisPin=0;thisPin<totalAppl;thisPin++){  
    if (msgString.equals("{\"command\":{\""+ String(myAppliance[thisPin])+"\": \"OFF\"}}")) {
      statusAppl[thisPin] = MODE_OFF;
    } else if (msgString.equals("{\"command\":{\""+String(myAppliance[thisPin])+"\": \"ON\"}}")) {
      statusAppl[thisPin] = MODE_ON;
    } else if (msgString.equals("{\"command\":{\""+String(myAppliance[thisPin])+"\": \"SENSE\"}}")) {
      statusAppl[thisPin] = MODE_SENSE;
    } else if (msgString.equals("{\"command\":{\""+String(myAppliance[thisPin])+"\": \"READ\"}}")) {
      statusAppl[thisPin] = MODE_READ;
    }
    
    
  }
 
  
}

void setup()
{
pinMode(LED_INFO,OUTPUT);
//pinMode(14,INPUT);
//pinMode(15,INPUT);
//digitalWrite(14,LOW);
//digitalWrite(15,LOW);

  Ethernet.begin(mac);
  if (client.connect(roomName)) {
    client.publish("outTopic","I AM CONNECTED");
    client.subscribe("inTopic");
  }
  
  for (int anPin = 0; anPin < totalAppl; anPin++){
    pinMode(ardPinRelay[anPin],OUTPUT);
    digitalWrite(ardPinRelay[anPin],HIGH);
  }
}

void loop()
{
  
    if (!ethClient.connected()){
        Ethernet.begin(mac);
        if (client.connect(roomName)) { 
          client.publish("outTopic","I AM CONNECTED");
          client.subscribe("inTopic");
         }
    } else if (ethClient.connected()){
      digitalWrite(LED_INFO, HIGH);
    }
   
    if (!client.connected()) {
       client_connect = false;
       client.connect(roomName);
       client.publish("outTopic","RECONNECTED");
       client.subscribe("inTopic");
    } 
  
//for led status
// always on- connected
// blinking - not connected

    if (!client.connected()){
        if (millis() > (blink_timer + 300)){
          blink_timer=millis();
          Serial.println(blink_timer);
          ledState = digitalRead(LED_INFO);
          Serial.println(ledState);
          if (ledState == LOW) {
              digitalWrite(LED_INFO,HIGH);
          }
          else if (ledState == HIGH) {
              digitalWrite(LED_INFO,LOW);
          }
        }
    } else if (client.connected()) {
      digitalWrite(LED_INFO,HIGH);
    }

//-----------bouncer procedure 
     bouncer1.update();
     int value1 = bouncer1.read();
     if ( value1 == HIGH){
       switch(statusAppl[0]){
         case 0:
           statusAppl[0]=1;
           break;
         case 1:
           statusAppl[0]=0;
           break;
       }
         delay(1000);
       }

     bouncer2.update();
     int value2 = bouncer2.read();
     if ( value2 == HIGH){
       switch(statusAppl[1]){
         case 0:
           statusAppl[1]=1;
           break;
         case 1:
           statusAppl[1]=0;
           break;
       }
         delay(1000);
       }

     bouncer3.update();
     int value3 = bouncer3.read();
     if ( value3 == HIGH){
       switch(statusAppl[2]){
         case 0:
           statusAppl[2]=1;
           break;
         case 1:
           statusAppl[2]=0;
           break;
       }
         delay(1000);
       }

     bouncer4.update();
     int value4 = bouncer4.read();
     if ( value4 == HIGH){
       switch(statusAppl[3]){
         case 0:
           statusAppl[3]=1;
           break;
         case 1:
           statusAppl[3]=0;
           break;
       }
         delay(1000);
       }

//     bouncer5.update();
//     int value5 = bouncer5.read();
//     if ( value5 == HIGH){
//       client.publish("outTopic","{\"command\":{\"RK_GROUP1\": \"CLICK\"}}");
//       delay(1000);
//     }
//
//     bouncer6.update();
//     int value6 = bouncer6.read();
//     if ( value6 == HIGH){
//       client.publish("outTopic","{\"command\":{\"RK_GROUP2\": \"CLICK\"}}");
//       delay(1000);
//     }

     
      for (int anPin = 0; anPin < totalAppl; anPin++){
        digitalWrite(ardPinRelay[anPin],!statusAppl[anPin]); 
        delay(100);       
      }


  
      if (millis() > (time + 8000)) {
        time = millis();

        for (int thisPin = 0; thisPin < totalAppl;thisPin++){
            switch(statusAppl[thisPin]){
              case 0:
                sstatus = "OFF";
                break;
              case 1:
                sstatus = "ON";
                break;
              case 2:
                sstatus = "SENSE";
                break;
              case 3:
                sstatus = "READ";
                break;
            }
            String pubString = "{\"report\":{\"" + String(myAppliance[thisPin])+ "\": \""+String(sstatus) +"\"}}";
            pubString.toCharArray(message_buff, pubString.length()+1);
            //Serial.println(pubString);
            client.publish("outTopic", message_buff);
        }
      }

  client.loop();

}

