/* Autor: derfaq (Facundo Daguerre) - Rolling Shutter Displays 
 * Date: 11/06/19
 */
 
/*  Layout /////////////////////////////////////////////////////////////////////////////
 * 
 * ╔═════════════════════════════════════════════════════════════════╗
 * ║                                                                 ║
 * ║                              Param                              ║
 * ║   Gruesa    Fina    Shift      x                                ║
 * ║     ○        ○        ○        ○                                ║
 * ║                                                 RSD             ║
 * ║   Param    Param    Param    Param               ☼              ║
 * ║     x        x        x        x                                ║
 * ║     ○        ○        ○        ○                                ║
 * ║                                                                 ║
 * ║    Bloq.    Play      Prev     Next                             ║
 * ║    Freq.    Pause     Button   Button                           ║
 * ║     .        .        .        .                                ║
 * ║     ■        ■        ■        ■                                ║
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

// Global variables  //////////////////////////////////////////////////////////////////////

unsigned int pot[8];
unsigned int prevPot[8];

bool buttonState[8];
bool buttonLastState[8];
char buttonPushCounter[8];   // counter for the number of button presses

bool led[8];

bool bloq = false;
bool play = false;

int screen = 0;
const int screen_size = 1;

//  Beginnig  /////////////////////////////////////////////////////////////////////////////

void setup() { 
  //Setup of Kilomux 
  KmShield.init();                                    // Initialize Kilomux shield hardware

  //Setup of RSD 
  rsd.begin( 30 , BWIDTH );
  
  rsd.attachChannel( red );
  rsd.attachChannel( green );
  rsd.attachChannel( blue );
  rsd.attachChannel( white );

  rsd.attachDraw( draw );

  //Comunications
  Serial.begin( 9600 );
  
}

// For ever  ///////////////////////////////////////////////////////////////////////////

void loop() {
  //Run the RSD engine
  rsd.update();
  
  //Tuning: Kilomux way
  int tick = map( KmShield.analogReadKm( MUX_A, 0 ) , 0 , 1023 , rsd.getLowerTick() , rsd.getHigherTick() );
  int fine = map( KmShield.analogReadKm( MUX_A, 1 ) , 0 , 1023 , rsd.getLowerFine() , rsd.getHigherFine() );

  if ( !bloq ) {
    rsd.setTick( tick );
    rsd.setFine( fine );
  }
                                                             
}

// Screens //////////////////////////////////////////////////////////////////////////////

void testScreenRGB() {
  //Clear screen
  display.clear();
  //Clear white
  white.clear();
  
  //Standarized order of the SMPTE/EBU color bar image : https://en.wikipedia.org/wiki/SMPTE_color_bars
  //from left to right, the colors are white, yellow, cyan, green, magenta, red,  blue and black
  for( int i = 0 ; i <= WIDTH ; i++ ) {
    colour c = ( i * 8 ) / WIDTH;
    display.line( i , c );
  }

  //Grid resolution
  for( int i = 0 ; i < BWIDTH/2 ; i++ ) {
    if( 1 - i%2 ) display.line( i , WHITE );
  }
}

void testScreenRGBW() {
  //Clear screen
  display.clear();
  //Fill white
  white.fill();
  
  //Standarized order of the SMPTE/EBU color bar image : https://en.wikipedia.org/wiki/SMPTE_color_bars
  //from left to right, the colors are white, yellow, cyan, green, magenta, red,  blue and black
  for( int i = 0 ; i <= WIDTH ; i++ ) {
    colour c = ( i * 8 ) / WIDTH;
    display.line( i , c );
  }

  //Grid resolution
  for( int i = 0 ; i < BWIDTH/2 ; i++ ) {
    if( 1 - i%2 ) display.line( i , WHITE );
  }
}

void (*screens[])() = { testScreenRGB , testScreenRGBW };

// Let's draw! //////////////////////////////////////////////////////////////////////////

void draw() {

  screens[screen]();
  
  //Serial diagnosis
  Serial.print("@frsd: ");
  Serial.print( rsd.getFrequency() , 10 );
  Serial.print(" , BWIDHT: ");
  Serial.print(BWIDTH);
  Serial.print(" , tick: ");
  Serial.print( rsd.getTick() );
  Serial.print(" , fine: ");
  Serial.println( rsd.getFine() );

  //Update Km
  updateKm();

  //Shift phase: Kilomux way
  rsd.shiftPhase( map( pot[3] , 0 , 1023 , -1 , +2 ) );
  
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

    case 1 : //Bloq button
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

     default: break;
     }
    
    buttonLastState[i]= buttonState[i];
  }

  
  //Update leds and states
  for( int i = 0 ; i < 4 ; i++ ) {
    if ( i == 0  ) {
      if ( buttonPushCounter[i]&1 ) {
        led[i] = HIGH;
        if ( !i ) bloq = true; //Que hice? con el if( !i )
      } else {
        led[i] = LOW;
        if( !i ) bloq = false;
      }
    }

    if ( i == 1 ) {
      if ( buttonPushCounter[i]&1 ) {
        led[i] = HIGH;
        play = true;
      } else {
        led[i] = LOW;
        play = false;
      }
    }
    
    KmShield.digitalWriteKm( i + 8 , led[i] );
  }
}
