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
    } while ( x1 > x0 );

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

  switch ( o ) {
    case 0 : return; break;
    case 1 : right = true; break;
    case 2 : right = false; break;
    default : return; break;
  }

  if ( x1 > x0 ) {
    do {
      if ( !( ( x1 % 2 )^right ) ) ch->clear( x1 );
      x1--;
    } while ( x1 > x0 );

    if ( !( ( x1 % 2 )^right ) ) ch->clear( x0 ) ;

  } else if ( x1 == x0 ) {

    if ( !( ( x1 % 2 )^right ) ) ch->clear( x0 ) ;

  } else { // if ( x1 < x0 )
    do {
      if ( !( ( x0 % 2 )^right ) )  ch->clear( x0 );
      x0--;
    } while ( x0 > x1 );

    if ( !( ( x0 % 2 )^right ) ) ch->clear( x1 ) ;
  }
}

void ShiftByOne( int begin , int end , Channel *ch ) {

  if ( begin > end ) { //Shift Right
    int i = end;
    do {
      ( ch->get( i + 1 ) ) ? ch->line( i ) : ch->clear( i ) ;
      i++;
    } while ( i < begin );
    ch->clear( begin );
  }

  if ( begin < end ) { //Shift Left
    int i = end;
    do {
      ( ch->get( i - 1 ) ) ? ch->line( i ) : ch->clear( i ) ;
      i--;
    } while ( i > begin );
    ch->clear( begin );
  }

}

void RollOver( int begin , int end , Channel *ch ) {

  if ( begin > end ) { //Shift Right
    int i = end;
    bool _end = ch->get(i);
    do {
      ( ch->get( i + 1 ) ) ? ch->line( i ) : ch->clear( i ) ;
      i++;
    } while ( i < begin );
    _end ? ch->line( begin ) : ch->clear( begin );
  }

  if ( begin < end ) { //Shift Left
    int i = end;
    bool _end = ch->get(i);
    do {
      ( ch->get( i - 1 ) ) ? ch->line( i ) : ch->clear( i ) ;
      i--;
    } while ( i > begin );
    _end ? ch->line( begin ) : ch->clear( begin );
  }
}

void copyBuffer( char *src , char *dst ) {
  for( uint8_t i = 0 ; i < BWIDTH ; i++ ) {
    *( dst + i ) = *( src + i );
  }
}

void copyBuffer( char *src , char *dst , int t ) {
  
  t = t % ( WIDTH + 1 );
  
  for( int _pos = 0 ; _pos < WIDTH ; _pos++ ) {
    
    int p = _pos + t;
    
    if ( *( src + _pos / 8 ) & ( 1 << _pos % 8 ) ) {
      
      if ( ( p ) > WIDTH ) { p = p - WIDTH - 1; }
      
      *( dst + p / 8 ) |= ( 1 << p % 8 );
    }
  }
  
}

/*  
void triangle( int begin , int end , int steps , Channel *ch ) {
  int interval;
  if( begin <= end ) {
    interval = ( end - begin ) / steps;
    for ( uint8_t i = 1 ; i < steps ; i++ ) {
      ch->fill( begin + interval*i , begin + interval*i + map( i , 1 , steps, 0 , interval ) );
    }
  } else {
    interval = ( begin - end ) / steps;
    for ( uint8_t i = 1 ; i < steps ; i++ ) {
      ch->fill( begin - interval*i , begin - interval*i - map( i , 1 , steps, 0 , interval ) );
    }
  }
}
*/

bool fillSafe( int y0 , int y1 , int x0 , int x1 , Channel *ch ) { 

  //Check boundaries
  if ( ( y0 < 0 ) || ( y1 < 0 ) ) return false;
  if ( ( y0 > WIDTH ) || ( y1 > WIDTH ) ) return false;
  
  if ( ( x0 < y0 ) && ( x1 < y0 ) ) return false;
  if ( ( x0 > y1 ) && ( x1 > y1 ) ) return false;
        
  if ( x1 > x0 ) {       
    if ( x1 > y1 ) x1 = y1;
    if ( x0 < y0 ) x0 = y0;
    
    do {
      ch->line( x1 );
      x1--;
    } while( x1 > x0 );
            
    ch->line( x0 );
            
    } else if( x1 == x0 ) {    
    
      if ( ( x1 > y0 ) && ( x1 < y1 ) ) ch->line( x1 );
    
  } else {
    if ( x0 > y1 ) x0 = y1;
    if ( x1 < y0 ) x1 = y0;
    
    do {
      ch->line( x0 );
      x0--;
    } while( x0 > x1 );        
    
      ch->line( x1 );
      
    }
}
