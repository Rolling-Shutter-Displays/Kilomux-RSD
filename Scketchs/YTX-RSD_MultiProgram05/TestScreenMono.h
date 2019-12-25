struct TestScreenMono : Program {
  bool channelActive[4];

  void setup() {
    buttonPushCounter[1] = 1;
  }
  
  void draw() {
    display.clear();
    white.clear();
  
    for( int channel = 0 ; channel < 4 ; channel++ ) { 
      if( channelActive[ channel ] ) {
        
        ch[ channel ]->fill( 0 , WIDTH/4 );

        for( int i = 1 ; i < (WIDTH/4) ; i++ ) {
          int val = (WIDTH/4)/i;
          if ( i%2 ) {
            ch[ channel ]->clear( WIDTH/2 -val, WIDTH/2 + val);
          } else {
            ch[ channel ]->fill( WIDTH/2 - val, WIDTH/2 + val);
          }
        }

        for( int i = 0 ; i < WIDTH/4 ; i++ ) {
          if( i%2 ) ch[ channel ]->line( WIDTH - i );
        }
      }
    }
    
  }
  
  void updateState() {
    for( int i = 0 ; i < 4 ; i++ ) {
      //Click detection
      if ( ( buttonState[i] == LOW ) && ( buttonLastState[i] == HIGH ) ) {
        buttonPushCounter[i]++;
      }
    
      if ( buttonPushCounter[i]&1 ) {
          led[i] = HIGH;
          channelActive[i] = true;
        } else {
          led[i] = LOW;
          channelActive[i] = false;
        }

      //Save lectures
      potValue[i] = pot[i];
      buttonLastState[i]= buttonState[i];
    }
    
  }

  void reset() {
    display.clear();
    white.clear();
  }
  
} testScreenMono;
