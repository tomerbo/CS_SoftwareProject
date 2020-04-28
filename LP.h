#ifndef LP_H_
#define LP_H_

#include "game.h"

double *LPcalc(Board *board, int tryWhenNoSol);
Board* readBoardLP(double* sol, Board *board, double D);

#endif /* LP_H_ */
