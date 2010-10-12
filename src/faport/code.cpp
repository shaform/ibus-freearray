/** 
 * @file code.cpp
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
#include <cstring>
#include <string>
#include <cstdlib>
#include <freearray/code.h>
#include "code.h"
#define insertkey(kc, ac, i) ( (ac) &= (~ARRAYCODE_MASK[i]), (ac) |= ( (kc) << 6 * (4 - (i) ) ) )
const ArrayCode ARRAYCODE_MASK[5] = {63u << 24, 63u << 18, 63u << 12, 63u << 6, 63u};

using namespace std;


ArrayCode keytocode(const FAFTKeyCode codes[])
{
	int i;
	for (i=0; codes[i] != KEYCODE_END && i < ARRAYCODE_MAX_LEN; ++i);
	return freearray::keytocode(freearray::KeyCodeVec(codes, codes+i));
}

int valid_kc(FAFTKeyCode kc)
{
	return freearray::is_key_valid(kc) || freearray::is_key_end(kc);
}

int valid_ac(ArrayCode ac)
{
	return freearray::is_code_valid(ac);
}

FAFTKeyCode get_code(ArrayCode ac, int pos)
{
	return freearray::get_key(ac, pos);
}

char *keystostr(FAFTKeyCode kcs[])
{
	int i;
	for (i=0; kcs[i]!=KEYCODE_END; ++i);

	return keysntostr(kcs, i);
}
char *keysntostr(FAFTKeyCode kcs[], int len)
{
	string str = freearray::keytostr(freearray::KeyCodeVec(kcs, kcs+len));
	if (str.size()>0) {
		char *s = (char *)calloc(str.size()+1, sizeof(char));
		if (s) {
			strcpy(s, str.c_str());
			return s;
		}
	}
	return 0;
}
char *codetostr(ArrayCode ac)
{
	string str = freearray::codetostr(ac);
	if (str.size()>0) {
		char *s = (char *)calloc(str.size()+1, sizeof(char));
		if (s) {
			strcpy(s, str.c_str());
			return s;
		}
	}

	return 0;
}



// Convert a KeyCode into a string.
char *keytostr(FAFTKeyCode kc)
{
	FAFTKeyCode keys[2];
	keys[0] = kc;
	keys[1] = KEYCODE_END;
	return keysntostr(keys, 1);

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
