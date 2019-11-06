/* Autor: derfaq (Facundo Daguerre) - Rolling Shutter Displays 
 * Date: 11/02/19
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
void updateKm();

RSD rsd;

Channel red( PIN_R , COMMON_CATHODE , BWIDTH );
Channel green( PIN_G , COMMON_CATHODE , BWIDTH );
Channel blue( PIN_B , COMMON_CATHODE , BWIDTH );
Channel white( PIN_W , COMMON_CATHODE , BWIDTH );

Screen display( &red , &green , &blue );

Channel *ch[4] = { &red , &green , &blue , &white };

///////////////////////////////////////////////////////////////////////////////////////////
// Global variables

unsigned int pot[8];
unsigned int prevPot[8];

bool button[8];
bool prevButton[8];

void setup() { 
  KmShield.init();                                    // Initialize Kilomux shield hardware
  updateKm();

  { // Presentation
  KmShield.digitalWritePortKm( 0x18 , 2 );
  delay(200);
  KmShield.digitalWritePortKm( 0x24 , 2 );
  delay(50);
  KmShield.digitalWritePortKm( 0x42 , 2 );
  delay(50);
  KmShield.digitalWritePortKm( 0x81 , 2 );
  delay(50);
  KmShield.digitalWritePortKm( 0x00 , 2 );
  delay(100);
  KmShield.digitalWritePortKm( 0x81 , 2 );
  delay(100);
  KmShield.digitalWritePortKm( 0x00 , 2 );
  }
  
  //Setup of the RSD 
  rsd.begin( 30 , BWIDTH );
  
  rsd.attachChannel( red );
  rsd.attachChannel( green );
  rsd.attachChannel( blue );
  rsd.attachChannel( white );

  rsd.attachDraw( draw );
  
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

  //White channel is used only for cursor
  white.clear();

  updateKm();

  //Shift phase: Kilomux way
  rsd.shiftPhase( map( pot[3] , 0 , 1023 , -1 , +2 ) );

  //Cursor
  if ( (frameCount%15) < 7 )  white.line( pot[0]>>2 );

  //Paint
  for ( int i = 0 ; i < 3 ; i++ ) {
    if( !button[i + 4] ) {
      KmShield.digitalWriteKm( i + 12 , HIGH );
      if( prevButton[i + 4] ) { 
        ch[i]->line( pot[0]>>2 ); // rising edge
      } else {
        ch[i]->fill( prevPot[0]>>2 , pot[0]>>2 ); // high state
      }
    } else {
      KmShield.digitalWriteKm( i + 12 , LOW ); // low state
    }
  }

  // Clear button = White button
  if( !button[7] ) {
    KmShield.digitalWriteKm( 15 , HIGH );
    if( prevButton[7] ) { 
        display.clearSafe( pot[0]>>2 ); // rising edge
      } else {
        display.clearSafe( prevPot[0]>>2 , pot[0]>>2 ); // high state
      }
  } else {
    KmShield.digitalWriteKm( 15 , LOW ); // low state
  }

}

void updateKm() {
  // Update pot values
  for( int i = 0 ; i < 8 ; i++ ) {
    prevPot[i] = pot[i];
    pot[i] = KmShield.analogReadKm( MUX_A, i );
  }
  
  // Update button states
  for( int i = 0 ; i < 8 ; i++ ) {
    prevButton[i] = button[i];
    button[i] = KmShield.digitalReadKm( MUX_B , i , PULLUP );
  }
}
