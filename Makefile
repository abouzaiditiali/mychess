.PHONY: clean

play: globals.h globals.c pos.h pos.c piece.h piece.c board.h board.c play.c
	clang -Wall -g -O0 -o play globals.c pos.c piece.c board.c play.c 

clean: 
	rm -rf play *.o *~ *dSYM
