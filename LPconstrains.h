#ifndef LPCONSTRAINS_H_
#define LPCONSTRAINS_H_

#include "game.h"
#include "gurobi_c.h"

int oneNumberConstrain(Board *board, int *var, int *indxBoardToVar, double *coeff,
		int *index, GRBenv *env, GRBmodel *model);
int rowConstrain(Board *board, int *var, int *indxBoardToVar, double *coeff,
		int *index, GRBenv *env, GRBmodel *model);
int columnConstrain(Board *board, int *var, int *indxBoardToVar, double *coeff,
		int *index, GRBenv *env, GRBmodel *model);
int blockConstrain(Board *board, int *var, int *indxBoardToVar, double *coeff,
		int *index, GRBenv *env, GRBmodel *model);

#endif /* LPCONSTRAINS_H_ */
