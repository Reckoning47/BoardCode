/*********************************************************************
 This is an example for our nRF51822 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/
// ATmega32u4 chip with Hardware SPI, CS = 8, IRQ = 7, RST = 4

#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"

#if SOFTWARE_SERIAL_AVAILABLE
  #include <SoftwareSerial.h>
#endif

/*=========================================================================
    APPLICATION SETTINGS

    FACTORYRESET_ENABLE       Perform a factory reset when running this sketch
   
                              Enabling this will put your Bluefruit LE module
                              in a 'known good' state and clear any config
                              data set in previous sketches or projects, so
                              running this at least once is a good idea.
   
                              When deploying your project, however, you will
                              want to disable factory reset by setting this
                              value to 0.  If you are making changes to your
                              Bluefruit LE device via AT commands, and those
                              changes aren't persisting across resets, this
                              is the reason why.  Factory reset will erase
                              the non-volatile memory where config data is
                              stored, setting it back to factory default
                              values.
       
                              Some sketches that require you to bond to a
                              central device (HID mouse, keyboard, etc.)
                              won't work at all with this feature enabled
                              since the factory reset will clear all of the
                              bonding data stored on the chip, meaning the
                              central device won't be able to reconnect.
    MINIMUM_FIRMWARE_VERSION  Minimum firmware version to have some new features
    MODE_LED_BEHAVIOUR        LED activity, valid options are
                              "DISABLE" or "MODE" or "BLEUART" or
                              "HWUART"  or "SPI"  or "MANUAL"
    -----------------------------------------------------------------------*/
    #define FACTORYRESET_ENABLE         1
    #define MINIMUM_FIRMWARE_VERSION    "0.6.6"
    #define MODE_LED_BEHAVIOUR          "MODE"
/*=========================================================================*/

// Create the bluefruit object, either software serial...uncomment these lines
/*
SoftwareSerial bluefruitSS = SoftwareSerial(BLUEFRUIT_SWUART_TXD_PIN, BLUEFRUIT_SWUART_RXD_PIN);

Adafruit_BluefruitLE_UART ble(bluefruitSS, BLUEFRUIT_UART_MODE_PIN,
                      BLUEFRUIT_UART_CTS_PIN, BLUEFRUIT_UART_RTS_PIN);
*/

/* ...or hardware serial, which does not need the RTS/CTS pins. Uncomment this line */
// Adafruit_BluefruitLE_UART ble(BLUEFRUIT_HWSERIAL_NAME, BLUEFRUIT_UART_MODE_PIN);

/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

/* ...software SPI, using SCK/MOSI/MISO user-defined SPI pins and then user selected CS/IRQ/RST */
//Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_SCK, BLUEFRUIT_SPI_MISO,
//                             BLUEFRUIT_SPI_MOSI, BLUEFRUIT_SPI_CS,
//                             BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);


// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}



//Test of PWM pins with potentiometer
int DRA1 = 6;  //set pin 6 to motor a direction
int DRA2 = 9; //set pin 9 to motor a direction 
int SPA = 11; //set pin 11 to pwm speed control for motor a

int DRB1 = 13;  //set pin 13 to motor b direction
int DRB2 = 12; //set pin 12 to motor b direction
int SPB = 10;  //set pin 10 to pwm speed control for motor b

//SWITCHED PINS TO TEST FOR FEATHER M0
//int DRA1 = 13;  //set pin 2 to motor a direction
//int DRA2 = 12; //set pin 3 to motor a direction
//int SPB = 11; //set pin 5 to pwm speed control for motor aint analogwritevalueA; //use this variable to write 0-5v (0-255) to the motor a
//
//int DRB1 = 6;  //set pin 8 to motor b direction
//int DRB2 = 9; //set pin 9 to motor b direction
//int SPA = 10; //set pin 6 to pwm speed control for motor b

//int analogwritevalueB; //use this variable to write 0-5v (0-255) to the motor b


void setup() {

  pinMode (DRA1, OUTPUT);
  pinMode (DRA2, OUTPUT);
  pinMode (SPA, OUTPUT);

  pinMode (DRB1, OUTPUT);
  pinMode (DRB2, OUTPUT);
  pinMode (SPB, OUTPUT);
  
  delay(500);
  Serial.begin(115200);
  
  Serial.println(F("Adafruit Bluefruit Command <-> Data Mode Example"));
  Serial.println(F("------------------------------------------------"));

  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ){
      error(F("Couldn't factory reset"));
    }
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in UART mode"));
  Serial.println(F("Then Enter characters to send to Bluefruit"));
  Serial.println();

  ble.verbose(false);  // debug info is a little annoying after this point!

  /* Wait for connection */
  while (! ble.isConnected()) {
      delay(500);
  }

  Serial.println(F("******************************"));

  // LED Activity command is only supported from 0.6.6
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    // Change Mode LED Activity
    Serial.println(F("Change LED activity to " MODE_LED_BEHAVIOUR));
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
  }

  // Set module to DATA mode
  Serial.println( F("Switching to DATA mode!") );
  ble.setMode(BLUEFRUIT_MODE_DATA);

  Serial.println(F("******************************"));
}

void loop() {
  
  // Check for user input
  char n, inputs[BUFSIZE+1];
  //String n;

  if (Serial.available())
  {
    n = Serial.readBytes(inputs, BUFSIZE);
    //n = Serial.readString()
    inputs[n] = 0;
    // Send characters to Bluefruit
    Serial.print("Sending: ");
    Serial.println(inputs);

    // Send input data to host via Bluefruit
    ble.print(inputs);
  }

  // Echo received data
  while ( ble.available() )
  {
    int c = ble.read();

    Serial.print((char)c);
    ///(LEFT)MOTOR A CONTROLS///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if((char) c == 'a'){
      digitalWrite (DRA1, LOW);
      digitalWrite (DRA2, HIGH); //set opposite DRA1 for cc or ccw rotation, set equal to DRA1 for motor off 
      analogWrite (SPA, 255);
    }
    if((char) c == 'b'){
      digitalWrite (DRA1, LOW);
      digitalWrite (DRA2, HIGH); //set opposite DRA1 for cc or ccw rotation, set equal to DRA1 for motor off 
      analogWrite (SPA, 235);
    }
    if((char) c == 'c'){
      digitalWrite (DRA1, LOW);
      digitalWrite (DRA2, HIGH); //set opposite DRA1 for cc or ccw rotation, set equal to DRA1 for motor off 
      analogWrite (SPA, 215);
    }
    if((char) c == 'd'){
      digitalWrite (DRA1, LOW);
      digitalWrite (DRA2, HIGH); //set opposite DRA1 for cc or ccw rotation, set equal to DRA1 for motor off 
      analogWrite (SPA, 195);
    }
    if((char) c == 'e'){
      digitalWrite (DRA1, LOW);
      digitalWrite (DRA2, HIGH); //set opposite DRA1 for cc or ccw rotation, set equal to DRA1 for motor off 
      analogWrite (SPA, 175);
    }
    if((char) c == 'f'){
      digitalWrite (DRA1, LOW);
      digitalWrite (DRA2, HIGH); //set opposite DRA1 for cc or ccw rotation, set equal to DRA1 for motor off 
      analogWrite (SPA, 155);
      
    }
    if((char) c == 'g'){
      digitalWrite (DRA1, LOW);
      digitalWrite (DRA2, HIGH); //set opposite DRA1 for cc or ccw rotation, set equal to DRA1 for motor off 
      analogWrite (SPA, 135);
      
    }
    if((char) c == 'h'){
      digitalWrite (DRA1, LOW);
      digitalWrite (DRA2, HIGH); //set opposite DRA1 for cc or ccw rotation, set equal to DRA1 for motor off 
      analogWrite (SPA, 115);
      
    }
    if((char) c == 'i'){
      digitalWrite (DRA1, LOW);
      digitalWrite (DRA2, HIGH); //set opposite DRA1 for cc or ccw rotation, set equal to DRA1 for motor off 
      analogWrite (SPA, 95);
      
    }
    if((char) c == 'j'){
      digitalWrite (DRA1, LOW);
      digitalWrite (DRA2, HIGH); //set opposite DRA1 for cc or ccw rotation, set equal to DRA1 for motor off 
      analogWrite (SPA, 75);
      
    }
    if((char) c == 'k'){
      digitalWrite (DRA1, LOW);
      digitalWrite (DRA2, HIGH); //set opposite DRA1 for cc or ccw rotation, set equal to DRA1 for motor off 
      analogWrite (SPA, 0);
      
    }
    if((char) c == 'l'){
      digitalWrite (DRA1, LOW);
      digitalWrite (DRA2, HIGH); //set opposite DRA1 for cc or ccw rotation, set equal to DRA1 for motor off 
      analogWrite (SPA, 0);
      
    }
    //dead zone
    if((char) c == 'm'){
      digitalWrite (DRA1, LOW);
      digitalWrite (DRA2, HIGH); //set opposite DRA1 for cc or ccw rotation, set equal to DRA1 for motor off 
      analogWrite (SPA, 0);
      
    }
    //end of dead zone

    //begin reverse()
    if((char) c == 'n'){
      digitalWrite (DRA1, HIGH);
      digitalWrite (DRA2, LOW); //set opposite DRA1 for cc or ccw rotation, set equal to DRA1 for motor off 
      analogWrite (SPA, 0);
      
    }
    if((char) c == 'o'){
      digitalWrite (DRA1, HIGH);
      digitalWrite (DRA2, LOW); //set opposite DRA1 for cc or ccw rotation, set equal to DRA1 for motor off 
      analogWrite (SPA, 0);
      
    }
    if((char) c == 'p'){
      digitalWrite (DRA1, HIGH);
      digitalWrite (DRA2, LOW); //set opposite DRA1 for cc or ccw rotation, set equal to DRA1 for motor off 
      analogWrite (SPA, 0);
      
    }
    if((char) c == 'q'){
      digitalWrite (DRA1, HIGH);
      digitalWrite (DRA2, LOW); //set opposite DRA1 for cc or ccw rotation, set equal to DRA1 for motor off 
      analogWrite (SPA, 75);
      
    }
    if((char) c == 'r'){
      digitalWrite (DRA1, HIGH);
      digitalWrite (DRA2, LOW); //set opposite DRA1 for cc or ccw rotation, set equal to DRA1 for motor off 
      analogWrite (SPA, 95);
      
    }
    if((char) c == 's'){
      digitalWrite (DRA1, HIGH);
      digitalWrite (DRA2, LOW); //set opposite DRA1 for cc or ccw rotation, set equal to DRA1 for motor off 
      analogWrite (SPA, 115);
      
    }
    if((char) c == 't'){
      digitalWrite (DRA1, HIGH);
      digitalWrite (DRA2, LOW); //set opposite DRA1 for cc or ccw rotation, set equal to DRA1 for motor off 
      analogWrite (SPA, 135);
      
    }
    if((char) c == 'u'){
      digitalWrite (DRA1, HIGH);
      digitalWrite (DRA2, LOW); //set opposite DRA1 for cc or ccw rotation, set equal to DRA1 for motor off 
      analogWrite (SPA, 155);
      
    }
    if((char) c == 'v'){
      digitalWrite (DRA1, HIGH);
      digitalWrite (DRA2, LOW); //set opposite DRA1 for cc or ccw rotation, set equal to DRA1 for motor off 
      analogWrite (SPA, 175);
      
    }
    if((char) c == 'w'){
      digitalWrite (DRA1, HIGH);
      digitalWrite (DRA2, LOW); //set opposite DRA1 for cc or ccw rotation, set equal to DRA1 for motor off 
      analogWrite (SPA, 195);
      
    }
    if((char) c == 'x'){
      digitalWrite (DRA1, HIGH);
      digitalWrite (DRA2, LOW); //set opposite DRA1 for cc or ccw rotation, set equal to DRA1 for motor off 
      analogWrite (SPA, 215);
      
    }
    if((char) c == 'y'){
      digitalWrite (DRA1, HIGH);
      digitalWrite (DRA2, LOW); //set opposite DRA1 for cc or ccw rotation, set equal to DRA1 for motor off 
      analogWrite (SPA, 235);
      
    }
    if((char) c == 'z'){
      digitalWrite (DRA1, HIGH);
      digitalWrite (DRA2, LOW); //set opposite DRA1 for cc or ccw rotation, set equal to DRA1 for motor off 
      analogWrite (SPA, 255);
      
    }
    

    /////END MOTOR A CONTROLS//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //BEGIN MOTOR B CONTROLS
    if((char) c == 'A'){
      digitalWrite (DRB1, LOW);
      digitalWrite (DRB2, HIGH); //set opposite DRB1 for cc or ccw rotation, set equal to DRB1 for motor off 
      analogWrite (SPB, 255);
      
    }
    if((char) c == 'B'){
      digitalWrite (DRB1, LOW);
      digitalWrite (DRB2, HIGH); //set opposite DRB1 for cc or ccw rotation, set equal to DRB1 for motor off 
      analogWrite (SPB, 235);
      
    }
    if((char) c == 'C'){
      digitalWrite (DRB1, LOW);
      digitalWrite (DRB2, HIGH); //set opposite DRB1 for cc or ccw rotation, set equal to DRB1 for motor off 
      analogWrite (SPB, 215);
      
    }
    if((char) c == 'D'){
      digitalWrite (DRB1, LOW);
      digitalWrite (DRB2, HIGH); //set opposite DRB1 for cc or ccw rotation, set equal to DRB1 for motor off 
      analogWrite (SPB, 195);
      
    }
    if((char) c == 'E'){
      digitalWrite (DRB1, LOW);
      digitalWrite (DRB2, HIGH); //set opposite DRB1 for cc or ccw rotation, set equal to DRB1 for motor off 
      analogWrite (SPB, 175);
      
    }
    if((char) c == 'F'){
      digitalWrite (DRB1, LOW);
      digitalWrite (DRB2, HIGH); //set opposite DRB1 for cc or ccw rotation, set equal to DRB1 for motor off 
      analogWrite (SPB, 155);
      
    }
    if((char) c == 'G'){
      digitalWrite (DRB1, LOW);
      digitalWrite (DRB2, HIGH); //set opposite DRB1 for cc or ccw rotation, set equal to DRB1 for motor off 
      analogWrite (SPB, 135);
      
    }
    if((char) c == 'H'){
      digitalWrite (DRB1, LOW);
      digitalWrite (DRB2, HIGH); //set opposite DRB1 for cc or ccw rotation, set equal to DRB1 for motor off 
      analogWrite (SPB, 115);
      
    }
    if((char) c == 'I'){
      digitalWrite (DRB1, LOW);
      digitalWrite (DRB2, HIGH); //set opposite DRB1 for cc or ccw rotation, set equal to DRB1 for motor off 
      analogWrite (SPB, 95);
      
    }
    if((char) c == 'J'){
      digitalWrite (DRB1, LOW);
      digitalWrite (DRB2, HIGH); //set opposite DRB1 for cc or ccw rotation, set equal to DRB1 for motor off 
      analogWrite (SPB, 75);
      
    }
    if((char) c == 'K'){
      digitalWrite (DRB1, LOW);
      digitalWrite (DRB2, HIGH); //set opposite DRB1 for cc or ccw rotation, set equal to DRB1 for motor off 
      analogWrite (SPB, 0);
      
    }
    if((char) c == 'L'){
      digitalWrite (DRB1, LOW);
      digitalWrite (DRB2, HIGH); //set opposite DRB1 for cc or ccw rotation, set equal to DRB1 for motor off 
      analogWrite (SPB, 0);
      
    }
    //dead zone
    if((char) c == 'M'){
      digitalWrite (DRB1, LOW);
      digitalWrite (DRB2, HIGH); //set opposite DRB1 for cc or ccw rotation, set equal to DRB1 for motor off 
      analogWrite (SPB, 0);
      
    }
    //end of dead zone

    //begin reverse()
    if((char) c == 'N'){
      digitalWrite (DRB1, HIGH);
      digitalWrite (DRB2, LOW); //set opposite DRB1 for cc or ccw rotation, set equal to DRB1 for motor off 
      analogWrite (SPB, 0);
      
    }
    if((char) c == 'O'){
      digitalWrite (DRB1, HIGH);
      digitalWrite (DRB2, LOW); //set opposite DRB1 for cc or ccw rotation, set equal to DRB1 for motor off 
      analogWrite (SPB, 0);
      
    }
    if((char) c == 'P'){
      digitalWrite (DRB1, HIGH);
      digitalWrite (DRB2, LOW); //set opposite DRB1 for cc or ccw rotation, set equal to DRB1 for motor off 
      analogWrite (SPB, 0);
      
    }
    if((char) c == 'Q'){
      digitalWrite (DRB1, HIGH);
      digitalWrite (DRB2, LOW); //set opposite DRB1 for cc or ccw rotation, set equal to DRB1 for motor off 
      analogWrite (SPB, 75);
      
    }
    if((char) c == 'R'){
      digitalWrite (DRB1, HIGH);
      digitalWrite (DRB2, LOW); //set opposite DRB1 for cc or ccw rotation, set equal to DRB1 for motor off 
      analogWrite (SPB, 95);
      
    }
    if((char) c == 'S'){
      digitalWrite (DRB1, HIGH);
      digitalWrite (DRB2, LOW); //set opposite DRB1 for cc or ccw rotation, set equal to DRB1 for motor off 
      analogWrite (SPB, 115);
      
    }
    if((char) c == 'T'){
      digitalWrite (DRB1, HIGH);
      digitalWrite (DRB2, LOW); //set opposite DRB1 for cc or ccw rotation, set equal to DRB1 for motor off 
      analogWrite (SPB, 135);
      
    }
    if((char) c == 'U'){
      digitalWrite (DRB1, HIGH);
      digitalWrite (DRB2, LOW); //set opposite DRB1 for cc or ccw rotation, set equal to DRB1 for motor off 
      analogWrite (SPB, 155);
      
    }
    if((char) c == 'V'){
      digitalWrite (DRB1, HIGH);
      digitalWrite (DRB2, LOW); //set opposite DRB1 for cc or ccw rotation, set equal to DRB1 for motor off 
      analogWrite (SPB, 175);
      
    }
    if((char) c == 'W'){
      digitalWrite (DRB1, HIGH);
      digitalWrite (DRB2, LOW); //set opposite DRB1 for cc or ccw rotation, set equal to DRB1 for motor off 
      analogWrite (SPB, 195);
      
    }
    if((char) c == 'X'){
      digitalWrite (DRB1, HIGH);
      digitalWrite (DRB2, LOW); //set opposite DRB1 for cc or ccw rotation, set equal to DRB1 for motor off 
      analogWrite (SPB, 215);
      
    }
    if((char) c == 'Y'){
      digitalWrite (DRB1, HIGH);
      digitalWrite (DRB2, LOW); //set opposite DRB1 for cc or ccw rotation, set equal to DRB1 for motor off 
      analogWrite (SPB, 235);
      
    }
    if((char) c == 'Z'){
      digitalWrite (DRB1, HIGH);
      digitalWrite (DRB2, LOW); //set opposite DRB1 for cc or ccw rotation, set equal to DRB1 for motor off 
      analogWrite (SPB, 255);
      
    }
    
    
    /////////////////END MOTOR B CONTROLS//////////////////////////////////////////////////////////////////
    
    
  }
}