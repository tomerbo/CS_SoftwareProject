#include <stdio.h>
#include <stdlib.h>

#include "command_types.h"
#include "history.h"
#include "game.h"

/*/*************************************************
 * Functions for Creating and Destroying History   *
 ***************************************************/

GameList* createHistory(){
	GameList* moveHistory = (GameList*)calloc(1 , sizeof(GameList));
	if(!moveHistory){
		printf("[Error] createHistory has failed\n");
		exit(0);
	}
	return moveHistory;
}

void destroyCommandMoveList(CommandMoves* CommandMove){
	MoveList* MoveListNode;
	while(CommandMove->moveListFirst){
		MoveListNode = CommandMove->moveListFirst->next;
		free(CommandMove->moveListFirst);
		CommandMove->moveListFirst = MoveListNode;
	}
}

void destroyCommandMove(Game* game, CommandMoves* CommandMove){
	CommandMoves* CommandMovePrev;
	CommandMoves* CommandMoveNext;
	CommandMovePrev = CommandMove->prev;
	CommandMoveNext = CommandMove->next;

	destroyCommandMoveList(CommandMove);

	if(!CommandMovePrev && !CommandMoveNext){
		game->moveHistory->FirstNode = NULL;
	}
	else if(!CommandMovePrev){
		game->moveHistory->FirstNode = CommandMoveNext;
		CommandMoveNext->prev = NULL;
	}
	else if(!CommandMoveNext){
		CommandMovePrev->next = NULL;
	}
	else{
		CommandMovePrev->next = CommandMoveNext;
		CommandMoveNext->prev = CommandMovePrev;
	}
	free(CommandMove);
}

void destroyHistory(Game *game){
	if(game->moveHistory->FirstNode){
		while(game->moveHistory->FirstNode){
			destroyCommandMove(game, game->moveHistory->FirstNode);
		}
	}
	free(game->moveHistory);
}

/*/*************************************************
 * 				"inside" functions 				   *
 ***************************************************/

void clearMoves(Game* game){
	destroyHistory(game);
	game->moveHistory = createHistory();
}


void clearBeyondCurr(Game* game){
	CommandMoves *CurrNext, *CurrNextNext;

	/* If curr is NULL then curr is at the start. (Can't undo) */
	if(!game->moveHistory->curr){
		clearMoves(game);
		return;
	}

	CurrNext = game->moveHistory->curr->next;

	while(CurrNext){
		CurrNextNext = CurrNext->next;
		destroyCommandMove(game, CurrNext);
		CurrNext = CurrNextNext;
	}

	game->moveHistory->curr->next = NULL;

}

void newCommandMoves(Game* game, enum Command command){
	CommandMoves* CommandMovesNode;
	clearBeyondCurr(game);
	if(!game->moveHistory->curr){
		game->moveHistory->curr = (CommandMoves*)calloc(1 , sizeof(CommandMoves));
		if(!game->moveHistory->curr){
			printf("[Error] newCommandMoves has failed\n");
			exit(0);
		}
		game->moveHistory->curr->command = command;
		game->moveHistory->FirstNode = game->moveHistory->curr;
	}
	else{
		CommandMovesNode = game->moveHistory->curr;
		game->moveHistory->curr = (CommandMoves*)calloc(1 , sizeof(CommandMoves));
		if(!game->moveHistory->curr){
			printf("[Error] newCommandMoves has failed\n");
			exit(0);
		}
		game->moveHistory->curr->prev = CommandMovesNode;
		CommandMovesNode->next = game->moveHistory->curr;
		game->moveHistory->curr->command = command;
	}
}


void addMoveToLastCommand(Game* game, int X, int Y, int oldValue, int newValue){
	MoveList* MoveListNode;
	if(!game->moveHistory->curr->moveListLast){
		game->moveHistory->curr->moveListLast = (MoveList*)calloc(1 , sizeof(MoveList));
		if(!game->moveHistory->curr->moveListLast){
			printf("[Error] addMoveToLastCommand has failed\n");
			exit(0);
		}
		game->moveHistory->curr->moveListFirst = game->moveHistory->curr->moveListLast;
	}
	else{
		MoveListNode = game->moveHistory->curr->moveListLast;
		game->moveHistory->curr->moveListLast = (MoveList*)calloc(1 , sizeof(MoveList));
		if(!game->moveHistory->curr->moveListLast){
			printf("[Error] addMoveToLastCommand has failed\n");
			exit(0);
		}
		MoveListNode->next = game->moveHistory->curr->moveListLast;
		game->moveHistory->curr->moveListLast->prev = MoveListNode;
	}
	game->moveHistory->curr->moveListLast->X = X;
	game->moveHistory->curr->moveListLast->Y = Y;
	game->moveHistory->curr->moveListLast->old = oldValue;
	game->moveHistory->curr->moveListLast->new = newValue;
}

void addBoardToLastCommand(Game* game, Board *OldBoard){
	int x, y;
	for(x=0; x<getBoardSize(game->board); ++x){
		for(y=0; y<getBoardSize(game->board); ++y){
			if(getCellNum(game->board, x, y) != getCellNum(OldBoard, x, y)){
				addMoveToLastCommand(game, x, y, getCellNum(OldBoard, x, y), getCellNum(game->board, x, y));
			}
		}
	}
}

/*/*************************************************
 * 				"outside" functions 			   *
 ***************************************************/

int undoMove(Game* game, int enablePrinting){
	MoveList* move;
	CommandMoves* curr = game->moveHistory->curr;
	if(!curr)
		return 0;

	if(enablePrinting)
		printf("Undoing the command \"%s\".\n", getHistoryCommandName(curr->command));

	move = curr->moveListLast;
	while(move){
		setCellNum(game->board, move->X, move->Y, move->old);
		if(enablePrinting)
			printf("Changed (%d,%d) from %d to %d\n", (move->X)+1, (move->Y)+1, move->new, move->old);
		move = move->prev;
	}

	game->moveHistory->curr = curr->prev;
	return 1;
}

int redoMove(Game* game){
	MoveList* move;
	CommandMoves* next;
	CommandMoves* curr = game->moveHistory->curr;
	if(!curr)
		next = game->moveHistory->FirstNode;
	else
		next = curr->next;
	if(!next)
		return 0;

	printf("Redoing the command \"%s\".\n", getHistoryCommandName(next->command));
	move = next->moveListFirst;
	while(move){
		setCellNum(game->board, move->X, move->Y, move->new);
		printf("Changed (%d,%d) from %d to %d\n", (move->X)+1, (move->Y)+1, move->old, move->new);
		move = move->next;
	}

	game->moveHistory->curr = next;
	return 1;
}


void resetMoves(Game* game){
	while(undoMove(game, 0));
}




