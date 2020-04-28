CC = gcc
OBJS = main.o main_aux.o game.o solver.o errors.o commands_analyze.o parser.o files.o commands.o command_types.o history.o LP.o ILP.o LPconstrains.o
EXEC = sudoku-console
COMP_FLAG = -ansi -Wall -Wextra -Werror -pedantic-errors

GUROBI_LIB = -L/usr/local/lib/gurobi563/lib -lgurobi56
GUROBI_COMP = -I/usr/local/lib/gurobi563/include

all:$(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(GUROBI_LIB) -o $@
main.o: main.c main_aux.h game.h SPBufferset.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
main_aux.o: main_aux.c main_aux.h game.h parser.h errors.h commands.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
game.o: game.c game.h main_aux.h parser.h solver.h history.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
solver.o: solver.c solver.h game.h LP.h ILP.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
errors.o: errors.c errors.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
commands_analyze.o: commands_analyze.c commands_analyze.h errors.h parser.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
parser.o: parser.c parser.h errors.h commands_analyze.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
files.o: files.c files.h game.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
commands.o: commands.c commands.h command_types.h game.h files.h history.h solver.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
command_types.o: command_types.c command_types.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
history.o: history.c history.h command_types.h game.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
LP.o: LP.c LP.h LPconstrains.h game.h solver.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
ILP.o: ILP.c ILP.h LPconstrains.h game.h solver.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
LPconstrains.o: LPconstrains.c LPconstrains.h game.h solver.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
clean:
	rm -f $(OBJS) $(EXEC)
