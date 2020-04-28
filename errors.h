#ifndef ERRORS_H_
#define ERRORS_H_

/** errors **
 * This module handled recurrent errors.
 */

void error_InvalidCommand();
void error_UnavailableCommand(char *s);
void error_CommandTooLong();
void error_CommandParamMissing();
void error_CommandParamExtra();
void error_CommandParamRange();
void error_CommandParamBoardIllegal();
void error_CommandBoardNotValid();

#endif /* ERRORS_H_ */
