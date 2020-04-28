
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LPconstrains.h"
#include "game.h"
#include "solver.h"
#include "gurobi_c.h"

int oneNumberConstrain(Board *board, int *var, int *indxBoardToVar, double *coeff,
		int *index, GRBenv *env, GRBmodel *model){
	int x, y, k, N;
	int error;
	int i;
	(void)env;
	N = getBoardSize(board);
	for (y = 0; y < N; y++) {
		for (x = 0; x < N; x++) {
			i=0;
			if(getCellIsFixed(board,x,y) || getCellNum(board,x,y)!=0)
				continue;
			for(k=1;k<=N;k++){
				if(var[y*N*N+x*N+k-1] == 1){
					coeff[i] = 1.0;
					index[i] = indxBoardToVar[y*N*N+x*N+k-1];
					i++;
				}
			}
			error = GRBaddconstr(model, i, index, coeff, GRB_EQUAL, 1.0, NULL);
			if (error) {
				return 0;
			}
		}
	}
	return 1;
}

int rowConstrain(Board *board, int *var, int *indxBoardToVar, double *coeff,
		int *index, GRBenv *env, GRBmodel *model){
	int x, y, k, i, N;
	int seenElement;
	int error;
	(void)env;
	N = getBoardSize(board);
	for (y = 0; y < N; y++) {
		for (k = 1; k <= N; k++) {
			i=0;
			seenElement = 0;
			for(x=0; x<N; x++){
				if(getCellIsFixed(board,x,y) || getCellNum(board,x,y)!=0){
					seenElement = getCellNum(board,x,y) == k;
					if(seenElement)
						break;
					else
						continue;
				}
				else{
					if(var[y*N*N+x*N+k-1] == 1){
						coeff[i] = 1.0;
						index[i] = indxBoardToVar[y*N*N+x*N+k-1];
						i++;
					}
				}
			}
			if(!seenElement){
				error = GRBaddconstr(model, i, index, coeff, GRB_EQUAL, 1.0, NULL);
				if (error) {
					return 0;
				}
			}
		}
	}
	return 1;
}

int columnConstrain(Board *board, int *var, int *indxBoardToVar, double *coeff,
		int *index, GRBenv *env, GRBmodel *model){
	int x, y, k, i, N;
	int seenElement;
	int error;
	(void)env;
	N = getBoardSize(board);
	for (x = 0; x < N; x++) {
		for (k = 1; k <= N; k++) {
			i=0;
			seenElement = 0;
			for(y=0; y<N; y++){
				if(getCellIsFixed(board,x,y) || getCellNum(board,x,y)!=0){
					seenElement = getCellNum(board,x,y) == k;
					if(seenElement)
						break;
					else
						continue;
				}
				else{
					if(var[y*N*N+x*N+k-1] == 1){
						coeff[i] = 1.0;
						index[i] = indxBoardToVar[y*N*N+x*N+k-1];
						i++;
					}
				}
			}
			if(!seenElement){
				error = GRBaddconstr(model, i, index, coeff, GRB_EQUAL, 1.0, NULL);
				if (error) {
					return 0;
				}
			}
		}
	}
	return 1;
}

/* constraint #4: prevent xijk from being equal to 1 for the same k in the block. */
int blockConstrain(Board *board, int *var, int *indxBoardToVar, double *coeff,
		int *index, GRBenv *env, GRBmodel *model){

	int blockY, blockX, x, y, k, i, W, H, N;
	int seenElement;
	int error;
	(void)env;
	N = getBoardSize(board);
	W = board->blockWidth;
	H = board->blockHeight;
	for (blockY = 0; blockY < W; blockY++) {
		for (blockX = 0; blockX < H; blockX++) {
			for (k = 1; k <= N; k++) {
				i=0;
				seenElement = 0;
				for(y=blockY*H;y<(blockY+1)*H;y++){
					for(x=blockX*W;x<(blockX+1)*W;x++){
						if(getCellIsFixed(board,x,y) || getCellNum(board,x,y)!=0){
							seenElement = getCellNum(board,x,y) == k;
							if(seenElement)
								break;
							else
								continue;
						}
						else{
							if(var[y*N*N+x*N+k-1] == 1){
								coeff[i] = 1.0;
								index[i] = indxBoardToVar[y*N*N+x*N+k-1];
								i++;
							}
						}
					}
					if(seenElement)
						break;
				}
				if(!seenElement){
					error = GRBaddconstr(model, i, index, coeff, GRB_EQUAL, 1.0, NULL);
					if (error) {
						return 0;
					}
				}
			}
		}
	}
	return 1;
}
