struct MirrorShift : Program {
  
  bool channelActive[4];

  void setup() {
    potValue[0] = 512;
    potValue[1] = 512;
    potValue[2] = 512;
    potValue[3] = 512;
    
    buttonPushCounter[0] = true;
    buttonPushCounter[2] = true;
  }
  
  void draw() {
    //Copy background
    red.copy( &red );
    green.copy( &green );
    blue.copy( &blue );
    white.copy( &white );

    if ( !paused ) {
    
    for( int i = 0 ; i < 4 ; i++ ) {
      
      ShiftByOne( WIDTH/2 + 3 , WIDTH - 2 , ch[i] );
      ShiftByOne( WIDTH/2 - 2 ,         2 , ch[i] );
    
      if ( channelActive[i] ) { 
        if ( !(int)random( potValue[i]>>7 ) ) {
          ch[i]->line( WIDTH/2 - 2 );
          ch[i]->line( WIDTH/2 + 3 );
        }
      }
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
          channelActive[i] = true;
        } else {
          led[i] = LOW;
          channelActive[i] = false;
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
      //
    }
  }

  void reset() {
    display.clear( WIDTH/2 - 1 , WIDTH/2 + 2 );
    display.clear( 0 , 1 );
    display.clear( WIDTH - 1 , WIDTH );

    white.clear( WIDTH/2 - 1 , WIDTH/2 + 2 );
    white.clear( 0 , 1 );
    white.clear( WIDTH - 1 , WIDTH );
    
    for( int i = 0 ; i < 4 ; i++ ) {
      potState[i] = false;
    }
    
    paused = false;  
  }
  
} mirrorShift;
