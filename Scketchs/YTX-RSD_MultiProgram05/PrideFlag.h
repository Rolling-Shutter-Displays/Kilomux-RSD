#define ORANGE 8

colour palette[8] =  { BLACK ,   RED ,    ORANGE ,      YELLOW ,     GREEN ,     BLUE ,     MAGENTA  ,   BLACK    };

const int p[9] =  { 0 , WIDTH/8 , WIDTH*2/8 , WIDTH*3/8 , WIDTH*4/8 , WIDTH*5/8 , WIDTH*6/8 , WIDTH*7/8 , WIDTH };
int diff[8];
int partition[9];

void prideFlag() {
  
  if( !pause ) {
  
  display.clear(); 
  white.clear();

  for( int i = 0 ; i < 8 ; i++ ) {
    diff[i] = ( WIDTH/16 ) * sin(( TWO_PI * ( frameCount%120 ) / 120 ) + TWO_PI*i/16 );
  }

  partition[0] = p[0];
  for( int i = 0 ; i < 8 ; i++ ) {
    partition[ i+1 ] = p[i+1] + diff[i];
  }
  partition[8] = p[8];

  for( int i = 0 ; i < 8 ; i++ ) {
    if( palette[i]==ORANGE ) {
      display.fill( partition[i] , partition[i+1] , RED );
      dither( partition[i] , partition[i+1] , green );
    } else {
      display.fill( partition[i] , partition[i+1] , palette[i] );
    }
  }
  
  } else {
    for ( int i = 0 ; i < 4 ; i++ ) {
      ch[i]->copy( ch[i] );
    }
  }
}
