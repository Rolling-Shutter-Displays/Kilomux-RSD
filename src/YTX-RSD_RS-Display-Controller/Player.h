#include "3DQuad.h"

struct Player : Program {
  
  bool light, direction;

  // Complementary colours
  colour palette[5][2] = { { RED , CYAN } , { GREEN , MAGENTA } , { BLUE , YELLOW } , { RED , MAGENTA } , { GREEN , YELLOW }};
  int paletteN = 0;
  int palette_size = 4;
  
  
  void setup() {
    potValue[0] = 0;
    potValue[1] = 0;
    potValue[2] = 0;
    potValue[3] = 0;
    
    reset();
  }
  
  void draw() {

    static int frame;
    
    if( !paused ) {
      clearBackground();
      
      colour c0, c1;
      c0 = palette[paletteN][0];
      c1 = palette[paletteN][1];
      
      for( int i = 0 ; i < BWIDTH ; i++ ) {

        if ( 0xCC & (1 << c0) ) *( ch[0]->get() + i ) |= pgm_read_byte( &D3Quad_r[frame][i]);
        if ( 0xF0 & (1 << c0) ) *( ch[1]->get() + i ) |= pgm_read_byte( &D3Quad_r[frame][i]);
        if ( 0xAA & (1 << c0) ) *( ch[2]->get() + i ) |= pgm_read_byte( &D3Quad_r[frame][i]);
        
        if ( 0xCC & (1 << c1) ) *( ch[0]->get() + i ) |= pgm_read_byte( &D3Quad_b[frame][i]);
        if ( 0xF0 & (1 << c1) ) *( ch[1]->get() + i ) |= pgm_read_byte( &D3Quad_b[frame][i]);
        if ( 0xAA & (1 << c1) ) *( ch[2]->get() + i ) |= pgm_read_byte( &D3Quad_b[frame][i]);
        
        if( light ) *( ch[3]->get() + i ) = pgm_read_byte( &D3Quad_w[frame][i]);
        
      }
      
      if( direction ) {
        ( frame < 59 ) ? frame++ : frame = 0;
      } else {
        ( frame > 0 ) ? frame-- : frame = 59;
      }
      
    } else { //if paused
      
      copyBackground();
      
    }
    
  }
  
  void updateState() {
    for( int i = 0 ; i < 4 ; i++ ) {
      
      //Click detection
      if ( ( buttonState[i] == LOW ) && ( buttonLastState[i] == HIGH ) ) {
        buttonPushCounter[i]++;
        switch( i ){
          case( 0 ): //Red button, palette >>
            if( paletteN < palette_size ) {
              paletteN++;
            } else {
              paletteN = 0;
            }
          break;
          case( 1 ): //Green button, palette <<
            if( paletteN > 0 ) {
              paletteN--;
            } else {
              paletteN = palette_size;
            }
          break;
          case( 2 ): //Blue button, direction
            direction = !direction;
            led[i] = !led[i];
          break;
          default: break;
        }
      }

      if( i == 3 ) {
      //Toggle detection
      if ( buttonPushCounter[i]&1 ) {
          led[i] = HIGH;
          light = true;
        } else {
          led[i] = LOW;
          light = false;
        }
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

} player;
