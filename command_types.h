#ifndef COMMAND_TYPES_H_
#define COMMAND_TYPES_H_

/* A list of all possible commands.
 * _OP is added to command types that return with an optional parameter
 */
enum Command {SolveC, EditC, EditC_OP, Mark_Errors, Print_Board, Set, Validate,
	Guess, Generate, Undo, Redo, Save, Hint, Guess_Hint, Num_Solutions, Autofill, Reset, Exit, None, Ignore};

/* For a Given enum Command
 * Returns the name (char*) of the different possible enum Command for a undo/redo commands. */
char* getHistoryCommandName(enum Command command);

#endif /* COMMAND_TYPES_H_ */
