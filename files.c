#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"



char* readWord(FILE *fp, int *wordLen){
	char *wordn;
	char *word = malloc(25), *wordp = word;
	size_t lenmax = 25, len = lenmax;
	int realLen = 0;
	int c;
	int isFirstSpaces = 1;

	if(word == NULL){
		printf("[Error] readWord has failed\n");
		exit(0);
	}

	for(;;) {
		c = fgetc(fp);
		if(c == EOF)
			break;

		if(--len == 0) {
			len = lenmax;
			wordn = realloc(wordp, lenmax *= 2);

			if(wordn == NULL) {
				printf("[Error] readWord has failed\n");
				exit(0);
			}
			word = wordn + (word - wordp);
			wordp = wordn;
		}

		if((c==' ' || c =='\n' || c =='\t' || c =='\r') && isFirstSpaces){

		}
		else{
			isFirstSpaces = 0;
			*word = c;
			if(*word==' ' || *word =='\n' || *word =='\t' || *word =='\r'){
				break;
			}
			realLen++;
			word++;
		}
	}
	*word = '\0';
	*wordLen = realLen;
	return wordp;
}


int isdig(char c){
	if(c >= '0' && c <= '9')
		return 1;
	else
		return 0;
}

int readNumAndIsfixed(int* num, int* isFixed, FILE *fp){
	int wordLen;
	char* word = readWord(fp, &wordLen);

	if(wordLen>3 || wordLen==0){
		free(word);
		return 0;
	}
	else if(wordLen == 1 && isdig(word[0])){
		*num = atoi(word);
		*isFixed = 0;
		free(word);
		return 1;
	}
	else if(wordLen == 2 && isdig(word[0]) && isdig(word[1])){
		*num = atoi(word);
		*isFixed = 0;
		free(word);
		return 1;
	}
	else if(wordLen == 2 && isdig(word[0]) && word[1]=='.'){
		*num = atoi(word);
		*isFixed = 1;
		free(word);
		return 1;
	}
	else if(wordLen == 3 && isdig(word[0]) && isdig(word[1]) && word[2]=='.'){
		*num = atoi(word);
		*isFixed = 1;
		free(word);
		return 1;
	}

	free(word);
	return 0;
}

int isMoreWords(FILE* fp){
	int wordLen;
	char* word = readWord(fp, &wordLen);
	if(wordLen == 0){
		free(word);
		return 0;
	}
	else{
		free(word);
		return 1;
	}
}

/*
 * @param Recives a pointer to a board(Board *board), a file path to read from and the current game mode.
 *
 * Reads the board from the file, if possible.
 *
 * @return 1 for success and 0 for failure.
 */
int readBoard(Board** board, char *file, int isEditMode){
	char* word;
	int wordLen;
	int num=0, isFixed, m, n, y, x;
	FILE *fp = fopen(file, "r");
	if (fp == NULL) {
		printf("[Error] Could not read the specified file.\n");
		return 0;
	}


	/* read m and n */
	word = readWord(fp, &wordLen);
	if(wordLen == 0){
		printf("[Error] The file does not have any words.\n");
		free(word);
		if (fclose(fp) == EOF) {
			printf("[Error] Could not close file.\n");
			return 0;
		}
		return 0;
	}
	if((num = atoi(word))>0){
		m = num;
		free(word);
	}
	else if(strcmp(word,"0")==0){
		printf("[Error] m cannot be equal to zero.\n");
		free(word);
		if (fclose(fp) == EOF) {
			printf("[Error] Could not close file.\n");
			return 0;
		}
		return 0;
	}
	else if(num<0){
		printf("[Error] m cannot be a negative number.\n");
		free(word);
		if (fclose(fp) == EOF) {
			printf("[Error] Could not close file.\n");
			return 0;
		}
		return 0;
	}
	else{
		printf("[Error] file first word is not a number.\n");
		free(word);
		if (fclose(fp) == EOF) {
			printf("[Error] Could not close file.\n");
			return 0;
		}
		return 0;
	}

	word = readWord(fp, &wordLen);
	if(wordLen == 0){
		printf("[Error] The file only has one word.\n");
		free(word);
		if (fclose(fp) == EOF) {
			printf("[Error] Could not close file.\n");
			return 0;
		}
		return 0;
	}
	if((num = atoi(word))>0){
		n = num;
		free(word);
	}
	else if(strcmp(word,"0")==0){
		printf("[Error] n cannot be equal to zero.\n");
		free(word);
		if (fclose(fp) == EOF) {
			printf("[Error] Could not close file.\n");
			return 0;
		}
		return 0;
	}
	else if(num<0){
		printf("[Error] n cannot be a negative number.\n");
		free(word);
		if (fclose(fp) == EOF) {
			printf("[Error] Could not close file.\n");
			return 0;
		}
		return 0;
	}
	else{
		printf("[Error] file second word is not a number.\n");
		free(word);
		if (fclose(fp) == EOF) {
			printf("[Error] Could not close file.\n");
			return 0;
		}
		return 0;
	}

	/* if m*n>99 then there is an error */
	if(n*m>99){
		printf("[Error] the numbers in the board cannot be larger than 3 digits. Therefore n*m has to be less than 100.\n");
		if (fclose(fp) == EOF) {
			printf("[Error] Could not close file.\n");
			return 0;
		}
		return 0;
	}

	/* create new board and fill it */
	*board = createEmptyBoard(n, m);

	for(y=0; y<n*m; ++y){
		for(x=0; x<n*m; ++x){

			if(readNumAndIsfixed(&num, &isFixed, fp)){
				if(num<0 || num>n*m){
					printf("[Error] The file has a number which is not in the valid range for the board.\n");
					destroyBoard(*board);
					if (fclose(fp) == EOF) {
						printf("[Error] Could not close file.\n");
						return 0;
					}
					return 0;
				}

				if(num==0 && isFixed){
					printf("[Error] The file has a number that indicates on an empty cell which is a fixed cell. This is not possible.\n");
					destroyBoard(*board);
					if (fclose(fp) == EOF) {
						printf("[Error] Could not close file.\n");
						return 0;
					}
					return 0;
				}

				setCellNum(*board, x, y, num);
				if(isEditMode)
					(*board)->board[x][y].isFixed = 0;
				else
					(*board)->board[x][y].isFixed = isFixed;
			}
			else{
				printf("[Error] The file has a missing valid number.\n");
				destroyBoard(*board);
				if (fclose(fp) == EOF) {
					printf("[Error] Could not close file.\n");
					return 0;
				}
				return 0;
			}
		}
	}

	if(isMoreWords(fp)){
		printf("[Error] The file is too long for the requested board size.\n");
		destroyBoard(*board);
		if (fclose(fp) == EOF) {
			printf("[Error] Could not close file.\n");
			return 0;
		}
		return 0;
	}

	if (fclose(fp) == EOF) {
		printf("[Error] Could not close file.\n");
		return 0;
	}

	return 1;
}


/*
 * @param Recives a valid board, a file path and the current game mode.
 *
 * Saves the board to the file, if possible.
 *
 * @return 1 for success and 0 for failure.
 */
int saveBoard(Board* board, char *file, int isEditMode){
	int y, x, num, isFixed;
	FILE *fp = fopen(file, "w");
	if (fp == NULL) {
		printf("[Error] Could not write to the specified file.\n");
		return 0;
	}

	/* Write "m n" to the file */
	if(fprintf(fp, "%d %d\n", getBlockHeight(board), getBlockWidth(board))<0){
		printf("[Error] Could not write to the specified file.\n");
		if (fclose(fp) == EOF) {
			printf("[Error] Could not close file.\n");
			return 0;
		}
		return 0;
	}

	for(y=0; y<getBoardSize(board); ++y){
		for(x=0; x<getBoardSize(board); ++x){
			num = getCellNum(board, x, y);
			isFixed = getCellIsFixed(board, x, y);
			if(isEditMode){
				if(x == getBoardSize(board)-1){
					if(num == 0){
						if(fprintf(fp, "0")<0){
							printf("[Error] Could not write to the specified file.\n");
							if (fclose(fp) == EOF) {
								printf("[Error] Could not close file.\n");
								return 0;
							}
							return 0;
						}
					}
					else{
						if(fprintf(fp, "%d.", num)<0){
							printf("[Error] Could not write to the specified file.\n");
							if (fclose(fp) == EOF) {
								printf("[Error] Could not close file.\n");
								return 0;
							}
							return 0;
						}
					}
				}
				else{
					if(num == 0){
						if(fprintf(fp, "0 ")<0){
							printf("[Error] Could not write to the specified file.\n");
							if (fclose(fp) == EOF) {
								printf("[Error] Could not close file.\n");
								return 0;
							}
							return 0;
						}
					}
					else{
						if(fprintf(fp, "%d. ", num)<0){
							printf("[Error] Could not write to the specified file.\n");
							if (fclose(fp) == EOF) {
								printf("[Error] Could not close file.\n");
								return 0;
							}
							return 0;
						}
					}
				}
			}
			else{
				if(x == getBoardSize(board)-1){
					if(num == 0){
						if(fprintf(fp, "0")<0){
							printf("[Error] Could not write to the specified file.\n");
							if (fclose(fp) == EOF) {
								printf("[Error] Could not close file.\n");
								return 0;
							}
							return 0;
						}
					}
					else if(isFixed){
						if(fprintf(fp, "%d.", num)<0){
							printf("[Error] Could not write to the specified file.\n");
							if (fclose(fp) == EOF) {
								printf("[Error] Could not close file.\n");
								return 0;
							}
							return 0;
						}
					}
					else{
						if(fprintf(fp, "%d", num)<0){
							printf("[Error] Could not write to the specified file.\n");
							if (fclose(fp) == EOF) {
								printf("[Error] Could not close file.\n");
								return 0;
							}
							return 0;
						}
					}
				}
				else{
					if(num == 0){
						if(fprintf(fp, "0 ")<0){
							printf("[Error] Could not write to the specified file.\n");
							if (fclose(fp) == EOF) {
								printf("[Error] Could not close file.\n");
								return 0;
							}
							return 0;
						}
					}
					else if(isFixed){
						if(fprintf(fp, "%d. ", num)<0){
							printf("[Error] Could not write to the specified file.\n");
							if (fclose(fp) == EOF) {
								printf("[Error] Could not close file.\n");
								return 0;
							}
							return 0;
						}
					}
					else{
						if(fprintf(fp, "%d ", num)<0){
							printf("[Error] Could not write to the specified file.\n");
							if (fclose(fp) == EOF) {
								printf("[Error] Could not close file.\n");
								return 0;
							}
							return 0;
						}
					}
				}
			}
		}
		if(fprintf(fp, "\n")<0){
			printf("[Error] Could not write to the specified file.\n");
			if (fclose(fp) == EOF) {
				printf("[Error] Could not close file.\n");
				return 0;
			}
			return 0;
		}
	}

	if (fclose(fp) == EOF) {
		printf("[Error] Could not close file.\n");
		return 0;
	}

	return 1;
}


