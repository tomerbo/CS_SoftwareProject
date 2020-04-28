#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "errors.h"
#include "commands_analyze.h"


/*
 * getCommand
 * @return The command entered, if valid, as an enum Command.
 */
enum Command getValidCommand(char *line, int *X, int *Y, int *Z, double *D, char **path, Game* game){
	char* word = strtok(line, " \t\r\n"); /* Initialize strtok. */

	if(!word) /* Ignore blank lines. */
		return Ignore;

	/* Check which command was entered, if any. */
	if(strcmp(word, "solve")==0)
		return checkSolve(path);
	else if(strcmp(word, "edit")==0)
		return checkEdit(path);
	else if(strcmp(word, "mark_errors")==0)
		return checkMarkErrors(X, game);
	else if(strcmp(word, "print_board")==0)
		return checkPrintBoard(game);
	else if(strcmp(word, "set")==0)
		return checkSet(X,Y,Z,game);
	else if(strcmp(word, "validate")==0)
		return checkValidate(game);
	else if(strcmp(word, "guess")==0)
		return checkGuess(D,game);
	else if(strcmp(word, "generate")==0)
		return checkGenerate(X,Y,game);
	else if(strcmp(word, "undo")==0)
		return checkUndo(game);
	else if(strcmp(word, "redo")==0)
		return checkRedo(game);
	else if(strcmp(word, "save")==0)
		return checkSave(path,game);
	else if(strcmp(word, "hint")==0)
		return checkHint(X,Y,game);
	else if(strcmp(word, "guess_hint")==0)
		return checkGuessHint(X,Y,game);
	else if(strcmp(word, "num_solutions")==0)
		return checkNumSolutions(game);
	else if(strcmp(word, "autofill")==0)
		return checkAutofill(game);
	else if(strcmp(word, "reset")==0)
		return checkReset(game);
	else if(strcmp(word, "exit")==0)
		return checkExit();

	error_InvalidCommand();
	if(game->state == Init)
		printf("Current mode is 'Init'. Please use one of the following commands: \n solve X\n edit [X]\n exit\n\n");
	else if(game->state == Solve)
		printf("Current mode is 'Solve'. Please use one of the following commands: \n solve X\n edit [X]\n mark_errors\n print_board\n set X Y Z\n validate\n guess X\n undo\n redo\n save X\n hint X Y\n guess_hint X Y\n num_solutions\n autofill\n reset \n exit\n\n");
	else if(game->state == Edit)
		printf("Current mode is 'Edit'. Please use one of the following commands: \n solve X\n edit [X]\n print_board\n set X Y Z\n validate\n generate X Y\n undo\n redo\n save X\n num_solutions\n reset \n exit\n\n");
	return None;
}
