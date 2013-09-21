/*
  LED Web Server
 
 A simple web server that sets colors on LED strip.
 
 GET request to the IP address expects the format:
 http://10.0.1.160/RRGGBB
 where RRGGBB is a hex RGB value
 
 Circuit:
 * Analog output attached to pin 3, 5 & 6
 * Ethernet Shield
 
 created Sept 2013
 by Seth Faxon
 
 */

#include <SPI.h>
#include <Ethernet.h>

// Hard coded IP address, update to something that will work on your network
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(10,0,1,160);

// Listen on port 80
EthernetServer server(80);

char color[6];    // red/gree/blue at 2 chars each

String stringBuffer; //string for fetching data from url path
String getRequest;

// could not use pin 4 because the ethenet shield uses it for something
int red = 5;    // LED connected to digital pin 3
int green = 6;    // LED connected to digital pin 5
int blue = 3;    // LED connected to digital pin 6

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  testLights();

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}

#define bufferMax 128
int bufferSize;
char buffer[bufferMax];

void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

 //       Serial.write(c);

        if (stringBuffer.length() < 100) {
          stringBuffer += c;
        }
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && stringBuffer.startsWith("GET")) {
          Serial.println("GET REQUEST:");
          getRequest = stringBuffer.substring(5,11);
          Serial.println(getRequest);
          stringBuffer = "";
        }
        if (c == '\n' && currentLineIsBlank) {

          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");

//            Serial.println(stringBuffer);
          client.println(getRequest);
          Serial.println("calling parseHex");
          int r = parseHex(getRequest.substring(0,2));
          int g = parseHex(getRequest.substring(2,4));
          int b = parseHex(getRequest.substring(4,6));
          Serial.println(r);
          Serial.println(g);
          Serial.println(b);
          analogWrite(red, r);
          analogWrite(green, g);
          analogWrite(blue, b);

          client.println("<br />RED: ");
          client.println(r);
          client.println("<br />GREEN: ");
          client.println(g);
          client.println("<br />BLUE: ");
          client.println(b);

          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection and clear buffers
    client.stop();
    getRequest = "";
    stringBuffer = "";
    Serial.println("client disonnected");
  }
}

// two char string
int parseHex(String s) {
  const char* c = s.c_str();
  return strtol(c, NULL, 16);
}

// runs through red, green, blue in fade pattern to make sure pins are set correctly
void testLights() {
  // fade in/out RED from min to max to min in increments of 5 points:
  for(int fadeValue = 0 ; fadeValue <= 255; fadeValue +=5) { 
    analogWrite(red, fadeValue);         
    // wait for 30 milliseconds to see the dimming effect    
    delay(30);
  }
  for(int fadeValue = 255 ; fadeValue >= 0; fadeValue -=5) { 
    analogWrite(red, fadeValue);         
    // wait for 30 milliseconds to see the dimming effect
    delay(30);
  }

  // fade in/out GREEN from min to max to min
  for(int fadeValue = 0 ; fadeValue <= 255; fadeValue +=5) { 
    analogWrite(green, fadeValue);
    delay(30);
  }
  for(int fadeValue = 255 ; fadeValue >= 0; fadeValue -=5) {
    analogWrite(green, fadeValue);
    delay(30);                            
  } 

  // BLUE
  for(int fadeValue = 0 ; fadeValue <= 255; fadeValue +=5) {
    analogWrite(blue, fadeValue);
    delay(30);
  }
  for(int fadeValue = 255 ; fadeValue >= 0; fadeValue -=5) { 
    analogWrite(blue, fadeValue);
    delay(30);
  }
}

