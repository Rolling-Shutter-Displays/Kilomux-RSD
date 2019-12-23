
void testScreenRGB() {
  //Clear screen
  display.clear();
  //Clear white
  white.clear();
  
  //Standarized order of the SMPTE/EBU color bar image : https://en.wikipedia.org/wiki/SMPTE_color_bars
  //from left to right, the colors are white, yellow, cyan, green, magenta, red,  blue and black
  for( int i = 0 ; i <= WIDTH ; i++ ) {
    colour c = ( i * 8 ) / WIDTH;
    display.line( i , c );
  }

  //Grid resolution
  for( int i = 0 ; i < BWIDTH/2 ; i++ ) {
    if( 1 - i%2 ) display.line( i , WHITE );
  }
}
