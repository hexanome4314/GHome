#ifndef __PARSER_H
#define __PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <string.h>
#include "engine.h"
#include "engine-types.h"

Rule * get_rules(char *filename);

#endif /* __PARSER_H */
