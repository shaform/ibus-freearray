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
#include "faport/code.h"
#include "acdata.h"
int faft_acdata_input(ACData *acd, FAFTKeyCode kc)
{
	if (acd == NULL || acd->len > ARRAYCODE_MAX_LEN || acd->len < 0)
		return AC_ERROR;

	if (valid_kc(kc)) {
		if (kc == KEYCODE_END && acd->len > 0) {
			acd->kcs[acd->len] = kc;
			acd->ac = keytocode(acd->kcs);
			return AC_COMMIT;
		} else if (acd->len == ARRAYCODE_MAX_LEN) {
			acd->kcs[acd->len] = KEYCODE_END;
			acd->ac = keytocode(acd->kcs);
			return AC_FULL;
		} else {
			acd->kcs[(acd->len)++] = kc;
			return AC_ABSORB;

		}
	}
	return AC_KEY_ERROR;
}
int faft_acdata_get_unfinished(ACData *acd, FAFTKeyCode *kcs)
{
	int i;
	for (i = 0; i < acd->len; ++i) {
		kcs[i] = acd->kcs[i];
	}
	kcs[i] = KEYCODE_END;
	return 1;
}
int faft_acdata_remove_last(ACData *acd)
{
	if (acd->len)
		--(acd->len);
	return acd->len;
}
int faft_acdata_remove_all(ACData *acd) { return (acd->len = 0) == 0; }
int faft_acdata_is_entering(ACData *acd) { return acd->len > 0; }
int faft_acdata_is_full(ACData *acd) { return acd->len == ARRAYCODE_MAX_LEN; }
int faft_acdata_length(ACData *acd) { return acd->len; }


