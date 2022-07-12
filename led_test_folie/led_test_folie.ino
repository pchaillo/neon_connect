// Fonctionne, met les 20 premières led en rouge et les dernières en verts

//Libraries
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

#define NUM_LEDS_FIRE 20
#define FRAMES_PER_SECOND 60

//Parameters
const int stripPin  = D6;
//Variables
bool gReverseDirection  = false;
//Objects
CRGB leds[NUM_LEDS];
void setup() {
 //Init Serial USB
 Serial.begin(9600);
 Serial.println(F("Initialize System"));
 //Init led strips
 FastLED.addLeds<LED_TYPE, stripPin, COLOR_ORDER>(leds, NUM_LEDS);
 FastLED.setBrightness(  BRIGHTNESS );
}
void loop() {
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
{leds[i].setRGB(0, 255, 0);}
   
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
