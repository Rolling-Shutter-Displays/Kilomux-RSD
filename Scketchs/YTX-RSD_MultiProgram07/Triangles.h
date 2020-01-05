struct Triangles : Program {
  
  bool channelActive[4];
  char steps[4];
  
  void setup() {
    potValue[0] = 0;
    potValue[1] = 0;
    potValue[2] = 0;
    potValue[3] = 0;
  }
  
  void draw() {
    if( !paused ) {
     clearBackground();
     for( int i = 0 ; i < 4 ; i++ ) {
      potState[i] = false;
      if ( channelActive[i] ) triangle( 0 , WIDTH , map( potValue[i] , 0 , 1023 , 0 , 50 ) , ch[i] );
     }
    } else { //if paused
      copyBackground();
      for( int i = 0 ; i < 4 ; i++ ) {
        potState[i] = true;
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

} triangles;
