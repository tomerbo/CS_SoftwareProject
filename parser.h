#ifndef PARSER_H_
#define PARSER_H_

#include "game.h"
#include "command_types.h"

/* Max input length: 256 cells for input + 1 cell for ending '\0' */
#define MaxLineLength 257

/*
 * getValidCommand - Reads the user input and return what valid command the user entered.
 */

enum Command getValidCommand(char *line, int *X, int *Y, int *Z, double *D, char **path, Game* game);

#endif
