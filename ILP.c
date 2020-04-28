#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LPconstrains.h"
#include "game.h"
#include "solver.h"
#include "gurobi_c.h"


int calcNumOfVarILP(Board *board, int *indxBoardToVar, int *var, int *hasNoSol){
	int x,y,k;
	int num;
	int N;
	int hasValue;
	int hasValidValue;
	N=getBoardSize(board);
	num=0;
	for(y=0;y<N;y++){
		for(x=0;x<N;x++){
			hasValue = 0;
			hasValidValue = 0;
			if(getCellIsFixed(board,x,y) || getCellNum(board,x,y)!=0)
				hasValue = 1;
			for(k=1;k<=N;k++){
				if(hasValue){
					var[y*N*N+x*N+k-1] = 0;
					indxBoardToVar[y*N*N+x*N+k-1] = -1;
				}
				else{
					if(isLegalValue(board,x,y,k,0)){
						var[y*N*N+x*N+k-1] = 1;
						indxBoardToVar[y*N*N+x*N+k-1] = num;
						num++;
						hasValidValue = 1;
					}
					else{
						var[y*N*N+x*N+k-1] = 0;
						indxBoardToVar[y*N*N+x*N+k-1] = -1;
					}
				}
			}
			if(hasValue == 0 && hasValidValue == 0){
				*hasNoSol = 1;
			}
		}
	}
	return num;
}

double *returnVectorILP(Board *board,int *var, int *indxBoardToVar, double *solveVector){
	double *ret;
	int x,y,k,N;
	N=getBoardSize(board);
	ret = (double*)calloc(N*N*N,sizeof(double));
	if(!ret){
		printf("[Error] returnVectorILP has failed.\n");
		exit(0);
	}
	for(y=0;y<N;y++){
		for(x=0;x<N;x++){
			for(k=1;k<=N;k++){
				if(getCellIsFixed(board,x,y) || getCellNum(board,x,y)!=0){
					ret[y*N*N+x*N+k-1] = 0;
				}
				else{
					if(var[y*N*N+x*N+k-1] == 1){
						ret[y*N*N+x*N+k-1] = solveVector[indxBoardToVar[y*N*N+x*N+k-1]];
					}
					else
						ret[y*N*N+x*N+k-1] = 0;
				}
			}
		}
	}
	return ret;
}

void freeMemILP(int *indxBoardToVar, int *var, double *coeff, int *index, char *vtype, double *solVector){
	free(indxBoardToVar);
	free(var);
	free(coeff);
	free (index);
	free(vtype);
	free(solVector);
}

double *ILPcalc(Board *board){
	GRBenv   *env=NULL;
	GRBmodel *model=NULL;

	int *indxBoardToVar=NULL;/*index array were the value is the index of the Board cell and value in the var array*/
	int *var=NULL;/*the varibles array, have the value 1 if its a varible in the model else have the value 0*/
	int *index=NULL;/*index for the constrains*/
	double *coeff=NULL, *solVector=NULL, *retVector=NULL;
	char *vtype=NULL;
	int numOfVars, hasNoSol;
	int error;
	int optimstatus;
	int N,i;
	hasNoSol=0;
	N = getBoardSize(board);
	solVector = NULL;
	vtype = NULL;


	indxBoardToVar = (int*)calloc(N*N*N,sizeof(int));
	var = (int*)calloc(N*N*N,sizeof(int));
	coeff = (double*)calloc(N,sizeof(double));
	index = (int*)calloc(N,sizeof(int));
	if(!indxBoardToVar || !var || !coeff || !index){
		printf("[Error] ILPcalc has failed\n");
		exit(0);
	}

	numOfVars = calcNumOfVarILP(board,indxBoardToVar,var,&hasNoSol);
	if(hasNoSol){
		freeMemILP(indxBoardToVar, var, coeff, index, vtype, solVector);
		return NULL;
	}
	vtype = (char*)calloc(numOfVars,sizeof(char));
	solVector = (double*)calloc(numOfVars,sizeof(double));
	if(!vtype || !solVector){
		printf("[Error] ILPcalc has failed\n");
		exit(0);
	}

	if(hasNoSol){
		freeMemILP(indxBoardToVar, var, coeff, index, vtype, solVector);
		return NULL;
	}
	/* Create environment - log file is ILP.log */
	error = GRBloadenv(&env, "ILP.log");
	if (error) {
		freeMemILP(indxBoardToVar, var, coeff, index, vtype, solVector);
		GRBfreeenv(env);
		return NULL;
	}

	error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
	if (error) {
		freeMemILP(indxBoardToVar, var, coeff, index, vtype, solVector);
		GRBfreeenv(env);
		return NULL;
	}

	for(i = 0;i<numOfVars;i++){
		vtype[i] = GRB_BINARY;
	}

	error = GRBnewmodel(env, &model, "ILP", numOfVars, NULL, NULL, NULL, vtype, NULL);
	if (error) {
		freeMemILP(indxBoardToVar, var, coeff, index, vtype, solVector);
		GRBfreemodel(model); GRBfreeenv(env);
		return NULL;
	}

	/* constraint #1: prevent xijk from being equal to 1 for two or more distinct values k at the same cell. */
	error = oneNumberConstrain(board,var,indxBoardToVar,coeff,index,env,model);
	if(!error){
		freeMemILP(indxBoardToVar, var, coeff, index, vtype, solVector);
		GRBfreemodel(model); GRBfreeenv(env);
		return NULL;
	}
	error = rowConstrain(board,var,indxBoardToVar,coeff,index,env,model);
	if(!error){
		freeMemILP(indxBoardToVar, var, coeff, index, vtype, solVector);
		GRBfreemodel(model); GRBfreeenv(env);
		return NULL;
	}
	error = columnConstrain(board,var,indxBoardToVar,coeff,index,env,model);
	if(!error){
		freeMemILP(indxBoardToVar, var, coeff, index, vtype, solVector);
		GRBfreemodel(model); GRBfreeenv(env);
		return NULL;
	}
	error = blockConstrain(board,var,indxBoardToVar,coeff,index,env,model);
	if(!error){
		freeMemILP(indxBoardToVar, var, coeff, index, vtype, solVector);
		GRBfreemodel(model); GRBfreeenv(env);
		return NULL;
	}
	error = GRBupdatemodel(model);
	if(error){
		freeMemILP(indxBoardToVar, var, coeff, index, vtype, solVector);
		GRBfreemodel(model); GRBfreeenv(env);
		return NULL;
	}
	/* Write model to 'ILP.lp' - this is not necessary but very helpful */
	error = GRBwrite(model, "ILP.lp");
	if (error) {
		freeMemILP(indxBoardToVar, var, coeff, index, vtype, solVector);
		GRBfreemodel(model); GRBfreeenv(env);
		return NULL;
	}

	/* Optimize model - need to call this before calculation */
	error = GRBoptimize(model);
	if (error){
		freeMemILP(indxBoardToVar, var, coeff, index, vtype, solVector);
		GRBfreemodel(model); GRBfreeenv(env);
		return NULL;
	}

	/* Get solution information */
	error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
	if (error) {
		freeMemILP(indxBoardToVar, var, coeff, index, vtype, solVector);
		GRBfreemodel(model); GRBfreeenv(env);
		return NULL;
	}

	/* get the solution - the assignment to each variable */
	error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, numOfVars, solVector);
	if (error) {
		freeMemILP(indxBoardToVar, var, coeff, index, vtype, solVector);
		GRBfreemodel(model); GRBfreeenv(env);
		return NULL;
	}

	/* solution found */
	if(optimstatus == GRB_OPTIMAL){
		retVector = returnVectorILP(board,var,indxBoardToVar,solVector);
		freeMemILP(indxBoardToVar, var, coeff, index, vtype, solVector);
		GRBfreemodel(model); GRBfreeenv(env);
		return retVector;
	}

	/* no solution found */
	else if(optimstatus == GRB_INF_OR_UNBD){
		freeMemILP(indxBoardToVar, var, coeff, index, vtype, solVector);
		GRBfreemodel(model); GRBfreeenv(env);
		return NULL;
	}

	/* error or calculation stopped */
	else{
		freeMemILP(indxBoardToVar, var, coeff, index, vtype, solVector);
		GRBfreemodel(model); GRBfreeenv(env);
		return NULL;
	}
}


Board* readBoardILP(double* sol, Board* enteredBoard){
	int X,Y,k, N;
	Board* board = createEmptyBoard(getBlockWidth(enteredBoard), getBlockHeight(enteredBoard));
	deepCloneBoard(enteredBoard, board);
	N = getBoardSize(board);

	for(X=0; X<N; ++X){
		for(Y=0; Y<N; ++Y){
			for(k=0; k<N; ++k){
				if(sol[Y*N*N+X*N+k]==1)
					setCellNum(board, X, Y, k+1);
				continue;
			}
		}
	}

	return board;
}


