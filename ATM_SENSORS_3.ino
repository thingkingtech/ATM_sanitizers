#include <FastLED.h>
#include <SharpIR.h>

#define led 11

#define NUM_LEDS 3 // number of led present in your strip
#define LED_PIN 12 // digital pin of your arduino
#define COLOR_ORDER GRB
#define CHIPSET     WS2811
#define BRIGHTNESS  255

CRGB leds[NUM_LEDS];

// Define model and input pin:
#define IRPin1 A1
#define IRPin2 A2
#define IRPin3 A3
#define model 20150

SharpIR mySensor1 = SharpIR(IRPin1, model);
SharpIR mySensor2 = SharpIR(IRPin2, model);
SharpIR mySensor3 = SharpIR(IRPin3, model);

void LEDSoff(){
  for (int i=0; i<NUM_LEDS; i++){
    leds[i] = CRGB::Black;
  }
  FastLED.show();
  digitalWrite(led, LOW);
}

void LEDSon(){
  for (int i=0; i<NUM_LEDS; i++){
    leds[i] = CRGB::White;
  }
  FastLED.show();
  digitalWrite(led, HIGH);
}

int dist = 200;
int timeout = 14000;

unsigned int dis[5];
unsigned int dis1, dis2, dis3;

bool high, san, person;

unsigned long now, since, timenow, timetill;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("ready");
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(led, OUTPUT);
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness(BRIGHTNESS);
  for (int i=0; i<NUM_LEDS; i++){
    leds[i] = CRGB::Black;
  }
  FastLED.show();
  digitalWrite(led, LOW);
  delay(500);

}

void printvalues(int x, int y, int z){
  Serial.println("dist 1: " + String(x));
  Serial.println("dist 2: " + String(y));
  Serial.println("dist 3: " + String(z));
}

int checkdis1(){
  for (int i=0; i<5; i++){
    dis[i] = mySensor1.distance();
    delay(5);
  }
  dis1 = (dis[0]+dis[1]+dis[3]+dis[4]+dis[5])/5;
  return dis1;
}

int checkdis2(){
  for (int i=0; i<5; i++){
    dis[i] = mySensor2.distance();
    delay(5);
  }
  dis2 = (dis[0]+dis[1]+dis[3]+dis[4]+dis[5])/5;
  return dis2;
}

int checkdis3(){
  for (int i=0; i<5; i++){
    dis[i] = mySensor3.distance();
    delay(5);
  }
  dis3 = (dis[0]+dis[1]+dis[3]+dis[4]+dis[5])/5;
  return dis3;
}

void loop() {
  // put your main code here, to run repeatedly:
  dis1 = checkdis1();
  dis2 = checkdis2();
  dis3 = checkdis3();
  //printvalues(dis1, dis2, dis3);

  if (dis1<dist or dis2<dist or dis3<dist){       // PERSON DETECTED, LEDS OFF
    LEDSoff();
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("person detected!");
    printvalues(dis1,dis2,dis3);
    timenow = millis();
    high=false;
    san=false;
    while (dis1<dist or dis2<dist or dis3<dist){  // LOOP WHILE PERSON DETECTED
      dis1 = checkdis1();
      dis2 = checkdis2();
      dis3 = checkdis3();
      timetill=millis();
      if (timetill-timenow>2000){                 // PERSON DEF DETECTED
        person=true;
      }
    }
    
  }
  if (dis1>dist and dis2>dist and dis3>dist){     // NOBODY DETECTED
    if (person){
      now = millis();
    }
    
    Serial.println("there is nobody");
    while (dis1>dist and dis2>dist and dis3>dist){
      dis1 = checkdis1();
      dis2 = checkdis2();
      dis3 = checkdis3();
      since = millis();
      digitalWrite(LED_BUILTIN, LOW);
      //Serial.println(timetill - timenow);
      
      if (since - now > 1500 and !high and person and !san){
        Serial.println("LEDS on");
        LEDSon();
        high=true;
        san=false;
        person = false;
      }
      
      if (since-now > timeout and high and !person){
        Serial.println("timeout - sanitized!");
        LEDSoff();
        high=false;
        san=true;
        person=false;
        break;
      }
    }
  }
 }
