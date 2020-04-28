#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main_aux.h"
#include "parser.h"
#include "game.h"
#include "errors.h"
#include "commands.h"

/* Starts the sudoku game. */
void startGame(Game *game){
	char line[MaxLineLength] = "";
	int X,Y,Z;
	double D;
	char *path;
	int isGameOver = 0;
	enum Command command = None;
	line[0]='\0'; line[MaxLineLength-1]='\0';

	printf("Welcome to \"The Sudoku\" game!\n");

	while(1){
		printf(" Please enter a command:\n>>> ");
		/* Read from the user and check it is not EOF. */
		if(fgets(line, MaxLineLength+1, stdin)==NULL){
			printf("\nExiting...\n");
			break;
		}
		/* Check if the user entered more then 256 characters. */
		if(strchr(line, '\n') == NULL) {
			int c;
			while((c = getc(stdin)) != '\n' && c != EOF);
			error_InvalidCommand();
			printf("Too many characters were entered in a single line. Please do not enter more than 256 characters.\n");
			command = None;
			line[0]='\0'; line[MaxLineLength-1]='\0';
			continue;
		}

		/* Check the user input for a valid command  */
		command = getValidCommand(line, &X, &Y, &Z, &D, &path, game);

		/* Check which valid command the user entered and call it. */
		if(command==SolveC){
			commandSolve(game, path, &isGameOver);
			free(path);
		}
		else if(command==EditC){
			commandEdit(game);
		}
		else if(command==EditC_OP){
			commandEdit_OP(game, path);
			free(path);
		}
		else if(command==Mark_Errors){
			commandMarkErrors(game, X);
		}
		else if(command==Print_Board){
			commandPrintBoard(game);
		}
		else if(command==Set){
			commandSet(game, X, Y, Z, &isGameOver);
		}
		else if(command==Validate){
			commandValidate(game);
		}
		else if(command==Guess){
			commandGuess(game, D, &isGameOver);
		}
		else if(command==Generate){
			commandGenerate(game, X, Y);
		}
		else if(command==Undo){
			commandUndo(game);
		}
		else if(command==Redo){
			commandRedo(game);
		}
		else if(command==Save){
			commandSave(game, path);
			free(path);
		}
		else if(command==Hint){
			commandHint(game, X, Y);
		}
		else if(command==Guess_Hint){
			commandGuessHint(game, X, Y);
		}
		else if(command==Num_Solutions){
			commandNumSolutions(game);
		}
		else if(command==Autofill){
			commandAutofill(game, &isGameOver);
		}
		else if(command==Reset){
			commandReset(game);
		}
		else if(command==Exit){
			printf("\nExiting...\n");
			break;
		}

		if(isGameOver){
			printf("[Hurray] Puzzle solved successfully.\n");
			game->state = Init;
			isGameOver = 0;
		}

		command = None;
		line[0]='\0'; line[MaxLineLength-1]='\0';
	}

}
