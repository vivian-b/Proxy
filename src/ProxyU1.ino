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

uint32_t lastTime = 0;


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
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(7, D6, SK6812RGBW);


// Define analogs & digital signals

int photoSens = A4; // Analog 5: Photocell
int tiltSens = A2; // A2: Tilt Ball Switch
int buzzSens = D3; // Digital 3: Buzz

// Define values

int resVal=0;  // store the value of the photocell

int resMode= 0;
int colorMode = 0;

int resModeBeta= 0;
int colorModeBeta = 0;
int lightModeBeta = 0;

int lastTiltState = 0;     
int tiltState  = 0;

// set up the pins & subscriptions
void setup() 
{
Serial.begin(9600);
pixels.begin(); 

pinMode(photoSens, INPUT);
pinMode(buzzSens, OUTPUT);
pinMode(tiltSens, INPUT_PULLUP);

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

if ((lastTime + 60000) <= millis()){
  lastTime = millis();

Particle.publish("alphaOnline", "on");

}


resVal=analogRead(photoSens);


if (200 < resVal){
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
  	pixels.setPixelColor(1, pixels.Color(5,0,0,0)); // Moderately bright green color.

Particle.publish("lighting1", String(resVal));
  }
  else if (100 < resVal){
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
  	pixels.setPixelColor(1, pixels.Color(0,5,0,0)); // Moderately bright green color.
        pixels.show();

    Particle.publish("lighting2", String(resVal));
  }
  else
  {
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
  	pixels.setPixelColor(1, pixels.Color(0,0,5,0)); // Moderately bright green color.
        pixels.show();

    Particle.publish("lighting3", String(resVal));

  }

tiltState = digitalRead(tiltSens);
tone(buzzSens, 1000, 10);

    if (tiltState != lastTiltState) {
    Particle.publish("alphaColor", String(colorMode));

    if ((digitalRead(tiltSens) == HIGH) ||(digitalRead(tiltSens) == LOW) ) { 
    if (colorMode < 5) { //until it reaches the last mode, cycle to the next mode
      colorMode = colorMode + 1;

    } else {
      colorMode = 0; // after the last mode, cycle backs to first mode 
    }
  }
      lastTiltState = tiltState; //upade state out of loop
    } 

switch (colorMode)
{
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

  if ((lightModeBeta = 0) && (colorModeBeta == 0)){
      	pixels.setPixelColor(3, pixels.Color(5,0,0,0));

  }
          
  pixels.setPixelColor(4, pixels.Color(0,0,0,5));

// Updates location
    locator.loop();
}


// Particle.subscribe("betaOnline", alphaHandler1);
// sound notification when other party goes online
void alphaHandler1(const char *event, const char *data)
{

Particle.publish("alphaOnline", "on"); // respond to the other being online

if (strcmp(data, "on") == 0) // -> beta is online
{
//sound notification 
tone(buzzSens, 1000, 500);
delay(200);
tone(buzzSens, 2000, 500);
delay(200);
tone(buzzSens, 3000, 200);

} else // -> -> beta goes offline
{
//sound notification 
tone(buzzSens, 1000, 100);
delay(200);
tone(buzzSens, 1000, 100);
  }
}

// Particle.subscribe("betaColor", alphaHandler2);
// updates the color selected from the tilt action from the other party
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

// Particle.subscribe("betaLight", alphaHandler3);
// photo resistor value

void alphaHandler3(const char *event, const char *data)
{

  //sound notification
  tone(buzzSens, 900, 500);
  delay(200);
  tone(buzzSens, 900, 500);

 if (strcmp(data, "low") == 0)
{
  pixels.setPixelColor(5, pixels.Color(5,0,0,0)); 
colorModeBeta = 0;

  } else if (strcmp(data, "mid") == 0)
{
  pixels.setPixelColor(5, pixels.Color(0,5,0,0)); 
colorModeBeta = 1;

  } else if (strcmp(data, "high") == 0)
{
  pixels.setPixelColor(5, pixels.Color(0,0,5,0)); 
colorModeBeta = 2;

}

// Particle.subscribe("betaLocate", alphaHandler4);
void alphaHandler4(const char *event, const char *data)
{
locator.publishLocation();
if (strcmp(data, "far") == 0){
  pixels.setPixelColor(0, pixels.Color(5,0,5,0)); 
} else { //near
  pixels.setPixelColor(0, pixels.Color(0,5,0,0)); 
}

}










