#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LPconstrains.h"
#include "game.h"
#include "solver.h"
#include "gurobi_c.h"


int calcNumOfVarLP(Board *board, int *indxBoardToVar, int *var, int *hasNoSol){
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

double* returnVectorLP(Board *board,int *var, int *indxBoardToVar, double *solveVector){
	double *ret;
	int x,y,k,N;
	N=getBoardSize(board);
	ret = (double*)calloc(N*N*N,sizeof(double));
	if(!ret){
		printf("[Error] retVector has failed.\n");
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

void freeMemLP(int *indxBoardToVar, int *var, double *coeff, int *index, double *obj, char *vtype, double *solVector,double *ub){
	free(indxBoardToVar);
	free(var);
	free(coeff);
	free (index);
	free(obj);
	free(vtype);
	free(solVector);
	free(ub);
}

double *LPcalc(Board *board, int tryWhenNoSol){
	GRBenv   *env=NULL;
	GRBmodel *model=NULL;

	int *indxBoardToVar=NULL;/*index array were the value is the index of the Board cell and value in the var array*/
	int *var=NULL;/*the varibles array, have the value 1 if its a varible in the model else have the value 0*/
	int *index=NULL;/*index for the constrains*/
	double *coeff=NULL, *solVector=NULL, *obj=NULL, *retVector=NULL, *ub=NULL;
	char *vtype=NULL;
	int numOfVars, hasNoSol;
	int error;
	int optimstatus;
	int N,i;
	hasNoSol=0;
	N = getBoardSize(board);

	indxBoardToVar = (int*)calloc(N*N*N,sizeof(int));
	var = (int*)calloc(N*N*N,sizeof(int));
	coeff = (double*)calloc(N,sizeof(double));
	index = (int*)calloc(N,sizeof(int));
	if(!indxBoardToVar || !var || !coeff || !index){
		printf("[Error] LPcalc has failed\n");
		exit(0);
	}

	numOfVars = calcNumOfVarLP(board,indxBoardToVar,var,&hasNoSol);
	if(hasNoSol && tryWhenNoSol==0){
		freeMemLP(indxBoardToVar,var,coeff,index,obj,vtype,solVector,ub);
		return NULL;
	}

	obj = (double*)calloc(numOfVars,sizeof(double));
	vtype = (char*)calloc(numOfVars,sizeof(char));
	solVector = (double*)calloc(numOfVars,sizeof(double));
	ub = (double*)calloc(numOfVars, sizeof(double));
	if(!vtype || !solVector || !ub || !obj){
		printf("[Error] LPcalc has failed\n");
		exit(0);
	}

	/* Create environment - log file is LP.log */
	error = GRBloadenv(&env, "LP.log");
	if (error) {
		freeMemLP(indxBoardToVar,var,coeff,index,obj,vtype,solVector,ub);
		GRBfreeenv(env);
		return NULL;
	}

	error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
	if (error) {
		freeMemLP(indxBoardToVar,var,coeff,index,obj,vtype,solVector,ub);
		GRBfreeenv(env);
		return NULL;
	}

	for(i = 0;i<numOfVars;i++){
		vtype[i] = GRB_CONTINUOUS;
		obj[i] = (double)(rand()%(getBoardSize(board)*4) + 1);
		ub[i] = 1.0;
	}
	error = GRBnewmodel(env, &model, "LP", numOfVars, obj, NULL, ub, vtype, NULL);
	if (error) {
		freeMemLP(indxBoardToVar,var,coeff,index,obj,vtype,solVector,ub);
		GRBfreemodel(model); GRBfreeenv(env);
		return NULL;
	}

	error = oneNumberConstrain(board,var,indxBoardToVar,coeff,index,env,model);
	if(!error){
		freeMemLP(indxBoardToVar,var,coeff,index,obj,vtype,solVector,ub);
		GRBfreemodel(model); GRBfreeenv(env);
		return NULL;
	}
	error = rowConstrain(board,var,indxBoardToVar,coeff,index,env,model);
	if(!error){
		freeMemLP(indxBoardToVar,var,coeff,index,obj,vtype,solVector,ub);
		GRBfreemodel(model); GRBfreeenv(env);
		return NULL;
	}
	error = columnConstrain(board,var,indxBoardToVar,coeff,index,env,model);
	if(!error){
		freeMemLP(indxBoardToVar,var,coeff,index,obj,vtype,solVector,ub);
		GRBfreemodel(model); GRBfreeenv(env);
		return NULL;
	}
	error = blockConstrain(board,var,indxBoardToVar,coeff,index,env,model);
	if(!error){
		freeMemLP(indxBoardToVar,var,coeff,index,obj,vtype,solVector,ub);
		GRBfreemodel(model); GRBfreeenv(env);
		return NULL;
	}
	error = GRBupdatemodel(model);
	if(error){
		freeMemLP(indxBoardToVar,var,coeff,index,obj,vtype,solVector,ub);
		GRBfreemodel(model); GRBfreeenv(env);
		return NULL;
	}
	/* Write model to 'LP.lp' - this is not necessary but very helpful */
	error = GRBwrite(model, "LP.lp");
	if (error) {
		freeMemLP(indxBoardToVar,var,coeff,index,obj,vtype,solVector,ub);
		GRBfreemodel(model); GRBfreeenv(env);
		return NULL;
	}

	/* Optimize model - need to call this before calculation */
	error = GRBoptimize(model);
	if (error){
		freeMemLP(indxBoardToVar,var,coeff,index,obj,vtype,solVector,ub);
		GRBfreemodel(model); GRBfreeenv(env);
		return NULL;
	}

	/* Get solution information */
	error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
	if (error) {
		freeMemLP(indxBoardToVar,var,coeff,index,obj,vtype,solVector,ub);
		GRBfreemodel(model); GRBfreeenv(env);
		return NULL;
	}

	/* get the solution - the assignment to each variable */
	error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, numOfVars, solVector);
	if (error) {
		freeMemLP(indxBoardToVar,var,coeff,index,obj,vtype,solVector,ub);
		GRBfreemodel(model); GRBfreeenv(env);
		return NULL;
	}

	/* solution found */
	if(optimstatus == GRB_OPTIMAL){
		retVector = returnVectorLP(board,var,indxBoardToVar,solVector);
		freeMemLP(indxBoardToVar,var,coeff,index,obj,vtype,solVector,ub);
		GRBfreemodel(model); GRBfreeenv(env);
		return retVector;
	}

	/* no solution found */
	else if(optimstatus == GRB_INF_OR_UNBD){
		freeMemLP(indxBoardToVar,var,coeff,index,obj,vtype,solVector,ub);
		GRBfreemodel(model); GRBfreeenv(env);
		return NULL;
	}

	/* error or calculation stopped */
	else{
		freeMemLP(indxBoardToVar,var,coeff,index,obj,vtype,solVector,ub);
		GRBfreemodel(model); GRBfreeenv(env);
		return NULL;
	}
}





int getValidRandomValue(double* sol, Board *board, int X, int Y, double D){
	double *scores;
	int *values;
	double randScore;
	int k, i, N, validCounter, randValue=0;
	N = getBoardSize(board);

	validCounter = 0;
	for(k=0; k<N; ++k){
		if(sol[Y*N*N+X*N+k]>=D && sol[Y*N*N+X*N+k]!=0 && isLegalValue(board, X, Y, k+1, 0)){
			validCounter += 1;
		}
	}

	if(validCounter==0)
		return 0;
	if(validCounter==1){
		for(k=0; k<N; ++k){
			if(sol[Y*N*N+X*N+k]>=D && sol[Y*N*N+X*N+k]!=0 && isLegalValue(board, X, Y, k+1, 0)){
				return k+1;
			}
		}
	}


	scores = (double*)calloc(validCounter, sizeof(double));
	values = (int*)calloc(validCounter, sizeof(int));
	if(!scores || !values){
		printf("[Error] getValidRandomValue has failed\n");
		exit(0);
	}

	i=0;
	for(k=0; k<N; ++k){
		if(sol[Y*N*N+X*N+k]>=D && sol[Y*N*N+X*N+k]!=0 && isLegalValue(board, X, Y, k+1, 0)){
			if(i==0){
				scores[i] = sol[Y*N*N+X*N+k];
				values[i] = k+1;
			}
			else{
				scores[i] = scores[i-1] + sol[Y*N*N+X*N+k];
				values[i] = k+1;
			}
			i += 1;
		}
	}

	randScore = ((double)rand() / (double)RAND_MAX) * scores[validCounter-1];

	for(i=0; i<validCounter; ++i){
		if(i==0){
			if(randScore <= scores[i]){
				randValue = values[i];
				break;
			}
		}
		else{
			if(randScore > scores[i-1] && randScore <= scores[i]){
				randValue = values[i];
				break;
			}
		}
	}

	free(scores);
	return randValue;

}

Board* readBoardLP(double* sol, Board *enteredBoard, double D){
	int X,Y, N;
	Board* board = createEmptyBoard(getBlockWidth(enteredBoard), getBlockHeight(enteredBoard));
	deepCloneBoard(enteredBoard, board);
	N = getBoardSize(board);

	for(X=0; X<N; ++X){
		for(Y=0; Y<N; ++Y){
			if(getCellNum(board, X, Y) == 0)
				setCellNum(board, X, Y, getValidRandomValue(sol, board, X, Y, D));
		}
	}

	return board;
}

