#include "board.h"

//Helper
void set_standard_position(Board* board) {
    unsigned char size = board->size;
    for (unsigned char c = 0; c < size; c++) {
        board_set(board, make_pos(1, c), 
                  piece_new(PAWN, BLACK_SIDE, NOT_MOVED, make_pos(1, c)));
        board_set(board, make_pos(6, c), 
                  piece_new(PAWN, WHITE_SIDE, NOT_MOVED, make_pos(6, c)));
    }
    piece_kind pk[8] = {ROOK, ROOK, KNIGHT, KNIGHT, 
                        BISHOP, BISHOP, QUEEN, KING};
    unsigned char positions[8] = {0, 7, 1, 6, 2, 5, 3, 4};
    for (unsigned char i = 0; i < 8; i++) {
        board_set(board, make_pos(0, positions[i]),
          piece_new(pk[i], BLACK_SIDE, NOT_MOVED, make_pos(0, positions[i])));
        board_set(board, make_pos(7, positions[i]),
          piece_new(pk[i], WHITE_SIDE, NOT_MOVED, make_pos(7, positions[i])));
    }
}

Board* board_new(board_direction direction, start_position position) {
    unsigned char size = 8;
    Board* board = (Board*)malloc(sizeof(Board));
    malloc_check(board);
    board->matrix = (Piece***)malloc(sizeof(Piece**) * size);
    malloc_check(board->matrix);
    for (unsigned char r = 0; r < size; r++) {
        board->matrix[r] = (Piece**)malloc(sizeof(Piece*) * size);
        malloc_check(board->matrix[r]);
        for (unsigned char c = 0; c < size; c++) {
            board->matrix[r][c] = NULL;
        }
    }
    board->size = size;
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
    unsigned char size = board->size;
    for (unsigned char r = 0; r < size; r++) {
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
    if (piece == NULL) {
        return '.';
    }
    switch (piece->kind) {
        case PAWN:
            return maybe_upp(piece->side, 'p');
        case KNIGHT:
            return maybe_upp(piece->side, 'n');
        case BISHOP:
            return maybe_upp(piece->side, 'b');
        case ROOK:
            return maybe_upp(piece->side, 'r');
        case QUEEN:
            return maybe_upp(piece->side, 'q');
        case KING:
            return maybe_upp(piece->side, 'k');
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
            if (board->direction == WHITE_MOVING_UP) {
                printf("%hhu", mu);
            } else {
                printf("%hhu", md);
            }
            printf(" |");
            for (unsigned char j = 0; j < 8; j++) {
                Piece* piece = board_get(board, make_pos(i - curr, j));
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
}

//Helper 
void bounds_check(unsigned char size, Pos pos) {
    if (pos.r < 0 || pos.r >= size || pos.c < 0 || pos.c >= size) {
        fprintf(stderr, "Tried to access position outside of board\n");
        exit(1);
    }
}
 
Piece* board_get(Board* board, Pos pos) {
    bounds_check(board->size, pos);
    Piece* piece = board->matrix[pos.r][pos.c];
    return piece;
}

void board_set(Board* board, Pos pos, Piece* piece) {
    bounds_check(board->size, pos);
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
    bounds_check(board->size, pos1);
    bounds_check(board->size, pos2);
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
    char ks, c, r, m;
    side side;
    piece_kind kind;
    Pos pos;
    piece_moved moved;
    if (sscanf(call, "%c%c%c%c", &ks, &c, &r, &m) == 4) {
        kind = find_kind(ks);
        if (is_upper_case(ks)) {
            side = WHITE_SIDE;
        } else {
            side = BLACK_SIDE;
        }
        if (c < 'a' || c > 'h' || r - '0' < 0 || r - '0' >= board->size) {
            fprintf(stderr, "Invalid column\n");
            exit(1);
        }
        if (board->direction == WHITE_MOVING_UP) {
            pos = make_pos(board->size - (r - '0'), c - 'a');
        } else {
            pos = make_pos((r - '0') - 1, 'h' - c);
        }
        if (m != 'm' && m != 'n') {
            fprintf(stderr, "Move status undetermined\n");
            exit(1);
        }
        if (m == 'm') {
            moved = MOVED;
        } else {
            moved = NOT_MOVED;
        }
    } else if (sscanf(call, "%c%c%c", &ks, &c, &r) == 3) {
        kind = find_kind(ks);
        if (is_upper_case(ks)) {
            side = WHITE_SIDE;
        } else {
            side = BLACK_SIDE;
        }
        if (c < 'a' || c > 'h' || r - '0' < 0 || r - '0' >= board->size) {
            fprintf(stderr, "Invalid column\n");
            exit(1);
        }
        if (board->direction == WHITE_MOVING_UP) {
            pos = make_pos(board->size - (r - '0'), c - 'a');
        } else {
            pos = make_pos((r - '0') - 1, 'h' - c);
        }
        moved = MOVED;
    } else {
        fprintf(stderr, "Invalid call\n");
        exit(1);
    }
    board_set(board, pos, piece_new(kind, side, moved, pos));
}

void board_flip(Board* board) {
    unsigned char size = board->size;
    for (unsigned char r = 0; r < (size / 2); r++) {
        for (unsigned char c = 0; c < size; c++) {
            Pos pos = make_pos(r, c);
            Pos mirr_pos = make_pos(size - 1 - r, size - 1 - c);
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
bool encounter(Board* board, piece_kind kind, side side, Pos pos, 
                                            Transformation transformation) {
    char* transformations = transformation.transformations;
    unsigned char max_repeat = transformation.max_repeat, size = board->size;
    
    for (unsigned char i = 0; i < transformation.len; i += 2) {
        char tr = transformations[i], tc = transformations[i + 1];
        Pos curr_pos = pos;
        
        for (unsigned char j = 0; j < max_repeat; j++) {
            curr_pos.r += tr;
            curr_pos.c += tc;
            if (curr_pos.r < 0 || curr_pos.r >= size || curr_pos.c < 0 ||
                    curr_pos.c >= size) {
                break;
            }
            Piece* piece = board_get(board, curr_pos);
            if (piece == NULL) {
                continue;
            }
            if (piece->side != side || piece->kind != kind) {
                break;
            }

            return true;
        }
    }
    return false;
}

//Helper
bool handle_kind(Board* board, piece_kind kind, board_direction direction, 
                                side side, bool* maybe_found, Pos pos) { 
    piece_kind pk[6] = {PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING};
    for (unsigned char i = 0; i < 6; i++) {
        if (kind == pk[i]) {
            if (maybe_found[i]) {
                break;
            }
            maybe_found[i] = true;
            if (kind == PAWN) {
                if (direction == WHITE_MOVING_UP) {
                    if (encounter(board, kind, side, pos,
                        transformation_get(kind, CAPTURE, BLACK_MOVING_UP))) {
                        return true;
                    }
                } else {
                    if (encounter(board, kind, side, pos, 
                        transformation_get(kind, CAPTURE, WHITE_MOVING_UP))) {
                        return true;
                    } 
                }
            } else {
                if (encounter(board, kind, side, pos, 
                        transformation_get(kind, CAPTURE, WHITE_MOVING_UP))) { 
                    return true;
                }
            }
        }
    }
    return false;
}

bool square_targeted(Board* board, Pos pos, side targeted) {
    bool maybe_found[6] = {false}; 
    board_direction direction = board->direction;
    Piece_entry* head;
    if (targeted == WHITE_SIDE) {
        head = board->black_pieces->head;
    } else {
        head = board->white_pieces->head;
    }
    while (head) {
        piece_kind kind = head->piece->kind;
        side side = head->piece->side;
        if (handle_kind(board, kind, direction, side, maybe_found, pos)) {
            return true;
        }
        head = head->next;
    }
    return false;
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
 
bool check(Board* board, side threatened) {
    Pos kpos;
    if (threatened == WHITE_SIDE) {
        kpos = kpos_get(board, WHITE_SIDE);
    } else {
        kpos = kpos_get(board, BLACK_SIDE);
    }
    return square_targeted(board, kpos, threatened);
    //be careful with case of king checking
}

























