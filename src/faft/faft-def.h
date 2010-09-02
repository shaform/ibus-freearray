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
#ifndef FAFT_DEF_H
#define FAFT_DEF_H
#include "chewing/chewing-private.h"
#include "freearray/kbtype.h"


typedef struct {
	FREEARRAY_KBTYPE kbtype;
	int auto_input_mode;
	int auto_clear_mode;
	int esc_clear_all_buff;
	int space_to_select;
	int any_row_key_select;
	int choose_phrase_backward;
	int short_code_input;
	int pre_input;
	int phrase_input;
	int array_symbol_input;

	/* Note: Easy Symbol Input is turned on no matter the setting here when in Chi mode.
	 * This only controls the behaviour in En mode.
	 */
	int easy_symbol_input;

	int max_preedit_buffer_len;
} FAFTSetting;

typedef enum { FAFT_VARIETY_CHI,
	FAFT_VARIETY_ENG,
} FAFT_VARIETY;

typedef enum { FAFT_SHAPE_HALF,
	FAFT_SHAPE_FULL,
} FAFT_SHAPE;

/* status */
/* FIXME: The use of this is rather strange
 * There are jusst too much state variables.
 */
typedef enum { FAFT_STATUS_IDLE, // No buffer
	FAFT_STATUS_BUFFER, // Some buffer
	FAFT_STATUS_EDIT, // Don't know buffer or not

	FAFT_STATUS_KEYCODE_ENTERING, // Unfinshed AC
	FAFT_STATUS_AC_ERROR, // Error AC && With unfinished AC

	FAFT_STATUS_CHAR_SELECT,

	FAFT_STATUS_PHRASE_INPUT,

	FAFT_STATUS_SYMBOL_TABLE,
	FAFT_STATUS_SYMBOL_SELECT,

	FAFT_STATUS_BYPASS, // A Special mode used in Eng mode when EASYSYMBOL is disabled, and the shape is half

} FAFT_STATUS;
/* key event status */
typedef enum { FAFT_KEY_EVENT_STATUS_NONE = 0,
	FAFT_KEY_EVENT_STATUS_ACCEPTED,
	FAFT_KEY_EVENT_STATUS_IGNORED,
	FAFT_KEY_EVENT_STATUS_PASSED,
} FAFT_KEY_EVENT_STATUS;



/* This is actually only be used by auto-clear, auto-input now */
/* key status */
typedef enum { FAFT_KEY_STATUS_NONE = 0,
	FAFT_KEY_STATUS_AC_ERROR,
	FAFT_KEY_STATUS_ACCEPTED,
	FAFT_KEY_STATUS_IGNORED,
	FAFT_KEY_STATUS_COMMIT,
} FAFT_KEY_STATUS;


typedef struct {
	FAFT_VARIETY variety;
	FAFT_SHAPE shape;

	FAFT_STATUS status;

	FAFT_KEY_EVENT_STATUS key_event_status;

	FAFT_KEY_STATUS key_status;
	FAFT_KEY_STATUS last_key_status;


	int is_commit_string_updated;
	int is_aux_string_updated;
	int is_buffer_updated;
	int is_keycode_string_updated;
	int is_lookup_table_updated;
	int is_quick_input_updated;
} FAFTState;

typedef struct FAFTContext {
	FAFTSetting *setting;
	ChewingData *data;
	ChewingOutput *output;
	FAFTState *state;


	int cand_no;
	int it_no;
	int quick_no;

} FAFTContext;





#endif
