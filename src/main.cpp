#include <ESP8266WiFi.h>

#define gpio4LEDPin 4 /* One LED connected to GPIO4 - D2 */
#define gpio5LEDPin 5 /* One LED connected to GPIO5 - D1 */

const char* ssid = "ssid"; /* Add your router's SSID */
const char* password = "pass"; /*Add the password */

int gpio4Value; 
int gpio5Value;

WiFiServer espServer(80); /* Instance of WiFiServer with port number 80 */
/* 80 is the Port Number for HTTP Web Server */

void setup() 
{
  Serial.begin(115200); /* Begin Serial Communication with 115200 Baud Rate */
  /* Configure GPIO4 and GPIO5 Pins as OUTPUTs */
  pinMode(gpio4LEDPin, OUTPUT);
  pinMode(gpio5LEDPin, OUTPUT);
  /* Set the initial values of GPIO4 and GPIO5 as LOW*/
  /* Both the LEDs are initially OFF */
  digitalWrite(gpio4LEDPin, LOW);
  digitalWrite(gpio5LEDPin, LOW);
  
  Serial.print("\n");
  Serial.print("Connecting to: ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA); /* Configure ESP8266 in STA Mode */
  WiFi.begin(ssid, password); /* Connect to Wi-Fi based on above SSID and Password */
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print("*");
    delay(500);
  }
  Serial.print("\n");
  Serial.print("Connected to Wi-Fi: ");
  Serial.println(WiFi.SSID());
  delay(100);
  /* The next four lines of Code are used for assigning Static IP to ESP8266 */
  /* Do this only if you know what you are doing */
  /* You have to check for free IP Addresses from your Router and */
  /* assign it to ESP8266 */
  /* If you are confirtable with this step, please un-comment the next four lines *
  /* if not, leave it as it is and proceed */
  //IPAddress ip(192,168,1,6);   
  //IPAddress gateway(192,168,1,1);   
  //IPAddress subnet(255,255,255,0);   
  //WiFi.config(ip, gateway, subnet);
  //delay(2000);
  Serial.print("\n");
  Serial.println("Starting ESP8266 Web Server...");
  espServer.begin(); /* Start the HTTP web Server */
  Serial.println("ESP8266 Web Server Started");
  Serial.print("\n");
  Serial.print("The URL of ESP8266 Web Server is: ");
  Serial.print("http://");
  Serial.println(WiFi.localIP());
  Serial.print("\n");
  Serial.println("Use the above URL in your Browser to access ESP8266 Web Server\n");
}

void loop()
{
  WiFiClient client = espServer.available(); /* Check if a client is available */
  if(!client)
  {
    return;
  }

  Serial.println("New Client!!!");

  String request = client.readStringUntil('\r'); /* Read the first line of the request from client */
  Serial.println(request); /* Print the request on the Serial monitor */
  /* The request is in the form of HTTP GET Method */ 
  client.flush();

  /* Extract the URL of the request */
  /* We have four URLs. If IP Address is 192.168.1.6 (for example),
   * then URLs are: 
   * 192.168.1.6/GPIO4ON and its request is GET /GPIO4ON HTTP/1.1
   * 192.168.1.6/GPIO4OFF and its request is GET /GPIO4OFF HTTP/1.1
   * 192.168.1.6/GPIO5ON and its request is GET /GPIO5ON HTTP/1.1
   * 192.168.1.6/GPIO4OFF and its request is GET /GPIO5OFF HTTP/1.1
   */
   /* Based on the URL from the request, turn the LEDs ON or OFF */
  if (request.indexOf("/GPIO4ON") != -1) 
  {
    Serial.println("GPIO4 LED is ON");
    digitalWrite(gpio4LEDPin, HIGH);
    gpio4Value = HIGH;
  } 
  if (request.indexOf("/GPIO4OFF") != -1)
  {
    Serial.println("GPIO4 LED is OFF");
    digitalWrite(gpio4LEDPin, LOW);
    gpio4Value = LOW;
  }
  if (request.indexOf("/GPIO5ON") != -1) 
  {
    Serial.println("GPIO5 LED is ON");
    digitalWrite(gpio5LEDPin, HIGH);
    gpio5Value = HIGH;
  } 
  if (request.indexOf("/GPIO5OFF") != -1)
  {
    Serial.println("GPIO5 LED is OFF");
    digitalWrite(gpio5LEDPin, LOW);
    gpio5Value = LOW;
  }


  /* HTTP Response in the form of HTML Web Page */
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(); //  IMPORTANT
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head>");
  client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  client.println("<link rel=\"icon\" href=\"data:,\">");
  /* CSS Styling for Buttons and Web Page */
  client.println("<style>");
  client.println("html { font-family: Courier New; display: inline-block; margin: 0px auto; text-align: center;}");
  client.println(".button {border: none; color: white; padding: 10px 20px; text-align: center;");
  client.println("text-decoration: none; font-size: 25px; margin: 2px; cursor: pointer;}");
  client.println(".button1 {background-color: #13B3F0;}");
  client.println(".button2 {background-color: #3342FF;}");
  client.println("</style>");
  client.println("</head>");
  
  /* The main body of the Web Page */
  client.println("<body>");
  client.println("<h2>ESP8266 Web Server</h2>");
  
  if(gpio4Value == LOW) 
  {
    client.println("<p>GPIO4 LED Status: OFF</p>");
    client.print("<p><a href=\"/GPIO4ON\"><button class=\"button button1\">Click to turn ON</button></a></p>");  
  } 
  else 
  {
    client.println("<p>GPIO4 LED Status: ON</p>");
    client.print("<p><a href=\"/GPIO4OFF\"><button class=\"button button2\">Click to turn OFF</button></a></p>"); 
  }
  
  if(gpio5Value == LOW) 
  {
    client.println("<p>GPIO5 LED Status: OFF</p>");
    client.print("<p><a href=\"/GPIO5ON\"><button class=\"button button1\">Click to turn ON</button></a></p>");  
  } 
  else 
  {
    client.println("<p>GPIO5 LED Status: ON</p>");
    client.print("<p><a href=\"/GPIO5OFF\"><button class=\"button button2\">Click to turn OFF</button></a></p>");  
  }
  
  client.println("</body>");
  client.println("</html>");
  client.print("\n");
  
  delay(1);
  /* Close the connection */
  client.stop();
  Serial.println("Client disconnected");
  Serial.print("\n");
}