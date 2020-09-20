//Extenden color mode, with white led
#define W 8
#define W_BLUE 9
#define W_RED 10
#define W_MAGENTA 11
#define W_GREEN 12
#define W_CYAN 13
#define W_YELLOW 14
#define W_WHITE 15

/*
{ 3 , 11 , 13 , 1 }

 */

struct PaletteGenerator : Program {

  char palette[8] = { BLACK , BLUE , RED , MAGENTA , GREEN , CYAN , YELLOW , WHITE };
  
  void setup() {
    potValue[0] = 0x00;
  }
  
  void draw() {
    //Clear screen
    display.clear();
    white.clear();

    for( int i = 0 ; i < 8 ; i++ ) {
      if( palette[i]&0x08 ) white.fill( i*WIDTH/8 , ( i + 1 )*WIDTH/8 );
      display.fill( i*WIDTH/8 , ( i + 1 )*WIDTH/8 , palette[i]&0x07 );
    }
  }
  
  
  void updateState() {
    for( int i = 0 ; i < 4 ; i++ ) {
      
      //Click detection
      if ( ( buttonState[i] == LOW ) && ( buttonLastState[i] == HIGH ) ) {
        buttonPushCounter[i]++;
        switch( i ){
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
      }

      //Update button states
      buttonLastState[i]= buttonState[i];
      
      //Update pot states
      potValue[i] = pot[i];

      if( !paused ){
        palette[i] = potValue[i]>>6;
      } else {
        palette[i+4] = potValue[i]>>6;
      }
     
      led[i] = !buttonState[i];
    }
    
  }

  void reset() {
    //Reset states
    paused = false;
  }

} paletteGenerator;
