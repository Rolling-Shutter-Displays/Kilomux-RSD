/* Autor: derfaq (Facundo Daguerre) - Rolling Shutter Displays 
 * Date: 11/06/19
 */
 
/*  Layout /////////////////////////////////////////////////////////////////////////////
 * 
 * ╔═════════════════════════════════════════════════════════════════╗
 * ║                                                                 ║
 * ║   Main     Sintonía                                             ║
 * ║   Param    Gruesa   Fina     Shift                              ║
 * ║     ○        ○        ○        ○                                ║
 * ║    p[0]     p[1]     p[2]     p[3]              RSD             ║
 * ║   Param    Param    Param    Param               ☼              ║
 * ║     x        x        x        x                                ║
 * ║     ○        ○        ○        ○                                ║
 * ║    p[5]     p[6]     p[7]     p[8]                              ║
 * ║                                                                 ║
 * ║    Bloq.    Play     Prev     Next                              ║
 * ║    Freq.    Pause    Button   Button                            ║
 * ║     .        .        .        .                                ║
 * ║     ■        ■        ■        ■                                ║
 * ║   ¿Save      MIDI                 } Hold States ?               ║
 * ║                                                                 ║               
 * ║                                                                 ║
 * ║     x        x        x        x                                ║
 * ║     .        .        .        .                                ║
 * ║     ■        ■        ■        ■                                ║
 * ║                                                                 ║
 * ╚═════════════════════════════════════════════════════════════════╝
 * 
 */

// Libraries  ////////////////////////////////////////////////////////////////////////////
 
#include <Kilomux.h>              // Import class declaration
#include <KilomuxDefs.h>          // Import Kilomux defines

#include <RSD.h>
#include <Channel.h>
#include <Screen.h>

// Definitions ////////////////////////////////////////////////////////////////////////////

// 6 = White = ActivateSensorButtonPin
// 7 = Green = ActivateSensorLedPin
// 8 = Blue  = SensorEchoPin
// 9 = Red   = SensorTriggerPin
// TODO: Pin-out

#define BWIDTH 32
#define WIDTH ((BWIDTH*8)-1)
#define PIN_R   SensorTriggerPin        // Pin de arduino conectado al LED Rojo
#define PIN_G   ActivateSensorLedPin    // Pin de arduino conectado al LED Verde
#define PIN_B   SensorEchoPin           // Pin de arduino conectado al LED Azul
#define PIN_W   ActivateSensorButtonPin // Pin de arduino conectado al LED Blanco

// Objets /////////////////////////////////////////////////////////////////////////////////

Kilomux KmShield;                                       // Objeto de la clase Kilomux

RSD rsd;

Channel red( PIN_R , COMMON_CATHODE , BWIDTH );
Channel green( PIN_G , COMMON_CATHODE , BWIDTH );
Channel blue( PIN_B , COMMON_CATHODE , BWIDTH );
Channel white( PIN_W , COMMON_CATHODE , BWIDTH );

Screen display( &red , &green , &blue );

Channel *ch[4] = { &red , &green , &blue , &white  };

// Global variables  //////////////////////////////////////////////////////////////////////

unsigned int pot[8];
unsigned int prevPot[8];

bool buttonState[8];
bool buttonLastState[8];
char buttonPushCounter[8];   // counter for the number of button presses

bool led[8];

bool bloq = false;
bool pause = false;

// Screens  ///////////////////////////////////////////////////////////////////////////////

#include "Screens.h"

//  Beginnig  /////////////////////////////////////////////////////////////////////////////

void setup() { 
  //Setup of Kilomux 
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

  //Setup of RSD 
  rsd.begin( 30 , BWIDTH );
  
  rsd.attachChannel( red );
  rsd.attachChannel( green );
  rsd.attachChannel( blue );
  rsd.attachChannel( white );

  rsd.attachDraw( draw );

  //Comunications
  //Serial.begin( 115200 );
  
}

// For ever  ///////////////////////////////////////////////////////////////////////////

void loop() {
  //Run the RSD engine
  rsd.update();
  
  //Tuning: Kilomux way
  int tick = map( KmShield.analogReadKm( MUX_A, 1 ) , 0 , 1023 , rsd.getLowerTick() , rsd.getHigherTick() );
  int fine = map( KmShield.analogReadKm( MUX_A, 2 ) , 0 , 1023 , rsd.getLowerFine() , rsd.getHigherFine() );

  if ( !bloq ) {
    rsd.setTick( tick );
    rsd.setFine( fine );
  }
                                                             
}

// Let's draw! //////////////////////////////////////////////////////////////////////////

void draw() {

  screens[screen]();
  
  //Serial.println( frameLost );
  
  //Update Km
  updateKm();

  //Shift phase: Kilomux way
  rsd.shiftPhase( map( pot[3] , 0 , 1023 , -1 , +2 ) );
  
}

// Update Kilomux ///////////////////////////////////////////////////////////////////////

void updateKm() {
  //Update pots
  for( int i = 0 ; i < 8 ; i++ ) {
    prevPot[i] = pot[i];
    pot[i] = KmShield.analogReadKm( MUX_A, i );
  }
  
  //Update buttons
  for( int i = 0 ; i < 8 ; i++ ) {
    buttonState[i] = KmShield.digitalReadKm( MUX_B , i , PULLUP );

    //Click detection
    if ( ( buttonState[i] == LOW ) && ( buttonLastState[i] == HIGH ) ) {
        buttonPushCounter[i]++;
    }

    //Release detection
    /*
    if ( ( buttonState[i] == LOW ) && ( buttonLastState[i] == HIGH ) ) {
        //buttonPushCounter[i]++;
    }
    */

    //Updete states
    updateStateMainControl();
    
    buttonLastState[i]= buttonState[i];
  }

  
  //Update leds
  for( int i = 0 ; i < 4 ; i++ ) {
    KmShield.digitalWriteKm( i + 8 , led[i] );
  }

}

void updateStateMainControl() {

  for( int i = 0 ; i < 4 ; i++ ) {
    switch( i ) {
      //Button 0 = Bloq Freq
      case 0: 
        if ( buttonPushCounter[0]&1 ) {
          led[0] = HIGH;
          bloq = true;
        } else {
          led[0] = LOW;
          bloq = false;
        }
      break;
      
      //Button 1 = Play/Pause
      case 1: 
        if ( buttonPushCounter[1]&1 ) {
          led[1] = HIGH;
          pause = true;
        } else {
          led[1] = LOW;
          pause = false;
        }
      break;
      
      //Button 2 = Prev Button
      case 2:
      if ( !buttonState[i] ) {
        led[i] = HIGH;
        if ( buttonLastState[i] ) {
          if ( screen > 0 ) {
            screen--;
          } else {
            screen = screen_size;
          }
        }
      } else {
        led[i] = LOW;
      }
      break;
      
      //Button 3 = Next Button
      case 3: //next Button
        if ( !buttonState[i] ) {
          led[i] = HIGH;
          if ( buttonLastState[i] ) {
            if ( screen < screen_size ) {
              screen++;
            } else {
              screen = 0;
            }
          }
        } else {
          led[i] = LOW;
        }
      break;
      
      default: break;
    }
  }
}
