/**
 * acdata.h
 * defines ACData
 */
/**
 * ibus-faft - FreeArray for Test for The Input Bus
 * 
 * Copyright (c) 2010
 * 	Yong-Siang Shih (Shaform) <shaform@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file.
 */


#ifndef FAFT_ACDATA_H
#define FAFT_ACDATA_H

#include "faport/kbtype.h"
#include "faport/code.h"

#define AC_ABSORB 1
#define AC_COMMIT 2
#define AC_KEY_ERROR 4
#define AC_ERROR 8
#define AC_FULL 32

#define AC_SIZE (ARRAYCODE_MAX_LEN*2+1)

typedef struct {
	ArrayCode ac;
	FAFTKeyCode kcs[ARRAYCODE_MAX_LEN+1];
	int len;
} ACData;

int faft_acdata_input(ACData *, FAFTKeyCode key);
int faft_acdata_get_unfinished(ACData *acd, FAFTKeyCode *kcs);



int faft_acdata_remove_last(ACData *acd);
int faft_acdata_remove_all(ACData *acd);
int faft_acdata_is_entering(ACData *acd);
int faft_acdata_is_full(ACData *acd);
int faft_acdata_length(ACData *acd);

#endif
