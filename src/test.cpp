// // We define MY_LED to be the LED that we want to blink.
// //
// // In this tutorial, we're using the blue D7 LED (next to D7 on the Photon
// // and Electron, and next to the USB connector on the Argon and Boron).
// const pin_t MY_LED = D7;



// // The setup() method is called once when the device boots.
// void setup()
// {
//     void betaHandler(const char *event, const char *data);

// 	// In order to set a pin, you must tell Device OS that the pin is
// 	// an OUTPUT pin. This is often done from setup() since you only need
// 	// to do it once.
// 	pinMode(MY_LED, OUTPUT);
// 	Particle.subscribe("booped", betaHandler);

// }

// // The loop() method is called frequently.
// void loop()
// {
// 	// Turn on the LED

// }

// void betaHandler(const char *event, const char *data)

// {
// digitalWrite(MY_LED, HIGH);
// Particle.publish("onStatus");
// }
