// Utils ///////////////////////////////////////////////////////////////////////////////

void copyBackground() {
  red.copy( &red );
  green.copy( &green );
  blue.copy( &blue );
  white.copy( &white );
}

void clearBackground() {
  //Clear screen
  display.clear();
  //Clear white
  white.clear();
}


void dither( int x0 , int x1 , Channel ch ) {
  if ( x1 > x0 ) {  
    do {
      if ( x1 % 2 ) ch.line( x1 );
      x1--;
    } while( x1 > x0 ); 
    
    if ( x1 % 2 ) ch.line( x0 ) ;
  }
    /*
  } else if( x1 == x0 ) {
            
            line( x1 );
            
        } else {
            
            do {
                line( x0 );
                x0--;
            } while( x0 > x1 );
            
            line( x1 );
        }
}
*/
}

void dither2( unsigned int x0 , unsigned int x1 , char o , Channel *ch ) {
  bool right;
  
  switch( o ) {
    case 0 : return; break;
    case 1 : right = true; break;
    case 2 : right = false; break;
    default : return; break; 
  }
  
  if ( x1 > x0 ) {  
    do {
      if ( !( ( x1 % 2 )^right ) ) ch->clear( x1 );
      x1--;
    } while( x1 > x0 ); 
    
    if ( !( ( x1 % 2 )^right ) ) ch->clear( x0 ) ;
  
  } else if( x1 == x0 ) {
            
    if ( !( ( x1 % 2 )^right ) ) ch->clear( x0 ) ;
            
  } else { // if ( x1 < x0 )
    do {
      if ( !( ( x0 % 2 )^right ) )  ch->clear( x0 );
      x0--;
    } while( x0 > x1 );
    
    if ( !( ( x0 % 2 )^right ) ) ch->clear( x1 ) ;
  }
}

void ShiftByOne( int begin , int end , Channel *ch ) {
  
  if( begin > end ) { //Shift Right
    int i = end;
    do {
      ( ch->get( i + 1 ) ) ? ch->line( i ) : ch->clear( i ) ;
      i++;
    } while ( i < begin );
    ch->clear( begin );
  }

  if( begin < end ) { //Shift Left
    int i = end;
    do {
      ( ch->get( i - 1 ) ) ? ch->line( i ) : ch->clear( i ) ;
      i--;
    } while ( i > begin );
    ch->clear( begin );
  }
  
}

void RollOver( int begin , int end , Channel *ch ) {
  
  if( begin > end ) { //Shift Right
    int i = end;
    bool _end = ch->get(i);
    do {
      ( ch->get( i + 1 ) ) ? ch->line( i ) : ch->clear( i ) ;
      i++;
    } while ( i < begin );
    _end ? ch->line( begin ) : ch->clear( begin );
  }

  if( begin < end ) { //Shift Left
    int i = end;
    bool _end = ch->get(i);
    do {
      ( ch->get( i - 1 ) ) ? ch->line( i ) : ch->clear( i ) ;
      i--;
    } while ( i > begin );
    _end ? ch->line( begin ) : ch->clear( begin );
  }
}
