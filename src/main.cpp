/*

 It connects to an MQTT server then:
  - on 0 switches off relay
  - on 1 switches on relay
  - on 2 switches the state of the relay

  - sends 0 on off relay
  - sends 1 on on relay

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 The current state is stored in EEPROM and restored on bootup

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
// #include <EEPROM.h>
#include "passwords.h" // credentials stored in this header as
// #define WIFI_SSID "ssid"
// #define WIFI_PASS "pass"
// #define MQTT_SERVER "ip"
// #define MQTT_USER "username"
// #define MQTT_PASS "pass"

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASS;
const char *mqtt_server = MQTT_SERVER;
const char *mqtt_username = MQTT_USER;
const char *mqtt_password = MQTT_PASS;
// The client id identifies the ESP8266 device. Think of it a bit like a hostname (Or just a name, like Greg).
const char *clientID = "ESP8266";

WiFiClient espClient;
PubSubClient client(mqtt_server, 1883, espClient);

// Connect an LED to each GPIO of your ESP8266
const int ledGPIO5 = 5;
const int ledGPIO4 = 4;
const int ledGPIO13 = 15;
const int ledGPIO15 = 13;
//Motor one
#define ENA 2
#define IN1 5
#define IN2 4

//Motor two
#define IN3 14
#define IN4 15
#define ENB 13

#define Speed 200

void forward() {
  analogWrite(ENA, Speed);
  analogWrite(ENB, Speed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void back() {
  analogWrite(ENA, Speed);
  analogWrite(ENB, Speed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
void turnright() {
  analogWrite(ENA, Speed);
  analogWrite(ENB, Speed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void turnleft() {
  analogWrite(ENA, Speed);
  analogWrite(ENB, Speed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
void Stop() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
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
  if (topic == "esp8266/F")
  {
    Serial.print("Changing forward to ");
    if (messageTemp == "1")
    {
      // digitalWrite(ledGPIO4, HIGH);
      forward();
      delay(150);
      Serial.print("On");
    }
    else if (messageTemp == "0")
    {
      // digitalWrite(ledGPIO4, LOW);
      Stop();
      Serial.print("Off");
    }
  }
  if (topic == "esp8266/B")
  {
    Serial.print("Changing back to ");
    if (messageTemp == "1")
    {
      // digitalWrite(ledGPIO5, HIGH);
      back();
      delay(200);
      Serial.print("On");
    }
    else if (messageTemp == "0")
    {
      digitalWrite(ledGPIO5, LOW);
      Stop();
      Serial.print("Off");
    }
  }
  if (topic == "esp8266/R")
  {
    Serial.print("Changing rear to ");
    if (messageTemp == "1")
    {
      // digitalWrite(ledGPIO5, HIGH);
      turnright();
      delay(200);
      Serial.print("On");
    }
    else if (messageTemp == "0")
    {
      digitalWrite(ledGPIO5, LOW);
      Stop();
      Serial.print("Off");
    }
  }
  if (topic == "esp8266/L")
  {
    Serial.print("Changing left to ");
    if (messageTemp == "1")
    {
      // digitalWrite(ledGPIO5, HIGH);
      turnleft();
      delay(200);
      Serial.print("On");
    }
    else if (messageTemp == "0")
    {
      digitalWrite(ledGPIO5, LOW);
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
    // Attempt to connect
    /*
     YOU  NEED TO CHANGE THIS NEXT LINE, IF YOU'RE HAVING PROBLEMS WITH MQTT MULTIPLE CONNECTIONS
     To change the ESP device ID, you will have to give a unique name to the ESP8266.
     Here's how it looks like now:
       if (client.connect("ESP8266Client")) {
     If you want more devices connected to the MQTT broker, you can do it like this:
       if (client.connect("ESPOffice")) {
     Then, for the other ESP:
       if (client.connect("ESPGarage")) {
      That should solve your MQTT multiple connections problem

     THE SECTION IN loop() function should match your device name
    */
    if (client.connect(clientID, mqtt_username, mqtt_password))
    {
      Serial.println("connected");
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
      client.subscribe("esp8266/F");
      client.subscribe("esp8266/B");
      client.subscribe("esp8266/R");
      client.subscribe("esp8266/L");
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

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

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
    /*
     YOU  NEED TO CHANGE THIS NEXT LINE, IF YOU'RE HAVING PROBLEMS WITH MQTT MULTIPLE CONNECTIONS
     To change the ESP device ID, you will have to give a unique name to the ESP8266.
     Here's how it looks like now:
       client.connect("ESP8266Client");
     If you want more devices connected to the MQTT broker, you can do it like this:
       client.connect("ESPOffice");
     Then, for the other ESP:
       client.connect("ESPGarage");
      That should solve your MQTT multiple connections problem

     THE SECTION IN recionnect() function should match your device name
    */
    client.connect(clientID, mqtt_username, mqtt_password);
}