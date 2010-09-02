/** 
 * @file code.h
 * @brief ArrayCode def
 * c version of code.h
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

/**
 *	KeyCode:
 *
 *	0 : END
 *	1~30 : Standard Array key
 *		31~60 : Additional Array key *reserved*
 *	61 : ?
 *	62 : *
 *	63 : Error
 *
 *
 *      ArrayCode:
 *
 *	*reserved*        1st           2nd           3rd           4th          5th
 *	|-- 2bits --|--- 6bits ---|--- 6bits ---|--- 6bits ---|--- 6bits ---|--- 6bits ---|
 *           00         KeyCode       KeyCode     ...trailing bits are filled with END(0)
 *
 * ArrayCode and phone happen to be able to be represented by the same datatype
 *
 */
#ifndef FREEARRAY_CODEC_H
#define FREEARRAY_CODEC_H

typedef unsigned short FAFTKeyCode;
typedef unsigned long ArrayCode;

#define KEYCODE_END (0u)
#define KEYCODE_ERROR (63u)
#define ARRAYCODE_MAX_LEN (4)
#define ARRAYCODE_MAX_LAN_ADD (5)




// Test if kc is a valid KeyCode
int valid_kc(FAFTKeyCode kc);
// Test if ac is a valid ArrayCode
int valid_ac(ArrayCode ac);

/**
 * @brief Convert a KeyCode array into an ArrayCode.
 *
 * If the length of the array is longer than KEYCODE_MAX_LEN, the behaviour is undefined.
 * If the array contains invalid KeyCodes, KEYCODE_ERROR is inserted into the ArrayCode
 * and the converting stops.
 *
 * !! The codes must end with KEYCODE_END
 *
 * @return The converted ArrayCode.
 */
ArrayCode keytocode(const FAFTKeyCode codes[]);



/**
 * @brief Convert an ArrayCode into a KeyCode array.
 * invalid KeyCodes will be converted into KEYCODE_ERRORs.
 *
 * !! caller must free the array
 *
 * @return The converted KeyCode array.
 */
FAFTKeyCode *codetokey(ArrayCode ac);


/**
 * @brief Convert a KeyCode array into a string.
 * !! must end with KEYCODE_END
 *
 * @return The converted string.
 * !! Caller must free the string.
 */
char *keystostr(FAFTKeyCode []);

char *keysntostr(FAFTKeyCode [], int len);



// Convert a KeyCode into a string.
char *keytostr(FAFTKeyCode kc);
// Convert an ArrayCode into a string.
char *codetostr(ArrayCode ac);

/*
ArrayCode strtocode(char []);
KeyCode *strtokeys(char []);
*/

// Get n-th KeyCode of an ArrayCode;
FAFTKeyCode get_code(ArrayCode ac, int pos);


/* temp */
/* For quick input only*/
ArrayCode faft_keycode_generate_next(ArrayCode ac);
#endif
