.PHONY: clean

play: globals.h globals.c utils.h utils.c pos.h pos.c piece.h piece.c \
      translation.h translation.c board.h board.c move.h move.c \
      logic.h logic.c play.c
	clang -Wall -g -O0 -o play \
	globals.c utils.c pos.c piece.c translation.c board.c move.c logic.c \
	play.c 

test: globals.h globals.c utils.h utils.c pos.h pos.c piece.h piece.c \
      translation.h translation.c board.h board.c move.h move.c \
      logic.h logic.c test.c
	clang -Wall -g -O0 -o test \
	globals.c utils.c pos.c piece.c translation.c board.c move.c logic.c \
	test.c 

tui: globals.h globals.c utils.h utils.c pos.h pos.c piece.h piece.c \
      translation.h translation.c board.h board.c move.h move.c \
      logic.h logic.c tui.c
	clang -Wall -g -O0 -o tui \
	globals.c utils.c pos.c piece.c translation.c board.c move.c logic.c \
	tui.c -lncurses

clean: 
	rm -rf play test tui *.o *~ *dSYM
