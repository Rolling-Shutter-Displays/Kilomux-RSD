/* Autor: derfaq (Facundo Daguerre) - Rolling Shutter Displays 
 * Date: 11/06/19
 */

/*  Layout /////////////////////////////////////////////////////////////////////////////
 * 
 * ╔═════════════════════════════════════════════════════════════════╗
 * ║                                                                 ║
 * ║   Sintonía                                                      ║
 * ║   Gruesa   Fina     Shift    HBlank                             ║
 * ║     ○        ○        ○        ○                                ║
 * ║                                                 RSD             ║
 * ║   Param    Param    Param    Param               ☼              ║
 * ║     x        x        x        x                                ║
 * ║     ○        ○        ○        ○                                ║
 * ║                                                                 ║
 * ║                                                                 ║
 * ║    On/      Bloq.    Play     Next                              ║
 * ║    Off      Freq.    Pause    Button                            ║
 * ║     .        .        .        .                                ║
 * ║     ■        ■        ■        ■                                ║
 * ║                                                                 ║
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

unsigned int pot[4];
unsigned int prevPot[4];

bool buttonState[4];
bool buttonLastState[4];
char buttonPushCounter[4];   // counter for the number of button presses

bool led[4];

// States
bool bloq = false;
bool pause = false;

// Programs  ///////////////////////////////////////////////////////////////////////////////
#include "Utils.h"
#include "Program.h"

#include "TestScreenRGB.h"
#include "MirrorShift.h"
//#include "Paint.h"
#include "Noise.h"
#include "FourColors.h"
#include "Zoom.h"
#include "Twist.h"
#include "Triangles.h"
#include "Triangles2.h"
#include "Player.h"

Program* programs[8] = { &player , &testScreenRGB , &twist , &triangles2 , &mirrorShift , &zoom , &noise , &fourColors  };

int program = 0;
const int program_size = 7;

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
  
  rsd.attachChannel( &red );
  rsd.attachChannel( &green );
  rsd.attachChannel( &blue );
  rsd.attachChannel( &white );

  rsd.attachDraw( draw );


  //Programs setup
  for( int i = 0 ; i <= program_size ; i++ ) {
    programs[i]->setup();
  }
  programs[program]->reset();
  
  //Comunications
  Serial.begin( 115200 );
  
}

// For ever  ///////////////////////////////////////////////////////////////////////////

void loop() {
  //Run the RSD engine
  rsd.update();
                                                            
}

// Let's draw! //////////////////////////////////////////////////////////////////////////

void draw() {

  //Tuning: Kilomux way
  int tick = map( KmShield.analogReadKm( MUX_A, 0 ) , 0 , 1023 , rsd.getLowerTick() , rsd.getHigherTick() );
  int fine = map( KmShield.analogReadKm( MUX_A, 1 ) , 0 , 1023 , rsd.getLowerFine() , rsd.getHigherFine() );

  if ( !bloq ) {
    rsd.setTick( tick );
    rsd.setFine( fine );
  }

  programs[program]->draw();
  
  //Update Km
  updateKm();

  //Shift phase: Kilomux way
  rsd.shiftPhase( map( pot[2] , 0 , 1023 , -1 , +2 ) );

  //Debug
  Serial.println( frameLost );
}

// Update Kilomux ///////////////////////////////////////////////////////////////////////

void updateKm() {
  
  // Update general RSD controls //
  
  for( int i = 0 ; i < 4 ; i++ ) {
    //Update pots
    pot[i] = KmShield.analogReadKm( MUX_A, i );
  
    //Update buttons
    buttonState[i] = KmShield.digitalReadKm( MUX_B , i , PULLUP );

    //Updete states
    
    //Click detection
    if ( ( buttonState[i] == LOW ) && ( buttonLastState[i] == HIGH ) ) {
        buttonPushCounter[i]++;
    }

    switch( i ) {

      //Button 0 = On/Off
      case 0: 
        if ( buttonPushCounter[i]&1 ) {
          led[i] = HIGH;
          rsd.switchOff();
        } else {
          led[i] = LOW;
          rsd.switchOn();
        }
      break;
      
      //Button 1 = Bloq Freq
      case 1: 
        if ( buttonPushCounter[i]&1 ) {
          led[i] = HIGH;
          bloq = true;
        } else {
          led[i] = LOW;
          bloq = false;
        }
      break;
      
      //Button 2 = Play/Pause
      case 2: 
        if ( buttonPushCounter[i]&1 ) {
          led[i] = HIGH;
          programs[program]->pause();
        } else {
          led[i] = LOW;
          programs[program]->play();
        }
      break;
         
      //Button 3 = Next Button
      case 3: //next Button
        if ( !buttonState[i] ) {
          led[i] = HIGH;
          
          if ( buttonLastState[i] ) {
            if ( program < program_size ) {
              program++;
            } else {
              program = 0;
            }
            programs[program]->reset();
            //Pause issue
            buttonPushCounter[2] = 0;
            led[2] = 0;
            //
          }
        } else {
          led[i] = LOW;
        }
      break;
      
      default: break;
    }

    //Save lectures
    prevPot[i] = pot[i];
    buttonLastState[i]= buttonState[i];

    //Update leds
    KmShield.digitalWriteKm( i + 8 , led[i] );
  }

  // Update programs controls //
  for( int i = 0 ; i < 4 ; i++ ) {
    //Update pots
    programs[program]->pot[i] = KmShield.analogReadKm( MUX_A , i + 4 );

    //Update buttons
    programs[program]->buttonState[i] = KmShield.digitalReadKm( MUX_B , i + 4 , PULLUP );

  }

  //Update states program
  programs[program]->updateState();


  //Update leds 
  for( int i = 0 ; i < 4 ; i++ ) {
    KmShield.digitalWriteKm( i + 12 , programs[program]->led[i] );   
  }

}
