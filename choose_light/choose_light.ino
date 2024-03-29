//Libraries for LED
#include <FastLED.h>//https://github.com/FastLED/FastLED
//Constants
#define NUM_STRIPS 1
#define NUM_LEDS 29
#define BRIGHTNESS 10 // Pour choisir l'intensité des couleurs
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB//BRG//RGB
#define FASTLED_ALLOW_INTERRUPTS 0
#define FASTLED_INTERRUPT_RETRY_COUNT 1
#define FRAMES_PER_SECOND 60
#define COOLING 55
#define SPARKING 120
#define FRAMES_PER_SECOND 60
//Parameters
const int stripPin  = D6;
//Variables
bool gReverseDirection  = false; // Bolléen pour inverser la direction du feu
//Objects

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
String bouton_2 = "on";

//String output14State = "off";
//String output27State = "off";

//int sliderValue = 10;
//String sliderValueText = "10";
String sliderValue = "10";
volatile int sliderInt = 10;

#define nb_segment 4
#define NUM_LEDS_FIRE 27 // 19
volatile byte feu_tab[nb_segment][2] =  {{0,7},{15,8},{16,20},{26,17}}; //[2][n] : [2] => debut et fin , [nb_segment] => nombre de segments 
volatile byte feu_ind[NUM_LEDS_FIRE];
CRGB leds[NUM_LEDS];
CRGB leds_memory[NUM_LEDS_FIRE];

void setup() {
  // put your setup code here, to run once:
   Serial.begin(115200); // TEST PORT SERIE
   pinMode(LED_BUILTIN, OUTPUT);
   delay(1000);
   Serial.println("serial test..."); // TEST PORT SERIE

   // Connect to Wi-Fi network with SSID and password
 // Serial.print("Setting AP (Access Point)…"); // TEST PORT SERIE
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();

  /*Serial.print("AP IP address: ");
  Serial.println(IP);*/ // TEST PORT SERIE

  FastLED.addLeds<LED_TYPE, stripPin, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(  BRIGHTNESS );
  
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
    WiFiClient client = server.available();   // Listen for incoming clients

    if (client) {                             // If a new client connects,
   // Serial.println("New Client.");          // print a message out in the serial port // TEST PORT SERIE
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
   //     Serial.write(c);                    // print it out the serial monitor // TEST PORT SERIE
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
          //    Serial.println("LED_BUILTIN on"); // TEST PORT SERIE
              Led_In_State = "on";
            //  digitalWrite(output12, LOW);
              digitalWrite(LED_BUILTIN, LOW);
            } 
            else if (header.indexOf("GET /12/off") >= 0) 
            {
           //   Serial.println("LED_BUILTIN off"); // TEST PORT SERIE
              Led_In_State = "off";
             // digitalWrite(output12, HIGH);
              digitalWrite(LED_BUILTIN, HIGH);
            }


            //for bouton_1
            else if (header.indexOf("GET /01/on") >= 0) 
            {
              bouton_1 = "on";
                color_1 = 0 ;
                color_2 = 255;
                color_3 = 0;
            } 
            else if (header.indexOf("GET /01/off") >= 0) 
            {
              bouton_1 = "off";
                color_1 = 0 ;
                color_2 = 0 ; // sliderInt
                color_3 = 255;
            }

            //for bouton_2
            else if (header.indexOf("GET /02/on") >= 0) 
            {
              bouton_2 = "on";
            } 
            else if (header.indexOf("GET /02/off") >= 0) 
            {
              bouton_2 = "off";

            }

            else if (header.indexOf("GET /slider?value=") >= 0)
            {
            //  Serial.println("TROP BIEN TROP BIEN");
              String text_raw = getValue(header, ' ', 1);
              String string_raw = getValue(text_raw, '=', 1);
              sliderInt = string_raw.toInt();
              sliderValue = String(sliderInt);

           //   Serial.println("Y:");
             // Serial.println(sliderInt);
            // Serial.println(text2);
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
            client.println(".button2 {background-color: #555555;}"); // Donne la couleur du bouton cliqué
            //client.println(".button2 {background-color: #555555;}</style>");

            // Code slider 
            /*
            client.println("<p><div class=\"/ard rangeslider P9\"></div></p><p><div class=\"ard rangeslider P10\"></div></p><p><div class=\"ard rangeslider P11\"></div></p> "); 
            client.println("<p><div class=\"ard rangeslider P9 value-50 range-0-100\"></div></p>");
            client.println(" <p><div class=\"display-color\"></div></p>"); */
            client.println(".slider { -webkit-appearance: none; margin: 14px; width: 360px; height: 25px; background: #FFD65C; outline: none; -webkit-transition: .2s; transition: opacity .2s;}");
            client.println(".slider::-webkit-slider-thumb {-webkit-appearance: none; appearance: none; width: 35px; height: 35px; background: #003249; cursor: pointer;}");
            client.println(".slider::-moz-range-thumb { width: 35px; height: 35px; background: #003249; cursor: pointer; }</style>");
            client.println("</head>");
            client.println("<script> function updateSlider() { var sliderValue = document.getElementById(\"pwmSlider\").value;document.getElementById(\"textSliderValue\").innerHTML = sliderValue;console.log(sliderValue);var xhr = new XMLHttpRequest();xhr.open(\"GET\", \"/slider?value=\"+sliderValue, true);xhr.send(); return sliderValue} </script>");

            // Web Page Heading
            client.println("<body><h1> Mera Neon </h1>");

            client.println("<p><span id=textSliderValue>" + sliderValue+ "</span></p>"); // creation du slider
            client.println("<p><input type=\"range\" onchange=\"updateSlider()\"  id=\"pwmSlider\" min=\"0\" max=\"255\" value="+sliderValue+"\" step=\"1\" class=\"slider\"></p>");
          //  client.println("<p><a href=\"/12/\"updateSlider()><button class=\"button\">ON</button></a></p>");

            // Display current state, and ON/OFF buttons for builtin led  
            client.println("<p> Test Connexion : Led In - State " + Led_In_State + "</p>");
            // If the output12State is off, it displays the ON button       
            if (Led_In_State=="on") {
              client.println("<p><a href=\"/12/off\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/12/on\"><button class=\"button button2\">OFF</button></a></p>");
            } 

            // Display current state, and ON/OFF buttons for bouton_1 => Choix couleur branche
            client.println("<p> LED color - State " + bouton_1 + "</p>");
            // If the output12State is off, it displays the ON button       
            if (bouton_1=="on") {
              client.println("<p><a href=\"/01/off\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/01/on\"><button class=\"button button2\">OFF</button></a></p>");
            } 

            // Display current state, and ON/OFF buttons for bouton_2 => Choix couleur flamme
            client.println("<p> LED color - Flammes " + bouton_2 + "</p>");
            // If the output12State is off, it displays the ON button       
            if (bouton_2=="on") {
              client.println("<p><a href=\"/02/off\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/02/on\"><button class=\"button button2\">OFF</button></a></p>");
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
  //  Serial.println("Usefull fata : "); // TEST PORT SERIE
   // Serial.println(header); // TEST PORT SERIE
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    
/*    Serial.println("Client disconnected.");
    Serial.println("");  */ } // IF CLIENT

    create_fire_led_tab();
    Serial.println("Boucle");
    for(int u = 0; u < NUM_LEDS_FIRE;u++){
    Serial.println(feu_ind[u]); }
    Serial.println("Fin Feu_Ind");
     Fire2012();
    // ledScenario();
     led_ASSIGMENT();
     FastLED.setBrightness(sliderInt);
     FastLED.show();
     FastLED.delay(1000 / FRAMES_PER_SECOND);

   /*  Serial.println("\n"); // TEST PORT SERIE slider
     Serial.print("Valeur du slider : ");
     Serial.println(sliderValue);
     Serial.println("\n");  */
     
     } // LOOP

/*
String sendPosition(String sliderValue) {
  /*console.log(slider+": "+value);

  var request = new XMLHttpRequest();
  request.open("POST", "https://foo.bar/");
  request.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
  request.send(encodeURI(slider)+"="+value);
  client.println("<p><a href=\"/12/"+sliderValue+"\"></a></p>");
}*/

void create_fire_led_tab(void){
  byte ind = 0;
    for (int y = 0; y < nb_segment; y++) {
      /* for(int ind = 0; ind < NUM_LEDS_FIRE;ind++){
          fire_ind[ind] = 
       } */
       Serial.println(feu_tab[y][0]);
       Serial.println(feu_tab[y][1]);
       if (feu_tab[y][0] < feu_tab[y][1]){
              for (int k = feu_tab[y][0] ; k < feu_tab[y][1] ; k++) {
                   feu_ind[ind] = k;
                   ind = ind + 1;
  }
       }
       else{
               for (int k = feu_tab[y][0] ; k > feu_tab[y][1]-2 ; k--) { // -1 pertinent ici ? // - 2 ?
                  //ind = ind + 1;
                  feu_ind[ind] = k;
                  ind = ind + 1;
  }
       }
}
}

void led_ASSIGMENT(void ) { /* function ledScenario */
 ////LEDS Strip scenario
 for (int i = 0; i < NUM_LEDS; i++) {

   if (i >= NUM_LEDS_FIRE)
{leds[i].setRGB(color_1, color_2, color_3);}
//else{
  else if (bouton_2 == "off"){
    
  //  CRGB leds_memory[NUM_LEDS_FIRE];

    
}
 }
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
{leds[i].setRGB(color_1, color_2, color_3);
//leds[i].setHue(sliderInt);  // change le mauvais truc
//leds[i] = HSV(leds[i].color,leds[i].saturation,sliderInt);
}
//else{
  else if (bouton_2 == "off"){

  int r = leds[i].r;
  int g = leds[i].g;
  int b = leds[i].b;

  /*leds[i].r = b; // FLAMME BLEUE
  leds[i].b = r; */
 
  leds[i].b = r; // FLAMME VIOLETTE 1 

 /* leds[i].b = r; // FLAMME VIOLETTE 2
  leds[i].g = b; */

/*  leds[i].r = b; // FLAMME VERTE
  leds[i].g = r; */
}
   
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
       // pixelnumber = j; // CLASSIQUE
       pixelnumber = feu_ind[j];
      }
      leds[pixelnumber] = color; // CLASSIQUE
    }
}

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
