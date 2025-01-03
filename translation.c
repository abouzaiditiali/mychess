#include "translation.h"

//Helper
TranslationToKind fill_translations(char* from, unsigned char len, 
                                                    unsigned char max_repeat) {
    TranslationToKind tk;
    tk.max_repeat = max_repeat;
    tk.len = len / 2;
    tk.translations = (Translation*)malloc(sizeof(Translation) * tk.len);
    unsigned char j = 0;
    for (unsigned char i = 0; i < len; i += 2) {
        tk.translations[j++] = (Translation){from[i], from[i + 1]};
    }
    return tk;
}

TranslationToKind translation_get(piece_kind kind, side side, move_type mt) {
    switch (kind) {
        case ROOK: {
            char d1[8] = {0, -1, -1, 0, 0, 1, 1, 0};
            return fill_translations(d1, 8, 7);
                   }
        case QUEEN: {
            char d2[16] = {0, -1, -1, 0, 0, 1, 1, 0, 
                            -1, -1, -1, 1, 1, 1, 1, -1};
            return fill_translations(d2, 16, 7);
                    }
        case BISHOP: {
            char d3[8] = {-1, -1, -1, 1, 1, 1, 1, -1};
            return fill_translations(d3, 8, 7);
                     }
        case KNIGHT: {
            char d4[16] = {-2, -1, -2, 1, -1, -2, -1, 2, 
                            1, -2, 1, 2, 2, -1, 2, 1};
            return fill_translations(d4, 16, 1);
                     }
        case KING: {
            char d5[16] = {0, -1, -1, 0, 0, 1, 1, 0, 
                            1, -1, -1, 1, 1, 1, 1, -1};
            return fill_translations(d5, 16, 1);
                   }
        case PAWN: {
            //mt is trivial everywhere else but not here
            if (side == WHITE_SIDE) {
                char d6[4] = {-1, -1, -1, 1};
                return fill_translations(d6, 4, 1);
            } else {
                char d7[4] = {1, -1, 1, 1};
                return fill_translations(d7, 4, 1);
            }
                   }
    }
}

