/**

Utilitaire pour parser le fichier de configuration des capteurs

**/

#ifndef __SENSOR_PARSE_H_
#define __SENSOR_PARSE_H_

#include "core.h"

/**
 * Lit un fichier de capteurs et rempli le tableau passé
 * en paramêtre selon les informations comprises dans le
 * fichier
 * \param path chemin du fichier xml
 * \param sensors tableau à remplir
 * \return <0 si erreur
 */
int read_sensors(const char* path, infos_sensor* sensor, int* drv);

#endif /* __SENSOR_PARSE_H_ */
