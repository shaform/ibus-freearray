/** 
 * @file main.c
 * @brief FreeArray for ibus
 */
/**
 * ibus-freearray - FreeArray for Test for The Input Bus
 *
 * Modified from ibus-array project.
 * Copyright (c) 2009  Yu-Chun Wang <mainlander1122@gmail.com>
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
#include <stdlib.h>
#include <ibus.h>
#include "ibus-freearray-engine.h"

static IBusBus *bus = NULL;
static IBusFactory *factory = NULL;


static gboolean ibus = FALSE;
static gboolean verbose = FALSE;

static const GOptionEntry entries[] =
{
    { "ibus", 'i', 0, G_OPTION_ARG_NONE, &ibus, "component is executed by ibus", NULL },
    { "verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose, "verbose", NULL },
    { NULL },
};

static void ibus_disconnected_cb (IBusBus  *bus, gpointer  user_data)
{
	ibus_quit ();
}


static void init (void)
{

	ibus_init ();

	bus = ibus_bus_new ();
	g_signal_connect (bus, "disconnected", G_CALLBACK (ibus_disconnected_cb), NULL);

	factory = ibus_factory_new (ibus_bus_get_connection (bus));

	ibus_factory_add_engine (factory, "freearray", IBUS_TYPE_FREEARRAY_ENGINE);

	if (ibus)
		ibus_bus_request_name (bus, "org.freedesktop.IBus.FreeArray", 0);
	else {
		IBusComponent *component;

		component = ibus_component_new ("org.freedesktop.IBus.FreeArray",
				"FreeArray for Test Component",
				"0.0.0",
				"GPLv3+",
				"Yong-Siang Shih (Shaform) <shaform at gmail.com>",
				"http://code.google.com/p/freearray/",
				"",
				"ibus-freearray");
		ibus_component_add_engine (component,
				ibus_engine_desc_new ("freearray",
					"FreeArray",
					"FrreArray Input Method",
					"zh_TW",
					"GPLv3+",
					"Yong-Siang Shih (Shaform) <shaform at gmail.com>",
					PKGDATADIR"/icons/ibus-freearray.svg",
					"us"));
		ibus_bus_register_component (bus, component);

		g_object_unref (component);
	}
}

int main(gint argc, gchar **argv)
{
	GError *error = NULL;
	GOptionContext *context;

	context = g_option_context_new ("- ibus freearray engine component");

	g_option_context_add_main_entries (context, entries, "ibus-freearray");

	if (!g_option_context_parse (context, &argc, &argv, &error)) {
		g_print ("Option parsing failed: %s\n", error->message);
		exit (-1);
	}

	g_option_context_free (context);

	init();
	ibus_main();

	return 0;
}
