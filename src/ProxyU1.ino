// /* 
// Project: Proxy U

//     Transmitter (publisher): User 1 - U1 - Alpha
//     Receiver (suscriber):    User 2 - U2 -Beta
    
//    Controller: Particle Argon / ID: Alpha

//     Description:
//     Two transceivers connected together by WiFi reacts to 
//     the other user's actions from the data the devices has sent through the cloud. 
    
//     A0  NeoPixel Jewel (Light)
//         - Displays lighting effects that are based on U1 own action (sensors) 
//         - Updates based on the U2's actions (suscribed to their sensors + location).

//     A2 Photocell /  Photoresistor
//         - Reads the analog voltage to determine the brightness of the ligth (A0)

//     D7  Titlt Ball Switch (Tilt Sensor) 
//        - Detects basic motion/orientation of U1
//        - Publishes state of the sensor to the Particle Cloud as event for U2 to react
    
// */

SYSTEM_THREAD(ENABLED);
SerialLogHandler logHandler;

// uint32_t lastTime = 0;
std::chrono::milliseconds publishPeriod = 5s;
unsigned long lastPublishMs;

// library for the Adafruit NeoPixel Jewel 
// link: https://github.com/adafruit/Adafruit_NeoPixel
#include <neopixel.h>

// integrating google maps for location
// link: https://github.com/particle-iot/google-maps-device-locator/blob/master/src/google-maps-device-locator.h
#include <google-maps-device-locator.h>

// Locator object as a global variable in the program:
GoogleMapsDeviceLocator locator;

// Track location of the device
// gives three values about location
// Latitude: The latitude of the device's location
// Longitude: The longitude of the device's location
// Circle of Uncertainty: The radius in meters that represents the uncertainty range of the reported location
void locationCallback(float lat1, float lon1, float accuracy)
{
    Serial.printlnf("Lat: %f Lon: %f Acc (m): %f", lat1, lon1, accuracy);  // writes the coordinates in the serial monitor
}

  float calculateDistance(float lat1, float lon1, float lat2, float lon2)
    {
    float distance;
    float temp1;
    float temp2;
    temp1=(float)((lat1-lat2)*(lat1-lat2));
    temp2=(float)((lon1-lon2)*(lon1-lon2));
    distance=temp1+temp2;
    return distance;

    }

// NeoPixel Light Setup
// Parameters: 
//  1 = number of pixels in strip
//  2 = pin number
//  3 = pixel type flags
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(7, D3, SK6812RGBW);


// Define analogs & digital signals

int photoSens = A4; // Analog 5: Photocell
int tiltSens = A2; // A2: Tilt Ball Switch
int buzzSens = D6; // Digital 3: Buzz

// Define values

int resVal=0;  // store the value of the photocell

// Define states
int resMode= 0;
int colorMode = 0;
int lightMode =0;

int lastTiltState = 0;     
int tiltState  = 0;

// define other particle's state
int resModeBeta= 0;
int colorModeBeta = 0;
int lightModeBeta = 0;


// Set up the pins & subscriptions
void setup() 
{
Serial.begin(9600);
pixels.begin(); 

pinMode(photoSens, INPUT);
pinMode(buzzSens, OUTPUT);
pinMode(tiltSens, INPUT);

// functions reacting to the subscribe events
void alphaHandler1(const char *event, const char *data); 
void alphaHandler2(const char *event, const char *data); 
void alphaHandler3(const char *event, const char *data); 
void alphaHandler4(const char *event, const char *data); 

Particle.subscribe("betaOnline", alphaHandler1);
Particle.subscribe("betaColor", alphaHandler2);
Particle.subscribe("betaLight", alphaHandler3);
Particle.subscribe("betaLocate", alphaHandler4);

locator.withSubscribe(locationCallback);
} 

void loop(){


//repeats publish every 30s
 if (millis() - lastPublishMs >= publishPeriod.count())
    {
    lastPublishMs = millis();

    if (Particle.connected())
        {
            Particle.publish("alphaOnline", "on");
    Particle.publish("alphaColor", String(colorMode));
    Particle.publish("alphaLight", String(lightMode));

        } 
    }

// Sensor Functions

// Photo Cell sensor interaction
// [Photo Cell] reads the value and publishes the result to the cloud
  resVal=analogRead(photoSens);

if (3100 < resVal){
  	pixels.setPixelColor(1, pixels.Color(5,0,0,0)); // Moderately bright green color.
    lightMode = 2;
  }
  else if (600 < resVal){
  	pixels.setPixelColor(1, pixels.Color(0,5,0,0)); // Moderately bright green color.
    lightMode = 1;

  } else if (99 > resVal) {
  	pixels.setPixelColor(1, pixels.Color(0,0,5,0)); // Moderately bright green color.
    lightMode = 0;
  }

// [Photo Cell] compares value to other particle to see if it matches
   if ((lightModeBeta = 0) && (lightMode == 0)){
      	pixels.setPixelColor(4, pixels.Color(5,0,0,0));

  } else if ((lightModeBeta = 1) && (lightMode == 1)){
      	pixels.setPixelColor(4, pixels.Color(0,5,0,0));

  } else if ((lightModeBeta = 2) && (lightMode == 2)){
      	pixels.setPixelColor(4, pixels.Color(0,0,5,0));
  }

// Tilt sensor interaction
// [Tilt sensor] reads whenever the tilt sensor activates
tiltState = digitalRead(tiltSens);
tone(buzzSens, 1000, 10);

    if (tiltState != lastTiltState) {
    // Particle.publish("alphaColor", String(colorMode));

    if ((digitalRead(tiltSens) == HIGH) ||(digitalRead(tiltSens) == LOW) ) { 
    if (colorMode < 5) { //until it reaches the last mode, cycle to the next mode
      colorMode = colorMode + 1;

    } else {
      colorMode = 0; // after the last mode, cycle backs to first mode 
    }
  }
      lastTiltState = tiltState; //upade state out of loop
    } 

// Color states to change the pixel display
switch (colorMode) {
  case 0:
   	pixels.setPixelColor(2, pixels.Color(5,0,0,0));
  break;
  case 1:
    	pixels.setPixelColor(2, pixels.Color(0,5,0,0));
  break;
  case 2:
   	pixels.setPixelColor(2, pixels.Color(0,0,5,0)); 
  break;
  case 3:
    pixels.setPixelColor(2, pixels.Color(2,0,3,0));
  break; 
    case 4:
    	pixels.setPixelColor(2, pixels.Color(2,3,0,0));
  break;
}
  
  // [Tilt sensor] compares the given color to the other particle
  if ((colorMode == 0) && (colorModeBeta == 0)){
      	pixels.setPixelColor(3, pixels.Color(5,0,0,0));

  } else if ((colorMode == 1) && (colorModeBeta == 1)){
      	pixels.setPixelColor(3, pixels.Color(0,5,0,0));

  } else if ((colorMode == 2) && (colorModeBeta == 2)){
      	pixels.setPixelColor(3, pixels.Color(0,0,5,0));

  }  else if ((colorMode == 3) && (colorModeBeta == 3)){
      	pixels.setPixelColor(3, pixels.Color(2,0,3,0));

  } else if ((colorMode == 4) && (colorModeBeta == 4)){
      	pixels.setPixelColor(3, pixels.Color(2,3,0,0));

  } else {
      	pixels.setPixelColor(3, pixels.Color(0,0,0,5));

  }

// [No Sensor] compares if all the sensors + other particle sensor matches
   if ((colorMode == 0) && (colorModeBeta == 0) && (lightModeBeta == 0)) {
         	pixels.setPixelColor(0, pixels.Color(1,2,2,0));

  }  

// Updates location
    locator.loop();
}

// Cloud interactions
// reactions based on the subscriptions to the other particle

// [First Interaction] Particle.subscribe("betaOnline", alphaHandler1);
// sound notification when other party stays online
void alphaHandler1(const char *event, const char *data)
{

if (strcmp(data, "on") == 0) // -> beta is online
{
//sound notification 
tone(buzzSens, 1000, 500);
delay(200);
tone(buzzSens, 2000, 500);
delay(200);
} 
}

// [Second Interaction] Particle.subscribe("betaColor", alphaHandler2);
// updates the color selected from the tilt action from the other particle
void alphaHandler2(const char *event, const char *data)
{
  if (strcmp(data, "0") == 0)
{
  pixels.setPixelColor(5, pixels.Color(5,0,0,0)); 
colorModeBeta = 0;

  } else if (strcmp(data, "1") == 0)
{
  pixels.setPixelColor(5, pixels.Color(0,5,0,0)); 
colorModeBeta = 1;

  } else if (strcmp(data, "2") == 0)
{
  pixels.setPixelColor(5, pixels.Color(0,0,5,0)); 
colorModeBeta = 2;

  } else if (strcmp(data, "3") == 0)
{
  pixels.setPixelColor(5, pixels.Color(2,0,3,0)); 
colorModeBeta = 3;

  }else if (strcmp(data, "4") == 0)
{
  pixels.setPixelColor(5, pixels.Color(2,3,0,0)); 
colorModeBeta = 4;

  } 
//   else
//   {
//   pixels.setPixelColor(5, pixels.Color(0,5,5,0)); 
// colorModeBeta = 5;

//   }  
}

// [Third Interaction] Particle.subscribe("betaLight", alphaHandler3);
// updates the value given from the photocell from the other particle
void alphaHandler3(const char *event, const char *data)
{

  //sound notification
  tone(buzzSens, 900, 500);
  delay(200);
  tone(buzzSens, 900, 500);

 if (strcmp(data, "0") == 0)
{
  pixels.setPixelColor(6, pixels.Color(5,0,0,0)); 
lightModeBeta = 0;

  } else if (strcmp(data, "1") == 0)
{
  pixels.setPixelColor(6, pixels.Color(0,5,0,0)); 
lightModeBeta = 1;

  } else if (strcmp(data, "2") == 0)
{
  pixels.setPixelColor(6, pixels.Color(0,0,5,0)); 
lightModeBeta = 2;
}
}

// [Fourth Interaction] Particle.subscribe("betaLocate", alphaHandler4);
// 

void alphaHandler4(const char *event, const char *data)
{
locator.publishLocation();
if (strcmp(data, "far") == 0){
  pixels.setPixelColor(0, pixels.Color(5,0,5,0)); 
} else { //near
  pixels.setPixelColor(0, pixels.Color(0,5,0,0)); 
}

}










