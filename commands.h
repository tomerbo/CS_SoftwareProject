#ifndef COMMANDS_H_
#define COMMANDS_H_

/** Command module **
 *
 * All commands are executed from here.
 *
 * See function name to understand which command it executes.
 *
 */

int commandSolve(Game* game, char *path, int *isGameOver);
void commandEdit(Game* game);
int commandEdit_OP(Game* game, char *path);
void commandMarkErrors(Game* game, int X);
void commandPrintBoard(Game* game);
int commandSet(Game* game, int X, int Y, int Z, int *isGameOver);
int commandValidate(Game* game);
int commandGuess(Game* game, double D, int *isGameOver);
int commandGenerate(Game* game, int X, int Y);
int commandUndo(Game* game);
int commandRedo(Game* game);
int commandSave(Game* game, char *path);
int commandHint(Game* game, int X, int Y);
int commandGuessHint(Game* game, int X, int Y);
int commandNumSolutions(Game* game);
int commandAutofill(Game* game, int *isGameOver);
int commandReset(Game* game);

#endif /* COMMANDS_H_ */
