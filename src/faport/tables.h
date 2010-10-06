/** 
 * @file tables.h
 * @brief Handles data tables
 * c version of tables.h
 *
 */

/**
 * FreeArray Input Method Library
 * 
 * Copyright (c) 2009-2010
 * 	Yong-Siang Shih (Shaform) <shaform@gmail.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file.
 */

#ifndef IBUS_FREEARRAY_TABLESC_H
#define IBUS_FREEARRAY_TABLESC_H
#define FAFT_WORD_MAX 7
#ifndef SEEK_SET
#define SEEK_SET 0
#endif


#include "code.h"

typedef struct {
	char word[FAFT_WORD_MAX];
} Word;
#ifdef __cplusplus
extern "C" {
#endif
int faft_table_init(const char *);
int faft_table_exit();

int faft_table_get_special(Word  *, ArrayCode);
int faft_table_get_special_again(Word  *);
ArrayCode faft_table_get_special_ac();

int faft_table_get_char_first(Word *, ArrayCode);
int faft_table_get_char_again(Word *);
int faft_table_get_char_next(Word *);

int faft_table_get_quick_char_first(Word *, ArrayCode);
int faft_table_get_quick_char_again(Word *);
int faft_table_get_quick_char_next(Word *);
ArrayCode faft_table_get_quick_char_ac();

ArrayCode faft_table_get_ac(const char *);
ArrayCode faft_table_get_ac_again();

int faft_table_get_phrase_first_strn(char *, int len, ArrayCode);
int faft_table_get_phrase_again_strn(char *, int len);
int faft_table_get_phrase_next_strn(char *, int len);

int faft_table_get_short_first(Word *, ArrayCode);
int faft_table_get_short_again(Word *);
int faft_table_get_short_next(Word *);
ArrayCode faft_table_get_short_ac();
int faft_table_get_symbol(Word *, ArrayCode);
int faft_table_get_symbol_again(Word *);

#ifdef __cplusplus
}
#endif




#endif
