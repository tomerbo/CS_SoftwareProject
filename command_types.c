#include <stdio.h>
#include <stdlib.h>

#include "command_types.h"

/* For a Given enum Command
 * Returns the name (char*) of the different possible enum Command for a undo/redo commands. */
char* getHistoryCommandName(enum Command command){
	switch(command)
	{
	case Set: return "Set";
	case Generate: return "Generate";
	case Autofill: return "Autofill";
	case Guess: return "Guess";
	default: return "";
	}

	return "";
}

