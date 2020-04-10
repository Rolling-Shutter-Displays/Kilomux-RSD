/* Autor: derfaq (Facundo Daguerre) - Rolling Shutter Displays 
 * Date: 05/04/20
 */
 
/*  Layout /////////////////////////////////////////////////////////////////////////////
 * 
 * ╔══════════════════════════════════════╗
 * ║                                                                 ║
 * ║   Main     Sintonía                                             ║
 * ║   Param    Gruesa   Fina     Shift                              ║
 * ║     ○        ○        ○        ○                              ║
 * ║    p[0]     p[1]     p[2]     p[3]              RSD             ║
 * ║   Param    Param    Param    Param               ☼              ║
 * ║     x        x        x        x                                ║
 * ║     ○        ○        ○        ○                              ║
 * ║    p[5]     p[6]     p[7]     p[8]                              ║
 * ║                                                                 ║
 * ║     On       Bloq.                                              ║
 * ║     Off      Freq      x        x                               ║
 * ║     .        .         .        .                               ║
 * ║     ■        ■        ■        ■                              ║
 * ║                                                                 ║
 * ║                                                                 ║               
 * ║                                                                 ║
 * ║     x        x        x        x                                ║
 * ║     .        .        .        .                                ║
 * ║     ■        ■        ■        ■                              ║
 * ║                                                                 ║
 * ╚══════════════════════════════════════╝
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

  //Comunications
  MIDI.setHandleNoteOn( handleNoteOn ); 
  MIDI.setHandleNoteOff( handleNoteOff );
  
  MIDI.begin( MIDI_CHANNEL_OMNI );


}

// For ever  ///////////////////////////////////////////////////////////////////////////

void loop() {
  //Run the RSD engine
  rsd.update();
  
  if ( !bloq ) {
    //Tuning: Kilomux way
    int thick = map( KmShield.analogReadKm( MUX_A, 0 ) , 0 , 1023 , rsd.getLowerThick() , rsd.getHigherThick() );
    int fine = map( KmShield.analogReadKm( MUX_A, 1 ) , 0 , 1023 , rsd.getLowerFine() , rsd.getHigherFine() );
    rsd.setThick( thick );
    rsd.setFine( fine );
  }
  
  //MIDI update
  MIDI.read();                                                    
}

//Kick
bool kick = false;
unsigned char wide = 0;
colour pallete[2] = { YELLOW , RED };
colour c;
unsigned char counterColour = 0;

//Snare

bool snare = false;
unsigned char density = 0;
bool randomComplete = false;

const PROGMEM uint8_t oneBitsSet[8]    = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
const PROGMEM uint8_t twoBitsSet[28]   = { 0x03, 0x05, 0x06, 0x09, 0x0A, 0x0C, 0x11, 0x12, 0x14, 0x18, 0x21, 0x22, 0x24, 0x28, 0x30, 0x41, 0x42, 0x44, 0x48, 0x50, 0x60, 0x81, 0x82, 0x84, 0x88, 0x90, 0xA0, 0xC0 };
const PROGMEM uint8_t threeBitsSet[56] = { 0x07, 0x0B, 0x0D, 0x0E, 0x13, 0x15, 0x16, 0x19, 0x1A, 0x1C, 0x23, 0x25, 0x26, 0x29, 0x2A, 0x2C, 0x31, 0x32, 0x34, 0x38, 0x43, 0x45, 0x46, 0x49, 0x4A, 0x4C, 0x51, 0x52, 0x54, 0x58, 0x61, 0x62, 0x64, 0x68, 0x70, 0x83, 0x85, 0x86, 0x89, 0x8A, 0x8C, 0x91, 0x92, 0x94, 0x98, 0xA1, 0xA2, 0xA4, 0xA8, 0xB0, 0xC1, 0xC2, 0xC4, 0xC8, 0xD0, 0xE0 };
const PROGMEM uint8_t fourBitsSet[70]  = { 0x0F, 0x17, 0x1B, 0x1D, 0x1E, 0x27, 0x2B, 0x2D, 0x2E, 0x33, 0x35, 0x36, 0x39, 0x3A, 0x3C, 0x47, 0x4B, 0x4D, 0x4E, 0x53, 0x55, 0x56, 0x59, 0x5A, 0x5C, 0x63, 0x65, 0x66, 0x69, 0x6A, 0x6C, 0x71, 0x72, 0x74, 0x78, 0x87, 0x8B, 0x8D, 0x8E, 0x93, 0x95, 0x96, 0x99, 0x9A, 0x9C, 0xA3, 0xA5, 0xA6, 0xA9, 0xAA, 0xAC, 0xB1, 0xB2, 0xB4, 0xB8, 0xC3, 0xC5, 0xC6, 0xC9, 0xCA, 0xCC, 0xD1, 0xD2, 0xD4, 0xD8, 0xE1, 0xE2, 0xE4, 0xE8, 0xF0 };

// Let's draw! //////////////////////////////////////////////////////////////////////////
void draw() {
  
  display.clear();
  //white.clear();

  if( kick ) display.fill( WIDTH/2 - wide , WIDTH/2 + wide , pallete[counterColour] );

  if ( !snare ) {
    white.clear();
    randomComplete = false;
  } else {
    if ( randomComplete ) {
      white.copy( &white );
    } else {
      for( int i = 0 ; i < BWIDTH ; i++ ) {
        switch( density ) {
          case 0:
            *(ch[3]->get() + i) = pgm_read_byte( oneBitsSet + random( 0 , 9 ) );
            break;
          case 1:
            *(ch[3]->get() + i) = pgm_read_byte( twoBitsSet + random( 0 , 29 ) );
            break;
          case 2:
            *(ch[3]->get() + i) = pgm_read_byte( threeBitsSet + random( 0 , 57 ) );
            break;
          case 3:
            *(ch[3]->get() + i) = pgm_read_byte( fourBitsSet + random( 0 , 71 ) );
          break;
          default: break;
        }
      }
      randomComplete = true;
    }
  }
  
  /*
  if( kick ) {
    display.fill( WIDTH/4 - wide/2 , WIDTH/4 + wide/2 , pallete[counterColour] );
    display.fill( WIDTH*3/4 - wide/2 , WIDTH*3/4 + wide/2 , pallete[counterColour] );
  }
  */
  
  //Update Km
  updateKm();

  //Shift phase: Kilomux way
  rsd.shiftPhase( map( pot[2] , 0 , 1023 , -1 , +2 ) );
}

// MIDI Input  //////////////////////////////////////////////////////////////////////////

void handleNoteOn( byte channel , byte pitch , byte velocity ) {
  if ( velocity == 0 )  handleNoteOff( channel , pitch , velocity );
  
  switch( channel ) {
    case 1: 
      kick = true;
      wide = velocity;
      counterColour = 1 - counterColour;
      led[ 3 + channel ] = HIGH;
    break;
    case 2: 
      snare = true;
      density = velocity>>5;
      led[ 3 + channel ] = HIGH;
    break;
    
    default: break;
  }
  
}


void handleNoteOff( byte channel , byte pitch , byte velocity ) {
  switch( channel ) {
    case 1: 
      kick = false;
      led[ 3 + channel ] = LOW;
    break;
    case 2: 
      snare = false;
      led[ 3 + channel ] = LOW;
    break;
    
    default: break;
  }
  
  
}

// Update Kilomux ///////////////////////////////////////////////////////////////////////

void updateKm() {
  
  for( int i = 0 ; i < 8 ; i++ ) {
    //Update pots
    pot[i] = KmShield.analogReadKm( MUX_A, i );
  
    //Update buttons
    buttonState[i] = KmShield.digitalReadKm( MUX_B , i , PULLUP );
    
    //Click detection
    if ( ( buttonState[i] == LOW ) && ( buttonLastState[i] == HIGH ) ) {
        buttonPushCounter[i]++;
    }
    
    switch ( i ) {

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
      /*  
      //Button 2 = Background keyboard
      case 2: 
        if ( buttonPushCounter[i]&1 ) {
          led[i] = HIGH;
          pianoKeyboardVisible = true;
        } else {
          led[i] = LOW;
          pianoKeyboardVisible = false;
        }
      break;
   /*
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
    /*
    case( 4 ):
      //Push
      if ( !buttonState[i] && buttonLastState[i] ) {
        MIDI.sendNoteOn( 60 , 127 , 1 );
        notes[0][ 60%24 ] = true;
        led[i] = HIGH;
      }
      //Release
      if( buttonState[i] && !buttonLastState[i] ){
        MIDI.sendNoteOff( 60 , 0 , 1 );
        notes[0][ 60%24 ] = false;
        led[i] = LOW;
      }
      break;
      
    case( 5 ):
      //Push
      if ( !buttonState[i] &&  buttonLastState[i] ) {
        MIDI.sendNoteOn( 60 , 127 , 2 );
        notes[1][ 60%24 ] = true;
        led[i] = HIGH;
      }
      //Release 
      if( buttonState[i] && !buttonLastState[i] ){
        MIDI.sendNoteOff( 60 , 0 , 2 );
        notes[1][ 60%24 ] = false;
        led[i] = LOW;
      }
      break;
    
    case( 6 ):
      //Push
      if ( !buttonState[i] && buttonLastState[i] ) {
        MIDI.sendNoteOn( 60 , 127 , 3 );
        notes[2][ 60%24 ] = true;
        led[i] = HIGH;
      } 
      //Release
      if( buttonState[i] && !buttonLastState[i] ){
        MIDI.sendNoteOff( 60 , 0 , 3 );
        notes[2][ 60%24 ] = false;
        led[i] = LOW;
      }
      break;
    
    /*
    case( 7 ):
      if ( buttonPushCounter[i]&1 ) {
        led[i] = HIGH;
        pianoKeyboardVisible = true;
      } else {
        led[i] = LOW;
        pianoKeyboardVisible = false;
      }
      break;
      */
     default: break;
    }
    
    buttonLastState[i]= buttonState[i];
  }

  
  //Update leds
  for( int i = 0 ; i < 8 ; i++ ) {
    KmShield.digitalWriteKm( i + 8 , led[i] );
  }

}
