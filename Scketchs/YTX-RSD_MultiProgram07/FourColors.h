struct FourColors : Program {
  
  bool channelActive[4];
  
  colour c[4];
  const unsigned int p[5] =  { 0 , WIDTH/4 , WIDTH*2/4 , WIDTH*3/4 , WIDTH };
  
  void setup() {
    potValue[0] = 256;
    potValue[1] = 512;
    potValue[2] = 768;
    potValue[3] = 0;
    
  }
  
  void draw() {
    if( !paused ) {
      clearBackground();
    
      //Set colors
      for( int i = 0 ; i < 4 ; i++ ) {
        c[i] = potValue[i]>>7 ;
      }
      //Draw bars
      for( int i = 0 ; i < 4 ; i++ ) {
        display.fill( p[i] + 1 , p[i+1] , c[i] );
      }
      //Dithers
      for( int i = 0 ; i < 4 ; i++ ) {
        dither2( 0 , WIDTH , buttonPushCounter[i]%3 , ch[i] );
      }
     
    } else { //if paused
      copyBackground();
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

} fourColors;
