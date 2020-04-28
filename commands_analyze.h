#ifndef COMMANDS_ANALYZE_H_
#define COMMANDS_ANALYZE_H_

/* commands_analyze
 * This module analyze the parameters the user entered for a given command
 * and check if the number of parameters is correct,
 * if the parameters are valid (correct range/legal...)
 * ...
 * and saves the parameters.
 * Returns the appropriate enum Command if the parameters and correct and valid,
 * Otherwise returns None (enum Command)
 */

enum Command checkSolve(char **path);
enum Command checkEdit(char **path);
enum Command checkMarkErrors(int *X, Game* game);
enum Command checkPrintBoard(Game* game);
enum Command checkSet(int *X, int *Y, int *Z, Game* game);
enum Command checkValidate(Game* game);
enum Command checkGuess(double *D, Game* game);
enum Command checkGenerate(int *X, int *Y, Game* game);
enum Command checkUndo(Game* game);
enum Command checkRedo(Game* game);
enum Command checkSave(char **path, Game* game);
enum Command checkHint(int *X, int *Y, Game* game);
enum Command checkGuessHint(int *X, int *Y, Game* game);
enum Command checkNumSolutions(Game* game);
enum Command checkAutofill(Game* game);
enum Command checkReset(Game* game);
enum Command checkExit();

#endif /* COMMANDS_ANALYZE_H_ */
