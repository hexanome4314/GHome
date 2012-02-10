#ifndef __ENGINE_H_
#define __ENGINE_H_

#include <stdio.h>
#include <pthread.h>
#include "memory.h"
#include "ios_api.h"

/**
 * Lancer la gestion des règles à partir d'un fichier de règles
 * \param file Le fichier de règle
 * \return 0 si tout va bien, négatif sinon
 */
int launch_engine(const char *file);

/**
 * Relit les règles à partir du fichier xml
 * \return 0 si tout va bien, négatif sinon
 */
int reload_rules();

#endif
