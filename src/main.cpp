/*
  AWS IoT WiFi

  This sketch securely connects to an AWS IoT using MQTT over WiFi.
  It uses a private key stored in the ATECC508A and a public
  certificate for SSL/TLS authetication.

  It publishes a message every 5 seconds to arduino/outgoing
  topic and subscribes to messages on the arduino/incoming
  topic.

  The circuit:
  - Arduino MKR WiFi 1010 or MKR1000

  The following tutorial on Arduino Project Hub can be used
  to setup your AWS account and the MKR board:

  https://create.arduino.cc/projecthub/132016/securely-connecting-an-arduino-mkr-wifi-1010-to-aws-iot-core-a9f365

  This example code is in the public domain.
*/




#include <ArduinoBearSSL.h>
#include <ArduinoECCX08.h>
#include <ArduinoMqttClient.h>
#include <WiFiNINA.h> 
#include <Arduino_MKRENV.h>
#include <arduinoUnity.h>
#include <stdio.h>
#include "arduino_secrets.h"

extern ENVClass ENV;

void connectWiFi();
unsigned long getTime();
void connectMQTT();
void publishMessage();
void onMessageReceived(int messageSize);

/////// Use Serial output for debugging purposes /////////
// USE_SERIAL = 1: Serial output, check output via Serial Monitor cannot be used as IoT Device without Serial connection via USB
// USE_SERIAL = 0: Enables device to connect to AWS IoT Core without USB connection to computer - only needs power via USB to work.
uint8_t USE_SERIAL = 0;



/////// Enter your sensitive data in arduino_secrets.h
const char ssid[]        = SECRET_SSID;
const char pass[]        = SECRET_PASS;
const char broker[]      = SECRET_BROKER;
const char* certificate  = SECRET_CERTIFICATE;

WiFiClient    wifiClient;            // Used for the TCP socket connection
BearSSLClient sslClient(wifiClient); // Used for SSL/TLS connection, integrates with ECC508
MqttClient    mqttClient(sslClient);

unsigned long lastMillis = 0;

void setup() {
  
  // --- Serial output -- uncomment Serial commands in order to debug with Serial monitor
  // --- Deactivated to automatically send MQTT messages after booting Arduino
  if(USE_SERIAL == 1)
  {
    Serial.begin(115200);
    while (!Serial);
  }
    

  if (!ECCX08.begin()) 
  {
    if(USE_SERIAL == 1)
    {
      Serial.println("No ECCX08 present!");
    }
    while (1);
  }

ENV.begin();
if (!ENV.begin()) 
{
  if(USE_SERIAL == 1)
  {
    Serial.println("Failed to initialize MKR ENV shield!");
  }
  while (1);
}

  // Set a callback to get the current time
  // used to validate the servers certificate
  ArduinoBearSSL.onGetTime(getTime);

  // Set the ECCX08 slot to use for the private key
  // and the accompanying public certificate for it
  sslClient.setEccSlot(0, certificate);


  // Set the message callback, this function is
  // called when the MQTTClient receives a message
  mqttClient.onMessage(onMessageReceived);
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED) 
  {
    connectWiFi();
  }

  if (!mqttClient.connected()) 
  {
    // MQTT client is disconnected, connect
    connectMQTT();
  }

  // poll for new MQTT messages and send keep alives
  mqttClient.poll();

  // publish a message roughly every 60 seconds.
  if (millis() - lastMillis > 60000) {
    
    lastMillis = millis();

    publishMessage();
  }
}

unsigned long getTime() 
{
  // get the current time from the WiFi module  
  return WiFi.getTime();
}

void connectWiFi()
{
  if(USE_SERIAL == 1)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.print(ssid);
    Serial.print(" ");
  }

  while (WiFi.begin(ssid, pass) != WL_CONNECTED)
  {
    // failed, retry
    if(USE_SERIAL == 1)
    {
      Serial.print(".");
    }
      delay(5000);
  }

  if(USE_SERIAL == 1)
  {
    Serial.println();
    Serial.println("You're connected to the network");
    Serial.println();
  }
}

void connectMQTT() {
  if(USE_SERIAL == 1)
  {
    Serial.print("Attempting to MQTT broker: ");
    Serial.print(broker);
    Serial.println(" ");
  }

  while (!mqttClient.connect(broker, 8883))
  {
    // failed, retry
    if(USE_SERIAL == 1)
    {
      Serial.print(".");
    }
    delay(5000);
  }

  if(USE_SERIAL == 1)
  {
    Serial.println();
    Serial.println("You're connected to the MQTT broker");
    Serial.println();
  }

  // subscribe to a topic
  mqttClient.subscribe("arduino/incoming");
}

void publishMessage() {
  //Serial.println("Publishing message");

  // Output sensor data via Serial Monitor
  if(USE_SERIAL == 1)
  {
    Serial.println("{");
    Serial.print(" \"timestamp\": ");
    Serial.print(getTime());
    Serial.println(",");
    Serial.print(" \"temp\": ");
    Serial.print(ENV.readTemperature());
    Serial.println(",");
    Serial.print(" \"hum\": ");
    Serial.print(ENV.readHumidity());
    Serial.println(",");
    Serial.print(" \"pres\": ");
    Serial.print(ENV.readPressure());
    Serial.println(",");
    Serial.print(" \"ill\": ");
    Serial.print(ENV.readIlluminance());
    Serial.println(",");
    Serial.print(" \"uv\": ");
    Serial.println(ENV.readUVIndex());
    Serial.println("}");
  }


  // Publish message over MQTT.
  if(!USE_SERIAL == 0)
  {
    mqttClient.beginMessage("iot/topic/outgoing");
    mqttClient.println("{");
    mqttClient.print(" \"timestamp\": ");
    mqttClient.print(getTime());
    mqttClient.println(",");
    mqttClient.print(" \"temp\": ");
    mqttClient.print(ENV.readTemperature());
    mqttClient.println(",");
    mqttClient.print(" \"hum\": ");
    mqttClient.print(ENV.readHumidity());
    mqttClient.println(",");
    mqttClient.print(" \"pres\": ");
    mqttClient.print(ENV.readPressure());
    mqttClient.println(",");
    mqttClient.print(" \"ill\": ");
    mqttClient.print(ENV.readIlluminance());
    mqttClient.println(",");
    mqttClient.print(" \"uv\": ");
    mqttClient.println(ENV.readUVIndex());
    mqttClient.println("}");
    mqttClient.endMessage();
  }
}

void onMessageReceived(int messageSize) {

  // If Serial monitor enables: we received a message, print out the topic and contents
  if(USE_SERIAL == 1)
  {
  Serial.print("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

  // use the Stream interface to print the contents
  while (mqttClient.available()) {
    Serial.print((char)mqttClient.read());
  }
  Serial.println();
  Serial.println();
  }

}

