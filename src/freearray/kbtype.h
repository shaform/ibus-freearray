/**
 * @file kbtype.h
 * @brief Defines the keyboard types that FreeArray library supports.
 * c version of kbtype.h
 *
 */

/**
 * FreeArray Input Method Library
 * 
 * Copyright (c) 2010
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
#ifndef FREEARRAY_KBTYPE_H
#define FREEARRAY_KBTYPE_H

#include "code.h"
#include <string.h>
typedef enum {
	FREEARRAY_KBTYPE_DEFAULT,
	FREEARRAY_KBTYPE_DVORAK,
	FREEARRAY_KBTYPE_NUM
} KbType;
typedef KbType FREEARRAY_KBTYPE;


/**
 * @brief Get the KeyCode of *c* in this KbType.
 * @return A KeyCode
 */
FAFTKeyCode faft_kbtype_get_kc(int ch, KbType kbt);


/**
 * @brief Get the character of *kc* in this KbType.
 *
 * Notice that the mapping from KeyCode to character may not be unique,
 * but the KbType must always return the same character for the same KeyCode.
 *
 * @return A character, EOF if failed
 */
int faft_kbtype_get_char(FAFTKeyCode kc, KbType kbt);

int faft_kbtype_is_special_end(int ch, KbType kbt);

#endif

