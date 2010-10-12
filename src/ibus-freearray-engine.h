/**
 * ibus-freearray - FreeArray for The Input Bus
 *
 * Copyright (c) 2010
 * 	Yong-Siang Shih (Shaform) <shaform at gmail.com>
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
#ifndef __IBUS_FAFT_ENGINE_H__
#define __IBUS_FAFT_ENGINE_H__

#include <ibus.h>

#define IBUS_TYPE_FREEARRAY_ENGINE	\
	(ibus_faft_engine_get_type())

GType ibus_faft_engine_get_type (void);

#endif
