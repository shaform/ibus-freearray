/**
 * A very simple program indeed, to test the faft IM without ibus-faft.
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
#include <stdio.h>
#include <stdlib.h>
#include <ibus.h>
#include "faft/faft.h"

#define IBUS_FAFT_ENGINE(s)	((s)->context)

typedef struct {
	FAFTContext *context;
} DummyEngine;

gboolean get_input(guint *keysym, guint *modifiers);
void ibus_faft_init(DummyEngine *);
/* Print out updates */
void ibus_faft_update(DummyEngine *);
void ibus_faft_update_lookup_table(DummyEngine *);
void ibus_faft_update_preedit_string(DummyEngine *c);

gboolean ibus_faft_engine_process_key_event(DummyEngine *engine, guint keysym, guint keycode, guint modifiers);

int main() {
	/* Init */
	DummyEngine engine;

	ibus_faft_init(&engine);

	char input_c;
	int input_d;
	guint keysym, modifiers;

	while (get_input(&keysym, &modifiers)) {
		if (ibus_faft_engine_process_key_event(&engine, keysym, 0, modifiers))
			printf("KeyEvent processed!\n");
		else
			printf("KeyEvent passed!\n");
	}
	faft_exit();


}


gboolean ibus_faft_engine_process_key_event(DummyEngine *faft, guint keysym, guint keycode, guint modifiers)
{

	/* Data */
	FAFTContext *context = faft->context;

	static gboolean SHIFTER = FALSE;

	/* Ignore all release events except for Shift-L*/
	if (modifiers & IBUS_RELEASE_MASK) {
		if (SHIFTER && keysym == IBUS_Shift_L) {
			// Toggle eng/chi
			faft_context_toggle_variety(context);

			faft_context_end_key_event(context);
			ibus_faft_refresh_property(faft, "faft_variety_prop");
			SHIFTER = FALSE;
			return TRUE;
		} else
			return FALSE;
	}




	guint keymask = modifiers & (IBUS_SHIFT_MASK | IBUS_CONTROL_MASK | IBUS_MOD1_MASK);

	/* A new key_event has happened */
	faft_context_new_key_event(context);

	SHIFTER = FALSE;
	if (keymask == 0) {
		switch (keysym){
			case IBUS_Return:
			case IBUS_KP_Enter:
				if (faft_context_is_lookup_table_shown(context)) {
					faft_context_reset_key_event(context);
					return ibus_faft_handle_pagedown(faft);
				} else
					faft_context_commit(context);
				/**
				 * Note: If cannot commit, it is ignored
				 * if no buffer or anything, it will not be processed
				 */

				break;
			case IBUS_Escape:
				faft_context_end_lookup_table(context);
				if (!faft_context_is_key_event_processed(context))
					faft_context_clear_keycode(context);

				if (!faft_context_is_key_event_processed(context)
						&& faft_context_is_esc_clear_all_buff(context))
					faft_context_clear_buffer(context);
				/**
				 * Note: It still can be ignored or not processed depending on the condition.
				 */

				break;
			case IBUS_BackSpace:
				faft_context_remove_last_keycode(context);
				/* Note: if a lookup table is up, it will be ignored 
				 * If the look up table is Short Code it may be acceptted
				 * All these means that it is processed
				 */
				if (!faft_context_is_key_event_processed(context))
					faft_context_remove_word_backward(context);
				/**
				 * Note: It still can be not processed depending on the condition.
				 */
				break;
			case IBUS_Delete:
			case IBUS_KP_Delete:
				/* This is the only possibility */
				faft_context_remove_word_forward(context);
				break;
			case IBUS_space:
				faft_context_input_char(context);

				if (!faft_context_is_key_event_processed(context))
					faft_context_select_cand(context, 0);

				/**
				 * This may activate space_to_select
				 * FIXME:
				 * Whether space_to_select should be activated by this implicitly,
				 * or by a explicit function should be thounght more clearly. 
				 */
				if (!faft_context_is_key_event_processed(context))
					faft_context_input_symbol(context, keysym);
				break;



				/* Merge these with ibus class methods */
			case IBUS_Page_Up: case IBUS_KP_Page_Up:
				faft_context_reset_key_event(context);
				return ibus_faft_handle_pageup(faft);
				break;

			case IBUS_Page_Down: case IBUS_KP_Page_Down:
				faft_context_reset_key_event(context);
				return ibus_faft_handle_pagedown(faft);
				break;

			case IBUS_Left: case IBUS_KP_Left:
				faft_context_reset_key_event(context);
				return ibus_faft_handle_left(faft);
				break;

			case IBUS_Right: case IBUS_KP_Right:
				faft_context_reset_key_event(context);
				return ibus_faft_handle_right(faft);
				break;



			case IBUS_Up:
			case IBUS_KP_Up:
				faft_context_prev_avail(context);

				if (faft_context_is_key_event_ignored(context))
					faft_context_last_avail(context);

				/* We must do this since if a lookup table is not present,
				 * all actions above about a lookup table would not be processed
				 */ 
				if (!faft_context_is_key_event_processed(context)
						&& faft_context_is_entering(context))
					faft_context_ignore_key_event(context);

				break;
			case IBUS_Down:
			case IBUS_KP_Down:
				faft_context_next_avail(context);

				if (faft_context_is_key_event_ignored(context))
					faft_context_first_avail(context);

				if (!faft_context_is_key_event_processed(context))
					faft_context_show_lookup_table(context);


				break;
			case IBUS_Home:
			case IBUS_KP_Home:
				faft_context_first_page(context);
				if (!faft_context_is_key_event_processed(context))
					faft_context_move_cursor_to_front(context);

				break;
			case IBUS_End:
			case IBUS_KP_End:
				faft_context_last_page(context);
				if (!faft_context_is_key_event_processed(context))
					faft_context_move_cursor_to_end(context);

				break;
			case IBUS_Tab:
			case IBUS_KP_Tab:
				faft_context_toggle_break_point(context);
				break;
			case IBUS_Shift_L:
				SHIFTER = TRUE;
				break;
			default:
				if (keysym >= IBUS_0 && keysym <= IBUS_9) {
					int nth = ((keysym-IBUS_0)+9)%10;
					faft_context_select_cand(context, nth);

					if (!faft_context_is_key_event_processed(context))
						faft_context_input_quick(context, nth);

					if (!faft_context_is_key_event_processed(context))
					faft_context_input_symbol(context, keysym);

					/* Check KPs */
				} else if (keysym >= IBUS_KP_0 && keysym <= IBUS_KP_9)
					faft_context_input_symbol(context, keysym - IBUS_KP_0 + IBUS_0);

				else if (keysym == IBUS_KP_Decimal)
					faft_context_input_symbol(context, IBUS_period);

				else if (keysym == IBUS_KP_Add)
					faft_context_input_symbol(context, IBUS_plus);

				else if (keysym ==IBUS_KP_Subtract )
					faft_context_input_symbol(context, IBUS_minus);

				else if (keysym == IBUS_KP_Multiply)
					faft_context_input_symbol(context, IBUS_asterisk);

				else if (keysym == IBUS_KP_Divide)
					faft_context_input_symbol(context, IBUS_slash);

				else if (keysym == IBUS_KP_Space)
					faft_context_input_symbol(context, IBUS_space);

				else if (keysym == IBUS_KP_Equal)
					faft_context_input_symbol(context, IBUS_equal);

				else {
					if (faft_context_is_special_end_key(context, keysym))
						faft_context_input_phrase(context);
					else if (keysym == '`')
						faft_context_open_symbol_table(context);

					if (!faft_context_is_key_event_processed(context))
						faft_context_any_row_select(context, keysym);

					if (!faft_context_is_key_event_processed(context))
						faft_context_input_keycode(context, keysym);

					if (!faft_context_is_key_event_processed(context))
						faft_context_input_symbol(context, keysym);

				}
				break;
		}
	} else if (keymask == IBUS_SHIFT_MASK) {
		switch(keysym) {

			case IBUS_space:
				// Toggle half/Full
				faft_context_toggle_shape(context);

				faft_context_end_key_event(context);
				ibus_faft_refresh_property(faft, "faft_shape_prop");
				return TRUE;
				break;
			default:

				if (faft_context_is_special_end_key(context, keysym))
					faft_context_input_array_symbol(context);

				if (!faft_context_is_key_event_processed(context))
					faft_context_input_symbol(context, keysym);
		}
	}

	faft_context_end_key_event(context);

	/* Notice even a key_event has been passed, some data may still be updated.
	 * You should check is_key_event_processed before you end the key event to make sure.
	 * Here, we simply do update on every case, and let the update function check is_updated. 
	 */
	ibus_faft_update(faft);

	if (faft_context_is_key_event_passed(context))
		return FALSE;
	else
		return TRUE;

}
void ibus_faft_init(DummyEngine *faft)
{
	if (!faft)
		return;

	faft_init( PKGDATADIR "/data");
	faft->context = faft_context_new();
	FAFTContext *ctx = faft->context;

	/* Default Settings */
	faft_context_set_kbtype(ctx, FREEARRAY_KBTYPE_DVORAK);
	faft_context_set_auto_input_mode(ctx);
	faft_context_set_auto_clear_mode(ctx);
	faft_context_unset_esc_clear_all_buff(ctx);
	faft_context_set_space_to_select(ctx);
	faft_context_set_any_row_key_select(ctx);
	faft_context_set_choose_phrase_backward (ctx);
	faft_context_set_short_code_input(ctx);
	faft_context_set_pre_input(ctx);
	faft_context_set_phrase_input(ctx);
	faft_context_set_array_symbol_input(ctx);
	faft_context_set_easy_symbol_input(ctx);
	faft_context_set_max_preedit_buffer_len(ctx, 20);
}
void ibus_faft_update(DummyEngine *c)
{
	FAFTContext *ctx = c->context;

	/* Update Lookup Table / Quick Input */
	if (faft_context_is_quick_input_updated(ctx) || faft_context_is_lookup_table_updated(ctx))
		printf("Lookup table is updated!\n");
	
	ibus_faft_update_lookup_table(c);


	/* Update Pre-edit String */
	if (faft_context_is_buffer_updated(ctx)|| faft_context_is_keycode_string_updated(ctx))
		printf("Preedit is updated!\n");

	ibus_faft_update_preedit_string(c);

	

	/* See if committed */
	if (faft_context_is_commit_string_updated(ctx)) {
		printf("Commit: ");
		char *s = faft_context_get_commit_string(ctx);
		printf("%s\n", s);
		free(s);
	} else
		printf("Didn't commit\n");

	faft_context_unset_commit_string_updated(ctx);
}
void ibus_faft_update_lookup_table(DummyEngine *c)
{
	FAFTContext *ctx = c->context;
	printf("Lookup table: ");
	int i, page_size=0;
	char *candidate = NULL;

	if (faft_context_is_lookup_table_shown(ctx)) {
		page_size = faft_context_cand_page_size(ctx);

		faft_context_cand_enumerate(ctx);

		for(i=0; i < page_size; i++) {
			if (faft_context_cand_has_next(ctx)) {
				candidate = faft_context_cand_string(ctx);
				printf("%d: %s, ", i+1, candidate);

				if (candidate)
					free(candidate);
			} else
				break;
		}

	} else if (faft_context_is_quick_input_updated(ctx)) {

		page_size = faft_context_quick_input_size(ctx);
		faft_context_quick_input_enumerate(ctx);

		for(i=0; i < page_size; i++) {
			if (faft_context_quick_input_has_next(ctx)) {
				candidate = faft_context_get_quick_input_string(ctx);
				printf("%d: %s, ", i+1, candidate);

				if (candidate)
					free(candidate);
			} else
				break;
		}

	}

	printf("\n");

	printf("Page size: %d\n", page_size);

	faft_context_unset_lookup_table_updated(ctx);
	faft_context_unset_quick_input_updated(ctx);
}
void ibus_faft_update_preedit_string(DummyEngine *faft)
{
	gchar *buff_str = faft_context_get_buffer_string(faft->context);
	gchar *kc_str = NULL;

	gint cursor = faft_context_get_buffer_cursor(faft->context);
	printf("Cursor: %d\n", cursor);

	gsize kc_len = 0;
	gchar *preeditBuf = NULL;

	/* Check if there is KeyCodes, if not, just use the original string */
	if (faft_context_is_keycode_entering(faft->context)) {
		
		/* This part should be moved into faft library */
		FAFTKeyCode kc_buff[ARRAYCODE_MAX_LEN + 1];
		faft_acdata_get_unfinished(&(faft->context->data->acData), kc_buff);

		kc_str = keystostr(kc_buff);
		kc_len = strlen(kc_str);

		gsize len = kc_len + strlen(buff_str);

		preeditBuf = g_new(gchar, len+1);
		preeditBuf[0]='\0';

		if (cursor > 0){
			g_utf8_strncpy(preeditBuf, buff_str, cursor);
		}

		gsize kc_start = strlen(preeditBuf);

		g_strlcat(preeditBuf, kc_str, len+1);
		g_strlcat(preeditBuf, buff_str+kc_start, len+1);

		g_free((gpointer) buff_str);
		g_free((gpointer) kc_str);
	} else
		preeditBuf = buff_str;


	printf("PreEdit: %s\n", preeditBuf);
	g_free(preeditBuf);
	/* Color the KeyCodes */
	gint cursorRight;
	if (kc_len>0)
		cursorRight = kc_len+1;
	else
		cursorRight = 1;
	printf("KeyCodes from %d to %d\n", cursor, cursor+cursorRight);
	faft_context_unset_buffer_updated(faft->context);
}

int ibus_faft_handle_pagedown(DummyEngine *c)
{
}
int ibus_faft_handle_pageup(DummyEngine *c)
{
}
int ibus_faft_handle_left(DummyEngine *c)
{
}
int ibus_faft_handle_right(DummyEngine *faft)
{
	FAFTContext *context = faft->context;

	/* A new keyevent has happened */
	faft_context_new_key_event(context);

	faft_context_next_page(context);
	if (faft_context_is_key_event_ignored(context))
		faft_context_first_page(context);

	/* Not processed, means lookup table may not be present */
	if (!faft_context_is_key_event_processed(context))
		faft_context_move_cursor_forward(context);

	/* This is essential */
	faft_context_end_key_event(context);

	if (faft_context_is_key_event_accepted(context))
		ibus_faft_update(faft);

	/* Passed means there is nothing in buffer, and lookup table is not shown, and... */
	if (faft_context_is_key_event_passed(context))
		return FALSE;
	else
		return TRUE;
}
int ibus_faft_refresh_property(DummyEngine *c, const char *s)
{
}
gboolean get_input(guint *keysym, guint *modifiers)
{
	char input_c;
	int input_d;
again:
	printf("Shift or not? (y/n) quit? (q/...)\n");
	do {
		if (!scanf("%c", &input_c))
			return FALSE;
	} while (input_c == '\n');

	switch (input_c) {
		case 'y':
			*modifiers = IBUS_SHIFT_MASK;
			break;
		case 'n':
			*modifiers = 0;
			break;
		case 'q':
			return FALSE;
			break;
		default:
			goto again;
	}
	printf("Input what?"
			" 0:default"
			" 1:enter"
			" 2:esc"
			" 3:backspace"
			" 4:delete"
			" 5:pageup"
			" 6:pagedown"
			" 7:left"
			" 8:right"
			" 9:up"
			" 10:down"
			" 11:home"
			" 12:end"
			" 13:tab"
			" 14:ShiftL"
			"\n");
	if (!scanf("%d", &input_d))
		return FALSE;
	switch (input_d) {
		case 0:
			printf("Input default: ");
			do {
				if (!scanf("%c", &input_c))
					return FALSE;
			} while (input_c == '\n');
			*keysym = input_c;
			break;
		case 1:
			*keysym = IBUS_Return;
			break;
		case 2:
			*keysym = IBUS_Escape;
			break;
		case 3:
			*keysym = IBUS_BackSpace;
			break;
		case 4:
			*keysym = IBUS_Delete;
			break;
		case 5:
			*keysym = IBUS_Page_Up;
			break;
		case 6:
			*keysym = IBUS_Page_Down;
			break;
		case 7:
			*keysym = IBUS_Left;
			break;
		case 8:
			*keysym = IBUS_Right;
			break;
		case 9:
			*keysym = IBUS_Up;
			break;
		case 10:
			*keysym = IBUS_Down;
			break;
		case 11:
			*keysym = IBUS_Home;
			break;
		case 12:
			*keysym = IBUS_End;
			break;
		case 13:
			*keysym = IBUS_Tab;
			break;
		case 14:
			*keysym = IBUS_Shift_L;
			break;
		default:
			*keysym = 0;
	}
	return TRUE;
}
