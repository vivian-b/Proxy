/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "/Users/vivian/Documents/ProxyU/src/ProxyU1.ino"
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

// library for the Adafruit NeoPixel Jewel 
// link: https://github.com/adafruit/Adafruit_NeoPixel
#include <neopixel.h>

// integrating google maps for location
// link: https://github.com/particle-iot/google-maps-device-locator/blob/master/src/google-maps-device-locator.h
#include <google-maps-device-locator.h>

// Locator object as a global variable in the program:
void locationCallback(float lat1, float lon1, float accuracy);
float calculateDistance(float lat1, float lon1, float lat2, float lon2);
void setup();
void loop();
#line 35 "/Users/vivian/Documents/ProxyU/src/ProxyU1.ino"
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

int lastButtonState = 0;     
int buttonState  = 0;

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
Particle.subscribe("betaLighting", alphaHandler3);
Particle.subscribe("betaLocate", alphaHandler4);

locator.withSubscribe(locationCallback);
} 

void loop(){

if (Particle.connected()) {
Particle.publish("alphaOnline", "on");
} 
if (Particle.disconnected()) {
Particle.publish("alphaOnline", "off");
}

resVal=analogRead(photoSens);
// 
// 
// 

if (300 < resVal){
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
  	pixels.setPixelColor(1, pixels.Color(10,0,0,0)); // Moderately bright green color.

        pixels.show();

Particle.publish("lighting1", String(resVal));

    pixels.show();
  }
  else if (100 < resVal){
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
  	pixels.setPixelColor(1, pixels.Color(0,10,0,0)); // Moderately bright green color.
        pixels.show();

    Particle.publish("lighting2", String(resVal));
    pixels.show();
  }
  else
  {
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
  	pixels.setPixelColor(1, pixels.Color(0,0,10,0)); // Moderately bright green color.
        pixels.show();

    Particle.publish("lighting3", String(resVal));

    pixels.show();
  }

buttonState = digitalRead(tiltSens);

    if (buttonState != lastButtonState) {
       Serial.println(colorMode);

     if ((digitalRead(tiltSens) == HIGH) ||(digitalRead(tiltSens) == LOW) ) { 

    if (colorMode < 6) { //until it reaches the last mode, cycle to the next mode
      colorMode = colorMode + 1;

    } else {
      colorMode = 0; // after the last mode, cycle backs to first mode 
    }
  }
      lastButtonState = buttonState; //upade state out of loop
    } 

switch (colorMode)
{
  case 0:
   	pixels.setPixelColor(2, pixels.Color(100,0,0));

  break;
  case 1:
    	pixels.setPixelColor(2, pixels.Color(0,100,0));

  break;
  case 2:
   	pixels.setPixelColor(2, pixels.Color(0,0,100)); 

  break;
  case 3:
    	pixels.setPixelColor(2, pixels.Color(100,0,100));

  break;   
   case 4:
    	pixels.setPixelColor(2, pixels.Color(0,100,100));

  break; 
    case 5:
    	pixels.setPixelColor(2, pixels.Color(100,100,0));

  break;
}
  
  if ((colorMode == 0) && (colorModeBeta == 0)){
      	pixels.setPixelColor(3, pixels.Color(100,0,0));

  } else if ((colorMode == 1) && (colorModeBeta == 1)){
        	pixels.setPixelColor(3, pixels.Color(0,100,0));

  } else if ((colorMode == 2) && (colorModeBeta == 2)){
        	pixels.setPixelColor(3, pixels.Color(0,0,100));

  }  else if ((colorMode == 3) && (colorModeBeta == 3)){
        	pixels.setPixelColor(3, pixels.Color(100,0,100));

  } else if ((colorMode == 4) && (colorModeBeta == 2)){
        	pixels.setPixelColor(3, pixels.Color(200,100,200));

  }else {
          	pixels.setPixelColor(3, pixels.Color(0,100,100));

  } 

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
if (strcmp(data, "1") == 0)
{
  pixels.setPixelColor(5, pixels.Color(2,0,0,1)); 
colorModeBeta = 1;

  } else if (strcmp(data, "2") == 0)
{
  pixels.setPixelColor(5, pixels.Color(0,2,0,1)); 
colorModeBeta = 2;

  } else if (strcmp(data, "3") == 0)
{
  pixels.setPixelColor(5, pixels.Color(0,0,2,1)); 
colorModeBeta = 3;

  }else if (strcmp(data, "4") == 0)
{
  pixels.setPixelColor(5, pixels.Color(2,0,2,1)); 
colorModeBeta = 4;

  } else
  {
  pixels.setPixelColor(5, pixels.Color(0,2,2,0)); 
colorModeBeta = 5;

  }  
}

// Particle.subscribe("betaLighting", alphaHandler3);
// photo resistor value

void alphaHandler3(const char *event, const char *data)
{

  //sound notification
  tone(buzzSens, 900, 500);
  delay(200);
  tone(buzzSens, 900, 500);

if (strcmp(data, "low") == 0)
  {
    pixels.setPixelColor(6, pixels.Color(2,0,0,1)); 
  } else if (strcmp(data, "high") == 0)
  {
    pixels.setPixelColor(6, pixels.Color(0,2,0,1)); 
  } else
  {
  pixels.setPixelColor(6, pixels.Color(0,0,2,1)); 
  }

}

// Particle.subscribe("betaLocate", alphaHandler4);
void alphaHandler4(const char *event, const char *data)
{
locator.publishLocation();
if (strcmp(data, "far") == 0){
  pixels.setPixelColor(0, pixels.Color(5,0,5,1)); 
} else { //near
  pixels.setPixelColor(0, pixels.Color(0,5,0,1)); 
}

}










