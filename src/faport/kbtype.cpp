/** 
 * @file kbtype.cpp
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
#include <freearray/kbtype.h>
#include "kbtype.h"
#include "code.h"


const freearray::KbType *kbts[] = {&freearray::kbt_default, &freearray::kbt_dvorak};


FAFTKeyCode faft_kbtype_get_kc(int ch, KbType kbt)
{
	if (kbt >= 0 && kbt < FREEARRAY_KBTYPE_NUM)
		return kbts[kbt]->get_key(ch);
	else
		return KEYCODE_ERROR;
}

int faft_kbtype_get_char(FAFTKeyCode kc, KbType kbt)
{
	// Check if conversion is possible
	if (kbt >= 0 && kbt < FREEARRAY_KBTYPE_NUM)
		return kbts[kbt]->get_char(kc);
	else
		return EOF;
}

int faft_kbtype_is_special_end(int ch, KbType kbt)
{
	if (kbt == FREEARRAY_KBTYPE_DVORAK)
		return ch == '-' || ch == '_';
	else
		return ch == '\'' || ch == '"';
}
