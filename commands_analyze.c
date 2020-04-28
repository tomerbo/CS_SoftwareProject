#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errors.h"
#include "parser.h"

/* Duplicates the string and return a pointer to duplication. */
char *strdupl(const char *c)
{
	char *dup = malloc(strlen(c) + 1);

	if(!dup){
		printf("[Error] strdupl has failed\n");
		exit(0);
	}

	if (dup != NULL)
		strcpy(dup, c);

	return dup;
}

int isFloatNumber(const char *str){
	char *endptr;

	if(str==NULL){
		return 0;
	}
	else if(str[0]=='\0'){
		return 0;
	}
	strtod(str, &endptr);

	if(str==endptr){ /* could not find number at the start */
		return 0;
	}

	if(endptr[0]!='\0'){ /* there are more characters */
		return 0;
	}

	return 1;
}

int isIntNumber(const char *str){
	char *endptr;

	if(str==NULL){
		return 0;
	}
	else if(str[0]=='\0'){
		return 0;
	}
	strtol(str, &endptr, 10);

	if(str==endptr){ /* could not find number at the start */
		return 0;
	}

	if(endptr[0]!='\0'){ /* there are more characters */
		return 0;
	}

	return 1;
}


enum Command checkSolve(char **path){
	char* word = strtok(NULL, " \t\r\n");
	if(!word){
		error_CommandParamMissing();
		printf("'solve' command should get one parameter. use as following: \"solve #FILE_PATH#\" .\n");
		return None;
	}
	*path = strdupl(word);

	word = strtok(NULL, " \t\r\n");
	if(word){
		error_CommandParamExtra();
		printf("'solve' command should get one parameter. Use as following: \"solve #FILE_PATH#\" .\n");
		return None;
	}

	return SolveC;
}


enum Command checkEdit(char **path){
	char* word = strtok(NULL, " \t\r\n");
	if(!word)
		return EditC;
	*path = strdupl(word);

	word = strtok(NULL, " \t\r\n");
	if(word){
		error_CommandParamExtra();
		printf("'edit' command should not get any parameter or get one parameters. Use as following: \"edit\" or \"edit #FILE_PATH#\" .\n");
		return None;
	}

	return EditC_OP;
}

enum Command checkMarkErrors(int *X, Game* game){
	char* word, *Xstr;
	if(game->state != Solve){
		error_UnavailableCommand("Solve");
		return None;
	}

	word = strtok(NULL, " \t\r\n");
	if(!word){
		error_CommandParamMissing();
		printf("'mark_errors' command should get one parameter. use as following: \"mark_errors #0or1#\" .\n");
		return None;
	}
	Xstr = strdupl(word);

	word = strtok(NULL, " \t\r\n");
	if(word){
		free(Xstr);
		error_CommandParamExtra();
		printf("'mark_errors' command should get one parameter. use as following: \"mark_errors #0or1#\" .\n");
		return None;
	}

	if(isIntNumber(Xstr) && (strtol(Xstr, NULL, 10)==0 || strtol(Xstr, NULL, 10)==1))
		*X = strtol(Xstr, NULL, 10);
	else{
		free(Xstr);
		error_CommandParamRange();
		printf("'mark_errors' command should get one parameter. Use as following: \"mark_errors #0or1#\" .\n");
		printf("Please fix the parameter value to 0 or 1 and try again.\n");
		return None;
	}

	free(Xstr);
	return Mark_Errors;
}

enum Command checkPrintBoard(Game* game){
	char* word;
	if(game->state == Init){
		error_UnavailableCommand("Edit and Solve");
		return None;
	}

	word = strtok(NULL, " \t\r\n");
	if(word){
		error_CommandParamExtra();
		printf("'print_board' command should not get any parameter.\n");
		return None;
	}

	return Print_Board;
}

enum Command checkSet(int *X, int *Y, int *Z, Game* game){
	char *Xstr, *Ystr, *Zstr;
	char* word;
	if(game->state == Init){
		error_UnavailableCommand("Edit and Solve");
		return None;
	}

	word = strtok(NULL, " \t\r\n");
	if(!word){
		error_CommandParamMissing();
		printf("'set' command should get 3 parameter. Use as following: \"set #XCordinate# #YCordinate# #NumberToSet#\" .\n");
		return None;
	}
	Xstr = strdupl(word);

	word = strtok(NULL, " \t\r\n");
	if(!word){
		free(Xstr);
		error_CommandParamMissing();
		printf("'set' command should get 3 parameter. Use as following: \"set #XCordinate# #YCordinate# #NumberToSet#\" .\n");
		return None;
	}
	Ystr = strdupl(word);

	word = strtok(NULL, " \t\r\n");
	if(!word){
		free(Xstr);
		free(Ystr);
		error_CommandParamMissing();
		printf("'set' command should get 3 parameter. Use as following: \"set #XCordinate# #YCordinate# #NumberToSet#\" .\n");
		return None;
	}
	Zstr = strdupl(word);

	word = strtok(NULL, " \t\r\n");
	if(word){
		free(Xstr); free(Ystr); free(Zstr);
		error_CommandParamExtra();
		printf("'set' command should get 3 parameter. Use as following: \"set #XCordinate# #YCordinate# #NumberToSet#\" .\n");
		return None;
	}

	if(isIntNumber(Xstr) && strtol(Xstr, NULL, 10)>0){
		*X = strtol(Xstr, NULL, 10);
	}
	else{
		free(Xstr); free(Ystr); free(Zstr);
		error_CommandParamRange();
		printf("'set' command should get 3 parameter. Use as following: \"set #XCordinate# #YCordinate# #NumberToSet#\" ."
				"Where #XCordinate#, #YCordinate# should be number larger than zero and #NumberToSet# should be a positive number (including zero).\n");
		printf("Please fix #XCordinate# value and try again.\n");
		return None;
	}

	if(isIntNumber(Ystr) && strtol(Ystr, NULL, 10)>0){
		*Y = strtol(Ystr, NULL, 10);
	}
	else{
		free(Xstr); free(Ystr); free(Zstr);
		error_CommandParamRange();
		printf("'set' command should get 3 parameter. Use as following: \"set #XCordinate# #YCordinate# #NumberToSet#\" ."
				"Where #XCordinate#, #YCordinate# should be number larger than zero and #NumberToSet# should be a positive number (including zero).\n");
		printf("Please fix #YCordinate# value and try again.\n");
		return None;
	}

	if(isIntNumber(Zstr) && strtol(Zstr, NULL, 10)>=0){
		*Z = strtol(Zstr, NULL, 10);
	}
	else{
		free(Xstr); free(Ystr); free(Zstr);
		error_CommandParamRange();
		printf("'set' command should get 3 parameter. Use as following: \"set #XCordinate# #YCordinate# #NumberToSet#\" ."
				"Where #XCordinate#, #YCordinate# should be number larger than zero and #NumberToSet# should be a positive number (including zero).\n");
		printf("Please fix #NumberToSet# value and try again.\n");
		return None;
	}

	free(Xstr); free(Ystr); free(Zstr);

	if(*X > getBoardSize(game->board)){
		error_CommandParamBoardIllegal();
		printf("'set' command should get 3 parameter. Use as following: \"set #XCordinate# #YCordinate# #NumberToSet#\" ."
				"Where #XCordinate# and #YCordinate# should be in range of the board size and #NumberToSet# is a number between 0 and the board size.\n");
		printf("Please fix #XCordinate# value and try again.\n");
		return None;
	}
	else if(*Y > getBoardSize(game->board)){
		error_CommandParamBoardIllegal();
		printf("'set' command should get 3 parameter. Use as following: \"set #XCordinate# #YCordinate# #NumberToSet#\" ."
				"Where #XCordinate# and #YCordinate# should be in range of the board size and #NumberToSet# is a number between 0 and the board size.\n");
		printf("Please fix #YCordinate# value and try again.\n");
		return None;
	}
	else if(*Z > getBoardSize(game->board)){
		error_CommandParamBoardIllegal();
		printf("'set' command should get 3 parameter. Use as following: \"set #XCordinate# #YCordinate# #NumberToSet#\" ."
				"Where #XCordinate# and #YCordinate# should be in range of the board size and #NumberToSet# is a number between 0 and the board size.\n");
		printf("Please fix #NumberToSet# value and try again.\n");
		return None;
	}

	return Set;
}

enum Command checkValidate(Game* game){
	char* word;
	if(game->state == Init){
		error_UnavailableCommand("Edit and Solve");
		return None;
	}

	word = strtok(NULL, " \t\r\n");
	if(word){
		error_CommandParamExtra();
		printf("'validate' command should not get any parameter.\n");
		return None;
	}

	return Validate;
}

enum Command checkGuess(double *D, Game* game){
	char* word, *Dstr;

	if(game->state != Solve){
		error_UnavailableCommand("Solve");
		return None;
	}

	word = strtok(NULL, " \t\r\n");
	if(!word){
		error_CommandParamMissing();
		printf("'guess' command should get one parameter. use as following: \"mark_errors #0or1#\" .\n");
		return None;
	}
	Dstr = strdupl(word);

	word = strtok(NULL, " \t\r\n");
	if(word){
		free(Dstr);
		error_CommandParamExtra();
		printf("'guess' command should get one parameter. use as following: \"mark_errors #0or1#\" .\n");
		return None;
	}

	if(isFloatNumber(Dstr))
		*D = strtod(Dstr, NULL);
	else{
		free(Dstr);
		error_CommandParamRange();
		printf("'guess' command should get one parameter. Use as following: \"guess #RealNumber#\" .\n");
		printf("Please fix the parameter value to a real number and try again.\n");
		return None;
	}
	free(Dstr);

	if(*D > 1 || *D < 0){
		error_CommandParamRange();
		printf("'guess' command should get one parameter. Use as following: \"guess #RealNumber#\" .\n");
		printf("Please fix the parameter value to a real number in the range of [0,1] and try again.\n");
		return None;
	}

	return Guess;
}

enum Command checkGenerate(int *X, int *Y, Game* game){
	char *Xstr, *Ystr;
	char* word;
	if(game->state != Edit){
		error_UnavailableCommand("Edit");
		return None;
	}

	word = strtok(NULL, " \t\r\n");
	if(!word){
		error_CommandParamMissing();
		printf("'generate' command should get 2 parameter. Use as following: \"generate #Number# #Number#\" .\n");
		return None;
	}
	Xstr = strdupl(word);

	word = strtok(NULL, " \t\r\n");
	if(!word){
		free(Xstr);
		error_CommandParamMissing();
		printf("'generate' command should get 2 parameter. Use as following: \"generate #Number# #Number#\" .\n");
		return None;
	}
	Ystr = strdupl(word);

	word = strtok(NULL, " \t\r\n");
	if(word){
		free(Xstr); free(Ystr);
		error_CommandParamExtra();
		printf("'generate' command should get 2 parameter. Use as following: \"generate #Number# #Number#\" .\n");
		return None;
	}

	if(isIntNumber(Xstr) && strtol(Xstr, NULL, 10)>=0){
		*X = strtol(Xstr, NULL, 10);
	}
	else{
		free(Xstr); free(Ystr);
		error_CommandParamRange();
		printf("'generate' command should get 2 parameter. Use as following: \"set #X# #Y#\" ."
				"Where #X# and #Y# should be positive integers.\n");
		printf("Please fix #X# value and try again.\n");
		return None;
	}

	if(isIntNumber(Ystr) && strtol(Ystr, NULL, 10)>=0){
		*Y = strtol(Ystr, NULL, 10);
	}
	else{
		free(Xstr); free(Ystr);
		error_CommandParamRange();
		printf("'generate' command should get 2 parameter. Use as following: \"set #X# #Y#\" . "
				"Where #X# and #Y# should be positive integers.\n");
		printf("Please fix #Y# value and try again.\n");
		return None;
	}

	free(Xstr); free(Ystr);

	return Generate;
}

enum Command checkUndo(Game* game){
	char* word;
	if(game->state == Init){
		error_UnavailableCommand("Edit and Solve");
		return None;
	}

	word = strtok(NULL, " \t\r\n");
	if(word){
		error_CommandParamExtra();
		printf("'undo' command should not get any parameter.\n");
		return None;
	}

	return Undo;
}

enum Command checkRedo(Game* game){
	char* word;
	if(game->state == Init){
		error_UnavailableCommand("Edit and Solve");
		return None;
	}

	word = strtok(NULL, " \t\r\n");
	if(word){
		error_CommandParamExtra();
		printf("'redo' command should not get any parameter.\n");
		return None;
	}

	return Redo;
}

enum Command checkSave(char **path, Game* game){
	char* word;
	if(game->state == Init){
		error_UnavailableCommand("Edit and Solve");
		return None;
	}

	word = strtok(NULL, " \t\r\n");
	if(!word){
		error_CommandParamMissing();
		printf("'save' command should get one parameter. use as following: \"save #FILE_PATH#\" .\n");
		return None;
	}
	*path = strdupl(word);

	word = strtok(NULL, " \t\r\n");
	if(word){
		error_CommandParamExtra();
		printf("'save' command should get one parameter. Use as following: \"save #FILE_PATH#\" .\n");
		return None;
	}

	return Save;
}

enum Command checkHint(int *X, int *Y, Game* game){
	char *Xstr, *Ystr;
	char* word;
	if(game->state != Solve){
		error_UnavailableCommand("Solve");
		return None;
	}

	word = strtok(NULL, " \t\r\n");
	if(!word){
		error_CommandParamMissing();
		printf("'hint' command should get 2 parameter. Use as following: \"hint #XCordinate# #YCordinate#\" .\n");
		return None;
	}
	Xstr = strdupl(word);

	word = strtok(NULL, " \t\r\n");
	if(!word){
		free(Xstr);
		error_CommandParamMissing();
		printf("'hint' command should get 2 parameter. Use as following: \"hint #XCordinate# #YCordinate#\" .\n");
		return None;
	}
	Ystr = strdupl(word);

	word = strtok(NULL, " \t\r\n");
	if(word){
		free(Xstr); free(Ystr);
		error_CommandParamExtra();
		printf("'hint' command should get 2 parameter. Use as following: \"hint #XCordinate# #YCordinate#\" .\n");
		return None;
	}

	if(isIntNumber(Xstr) && strtol(Xstr, NULL, 10)>0){
		*X = strtol(Xstr, NULL, 10);
	}
	else{
		free(Xstr); free(Ystr);
		error_CommandParamRange();
		printf("'hint' command should get 2 parameter. Use as following: \"hint #XCordinate# #YCordinate#\" ."
				"Where #XCordinate# and #YCordinate# should be integers larger than zero.\n");
		printf("Please fix #XCordinate# value and try again.\n");
		return None;
	}

	if(isIntNumber(Ystr) && strtol(Ystr, NULL, 10)>0){
		*Y = strtol(Ystr, NULL, 10);
	}
	else{
		free(Xstr); free(Ystr);
		error_CommandParamRange();
		printf("'hint' command should get 2 parameter. Use as following: \"hint #XCordinate# #YCordinate#\" ."
				"Where #XCordinate# and #YCordinate# should be integers larger than zero.\n");
		printf("Please fix #YCordinate# value and try again.\n");
		return None;
	}

	free(Xstr); free(Ystr);
	return Hint;
}

enum Command checkGuessHint(int *X, int *Y, Game* game){
	char *Xstr, *Ystr;
	char* word;
	if(game->state != Solve){
		error_UnavailableCommand("Solve");
		return None;
	}

	word = strtok(NULL, " \t\r\n");
	if(!word){
		error_CommandParamMissing();
		printf("'guess_hint' command should get 2 parameter. Use as following: \"guess_hint #XCordinate# #YCordinate#\" .\n");
		return None;
	}
	Xstr = strdupl(word);

	word = strtok(NULL, " \t\r\n");
	if(!word){
		free(Xstr);
		error_CommandParamMissing();
		printf("'guess_hint' command should get 2 parameter. Use as following: \"guess_hint #XCordinate# #YCordinate#\" .\n");
		return None;
	}
	Ystr = strdupl(word);

	word = strtok(NULL, " \t\r\n");
	if(word){
		free(Xstr); free(Ystr);
		error_CommandParamExtra();
		printf("'guess_hint' command should get 2 parameter. Use as following: \"guess_hint #XCordinate# #YCordinate#\" .\n");
		return None;
	}

	if(isIntNumber(Xstr) && strtol(Xstr, NULL, 10)>0){
		*X = strtol(Xstr, NULL, 10);
	}
	else{
		free(Xstr); free(Ystr);
		error_CommandParamRange();
		printf("'guess_hint' command should get 2 parameter. Use as following: \"guess_hint #XCordinate# #YCordinate#\" ."
				"Where #XCordinate# and #YCordinate# should be integers larger than zero.\n");
		printf("Please fix #XCordinate# value and try again.\n");
		return None;
	}

	if(isIntNumber(Ystr) && strtol(Ystr, NULL, 10)>0){
		*Y = strtol(Ystr, NULL, 10);
	}
	else{
		free(Xstr); free(Ystr);
		error_CommandParamRange();
		printf("'guess_hint' command should get 2 parameter. Use as following: \"guess_hint #XCordinate# #YCordinate#\" ."
				"Where #XCordinate# and #YCordinate# should be integers larger than zero.\n");
		printf("Please fix #YCordinate# value and try again.\n");
		return None;
	}

	free(Xstr); free(Ystr);
	return Guess_Hint;
}

enum Command checkNumSolutions(Game* game){
	char* word;
	if(game->state == Init){
		error_UnavailableCommand("Edit and Solve");
		return None;
	}

	word = strtok(NULL, " \t\r\n");
	if(word){
		error_CommandParamExtra();
		printf("'num_solutions' command should not get any parameter.\n");
		return None;
	}

	return Num_Solutions;
}

enum Command checkAutofill(Game* game){
	char* word;
	if(game->state != Solve){
		error_UnavailableCommand("Solve");
		return None;
	}

	word = strtok(NULL, " \t\r\n");
	if(word){
		error_CommandParamExtra();
		printf("'autofill' command should not get any parameter.\n");
		return None;
	}

	return Autofill;
}

enum Command checkReset(Game* game){
	char* word;
	if(game->state == Init){
		error_UnavailableCommand("Edit and Solve");
		return None;
	}

	word = strtok(NULL, " \t\r\n");
	if(word){
		error_CommandParamExtra();
		printf("'reset' command should not get any parameter.\n");
		return None;
	}

	return Reset;
}

enum Command checkExit(){
	char* word = strtok(NULL, " \t\r\n");
	if(word){
		error_CommandParamExtra();
		printf("'exit' command should not get any parameter.\n");
		return None;
	}

	return Exit;
}
