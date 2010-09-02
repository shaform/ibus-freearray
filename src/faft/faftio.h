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
#ifndef FAFT_IO_H
#define FAFT_IO_H
#include "faft-def.h"

/* General */
void faft_init(const char *data_path);
void faft_exit();

FAFTContext *faft_context_new();
void faft_context_delete(FAFTContext *);
void faft_context_reset(FAFTContext *);




/**
 * Handles Key Event
 */
/* The functions here consider settings & states.
 * Normally, you should only use the functions here to change the FAFTContext.
 * But if you want to do something without changing the state of the context,
 * you can use internal functions instead.
 */
/**
 * When a keyevent is not ended but mulitple actions is taken,
 * the key event state is determined by the last action.
 * (But if an action is not proccessed, status would not be changed)
 * The info about whether something is updated is always correct.
 * 
 * If a keyevent is ended and a action happens before a new keyevent is started,
 * the key event status may be incorrect but some actions may be done, though the behaviour may be unexpected.
 * 
 * KeyEvent can be 
 * process -> ignore, accept, pass.
 * 
 * If an action is not processed but ended, it changes to passed.
 */
/*
 * Output would be updated only if a key event is ended.
 */
int faft_context_new_key_event(FAFTContext *);
int faft_context_end_key_event(FAFTContext *);
/* After a reset, you have to new a key event again before doing any actions */
int faft_context_reset_key_event(FAFTContext *);
/**
 * Change the state of Key Event directly.
 * Use with caution!
 * Nomarly, you do not need these functions.
 * These are for internal use and special cases. (which actually do happen.)
 */
int faft_context_ignore_key_event(FAFTContext *);
int faft_context_pass_key_event(FAFTContext *);
int faft_context_accept_key_event(FAFTContext *);



/**
 * Check the state of Key Event
 */
int faft_context_is_key_event_processed(FAFTContext *);

int faft_context_is_key_event_accepted(FAFTContext *);
int faft_context_is_key_event_ignored(FAFTContext *);
int faft_context_is_key_event_passed(FAFTContext *);






/**
 * Lookup Table
 */

int faft_context_end_lookup_table(FAFTContext *);
int faft_context_show_lookup_table(FAFTContext *);

int faft_context_first_avail(FAFTContext *);
int faft_context_last_avail(FAFTContext *);
int faft_context_prev_avail(FAFTContext *);
int faft_context_next_avail(FAFTContext *);

int faft_context_first_page(FAFTContext *);
int faft_context_last_page(FAFTContext *);
int faft_context_prev_page(FAFTContext *);
int faft_context_next_page(FAFTContext *);

int faft_context_select_cand(FAFTContext *, int nth);
int faft_context_any_row_select(FAFTContext *, int keysym);

/**
 * Input
 *
 * The most important part of faftio.
 */
int faft_context_input_keycode(FAFTContext *, int keysym);
int faft_context_input_char(FAFTContext *);
int faft_context_input_symbol(FAFTContext *, int keysym);
int faft_context_input_quick(FAFTContext *, int nth);
int faft_context_input_phrase(FAFTContext *);
int faft_context_input_array_symbol(FAFTContext *);

/**
 * Buffer
 *
 */

int faft_context_clear_all_buffer(FAFTContext *);

int faft_context_clear_aux(FAFTContext *);

int faft_context_clear_keycode(FAFTContext *);
int faft_context_remove_last_keycode(FAFTContext *);

int faft_context_clear_buffer(FAFTContext *);
int faft_context_remove_word_backward(FAFTContext *);
int faft_context_remove_word_forward(FAFTContext *);

int faft_context_move_cursor_forward(FAFTContext *);
int faft_context_move_cursor_backward(FAFTContext *);
int faft_context_move_cursor_to_front(FAFTContext *);
int faft_context_move_cursor_to_end(FAFTContext *);
int faft_context_toggle_break_point(FAFTContext *);

/**
 * Other Functions
 *
 */
int faft_context_commit(FAFTContext *);
int faft_context_open_symbol_table(FAFTContext *);
int faft_context_toggle_variety(FAFTContext *);
int faft_context_toggle_shape(FAFTContext *);

/**
 *
 * Handles Output
 */
#endif
