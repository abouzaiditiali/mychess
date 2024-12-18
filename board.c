#include "board.h"

//Helper
void set_standard_position(Board* board) {
    unsigned char size = board->size;
    for (unsigned char c = 0; c < size; c++) {
        Piece* black_pawn = piece_new(PAWN, BLACK_SIDE, make_pos(1, c));
        set_pawn_transformation(black_pawn, MOVING_DOWN);
        board_set(board, make_pos(1, c), PIECE, black_pawn);

        Piece* white_pawn = piece_new(PAWN, WHITE_SIDE, make_pos(6, c));
        set_pawn_transformation(white_pawn, MOVING_UP);
        board_set(board, make_pos(6, c), PIECE, white_pawn);
    }
    Piece* black_rookl = piece_new(ROOK, BLACK_SIDE, make_pos(0, 0));           
    board_set(board, make_pos(0, 0), PIECE, black_rookl);
    Piece* black_rookr = piece_new(ROOK, BLACK_SIDE, make_pos(0, 7));           
    board_set(board, make_pos(0, 7), PIECE, black_rookr);
    Piece* black_knightl = piece_new(KNIGHT, BLACK_SIDE, make_pos(0, 1));        
    board_set(board, make_pos(0, 1), PIECE, black_knightl);
    Piece* black_knightr = piece_new(KNIGHT, BLACK_SIDE, make_pos(0, 6));       
    board_set(board, make_pos(0, 6), PIECE, black_knightr);
    Piece* black_bishopl = piece_new(BISHOP, BLACK_SIDE, make_pos(0, 2));       
    board_set(board, make_pos(0, 2), PIECE, black_bishopl);
    Piece* black_bishopr = piece_new(BISHOP, BLACK_SIDE, make_pos(0, 5));       
    board_set(board, make_pos(0, 5), PIECE, black_bishopr);
    Piece* black_queen = piece_new(QUEEN, BLACK_SIDE, make_pos(0, 3));          
    board_set(board, make_pos(0, 3), PIECE, black_queen);
    Piece* black_king = piece_new(KING, BLACK_SIDE, make_pos(0, 4));            
    board_set(board, make_pos(0, 4), PIECE, black_king);
    
    Piece* white_rookl = piece_new(ROOK, WHITE_SIDE, make_pos(7, 0));           
    board_set(board, make_pos(7, 0), PIECE, white_rookl);
    Piece* white_rookr = piece_new(ROOK, WHITE_SIDE, make_pos(7, 7));           
    board_set(board, make_pos(7, 7), PIECE, white_rookr);
    Piece* white_knightl = piece_new(KNIGHT, WHITE_SIDE, make_pos(7, 1));       
    board_set(board, make_pos(7, 1), PIECE, white_knightl);
    Piece* white_knightr = piece_new(KNIGHT, WHITE_SIDE, make_pos(7, 6));       
    board_set(board, make_pos(7, 6), PIECE, white_knightr);
    Piece* white_bishopl = piece_new(BISHOP, WHITE_SIDE, make_pos(7, 2));       
    board_set(board, make_pos(7, 2), PIECE, white_bishopl);
    Piece* white_bishopr = piece_new(BISHOP, WHITE_SIDE, make_pos(7, 5));       
    board_set(board, make_pos(7, 5), PIECE, white_bishopr);
    Piece* white_queen = piece_new(QUEEN, WHITE_SIDE, make_pos(7, 3));          
    board_set(board, make_pos(7, 3), PIECE, white_queen);
    Piece* white_king = piece_new(KING, WHITE_SIDE, make_pos(7, 4));            
    board_set(board, make_pos(7, 4), PIECE, white_king);
}

Cell* cell_new(cell_type type, Piece* piece) {
    Cell* res = (Cell*)malloc(sizeof(Cell));
    res->type = type;
    res->piece = piece;
    return res;
}

void cell_free(Cell* cell) {
    if (cell->piece) {
        piece_free(cell->piece);
    }
    free(cell);
}

Board* board_new(move_direction direction) {
    unsigned char size = 8;
    Board* board = (Board*)malloc(sizeof(Board));
    Cell*** matrix = (Cell***)malloc(sizeof(Cell**) * size);
    for (unsigned char r = 0; r < size; r++) {
        matrix[r] = (Cell**)malloc(sizeof(Cell*) * size);
        for (unsigned char c = 0; c < size; c++) {
            matrix[r][c] = cell_new(EMPTY, NULL);
        }
    }
    board->matrix = matrix;
    board->size = size;
    board->direction = direction;
    set_standard_position(board);
//    if (direction == MOVING_DOWN) {
//        board_flip(board);
//    }
    return board;
}

void board_free(Board* board) {
    unsigned char size = board->size;
    for (unsigned char r = 0; r < size; r++) {
        for (unsigned char c = 0; c < size; c++) {
            cell_free(board_get(board, make_pos(r, c)));
        }
        free(board->matrix[r]);
    }
    free(board->matrix);
}

//Helper 
void print_chars(move_direction direction) {
    printf("  ");
    if (direction == MOVING_UP) {
        for (char c = 'a'; c <= 'h'; c++) {
            printf("   %c", c);
        }
    } else {
        for (char c = 'h'; c >= 'a'; c--) {
            printf("   %c", c);
        }
    }
    printf("\n");
}

//Helper
char maybe_upp(piece_side side, char c) {
    if (side == WHITE_SIDE) {
        char res = c - ('a' - 'A');
        return res;
    }
    return c;
}

//Helper
char convert_cell_to_char(Cell* cell) {
    char res;
    if (cell->type == EMPTY) {
        res = '.';
        return res;
    }
    switch (cell->piece->kind) {
        case PAWN:
            return maybe_upp(cell->piece->side, 'p');
        case KNIGHT:
            return maybe_upp(cell->piece->side, 'n');
        case BISHOP:
            return maybe_upp(cell->piece->side, 'b');
        case ROOK:
            return maybe_upp(cell->piece->side, 'r');
        case QUEEN:
            return maybe_upp(cell->piece->side, 'q');
        case KING:
            return maybe_upp(cell->piece->side, 'k');
    }
}

void board_show(Board* board) {
    printf("\n");
    print_chars(board->direction);
    unsigned char mu = 8, md = 1, curr = 1;
    for (unsigned char i = 0; i < 17; i++) {
        if (i % 2 == 0) {
            printf("   ");
            for (unsigned char j = 0; j < 8; j++) {
                printf(" ---");
            }
        } else {
            printf(" ");
            if (board->direction == MOVING_UP) {
                printf("%hhu", mu);
            } else {
                printf("%hhu", md);
            }
            printf(" |");
            for (unsigned char j = 0; j < 8; j++) {
                Cell* cell = board_get(board, make_pos(i - curr, j));
                char c = convert_cell_to_char(cell);
                printf(" %c |", c);
            }
            curr++;
            printf(" ");
            if (board->direction == MOVING_UP) {
                printf("%hhu", mu--);
            } else {
                printf("%hhu", md++);
            }
        }
        printf("\n"); 
    }
    print_chars(board->direction);
    printf("\n");
}
 
Cell* board_get(Board* board, Pos pos) {
    Cell* res = board->matrix[pos.r][pos.c];
    return res;
}

void board_set(Board* board, Pos pos, cell_type type, Piece* piece) {
    Cell* cell = board_get(board, pos);
    cell->type = type;
    cell->piece = piece;
    if (cell->piece) {
        cell->piece->position = pos;
    }
}
            























