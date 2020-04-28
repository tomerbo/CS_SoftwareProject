#ifndef ILP_H_
#define ILP_H_

#include "game.h"

double *ILPcalc(Board *board);
Board* readBoardILP(double* sol, Board* enteredBoard);

#endif /* ILP_H_ */
