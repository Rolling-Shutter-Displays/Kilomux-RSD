/* Autor: derfaq (Facundo Daguerre) - Rolling Shutter Displays 
 * Date: 10/27/19
 */

/*
 * Inclusión de librerías. 
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
// Global variables

unsigned int pot[8];
bool button[8];

void setup() { 
  KmShield.init();                                    // Initialize Kilomux shield hardware

  //Setup of the RSD 
  rsd.begin( 30 , BWIDTH );
  
  rsd.attachChannel( red );
  rsd.attachChannel( green );
  rsd.attachChannel( blue );
  rsd.attachChannel( white );

  rsd.attachDraw( draw );

  updateKm();
  
#if defined( SERIAL_COMMS )
  Serial.begin( 115200 );
#endif

}

void loop() {
  //Run the RSD engine
  rsd.update();
  
  // Tuning: KiloMux way
  int tick = map( pot[1] , 0 , 1023 , rsd.getLowerTick() , rsd.getHigherTick() );
  int fine = map( pot[2] , 0 , 1023 , rsd.getLowerFine() , rsd.getHigherFine() );
  rsd.setTick( tick );
  rsd.setFine( fine );
                                                             
}

//Let's draw!
void draw() {
  //Copy background
  red.copy( &red );
  green.copy( &green );
  blue.copy( &blue );
  
  white.clear();

  updateKm();

  //Cursor
  if ( (frameCount%15) < 7 )  white.line( pot[0]>>2 );
  
  //Paint
  for ( int i = 0 ; i < 8 ; i++ ) {
    if( !button[i] ) display.line( pot[0]>>2 , i );
  }

}

void updateKm() {
  // Update pot values
  for( int i = 0 ; i < 8 ; i++ ) {
    pot[i] = KmShield.analogReadKm( MUX_A, i );
  }
  
  // Update button states
  for( int i = 0 ; i < 8 ; i++ ) {
    button[i] = KmShield.digitalReadKm( MUX_B , i , PULLUP );
  }
}
