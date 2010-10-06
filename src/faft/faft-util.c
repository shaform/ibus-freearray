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
#include <stdlib.h>
#include "chewing/chewingutil.h"
#include "chewing/mod_aux.h"
#include "chewing/chewing-utf8-util.h"
#include "faport/tables.h"
#include "faft-util.h"
/* Routines */
int faft_data_add_symbol(const char *s, ChewingData *data)
{
	if (s == NULL || s[0] == '\0')
		return 0;
	/* This part is copied from libchewing */
	memmove(&(data->chiSymbolBuf[data->chiSymbolCursor + 1]),
			&(data->chiSymbolBuf[data->chiSymbolCursor]),
			sizeof(wch_t)
			* (data->chiSymbolBufLen - data->chiSymbolCursor));
	data->chiSymbolBuf[data->chiSymbolCursor].wch = (wchar_t) 0;
	ueStrNCpy((char *) data->chiSymbolBuf[data->chiSymbolCursor].s,
			s, 1, STRNCPY_CLOSE);


	int key = FindSymbolKey(s);
	data->symbolKeyBuf[data->chiSymbolCursor] = key ? key : '1';

	data->bUserArrCnnct[ PhoneSeqCursor(data)] = 0;
	++(data->chiSymbolBufLen);
	++(data->chiSymbolCursor); 

	return 1;
}
int faft_data_add_fixed(const char *s, ChewingData *data, ArrayCode ac)
{
	if (s == NULL || s[0] == '\0')
		return 0;

	AddChi(ac, data);
	--(data->chiSymbolCursor);
	int i_p = PhoneSeqCursor(data);
	++(data->chiSymbolCursor);

	/* Add selected character */
	ueStrNCpy(data->selectStr[data->nSelect], s, 1, STRNCPY_CLOSE);
	/* Add one character interval */
	data->selectInterval[data->nSelect].from = i_p;
	data->selectInterval[data->nSelect].to = i_p+1;
	++(data->nSelect);

	return 1;
}
/* Basic */
int faft_data_add_special(ArrayCode ac, ChewingData *data)
{
	/* note it is assumed that the ac & n is valid */
	ArrayCode ac_new;

	Word w;
	faft_table_get_special(&w, ac);
	ac_new = faft_table_get_special_ac();

	return faft_data_add_fixed(w.word, data, ac_new);
}

/**
 * For outputs
 *
 */
int faft_context_interval_enumerate(FAFTContext *ctx)
{
	chewing_interval_Enumerate(ctx);
	return 1;
}
int faft_context_interval_has_next(FAFTContext *ctx)
{
	return chewing_interval_hasNext(ctx);
}
int faft_context_interval_get(FAFTContext *ctx, IntervalType *it)
{
	chewing_interval_Get(ctx, it);
}
int faft_context_cand_page_size(FAFTContext *ctx)
{
	return chewing_cand_ChoicePerPage(ctx);
}
int faft_context_cand_enumerate(FAFTContext *ctx)
{
	chewing_cand_Enumerate(ctx);
	return 1;
}
int faft_context_cand_has_next(FAFTContext *ctx)
{
	return chewing_cand_hasNext(ctx);
}
char *faft_context_cand_string(FAFTContext *ctx)
{
	return chewing_cand_String(ctx);
}
int faft_context_quick_input_size(FAFTContext *ctx)
{
	return ctx ? ctx->data->quick.len : 0;
}
int faft_context_quick_input_enumerate(FAFTContext *ctx)
{
	if (ctx) ctx->quick_no = -1;
	return 1;
}
int faft_context_quick_input_has_next(FAFTContext *ctx)
{
	return ctx ? ctx->quick_no + 1 < faft_context_quick_input_size(ctx) : 0;
}
char *faft_context_get_quick_input_string(FAFTContext *ctx)
{
	char *s = (char *)calloc(MAX_UTF8_SIZE + 1, sizeof(char));
	s[0] = '\0';

	if (faft_context_quick_input_has_next(ctx)) {
		++(ctx->quick_no);
		strncpy(s, ctx->data->quick.string[ctx->quick_no], MAX_UTF8_SIZE +1);
	}

	return s;
}
char *faft_context_get_commit_string(FAFTContext *ctx)
{
	return chewing_commit_String(ctx);
}
char *faft_context_get_buffer_string(FAFTContext *ctx)
{
	return chewing_buffer_String(ctx);
}
int faft_context_get_buffer_cursor(FAFTContext *ctx)
{
	return chewing_cursor_Current(ctx);
}
int faft_context_release_buffer(FAFTContext *ctx)
{

	/* Carefully, we must avoid throwing out commit string */
	int throwEnd = CountReleaseNum(ctx->data);
	if (throwEnd != 0) {
		int old_nCommitStr = ctx->output->nCommitStr;
		wch_t *to_commit = ctx->output->commitStr;

		if (old_nCommitStr + throwEnd <= MAX_PHONE_SEQ_LEN
				&& faft_context_is_commit_string_updated(ctx))

			to_commit+=old_nCommitStr;

		else
			old_nCommitStr = 0;


		WriteChiSymbolToBuf(to_commit, throwEnd, ctx->data);
		ctx->output->nCommitStr = throwEnd + old_nCommitStr;
		KillFromLeft(ctx->data, throwEnd);


		faft_context_set_commit_string_updated(ctx);
	}

	return throwEnd;
}

int faft_context_is_special_end_key(FAFTContext *ctx, int keysym)
{
	char end1, end2;
	if (ctx->setting->kbtype == FREEARRAY_KBTYPE_DVORAK)
		end1 = '-', end2 = '_';
	else
		end1 = '\'', end2 = '"';

	return keysym == end1 || keysym == end2;
}

int faft_context_produce_quick_input(FAFTContext *ctx)
{
	ctx->data->quick.present = 0;
	int len = ctx->data->quick.len = 0;
	int i;
	for (i=0; i<10; ++i) {
		ctx->data->quick.ac[i] = 0;
		ctx->data->quick.string[i][0] = '\0';
	}
	if (!faft_context_is_keycode_entering(ctx))
		return 0;

	Word w;
	FAFTKeyCode kc_t[ARRAYCODE_MAX_LEN+1];
	faft_acdata_get_unfinished(&(ctx->data->acData), kc_t);
	ArrayCode ac = keytocode(kc_t);
	int ret;

	if (faft_context_is_short_code_input(ctx)) {
		ret = faft_table_get_short_first(&w, ac);

		for (i=0; i<10 && ret; ++i, ++len) {
			strcpy(ctx->data->quick.string[i], w.word);
			ctx->data->quick.ac[i] = faft_table_get_short_ac();

			ret = faft_table_get_short_next(&w);
		}
	}
	/* If not short_code, len would be zero */
	if (faft_context_is_pre_input(ctx)) {
		ret = faft_table_get_quick_char_first(&w, ac);
		for (i=0; i<10 && ret; ++i) {
			if (ctx->data->quick.ac[i] != 0)
				continue;

			strcpy(ctx->data->quick.string[i], w.word);
			ctx->data->quick.ac[i] = faft_table_get_quick_char_ac();

			ret = faft_table_get_quick_char_next(&w);
		}
		if (len == 0)
			len = i;
	}
	if (len == 0) {
		return 0;
	} else {
		ctx->data->quick.present = 1;
		ctx->data->quick.len = len;
		return 1;
	}
} 
#define CEIL_DIV( a, b ) 	( ( a + b - 1 ) / b )
int faft_util_produce_phrase_input(ChoiceInfo *pci, AvailInfo *pai)
{
	int i;
	int len = MAX_PHRASE_LEN * MAX_UTF8_SIZE + 1;
	char temp_s[MAX_PHRASE_LEN * MAX_UTF8_SIZE + 1];

	faft_table_get_phrase_again_strn(temp_s, len);
	pci->nTotalChoice = 0;
	for (i = 0; i < MAX_CHOICE; i++ ) {
		strcpy(pci->totalChoiceStr[pci->nTotalChoice], temp_s);
		pci->nTotalChoice++; 

		if (faft_table_get_phrase_next_strn(temp_s, len) == 0)
			break;
	}
	pai->avail[ 0 ].len = 1;
	pai->avail[ 0 ].id = -1;  
	pai->nAvail = 1;
	pai->currentAvail = 0;
	pci->nChoicePerPage = 10;

	if (pci->nChoicePerPage > 10) {
		pci->nChoicePerPage = 10;
	}
	pci->nPage = CEIL_DIV( pci->nTotalChoice, pci->nChoicePerPage );
	pci->pageNo = 0;
	/* To prvent cursor from changing by ChoiceEndChoice */
	pci->isSymbol = 2;
}

/**
 * Checks for states
 */
inline int faft_context_is_commit_string_updated(FAFTContext *ctx)
{
	return ctx && ctx->state->is_commit_string_updated;
}
inline int faft_context_is_keycode_string_updated(FAFTContext *ctx)
{
	return ctx && ctx->state->is_keycode_string_updated;
}
inline int faft_context_is_lookup_table_updated(FAFTContext *ctx)
{
	return ctx && ctx->state->is_lookup_table_updated;
}
inline int faft_context_is_buffer_updated(FAFTContext *ctx)
{
	return ctx && ctx->state->is_buffer_updated;
}
inline int faft_context_is_aux_string_updated(FAFTContext *ctx)
{
	return ctx && ctx->state->is_aux_string_updated;
}
inline int faft_context_is_quick_input_updated(FAFTContext *ctx)
{
	return ctx && ctx->state->is_quick_input_updated;
}


inline int faft_context_set_commit_string_updated(FAFTContext *ctx)
{
	return ctx && (ctx->state->is_commit_string_updated = 1);
}
inline int faft_context_set_keycode_string_updated(FAFTContext *ctx)
{
	return ctx && (ctx->state->is_keycode_string_updated = 1);
}
inline int faft_context_set_lookup_table_updated(FAFTContext *ctx)
{
	return ctx && (ctx->state->is_lookup_table_updated = 1);
}
inline int faft_context_set_buffer_updated(FAFTContext *ctx)
{
	return ctx && (ctx->state->is_buffer_updated = 1);
}
inline int faft_context_set_aux_string_updated(FAFTContext *ctx)
{
	return ctx && (ctx->state->is_aux_string_updated = 1);
}
inline int faft_context_set_quick_input_updated(FAFTContext *ctx)
{
	return ctx && (ctx->state->is_quick_input_updated = 1);
}


inline int faft_context_unset_commit_string_updated(FAFTContext *ctx)
{
	return ctx && !(ctx->state->is_commit_string_updated = 0);
}
inline int faft_context_unset_keycode_string_updated(FAFTContext *ctx)
{
	return ctx && !(ctx->state->is_keycode_string_updated = 0);
}
inline int faft_context_unset_lookup_table_updated(FAFTContext *ctx)
{
	return ctx && !(ctx->state->is_lookup_table_updated = 0);
}
inline int faft_context_unset_buffer_updated(FAFTContext *ctx)
{
	return ctx && !(ctx->state->is_buffer_updated = 0);
}
inline int faft_context_unset_aux_string_updated(FAFTContext *ctx)
{
	return ctx && !(ctx->state->is_aux_string_updated = 0);
}
inline int faft_context_unset_quick_input_updated(FAFTContext *ctx)
{
	return ctx && !(ctx->state->is_quick_input_updated = 0);
}


inline int faft_context_is_lookup_table_shown(FAFTContext *ctx)
{
	return ctx && ctx->data && ctx->data->bSelect;
}
inline int faft_context_is_keycode_entering(FAFTContext *ctx)
{
	return ctx && ctx->data && ctx->data->acData.len;
}
inline int faft_context_is_buffer_entering(FAFTContext *ctx)
{
	return ctx && ctx->data && ctx->data->chiSymbolBufLen;
}
inline int faft_context_is_entering(FAFTContext *ctx)
{
	return faft_context_is_buffer_entering(ctx)
		|| faft_context_is_keycode_entering(ctx)
		|| faft_context_is_lookup_table_shown(ctx);
}
/**
 * Settings
 *
 *
 */
FAFT_VARIETY faft_context_get_variety(FAFTContext *ctx)
{
	return ctx->state->variety;
}
FAFT_SHAPE faft_context_get_shape(FAFTContext *ctx)
{
	return ctx->state->shape;
}
int faft_context_set_variety(FAFTContext *ctx, FAFT_VARIETY v)
{
	if (ctx && ctx->state) {
		ctx->state->variety = v;
		return 1;
	} else
		return 0;
}
int faft_context_set_shape(FAFTContext *ctx, FAFT_SHAPE s)
{
	if (ctx && ctx->state) {
		ctx->state->shape = s;
		return 1;
	} else
		return 0;
}

FREEARRAY_KBTYPE faft_context_get_kbtype(FAFTContext *ctx)
{
	return ctx->setting->kbtype;
}
int faft_context_set_kbtype(FAFTContext *ctx, FREEARRAY_KBTYPE kbt)
{
	ctx->setting->kbtype = kbt;
	return 1;
}
int faft_context_get_max_preedit_buffer_len(FAFTContext *ctx)
{
	return ctx->setting->max_preedit_buffer_len;
}
int faft_context_set_max_preedit_buffer_len(FAFTContext *ctx, int len)
{
	ctx->data->config.maxChiSymbolLen = ctx->setting->max_preedit_buffer_len = len;
}


int faft_context_is_easy_symbol_input(FAFTContext *ctx)
{
	if (ctx && ctx->setting)
		return ctx->setting->easy_symbol_input;
	else
		return 0;
}

int faft_context_is_auto_input_mode(FAFTContext *ctx)
{
	if (ctx && ctx->setting)
		return ctx->setting->auto_input_mode;
	else
		return 0;
}
int faft_context_is_auto_clear_mode(FAFTContext *ctx)
{
	if (ctx && ctx->setting)
		return ctx->setting->auto_clear_mode;
	else
		return 0;
}
int faft_context_is_esc_clear_all_buff(FAFTContext *ctx)
{
	if (ctx && ctx->setting)
		return ctx->setting->esc_clear_all_buff;
	else
		return 0;
}
int faft_context_is_space_to_select(FAFTContext *ctx)
{
	if (ctx && ctx->setting)
		return ctx->setting->space_to_select;
	else
		return 0;
}
int faft_context_is_any_row_key_select(FAFTContext *ctx)
{
	if (ctx && ctx->setting)
		return ctx->setting->any_row_key_select;
	else
		return 0;
}
int faft_context_is_choose_phrase_backward(FAFTContext *ctx)
{
	if (ctx && ctx->setting)
		return ctx->setting->choose_phrase_backward;
	else
		return 0;
}
int faft_context_is_short_code_input(FAFTContext *ctx)
{
	if (ctx && ctx->setting)
		return ctx->setting->short_code_input;
	else
		return 0;
}
int faft_context_is_pre_input(FAFTContext *ctx)
{
	if (ctx && ctx->setting)
		return ctx->setting->pre_input;
	else
		return 0;
}
int faft_context_is_phrase_input(FAFTContext *ctx)
{
	if (ctx && ctx->setting)
		return ctx->setting->phrase_input;
	else
		return 0;
}
int faft_context_is_array_symbol_input(FAFTContext *ctx)
{
	if (ctx && ctx->setting)
		return ctx->setting->array_symbol_input;
	else
		return 0;
}
int faft_context_set_auto_input_mode(FAFTContext *ctx)
{
	if (ctx && ctx->setting)
		return (ctx->setting->auto_input_mode = 1);
	else
		return 0;
}
int faft_context_set_auto_clear_mode(FAFTContext *ctx)
{
	if (ctx && ctx->setting)
		return (ctx->setting->auto_clear_mode = 1);
	else
		return 0;
}
int faft_context_set_esc_clear_all_buff(FAFTContext *ctx)
{
	if (ctx && ctx->setting)
		return (ctx->setting->esc_clear_all_buff = 1);
	else
		return 0;
}
int faft_context_set_space_to_select(FAFTContext *ctx)
{
	if (ctx && ctx->setting)
		return (ctx->setting->space_to_select = 1);
	else
		return 0;
}
int faft_context_set_any_row_key_select(FAFTContext *ctx)
{
	if (ctx && ctx->setting)
		return (ctx->setting->any_row_key_select = 1);
	else
		return 0;
}
int faft_context_set_choose_phrase_backward(FAFTContext *ctx)
{
	if (ctx && ctx->setting && ctx->data) {
		return (ctx->data->config.bPhraseChoiceRearward = ctx->setting->choose_phrase_backward = 1);
	} else
		return 0;
}
int faft_context_set_short_code_input(FAFTContext *ctx)
{
	if (ctx && ctx->setting)
		return (ctx->setting->short_code_input = 1);
	else
		return 0;
}
int faft_context_set_pre_input(FAFTContext *ctx)
{
	if (ctx && ctx->setting)
		return (ctx->setting->pre_input = 1);
	else
		return 0;
}
int faft_context_set_phrase_input(FAFTContext *ctx)
{
	if (ctx && ctx->setting)
		return (ctx->setting->phrase_input = 1);
	else
		return 0;
}
int faft_context_set_array_symbol_input(FAFTContext *ctx)
{
	if (ctx && ctx->setting)
		return (ctx->setting->array_symbol_input = 1);
	else
		return 0;
}
int faft_context_set_easy_symbol_input(FAFTContext *ctx)
{
	if (ctx && ctx->setting)
		return (ctx->setting->easy_symbol_input = 1);
	else
		return 0;
}

int faft_context_unset_auto_input_mode(FAFTContext *ctx)
{
	if (ctx && ctx->setting)
		return 0 == (ctx->setting->auto_input_mode = 0);
	else
		return 0;
}
int faft_context_unset_auto_clear_mode(FAFTContext *ctx)
{
	if (ctx && ctx->setting)
		return 0 == (ctx->setting->auto_clear_mode = 0);
	else
		return 0;
}
int faft_context_unset_esc_clear_all_buff(FAFTContext *ctx)
{
	if (ctx && ctx->setting)
		return 0 == (ctx->setting->esc_clear_all_buff = 0);
	else
		return 0;
}
int faft_context_unset_space_to_select(FAFTContext *ctx)
{
	if (ctx && ctx->setting)
		return 0 == (ctx->setting->space_to_select = 0);
	else
		return 0;
}
int faft_context_unset_any_row_key_select(FAFTContext *ctx)
{
	if (ctx && ctx->setting)
		return 0 == (ctx->setting->any_row_key_select = 0);
	else
		return 0;
}
int faft_context_unset_choose_phrase_backward(FAFTContext *ctx)
{
	if (ctx && ctx->setting && ctx->data) {
		return 0 == (ctx->data->config.bPhraseChoiceRearward = ctx->setting->choose_phrase_backward = 0);
	} else
		return 0;
}
int faft_context_unset_short_code_input(FAFTContext *ctx)
{
	if (ctx && ctx->setting)
		return 0 == (ctx->setting->short_code_input = 0);
	else
		return 0;
}
int faft_context_unset_pre_input(FAFTContext *ctx)
{
	if (ctx && ctx->setting)
		return 0 == (ctx->setting->pre_input = 0);
	else
		return 0;
}
int faft_context_unset_phrase_input(FAFTContext *ctx)
{
	if (ctx && ctx->setting)
		return 0 == (ctx->setting->phrase_input = 0);
	else
		return 0;
}
int faft_context_unset_array_symbol_input(FAFTContext *ctx)
{
	if (ctx && ctx->setting)
		return 0 == (ctx->setting->array_symbol_input = 0);
	else
		return 0;
}
int faft_context_unset_easy_symbol_input(FAFTContext *ctx)
{
	if (ctx && ctx->setting)
		return 0 == (ctx->setting->easy_symbol_input = 0);
	else
		return 0;
}
