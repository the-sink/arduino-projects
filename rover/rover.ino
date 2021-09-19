#define IN1 32
#define IN2 33
#define IN3 25
#define IN4 26
#define ENA 27
#define ENB 14
#define POWER 12
#define BUZZER 13

//    Left motor truth table
//  ENA         IN1               IN2         Description  
//  LOW   Not Applicable    Not Applicable    Motor is off
//  HIGH        LOW               LOW         Motor is stopped (brakes)
//  HIGH        HIGH              LOW         Motor is on and turning forwards
//  HIGH        LOW               HIGH        Motor is on and turning backwards
//  HIGH        HIGH              HIGH        Motor is stopped (brakes)

//    Right motor truth table
//  ENB         IN3             IN4         Description  
//  LOW   Not Applicable   Not Applicable   Motor is off
//  HIGH        LOW             LOW         Motor is stopped (brakes)
//  HIGH        LOW             HIGH        Motor is on and turning forwards
//  HIGH        HIGH            LOW         Motor is on and turning backwards
//  HIGH        HIGH            HIGH        Motor is stopped (brakes)  

//    The direction of the car's movement
//  Left motor    Right motor     Description  
//  stop(off)     stop(off)       Car is stopped
//  forward       forward         Car is running forwards
//  forward       backward        Car is turning right
//  backward      forward         Car is turning left
//  backward      backward        Car is running backwards

#include <WiFi.h>
#include <analogWrite.h>

// Replace with your network credentials
const char* ssid = "";
const char* password = "";
WiFiServer server(80);

String header;

unsigned long previousMillis = 0;
unsigned long interval = 300;
unsigned long currentTime = millis();
unsigned long previousTime = 0;
unsigned long timeoutTime = 2000;

bool fault = false;

void play_tone(int freq, int len) {
  ledcWriteTone(0,freq);
  delay(len);
  ledcWrite(0, 0);
}

void stop_movement() {
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

void left() {
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
  analogWrite(ENA, 245);
  analogWrite(ENB, 245);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void forwardleft() {
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
  analogWrite(ENA, 170);
  analogWrite(ENB, 255);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void forwardright() {
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
  analogWrite(ENA, 255);
  analogWrite(ENB, 170);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void backwardleft() {
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
  analogWrite(ENA, 170);
  analogWrite(ENB, 255);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void backwardright() {
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
  analogWrite(ENA, 255);
  analogWrite(ENB, 170);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void right() {
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
  analogWrite(ENA, 245);
  analogWrite(ENB, 245);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void backward() {
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
  analogWrite(ENA, 255);
  analogWrite(ENB, 255);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void forward() {
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
  analogWrite(ENA, 255);
  analogWrite(ENB, 255);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void power() {
  digitalWrite(POWER, !digitalRead(POWER));
  play_tone(440, 25);
}

void setup()
{
pinMode(ENA, OUTPUT);
pinMode(ENB, OUTPUT);
pinMode(IN1, OUTPUT);
pinMode(IN2, OUTPUT);
pinMode(IN3, OUTPUT);
pinMode(IN4, OUTPUT);
pinMode(POWER, OUTPUT);
pinMode(BUZZER, OUTPUT);
pinMode(2, OUTPUT);

//WiFi.mode(WIFI_MODE_APSTA);
WiFi.softAP("rover");

digitalWrite(ENA, LOW);
digitalWrite(ENB, LOW);
digitalWrite(POWER, LOW);

ledcSetup(0,1E5,12);
ledcAttachPin(BUZZER,0);

digitalWrite(2, LOW);
  
Serial.begin(115200);
//Serial.print("Connecting to ");
//Serial.println(ssid);
//WiFi.begin(ssid, password);
//while (WiFi.status() != WL_CONNECTED) {
  delay(500);
//  Serial.print(".");
//}
// Print local IP address and start web server
Serial.println("");
Serial.println("WiFi connected.");
Serial.println("IP address: ");
Serial.println(WiFi.localIP());
Serial.print("IP as soft AP: ");
Serial.println(WiFi.softAPIP());
server.begin();
digitalWrite(2, HIGH);
play_tone(440, 30);
delay(20);
play_tone(440, 30);
delay(500);
digitalWrite(2, LOW);
}


void loop()
{
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            digitalWrite(2, LOW);
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /forward") >= 0) {
              forward();
            } else if (header.indexOf("GET /backward") >= 0) {
              backward();
            } else if (header.indexOf("GET /left") >= 0) {
              left();
            } else if (header.indexOf("GET /right") >= 0) {
              right();
            } else if (header.indexOf("GET /stop") >= 0) {
              stop_movement();
            } else if (header.indexOf("GET /power") >= 0) {
              power();
            } else if (header.indexOf("GET /forwardleft") >= 0) {
              forwardleft();
            } else if (header.indexOf("GET /forwardright") >= 0) {
              forwardright();
            } else if (header.indexOf("GET /backwardleft") >= 0) {
              backwardleft();
            } else if (header.indexOf("GET /backwardright") >= 0) {
              backwardright();
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>Rover Controller</h1>");

            // Input buttons
            client.println("<p style=\"user-select: none;\"><button class=\"button\" id=\"forwardleft\">FL</button><button class=\"button\" id=\"forward\">F</button><button class=\"button\" id=\"forwardright\">FR</button></p>");
            client.println("<p style=\"user-select: none;\"><button class=\"button\" id=\"left\">L</button><button class=\"button\" id=\"stop\">S</button><button class=\"button\" id=\"right\">R</button></p>");
            client.println("<p style=\"user-select: none;\"><button class=\"button\" id=\"backwardleft\">BL</button><button class=\"button\" id=\"backward\">B</button><button class=\"button\" id=\"backwardright\">BR</button></p>");

            client.println("<p>Buttons only work on mobile devices.</p>");

            // Script for sending HTTP requests
            client.println("<script>");
            client.println("var xmlHttp = new XMLHttpRequest();");
            client.println("var buttons = document.getElementsByClassName(\"button\");");
            client.println("Array.from(buttons).forEach((element) => {");
            client.println("element.addEventListener(\"touchstart\", function(){");
            client.println("console.log(\"/\" + element.id);");
            client.println("xmlHttp.open( \"GET\", \"/\" + element.id);");
            client.println("xmlHttp.send(null);");
            client.println("});");
            client.println("element.addEventListener(\"touchend\", function(){");
            client.println("console.log(\"/stop\");");
            client.println("xmlHttp.open( \"GET\", \"/stop\");");
            client.println("xmlHttp.send(null);");
            client.println("});");
            client.println("});");
            client.println("</script>");
            
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("");
  }
}
