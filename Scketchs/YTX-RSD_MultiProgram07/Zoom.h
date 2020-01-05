struct Zoom : Program {
  
  bool channelActive[4];
  
  void setup() {
    potValue[0] = 0;
    potValue[1] = 0;
    potValue[2] = 0;
    potValue[3] = 0;
  }
  
  void draw() {
    if( !paused ) {
      clearBackground();
      
      int d = ( potValue[0]>>2 ) + 1;
      
      int pos = WIDTH/2 + d/2;
      ch[0]->lineSafe( pos );
      do {   
        pos = pos + d;
      } while ( ch[0]->lineSafe( pos ) );
      
      pos = WIDTH/2 - d/2;
      ch[0]->lineSafe( pos );
      do {   
        pos = pos - d;
      } while ( ch[0]->lineSafe( pos ) );

      if( channelActive[3] ) ch[3]->fill();
     
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

} zoom;
