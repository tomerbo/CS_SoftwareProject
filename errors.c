#include <stdio.h>
#include <stdlib.h>

void error_InvalidCommand(){
	printf("[Error] invalid command.\n");
}

void error_UnavailableCommand(char *s){
	printf("[Error] invalid command. This command is not available at the current mode.\n");
	printf("This command is only available for the following modes: %s", s);
	printf(".\n");
}

void error_CommandTooLong(){
	printf("[Error] invalid command. Too many characters were entered in a single line.\n");
}

void error_CommandParamMissing(){
	printf("[Error] number of parameters entered for the command is not enough.\n");
}

void error_CommandParamExtra(){
	printf("[Error] number of parameters entered for the command is too much.\n");
}

void error_CommandParamRange(){
	printf("[Error] command parameters values are not in the range of possible input for that command.\n");
}

void error_CommandParamBoardIllegal(){
	printf("[Error] command parameters values are illegal for the current board.\n");
}

void error_CommandBoardNotValid(){
	printf("[Error] the board is not valid for that command.\n");
}
