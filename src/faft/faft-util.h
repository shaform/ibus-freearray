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
#ifndef FAFT_UTIL_H
#define FAFT_UTIL_H
#include "faft-def.h"
int faft_data_add_symbol(const char *s, ChewingData *data);
int faft_data_add_fixed(const char *, ChewingData *, ArrayCode);
int faft_data_add_special(ArrayCode , ChewingData *);
/**
 * For outputs
 *
 */

int faft_context_interval_enumerate(FAFTContext *);
int faft_context_interval_has_next(FAFTContext *);
int faft_context_interval_get(FAFTContext *, IntervalType *it);

int faft_context_cand_page_size(FAFTContext *);
int faft_context_cand_enumerate(FAFTContext *);
int faft_context_cand_has_next(FAFTContext *);
char *faft_context_cand_string(FAFTContext *);

int faft_context_quick_input_size(FAFTContext *);
int faft_context_quick_input_enumerate(FAFTContext *);
int faft_context_quick_input_has_next(FAFTContext *);

char *faft_context_get_quick_input_string(FAFTContext *);
char *faft_context_get_commit_string(FAFTContext *);
char *faft_context_get_buffer_string(FAFTContext *);
int faft_context_get_buffer_cursor(FAFTContext *);

/**
 * Utils
 */
int faft_context_release_buffer(FAFTContext*);
int faft_context_is_special_end_key(FAFTContext *, int keysym);
int faft_context_produce_quick_input(FAFTContext *);
int faft_util_produce_phrase_input(ChoiceInfo *, AvailInfo *);

/**
 * Checks for states
 */
int faft_context_is_commit_string_updated(FAFTContext *);
int faft_context_is_keycode_string_updated(FAFTContext *);
int faft_context_is_lookup_table_updated(FAFTContext *);
int faft_context_is_buffer_updated(FAFTContext *);
int faft_context_is_aux_string_updated(FAFTContext *);

/* FIXME: This should be merged to lookup table, but it's easier to do it this way. */
int faft_context_is_quick_input_updated(FAFTContext *);

int faft_context_set_commit_string_updated(FAFTContext *);
int faft_context_set_keycode_string_updated(FAFTContext *);
int faft_context_set_lookup_table_updated(FAFTContext *);
int faft_context_set_buffer_updated(FAFTContext *);
int faft_context_set_aux_string_updated(FAFTContext *);

/* FIXME: This should be merged to lookup table, but it's easier to do it this way. */
int faft_context_set_quick_input_updated(FAFTContext *);

int faft_context_unset_commit_string_updated(FAFTContext *);
int faft_context_unset_keycode_string_updated(FAFTContext *);
int faft_context_unset_lookup_table_updated(FAFTContext *);
int faft_context_unset_buffer_updated(FAFTContext *);
int faft_context_unset_aux_string_updated(FAFTContext *);

/* FIXME: This should be merged to lookup table, but it's easier to do it this way. */
int faft_context_unset_quick_input_updated(FAFTContext *);

int faft_context_is_lookup_table_shown(FAFTContext *);
int faft_context_is_keycode_entering(FAFTContext *);
int faft_context_is_buffer_entering(FAFTContext *);
int faft_context_is_entering(FAFTContext *);

/**
 * Settings
 *
 *
 */


FAFT_VARIETY faft_context_get_variety(FAFTContext *);
FAFT_SHAPE faft_context_get_shape(FAFTContext *);
int faft_context_set_variety(FAFTContext *, FAFT_VARIETY);
int faft_context_set_shape(FAFTContext *, FAFT_SHAPE);

FREEARRAY_KBTYPE faft_context_get_kbtype(FAFTContext *);
int faft_context_set_kbtype(FAFTContext *, FREEARRAY_KBTYPE);

int faft_context_get_max_preedit_buffer_len(FAFTContext *);
int faft_context_set_max_preedit_buffer_len(FAFTContext *, int len);

int faft_context_is_auto_input_mode(FAFTContext *);
int faft_context_is_auto_clear_mode(FAFTContext *);
int faft_context_is_esc_clear_all_buff(FAFTContext *);
int faft_context_is_space_to_select(FAFTContext *);
int faft_context_is_any_row_key_select(FAFTContext *);
int faft_context_is_choose_phrase_backward(FAFTContext *);
int faft_context_is_short_code_input(FAFTContext *);
int faft_context_is_pre_input(FAFTContext *);
int faft_context_is_phrase_input(FAFTContext *);
int faft_context_is_array_symbol_input(FAFTContext *);

/* Note: Easy Symbol Input is turned on no matter the setting here when in Chi mode.
 * This only controls the behaviour in En mode.
 */
int faft_context_is_easy_symbol_input(FAFTContext *);

int faft_context_set_auto_input_mode(FAFTContext *);
int faft_context_set_auto_clear_mode(FAFTContext *);
int faft_context_set_esc_clear_all_buff(FAFTContext *);
int faft_context_set_space_to_select(FAFTContext *);
int faft_context_set_any_row_key_select(FAFTContext *);
int faft_context_set_choose_phrase_backward(FAFTContext *);
int faft_context_set_short_code_input(FAFTContext *);
int faft_context_set_pre_input(FAFTContext *);
int faft_context_set_phrase_input(FAFTContext *);
int faft_context_set_array_symbol_input(FAFTContext *);
int faft_context_set_easy_symbol_input(FAFTContext *);

int faft_context_unset_auto_input_mode(FAFTContext *);
int faft_context_unset_auto_clear_mode(FAFTContext *);
int faft_context_unset_esc_clear_all_buff(FAFTContext *);
int faft_context_unset_space_to_select(FAFTContext *);
int faft_context_unset_any_row_key_select(FAFTContext *);
int faft_context_unset_choose_phrase_backward(FAFTContext *);
int faft_context_unset_short_code_input(FAFTContext *);
int faft_context_unset_pre_input(FAFTContext *);
int faft_context_unset_phrase_input(FAFTContext *);
int faft_context_unset_array_symbol_input(FAFTContext *);
int faft_context_unset_easy_symbol_input(FAFTContext *);



#endif
