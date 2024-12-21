#include "board.h"

//Helper
void set_standard_position(Board* board) {
    for (unsigned char c = 0; c < board_size; c++) {
        board_set(board, pos_make(1, c), 
                  piece_new(PAWN, BLACK_SIDE, NOT_MOVED, pos_make(1, c)));
        board_set(board, pos_make(6, c), 
                  piece_new(PAWN, WHITE_SIDE, NOT_MOVED, pos_make(6, c)));
    }
    piece_kind pk[8] = {ROOK, ROOK, KNIGHT, KNIGHT, 
                        BISHOP, BISHOP, QUEEN, KING};
    unsigned char positions[8] = {0, 7, 1, 6, 2, 5, 3, 4};
    for (unsigned char i = 0; i < 8; i++) {
        board_set(board, pos_make(0, positions[i]),
          piece_new(pk[i], BLACK_SIDE, NOT_MOVED, pos_make(0, positions[i])));
        board_set(board, pos_make(7, positions[i]),
          piece_new(pk[i], WHITE_SIDE, NOT_MOVED, pos_make(7, positions[i])));
    }
}

Board* board_new(board_direction direction, start_position position) {
    Board* board = (Board*)malloc(sizeof(Board));
    malloc_check(board);
    board->matrix = (Piece***)malloc(sizeof(Piece**) * board_size);
    malloc_check(board->matrix);
    for (unsigned char r = 0; r < board_size; r++) {
        board->matrix[r] = (Piece**)malloc(sizeof(Piece*) * board_size);
        malloc_check(board->matrix[r]);
        for (unsigned char c = 0; c < board_size; c++) {
            board->matrix[r][c] = NULL;
        }
    }
    board->direction = WHITE_MOVING_UP;
    board->white_pieces = piecelist_new();
    board->black_pieces = piecelist_new();
    if (position == STANDARD_POSITION) {
        set_standard_position(board);
    }
    if (direction == BLACK_MOVING_UP) {
        board_flip(board);
    }
    return board;
}

void board_free(Board* board) {
    piecelist_free(board->white_pieces);
    piecelist_free(board->black_pieces);
    for (unsigned char r = 0; r < board_size; r++) {
        free(board->matrix[r]);
    }
    free(board->matrix);
    free(board);
}

//Helper 
void print_chars(board_direction direction) {
    printf("  ");
    if (direction == WHITE_MOVING_UP) {
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
char maybe_upp(side side, char c) {
    if (side == WHITE_SIDE) {
        char new_c = c - ('a' - 'A');
        return new_c;
    }
    return c;
}

//Helper
char convert_piece_to_char(Piece* piece) {
    char pk_chars[] = {'p', 'b', 'n', 'r', 'q', 'k'};
    if (piece == NULL) {
        return '.';
    }
    return maybe_upp(piece->side, pk_chars[piece->kind]);
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
            if (board->direction == WHITE_MOVING_UP) {
                printf("%hhu", mu);
            } else {
                printf("%hhu", md);
            }
            printf(" |");
            for (unsigned char j = 0; j < 8; j++) {
                Piece* piece = board_get(board, pos_make(i - curr, j));
                char c = convert_piece_to_char(piece);
                printf(" %c |", c);
            }
            curr++;
            printf(" ");
            if (board->direction == WHITE_MOVING_UP) {
                printf("%hhu", mu--);
            } else {
                printf("%hhu", md++);
            }
        }
        printf("\n"); 
    }
    print_chars(board->direction);
    printf("\n");
    printf("%hhu white pieces still on board:\n", board->white_pieces->len);
    piecelist_show(board->white_pieces, board->direction);
    printf("%hhu black pieces still on board:\n", board->black_pieces->len);
    piecelist_show(board->black_pieces, board->direction);
}

Piece* board_get(Board* board, Pos pos) {
    bounds_check(pos);
    Piece* piece = board->matrix[pos.r][pos.c];
    return piece;
}

void board_set(Board* board, Pos pos, Piece* piece) {
    bounds_check(pos);
    Piece* to_delete = board_get(board, pos);
    if (to_delete) {
        if (to_delete->side == WHITE_SIDE) {
            piecelist_delete(board->white_pieces, to_delete);
        } else {
            piecelist_delete(board->black_pieces, to_delete);
        }
    }
    if (piece) {
        if (piece->side == WHITE_SIDE) {
            piecelist_insert(board->white_pieces, piece);
        } else {
            piecelist_insert(board->black_pieces, piece);
        }
        piece->position = pos; //in case
    }
    board->matrix[pos.r][pos.c] = piece;
}

void board_swap(Board* board, Pos pos1, Pos pos2) {
    bounds_check(pos1);
    bounds_check(pos2);
    Piece* piece1 = board_get(board, pos1);
    Piece* piece2 = board_get(board, pos2);
    board->matrix[pos1.r][pos1.c] = piece2;
    board->matrix[pos2.r][pos2.c] = piece1;
    if (piece2) {
        piece2->position = pos1; //in case
    }
    if (piece1) {
        piece1->position = pos2; //in case
    }
}

//Helper
bool is_upper_case(char c) {
    return c >= 'A' && c <= 'Z';
}

//Helper
piece_kind find_kind(char c) {
    switch (c) {
        case 'P':
        case 'p':
            return PAWN;
        case 'R':
        case 'r':
            return ROOK;
        case 'Q':
        case 'q':
            return QUEEN;
        case 'K':
        case 'k':
            return KING;
        case 'B':
        case 'b':
            return BISHOP;
        case 'N':
        case 'n':
            return KNIGHT;
        default: {
            fprintf(stderr, "no kind matches to the character provided\n");
            exit(1);
        }
    }
}

void place_piece(Board* board, char* call) {
    char ks, c, m;
    unsigned char r;
    unsigned char count = sscanf(call, "%c%c%hhu%c", &ks, &c, &r, &m);
    if (count < 3) {
        fprintf(stderr, "Invalid call\n");
        exit(1);
    }
    piece_kind kind = find_kind(ks);
    side side;
    if (is_upper_case(ks)) {
        side = WHITE_SIDE;
    } else {
        side = BLACK_SIDE;
    }
    Pos pos = square_convert(square_make(c, r), board->direction); 
    piece_moved moved = MOVED;
    if (count == 4) {
        if (m == 'n') {
            moved = NOT_MOVED;
        }
    }
    board_set(board, pos, piece_new(kind, side, moved, pos));
}

void board_flip(Board* board) {
    for (unsigned char r = 0; r < (board_size / 2); r++) {
        for (unsigned char c = 0; c < board_size; c++) {
            Pos pos = pos_make(r, c);
            Pos mirr_pos = pos_make(board_size - 1 - r, board_size - 1 - c);
            board_swap(board, pos, mirr_pos);
        }
    }
    if (board->direction == WHITE_MOVING_UP) {
        board->direction = BLACK_MOVING_UP;
    } else {
        board->direction = WHITE_MOVING_UP;
    }
}

//Helper
Piece* encounter(Board* board, piece_kind kind, side side, Pos pos, 
                                            Transformation transformation) {
    char* transformations = transformation.transformations;
    unsigned char max_repeat = transformation.max_repeat; 
    Pos curr_pos;
    for (unsigned char i = 0; i < transformation.len; i += 2) {
        char tr = transformations[i], tc = transformations[i + 1];
        curr_pos = pos;
        for (unsigned char j = 0; j < max_repeat; j++) {
            curr_pos.r += tr;
            curr_pos.c += tc;
            if (curr_pos.r < 0 || curr_pos.r >= board_size || curr_pos.c < 0 ||
                    curr_pos.c >= board_size) {
                break;
            }
            Piece* piece = board_get(board, curr_pos);
            if (piece == NULL) {
                continue;
            }
            if (piece->side != side || piece->kind != kind) {
                break;
            }
            return piece;
        }
    }
    return NULL;
}

//Helper (only check pieces still on board, and not check the same piece twice)
Piece* handle_kind(Board* board, piece_kind kind, board_direction direction, 
                      side side, bool* maybe_found, Pos pos, piece_kind* pk) { 
    for (unsigned char i = 0; i < 6; i++) {
        if (kind == pk[i]) {
            if (maybe_found[i]) {
                break;
            }
            maybe_found[i] = true;
            Transformation t;
            if (kind == PAWN && direction == WHITE_MOVING_UP) {
                t = transformation_get(kind, CAPTURE, BLACK_MOVING_UP);
            } else {
                t = transformation_get(kind, CAPTURE, WHITE_MOVING_UP);
            }
            Piece* piece = encounter(board, kind, side, pos, t);
            if (piece) {
                transformation_free(t);
                return piece;
            }
        }
    }
    return NULL;
}

//Helper 
bool kind_in_kinds(piece_kind kind, piece_kind* pk, unsigned char pklen) {
    for (unsigned char i = 0; i < pklen; i++) {
        if (pk[i] == kind) {
            return true;
        }
    }
    return false;
}

//Helper (returns first piece that targets, could be many)
Piece* square_targeted(Board* board, Pos pos, side targeted, piece_kind* pk,
                                                        unsigned char pklen) {
    bool maybe_found[6] = {false}; //6 max number of kinds
    board_direction direction = board->direction;
    Piece_entry* head;
    if (targeted == WHITE_SIDE) {
        head = board->black_pieces->head;
    } else {
        head = board->white_pieces->head;
    }
    while (head) {
        piece_kind kind = head->piece->kind;
        if (kind_in_kinds(kind, pk, pklen)) {
            side side = head->piece->side;
            Piece* piece = handle_kind(board, kind, direction, side, 
                                                        maybe_found, pos, pk); 
            if (piece) {
                return piece;
            }
        }
        head = head->next;
    }
    return NULL;
}

//Helper
Pos kpos_get(Board* board, side kside) {
    Piece_entry* head;
    if (kside == WHITE_SIDE) {
        head = board->white_pieces->head;
    } else {
        head = board->black_pieces->head;
    }
    if (head) { //king expected to be first in the linked list
        if (head->piece->kind == KING) {
            return head->piece->position;
        }
    }
    fprintf(stderr, "King Not Found\n");
    exit(1);
}
 
Piece* check(Board* board, side threatened) {
    Pos kpos;
    if (threatened == WHITE_SIDE) {
        kpos = kpos_get(board, WHITE_SIDE);
    } else {
        kpos = kpos_get(board, BLACK_SIDE);
    }
    piece_kind pk[5] = {ROOK, PAWN, BISHOP, KNIGHT, QUEEN};
    return square_targeted(board, kpos, threatened, pk, 5);
}

//Helper (assumes the two pos are on same line, t is always of length 2)
void fill_transformation(char* transformation, Pos from, Pos to) {
    if (from.r == to.r) {
        if (to.c > from.c) {
            transformation[0] = 0; 
            transformation[1] = 1;
        } else {
            transformation[0] = 0;
            transformation[1] = -1;
        }
    } else if (from.c == to.c) { 
        if (to.r > from.r) {
            transformation[0] = 1;
            transformation[1] = 0;
        } else {
            transformation[0] = -1;
            transformation[1] = 0;
        }
    } else { //diagonal outcome
        if (to.r > from.r) {
            if (to.c < from.c) {
                transformation[0] = 1;
                transformation[1] = -1;
            } else {
                transformation[0] = 1;
                transformation[1] = 1;
            }
        } else {
            if (to.c < from.c) {
                transformation[0] = -1;
                transformation[1] = -1;
            } else {
                transformation[1] = -1;
                transformation[1] = 1;
            }
        }
    }
}

//Helper (t is always of length 2)
void reverse_transformation(char* transformation) {
    transformation[0] = -transformation[0];
    transformation[1] = -transformation[1];
}

//Helper (get converse) 
side opp_side(side side) {
    if (side == WHITE_SIDE) {
        return BLACK_SIDE;
    } else {
        return WHITE_SIDE;
    }
}

//there is always only one piece that ever pins in each case
Piece* pin(Board* board, Piece* piece) {
    if (piece == NULL) {
        fprintf(stderr, "Empty cell cannot be pinned\n");
        exit(1);
    }
    if (piece->kind == KING) {
        fprintf(stderr, "King cannot be pinned\n");
        exit(1);
    }
    Pos kpos = kpos_get(board, piece->side);
    Pos ppos = piece->position;
    if (!(kpos.r == ppos.r) && !(kpos.c == ppos.c) && 
               !(abs(kpos.r - ppos.r) == abs(kpos.c - ppos.c))) {
        return NULL; //not in same line
    }
    Transformation t;
    t.len = 2;
    t.max_repeat = 7;
    t.transformations = (char*)malloc(sizeof(char) * 2); 
    malloc_check(t.transformations);
    fill_transformation(t.transformations, ppos, kpos);
    piece_kind pk[2]; //pieces maybe pinning
    if (abs(kpos.r - ppos.r) == abs(kpos.c - ppos.c)) {
        pk[0] = BISHOP;
        pk[1] = QUEEN;
    } else {
        pk[0] = ROOK;
        pk[1] = QUEEN;
    }
    if (!encounter(board, KING, piece->side, ppos, t)) {
        transformation_free(t);
        return NULL; //line between king and piece is not empty
    }
    reverse_transformation(t.transformations); //checking opp direction now
    for (unsigned char i = 0; i < 2; i++) {
        Piece* pinning = encounter(board, pk[i], opp_side(piece->side), ppos, t);
        if (pinning) {
            transformation_free(t);
            return pinning;
        }
    }
    transformation_free(t);
    return NULL; //line is not empty between piece and the maybe-pinning pcs
}





























