/**

Utilitaire pour parser le fichier de configuration des capteurs

**/

#ifndef __CONFIG_H_
#define __CONFIG_H_

#include "core.h"

/**
 * Lit un fichier de capteurs et rempli le tableau passé
 * en paramêtre selon les informations comprises dans le
 * fichier
 * \param path chemin du fichier xml
 * \param sensors tableau à remplir
 * \return <0 si erreur
 */
int read_sensors(const char* path, infos_sensor* sensor, infos_drv* drv);

/**
 * Lit le fichier de configuration&authentification auth.xml
 * renseigne les variables fournis en entrée
 * \param pw le password du telnet
 * \param port_remote_control le port du telnet
 * \param port_actionner le port de l'actionner (pour le php)
 */
xmlChar* read_auth( unsigned int* port_remote_control, unsigned int* port_actionner);

#endif /* __SENSOR_PARSE_H_ */
