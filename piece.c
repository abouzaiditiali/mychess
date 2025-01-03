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

bool is_sliding_piece(piece_kind kind) {
    return kind == QUEEN || kind == ROOK || kind == BISHOP; 
}

void piece_show(Piece* piece) {
    if (piece == NULL) {
        fprintf(stderr, "Cannot show an empty cell\n");
        exit(1);
    }
    char* kind;
    char* pk_strings[] = {"PAWN", "BISHOP", "KNIGHT", "ROOK", "QUEEN", "KING"};
    kind = pk_strings[piece->kind];
    printf("(%s, ", kind);
    square_show(pos_convert(piece->position, WHITES_PERSPECTIVE));
    if (piece->moved == MOVED) {
        printf(", MOVED)");
    } else {
        printf(")");
    }
}

void piece_free(Piece* piece) {
    free(piece);
}

/* Piece Linked List */

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
        char* side;
        if (head->piece->side == WHITE_SIDE) {
            side = "WHITE";
        } else {
            side = "BLACK";
        }
        printf("%hhu %s pieces still on board\n", piecelist->len, side);
        while (head) {
            piece_show(head->piece);
            printf(", ");
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
                (piecelist->len)--;
                break;
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

