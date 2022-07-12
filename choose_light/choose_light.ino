//Libraries for LED
#include <FastLED.h>//https://github.com/FastLED/FastLED
//Constants
#define NUM_STRIPS 1
#define NUM_LEDS 29
#define BRIGHTNESS 10
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB//BRG//RGB
#define FASTLED_ALLOW_INTERRUPTS 0
#define FASTLED_INTERRUPT_RETRY_COUNT 1
#define FRAMES_PER_SECOND 60
#define COOLING 55
#define SPARKING 120
#define NUM_LEDS_FIRE 19
#define FRAMES_PER_SECOND 60
//Parameters
const int stripPin  = D6;
//Variables
bool gReverseDirection  = false;
//Objects
CRGB leds[NUM_LEDS];
// for LED

#include <ESP8266WiFi.h>

const char* ssid     = "Mera-Neon";
const char* password = "feu596275";

volatile int color_1 = 0;
volatile int color_2 = 255;
volatile int color_3 = 0;

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
//this pins for esp32 
//change the pins for esp8266
String Led_In_State = "on";
String bouton_1 = "on";
//String output14State = "off";
//String output27State = "off";

String sliderValue = "10";

void setup() {
  // put your setup code here, to run once:
   Serial.begin(115200);
   pinMode(LED_BUILTIN, OUTPUT);
   delay(1000);
   Serial.println("serial test...");

   // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  FastLED.addLeds<LED_TYPE, stripPin, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(  BRIGHTNESS );
  
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
    WiFiClient client = server.available();   // Listen for incoming clients

    if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
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

            // turns the GPIOs on and off
            //for LED_BUILTIN
            if (header.indexOf("GET /12/on") >= 0) 
            {
              Serial.println("LED_BUILTIN on");
              Led_In_State = "on";
            //  digitalWrite(output12, LOW);
              digitalWrite(LED_BUILTIN, LOW);
            } 
            else if (header.indexOf("GET /12/off") >= 0) 
            {
              Serial.println("LED_BUILTIN off");
              Led_In_State = "off";
             // digitalWrite(output12, HIGH);
              digitalWrite(LED_BUILTIN, HIGH);
            }

            // turns the GPIOs on and off
            //for bouton_1
            if (header.indexOf("GET /01/on") >= 0) 
            {
              bouton_1 = "on";
                color_1 = 255 ;
                color_2 = 255 ;
                color_3 = 0;
            } 
            else if (header.indexOf("GET /01/off") >= 0) 
            {
              bouton_1 = "off";
                color_1 = 255 ;
                color_2 = 0 ;
                color_3 = 255;
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
            client.println(".button2 {background-color: #555555;}");
            //client.println(".button2 {background-color: #555555;}</style>");

            // TEST POUR slider 
            /*
            client.println("<p><div class=\"/ard rangeslider P9\"></div></p><p><div class=\"ard rangeslider P10\"></div></p><p><div class=\"ard rangeslider P11\"></div></p> "); 
            client.println("<p><div class=\"ard rangeslider P9 value-50 range-0-100\"></div></p>");
            client.println(" <p><div class=\"display-color\"></div></p>"); */
            client.println(".slider { -webkit-appearance: none; margin: 14px; width: 360px; height: 25px; background: #FFD65C; outline: none; -webkit-transition: .2s; transition: opacity .2s;}");
            client.println(".slider::-webkit-slider-thumb {-webkit-appearance: none; appearance: none; width: 35px; height: 35px; background: #003249; cursor: pointer;}");
            client.println(".slider::-moz-range-thumb { width: 35px; height: 35px; background: #003249; cursor: pointer; }</style>");
                    
             client.println("</head>");

            // Web Page Heading
            client.println("<body><h1> Mera Neon </h1>");

            client.println("<p><span id=textSliderValue>" + sliderValue+ "</span></p>"); // creation du slider
            client.println("<p><input type=\"range\" onchange=\"updateSliderPWM(this)\" id=\"pwmSlider\" min=\"0\" max=\"255\" value="+sliderValue+"\" step=\"1\" class=\"slider\"></p>");

            // Display current state, and ON/OFF buttons for builtin led  
            client.println("<p> Led In - State " + Led_In_State + "</p>");
            // If the output12State is off, it displays the ON button       
            if (Led_In_State=="on") {
              client.println("<p><a href=\"/12/off\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/12/on\"><button class=\"button button2\">OFF</button></a></p>");
            } 

            // Display current state, and ON/OFF buttons for bouton_1
            client.println("<p> LED color - State " + bouton_1 + "</p>");
            // If the output12State is off, it displays the ON button       
            if (bouton_1=="on") {
              client.println("<p><a href=\"/01/off\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/01/on\"><button class=\"button button2\">OFF</button></a></p>");
            } 

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
    Serial.println(header);
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println(""); }

     ledScenario();
     Fire2012();
     FastLED.show();
     FastLED.delay(1000 / FRAMES_PER_SECOND);
 }

void ledScenario(void ) { /* function ledScenario */
 ////LEDS Strip scenario
 for (int i = 0; i < NUM_LEDS; i++) {
  /* V1 fixe
   if (i < NUM_LED_FIRE){
   leds[i].setRGB(255, 0, 0);
  // leds[i + 3].setRGB(255, 0, 255);
   FastLED.show();}
   else{
      leds[i].setRGB(0, 255, 0);
      FastLED.show();
   }*/

   if (i >= NUM_LEDS_FIRE)
{leds[i].setRGB(color_1, color_2, color_3);}
   
 //  delay(100);
  // leds[i] = CRGB::Black;
  // FastLED.show();
 //  delay(100);
 }
}

void Fire2012()
{
// Array of temperature readings at each simulation cell
  static uint8_t heat[NUM_LEDS_FIRE];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS_FIRE; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS_FIRE - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS_FIRE; j++) {
      CRGB color = HeatColor( heat[j]);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (NUM_LEDS_FIRE-1) - j;
      } else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;
    }
}
