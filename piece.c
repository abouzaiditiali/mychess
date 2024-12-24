#include "piece.h"

Piece* piece_new(piece_kind kind, side side, piece_moved moved, Pos pos) {
    Piece* piece = (Piece*)malloc(sizeof(Piece));
    malloc_check(piece);
    piece->kind = kind;
    piece->side = side;
    piece->position = pos;
    piece->moved = moved;
    return piece;
}

void piece_show(Piece* piece) {
    if (piece == NULL) {
        fprintf(stderr, "Cannot show an empty cell\n");
        exit(1);
    }
    char* kind;
    char* pk_strings[] = {"PAWN", "BISHOP", "KNIGHT", "ROOK", "QUEEN", "KING"};
    kind = pk_strings[piece->kind];
    char* side; 
    if (piece->side == WHITE_SIDE) {
        side = "WHITE";
    } else {
        side = "BLACK";
    }
    char* moved;
    if (piece->moved == MOVED) {
        moved = "MOVED";
    } else {
        moved = "NOT MOVED";
    }
    printf("Kind: %s, ", kind);
    printf("Side: %s, ", side);
    printf("Position: ");
    square_show(pos_convert(piece->position, WHITES_PERSPECTIVE));
    printf(", ");
    printf("Status: %s\n", moved);
}

void piece_free(Piece* piece) {
    free(piece);
}

//Helper
//Transformation copy_transformation(char* from, unsigned char len, 
//                                        unsigned char max_repeat) {
//    Transformation t;
//    t.max_repeat = max_repeat;
//    t.transformations = (char*)malloc(sizeof(char)*len);
//    malloc_check(t.transformations);
//    for (unsigned char i = 0; i < len; i++) {
//        t.transformations[i] = from[i];
//    }
//    t.len = len;
//    return t;
//}
//
//Transformation transformation_get(piece_kind kind, board_direction direction,
//                                    piece_moved moved, move_type move_type) {
//    char knight[16] = {-2, -1, -2, 1, -1, -2, -1, 2, 1, -2, 1, 2, 2, -1, 2, 1};
//    char bishop[8] = {-1, -1, -1, 1, 1, -1, 1, 1};
//    char queen[16] = {-1, -1, -1, 0, -1, 1, 0, -1, 0, 1, 1, -1, 1, 0, 1, 1};
//    char rook_castle_wu[4] = {0, -2, 0, 3}; //whichever is in bounds
//    char rook_castle_bu[4] = {0, 2, 0, -3}; //whichever is in bounds
//    char rook[8] = {-1, 0, 0, -1, 0, 1, 1, 0};
//    char king_castle_wu[4] = {0, -3, 0, 2};
//    char king_castle_bu[4] = {0, -2, 0, 3};
//    char king[16] = {-1, -1, -1, 0, -1, 1, 0, -1, 0, 1, 1, -1, 1, 0, 1, 1};
//    char pawn_capture_wu[4] = {-1, -1, -1, 1};
//    char pawn_wum[2] = {-1, 0}; 
//    char pawn_wun[2] = {-2, 0}; 
//    char pawn_capture_bu[4] = {1, -1, 1, 1};
//    char pawn_bum[2] = {1, 0}; 
//    char pawn_bun[2] = {2, 0}; 
//    switch (kind) {
//        case KNIGHT:
//            return copy_transformation(knight, 16, 1);
//        case BISHOP:
//            return copy_transformation(bishop, 8, 7);
//        case QUEEN:
//            return copy_transformation(queen, 16, 7);
//        case ROOK: {
//            if (move_type == CASTLE && moved == NOT_MOVED) {
//                if (direction == WHITE_MOVING_UP) {
//                    return copy_transformation(rook_castle_wu, 4, 1);
//                } else {
//                    return copy_transformation(rook_castle_bu, 4, 1);
//                }
//            }
//            return copy_transformation(rook, 8, 7);
//        }
//        case KING: {
//            if (move_type == CASTLE && moved == NOT_MOVED) {
//                if (direction == WHITE_MOVING_UP) {
//                    return copy_transformation(king_castle_wu, 4, 1);
//                } else {
//                    return copy_transformation(king_castle_bu, 4, 1);
//                }
//            }
//            return copy_transformation(king, 16, 1);
//        }
//        case PAWN: { //these are in relation to white
//            if (direction == WHITE_MOVING_UP) {
//                if (move_type != CAPTURE && move_type != EN_PASSANT) {
//                    if (moved == MOVED) {
//                        return copy_transformation(pawn_wum, 2, 1);
//                    } else {
//                        return copy_transformation(pawn_wun, 2, 1);
//                    }
//                }
//                return copy_transformation(pawn_capture_wu, 4, 1);
//            } else {
//                if (move_type != CAPTURE && move_type != EN_PASSANT) {
//                    if (moved == MOVED) { 
//                        return copy_transformation(pawn_bum, 2, 1);
//                    } else {
//                        return copy_transformation(pawn_bun, 2, 1);
//                    }
//                }
//                return copy_transformation(pawn_capture_bu, 4, 1);
//            }
//        }
//    }
//}
// 
//void transformation_free(Transformation t) {
//    free(t.transformations);
//}

Piecelist* piecelist_new() {
    Piecelist* piecelist = (Piecelist*)malloc(sizeof(Piecelist));
    malloc_check(piecelist);
    piecelist->head = NULL;
    piecelist->len = 0;
    return piecelist;
}

void piecelist_show(Piecelist* piecelist) {
    Piece_entry* head = piecelist->head;
    if (head) {
        while (head) {
            piece_show(head->piece);
            head = head->next;
        }
    }
    printf("\n");
}

void piecelist_insert(Piecelist* piecelist, Piece* piece) {
    Piece_entry* piece_entry = (Piece_entry*)malloc(sizeof(Piece_entry));
    malloc_check(piece_entry);
    piece_entry->piece = piece;
    if (piece->kind == KING) {
        piece_entry->next = piecelist->head;
        piecelist->head = piece_entry;
    } else if (piecelist->len == 0) {
        piece_entry->next = NULL;
        piecelist->head = piece_entry;
    } else {
        Piece_entry* tmp = piecelist->head->next;
        piecelist->head->next = piece_entry;
        piece_entry->next = tmp;
    }
    (piecelist->len)++;
}

//Helper
void piece_entry_free(Piece_entry* piece_entry) {
    piece_free(piece_entry->piece);
    free(piece_entry);
}

void piecelist_delete(Piecelist* piecelist, Piece* piece) {
    if (piecelist->len == 0) {
        fprintf(stderr, "No pieces to delete from\n");
        exit(1);
    }
    Piece_entry* head = piecelist->head; 
    if (head->piece == piece) {
        Piece_entry* to_delete = head;
        piecelist->head = head->next;
        piece_entry_free(to_delete);
    } else {
        Piece_entry* curr = head;
        while (curr->next) {
            if (curr->next->piece == piece) {
                Piece_entry* to_delete = curr->next; 
                curr->next = curr->next->next;
                piece_entry_free(to_delete);
            }
            curr = curr->next;
        }
    }
}

void piecelist_free(Piecelist* piecelist) {
    Piece_entry* to_delete;
    while (piecelist->head) {
        to_delete = piecelist->head;
        piecelist->head = piecelist->head->next;
        piece_entry_free(to_delete);
    }
    free(piecelist);
}

