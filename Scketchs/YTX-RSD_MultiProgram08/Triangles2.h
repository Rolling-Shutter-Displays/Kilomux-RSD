struct Triangles2 : Program {
  
  bool channelActive[4];

  char buffer[BWIDTH];
  
  int pos[4];
  int steps[4];
  
  void setup() {
    steps[0] = 16;
    steps[3] = 14;
    
    buttonPushCounter[0] = 1;
    buttonPushCounter[3] = 1;
    
    reset();
  }
  
  void draw() {
    if( !paused ) {
      clearBackground();

      for( int i = 0 ; i < 4 ; i++ ) {
        
        pos[i] = map( potValue[i] , 0 , 1023 , 0 , WIDTH );

        if ( channelActive[i] ) triangle( 0  , WIDTH/2 , steps[i] , ch[i] );
        if ( channelActive[i] ) triangle( WIDTH , WIDTH/2 , steps[i] , ch[i] );

        copyBuffer( ch[i]->get() , buffer );
        ch[i]->clear();
        copyBuffer( buffer , ch[i]->get() , pos[i] );
        
      }
    
    } else { //if paused
      
      clearBackground();
      
      for( int i = 0 ; i < 4 ; i++ ) {
        steps[i] = map( potValue[i] , 0 , 1023 , 0 , 40 );
        
        if ( channelActive[i] ) triangle( 0  , WIDTH/2 , steps[i] , ch[i] );
        if ( channelActive[i] ) triangle( WIDTH , WIDTH/2 , steps[i] , ch[i] );

        copyBuffer( ch[i]->get() , buffer );
        ch[i]->clear();
        copyBuffer( buffer , ch[i]->get() , pos[i] );
      
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
  int diff;
  
  if ( !paused ) {
    diff = frameCount%interval;
  } else {
    diff = 0;
  }

  if ( begin < end ) {

    if ( begin < 0 ) return;

    for ( int i = 0 ; i < steps ; i++ ) {
      fillSafe( begin , end , begin + interval * i + diff , begin + interval * i + map( i , 0 , steps, 0 , interval ) + diff , ch );
    }

  } else if( begin > end ) {
    
    if( end > WIDTH ) return;
    
    for ( int i = 0 ; i < steps ; i++ ) {
      fillSafe( end , begin , begin - interval * i - diff , begin - interval * i - map( i , 0 , steps, 0 , interval ) - diff , ch );
    }
  
  } else {
    
    ch->lineSafe( begin );
  
  }
  
  }

} triangles2;
