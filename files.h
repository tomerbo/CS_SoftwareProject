#ifndef FILES_H_
#define FILES_H_

/** files **
 * This module handles all the interaction with files.
 *
 * readBoard - Reads the board from the file, if possible.
 * 			   handles all errors of reading from a file and checking for a valid board format.
 * saveBoard - Saves the board to the file, if possible.
 * 			   handles all errors of writing to a file.
 *
 */

int readBoard(Board** board, char *file, int isEditMode);
int saveBoard(Board* board, char *file, int isEditMode);

#endif /* FILES_H_ */
