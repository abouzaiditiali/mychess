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

void piece_free(Piece* piece) {
    free(piece);
}

//Helper
Transformation copy_transformation(char* from, unsigned char len, 
                                        unsigned char max_repeat) {
    Transformation t;
    t.max_repeat = max_repeat;
    t.transformations = (char*)malloc(sizeof(char)*len);
    malloc_check(t.transformations);
    for (unsigned char i = 0; i < len; i++) {
        t.transformations[i] = from[i];
    }
    t.len = len;
    return t;
}

Transformation transformation_get(piece_kind kind, move_type move_type, 
                                                board_direction direction) {
    switch (kind) {
        case KNIGHT: {
            char tn[16] = {-2, -1, -2, 1, -1, -2, -1, 2, 
                           1, -2, 1, 2, 2, -1, 2, 1};
            return copy_transformation(tn, 16, 1);
        }
        case BISHOP: {
            char tb[8] = {-1, -1, -1, 1, 1, -1, 1, 1};
            return copy_transformation(tb, 8, 7);
        }
        case QUEEN: {
            char tq[16] = {-1, -1, -1, 0, -1, 1, 0, -1, 
                           0, 1, 1, -1, 1, 0, 1, 1};
            return copy_transformation(tq, 16, 7);
        }
        case ROOK: {
            if (move_type == CASTLE) {
                if (direction == WHITE_MOVING_UP) {
                    char trcu[4] = {0, -2, 0, 3}; //whichever is in bounds
                    return copy_transformation(trcu, 4, 1);
                } else {
                    char trcd[4] = {0, 2, 0, -3}; //whichever is in bounds
                    return copy_transformation(trcd, 4, 1);
                }
            }
            char tr[8] = {-1, 0, 0, -1, 0, 1, 1, 0};
            return copy_transformation(tr, 8, 7);
        }
        case KING: {
            if (move_type == CASTLE) {
                if (direction == WHITE_MOVING_UP) {
                    char tkcu[4] = {0, -3, 0, 2};
                    return copy_transformation(tkcu, 4, 1);
                } else {
                    char tkcd[4] = {0, -2, 0, 3};
                    return copy_transformation(tkcd, 4, 1);
                }
            }
            char tk[16] = {-1, -1, -1, 0, -1, 1, 0, -1, 
                           0, 1, 1, -1, 1, 0, 1, 1};
            return copy_transformation(tk, 16, 1);
        }
        case PAWN: {
            if (direction == WHITE_MOVING_UP) {
                if (move_type != CAPTURE && move_type != EN_PASSANT) {
                    char tpu[2] = {-1, 0}; 
                    return copy_transformation(tpu, 2, 2);
                }
                char tpcu[4] = {-1, -1, -1, 1};
                return copy_transformation(tpcu, 4, 1);
            } else {
                if (move_type != CAPTURE && move_type != EN_PASSANT) {
                    char tpd[2] = {1, 0}; 
                    return copy_transformation(tpd, 2, 2);
                }
                char tpcd[4] = {1, -1, 1, 1};
                return copy_transformation(tpcd, 4, 1);
            }
        }
    }
}
 
Piecelist* piecelist_new() {
    Piecelist* piecelist = (Piecelist*)malloc(sizeof(Piecelist));
    malloc_check(piecelist);
    piecelist->head = NULL;
    piecelist->len = 0;
    return piecelist;
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

void piece_entry_free(Piece_entry* piece_entry) {
    piece_free(piece_entry->piece);
    free(piece_entry);
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

