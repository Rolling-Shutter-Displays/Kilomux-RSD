struct Paint : Program {
  
  void draw() {
    //Copy background
    red.copy( &red );
    green.copy( &green );
    blue.copy( &blue );
    
    white.clear();

    if ( !paused ) {  
      if ( (frameCount%15) < 7 )  white.line( pot[4]>>2 );
    }
  }
  
  void updateState() {
    //RGB Buttons
    for( int i = 0 ; i < 3 ; i++ ) {
      if( !buttonState[i] ) {
        led[i] = HIGH;
        if( buttonLastState[i] ) { 
          ch[i]->line( pot[0]>>2 ); // rising edge
        } else {
          ch[i]->fill( potValue[0]>>2 , pot[0]>>2 ); // high state
        }
      } else {
        led[i] = LOW;
      }
    }
    // Clear button = White button
    if( !buttonState[3] ) {
      led[3] = HIGH;
      if( buttonLastState[3] ) { 
        display.clearSafe( pot[0]>>2 ); // rising edge
      } else {
        display.clearSafe( potValue[0]>>2 , pot[0]>>2 ); // high state
      }
    } else {
      led[3] = LOW;
    }
      
    //Save lectures
    for( int i = 0 ; i < 4 ; i++ ) {
      potValue[i] = pot[i];
      buttonLastState[i]= buttonState[i];
    }
  }

  void reset() {
    //Transitions?
    /*
    //Clear background
    display.clear();
    white.clear();
    */
    /*
    //Copy background
    red.copy( &red );
    green.copy( &green );
    blue.copy( &blue );
    white.copy( &white );
    */
    //Reset states
    for( int i = 0 ; i < 4 ; i++ ) {
      potState[i] = false;
    }
    
    paused = false;
  }
  
} paint;
