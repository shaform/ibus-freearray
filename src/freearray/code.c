/** 
 * @file code.c
 * @brief ArrayCode def
 * c version of code.cpp
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
#include <stdlib.h>
#include "code.h"

#define insertkey(kc, ac, i) ( (ac) &= (~ARRAYCODE_MASK[i]), (ac) |= ( (kc) << 6 * (4 - (i) ) ) )
const ArrayCode ARRAYCODE_MASK[5] = {63u << 24, 63u << 18, 63u << 12, 63u << 6, 63u};

ArrayCode keytocode(const FAFTKeyCode codes[])
{
	ArrayCode ac = 0u;
	int i;
	for (i=0; codes[i] != KEYCODE_END && i < ARRAYCODE_MAX_LEN; ++i) {
		if (valid_kc(codes[i]))
			insertkey(codes[i], ac, i);
		else {
			insertkey(KEYCODE_ERROR, ac, i);
			break;
		}
	}
	return ac;
}

int valid_kc(FAFTKeyCode kc)
{
	return (kc <= 30 && kc >= 1) || kc == KEYCODE_END;
}

int valid_ac(ArrayCode ac)
{
	FAFTKeyCode kc = 0u;
	int i;
	// Check if all KeyCodes are valid and the ArrayCode is ended with a KEYCODE_END
	for (i=0; i<ARRAYCODE_MAX_LEN; ++i) {
		kc = get_code(ac, i);
		if (valid_kc(kc)) {
			if (kc == KEYCODE_END)
				break;
		} else
			return 0;
	}

	// Check if all remaining KeyCodes are KEYCODE_ENDs
	for (++i; i<=ARRAYCODE_MAX_LEN; ++i)
		if (get_code(ac, i) != KEYCODE_END)
			return 0;

	// Return true if all tests were passed
	return 1;
}

FAFTKeyCode get_code(ArrayCode ac, int pos)
{
	return (ac & ARRAYCODE_MASK[pos]) >> 6*(ARRAYCODE_MAX_LEN - pos);
}

char *keystostr(FAFTKeyCode kcs[])
{
	int i;
	for (i=0; kcs[i]!=KEYCODE_END; ++i);

	return keysntostr(kcs, i);
}
char *keysntostr(FAFTKeyCode kcs[], int len)
{
	if (len>0) {
		char *s = (char *)calloc(len*2+1, sizeof(char));
		if (s) {
			s[0] = '\0';
			int i;
			for (i=0; i<len; ++i) {
				char *a_s = keytostr(kcs[i]);
				if (a_s) {
					strcat(s, a_s);
					free((void *)a_s);
				} else
					break;
			}
			return s;
		}
	}
	return NULL;
}
char *codetostr(ArrayCode ac)
{
	char *s = (char *)calloc(ARRAYCODE_MAX_LEN*2+1, sizeof(char));

	int i;
	for (i=0; i<ARRAYCODE_MAX_LEN; ++i) {
		FAFTKeyCode kc = get_code(ac, i);
		if (valid_kc(kc) && kc != KEYCODE_END) {
			char *s_k = keytostr(kc);
			if (s_k) {
				strcat(s, s_k);
				free((void *)s_k);
			} else
				break;
		} else break;
	}
	return s;
}



// Convert a KeyCode into a string.
char *keytostr(FAFTKeyCode kc)
{

	char temp[] = "^-v";
	char *cstr = (char *)calloc(3, sizeof(char));
	cstr[0] = '0'+(kc%10);
	cstr[1] = temp[(kc-1)/10];
	cstr[2] = '\0';
	if (kc == KEYCODE_END) {
		cstr[0] = '$';
		cstr[1] = '$';
	} else if (!valid_kc(kc)) {
		cstr[0] = 'X';
		cstr[1] = 'X';
	}
	return cstr;
}
ArrayCode faft_keycode_generate_next(ArrayCode ac)
{
	int i;
	FAFTKeyCode kc_t = 0;
	for (i=0; i<ARRAYCODE_MAX_LEN && get_code(ac, i) != KEYCODE_END; ++i)
		kc_t = get_code(ac, i);

	/* Notice, here it may produce KeyCode '31', which is not a valid KeyCode now */
	if (i>0 && i <= ARRAYCODE_MAX_LEN && kc_t <= 30 && kc_t > 0) {
		++kc_t;
		insertkey(kc_t, ac, i-1);
	}

	return ac;
}
