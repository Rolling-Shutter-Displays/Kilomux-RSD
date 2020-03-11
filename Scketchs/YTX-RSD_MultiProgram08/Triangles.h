struct Triangles : Program {
  
  bool channelActive[4];
  
  int speed[4];
  
  void setup() {
    potValue[0] = 512;
    potValue[1] = 0;
    potValue[2] = 0;
    potValue[3] = 0;
    
    buttonPushCounter[0] = 1;
    
    reset();
  }
  
  void draw() {
    if( !paused ) {
      
      copyBackground();

      for( int i = 0 ; i < 4 ; i++ ) {
        speed[i] = map( potValue[i] , 0 , 1023 , -2 , + 2.9 );
        
        Serial.println(speed[i]);

        if( speed[i] > 0 ) {
          do {
            RollOver( 0 , WIDTH , ch[i] );
            speed[i]--;
          } while( speed[i] );
        } else if( speed[i] < 0 ) {
          do {
            RollOver( WIDTH , 0 , ch[i] );
            speed[i]++;
          } while( speed[i] );
        }
      }
    
    } else { //if paused
      
      clearBackground();
     
      display.line( 0 , RED );
      display.line( WIDTH , BLUE );
      
      for( int i = 0 ; i < 4 ; i++ ) {
        if ( channelActive[i] ) triangle( 0  , WIDTH/2 , map( potValue[i] , 0 , 1023 , 0 , 50 ) , ch[i] );
        if ( channelActive[i] ) triangle( WIDTH , WIDTH/2 , map( potValue[i] , 0 , 1023 , 0 , 50 ) , ch[i] );
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
      potState[i] = true;
    }
    
    paused = false;
  }

  void triangle( int begin , int end , int steps , Channel *ch ) {

  int interval = ( end - begin ) / steps;
  interval = abs( interval ) + 1;

  if ( begin < end ) {

    if ( begin < 0 ) return;

    for ( int i = 0 ; i < steps ; i++ ) {
      fillSafe( begin , end , begin + interval * i + frameCount%interval , begin + interval * i + map( i , 0 , steps, 0 , interval ) + frameCount%interval , ch );
    }

  } else if( begin > end ) {
    
    if( end > WIDTH ) return;
    
    for ( int i = 0 ; i < steps ; i++ ) {
      fillSafe( end , begin , begin - interval * i - frameCount%interval , begin - interval * i - map( i , 0 , steps, 0 , interval ) - frameCount%interval , ch );
    }
  
  } else {
    
    ch->lineSafe( begin );
  
  }
  
  }

} triangles;
