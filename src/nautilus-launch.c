/*  -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*  Please make sure that the TAB width in your editor is set to 4 spaces  */

/*\
|*|
|*|	nautilus-launch.c
|*|
|*|	Copyright (C) 2020 Stefano Gioffr&eacute; <madmurphy333@gmail.com>
|*|
|*|	Nautilus Launch is free software: you can redistribute it and/or modify it
|*|	under the terms of the GNU General Public License as published by the
|*|	Free Software Foundation, either version 3 of the License, or
|*|	(at your option) any later version.
|*|
|*|	Nautilus Launch is distributed in the hope that it will be useful, but
|*|	WITHOUT ANY WARRANTY; without even the implied warranty of
|*|	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
|*|	See the GNU General Public License for more details.
|*|
|*|	You should have received a copy of the GNU General Public License along
|*|	with this program. If not, see <http://www.gnu.org/licenses/>.
|*|
\*/



#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib.h>
#include <gio/gdesktopappinfo.h>
#include <nautilus-extension.h>

#ifdef ENABLE_NLS
#include <glib/gi18n-lib.h>
#else
#define _(STRING) STRING
#endif



/*

	GLOBAL TYPES AND VARIABLES

*/


typedef struct {
	GObject parent_slot;
} NautilusLaunch;

typedef struct {
	GObjectClass parent_slot;
} NautilusLaunchClass;

static GType provider_types[1];
static GType nautilus_launch_type;
static GObjectClass * parent_class;



/*

	FUNCTIONS

*/


static void nautilus_launch_clicked (
	NautilusMenuItem * const menu_item,
	gpointer user_data
) {

	gchar * argv[] = { NULL, NULL };
	gchar * wdir;
	GError * launcherr = NULL;
	GDesktopAppInfo * dfinfo;
	GList * const file_selection = g_object_get_data(
		(GObject *) menu_item,
		"nautilus_launch_files"
	);

	for (GList * iter = file_selection; iter; iter = iter->next) {

		argv[0] = g_file_get_path(
			nautilus_file_info_get_location(NAUTILUS_FILE_INFO(iter->data))
		);

		if (!argv[0]) {

			fprintf(stderr, "Nautilus Launch: %s\n", _("Error accessing file's path"));
			continue;

		}

		if (
			nautilus_file_info_is_mime_type(
				NAUTILUS_FILE_INFO(iter->data),
				"application/x-desktop"
			)
		) {

			/*  This is a .desktop launcher  */

			dfinfo = g_desktop_app_info_new_from_filename(argv[0]);

			if (!dfinfo) {

				fprintf(stderr, "Nautilus Launch: %s\n", _("Launcher has become invalid"));
				continue;

			}

			/*  Launcher is good, let's launch it  */

			if (
				!g_desktop_app_info_launch_uris_as_manager(
					dfinfo,
					NULL,
					NULL,
					G_SPAWN_DEFAULT,
					NULL,
					NULL,
					NULL,
					NULL,
					&launcherr
				)
			) {

				fprintf(stderr, "Nautilus Launch: %s\n", launcherr->message);
				g_error_free(launcherr);
				launcherr = NULL;

			}

			g_object_unref(dfinfo);

		} else {

			/*  This is a regular executable  */

			wdir = g_file_get_path(
				nautilus_file_info_get_parent_location(NAUTILUS_FILE_INFO(iter->data))
			);

			if (!wdir) {

				fprintf(stderr, "Nautilus Launch: %s\n", _("Error accessing current working directory"));

			}

			if (
				!g_spawn_async(
					wdir ? wdir : g_get_tmp_dir(),
					argv,
					NULL,
					G_SPAWN_DEFAULT,
					NULL,
					NULL,
					NULL,
					&launcherr
				)
			) {

				fprintf(stderr, "Nautilus Launch: %s\n", launcherr->message);
				g_error_free(launcherr);
				launcherr = NULL;

			}

			g_free(wdir);

		}

		g_free(argv[0]);

	}

}


GType nautilus_launch_get_type (void) {

	return nautilus_launch_type;

}


static void nautilus_launch_class_init (
	NautilusLaunchClass * const nautilus_launch_class,
	gpointer class_data
) {

	parent_class = g_type_class_peek_parent(nautilus_launch_class);

}


static GList * nautilus_launch_get_file_items (
	NautilusMenuProvider * const provider,
	GtkWidget * const window,
	GList * const file_selection
) {

	GFileInfo * finfo;
	GDesktopAppInfo * dfinfo;
	gsize listlen = 0;
	gchar * fpath;

	for (GList * iter = file_selection; iter; iter = iter->next, listlen++) {

		if (nautilus_file_info_is_directory(NAUTILUS_FILE_INFO(iter->data))) {

			/*  Directories are present in the selection  */

			return NULL;

		}

		if (
			nautilus_file_info_is_mime_type(
				NAUTILUS_FILE_INFO(iter->data),
				"application/x-desktop"
			)
		) {

			/*  This is a .desktop launcher  */

			fpath = g_file_get_path(
				nautilus_file_info_get_location(NAUTILUS_FILE_INFO(iter->data))
			);

			if (!fpath) {

				fprintf(stderr, "Nautilus Launch: %s\n", _("Error accessing launcher's path"));
				return NULL;

			}

			dfinfo = g_desktop_app_info_new_from_filename(fpath);

			g_free(fpath);

			if (dfinfo) {

				/*  Launcher is good  */

				g_object_unref(dfinfo);
				continue;

			}

			/*  Launcher is not good  */

			return NULL;

		}

		finfo = g_file_query_info(
			nautilus_file_info_get_location(NAUTILUS_FILE_INFO(iter->data)),
			G_FILE_ATTRIBUTE_ACCESS_CAN_EXECUTE,
			G_FILE_QUERY_INFO_NONE,
			NULL,
			NULL
		);

		if (!finfo) {

			/*  Cannot get file attributes  */

			return NULL;

		}

		if (!g_file_info_get_attribute_boolean(finfo, G_FILE_ATTRIBUTE_ACCESS_CAN_EXECUTE)) {

			/*  Non-executable files are present in the selection  */

			g_object_unref(finfo);
			return NULL;

		}

		g_object_unref(finfo);

	}

	NautilusMenuItem * const
		menu_item	=	listlen > 1 ?
							nautilus_menu_item_new(
								"NautilusLaunch::launch",
								_("Launch all"),
								_("Launch all the selected applications"),
								NULL /* icon name or `NULL` */
							)
						:
							nautilus_menu_item_new(
								"NautilusLaunch::launch",
								_("Launch"),
								_("Launch the selected application"),
								NULL /* icon name or `NULL` */
							);

	g_signal_connect(
		menu_item,
		"activate",
		G_CALLBACK(nautilus_launch_clicked),
		provider
	);

	g_object_set_data_full(
		(GObject *) menu_item, "nautilus_launch_files",
		nautilus_file_info_list_copy(file_selection),
		(GDestroyNotify) nautilus_file_info_list_free
	);

	return g_list_append(NULL, menu_item);

}


static void nautilus_launch_menu_provider_iface_init (
	NautilusMenuProviderIface * const iface,
	gpointer iface_data
) {

	iface->get_file_items = nautilus_launch_get_file_items;

}


static void nautilus_launch_register_type (GTypeModule * const module) {

	static const GTypeInfo info = {
		sizeof(NautilusLaunchClass),
		(GBaseInitFunc) NULL,
		(GBaseFinalizeFunc) NULL,
		(GClassInitFunc) nautilus_launch_class_init,
		(GClassFinalizeFunc) NULL,
		NULL,
		sizeof(NautilusLaunch),
		0,
		(GInstanceInitFunc) NULL,
	};

	static const GInterfaceInfo menu_provider_iface_info = {
		(GInterfaceInitFunc) nautilus_launch_menu_provider_iface_init,
		(GInterfaceFinalizeFunc) NULL,
		NULL
	};

	nautilus_launch_type = g_type_module_register_type(
		module,
		G_TYPE_OBJECT,
		"NautilusLaunch",
		&info,
		0
	);

	g_type_module_add_interface(
		module,
		nautilus_launch_type,
		NAUTILUS_TYPE_MENU_PROVIDER,
		&menu_provider_iface_info
	);

}


void nautilus_module_initialize (GTypeModule  * const module) {

	#ifdef ENABLE_NLS
	bindtextdomain(GETTEXT_PACKAGE, NAUTILUS_LAUNCH_LOCALEDIR);
	#endif

	nautilus_launch_register_type(module);
	*provider_types = nautilus_launch_get_type();

}


void nautilus_module_shutdown (void) {

	/*  Any module-specific shutdown  */

}


void nautilus_module_list_types (const GType ** types, int * num_types) {

	*types = provider_types;
	*num_types = G_N_ELEMENTS(provider_types);

}


/*  EOF  */

