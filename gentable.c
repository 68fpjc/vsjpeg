/*
	gentable.c
			Written by Igarashi
*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<ctype.h>

int main( int argc, char *argv[] )
{
	int	nshift, i;
	if ( argc != 2 )
		return EXIT_FAILURE;
	switch ( tolower( *argv[1])) {
		case 'r':
			nshift = 6;
			break;
		case 'g':
			nshift = 11;
			break;
		case 'b':
			nshift = 1;
			break;
		default:
			return EXIT_FAILURE;
			break;
	}
	for ( i = 0; i < 256; i++ ) {
		printf( "%s0x%04x%s%s",
					( i % 8 )? "": "\t",
					(( 32 * i ) / 256 ) << nshift,
					((( i + 1 ) % 8 ) || ( i != 255 ))? ", ": "",
					(( i + 1 ) % 8 )? "": "\n" );
	}
	return EXIT_SUCCESS;
}

