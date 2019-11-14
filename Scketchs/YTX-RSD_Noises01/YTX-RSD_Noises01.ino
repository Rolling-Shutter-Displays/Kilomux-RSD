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
 * ║    Bloq.     Prev     Next                                      ║
 * ║    Freq.     Button   Button   x                                ║
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

Channel *ch[4] = { &red , &green , &blue , &white  };

// Global variables  //////////////////////////////////////////////////////////////////////

unsigned int pot[8];
unsigned int prevPot[8];

bool buttonState[8];
bool buttonLastState[8];
char buttonPushCounter[8];   // counter for the number of button presses

bool led[8];
bool bloq = false;

int screen = 3;
const int screen_size = 4;

uint8_t oneBitsSet[8]    = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
uint8_t twoBitsSet[28]   = { 0x03, 0x05, 0x06, 0x09, 0x0A, 0x0C, 0x11, 0x12, 0x14, 0x18, 0x21, 0x22, 0x24, 0x28, 0x30, 0x41, 0x42, 0x44, 0x48, 0x50, 0x60, 0x81, 0x82, 0x84, 0x88, 0x90, 0xA0, 0xC0 };
uint8_t threeBitsSet[56] = { 0x07, 0x0B, 0x0D, 0x0E, 0x13, 0x15, 0x16, 0x19, 0x1A, 0x1C, 0x23, 0x25, 0x26, 0x29, 0x2A, 0x2C, 0x31, 0x32, 0x34, 0x38, 0x43, 0x45, 0x46, 0x49, 0x4A, 0x4C, 0x51, 0x52, 0x54, 0x58, 0x61, 0x62, 0x64, 0x68, 0x70, 0x83, 0x85, 0x86, 0x89, 0x8A, 0x8C, 0x91, 0x92, 0x94, 0x98, 0xA1, 0xA2, 0xA4, 0xA8, 0xB0, 0xC1, 0xC2, 0xC4, 0xC8, 0xD0, 0xE0 };
uint8_t fourBitsSet[70]  = { 0x0F, 0x17, 0x1B, 0x1D, 0x1E, 0x27, 0x2B, 0x2D, 0x2E, 0x33, 0x35, 0x36, 0x39, 0x3A, 0x3C, 0x47, 0x4B, 0x4D, 0x4E, 0x53, 0x55, 0x56, 0x59, 0x5A, 0x5C, 0x63, 0x65, 0x66, 0x69, 0x6A, 0x6C, 0x71, 0x72, 0x74, 0x78, 0x87, 0x8B, 0x8D, 0x8E, 0x93, 0x95, 0x96, 0x99, 0x9A, 0x9C, 0xA3, 0xA5, 0xA6, 0xA9, 0xAA, 0xAC, 0xB1, 0xB2, 0xB4, 0xB8, 0xC3, 0xC5, 0xC6, 0xC9, 0xCA, 0xCC, 0xD1, 0xD2, 0xD4, 0xD8, 0xE1, 0xE2, 0xE4, 0xE8, 0xF0 };

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
  Serial.begin( 115200 );
  
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

void whiteNoise() {
  //Clear screen
  display.clear();
  //Clear white
  white.clear();
  
  //Standarized order of the SMPTE/EBU color bar image : https://en.wikipedia.org/wiki/SMPTE_color_bars
  //from left to right, the colors are white, yellow, cyan, green, magenta, red,  blue and black
  for( int i = 0 ; i <= WIDTH ; i++ ) {
    if ( random( 0 , 2 ) ) white.line( i );
  }

}

void whiteNoise2() {
  //Clear screen
  display.clear();
  //Clear white
  white.clear();
  
  for( int i = 0 ; i <= BWIDTH ; i++ ) {
    *(white.get() + i) = fourBitsSet[random(0,71)];
    //*(white.get() + i) = fourBitsSet[i];
  }
  /*
  for( int i = 0 ; i < 70 ; i++ ) {
    *(white.get()+1) = fourBitsSet[i];
  }
  */

}

void whiteNoise3() {
  //Clear screen
  display.clear();
  //Clear white
  white.clear();
  
  for( int i = 0 ; i <= BWIDTH ; i++ ) {
    uint8_t l = map( pot[4] , 0 , 1023 , 0 , 8 );
    
    switch( l ) {
      case 0:
        *(white.get() + i) = 0x00;
        break;
      case 1:
        *(white.get() + i) = oneBitsSet[ random( 0 , 9 ) ];
        break;
      case 2:
        *(white.get() + i) = twoBitsSet[ random( 0 , 29 ) ];
        break;
      case 3:
        *(white.get() + i) = threeBitsSet[ random( 0 , 57 ) ];
        break;
      case 4:
        *(white.get() + i) = fourBitsSet[ random( 0 , 71 ) ];
        break;
      case 5:
        *(white.get() + i) = ( ~threeBitsSet[ random( 0 , 57 ) ] ) & 0xFF ;
        break;
      case 6:
        *(white.get() + i) = ( ~twoBitsSet[ random( 0 , 29 ) ] ) & 0xFF ;
        break;
      case 7:
        *(white.get() + i) = ( ~oneBitsSet[ random( 0 , 9 ) ] ) & 0xFF;
        break;
      case 8:
        *(white.get() + i) = 0xFF;
        break;
      default: break;   
    }
  }
  white.fill( 0 , 7 );
  white.clear( WIDTH - 7 , WIDTH );

}

void whiteNoise4() {
  //Clear screen
  display.clear();
  //Clear white
  white.clear();

  //Params
  uint8_t level[4];
  for ( int i = 0 ; i < 4 ; i++ ) {
    level[i] = map( pot[4 + i] , 0 , 1023 , 0 , 8 );
  }

  for( int j = 0 ; j < 4 ; j++ ) {
    
    for( int i = 0 ; i <= BWIDTH ; i++ ) {
    
        switch( level[j] ) {
          case 0:
            *(ch[j]->get() + i) = 0x00;
            break;
          case 1:
            *(ch[j]->get() + i) = oneBitsSet[ random( 0 , 9 ) ];
            break;
          case 2:
            *(ch[j]->get() + i) = twoBitsSet[ random( 0 , 29 ) ];
            break;
          case 3:
            *(ch[j]->get() + i) = threeBitsSet[ random( 0 , 57 ) ];
            break;
          case 4:
            *(ch[j]->get() + i) = fourBitsSet[ random( 0 , 71 ) ];
            break;
          case 5:
            *(ch[j]->get() + i) = ( ~threeBitsSet[ random( 0 , 57 ) ] ) & 0xFF ;
            break;
          case 6:
            *(ch[j]->get() + i) = ( ~twoBitsSet[ random( 0 , 29 ) ] ) & 0xFF ;
            break;
          case 7:
            *(ch[j]->get() + i) = ( ~oneBitsSet[ random( 0 , 9 ) ] ) & 0xFF;
            break;
          case 8:
            *(ch[j]->get() + i) = 0xFF;
            break;
          default: break;   
        }
      }
    }

  white.fill( 0 , 7 );
  white.clear( WIDTH - 7 , WIDTH );
  
}

void (*screens[])() = { testScreenRGB , whiteNoise , whiteNoise2 , whiteNoise3 , whiteNoise4 };

// Let's draw! //////////////////////////////////////////////////////////////////////////

void draw() {

  screens[screen]();
  /*
  //Serial diagnosis
  Serial.print("@frsd: ");
  Serial.print( rsd.getFrequency() , 10 );
  Serial.print(" , BWIDHT: ");
  Serial.print(BWIDTH);
  Serial.print(" , tick: ");
  Serial.print( rsd.getTick() );
  Serial.print(" , fine: ");
  Serial.println( rsd.getFine() );
  */
  
  Serial.println( frameLost );
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

    case 1: //prev Button
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

    case 2: //next Button
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

     default: break;
    }
    
    buttonLastState[i]= buttonState[i];
  }

  
  //Update leds and states
  for( int i = 0 ; i < 4 ; i++ ) {
    if ( i == 0 ) {
      if ( buttonPushCounter[i]&1 ) {
        led[i] = HIGH;
        if ( !i ) bloq = true;
      } else {
        led[i] = LOW;
        if( !i ) bloq = false;
      }
    }
    
    KmShield.digitalWriteKm( i + 8 , led[i] );
  }
}
