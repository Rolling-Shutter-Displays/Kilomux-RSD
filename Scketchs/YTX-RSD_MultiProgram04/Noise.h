
const PROGMEM uint8_t oneBitsSet[8]    = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
const PROGMEM uint8_t twoBitsSet[28]   = { 0x03, 0x05, 0x06, 0x09, 0x0A, 0x0C, 0x11, 0x12, 0x14, 0x18, 0x21, 0x22, 0x24, 0x28, 0x30, 0x41, 0x42, 0x44, 0x48, 0x50, 0x60, 0x81, 0x82, 0x84, 0x88, 0x90, 0xA0, 0xC0 };
const PROGMEM uint8_t threeBitsSet[56] = { 0x07, 0x0B, 0x0D, 0x0E, 0x13, 0x15, 0x16, 0x19, 0x1A, 0x1C, 0x23, 0x25, 0x26, 0x29, 0x2A, 0x2C, 0x31, 0x32, 0x34, 0x38, 0x43, 0x45, 0x46, 0x49, 0x4A, 0x4C, 0x51, 0x52, 0x54, 0x58, 0x61, 0x62, 0x64, 0x68, 0x70, 0x83, 0x85, 0x86, 0x89, 0x8A, 0x8C, 0x91, 0x92, 0x94, 0x98, 0xA1, 0xA2, 0xA4, 0xA8, 0xB0, 0xC1, 0xC2, 0xC4, 0xC8, 0xD0, 0xE0 };
const PROGMEM uint8_t fourBitsSet[70]  = { 0x0F, 0x17, 0x1B, 0x1D, 0x1E, 0x27, 0x2B, 0x2D, 0x2E, 0x33, 0x35, 0x36, 0x39, 0x3A, 0x3C, 0x47, 0x4B, 0x4D, 0x4E, 0x53, 0x55, 0x56, 0x59, 0x5A, 0x5C, 0x63, 0x65, 0x66, 0x69, 0x6A, 0x6C, 0x71, 0x72, 0x74, 0x78, 0x87, 0x8B, 0x8D, 0x8E, 0x93, 0x95, 0x96, 0x99, 0x9A, 0x9C, 0xA3, 0xA5, 0xA6, 0xA9, 0xAA, 0xAC, 0xB1, 0xB2, 0xB4, 0xB8, 0xC3, 0xC5, 0xC6, 0xC9, 0xCA, 0xCC, 0xD1, 0xD2, 0xD4, 0xD8, 0xE1, 0xE2, 0xE4, 0xE8, 0xF0 };

struct Noise : Program {
  //Params
  uint8_t level[4];
  
  void draw() {
    if( !paused ) {
    //Clear screen
    display.clear();
    //Clear white
    white.clear();
    
    for( int j = 0 ; j < 4 ; j++ ) {
      
      for( int i = 0 ; i < BWIDTH ; i++ ) {
        
        switch( level[j] ) {
          case 0:
            *(ch[j]->get() + i) = 0x00;
            break;
          case 1:
            *(ch[j]->get() + i) = pgm_read_byte( oneBitsSet + random( 0 , 9 ) );
            break;
          case 2:
            *(ch[j]->get() + i) = pgm_read_byte( twoBitsSet + random( 0 , 29 ) );
            break;
          case 3:
            *(ch[j]->get() + i) = pgm_read_byte( threeBitsSet + random( 0 , 57 ) );
            break;
          case 4:
            *(ch[j]->get() + i) = pgm_read_byte( fourBitsSet + random( 0 , 71 ) );
            break;
          case 5:
            *(ch[j]->get() + i) =  ( ~pgm_read_byte( threeBitsSet + random( 0 , 57 ) ) ) & 0xFF ;
            break;
          case 6:
            *(ch[j]->get() + i) = ( ~pgm_read_byte( twoBitsSet + random( 0 , 29 ) ) ) & 0xFF ;
            break;
          case 7:
            *(ch[j]->get() + i) = ( ~pgm_read_byte( oneBitsSet + random( 0 , 9 ) ) ) & 0xFF;
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

  void updateState() {
    
    for( int i = 0 ; i < 4 ; i++ ) {
      
      //Push counter
      if ( ( buttonState[i] == LOW ) && ( buttonLastState[i] == HIGH ) ) {
        buttonPushCounter[i]++;
      }
      
      //Click detection: update flags and LEDs
      if ( buttonPushCounter[i]&1 ) {
          led[i] = HIGH;
          //channelActive[i] = true;
        } else {
          led[i] = LOW;
          //channelActive[i] = false;
        }
      
      //Update button states
      buttonLastState[i]= buttonState[i];
      //
      
      //Potentiometer pick-up
      if ( !potState[i] ) {
        
        if( ( pot[i] < 10 ) && ( potValue[i] < 10 ) ) {
          potState[i] = true;
        }
        
        if ( ( pot[i] > 10 ) && ( pot[i] < ( 1023 - 10 ) ) ) {
          if ( ( potValue[i] > ( pot[i] - 10 )) && ( potValue[i] < ( pot[i] + 10 ) ) ){
            potState[i] = true;   
          }
        }

        if( ( pot[i] > ( 1023 - 10 ) ) && ( potValue[i] > ( 1023 - 10 ) ) ) {
          potState[i] = true;
        }
                 
      }
      
      if( potState[i] ) potValue[i] = pot[i];

      level[i] = (uint8_t)map( potValue[i] , 0 , 1023 , 0 , 8 );
    }
  }
  
  void pause() {
    paused = true;
  }

  void play() {
    paused = false;
  }

  void reset() {
    display.clear();
    white.clear();
    
    for( int i = 0 ; i < 4 ; i++ ) {
      potState[i] = false;
    }

    paused = false;
    
  }
} noise;
