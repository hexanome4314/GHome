#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "console.h"

#define	BUFFER_SIZE	str_len + 128

/**
Ecrit dans le descripteur le contenu du buffer
\param  fd      Le descripteur de destination
        str  La chaîne qui doit être écrite
\return Le nombre de caractères écrits, ou -1 si une erreur est survenue
*/
int print( int fd, const char* str, ... )
{
	char* buffer;
	va_list argList;
	register int str_len;
	register int res;

	/* Récupère la taille minimale du buffer */
	str_len = strlen( str );

	/* Allocation du nouveau buffer */
	buffer = (char*) malloc( BUFFER_SIZE );

	/* Subsistution des arguments */
	va_start( argList, str );
   	vsnprintf( buffer, BUFFER_SIZE, str, argList );

	/* Null terminated string */
   	buffer[BUFFER_SIZE] = 0;

	/* Ecriture */
	res = write( fd, buffer, strlen(buffer) );

	/* Libération */
	free( buffer );

	return res;
}

