/*
BLACK 0
BLUE 1
RED 2
MAGENTA 3
GREEN 4
CYAN 5
YELLOW 6
WHITE 7
*/

//Extenden color mode, with white led
#define W 8
#define W_BLUE 9
#define W_RED 10
#define W_MAGENTA 11
#define W_GREEN 12
#define W_CYAN 13
#define W_YELLOW 14
#define W_WHITE 15

struct PaletteGenerator : Program {

  char palettes4[16][4] =
  {
  {  2 ,  5 , 13 , 10 }, //= { RED , CYAN , W_CYAN , W_RED }
  {  4 ,  3 , 11 , 12 }, //
  {  1 ,  6 , 14 ,  9 }, //
  {  3 , 11 , 13 ,  1 }, //= { MAGENTA , W_MAGENTA , W_CYAN , BLUE }
  {  2 ,  1 ,  9 , 10 }, //
  {  4 ,  2 , 12 , 14 },
  {  5 ,  3 ,  6 ,  0 },
  { 10 ,  5 , 12 , 14 },
  {  1 ,  5 , 12 , 14 },
  { 10 ,  5 , 12 , 14 },
  { 10 , 14 ,  6 , 14 },
  {  2 , 14 ,  6 , 14 },
  {  1 ,  2 ,  5 , 10 },
  {  0 ,  8 ,  7 , 15 },
  {  3 ,  8 ,  7 , 15 },
  { 11 ,  1 , 12 , 13 }
  };
  int palettes4_size = 15;
  int paletteN = palettes4_size;
  
  bool equ = false;
  bool generate = false;
  
  char palette[8] = { BLACK , BLUE , RED , MAGENTA , GREEN , CYAN , YELLOW , WHITE };
  
  void setup() {}
  
  void draw() {
    //Clear screen
    clearBackground();
    
    //Display
    for( int i = 0 ; i < 8 ; i++ ) {
      if( palette[i]&0x08 ) white.fill( i*WIDTH/8 , ( i + 1 )*WIDTH/8 );
      display.fill( i*WIDTH/8 , ( i + 1 )*WIDTH/8 , palette[i]&0x07 );
    }

    if( !equ ) {
      int probe = 0;
      for( int i = 0 ; i < 4 ; i++ ) {
        if( palette[i] < palettes4[paletteN][i] ) { palette[i]++; probe++; }
        if( palette[i] > palettes4[paletteN][i] ) { palette[i]--; probe++; }
        if( palette[i+4] < palettes4[paletteN][i] ) { palette[i+4]++; probe++; }
        if( palette[i+4] > palettes4[paletteN][i] ) { palette[i+4]--; probe++; }
      }
      if( !probe ) equ = true;
    }
    
  }
  
  void updateState() {
    for( int i = 0 ; i < 4 ; i++ ) {
      
      //Click detection
      if ( ( buttonState[i] == LOW ) && ( buttonLastState[i] == HIGH ) ) {
        buttonPushCounter[i]++;
        switch( i ){
          case( 0 ): //Red button, browser
            if( paletteN < palettes4_size ) {
              paletteN++;
            } else {
              paletteN = 0;
            }
            equ = false;
          break;
          case( 3 ): //White button, print function
            Serial.print("{ ");
            for( int i = 0 ; i < 4 ; i++ ){
              Serial.print( (int)palette[i] );
              ( i < 3 ) ? Serial.print(" , ") : Serial.println(" }");
            }
           break;
           default: break;
        }
      }
      
      //Toggle detection
      if ( buttonPushCounter[i]&1 ) {
        if( i == 1 ){ //Green button
          led[i] = HIGH;
          generate = true;
        }
      } else {
        if( i == 1 ){ //Green button
          generate = false;
          led[i] = LOW;
        }
      }

      //Update button states
      buttonLastState[i]= buttonState[i];
      
      //Update pot states
      potValue[i] = pot[i];

      if( generate ){
        if( !paused ){
          palette[i] = potValue[i]>>6;
        } else {
          palette[i+4] = potValue[i]>>6;
        }
      }
     
      if( i != 1 ) led[i] = !buttonState[i];
    }
    
  }

  void reset() {
    //Reset states
    paused = false;
    equ = true;
  }

} paletteGenerator;
