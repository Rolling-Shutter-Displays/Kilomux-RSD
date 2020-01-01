struct Twist : Program {
  
  bool channelActive[4];

  colour c[4] = { RED , BLACK , RED , BLACK };
  const unsigned int p[5] =  { 0 , WIDTH/4 , WIDTH*2/4 , WIDTH*3/4 , WIDTH };
  
  void setup() {
    potValue[0] = 1023*2/4;
    potValue[1] = 1023*3/4;
    potValue[2] = 0;
    potValue[3] = 0;
  }
  
  void draw() {
    copyBackground();
    
    if( !paused ) {
      /*
      int x0 = random( 0 , 127 );
      int x1 = random( 0 , 127 );
      /*
      while ( x1 > x0 ) {
        x1 = random( 0 , 127 );
      }
      */
      int x0 = potValue[0]>>2;
      int x1 = potValue[1]>>2;
      
      for( int i = 0 ; i < 4 ; i++ ) {
        int x0 = random( 0 ,  WIDTH );
        int x1 = random( 0 , WIDTH );
        RollOver( x0 , x1 , ch[i] );
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

    /*
    clearBackground();

    //Draw bars
    for( int i = 0 ; i < 4 ; i++ ) {
      display.fill( p[i] + 1 , p[i+1] , c[i] );
    }
    */
  }

} twist;
