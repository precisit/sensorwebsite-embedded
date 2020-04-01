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
#include <WiFiNINA.h> // change to #include <WiFi101.h> for MKR1000
#include <Arduino_MKRENV.h>
#include <arduinoUnity.h>
#include <stdio.h>
#include "test.h"


#include "arduino_secrets.h"
extern ENVClass ENV;

void connectWiFi();
unsigned long getTime();
void connectMQTT();
void publishMessage();
void onMessageReceived(int messageSize);


/////// Enter your sensitive data in arduino_secrets.h
const char ssid[]        = SECRET_SSID;
const char pass[]        = SECRET_PASS;
const char broker[]      = SECRET_BROKER;
const char* certificate  = SECRET_CERTIFICATE;

WiFiClient    wifiClient;            // Used for the TCP socket connection
BearSSLClient sslClient(wifiClient); // Used for SSL/TLS connection, integrates with ECC508
MqttClient    mqttClient(sslClient);

unsigned long lastMillis = 0;
int a = 0;

void setup() {
  // --- Serial output -- uncomment Serial commands in order to debug with Serial monitor
  // --- Deactivated to automatically send MQTT messages after booting Arduino
  //Serial.begin(115200);
  //while (!Serial);

  a = 2;
  if (!ECCX08.begin()) {
 //   Serial.println("No ECCX08 present!");
    while (1);
  }

ENV.begin();
if (!ENV.begin()) {
 // Serial.println("Failed to initialize MKR ENV shield!");
  while (1);
}

  // Set a callback to get the current time
  // used to validate the servers certificate
  ArduinoBearSSL.onGetTime(getTime);

  // Set the ECCX08 slot to use for the private key
  // and the accompanying public certificate for it
  sslClient.setEccSlot(0, certificate);

  // Optional, set the client id used for MQTT,
  // each device that is connected to the broker
  // must have a unique client id. The MQTTClient will generate
  // a client id for you based on the millis() value if not set
  //
  // mqttClient.setId("clientId");

  // Set the message callback, this function is
  // called when the MQTTClient receives a message
  mqttClient.onMessage(onMessageReceived);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (!mqttClient.connected()) {
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

unsigned long getTime() {
  // get the current time from the WiFi module  
  return WiFi.getTime();
}

void connectWiFi() {
  //Serial.print("Attempting to connect to SSID: ");
  //Serial.print(ssid);
  //Serial.print(" ");

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    //Serial.print(".");
    delay(5000);
  }
  //Serial.println();

  //Serial.println("You're connected to the network");
  //Serial.println();
}

void connectMQTT() {
  //Serial.print("Attempting to MQTT broker: ");
  //Serial.print(broker);
  //Serial.println(" ");

  while (!mqttClient.connect(broker, 8883)) {
    // failed, retry
    //Serial.print(".");
    delay(5000);
  }
  //Serial.println();

  //Serial.println("You're connected to the MQTT broker");
  //Serial.println();

  // subscribe to a topic
  mqttClient.subscribe("arduino/incoming");
}

void publishMessage() {
  //Serial.println("Publishing message");

  
  // send message, the Print interface can be used to set the message contents
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

void onMessageReceived(int messageSize) {
  // we received a message, print out the topic and contents
  //Serial.print("Received a message with topic '");
  //Serial.print(mqttClient.messageTopic());
  //Serial.print("', length ");
  //Serial.print(messageSize);
  //Serial.println(" bytes:");

  // use the Stream interface to print the contents
  while (mqttClient.available()) {
    Serial.print((char)mqttClient.read());
  }
  //Serial.println();

  //Serial.println();

}

