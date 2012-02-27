#ifndef __ENGINE_H_
#define __ENGINE_H_

#include <stdio.h>
#include <pthread.h>
#include "memory.h"
#include "ios_api.h"
#include "core.h"

/**
 * Lancer la gestion des règles à partir d'un fichier de règles
 * \param file Le fichier de règle
 * \param sensor structure décrivant les capteurs à qui appliquer les regles
 * \return 0 si tout va bien, négatif sinon
 */
int launch_engine(const char *file, infos_sensor *sensor);

/**
 * Relit les règles à partir du fichier xml
 * \param sensor structure décrivant les capteurs à qui appliquer les regles
 * \return 0 si tout va bien, négatif sinon
 */
int reload_rules(infos_sensor *sensor);

/**
 * Initialise la gestion des règles à partir d'un fichier de règles
 * Suite à l'appel de l'initialisation, les valeurs des capteurs
 * peuvent être prisent en compte en appelant apply_actions
 * \param file Le fichier de règles
 * \param sensor structure permettant de faire le lien entre nom et fd
 * \return 0 si tout va bien, négatif sinon
 */
int init_engine(const char *file, infos_sensor *sensor);

/**
 * Parcours l'ensemble des règles et applique les actions associées de
 * chacune si les conditions sont vérifiées
 * \param device file descriptor du capteur ayant envoyé une info
 * \param field le champ modifié
 * \param val la nouvelle valeur
 */
void apply_actions(int device, unsigned int field, float val);

/**
 * Ecrit les règles en applications sur une sortie
 * \param output la sortie
 */
void fprint_rules(int output);

#endif
