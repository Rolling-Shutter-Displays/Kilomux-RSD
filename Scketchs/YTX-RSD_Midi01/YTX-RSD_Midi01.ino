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
 * ║    Bloq.                                                        ║
 * ║    Freq.     x        x        x                                ║
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

#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();

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
  MIDI.begin(MIDI_CHANNEL_OMNI);
  
}

// For ever  ///////////////////////////////////////////////////////////////////////////

void loop() {
  //Run the RSD engine
  rsd.update();
  
  if ( !bloq ) {
    //Tuning: Kilomux way
    int tick = map( KmShield.analogReadKm( MUX_A, 1 ) , 0 , 1023 , rsd.getLowerTick() , rsd.getHigherTick() );
    int fine = map( KmShield.analogReadKm( MUX_A, 2 ) , 0 , 1023 , rsd.getLowerFine() , rsd.getHigherFine() );
    rsd.setTick( tick );
    rsd.setFine( fine );
  }

  MIDI.read();
                                                             
}

// Let's draw! //////////////////////////////////////////////////////////////////////////

colour palette[3] =  { BLACK , BLUE , MAGENTA };

void draw() {
  //Copy background
  red.copy( &red );
  green.copy( &green );
  blue.copy( &blue );
  white.copy( &white );

  white.line( 129 );
  white.line( 116 );
  
  ShiftByOne( 130 , 255 );
  ShiftByOne( 115 , 0 );

  for( int i = 0 ; i < 4 ; i++ ) {
    if( !buttonState[ i + 4 ] ) {
      ch[i]->line( 115 );
      ch[i]->line( 130 );
    }
  }
  
  //Update Km
  updateKm();

  //Shift phase: Kilomux way
  rsd.shiftPhase( map( pot[3] , 0 , 1023 , -1 , +2 ) );
  
}

void ShiftByOne( int begin , int end ) {
  
  if( begin > end ) { //Shift Right
    int i = end;
    do {
      display.line( i , display.get( i + 1 ) );
      white.line( i , (int)white.get( i + 1 ) );
      i++;
    } while ( i < begin );
    display.line( begin , BLACK );
    white.line( begin , BLACK );
  }

  if( begin < end ) { //Shift Left
    int i = end;
    do {
      display.line( i , display.get( i - 1 ) );
      white.line( i , (int)white.get( i - 1 ) );
      i--;
    } while ( i > begin );
    display.line( begin ,  BLACK );
    white.line( begin ,  BLACK );
  }
  
}

// Update Kilomux ///////////////////////////////////////////////////////////////////////

void updateKm() {
  //Update pot values
  for( int i = 0 ; i < 8 ; i++ ) {
    prevPot[i] = pot[i];
    pot[i] = KmShield.analogReadKm( MUX_A, i );
  }
  
  //Update button states
  for( int i = 0 ; i < 8 ; i++ ) {
    buttonState[i] = KmShield.digitalReadKm( MUX_B , i , PULLUP );
    
    switch ( i ) {

    case 0 : //Bloq button
      if ( buttonState[i] != buttonLastState[i] ) {
        if ( buttonState[i] == LOW ) buttonPushCounter[i]++;
      }
      break;
    /*
    case 1 : //Play-pause button
      if ( buttonState[i] != buttonLastState[i] ) {
        if ( buttonState[i] == LOW ) buttonPushCounter[i]++;
      }
      break;

    case 2: //prev Button
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
     break;
    */
    case( 4 ):
      if ( !buttonState[i] && buttonLastState[i] ) {
        MIDI.sendNoteOn(42, 127, 1);
      }
      if( buttonState[i] && !buttonLastState[i] ){
        MIDI.sendNoteOff(42, 0, 1);
      }
      break;
    case( 5 ):
      if ( !buttonState[i] &&  buttonLastState[i] ) {
        MIDI.sendNoteOn(43, 127, 1);
      } 
      if( buttonState[i] && !buttonLastState[i] ){
        MIDI.sendNoteOff(43, 0, 1);
      }
      break;
    case( 6 ):
      if ( !buttonState[i] && buttonLastState[i] ) {
        MIDI.sendNoteOn(44, 127, 1);
      } 
      if( buttonState[i] && !buttonLastState[i] ){
        MIDI.sendNoteOff(44, 0, 1);
      }
      break;
    case( 7 ):
      if ( !buttonState[i] && buttonLastState[i] ) {
        MIDI.sendNoteOn(45, 127, 1);
      } 
      if( buttonState[i] && !buttonLastState[i] ){
        MIDI.sendNoteOff(45, 0, 1);
      }
      break;
      
     default: break;
    }
    
    buttonLastState[i]= buttonState[i];
  }

  
  //Update leds and states
  for( int i = 0 ; i < 4 ; i++ ) {
    if ( i == 0 ) {
      if ( buttonPushCounter[i]&1 ) {
        led[i] = HIGH;
        bloq = true;
      } else {
        led[i] = LOW;
        bloq = false;
      }
    }

    if ( i == 1 ) {
      if ( buttonPushCounter[i]&1 ) {
        led[i] = HIGH;
        pause = true;
      } else {
        led[i] = LOW;
        pause = false;
      }
    }
    KmShield.digitalWriteKm( i + 8 , led[i] );
  }
}
