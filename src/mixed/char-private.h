#ifndef MIXED_CHAR_H
#define MIXED_CHAR_H
#include "freearray/tables.h"
#include "chewing/global.h"
#define GetCharFirst(w, c)	faft_table_get_char_first((w), (c))
#define GetCharNext(w)		faft_table_get_char_next((w))
#endif
