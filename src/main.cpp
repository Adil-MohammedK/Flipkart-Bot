/*

 It connects to an MQTT server then:
  - on 0 switches off relay
  - on 1 switches on relay

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 The current state is stored in EEPROM and restored on bootup

*/

#include <ESP8266WiFi.h>
// #include<Wifi.h>  Uncomment for ESP32
#include <PubSubClient.h>
#include "passwords.h" // credentials stored in this header as
// #define WIFI_SSID "ssid"
// #define WIFI_PASS "pass"
// #define MQTT_SERVER "ip"
// #define MQTT_USER "username"
// #define MQTT_PASS "pass"



// Fill out the credentials
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASS;

const char *mqtt_server = MQTT_SERVER;
// Uncomment below if MQTT uses passowrds
// const char *mqtt_username = MQTT_USER; 
// const char *mqtt_password = MQTT_PASS;
// The client id identifies the ESP8266 device. Think of it a bit like a hostname (Or just a name, like Greg).
const char *clientID = "Bot1";

WiFiClient espClient;
PubSubClient client(mqtt_server, 1883, espClient);


// For ESP8266
// //Motor one
// // #define ENA 2
// #define IN1 5
// #define IN2 4

// //Motor two
// #define IN3 14
// #define IN4 15
// // #define ENB 13

// For ESP32
//Motor one
// #define ENA 2
#define IN1 23 
#define IN2 22

//Motor two
#define IN3 21
#define IN4 19
// #define ENB 13

#define Speed 200

void forward() {
  // analogWrite(ENA, Speed);
  // analogWrite(ENB, Speed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void back() {
  // analogWrite(ENA, Speed);
  // analogWrite(ENB, Speed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
void turnright() {
  // analogWrite(ENA, Speed);
  // analogWrite(ENB, Speed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void turnleft() {
  // analogWrite(ENA, Speed);
  // analogWrite(ENB, Speed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
void Stop() {
  // analogWrite(ENA, 0);
  // analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// Don't change the function below. This functions connects your ESP8266 to your router
void setup_wifi()
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

// This functions is executed when some device publishes a message to a topic that your ESP8266 is subscribed to
// Change the function below to add logic to your program, so when a device publishes a message to a topic that
// your ESP8266 is subscribed you can actually do something
void callback(String topic, byte *message, unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic home/office/esp1/gpio2, you check if the message is either 1 or 0. Turns the ESP GPIO according to the message
  if (topic == "Bot1/F")
  {
    Serial.print("Changing forward to ");
    if (messageTemp == "1")
    {
      // digitalWrite(IN2, HIGH);
      forward();
      delay(1000);
      Serial.print("On");
    }
    else if (messageTemp == "0")
    {
      // digitalWrite(IN2, LOW);
      Stop();
      Serial.print("Off");
    }
  }
  if (topic == "Bot1/B")
  {
    Serial.print("Changing back to ");
    if (messageTemp == "1")
    {
      // digitalWrite(IN1, HIGH);
      back();
      delay(1000);
      Serial.print("On");
    }
    else if (messageTemp == "0")
    {
      digitalWrite(IN1, LOW);
      Stop();
      Serial.print("Off");
    }
  }
  if (topic == "Bot1/R")
  {
    Serial.print("Changing rear to ");
    if (messageTemp == "1")
    {
      // digitalWrite(IN1, HIGH);
      turnright();
      delay(500);
      Serial.print("On");
    }
    else if (messageTemp == "0")
    {
      digitalWrite(IN1, LOW);
      Stop();
      Serial.print("Off");
    }
  }
  if (topic == "Bot1/L")
  {
    Serial.print("Changing left to ");
    if (messageTemp == "1")
    {
      // digitalWrite(IN1, HIGH);
      turnleft();
      delay(500);
      Serial.print("On");
    }
    else if (messageTemp == "0")
    {
      digitalWrite(IN1, LOW);
      Stop();
      Serial.print("Off");
    }
  }
  Serial.println();
}

// This functions reconnects your ESP8266 to your MQTT broker
// Change the function below if you want to subscribe to more topics with your ESP8266
void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Uncomment if using passowrds
    // if (client.connect(clientID, mqtt_username, mqtt_password))
    if (client.connect(clientID))
    {
      Serial.println("connected");
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
      client.subscribe("Bot1/F");
      client.subscribe("bot1/B");
      client.subscribe("Bot1/R");
      client.subscribe("Bot1/L");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// The setup function sets your ESP GPIOs to Outputs, starts the serial communication at a baud rate of 115200
// Sets your mqtt broker and sets the callback function
// The callback function is what receives messages and actually controls the LEDs
void setup()
{
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

// For this project, you don't need to change anything in the loop function.
// Basically it ensures that you ESP is connected to your broker
void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  if (!client.loop())
    
    client.connect(clientID);
    // Uncomment if using passwords
    // client.connect(clientID, mqtt_username, mqtt_password);

}