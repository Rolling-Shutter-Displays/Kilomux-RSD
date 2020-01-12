#include "3DQuad.h"

struct Player : Program {
  
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
      
      clearBackground();

      for( int i = 0 ; i < BWIDTH ; i++ ) {
        *( ch[0]->get() + i ) = pgm_read_byte( &D3Quad_r[frameCount%60][i]);
        *( ch[3]->get() + i ) = pgm_read_byte( &D3Quad_b[frameCount%60][i]);
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

} player;
