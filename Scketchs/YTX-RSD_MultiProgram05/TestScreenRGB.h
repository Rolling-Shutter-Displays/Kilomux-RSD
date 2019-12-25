struct TestScreenRGB : Program {
  bool channelActive[4];
  char bars[4];
  const unsigned int p[9] =  { 0 , WIDTH/8 , WIDTH*2/8 , WIDTH*3/8 , WIDTH*4/8 , WIDTH*5/8 , WIDTH*6/8 , WIDTH*7/8 , WIDTH };
  
  void setup() {
    potValue[0] = 0xCC;
    potValue[1] = 0xF0;
    potValue[2] = 0xAA;
    potValue[3] = 0x0F;
    
    buttonPushCounter[0] = 1;
    buttonPushCounter[1] = 1;
    buttonPushCounter[2] = 1;
  }
  
  void draw() {
    if( !paused ) {
    //Clear screen
    display.clear();
    //Clear white
    white.clear();

    for( int i = 0 ; i < 4 ; i++ ) {
      bars[i] = potValue[i]>>2 ;
    }

    for( int i = 0 ; i < 8 ; i++ ) {
      for( int j = 0 ; j < 4 ; j++ ) {
        if( ( bars[j] & ( 1 << i ) ) && channelActive[j] ) {
          ch[j]->fill( p[i] + 1 , p[i+1] );
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
    //Reset states
    for( int i = 0 ; i < 4 ; i++ ) {
      potState[i] = false;
    }
    
    paused = false;
  }

} testScreenRGB;
