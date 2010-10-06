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
#include "faft-def.h"
#include "faft-util.h"
#include "faftio.h"

#include "chewing/chewing-utf8-util.h"
#include "chewing/chewingio.h"
#include "chewing/chewingutil.h"
#include "chewing/mod_aux.h"
#include "chewing/tree-private.h"
#include "chewing/dict-private.h"
#include "chewing/choice-private.h"

#include "faport/tables.h"

#include <stdlib.h>
#include <ctype.h>





/* ibus-faft context util */
void faft_init(const char *data_path)
{
	InitTree(data_path);
	InitDict(data_path);
	InitSymbolTable(data_path);
	faft_table_init(data_path);
}
void faft_exit()
{
	faft_table_exit();
	chewing_Terminate();
}


FAFTContext *faft_context_new()
{
	FAFTContext *context = (FAFTContext *)calloc(1, sizeof(FAFTContext));
	ChewingData *data = (ChewingData *)calloc(1, sizeof(ChewingData));
	ChewingOutput *output = (ChewingOutput *)calloc(1, sizeof(ChewingOutput));
	FAFTSetting *setting = (FAFTSetting *)calloc(1, sizeof(FAFTSetting));
	FAFTState *state = (FAFTState *)calloc(1, sizeof(FAFTState));

	if (context && data && output && setting && state) {
		context->data = data;
		context->output = output;
		context->setting = setting;
		context->state = state;

		faft_context_reset(context);
		return context;
	} else {
		free(context);
		free(data);
		free(output);
		free(setting);
		free(state);
		return NULL;
	}
}

void faft_context_delete(FAFTContext *context)
{
	if (context) {
		free(context->data);
		free(context->output);
		free(context->state);
		free(context->setting);
		free(context);
	}
}

void faft_context_reset(FAFTContext *context)
{
	if (context) {
		if (context->data) {
			memset(context->data, 0, sizeof(*(context->data)));

			context->data->config.candPerPage = 10;
			context->data->config.bAutoShiftCur = 1;

			context->data->config.maxChiSymbolLen = context->setting->max_preedit_buffer_len;
			context->data->config.bPhraseChoiceRearward = context->setting->choose_phrase_backward;
		}
		if (context->state) {
			context->state->variety = FAFT_VARIETY_CHI;
			context->state->shape = FAFT_SHAPE_HALF;
			context->state->status = FAFT_STATUS_IDLE;
			context->state->key_status = FAFT_KEY_STATUS_NONE;
			context->state->last_key_status = FAFT_KEY_STATUS_NONE;
			context->state->key_event_status = FAFT_KEY_EVENT_STATUS_NONE;
		}
	}
}

/**
 * Handles Key Event
 */
int faft_context_new_key_event(FAFTContext *ctx)
{
	/* It actually does nothing now XD
	 * call reset!
	 */
	return ctx && faft_context_reset_key_event(ctx);
}
int faft_context_end_key_event(FAFTContext *ctx)
{
	if (!ctx)
		return 0;

	if (faft_context_is_key_event_processed(ctx))
		MakeOutput(ctx->output, ctx->data);
	else
		faft_context_pass_key_event(ctx);

	ctx->state->last_key_status = ctx->state->key_status;


	/* Handles Quick Input (ShortCode & Pre-Input) */
	if (faft_context_is_keycode_string_updated(ctx)) {
		faft_context_produce_quick_input(ctx);
		faft_context_set_quick_input_updated(ctx);
	}

	return 1;
}
/* After a reset, you have to new a key event again before doing any actions */
/* <- now, this is not true actually */
int faft_context_reset_key_event(FAFTContext *ctx)
{
	if (!ctx)
		return 0;

	ctx->state->key_status = FAFT_KEY_STATUS_NONE;
	ctx->state->key_event_status = FAFT_KEY_EVENT_STATUS_NONE;
	return 1;
}
/**
 * Change the state of Key Event directly.
 * Use with caution!
 * Nomarly, you do not need these functions.
 */
inline int faft_context_ignore_key_event(FAFTContext *ctx)
{ return ctx && (ctx->state->key_event_status = FAFT_KEY_EVENT_STATUS_IGNORED); }
inline int faft_context_pass_key_event(FAFTContext *ctx)
{ return ctx && (ctx->state->key_event_status = FAFT_KEY_EVENT_STATUS_PASSED); }
inline int faft_context_accept_key_event(FAFTContext *ctx)
{ return ctx && (ctx->state->key_event_status = FAFT_KEY_EVENT_STATUS_ACCEPTED); }



/**
 * Check the state of Key Event
 */
inline int faft_context_is_key_event_processed(FAFTContext *ctx)
{ return ctx && ctx->state->key_event_status; }

inline int faft_context_is_key_event_accepted(FAFTContext *ctx)
{ return ctx && ctx->state->key_event_status == FAFT_KEY_EVENT_STATUS_ACCEPTED; }
inline int faft_context_is_key_event_ignored(FAFTContext *ctx)
{ return ctx && ctx->state->key_event_status == FAFT_KEY_EVENT_STATUS_IGNORED; }
inline int faft_context_is_key_event_passed(FAFTContext *ctx)
{ return ctx && ctx->state->key_event_status == FAFT_KEY_EVENT_STATUS_PASSED; }





/**
 * Lookup Table
 */

int faft_context_end_lookup_table(FAFTContext *ctx)
{
	if (!faft_context_is_lookup_table_shown(ctx))
		return 0;

	faft_context_accept_key_event(ctx);

	ChoiceEndChoice(ctx->data);
	ctx->state->status = FAFT_STATUS_EDIT;
	faft_context_set_lookup_table_updated(ctx);

	return 1;
}
int faft_context_show_lookup_table(FAFTContext *ctx)
{
	if (!faft_context_is_entering(ctx))
		return 0;

	if (faft_context_is_lookup_table_shown(ctx) || faft_context_is_keycode_entering(ctx)) {
		faft_context_ignore_key_event(ctx);
		return 0;
	}


	int cursor = ctx->data->chiSymbolCursor;

	if (cursor == ctx->data->chiSymbolBufLen)
		--cursor;

	/* Notice this may change the position of cursor */
	if (ChewingIsChiAt(cursor, ctx->data)) {
		ChoiceFirstAvail(ctx->data);

		ctx->state->status = FAFT_STATUS_CHAR_SELECT;

	} else if (ctx->data->symbolKeyBuf[cursor]) {
		OpenSymbolChoice(ctx->data);
		ctx->state->status = FAFT_STATUS_SYMBOL_SELECT;
	}

	if (ctx->data->bSelect) {
		faft_context_accept_key_event(ctx);
		faft_context_set_lookup_table_updated(ctx);
		return 1;

	} else {
		ctx->state->status = FAFT_STATUS_EDIT;
		faft_context_ignore_key_event(ctx);
		return 0;
	}
}

int faft_context_first_avail(FAFTContext *ctx)
{
	if (!faft_context_is_lookup_table_shown(ctx))
		return 0;


	int first = ctx->data->availInfo.nAvail - 1;
	if (ctx->data->choiceInfo.isSymbol || ctx->data->availInfo.currentAvail == first) {
		faft_context_ignore_key_event(ctx);
		return 0;
	}


	ctx->data->availInfo.currentAvail = first;
	SetChoiceInfo(
			&(ctx->data->choiceInfo), 
			&(ctx->data->availInfo), 
			ctx->data->phoneSeq,
			PhoneSeqCursor(ctx->data),
			ctx->data->config.candPerPage);

	faft_context_set_lookup_table_updated(ctx);
	faft_context_accept_key_event(ctx);

	return 1;
}

int faft_context_last_avail(FAFTContext *ctx)
{
	if (!faft_context_is_lookup_table_shown(ctx))
		return 0;


	int last = 0;
	if (ctx->data->choiceInfo.isSymbol || ctx->data->availInfo.currentAvail == last) {
		faft_context_ignore_key_event(ctx);
		return 0;
	}


	ctx->data->availInfo.currentAvail = last;
	SetChoiceInfo(
			&(ctx->data->choiceInfo), 
			&(ctx->data->availInfo), 
			ctx->data->phoneSeq,
			PhoneSeqCursor(ctx->data),
			ctx->data->config.candPerPage);

	faft_context_set_lookup_table_updated(ctx);
	faft_context_accept_key_event(ctx);

	return 1;
}
int faft_context_prev_avail(FAFTContext *ctx)
{
	if (!faft_context_is_lookup_table_shown(ctx))
		return 0;



	int first = ctx->data->availInfo.nAvail - 1;
	if (ctx->data->choiceInfo.isSymbol || ctx->data->availInfo.currentAvail == first) {
		faft_context_ignore_key_event(ctx);
		return 0;
	}


	++(ctx->data->availInfo.currentAvail);
	SetChoiceInfo(
			&(ctx->data->choiceInfo), 
			&(ctx->data->availInfo), 
			ctx->data->phoneSeq,
			PhoneSeqCursor(ctx->data),
			ctx->data->config.candPerPage);
	faft_context_set_lookup_table_updated(ctx);
	faft_context_accept_key_event(ctx);
	return 1;
}
int faft_context_next_avail(FAFTContext *ctx)
{
	if (!faft_context_is_lookup_table_shown(ctx))
		return 0;



	int last = 0;
	if (ctx->data->choiceInfo.isSymbol || ctx->data->availInfo.currentAvail == last) {
		faft_context_ignore_key_event(ctx);
		return 0;
	}


	--(ctx->data->availInfo.currentAvail);
	SetChoiceInfo(
			&(ctx->data->choiceInfo), 
			&(ctx->data->availInfo), 
			ctx->data->phoneSeq,
			PhoneSeqCursor(ctx->data),
			ctx->data->config.candPerPage);

	faft_context_set_lookup_table_updated(ctx);
	faft_context_accept_key_event(ctx);

	return 1;
}

int faft_context_first_page(FAFTContext *ctx)
{
	if (!faft_context_is_lookup_table_shown(ctx))
		return 0;


	if (ctx->data->choiceInfo.pageNo > 0) {
		ctx->data->choiceInfo.pageNo = 0;
		faft_context_set_lookup_table_updated(ctx);
		faft_context_accept_key_event(ctx);
		return 1;
	}

	faft_context_ignore_key_event(ctx);

	return 0;
}
int faft_context_last_page(FAFTContext *ctx)
{
	if (!faft_context_is_lookup_table_shown(ctx))
		return 0;


	if (ctx->data->choiceInfo.pageNo < ctx->data->choiceInfo.nPage - 1) {
		ctx->data->choiceInfo.pageNo = ctx->data->choiceInfo.nPage - 1;
		faft_context_set_lookup_table_updated(ctx);
		faft_context_accept_key_event(ctx);
		return 1;
	}

	faft_context_ignore_key_event(ctx);

	return 0;
}
int faft_context_prev_page(FAFTContext *ctx)
{
	if (!faft_context_is_lookup_table_shown(ctx))
		return 0;


	if (ctx->data->choiceInfo.pageNo > 0) {
		--(ctx->data->choiceInfo.pageNo);

		faft_context_set_lookup_table_updated(ctx);
		faft_context_accept_key_event(ctx);
		return 1;
	}

	faft_context_ignore_key_event(ctx);

	return 0;
}
int faft_context_next_page(FAFTContext *ctx)
{
	if (!faft_context_is_lookup_table_shown(ctx))
		return 0;


	if (ctx->data->choiceInfo.pageNo < ctx->data->choiceInfo.nPage - 1) {
		++(ctx->data->choiceInfo.pageNo);
		faft_context_set_lookup_table_updated(ctx);
		faft_context_accept_key_event(ctx);
		return 1;
	}

	faft_context_ignore_key_event(ctx);

	return 0;
}

int faft_context_select_cand(FAFTContext *ctx, int nth)
{
	if (!faft_context_is_lookup_table_shown(ctx))
		return 0;
	if (ctx->state->status == FAFT_STATUS_PHRASE_INPUT) {
		nth += ctx->data->choiceInfo.pageNo * ctx->data->choiceInfo.nChoicePerPage;
		if (nth < ctx->data->choiceInfo.nTotalChoice) {
			char *s = ctx->data->choiceInfo.totalChoiceStr[nth];
			char w[MAX_UTF8_SIZE + 1];
			int len = ueStrLen(s);
			int i;
			for (i=0; i < len; ++i) {
				ueStrNCpy(w, s, 1, STRNCPY_CLOSE);
				s = ueStrSeek(s, 1);

				ArrayCode ac;
				if (ac = faft_table_get_ac(w))
					faft_data_add_fixed(w, ctx->data, ac);
				else
					faft_data_add_symbol(w, ctx->data);

				CallPhrasing(ctx->data);
				if(faft_context_release_buffer(ctx))
					CallPhrasing(ctx->data);
			}
			ChoiceEndChoice(ctx->data);

			faft_context_set_lookup_table_updated(ctx);

			faft_acdata_remove_all(&(ctx->data->acData));
			faft_context_set_buffer_updated(ctx);
			faft_context_set_keycode_string_updated(ctx);

			faft_context_accept_key_event(ctx);
			return 1;
		} else
			faft_context_ignore_key_event(ctx);
		return 0;

	}

	if (DoSelect(ctx->data, nth)) {
		if (!ctx->data->bSelect) {
			CallPhrasing(ctx->data);
			faft_context_set_buffer_updated(ctx);
		}

		faft_context_set_lookup_table_updated(ctx);

		/* This may not be true, but we still call it anyway */
		faft_context_set_buffer_updated(ctx);

		faft_context_accept_key_event(ctx);

		return 1;
	}

	faft_context_ignore_key_event(ctx);

	return 0;
}
int faft_context_any_row_select(FAFTContext *ctx, int keysym)
{
	if (!faft_context_is_lookup_table_shown(ctx) || !faft_context_is_any_row_key_select(ctx))
		return 0;

	FAFTKeyCode kc = faft_kbtype_get_kc(keysym, ctx->setting->kbtype);
	if (!valid_kc(kc) || kc == KEYCODE_END)
		return 0;

	int nth = (kc + 9) % 10;

	return faft_context_select_cand(ctx, nth);
}

/**
 * Input
 *
 *
 */
int faft_context_input_keycode(FAFTContext *ctx, int keysym)
{
	if (faft_context_get_variety(ctx) == FAFT_VARIETY_ENG)
		return 0;

	if (faft_context_is_lookup_table_shown(ctx)) {
		faft_context_ignore_key_event(ctx);
		return 0;
	}

	FAFTKeyCode kc = faft_kbtype_get_kc(keysym, ctx->setting->kbtype);
	if (!valid_kc(kc))
		return 0;


	if (faft_acdata_is_full(&(ctx->data->acData))) {
		if (faft_context_is_auto_input_mode(ctx)) {
			faft_context_input_char(ctx);
			/* Note that even if the ArrayCode is wrong,
			 * auto-clear may be activated, so we still can put this keycode into buffer*/
			if (faft_acdata_is_full(&(ctx->data->acData))) {
				faft_context_ignore_key_event(ctx);
				return 0;
			}
		} else {
			faft_context_ignore_key_event(ctx);
			ctx->state->key_status = FAFT_KEY_STATUS_IGNORED;

			return 0;
		}
	}

	/* acdata may not be full, or auto-clear, auto-input may have happened */
	faft_acdata_input(&(ctx->data->acData), kc);

	faft_context_set_keycode_string_updated(ctx);
	faft_context_accept_key_event(ctx);

	ctx->state->key_status = FAFT_KEY_STATUS_ACCEPTED;
	return 1;
}
int faft_context_input_char(FAFTContext *ctx)
{
	if (faft_context_get_variety(ctx) == FAFT_VARIETY_ENG)
		return 0;

	if (faft_context_is_lookup_table_shown(ctx)) {
		faft_context_ignore_key_event(ctx);
		return 0;
	}

	if (faft_context_is_keycode_entering(ctx)) {
		faft_acdata_input(&(ctx->data->acData), KEYCODE_END);

		// See if there is special
		if (faft_table_get_special(NULL, ctx->data->acData.ac)) {
			faft_data_add_special(ctx->data->acData.ac, ctx->data);

			// See if there is chi
		} else if (faft_table_get_char_first(NULL, ctx->data->acData.ac)) {
			AddChi(ctx->data->acData.ac, ctx->data);

		} else {
			if (ctx->state->last_key_status == FAFT_KEY_STATUS_AC_ERROR
					&& faft_context_is_auto_clear_mode(ctx)) {
				faft_acdata_remove_all(&(ctx->data->acData));
				faft_context_accept_key_event(ctx);
				faft_context_set_keycode_string_updated(ctx);

				ctx->state->key_status = FAFT_KEY_STATUS_ACCEPTED;

			} else {
				faft_context_ignore_key_event(ctx);
				ctx->state->key_status = FAFT_KEY_STATUS_AC_ERROR;
			}

			return 0;
		}

		ctx->state->key_status = FAFT_KEY_STATUS_COMMIT;

		CallPhrasing(ctx->data);
		if(faft_context_release_buffer(ctx))
			CallPhrasing(ctx->data);

		faft_acdata_remove_all(&(ctx->data->acData));
		faft_context_set_keycode_string_updated(ctx);
		faft_context_set_buffer_updated(ctx);

		faft_context_accept_key_event(ctx);

		return 1;

	} else
		return 0;
}

int faft_context_input_symbol(FAFTContext *ctx, int keysym)
{
	/* Skip the special key */
	if (keysym & 0xFF00)
		return 0;

	/* Try auto_input if there is keycodes */
	if (faft_context_is_auto_input_mode(ctx) && !faft_context_is_lookup_table_shown(ctx)
			&& faft_context_is_keycode_entering(ctx) && isprint(keysym))
		faft_context_input_char(ctx);
	/* Notice auto-clear may happen also */

	/* Notice if auto-input happens, there may be something committed because of the limit of buffer's length.
	 * This have to be processed carefully, or the string would be lost if we commit for the second time.
	 * To work around this, we depend on the faft_set_commit_string_updated /unset_updated functions
	 * to change the behaviour of faft_context_commit(),
	 * 
	 * Carefully!! Everytime you commit something, call set_commit_string_updated!
	 * and after you retrieve the commit string from frontend, you must call unset_commit_string !!
	 */


	/* If in Eng mode, KeyCodes would be cleared no whether in auto-clear or not.
	 * And lookup table is also cleared
	 * If the user call input_symbol before calling any_row_select in Eng mode,
	 * auto_select would never have chance to be activated.
	 */
	if (ctx->state->variety == FAFT_VARIETY_ENG
			&& (faft_context_is_keycode_entering(ctx) || faft_context_is_lookup_table_shown(ctx))) {
		faft_context_end_lookup_table(ctx);
		faft_context_clear_keycode(ctx);
	}

	/* This means we are in Ch mode & input_symbol is not possible */
	if (faft_context_is_keycode_entering(ctx)
			|| faft_context_is_lookup_table_shown(ctx)) {
		faft_context_ignore_key_event(ctx);
		return 0;
	}



	if (ctx->state->variety == FAFT_VARIETY_ENG) {
		if (faft_context_is_easy_symbol_input(ctx)) {
			/* We ignore it here since we may want to pass it if in plain Eng mode */
			if (!isprint(keysym)) {
				faft_context_ignore_key_event(ctx);
				return 0;
			}

			if (ctx->state->shape == FAFT_SHAPE_HALF)
				SymbolInput(keysym, ctx->data);
			else {
				if (SpecialSymbolInput(keysym, ctx->data) == ZUIN_IGNORE)
					FullShapeSymbolInput(keysym, ctx->data);

			}

			/* Go to end */
		} else {

			/* If in Eng mode and the easy_symbol_input is not activated,
			 * goes directly into plain eng mode */

			if (faft_context_is_buffer_entering(ctx))
				faft_context_commit(ctx);

			faft_context_pass_key_event(ctx);
			return 0;
		}
	} else {
		if (faft_context_is_space_to_select(ctx) && keysym == ' ' && faft_context_is_buffer_entering(ctx)) {
			faft_context_show_lookup_table(ctx);
			/* Is this behaviour right?, should we return the return value of show_table
			 * or, should we move space_to_select out of this function?
			 */
			return 0;
		}
		if (!isprint(keysym)) {
			faft_context_ignore_key_event(ctx);
			return 0;
		}

		if (ctx->state->shape == FAFT_SHAPE_HALF)
			SymbolInput(keysym, ctx->data);
		else {
			if (SpecialSymbolInput(keysym, ctx->data) == ZUIN_IGNORE)
				FullShapeSymbolInput(keysym, ctx->data);

		}
			/* Go to end */
	}

	CallPhrasing(ctx->data);

	if(faft_context_release_buffer(ctx))
		CallPhrasing(ctx->data);

	faft_context_accept_key_event(ctx);
	faft_context_set_buffer_updated(ctx);

	return 1;
}

int faft_context_input_quick(FAFTContext *ctx, int nth)
{
	if (ctx->data->quick.present && ctx->data->quick.ac[nth]) {

		int i_p = PhoneSeqCursor(ctx->data);
		AddChi(ctx->data->quick.ac[nth], ctx->data);

		// Add selected character
		ueStrNCpy(ctx->data->selectStr[ctx->data->nSelect], ctx->data->quick.string[nth], 1, STRNCPY_CLOSE);
		// Add one character interval
		ctx->data->selectInterval[ctx->data->nSelect].from = i_p;
		ctx->data->selectInterval[ctx->data->nSelect].to = i_p+1;
		++(ctx->data->nSelect);
		CallPhrasing(ctx->data);

		faft_acdata_remove_all(&(ctx->data->acData));
		faft_context_set_buffer_updated(ctx);
		faft_context_set_keycode_string_updated(ctx);
		faft_context_accept_key_event(ctx);

		if(faft_context_release_buffer(ctx))
			CallPhrasing(ctx->data);

		return 1;

	} else 
		return 0;
}
int faft_context_input_phrase(FAFTContext *ctx)
{
	if (faft_context_get_variety(ctx) == FAFT_VARIETY_ENG
			|| !faft_context_is_phrase_input(ctx))
		return 0;

	if (faft_context_is_lookup_table_shown(ctx)) {
		faft_context_ignore_key_event(ctx);
		return 0;
	}

	if (faft_context_is_keycode_entering(ctx)) {
		faft_acdata_input(&(ctx->data->acData), KEYCODE_END);

		// See if there is phrase
		if (faft_table_get_phrase_first_strn(NULL, 0, ctx->data->acData.ac)) {
			ctx->data->choiceInfo.oldChiSymbolCursor = ctx->data->chiSymbolCursor;
			faft_util_produce_phrase_input(&(ctx->data->choiceInfo), &(ctx->data->availInfo));
			ctx->data->bSelect = 1;
			ctx->state->key_status = FAFT_KEY_STATUS_ACCEPTED;
			ctx->state->status = FAFT_STATUS_PHRASE_INPUT;

			faft_context_set_buffer_updated(ctx);
			faft_context_set_lookup_table_updated(ctx);

			faft_context_accept_key_event(ctx);

			return 1;
		} else {
			if (ctx->state->last_key_status == FAFT_KEY_STATUS_AC_ERROR
					&& faft_context_is_auto_clear_mode(ctx)) {
				faft_acdata_remove_all(&(ctx->data->acData));
				faft_context_accept_key_event(ctx);
				faft_context_set_keycode_string_updated(ctx);

				ctx->state->key_status = FAFT_KEY_STATUS_ACCEPTED;

			} else {
				faft_context_ignore_key_event(ctx);
				ctx->state->key_status = FAFT_KEY_STATUS_AC_ERROR;
			}

			return 0;
		}


	} else
		return 0;
}
int faft_context_input_array_symbol(FAFTContext *ctx)
{
	if (faft_context_get_variety(ctx) == FAFT_VARIETY_ENG)
		return 0;

	if (faft_context_is_lookup_table_shown(ctx)) {
		faft_context_ignore_key_event(ctx);
		return 0;
	}

	if (faft_context_is_keycode_entering(ctx)) {
		faft_acdata_input(&(ctx->data->acData), KEYCODE_END);

		// See if there is array symbol
		Word w;
		if (faft_table_get_symbol(&w, ctx->data->acData.ac)) {
			faft_data_add_symbol(w.word, ctx->data);
			CallPhrasing(ctx->data);

		} else {
			if (ctx->state->last_key_status == FAFT_KEY_STATUS_AC_ERROR
					&& faft_context_is_auto_clear_mode(ctx)) {
				faft_acdata_remove_all(&(ctx->data->acData));
				faft_context_accept_key_event(ctx);
				faft_context_set_keycode_string_updated(ctx);

				ctx->state->key_status = FAFT_KEY_STATUS_ACCEPTED;

			} else {
				faft_context_ignore_key_event(ctx);
				ctx->state->key_status = FAFT_KEY_STATUS_AC_ERROR;
			}

			return 0;
		}

		ctx->state->key_status = FAFT_KEY_STATUS_COMMIT;

		if(faft_context_release_buffer(ctx))
			CallPhrasing(ctx->data);

		faft_acdata_remove_all(&(ctx->data->acData));
		faft_context_set_keycode_string_updated(ctx);
		faft_context_set_buffer_updated(ctx);

		faft_context_accept_key_event(ctx);

		return 1;

	} else
		return 0;
}

/**
 * Buffer
 *
 */

int faft_context_clear_all_buffer(FAFTContext *ctx)
{
	return faft_context_clear_aux(ctx)
		|| faft_context_end_lookup_table(ctx)
		|| faft_context_clear_keycode(ctx)
		|| faft_context_clear_buffer(ctx);
}
int faft_context_clear_aux(FAFTContext *ctx)
{
	return 1;
}

int faft_context_clear_keycode(FAFTContext *ctx)
{
	if (faft_context_is_lookup_table_shown(ctx)) {
		faft_context_ignore_key_event(ctx);
		return 0;
	} else if (faft_context_is_keycode_entering(ctx)) {
		faft_acdata_remove_all(&(ctx->data->acData));

		faft_context_accept_key_event(ctx);
		faft_context_set_keycode_string_updated(ctx);
		return 1;
	} else
		return 0;
}
int faft_context_remove_last_keycode(FAFTContext *ctx)
{
	if (faft_context_is_lookup_table_shown(ctx)) {
		faft_context_ignore_key_event(ctx);
		return 0;
	} else if (faft_context_is_keycode_entering(ctx)) {
		faft_acdata_remove_last(&(ctx->data->acData));

		faft_context_accept_key_event(ctx);
		faft_context_set_keycode_string_updated(ctx);
		return 1;
	} else
		return 0;
}

int faft_context_clear_buffer(FAFTContext *ctx)
{
	if (faft_context_is_lookup_table_shown(ctx)
			|| faft_context_is_keycode_entering(ctx)) {
		faft_context_ignore_key_event(ctx);
		return 0;
	} else if (faft_context_is_buffer_entering(ctx)) {
		CleanAllBuf(ctx->data);
		faft_context_accept_key_event(ctx);
		faft_context_set_buffer_updated(ctx);
		return 1;
	} else
		return 0;
}
int faft_context_remove_word_backward(FAFTContext *ctx)
{
	if (faft_context_is_lookup_table_shown(ctx)
			|| faft_context_is_keycode_entering(ctx)) {
		faft_context_ignore_key_event(ctx);
		return 0;
	} else if (faft_context_is_buffer_entering(ctx)) {
		ChewingKillChar(ctx->data,
				ctx->data->chiSymbolCursor - 1,
				DECREASE_CURSOR);
		CallPhrasing(ctx->data);

		faft_context_accept_key_event(ctx);
		faft_context_set_buffer_updated(ctx);
		return 1;
	} else
		return 0;
}
int faft_context_remove_word_forward(FAFTContext *ctx)
{
	if (faft_context_is_lookup_table_shown(ctx)
			|| faft_context_is_keycode_entering(ctx)) {
		faft_context_ignore_key_event(ctx);
		return 0;
	} else if (faft_context_is_buffer_entering(ctx)) {
		if (ctx->data->chiSymbolCursor < ctx->data->chiSymbolBufLen) {
			ChewingKillChar(ctx->data,
					ctx->data->chiSymbolCursor,
					NONDECREASE_CURSOR);
			CallPhrasing(ctx->data);

			faft_context_accept_key_event(ctx);
			faft_context_set_buffer_updated(ctx);

			return 1;
		} else {
			faft_context_ignore_key_event(ctx);
			return 0;
		}
	} else
		return 0;
}

int faft_context_move_cursor_forward(FAFTContext *ctx)
{
	if (faft_context_is_lookup_table_shown(ctx)
			|| faft_context_is_keycode_entering(ctx)) {
		faft_context_ignore_key_event(ctx);
		return 0;
	} else if (faft_context_is_buffer_entering(ctx)) {
		if (ctx->data->chiSymbolCursor < ctx->data->chiSymbolBufLen) {

			++(ctx->data->chiSymbolCursor);

			faft_context_accept_key_event(ctx);
			faft_context_set_buffer_updated(ctx);

			return 1;
		} else {
			faft_context_ignore_key_event(ctx);
			return 0;
		}
	} else
		return 0;
}
int faft_context_move_cursor_backward(FAFTContext *ctx)
{
	if (faft_context_is_lookup_table_shown(ctx)
			|| faft_context_is_keycode_entering(ctx)) {
		faft_context_ignore_key_event(ctx);
		return 0;
	} else if (faft_context_is_buffer_entering(ctx)) {
		if (ctx->data->chiSymbolCursor > 0) {

			--(ctx->data->chiSymbolCursor);

			faft_context_accept_key_event(ctx);
			faft_context_set_buffer_updated(ctx);

			return 1;
		} else {
			faft_context_ignore_key_event(ctx);
			return 0;
		}
	} else
		return 0;
}
int faft_context_move_cursor_to_front(FAFTContext *ctx)
{
	if (faft_context_is_lookup_table_shown(ctx)
			|| faft_context_is_keycode_entering(ctx)) {
		faft_context_ignore_key_event(ctx);
		return 0;
	} else if (faft_context_is_buffer_entering(ctx)) {
		if (ctx->data->chiSymbolCursor > 0) {

			ctx->data->chiSymbolCursor = 0;

			faft_context_accept_key_event(ctx);
			faft_context_set_buffer_updated(ctx);

			return 1;
		} else {
			faft_context_ignore_key_event(ctx);
			return 0;
		}
	} else
		return 0;
}
int faft_context_move_cursor_to_end(FAFTContext *ctx)
{
	if (faft_context_is_lookup_table_shown(ctx)
			|| faft_context_is_keycode_entering(ctx)) {
		faft_context_ignore_key_event(ctx);
		return 0;
	} else if (faft_context_is_buffer_entering(ctx)) {
		if (ctx->data->chiSymbolCursor < ctx->data->chiSymbolBufLen) {

			ctx->data->chiSymbolCursor = ctx->data->chiSymbolBufLen -1;

			faft_context_accept_key_event(ctx);
			faft_context_set_buffer_updated(ctx);

			return 1;
		} else {
			faft_context_ignore_key_event(ctx);
			return 0;
		}
	} else
		return 0;
}
int faft_context_toggle_break_point(FAFTContext *ctx)
{
	if (faft_context_is_lookup_table_shown(ctx)
			|| faft_context_is_keycode_entering(ctx)) {
		faft_context_ignore_key_event(ctx);
		return 0;
	} else if (faft_context_is_buffer_entering(ctx)) {
		if (ctx->data->chiSymbolCursor == ctx->data->chiSymbolBufLen) {
			ctx->data->phrOut.nNumCut++;
		} else if (ChewingIsChiAt(ctx->data->chiSymbolCursor - 1, ctx->data)) {
			int cursor = PhoneSeqCursor(ctx->data);
			if (IsPreferIntervalConnted(cursor, ctx->data)) {
				ctx->data->bUserArrBrkpt[ cursor ] = 1;
				ctx->data->bUserArrCnnct[ cursor ] = 0;
			} else {
				ctx->data->bUserArrBrkpt[ cursor ] = 0;
				ctx->data->bUserArrCnnct[ cursor ] = 1;
			}
		}
		CallPhrasing(ctx->data);

		faft_context_accept_key_event(ctx);
		faft_context_set_buffer_updated(ctx);

		return 1;
	} else
		return 0;

}

/**
 * Functions
 *
 */
int faft_context_commit(FAFTContext *ctx)
{
	if (faft_context_is_lookup_table_shown(ctx)
			|| faft_context_is_keycode_entering(ctx)) {
		faft_context_ignore_key_event(ctx);
		return 0;

	} else if (faft_context_is_buffer_entering(ctx)) {
		int nCommitStr = ctx->data->chiSymbolBufLen;
		int old_nCommitStr = ctx->output->nCommitStr;
		wch_t *to_commit = ctx->output->commitStr;

		if (old_nCommitStr + nCommitStr <= MAX_PHONE_SEQ_LEN
				&& faft_context_is_commit_string_updated(ctx))
			to_commit+=old_nCommitStr;
		else
			old_nCommitStr = 0;


		WriteChiSymbolToBuf(to_commit, nCommitStr, ctx->data);
		CleanAllBuf(ctx->data);  
		ctx->output->nCommitStr = nCommitStr + old_nCommitStr;

		faft_context_accept_key_event(ctx);
		faft_context_set_buffer_updated(ctx);
		faft_context_set_commit_string_updated(ctx);

		return 1;
	} else
		return 0;
}
int faft_context_open_symbol_table(FAFTContext *ctx)
{
	if (ctx->state->variety == FAFT_VARIETY_ENG)
		return 0;

	if (faft_context_is_lookup_table_shown(ctx)
			|| faft_context_is_keycode_entering(ctx)) {
		faft_context_ignore_key_event(ctx);
		return 0;
	}

	ctx->data->bSelect = 1;
	ctx->data->choiceInfo.oldChiSymbolCursor = ctx->data->chiSymbolCursor;
	HaninSymbolInput(&(ctx->data->choiceInfo),
			&(ctx->data->availInfo),
			ctx->data->phoneSeq,
			ctx->data->config.candPerPage);
	faft_context_set_lookup_table_updated(ctx);
	faft_context_accept_key_event(ctx);
}
int faft_context_toggle_variety(FAFTContext *ctx)
{
	if (ctx && ctx->state)
		if (ctx->state->variety == FAFT_VARIETY_CHI)
			ctx->state->variety = FAFT_VARIETY_ENG;
		else
			ctx->state->variety = FAFT_VARIETY_CHI;
	faft_context_accept_key_event(ctx);
	return 1;
}
int faft_context_toggle_shape(FAFTContext *ctx)
{
	if (ctx && ctx->state)
		if (ctx->state->shape == FAFT_SHAPE_HALF)
			ctx->state->shape = FAFT_SHAPE_FULL;
		else
			ctx->state->shape = FAFT_SHAPE_HALF;
	faft_context_accept_key_event(ctx);
	return 1;
}
