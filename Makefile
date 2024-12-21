.PHONY: clean

play: globals.h globals.c utils.h utils.c pos.h pos.c piece.h piece.c board.h board.c play.c
	clang -Wall -g -O0 -o play globals.c utils.c pos.c piece.c board.c play.c 

clean: 
	rm -rf play *.o *~ *dSYM
