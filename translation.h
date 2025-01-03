#ifndef TRANSLATION_H
#define TRANSLATION_H

#include "pos.h"

typedef struct { char r, c;
} Translation;
typedef struct { Translation* translations; unsigned char len, max_repeat;
} TranslationToKind;

TranslationToKind translation_get(piece_kind kind, side side, move_type mt); 

#endif /* TRANSLATION_H */
