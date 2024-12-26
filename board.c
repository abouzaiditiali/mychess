#include "board.h"

Board* board_new() {
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
    board->white_pieces = piecelist_new();
    board->black_pieces = piecelist_new();
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
void print_chars(player_perspective perspective) {
    printf("  ");
    for (unsigned char i = 0; i < 8; i++) { //added
        printf("   %hhu", i); //added
    } //added
//    if (perspective == WHITES_PERSPECTIVE) {
//        for (char c = 'a'; c <= 'h'; c++) {
//            printf("   %c", c);
//        }
//    } else {
//        for (char c = 'h'; c >= 'a'; c--) {
//            printf("   %c", c);
//        }
//    }
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

void board_show(Board* board, player_perspective perspective) {
    printf("\n");
    print_chars(perspective);
    //unsigned char mu = 8, md = 1, curr = 1;
    unsigned char mu = 0, md = 7, curr = 0; //added
    for (unsigned char i = 0; i < 17; i++) {
        if (i % 2 == 0) {
            printf("   ");
            for (unsigned char j = 0; j < 8; j++) {
                printf(" ---");
            }
        } else {
            printf(" ");
            if (perspective == WHITES_PERSPECTIVE) {
                printf("%hhu", mu);
            } else {
                printf("%hhu", md);
            }
            printf(" |");
            for (unsigned char j = 0; j < 8; j++) {
                //Pos wp_pos = pos_make(i - curr, j);
                //Piece* piece;
                //if (perspective == WHITES_PERSPECTIVE) {
                //    piece = board_get(board, wp_pos);
                //} else {
                //    piece = board_get(board, pos_flip(wp_pos));
                //}
                //char c = convert_piece_to_char(piece);
                //printf(" %c |", c);
                printf(" . |"); //added
            }
            curr++;
            printf(" ");
            if (perspective == WHITES_PERSPECTIVE) {
                //printf("%hhu", mu--);
                printf("%hhu", mu++); //added
            } else {
                //printf("%hhu", md++);
                printf("%hhu", md--); //added
            }
        }
        printf("\n"); 
    }
    print_chars(perspective);
    printf("\n");
    printf("%hhu white pieces still on board\n", board->white_pieces->len);
    piecelist_show(board->white_pieces);
    printf("%hhu black pieces still on board\n", board->black_pieces->len);
    piecelist_show(board->black_pieces);
    printf("\n");
}

Piece* board_get(Board* board, Pos pos) {
    bounds_check(pos.r, pos.c);
    return board->matrix[pos.r][pos.c];
}

void board_set(Board* board, Pos pos, Piece* piece) {
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

//void board_swap(Board* board, Pos pos1, Pos pos2) {
//    bounds_check(pos1.r, pos1.c);
//    bounds_check(pos2.r, pos2.c);
//    Piece* piece1 = board_get(board, pos1);
//    Piece* piece2 = board_get(board, pos2);
//    board->matrix[pos1.r][pos1.c] = piece2;
//    board->matrix[pos2.r][pos2.c] = piece1;
//    if (piece2) {
//        piece2->position = pos1; //in case
//    }
//    if (piece1) {
//        piece1->position = pos2; //in case
//    }
//}

//Helper
bool is_upper_case(char c) {
    return c >= 'A' && c <= 'Z';
}

//Helper
piece_kind find_kind(char c) {
    if (c == 'P' || c == 'p') return PAWN;
    if (c == 'R' || c == 'r') return ROOK;
    if (c == 'Q' || c == 'q') return QUEEN;
    if (c == 'K' || c == 'k') return KING;
    if (c == 'B' || c == 'b') return BISHOP;
    if (c == 'N' || c == 'n') return KNIGHT;
    fprintf(stderr, "No kind matches the character provided\n");
    exit(1);
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
    Pos pos = square_convert(square_make(c, r));
    piece_moved moved = MOVED;
    if (count == 4) {
        if (m == 'n') {
            moved = NOT_MOVED;
        }
    }
    board_set(board, pos, piece_new(kind, side, moved, pos));
}

//void board_flip(Board* board) {
//    for (unsigned char r = 0; r < (board_size / 2); r++) {
//        for (unsigned char c = 0; c < board_size; c++) {
//            Pos pos = pos_make(r, c);
//            Pos fpos = pos_flip(pos);
//            board_swap(board, pos, fpos);
//        }
//    }
//}

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
//
////Helper (get converse) 
//side opp_side(side side) {
//    if (side == WHITE_SIDE) {
//        return BLACK_SIDE;
//    } else {
//        return WHITE_SIDE;
//    }
//}
//
//Piece* check(Board* board, side threatened) {
//    Pos kpos = kpos_get(board, threatened);
//    piece_kind pk[5] = {ROOK, BISHOP, QUEEN, KNIGHT, PAWN};
//    return square_targeted(board, kpos, opp_side(threatened), pk, 5);
//}
//
////maybe use?
//void fill_transformation(char* transformation, Pos from, Pos to);
//void reverse_transformation(char* transformation);
//Piece* square_targeted(Board* board, Pos pos, side targeting, piece_kind* pk,
//                            unsigned char pklen);
//
////Helper
////pos and transformation are both the king's
//bool safe_escape(Board* board, Piece* checking, Pos pos, 
//                                              Transformation transformation) {
//    char* transformations = transformation.transformations;
//    unsigned char max_repeat = transformation.max_repeat; 
//    Pos curr_pos;
//    for (unsigned char i = 0; i < transformation.len; i += 2) {
//        char tr = transformations[i], tc = transformations[i + 1];
//        curr_pos = pos;
//        for (unsigned char j = 0; j < max_repeat; j++) {
//            curr_pos.r += tr;
//            curr_pos.c += tc;
//            if (curr_pos.r < 0 || curr_pos.r >= board_size || curr_pos.c < 0 ||
//                    curr_pos.c >= board_size) {
//                break;
//            }
//            Piece* piece = board_get(board, curr_pos);
//            if (!piece && piece->side == opp_side(checking->side)) {
//                break;
//            }
//            piece_kind pk[6] = {PAWN, BISHOP, KNIGHT, ROOK, QUEEN, KING};
//            if (square_targeted(board, curr_pos, checking->side, pk, 6)) {
//                //square_protected
//                break;
//            }
//            if (checking->kind == QUEEN || checking->kind == BISHOP ||
//                                                    checking->kind == ROOK) {
//                char* t = (char*)malloc(sizeof(char) * 2);
//                fill_transformation(t, pos, checking->position);
//                reverse_transformation(t);
//                Pos reversed_pos = pos_make(pos.r + t[0], pos.c + t[1]);
//                free(t);
//                return (!pos_cmp(reversed_pos, piece->position));
//            }
//            return true;
//        }
//    }
//    return false;
//}
//
////wait what about double check, fuck
////if double check, only check for the escape
////also very important, for efficiency, only check if it is a double check if
////the piece checking has some piece along the same line
////what if i were to check for bishop, queen, rook, and if they are not the piece checking
////it cannot be a double check, if they are, check for any pieces that were in
////the trajectory between the piece checking and the king in the previous move!
//bool checkmate(Board* board, side threatened) {
//    Piece* checking = check(board, threatened);
//    if (checking == NULL) {
//        return false;
//    }
//    Pos kpos = kpos_get(board, threatened);
//    Transformation t = transformation_get(KING, board->direction,
//                                          MOVED, CAPTURE); //only kind matters
//    if (!safe_escape(board, checking, kpos, t)) {
//        return false;
//    }
//    //second case only applies to queen, bishop, and rook
//    //see trajectory between king and piece checking
//    //check every square and find the one where a piece can move. 
//    //it is important to check all pieces as square_targeted returns the first
//    //piece and that one might be pinned but there could be another piece
//    //also targeting that has been unaccounted for
//
//    //check if piece checking can be captured 
//    return true;
//}



























