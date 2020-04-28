#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "SPBufferset.h"

#include "main_aux.h"
#include "game.h"

int main(void){
	Game *game;
	SP_BUFF_SET();

	/* Use current time as seed for random generator */
	srand(time(0));

	/* Create the game structure */
	game = createGame();

	/* Start the sudoku game */
	startGame(game);

	/* Free all memory before exiting game */
	destroyGame(game);
	return 0;
}
