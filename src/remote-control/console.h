#ifndef __CONSOLE_H__
#define __CONSOLE_H__

/**
Ecrit dans le descripteur le contenu du buffer
\param	fd	Le descripteur de destination
	buffer	La chaîne qui doit être écrite
\return	Le nombre de caractères écrits, ou -1 si une erreur est survenue
*/
int print( int fd, const char* str, ... );

#endif
