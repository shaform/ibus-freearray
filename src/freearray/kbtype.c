/** 
 * @file kbtype.c
 * @brief Defines the keyboard types that FreeArray library supports.
 * c version of kbtype.cpp
 *
 */

/**
 * FreeArray Input Method Library
 * 
 * Copyright (c) 2007-2010
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

#include <string.h>
#include "kbtype.h"
#include "code.h"

#define MAX_CONVERT 2
#define MAX_CSTR_LEN 30
#define EOF (-1)

static const char convert_string[FREEARRAY_KBTYPE_NUM][MAX_CONVERT][MAX_CSTR_LEN + 1] = {
	/* Default USA Qwerty Keyboard */
	"qwertyuiopasdfghjkl;zxcvbnm,./",
	"QWERTYUIOPASDFGHJKL:ZXCVBNM<>?",

	/* USA Dvorak Keyboard */
	"',.pyfgcrlaoeuidhtns;qjkxbmwvz",
	"\"<>PYFGCRLAOEUIDHTNS:QJKXBMWVZ",
};
FAFTKeyCode faft_kbtype_get_kc(int ch, KbType kbt)
{
int i;
	if (kbt >= 0 && kbt < FREEARRAY_KBTYPE_NUM)
		for (i=0; i<MAX_CONVERT; ++i) {
			const char *found = strchr(convert_string[kbt][i], ch);
			if (found != NULL)
				return (found - convert_string[kbt][i])+1;
		}

	return KEYCODE_ERROR;
}
int faft_kbtype_get_char(FAFTKeyCode kc, KbType kbt)
{
	// Check if conversion is possible
	if (valid_kc(kc) && kbt >=0 && kbt < FREEARRAY_KBTYPE_NUM) {

		// Set the search position for kc
		int pos = (kc + MAX_CSTR_LEN - 1) % MAX_CSTR_LEN;
		int i;
		// Search for the first convert_string which is able to convert kc
		for (i=0; i<MAX_CONVERT; ++i) {
			if (pos < strlen(convert_string[kbt][i]))
				return convert_string[kbt][i][pos];
		}
	}
	return EOF;
}

int faft_kbtype_is_special_end(int ch, KbType kbt)
{
	if (kbt == FREEARRAY_KBTYPE_DVORAK)
		return ch == '-' || ch == '_';
	else
		return ch == '\'' || ch == '"';
}
