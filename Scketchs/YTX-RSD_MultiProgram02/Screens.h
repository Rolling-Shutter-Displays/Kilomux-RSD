// Utils ///////////////////////////////////////////////////////////////////////////////
 
void dither( int x0 , int x1 , Channel ch ) {
  if ( x1 > x0 ) {  
    do {
      if ( x1 % 2 ) ch.line( x1 );
      x1--;
    } while( x1 > x0 ); 
    
    if ( x1 % 2 ) ch.line( x0 ) ;
  }
    /*
  } else if( x1 == x0 ) {
            
            line( x1 );
            
        } else {
            
            do {
                line( x0 );
                x0--;
            } while( x0 > x1 );
            
            line( x1 );
        }
}
*/
}

void ShiftByOne( int begin , int end , Channel *ch) {
  
  if( begin > end ) { //Shift Right
    int i = end;
    do {
      ( ch->get( i + 1 ) ) ? ch->line( i ) : ch->clear( i ) ;
      i++;
    } while ( i < begin );
    ch->clear( begin );
  }

  if( begin < end ) { //Shift Left
    int i = end;
    do {
      ( ch->get( i - 1 ) ) ? ch->line( i ) : ch->clear( i ) ;
      i--;
    } while ( i > begin );
    ch->clear( begin );
  }
  
}

// Screens //////////////////////////////////////////////////////////////////////////////


struct Program {
  virtual void draw() {}
  virtual void updateState() {}
};

struct TestScreenMonoS : Program {
  void draw() {
    display.clear();
    white.clear();
    red.fill( 0 , WIDTH/2 );
  }
  void updateState() {
    led[4] = HIGH;
  }
} testScreenMonoS;

struct TestScreenMonoT : Program {
  void draw() {
    display.clear();
    white.clear();
    blue.fill( 0 , WIDTH/2 );
  }
  void updateState() {
    led[4] = LOW;
  }
} testScreenMonoT;

colour palette2[3] =  { BLACK , BLUE , MAGENTA };

struct MirrorShift : Program {
  //unsigned int param[4];
  bool channelActive[4];
  
  void draw() {
    //Copy background
    red.copy( &red );
    green.copy( &green );
    blue.copy( &blue );
    white.copy( &white );

    
    for( int i = 0 ; i < 4 ; i++ ) {
      
      ShiftByOne( 130 , 255 , ch[i] );
      ShiftByOne( 125 , 0 , ch[i] );
    
      if ( channelActive[i] ) {
        if ( !(int)random(3) ) {
          ch[i]->line( 125 );
          ch[i]->line( 130 );
        }
      }
    }
    
  }
  
  void updateState() {
    for( int i = 4 ; i < 8 ; i++ ) {
      if ( buttonPushCounter[i]&1 ) {
          led[i] = HIGH;
          channelActive[i-4] = true;
        } else {
          led[i] = LOW;
          channelActive[i-4] = false;
        }
    }
  }
  
} mirrorShift;


Program* programs[3] = { &testScreenMonoS , &testScreenMonoT , &mirrorShift };
const int programs_size = 2;

void testScreenMono() {
  display.clear();
  white.clear();
  
  int channel = pot[0] >> 8 ;
  
  ch[channel]->clear();
  
  ch[channel]->fill( 0 , WIDTH/4 );

  
  for( int i = 1 ; i < (WIDTH/4) ; i++ ) {
    int val = (WIDTH/4)/i;
    if ( i%2 ) {
      ch[channel]->clear( WIDTH/2 -val, WIDTH/2 + val);
    } else {
      ch[channel]->fill( WIDTH/2 - val, WIDTH/2 + val);
    }
  }

  for( int i = 0 ; i < WIDTH/4 ; i++ ) {
    if( i%2 ) ch[channel]->line( WIDTH - i );
  }
}

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

uint8_t oneBitsSet[8]    = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
uint8_t twoBitsSet[28]   = { 0x03, 0x05, 0x06, 0x09, 0x0A, 0x0C, 0x11, 0x12, 0x14, 0x18, 0x21, 0x22, 0x24, 0x28, 0x30, 0x41, 0x42, 0x44, 0x48, 0x50, 0x60, 0x81, 0x82, 0x84, 0x88, 0x90, 0xA0, 0xC0 };
uint8_t threeBitsSet[56] = { 0x07, 0x0B, 0x0D, 0x0E, 0x13, 0x15, 0x16, 0x19, 0x1A, 0x1C, 0x23, 0x25, 0x26, 0x29, 0x2A, 0x2C, 0x31, 0x32, 0x34, 0x38, 0x43, 0x45, 0x46, 0x49, 0x4A, 0x4C, 0x51, 0x52, 0x54, 0x58, 0x61, 0x62, 0x64, 0x68, 0x70, 0x83, 0x85, 0x86, 0x89, 0x8A, 0x8C, 0x91, 0x92, 0x94, 0x98, 0xA1, 0xA2, 0xA4, 0xA8, 0xB0, 0xC1, 0xC2, 0xC4, 0xC8, 0xD0, 0xE0 };
uint8_t fourBitsSet[70]  = { 0x0F, 0x17, 0x1B, 0x1D, 0x1E, 0x27, 0x2B, 0x2D, 0x2E, 0x33, 0x35, 0x36, 0x39, 0x3A, 0x3C, 0x47, 0x4B, 0x4D, 0x4E, 0x53, 0x55, 0x56, 0x59, 0x5A, 0x5C, 0x63, 0x65, 0x66, 0x69, 0x6A, 0x6C, 0x71, 0x72, 0x74, 0x78, 0x87, 0x8B, 0x8D, 0x8E, 0x93, 0x95, 0x96, 0x99, 0x9A, 0x9C, 0xA3, 0xA5, 0xA6, 0xA9, 0xAA, 0xAC, 0xB1, 0xB2, 0xB4, 0xB8, 0xC3, 0xC5, 0xC6, 0xC9, 0xCA, 0xCC, 0xD1, 0xD2, 0xD4, 0xD8, 0xE1, 0xE2, 0xE4, 0xE8, 0xF0 };

void whiteNoise() {
  //Clear screen
  display.clear();
  //Clear white
  white.clear();

  //Params
  uint8_t level[4];
  for ( int i = 0 ; i < 4 ; i++ ) {
    level[i] = map( pot[4 + i] , 0 , 1023 , 0 , 8 );
  }

  if( !pause ) {
    
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
  
  } else {
    for ( int i = 0 ; i < 4 ; i++ ) {
      ch[i]->copy( ch[i] );
    }
  }
  
}

#define ORANGE 8

colour palette[8] =  { BLACK ,   RED ,    ORANGE ,      YELLOW ,     GREEN ,     BLUE ,     MAGENTA  ,   BLACK    };

const int p[9] =  { 0 , WIDTH/8 , WIDTH*2/8 , WIDTH*3/8 , WIDTH*4/8 , WIDTH*5/8 , WIDTH*6/8 , WIDTH*7/8 , WIDTH };
int diff[8];
int partition[9];

void prideFlag() {
  
  if( !pause ) {
  
  display.clear(); 
  white.clear();

  for( int i = 0 ; i < 8 ; i++ ) {
    diff[i] = ( WIDTH/16 ) * sin(( TWO_PI * ( frameCount%120 ) / 120 ) + TWO_PI*i/16 );
  }

  partition[0] = p[0];
  for( int i = 0 ; i < 8 ; i++ ) {
    partition[ i+1 ] = p[i+1] + diff[i];
  }
  partition[8] = p[8];

  for( int i = 0 ; i < 8 ; i++ ) {
    if( palette[i]==ORANGE ) {
      display.fill( partition[i] , partition[i+1] , RED );
      dither( partition[i] , partition[i+1] , green );
    } else {
      display.fill( partition[i] , partition[i+1] , palette[i] );
    }
  }
  
  } else {
    for ( int i = 0 ; i < 4 ; i++ ) {
      ch[i]->copy( ch[i] );
    }
  }
}

/*
boolean channelStateShifty[4];

void updateKmShifty() {
  
}

void shifty() {
  
  red.copy( &red );
  green.copy( &green );
  blue.copy( &blue );
  
  ShiftByOne( 130 , 255 );
  ShiftByOne( 115 , 0 );
  colour c = palette[(int)random(3)];
  display.line( 115 , c );
  display.line( 130 , c );
}
*/

void (*screens[])() = { testScreenMono , testScreenRGB , whiteNoise , prideFlag };

int screen = 0;
const int screen_size = 2;
