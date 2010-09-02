/**
 * ibus-faft - FreeArray for Test for The Input Bus
 *
 * Copyright (c) 2010
 * 	Yong-Siang Shih (Shaform) <shaform@gmail.com>
 *
 * Some code is borrowed from ibus-chewing project.
 * Copyright (c) 2009  Red Hat, Inc. All rights reserved.
 * Copyright (c) 2009  Ding-Yi Chen <dchen at redhat.com>
 *
 * Some code is borrowed from ibus-array project.
 * Copyright (c) 2009  Yu-Chun Wang <mainlander1122@gmail.com>
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
#include <libintl.h>
#include <ibus.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include <glib.h>
#include <glib-object.h>

#include "faft/faft.h"
#include "ibus-faft-engine.h"
#include "properties.h"

/* The use of these status should be considered more clearly */
#define ENGINE_STATUS_INITIALIZED	0x1
#define ENGINE_STATUS_ENABLED		0x2
#define ENGINE_STATUS_FOCUS_IN		0x4
#define	ENGINE_STATUS_SHOW_CANDIDATE    0x8
#define ENGINE_STATUS_NEED_COMMIT	0x10

/**
 * Type checking and casting macros
 */
#define IBUS_FAFT_ENGINE(obj)	\
	G_TYPE_CHECK_INSTANCE_CAST((obj), ibus_faft_engine_get_type(), IBusFAFTEngine)

#define IBUS_FAFT_ENGINE_CONST(obj)	\
	G_TYPE_CHECK_INSTANCE_CAST((obj), ibus_faft_engine_get_type(), IBusFAFTEngine const)

#define IBUS_FAFT_ENGINE_CLASS(klass)	\
	G_TYPE_CHECK_CLASS_CAST((klass), ibus_faft_engine_get_type(), IBusFAFTEngineClass)

#define IBUS_IS_FAFT_ENGINE(obj)	\
	G_TYPE_CHECK_INSTANCE_TYPE((obj), ibus_faft_engine_get_type ())

#define IBUS_FAFT_ENGINE_GET_CLASS(obj)	\
	G_TYPE_INSTANCE_GET_CLASS((obj), ibus_faft_engine_get_type(), IBusFAFTEngineClass)
/*
 * Main object structure
 */


typedef struct _IBusFAFTEngine IBusFAFTEngine;
struct _IBusFAFTEngine {
	IBusEngine __parent__;

	/* faft main data */
	FAFTContext * context;


	/* used for property buttons */
	IBusProperty * variety_prop;
	IBusProperty * shape_prop;
	IBusProperty * settings_prop;
	IBusPropList * prop_list;


	IBusConfig * config;

	IBusLookupTable * table;

	/* the use of this should be considered more clearly */
	int status;
};

/*
 * Class definition
 */
typedef struct _IBusFAFTEngineClass IBusFAFTEngineClass;
struct _IBusFAFTEngineClass {
	IBusEngineClass __parent__;

	IBusText * variety_prop_label_chi;
	IBusText * variety_prop_label_eng;
	IBusText * shape_prop_label_full;
	IBusText * shape_prop_label_half;
	IBusText * settings_prop_label;
	IBusText * emptyText;
};



static IBusEngineClass *parent_class = NULL;


/**
 * Functions prototype
 */

/**
 * IBus-FAFT engine class methods
 */
/* Initiate the ibus-faft */ 
static void ibus_faft_engine_class_init(IBusFAFTEngineClass *klass);
/* done */
static void ibus_faft_engine_init(IBusFAFTEngine *engine);
/* done */
static void ibus_faft_engine_destroy(IBusFAFTEngine *engine);
/* done */
static gboolean ibus_faft_engine_process_key_event(IBusEngine *engine, guint keysym, guint keycode, guint modifiers);
/* done */
static void ibus_faft_engine_focus_in(IBusEngine *engine);
/* done */
static void ibus_faft_engine_focus_out(IBusEngine *engine);
/* done */
static void ibus_faft_engine_reset(IBusEngine *engine);
/* done */
static void ibus_faft_engine_enable(IBusEngine *engine);
/* done */
static void ibus_faft_engine_disable(IBusEngine *engine);
/* done */
static void ibus_faft_engine_page_up(IBusEngine *engine);
/* done */
static void ibus_faft_engine_page_down(IBusEngine *engine);
/* done */
static void ibus_faft_engine_cursor_up(IBusEngine *engine);
/* done */
static void ibus_faft_engine_cursor_down(IBusEngine *engine);
/* done */
static void ibus_faft_engine_property_activate(IBusEngine *engine, const gchar *prop_name, guint prop_state);
/* done */
static void ibus_faft_engine_property_show(IBusEngine *engine, const gchar *prop_name);
/* done */
static void ibus_faft_engine_property_hide (IBusEngine *engine, const gchar *prop_name);



/* IBus-FAFT Engine input helper utils */

static gboolean ibus_faft_handle_left(IBusFAFTEngine *faft);
static gboolean ibus_faft_handle_right(IBusFAFTEngine *faft);
static gboolean ibus_faft_handle_pagedown(IBusFAFTEngine *faft);
static gboolean ibus_faft_handle_pageup(IBusFAFTEngine *faft);
static void ibus_faft_force_commit(IBusFAFTEngine *faft);
static void ibus_faft_commit(IBusFAFTEngine *faft);

/* IBus-FAFT Engine background utils */
static void ibus_faft_engine_show_lookup_table(IBusFAFTEngine * faft, gboolean isShow);
static void ibus_faft_update (IBusFAFTEngine *faft);
static void ibus_faft_update_lookup_table(IBusFAFTEngine *faft);
static void ibus_faft_load_setting(IBusFAFTEngine *faft);

static void ibus_faft_hide_property_list(IBusFAFTEngine *faft);

static void ibus_faft_refresh_property(IBusFAFTEngine *faft, const gchar *prop_name);

static void ibus_faft_refresh_property_list(IBusFAFTEngine *faft);

static IBusProperty *ibus_faft_get_iBusProperty(IBusFAFTEngine *faft, const gchar *prop_name);
static IBusText *ibus_faft_make_preedit_string(IBusFAFTEngine *faft, gint *cursor);
/* done */
static void ibus_config_value_changed (IBusConfig *config, const gchar *section, const gchar *name, GValue *value, gpointer user_data);




/**
 * Initialization
 * Basic Definition
 *
 */
GType ibus_faft_engine_get_type(void)
{
	static GType type = 0;

	if (type == 0) {
		static const GTypeInfo type_info = {
			sizeof (IBusFAFTEngineClass),
			(GBaseInitFunc) NULL,
			(GBaseFinalizeFunc) NULL,
			(GClassInitFunc) ibus_faft_engine_class_init,
			(GClassFinalizeFunc) NULL,
			NULL /* class_data */,
			sizeof (IBusFAFTEngine),
			0 /* n_preallocs */,
			(GInstanceInitFunc) ibus_faft_engine_init,
			NULL
		};

		type = g_type_register_static (IBUS_TYPE_ENGINE, "IBusFAFTEngine", &type_info, (GTypeFlags)0);
	}

	return type;
}

static void ibus_faft_engine_class_init(IBusFAFTEngineClass *klass)
{
	IBusObjectClass *ibus_object_class = IBUS_OBJECT_CLASS (klass);
	IBusEngineClass *engine_class = IBUS_ENGINE_CLASS (klass);

	/* Init property labels */
	klass->variety_prop_label_chi = ibus_text_new_from_static_string("Chi");
	klass->variety_prop_label_eng = ibus_text_new_from_static_string("Eng");
	klass->shape_prop_label_full = ibus_text_new_from_static_string("Full");
	klass->shape_prop_label_half = ibus_text_new_from_static_string("Half");
	klass->settings_prop_label = ibus_text_new_from_static_string("Settings");
	klass->emptyText = ibus_text_new_from_static_string("");

	/* Init class methods */
	parent_class = g_type_class_ref (IBUS_TYPE_ENGINE);

	ibus_object_class->destroy = (IBusObjectDestroyFunc) ibus_faft_engine_destroy;

	engine_class->process_key_event = ibus_faft_engine_process_key_event;

	engine_class->reset = ibus_faft_engine_reset;
	engine_class->focus_in = ibus_faft_engine_focus_in;
	engine_class->focus_out = ibus_faft_engine_focus_out;
	engine_class->enable = ibus_faft_engine_enable;
	engine_class->disable = ibus_faft_engine_disable;

	engine_class->page_up = ibus_faft_engine_page_up;
	engine_class->page_down = ibus_faft_engine_page_down;
	engine_class->cursor_up = ibus_faft_engine_cursor_up;
	engine_class->cursor_down = ibus_faft_engine_cursor_down;

	engine_class->property_activate = ibus_faft_engine_property_activate;
	engine_class->property_show = ibus_faft_engine_property_show;
	engine_class->property_hide = ibus_faft_engine_property_hide;
}


static void ibus_faft_engine_init(IBusFAFTEngine *faft)
{
	faft->context = NULL;
	faft->table = ibus_lookup_table_new(10,0,FALSE,TRUE);
	faft->config = NULL;


	/* Set up property buttons */
	faft->variety_prop = 
		ibus_property_new("faft_variety_prop", PROP_TYPE_NORMAL,
				IBUS_FAFT_ENGINE_GET_CLASS(faft)->variety_prop_label_chi,
				NULL, NULL, TRUE, TRUE,
				PROP_STATE_UNCHECKED, NULL);
	faft->shape_prop = 
		ibus_property_new("faft_shape_prop", PROP_TYPE_NORMAL,
				IBUS_FAFT_ENGINE_GET_CLASS(faft)->shape_prop_label_half,
				NULL, NULL, TRUE, TRUE,
				PROP_STATE_UNCHECKED, NULL);

	IBusText *settings_tooltip = ibus_text_new_from_string("Configure FAFT engine");

	faft->settings_prop = 
		ibus_property_new("faft_settings_prop", PROP_TYPE_NORMAL,
				IBUS_FAFT_ENGINE_GET_CLASS(faft)->settings_prop_label,
				"gtk-preferences", NULL, TRUE, TRUE,
				PROP_STATE_UNCHECKED, NULL);

	g_object_unref((gpointer) settings_tooltip);

	faft->prop_list = ibus_prop_list_new();

	/* initialize the object here */
	if (!(faft->status & ENGINE_STATUS_INITIALIZED)) {
		faft_init(PKGDATADIR "/data");
		faft->context = faft_context_new();
	}

	/* Put property buttons into the prop_list */
	ibus_prop_list_append(faft->prop_list, faft->variety_prop);
	ibus_prop_list_append(faft->prop_list, faft->shape_prop);
	ibus_prop_list_append(faft->prop_list, faft->settings_prop);


	/* Engine Status used by ibus-faft only */
	faft->status = ENGINE_STATUS_INITIALIZED;
}

/**
 * Properties
 *
 *
 *
 */


/* Display a property*/
static void ibus_faft_engine_property_show(IBusEngine *engine, const gchar *prop_name)
{
	IBusFAFTEngine *faft = IBUS_FAFT_ENGINE(engine);
	IBusProperty *prop = ibus_faft_get_iBusProperty(faft, prop_name);
	ibus_property_set_visible(prop, TRUE);
	ibus_engine_update_property(engine, prop);
}
/* Hide a property*/
static void ibus_faft_engine_property_hide(IBusEngine *engine, const gchar *prop_name)
{
	IBusFAFTEngine *faft = IBUS_FAFT_ENGINE(engine);
	IBusProperty *prop = ibus_faft_get_iBusProperty(faft, prop_name);
	ibus_property_set_visible(prop, FALSE);
	ibus_engine_update_property(engine, prop);
}
/* Toggle a property*/
static void ibus_faft_engine_property_activate(IBusEngine *engine, const gchar *prop_name, guint prop_state)
{
	IBusFAFTEngine *faft = IBUS_FAFT_ENGINE(engine);

	gboolean changed = TRUE;
	if (g_strcmp0(prop_name, "faft_variety_prop") == 0)
		faft_context_toggle_variety(faft->context);

	else if (g_strcmp0(prop_name, "faft_shape_prop") == 0)
		faft_context_toggle_shape(faft->context);

	else if (g_strcmp0(prop_name, "faft_settings_prop") == 0) {
		GError *error = NULL;
		gchar *argv[2] = { NULL, };
		gchar *path;
		const gchar* libexecdir;

		libexecdir = g_getenv("LIBEXECDIR");
		if (libexecdir == NULL)
			libexecdir = LIBEXECDIR;

		path = g_build_filename(libexecdir, "ibus-setup-faft", NULL);
		argv[0] = path;
		argv[1] = NULL;
		g_spawn_async (NULL, argv, NULL, 0, NULL, NULL, NULL, &error);

		g_free(path);

		changed = FALSE;
	} else
		changed = FALSE;

	if (changed)
		ibus_faft_refresh_property(faft, prop_name);
}



/**
 * Movements
 *
 */
static void ibus_faft_engine_page_up(IBusEngine * engine)
{
	IBusFAFTEngine *faft = IBUS_FAFT_ENGINE(engine);
	ibus_faft_handle_pageup(faft);
}
static void ibus_faft_engine_page_down(IBusEngine * engine)
{
	IBusFAFTEngine *faft = IBUS_FAFT_ENGINE(engine);
	ibus_faft_handle_pagedown(faft);
}
static void ibus_faft_engine_cursor_up(IBusEngine * engine)
{
	IBusFAFTEngine *faft = IBUS_FAFT_ENGINE(engine);
	ibus_faft_handle_left(faft);
}
static void ibus_faft_engine_cursor_down(IBusEngine * engine)
{
	IBusFAFTEngine *faft = IBUS_FAFT_ENGINE(engine);
	ibus_faft_handle_right(faft);
}



static void ibus_faft_engine_enable(IBusEngine * engine)
{
	IBusFAFTEngine *faft = IBUS_FAFT_ENGINE(engine);

	if (!faft->config){
		/* connections_list is not avail in init, so we put it here */
		GList *connections_list = ibus_service_get_connections(IBUS_SERVICE(engine));
		g_assert(connections_list);
		g_assert(connections_list->data);
		IBusConnection *iConnection = (IBusConnection *) connections_list->data;
		faft->config = ibus_config_new(iConnection);
		ibus_faft_load_setting(faft);
		g_signal_connect (faft->config, "value-changed",
				G_CALLBACK(ibus_config_value_changed), faft);
	}
	ibus_faft_refresh_property_list(faft);

	faft->context->state->status = FAFT_STATUS_IDLE;
	faft->status |= ENGINE_STATUS_ENABLED;
}
static void ibus_faft_engine_disable (IBusEngine * engine)
{

	IBusFAFTEngine *faft = IBUS_FAFT_ENGINE(engine);
	ibus_faft_force_commit(faft);
	ibus_faft_hide_property_list(faft);

	faft->context->state->status = FAFT_STATUS_BYPASS;
	faft->status &= (~ENGINE_STATUS_ENABLED);
}

static void ibus_faft_engine_focus_out (IBusEngine *engine)
{
	IBusFAFTEngine *faft = IBUS_FAFT_ENGINE(engine);

	ibus_faft_force_commit(faft);
	faft->status &= (~ENGINE_STATUS_FOCUS_IN);
}

static void ibus_faft_engine_focus_in(IBusEngine *engine)
{
	IBusFAFTEngine *faft = IBUS_FAFT_ENGINE(engine);


	ibus_faft_refresh_property_list(faft);
	ibus_faft_update(faft);

	faft->status |= ENGINE_STATUS_FOCUS_IN;
}



static void ibus_faft_engine_destroy(IBusFAFTEngine *faft)
{

	if(faft->context) faft_context_delete (faft->context);

	if(faft->table) g_object_unref ((gpointer) faft->table);

	if(faft->variety_prop) g_object_unref ((gpointer) faft->variety_prop);

	if(faft->shape_prop) g_object_unref ((gpointer) faft->shape_prop);

	if(faft->settings_prop) g_object_unref ((gpointer) faft->settings_prop);

	if(faft->prop_list) g_object_unref ((gpointer) faft->prop_list);


	faft->context = NULL;
	faft->table = NULL;
	faft->variety_prop = NULL;
	faft->shape_prop = NULL;
	faft->settings_prop = NULL;
	faft->prop_list = NULL;

	faft->status &= ~ENGINE_STATUS_INITIALIZED;
	faft_exit();

	IBUS_OBJECT_CLASS (parent_class)->destroy ((IBusObject *) faft);
}




static void ibus_faft_engine_reset(IBusEngine * engine)
{
	IBusFAFTEngine *faft = (IBusFAFTEngine*) engine;
	if (faft->context)
		faft_context_reset(faft->context);


	ibus_lookup_table_clear(faft->table);
	ibus_engine_hide_auxiliary_text(IBUS_ENGINE(engine));
	ibus_engine_hide_lookup_table(IBUS_ENGINE(faft));

	ibus_faft_refresh_property_list(faft);

	parent_class->reset(engine);
}


#define is_alpha(c) (((c) >= IBUS_a && (c) <= IBUS_z) || ((c) >= IBUS_A && (c) <= IBUS_Z))


/**
 * The most important part of FAFT.
 * Process all key input and controls the behaviour of FAFT.
 */
gboolean ibus_faft_engine_process_key_event(IBusEngine *engine, guint keysym, guint keycode, guint modifiers)
{

	/* Data */
	IBusFAFTEngine *faft = IBUS_FAFT_ENGINE(engine);
	FAFTContext *context = faft->context;

	static gboolean SHIFTER = FALSE;

	/* Ignore all release events except for Shift-R*/
	if (modifiers & IBUS_RELEASE_MASK) {
		if (SHIFTER && keysym == IBUS_Shift_R) {
			/* Toggle eng/chi */
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

				faft_context_select_cand(context, 0);

				if (!faft_context_is_key_event_processed(context))
					faft_context_input_char(context);

				/**
				 * This may activate space_to_select
				 * FIXME:
				 * Whether space_to_select should be activated by this implicitly
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
			case IBUS_Shift_R:
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
				/* Toggle half/Full */
				faft_context_toggle_shape(context);

				faft_context_end_key_event(context);
				ibus_faft_refresh_property(faft, "faft_shape_prop");
				return TRUE;
				break;
			case IBUS_Return:
				/* Prevent pressing Enter accidentally when using Array Symbol Input */
				if (faft_context_is_entering(context))
					faft_context_ignore_key_event(context);
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






/* ibus-faft util */
static gboolean ibus_faft_handle_left(IBusFAFTEngine *faft)
{
	FAFTContext *context = faft->context;

	/* A new keyevent has happened */
	faft_context_new_key_event(context);

	faft_context_prev_page(context);
	if (faft_context_is_key_event_ignored(context))
		faft_context_last_page(context);

	/* Not processed, means lookup table may not be present */
	if (!faft_context_is_key_event_processed(context))
		faft_context_move_cursor_backward(context);

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

static gboolean ibus_faft_handle_right(IBusFAFTEngine *faft)
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
static gboolean ibus_faft_handle_pagedown(IBusFAFTEngine *faft)
{
	FAFTContext *context = faft->context;

	/* A new keyevent has happened */
	faft_context_new_key_event(context);

	/**
	 *  Go to next page and if already on the first page, go next avalible
	 */
	faft_context_next_page(context);
	if (faft_context_is_key_event_ignored(context))
		faft_context_next_avail(context);

	if (faft_context_is_key_event_ignored(context))
		faft_context_first_avail(context);

	if (faft_context_is_key_event_ignored(context))
		faft_context_first_page(context);

	if (!faft_context_is_key_event_processed(context))
		if (faft_context_is_keycode_entering(context) || faft_context_is_buffer_entering(context))
			faft_context_ignore_key_event(context);

	faft_context_end_key_event(context);

	if (faft_context_is_key_event_accepted(context))
		ibus_faft_update(faft);

	if (faft_context_is_key_event_passed(context))
		return FALSE;
	else
		return TRUE;
}
static gboolean ibus_faft_handle_pageup(IBusFAFTEngine *faft)
{
	FAFTContext *context = faft->context;

	/* A new keyevent has happened */
	faft_context_new_key_event(context);

	/**
	 *  Go to previous page and if already on the first page, go previous avalible
	 */
	faft_context_prev_page(context);

	if (faft_context_is_key_event_ignored(context))
		faft_context_prev_avail(context);

	if (faft_context_is_key_event_ignored(context))
		faft_context_last_avail(context);

	if (faft_context_is_key_event_ignored(context))
		faft_context_last_page(context);

	if (!faft_context_is_key_event_processed(context))
		if (faft_context_is_keycode_entering(context) || faft_context_is_buffer_entering(context))
			faft_context_ignore_key_event(context);

	faft_context_end_key_event(context);


	if (faft_context_is_key_event_accepted(context))
		ibus_faft_update(faft);

	if (faft_context_is_key_event_passed(context))
		return FALSE;
	else
		return TRUE;
}

static void ibus_faft_force_commit(IBusFAFTEngine *faft)
{

	FAFTContext *context = faft->context;

	faft_context_new_key_event(context);

	faft_context_end_lookup_table(context);
	faft_context_clear_keycode(context);
	faft_context_commit(context);

	faft_context_end_key_event(context);

	ibus_faft_update(faft);

}

/* ibus-faft background util */


/*
 *
 * Update the state of ibus_faft
 */
static void ibus_faft_update(IBusFAFTEngine *faft)
{
	FAFTContext *ctx = faft->context;

	/* Update Lookup Table / Quick Input */
	if (faft_context_is_quick_input_updated(ctx) || faft_context_is_lookup_table_updated(ctx))
		ibus_faft_update_lookup_table(faft);

	/* See if committed */
	ibus_faft_commit(faft);

	/* Update Pre-edit String */
	gint cursor;
	IBusText *iText = NULL;
	if (faft_context_is_buffer_entering(ctx) || faft_context_is_keycode_entering(ctx)) {
		iText = ibus_faft_make_preedit_string(faft, &cursor);
		ibus_engine_update_preedit_text(IBUS_ENGINE(faft), iText, cursor, TRUE);
	} else
		ibus_engine_update_preedit_text(IBUS_ENGINE(faft), IBUS_FAFT_ENGINE_GET_CLASS(faft)->emptyText, 0, FALSE);
	
	if (iText)
		g_object_unref((gpointer)iText);


}
static void ibus_faft_update_lookup_table(IBusFAFTEngine *faft)
{
	FAFTContext *ctx = faft->context;

	ibus_lookup_table_clear(faft->table);

	int i=0;
	gchar *candidate = NULL;
	IBusText *iText = NULL;

	int page_size = 0;
	if (faft_context_is_lookup_table_shown(ctx)) {
		page_size = faft_context_cand_page_size(ctx);
		faft_context_cand_enumerate(ctx);

		for(i=0; i < page_size; i++) {
			if (faft_context_cand_has_next(ctx)) {
				candidate = faft_context_cand_string(ctx);
				iText = ibus_text_new_from_string((gpointer) candidate);
				ibus_lookup_table_append_candidate(faft->table, (gpointer) iText);

				g_free((gpointer)candidate);
				g_object_unref((gpointer)iText);
			} else
				break;
		}

	} else if (faft_context_is_quick_input_updated(ctx)) {
		page_size = faft_context_quick_input_size(ctx);

		faft_context_quick_input_enumerate(ctx);
		for(i=0; i < page_size; i++) {
			if (faft_context_quick_input_has_next(ctx)) {
				candidate = faft_context_get_quick_input_string(ctx);
				iText = ibus_text_new_from_string((gpointer) candidate);
				ibus_lookup_table_append_candidate(faft->table, (gpointer) iText);
				g_free((gpointer)candidate);
				g_object_unref((gpointer)iText);
			} else
				break;
		}

	}
	if (page_size)
		ibus_faft_engine_show_lookup_table(faft, TRUE);
	else
		ibus_faft_engine_show_lookup_table(faft, FALSE);

	faft_context_unset_lookup_table_updated(ctx);
	faft_context_unset_quick_input_updated(ctx);
}
static IBusText *ibus_faft_make_preedit_string(IBusFAFTEngine *faft, gint *cursor)
{

	gchar *buff_str = faft_context_get_buffer_string(faft->context);
	gchar *kc_str = NULL;

	*cursor = faft_context_get_buffer_cursor(faft->context);

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

		if (*cursor > 0){
			g_utf8_strncpy(preeditBuf, buff_str, *cursor);
		}

		gsize kc_start = strlen(preeditBuf);

		g_strlcat(preeditBuf, kc_str, len+1);
		g_strlcat(preeditBuf, buff_str+kc_start, len+1);

		g_free((gpointer) buff_str);
		g_free((gpointer) kc_str);
	} else
		preeditBuf = buff_str;

	int testlen = strlen(preeditBuf);
	IBusText *iText = ibus_text_new_from_string(preeditBuf);
	g_free((gpointer) preeditBuf);

	if (iText == NULL) return NULL;

	/* Color the KeyCodes */
	gint cursorRight = *cursor;
	if (kc_len>0)
		cursorRight += kc_len;
	else
		cursorRight += 1;

	/* Temp solution */
	if (! (*cursor == faft->context->data->chiSymbolBufLen && kc_len == 0)) {
		ibus_text_append_attribute (iText, IBUS_ATTR_TYPE_FOREGROUND, 0x00ffffff,
				*cursor, cursorRight);
		ibus_text_append_attribute (iText, IBUS_ATTR_TYPE_BACKGROUND, 0x00000000,
				*cursor, cursorRight);
	}

/* This part is a terrible bug!! */
	/*
	IntervalType it;
	while(faft_context_interval_has_next(faft->context)) {
		faft_context_interval_get(faft->context, &it);
		IntervalType it_f, it_b;
		it_f.from = it.from;
		it_f.to = it.to < *cursor ? it.to : *cursor;

		it_b.from = it.from < *cursor ? *cursor + cursorRight : it.from + cursorRight;
		it_b.to = it.to + cursorRight;

		if (it_f.from < it_f.to)
			ibus_text_append_attribute (iText, IBUS_ATTR_TYPE_UNDERLINE, IBUS_ATTR_UNDERLINE_DOUBLE,
					it_f.from, it_f.to);
		if (it_b.from < it_b.to)
			ibus_text_append_attribute (iText, IBUS_ATTR_TYPE_UNDERLINE, IBUS_ATTR_UNDERLINE_DOUBLE,
					it_b.from, it_b.to);
	}
	*/
	ibus_text_append_attribute (iText, IBUS_ATTR_TYPE_UNDERLINE, IBUS_ATTR_UNDERLINE_SINGLE, 0, -1);

	return iText;
}
void ibus_faft_set_lookup_table_label(IBusFAFTEngine *faft, const gchar * labels)
{

	int i,len=strlen(labels);
	g_array_set_size(faft->table->labels,0);
	for(i=0;i<len;i++){
		IBusText *text = ibus_text_new_from_unichar((gunichar) labels[i]);
		ibus_lookup_table_append_label (faft->table,text);
	}
}
static void ibus_faft_engine_show_lookup_table(IBusFAFTEngine * faft, gboolean isShow)
{

	ibus_engine_update_lookup_table(IBUS_ENGINE(faft), faft->table, isShow);
	if (isShow)
		ibus_engine_show_lookup_table(IBUS_ENGINE(faft));
	else
		ibus_engine_hide_lookup_table(IBUS_ENGINE(faft));
}

static void ibus_faft_commit(IBusFAFTEngine *faft)
{
	FAFTContext *ctx = faft->context;

	if (faft_context_is_commit_string_updated(ctx)) {
		IBusText *iText = NULL;

		gchar *commit_string = faft_context_get_commit_string(ctx);
		if (commit_string && commit_string[0]) {
			iText = ibus_text_new_from_string(commit_string);
			ibus_engine_commit_text(IBUS_ENGINE(faft), iText);
			g_free((gpointer) commit_string);

			if (iText)
				g_object_unref((gpointer) iText);
		}

		faft_context_unset_commit_string_updated(ctx);
	}
}
static IBusProperty *ibus_faft_get_iBusProperty(IBusFAFTEngine *faft, const gchar *prop_name)
{
	if (g_strcmp0(prop_name, "faft_variety_prop") == 0)
		return faft->variety_prop;

	else if (g_strcmp0(prop_name,"faft_shape_prop") == 0)
		return faft->shape_prop;

	else if (g_strcmp0(prop_name,"faft_settings_prop") == 0)
		return faft->settings_prop;

	else
		return NULL;
}

static void ibus_faft_refresh_property_list(IBusFAFTEngine *faft)
{
	ibus_faft_refresh_property(faft,"faft_variety_prop");
	ibus_faft_refresh_property(faft,"faft_shape_prop");
	ibus_faft_refresh_property(faft,"faft_settings_prop");

	if (faft->status & (ENGINE_STATUS_ENABLED | ENGINE_STATUS_FOCUS_IN)) {
		ibus_engine_register_properties(IBUS_ENGINE(faft), faft->prop_list);
		IBUS_ENGINE_GET_CLASS(faft)->property_show(IBUS_ENGINE(faft), "faft_variety_prop");
		IBUS_ENGINE_GET_CLASS(faft)->property_show(IBUS_ENGINE(faft), "faft_shape_prop");
		IBUS_ENGINE_GET_CLASS(faft)->property_show(IBUS_ENGINE(faft), "faft_settings_prop");
	}
}

static void ibus_faft_refresh_property(IBusFAFTEngine *faft, const gchar *prop_name)
{
	FAFTContext *ctx = faft->context;

	if (g_strcmp0(prop_name,"faft_variety_prop") == 0) {
		if (faft_context_get_variety(ctx) == FAFT_VARIETY_ENG)
			ibus_property_set_label(faft->variety_prop,
					IBUS_FAFT_ENGINE_GET_CLASS(faft)->variety_prop_label_eng);
		else
			ibus_property_set_label(faft->variety_prop,
					IBUS_FAFT_ENGINE_GET_CLASS(faft)->variety_prop_label_chi);

		ibus_engine_update_property(IBUS_ENGINE(faft), faft->variety_prop);

	} else if (g_strcmp0(prop_name, "faft_shape_prop") == 0) {
		if (faft_context_get_shape(ctx) == FAFT_SHAPE_FULL)
			ibus_property_set_label(faft->shape_prop,
					IBUS_FAFT_ENGINE_GET_CLASS(faft)->shape_prop_label_full);
		else
			ibus_property_set_label(faft->shape_prop,
					IBUS_FAFT_ENGINE_GET_CLASS(faft)->shape_prop_label_half);

		ibus_engine_update_property(IBUS_ENGINE(faft), faft->shape_prop);

	}
}

static void ibus_faft_hide_property_list(IBusFAFTEngine *faft)
{

	IBUS_ENGINE_GET_CLASS(faft)->property_hide(IBUS_ENGINE(faft), "faft_variety_prop");
	IBUS_ENGINE_GET_CLASS(faft)->property_hide(IBUS_ENGINE(faft), "faft_shape_prop");
	IBUS_ENGINE_GET_CLASS(faft)->property_hide(IBUS_ENGINE(faft), "faft_settings_prop");
}

static void ibus_faft_load_setting(IBusFAFTEngine *faft)
{
	if (!(faft && faft->config)) return;
	int i;
	GValue val={0};

	for (i=0; ibus_faft_properties[i].key[0]; i++) {
		if (ibus_config_get_value(faft->config, "engine/FAFT", ibus_faft_properties[i].key, (gpointer) &val))

			ibus_config_value_changed(faft->config, "engine/FAFT", ibus_faft_properties[i].key, (gpointer) &val, faft);

		else {
			g_value_init((gpointer) &val, G_TYPE_INT);
			g_value_set_int((gpointer) &val, ibus_faft_properties[i].def);
			ibus_config_set_value(faft->config, "engine/FAFT", ibus_faft_properties[i].key, (gpointer) &val);
		}
		g_value_unset((gpointer) &val);
	}
}

static void ibus_config_value_changed(IBusConfig *config,
		const gchar *section,
		const gchar *name,
		GValue *value,
		gpointer user_data)
{
	IBusFAFTEngine *faft = IBUS_FAFT_ENGINE(user_data);
	FAFTContext *ctx;

	/* Check if context exists */
	if (faft && faft->context)
		ctx = faft->context;
	else
		return;

	if (g_strcmp0(section, "engine/FAFT") == 0) {
		gint val_d = g_value_get_int(value);

		if (g_strcmp0(name, "KbType") == 0)
			if (val_d == FREEARRAY_KBTYPE_DVORAK)
				faft_context_set_kbtype(ctx, FREEARRAY_KBTYPE_DVORAK);
			else
				faft_context_set_kbtype(ctx, FREEARRAY_KBTYPE_DEFAULT);

		else if (g_strcmp0(name, "MaxSize") == 0)
			faft_context_set_max_preedit_buffer_len(ctx, val_d);

		else if (g_strcmp0(name, "AutoInput") == 0)
			if (val_d)
				faft_context_set_auto_input_mode(ctx);
			else
				faft_context_unset_auto_input_mode(ctx);

		else if (g_strcmp0(name, "AutoClear") == 0)
			if (val_d)
				faft_context_set_auto_clear_mode(ctx);
			else
				faft_context_unset_auto_clear_mode(ctx);

		else if (g_strcmp0(name, "EscClear") == 0)
			if (val_d)
				faft_context_set_esc_clear_all_buff(ctx);
			else
				faft_context_unset_esc_clear_all_buff(ctx);

		else if (g_strcmp0(name, "SpaceSelect") == 0)
			if (val_d)
				faft_context_set_space_to_select(ctx);
			else
				faft_context_unset_space_to_select(ctx);

		else if (g_strcmp0(name, "AnyRowSelect") == 0)
			if (val_d)
				faft_context_set_any_row_key_select(ctx);
			else
				faft_context_unset_any_row_key_select(ctx);

		else if (g_strcmp0(name, "ChooseBackward") == 0)
			if (val_d)
				faft_context_set_choose_phrase_backward(ctx);
			else
				faft_context_unset_choose_phrase_backward(ctx);

		else if (g_strcmp0(name, "ShortCode") == 0)
			if (val_d)
				faft_context_set_short_code_input(ctx);
			else
				faft_context_unset_short_code_input(ctx);

		else if (g_strcmp0(name, "PreInput") == 0)
			if (val_d)
				faft_context_set_pre_input(ctx);
			else
				faft_context_unset_pre_input(ctx);

		else if (g_strcmp0(name, "PhraseInput") == 0)
			if (val_d)
				faft_context_set_phrase_input(ctx);
			else
				faft_context_unset_phrase_input(ctx);

		else if (g_strcmp0(name, "ArraySymbol") == 0)
			if (val_d)
				faft_context_set_array_symbol_input(ctx);
			else
				faft_context_unset_array_symbol_input(ctx);

		else if (g_strcmp0(name, "EasySymbol") == 0)
			if (val_d)
				faft_context_set_easy_symbol_input(ctx);
			else
				faft_context_unset_easy_symbol_input(ctx);
	}
}
