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


boolean isBlack[7] = { false , true , true , true , false , true , true };
boolean notes[24];

// MIDI Input  //////////////////////////////////////////////////////////////////////////
void handleNoteOn( byte channel , byte pitch , byte velocity ) {
    notes[ pitch%24 ] = true;
    led[ 4 + pitch%4 ] = HIGH;
}

void handleNoteOff( byte channel , byte pitch , byte velocity ) {
    notes[ pitch%24 ] = false;
    led[ 4 + pitch%4 ] = LOW;
}

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

// Let's draw! //////////////////////////////////////////////////////////////////////////

void draw() {
  
  display.clear();
  white.clear();

  //Piano black and white background
  int stroke = 2;
  
  for( int i = 0 ; i < 14 ; i++ ) {
    //Whites
    white.fill( (WIDTH*i/14) + stroke , (WIDTH*(i + 1))/14 - stroke ); //Multiply first, then divide 
    //Blacks
    if ( isBlack[i%7] ) {
      white.clear( (WIDTH*i/14) - WIDTH/48 , (WIDTH*i/14) + WIDTH/48  );
    }
  }

  //Event MIDI
  int note = 0;
  
  for( int i = 0 ; i < 14 ; i++ ) {
    //Whites
    if( notes[note] ){ 
      white.clear( (WIDTH*i/14) + stroke , (WIDTH*(i + 1))/14 - stroke );
      display.fill( (WIDTH*i/14) + stroke , (WIDTH*(i + 1))/14 - stroke , RED ); 
    }
    //Blacks
    if ( isBlack[i%7] ) {
      note++;
      if( notes[note] ){ 
        display.fill( (WIDTH*i/14) - WIDTH/48 , (WIDTH*i/14) + WIDTH/48 , MAGENTA ); 
      }
    }
    
    note++;
  }
  
  
  //Update Km
  updateKm();

  //Shift phase: Kilomux way
  rsd.shiftPhase( map( pot[3] , 0 , 1023 , -1 , +2 ) );
  
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
      //Push
      if ( !buttonState[i] && buttonLastState[i] ) {
        MIDI.sendNoteOn(131, 127, 1);
        notes[131%24] = true;
        led[i] = HIGH;
      }
      //Release
      if( buttonState[i] && !buttonLastState[i] ){
        MIDI.sendNoteOff(131, 0, 1);
        notes[131%24] = false;
        led[i] = LOW;
      }
      break;
      
    case( 5 ):
      //Push
      if ( !buttonState[i] &&  buttonLastState[i] ) {
        MIDI.sendNoteOn(132, 127, 1);
        notes[132%24] = true;
        led[i] = HIGH;
      }
      //Release 
      if( buttonState[i] && !buttonLastState[i] ){
        MIDI.sendNoteOff(132, 0, 1);
        notes[132%24] = false;
        led[i] = LOW;
      }
      break;
    
    case( 6 ):
      //Push
      if ( !buttonState[i] && buttonLastState[i] ) {
        MIDI.sendNoteOn(133, 127, 1);
        notes[133%24] = true;
        led[i] = HIGH;
      } 
      //Release
      if( buttonState[i] && !buttonLastState[i] ){
        MIDI.sendNoteOff(133, 0, 1);
        notes[133%24] = false;
        led[i] = LOW;
      }
      break;
      
    case( 7 ):
      //Push
      if ( !buttonState[i] && buttonLastState[i] ) {
        MIDI.sendNoteOn(134, 127, 1);
        notes[134%24] = true;
        led[i] = HIGH;
      }
      //Release 
      if( buttonState[i] && !buttonLastState[i] ){
        MIDI.sendNoteOff(134, 0, 1);
        notes[134%24] = false;
        led[i] = LOW;
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

  for( int i = 0 ; i < 4 ; i++ ) {
    KmShield.digitalWriteKm( i + 12 , led[ i + 4 ] );
  }
}
