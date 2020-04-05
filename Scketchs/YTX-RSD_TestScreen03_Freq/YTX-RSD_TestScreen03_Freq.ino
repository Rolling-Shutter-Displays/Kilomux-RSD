/* Autor: derfaq (Facundo Daguerre) - Rolling Shutter Displays
   Date: 09/27/19
*/

/*
   Inclusión de librerías.
*/

#include <Kilomux.h>              // Import class declaration
#include <KilomuxDefs.h>          // Import Kilomux defines

#include <RSD.h>
#include <Channel.h>
#include <Screen.h>

// DEFINES PARA EL RSD

//6 = White = ActivateSensorButtonPin
//7 = Green = ActivateSensorLedPin
//8 = Blue  = SensorEchoPin
//9 = Red   = SensorTriggerPin

#define BWIDTH 32
#define WIDTH ((BWIDTH*8)-1)
#define PIN_R   SensorTriggerPin        // Pin de arduino conectado al LED Rojo
#define PIN_G   ActivateSensorLedPin    // Pin de arduino conectado al LED Verde
#define PIN_B   SensorEchoPin           // Pin de arduino conectado al LED Azul
#define PIN_W   ActivateSensorButtonPin // Pin de arduino conectado al LED Blanco

#define SERIAL_COMMS

// Instancias de objetos //////////////////////////////////////////////////////////////////

Kilomux KmShield;                                       // Objeto de la clase Kilomux

RSD rsd;

Channel red( PIN_R , COMMON_CATHODE , BWIDTH );
Channel green( PIN_G , COMMON_CATHODE , BWIDTH );
Channel blue( PIN_B , COMMON_CATHODE , BWIDTH );
Channel white( PIN_W , COMMON_CATHODE , BWIDTH );

Screen display( &red , &green , &blue );

///////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  KmShield.init();                                    // Initialize Kilomux shield hardware

  //Setup of the RSD
  rsd.begin( 30 , BWIDTH );

  rsd.attachChannel( &red );
  rsd.attachChannel( &green );
  rsd.attachChannel( &blue );
  rsd.attachChannel( &white );

  rsd.attachDraw( draw );

  Serial.begin(9600);

}

void loop() {
  //Run the RSD engine
  rsd.update();

  // Tuning: KiloMux way
  float freq = map( KmShield.analogReadKm( MUX_A, 0 ) , 0 , 1023 , 23 , 123 ) + 0.01 * map( KmShield.analogReadKm( MUX_A, 1 ) , 0 , 1023 , 0 , +100 )
                                                                           + 0.0001 * map( KmShield.analogReadKm( MUX_A, 2 ) , 0 , 1023 , -100 , +100 );
  rsd.setFrequency( freq );
  Serial.println( rsd.getFrequency() , 10 );
}

//Let's draw!
void draw() {
  display.clear();
  white.clear();

  int inc = map( KmShield.analogReadKm( MUX_A, 3 ) , 0 , 1023 , 1 , 10 );

  display.fill( 0 , inc , WHITE );
  white.fill( 0 , inc );

  //display.fill( WIDTH / 4 , WIDTH / 4 + inc , MAGENTA );

  //display.fill( WIDTH / 2 , WIDTH / 2 + inc, YELLOW );

  //display.fill( WIDTH * 3 / 4 ,  WIDTH * 3 / 4 + inc, CYAN );
}
