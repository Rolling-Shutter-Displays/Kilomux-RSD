struct Disolve : Program {

  void setup() {}
  
  void draw() {
    copyBackground();
    
    if( !paused ) {    
      for( int i = 0 ; i < 4 ; i++ ) {
        int x0 = random( 0 ,  WIDTH );
        int x1 = random( 0 , WIDTH );
        RollOver( x0 , x1 , ch[i] );
      }
    }
  }
  
  void updateState() {}

  void reset() { paused = false; }

} disolve;
