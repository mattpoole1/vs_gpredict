
#ifdef HAVE_CONFIG_H
#include <build-config.h>
#endif
#include <glib/gi18n.h>
#include <glib/gstdio.h>
#include <gtk/gtk.h>

#include "compat.h"
#include "gpredict-utils.h"
#include "station-conf.h"
#include "sat-cfg.h"
#include "sat-log.h"
#include "sat-pref-stn.h"
#include "sat-pref-stn-data.h"
#include "sat-pref-stn-editor.h"


extern GtkWidget *window;       /* dialog window defined in sat-pref.c */
static GtkWidget *addbutton;
static GtkWidget *editbutton;
static GtkWidget *delbutton;
static GtkWidget *stnlist;


static void add_cb(GtkWidget * button, gpointer data)
{
    GtkTreeIter     item;       /* new item added to the list store */
    GtkListStore   *liststore;

    (void)button;
    (void)data;

    station_conf_t    conf = {
        .name = NULL,
        .host = NULL,
        .port = 4520,

        /* Toggle Utilities ON/OFF */
        .util1 = 0,
        .util1_name = NULL,
        .util2 = 0,
        .util2_name = NULL,
        .util3 = 0,
        .util3_name = NULL,
        .util4 = 0,
        .util4_name = NULL,
        .util5 = 0,
        .util5_name = NULL,
        .util6 = 0,
        .util6_name = NULL,

        /* Monitor 2 ADCs */
        .ana1 = 0,
        .ana1_name = NULL,
        .ana2 = 0,
        .ana2_name = NULL,
 
        /* Monitor 3 digital inputs */
        .dig1 = 0,
        .dig1_name = NULL,
        .dig2 = 0,
        .dig2_name = NULL,
        .dig3 = 0,
        .dig3_name = NULL,

        /* Use 4 relays for station config */
        .config1 = 0,
        .config1_name = NULL,
        .config1_on = NULL,
        .config1_off = NULL,
        .config2 = 0,
        .config2_name = NULL,
        .config2_on = NULL,
        .config2_off = NULL,
        .config3 = 0,
        .config3_name = NULL,
        .config3_on = NULL,
        .config3_off = NULL,
        .config4 = 0,
        .config4_name = NULL,
        .config4_on = NULL,
        .config4_off = NULL        
    };

    /* run stn conf editor */
    sat_pref_stn_editor_run(&conf);

    /* add new stn to the list */
    if (conf.name != NULL)
    {
        liststore =
            GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(stnlist)));
        gtk_list_store_append(liststore, &item);
        gtk_list_store_set(liststore, &item,
                           STN_LIST_COL_NAME, conf.name,
                           STN_LIST_COL_HOST, conf.host,
                           STN_LIST_COL_PORT, conf.port,
                           STN_LIST_COL_UTIL1, conf.util1,
                           STN_LIST_COL_UTIL1_NAME, conf.util1_name,
                           STN_LIST_COL_UTIL2, conf.util2,
                           STN_LIST_COL_UTIL2_NAME, conf.util2_name,
                           STN_LIST_COL_UTIL3, conf.util3,
                           STN_LIST_COL_UTIL3_NAME, conf.util3_name,
                           STN_LIST_COL_UTIL4, conf.util4,
                           STN_LIST_COL_UTIL4_NAME, conf.util4_name,
                           STN_LIST_COL_UTIL5, conf.util5,
                           STN_LIST_COL_UTIL5_NAME, conf.util5_name,
                           STN_LIST_COL_UTIL6, conf.util6,
                           STN_LIST_COL_UTIL6_NAME, conf.util6_name,
                           STN_LIST_COL_ANA1, conf.ana1,
                           STN_LIST_COL_ANA1_NAME, conf.ana1_name,
                           STN_LIST_COL_ANA2, conf.ana2,
                           STN_LIST_COL_ANA2_NAME, conf.ana2_name,
                           STN_LIST_COL_DIG1, conf.dig1,
                           STN_LIST_COL_DIG1_NAME, conf.dig1_name,
                           STN_LIST_COL_DIG2, conf.dig2,
                           STN_LIST_COL_DIG2_NAME, conf.dig2_name,
                           STN_LIST_COL_DIG3, conf.dig3,
                           STN_LIST_COL_DIG3_NAME, conf.dig3_name,
                           STN_LIST_COL_CONFIG1, conf.config1,
                           STN_LIST_COL_CONFIG1_NAME, conf.config1_name,
                           STN_LIST_COL_CONFIG1_ON, conf.config1_on,
                           STN_LIST_COL_CONFIG1_OFF, conf.config1_off,
                           STN_LIST_COL_CONFIG2, conf.config2,
                           STN_LIST_COL_CONFIG2_NAME, conf.config2_name,
                           STN_LIST_COL_CONFIG2_ON, conf.config2_on,
                           STN_LIST_COL_CONFIG2_OFF, conf.config2_off,
                           STN_LIST_COL_CONFIG3, conf.config3,
                           STN_LIST_COL_CONFIG3_NAME, conf.config3_name,
                           STN_LIST_COL_CONFIG3_ON, conf.config3_on,
                           STN_LIST_COL_CONFIG3_OFF, conf.config3_off,
                           STN_LIST_COL_CONFIG4, conf.config4,
                           STN_LIST_COL_CONFIG4_NAME, conf.config4_name,
                           STN_LIST_COL_CONFIG4_ON, conf.config4_on,
                           STN_LIST_COL_CONFIG4_OFF, conf.config4_off, -1);


        g_free(conf.name);

        if (conf.host != NULL)
            g_free(conf.host);
    }
}

static void edit_cb(GtkWidget * button, gpointer data)
{
    GtkTreeModel   *model = gtk_tree_view_get_model(GTK_TREE_VIEW(stnlist));
    GtkTreeModel   *selmod;
    GtkTreeSelection *selection;
    GtkTreeIter     iter;

    (void)button;               /* avoid unused parameter compiler warning */
    (void)data;                 /* avoid unused parameter compiler warning */

    station_conf_t    conf = {
        .name = NULL,
        .host = NULL,
        .port = 4520,

        /* Toggle Utilities ON/OFF */
        .util1 = 0,
        .util1_name = NULL,
        .util2 = 0,
        .util2_name = NULL,
        .util3 = 0,
        .util3_name = NULL,
        .util4 = 0,
        .util4_name = NULL,
        .util5 = 0,
        .util5_name = NULL,
        .util6 = 0,
        .util6_name = NULL,

        /* Monitor 2 ADCs */
        .ana1 = 0,
        .ana1_name = NULL,
        .ana2 = 0,
        .ana2_name = NULL,
 
        /* Monitor 3 digital inputs */
        .dig1 = 0,
        .dig1_name = NULL,
        .dig2 = 0,
        .dig2_name = NULL,
        .dig3 = 0,
        .dig3_name = NULL,

        /* Use 4 relays for station config */
        .config1 = 0,
        .config1_name = NULL,
        .config1_on = NULL,
        .config1_off = NULL,
        .config2 = 0,
        .config2_name = NULL,
        .config2_on = NULL,
        .config2_off = NULL,
        .config3 = 0,
        .config3_name = NULL,
        .config3_on = NULL,
        .config3_off = NULL,
        .config4 = 0,
        .config4_name = NULL,
        .config4_on = NULL,
        .config4_off = NULL        
    };

    /* If there are no entries, we have a bug since the button should 
       have been disabled. */
    if (gtk_tree_model_iter_n_children(model, NULL) < 1)
    {

        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s:%s: Edit button should have been disabled."),
                    __FILE__, __func__);
        //gtk_widget_set_sensitive (button, FALSE);

        return;
    }

    /* get selected row
       FIXME: do we really need to work with two models?
     */
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(stnlist));
    if (gtk_tree_selection_get_selected(selection, &selmod, &iter))
    {
        gtk_tree_model_get(model, &iter,
                           STN_LIST_COL_NAME, &conf.name,
                           STN_LIST_COL_HOST, &conf.host,
                           STN_LIST_COL_PORT, &conf.port,
                           STN_LIST_COL_UTIL1, &conf.util1,
                           STN_LIST_COL_UTIL1_NAME, &conf.util1_name,
                           STN_LIST_COL_UTIL2, &conf.util2,
                           STN_LIST_COL_UTIL2_NAME, &conf.util2_name,
                           STN_LIST_COL_UTIL3, &conf.util3,
                           STN_LIST_COL_UTIL3_NAME, &conf.util3_name,
                           STN_LIST_COL_UTIL4, &conf.util4,
                           STN_LIST_COL_UTIL4_NAME, &conf.util4_name,
                           STN_LIST_COL_UTIL5, &conf.util5,
                           STN_LIST_COL_UTIL5_NAME, &conf.util5_name,
                           STN_LIST_COL_UTIL6, &conf.util6,
                           STN_LIST_COL_UTIL6_NAME, &conf.util6_name,
                           STN_LIST_COL_ANA1, &conf.ana1,
                           STN_LIST_COL_ANA1_NAME, &conf.ana1_name,
                           STN_LIST_COL_ANA2, &conf.ana2,
                           STN_LIST_COL_ANA2_NAME, &conf.ana2_name,
                           STN_LIST_COL_DIG1, &conf.dig1,
                           STN_LIST_COL_DIG1_NAME, &conf.dig1_name,
                           STN_LIST_COL_DIG2, &conf.dig2,
                           STN_LIST_COL_DIG2_NAME, &conf.dig2_name,
                           STN_LIST_COL_DIG3, &conf.dig3,
                           STN_LIST_COL_DIG3_NAME, &conf.dig3_name,
                           STN_LIST_COL_CONFIG1, &conf.config1,
                           STN_LIST_COL_CONFIG1_NAME, &conf.config1_name,
                           STN_LIST_COL_CONFIG1_ON, &conf.config1_on,
                           STN_LIST_COL_CONFIG1_OFF, &conf.config1_off,
                           STN_LIST_COL_CONFIG2, &conf.config2,
                           STN_LIST_COL_CONFIG2_NAME, &conf.config2_name,
                           STN_LIST_COL_CONFIG2_ON, &conf.config2_on,
                           STN_LIST_COL_CONFIG2_OFF, &conf.config2_off,
                           STN_LIST_COL_CONFIG3, &conf.config3,
                           STN_LIST_COL_CONFIG3_NAME, &conf.config3_name,
                           STN_LIST_COL_CONFIG3_ON, &conf.config3_on,
                           STN_LIST_COL_CONFIG3_OFF, &conf.config3_off,
                           STN_LIST_COL_CONFIG4, &conf.config4,
                           STN_LIST_COL_CONFIG4_NAME, &conf.config4_name,
                           STN_LIST_COL_CONFIG4_ON, &conf.config4_on,
                           STN_LIST_COL_CONFIG4_OFF, &conf.config4_off, -1);
    }
    else
    {
        GtkWidget      *dialog;

        dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                        GTK_DIALOG_MODAL |
                                        GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_ERROR,
                                        GTK_BUTTONS_OK,
                                        _
                                        ("Select the station you want to edit\n"
                                         "and try again!"));
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);

        return;
    }

    /* run station configuration editor */
    sat_pref_stn_editor_run(&conf);

    /* apply changes */
    if (conf.name != NULL)
    {
        gtk_list_store_set(GTK_LIST_STORE(model), &iter,
                           STN_LIST_COL_NAME, conf.name,
                           STN_LIST_COL_HOST, conf.host,
                           STN_LIST_COL_PORT, conf.port,
                           STN_LIST_COL_UTIL1, conf.util1,
                           STN_LIST_COL_UTIL1_NAME, conf.util1_name,
                           STN_LIST_COL_UTIL2, conf.util2,
                           STN_LIST_COL_UTIL2_NAME, conf.util2_name,
                           STN_LIST_COL_UTIL3, conf.util3,
                           STN_LIST_COL_UTIL3_NAME, conf.util3_name,
                           STN_LIST_COL_UTIL4, conf.util4,
                           STN_LIST_COL_UTIL4_NAME, conf.util4_name,
                           STN_LIST_COL_UTIL5, conf.util5,
                           STN_LIST_COL_UTIL5_NAME, conf.util5_name,
                           STN_LIST_COL_UTIL6, conf.util6,
                           STN_LIST_COL_UTIL6_NAME, conf.util6_name,
                           STN_LIST_COL_ANA1, conf.ana1,
                           STN_LIST_COL_ANA1_NAME, conf.ana1_name,
                           STN_LIST_COL_ANA2, conf.ana2,
                           STN_LIST_COL_ANA2_NAME, conf.ana2_name,
                           STN_LIST_COL_DIG1, conf.dig1,
                           STN_LIST_COL_DIG1_NAME, conf.dig1_name,
                           STN_LIST_COL_DIG2, conf.dig2,
                           STN_LIST_COL_DIG2_NAME, conf.dig2_name,
                           STN_LIST_COL_DIG3, conf.dig3,
                           STN_LIST_COL_DIG3_NAME, conf.dig3_name,
                           STN_LIST_COL_CONFIG1, conf.config1,
                           STN_LIST_COL_CONFIG1_NAME, conf.config1_name,
                           STN_LIST_COL_CONFIG1_ON, conf.config1_on,
                           STN_LIST_COL_CONFIG1_OFF, conf.config1_off,
                           STN_LIST_COL_CONFIG2, conf.config2,
                           STN_LIST_COL_CONFIG2_NAME, conf.config2_name,
                           STN_LIST_COL_CONFIG2_ON, conf.config2_on,
                           STN_LIST_COL_CONFIG2_OFF, conf.config2_off,
                           STN_LIST_COL_CONFIG3, conf.config3,
                           STN_LIST_COL_CONFIG3_NAME, conf.config3_name,
                           STN_LIST_COL_CONFIG3_ON, conf.config3_on,
                           STN_LIST_COL_CONFIG3_OFF, conf.config3_off,
                           STN_LIST_COL_CONFIG4, conf.config4,
                           STN_LIST_COL_CONFIG4_NAME, conf.config4_name,
                           STN_LIST_COL_CONFIG4_ON, conf.config4_on,
                           STN_LIST_COL_CONFIG4_OFF, conf.config4_off, -1);
    }

    /* clean up memory */
    if (conf.name)
        g_free(conf.name);

    if (conf.host != NULL)
        g_free(conf.host);
}

static void delete_cb(GtkWidget * button, gpointer data)
{
    GtkTreeModel   *model = gtk_tree_view_get_model(GTK_TREE_VIEW(stnlist));
    GtkTreeSelection *selection;
    GtkTreeIter     iter;

    (void)button;
    (void)data;

    /* If there are no entries, we have a bug since the button should 
       have been disabled. */
    if (gtk_tree_model_iter_n_children(model, NULL) < 1)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s:%s: Delete button should have been disabled."),
                    __FILE__, __func__);
        //gtk_widget_set_sensitive (button, FALSE);

        return;
    }

    /* get selected row */
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(stnlist));
    if (gtk_tree_selection_get_selected(selection, NULL, &iter))
    {
        gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
    }
    else
    {
        GtkWidget      *dialog;

        dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                        GTK_DIALOG_MODAL |
                                        GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_ERROR,
                                        GTK_BUTTONS_OK,
                                        _
                                        ("Select the station you want to delete\n"
                                         "and try again!"));
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}

static void row_activated_cb(GtkTreeView * tree_view,
                             GtkTreePath * path,
                             GtkTreeViewColumn * column, gpointer user_data)
{
    (void)tree_view;
    (void)path;
    (void)column;
    (void)user_data;

    edit_cb(editbutton, NULL);
}
static GtkTreeModel *create_and_fill_model()
{
    GtkListStore   *liststore;  /* the list store data structure */
    GtkTreeIter     item;       /* new item added to the list store */
    GDir           *dir = NULL; /* directory handle */
    GError         *error = NULL;       /* error flag and info */
    gchar          *dirname;    /* directory name */
    gchar         **vbuff;
    const gchar    *filename;   /* file name */
    station_conf_t    conf;

    /* create a new list store */
    liststore = gtk_list_store_new(STN_LIST_COL_NUM, G_TYPE_STRING,     // name
                                   G_TYPE_STRING,       // host
                                   G_TYPE_INT,          // port
                                   G_TYPE_INT,          // util1 relay
                                   G_TYPE_STRING,       // util1 name
                                   G_TYPE_INT,          // util2 relay
                                   G_TYPE_STRING,       // util2 name
                                   G_TYPE_INT,          // util3 relay
                                   G_TYPE_STRING,       // util3 name
                                   G_TYPE_INT,          // util4 relay
                                   G_TYPE_STRING,       // util4 name
                                   G_TYPE_INT,          // util5 relay
                                   G_TYPE_STRING,       // util5 name
                                   G_TYPE_INT,          // util6 relay
                                   G_TYPE_STRING,       // util6 name
                                   G_TYPE_INT,          // ana1 relay
                                   G_TYPE_STRING,       // ana1 name
                                   G_TYPE_INT,          // ana2 relay
                                   G_TYPE_STRING,       // ana2 name
                                   G_TYPE_INT,          // dig1 relay
                                   G_TYPE_STRING,       // dig1 name
                                   G_TYPE_INT,          // dig2 relay
                                   G_TYPE_STRING,       // dig2 name
                                   G_TYPE_INT,          // dig3 relay
                                   G_TYPE_STRING,       // dig3 name
                                   G_TYPE_INT,          // config1 relay
                                   G_TYPE_STRING,       // config1 name
                                   G_TYPE_STRING,       // config1 on
                                   G_TYPE_STRING,       // config1 off
                                   G_TYPE_INT,          // config2 relay
                                   G_TYPE_STRING,       // config2 name
                                   G_TYPE_STRING,       // config2 on
                                   G_TYPE_STRING,       // config2 off
                                   G_TYPE_INT,          // config3 relay
                                   G_TYPE_STRING,       // config3 name
                                   G_TYPE_STRING,       // config3 on
                                   G_TYPE_STRING,       // config3 off
                                   G_TYPE_INT,          // config4 relay
                                   G_TYPE_STRING,       // config4 name
                                   G_TYPE_STRING,       // config4 on
                                   G_TYPE_STRING       // config4 off

        );
    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(liststore),
                                         STN_LIST_COL_NAME,
                                         GTK_SORT_ASCENDING);
    /* open configuration directory */
    dirname = get_hwconf_dir();

    dir = g_dir_open(dirname, 0, &error);
    if (dir)
    {
        /* read each .stn file */
        while ((filename = g_dir_read_name(dir)))
        {
            if (g_str_has_suffix(filename, ".stn"))
            {
                vbuff = g_strsplit(filename, ".stn", 0);
                conf.name = g_strdup(vbuff[0]);
                g_strfreev(vbuff);
                if (station_conf_read(&conf))
                {
                    /* insert conf into liststore */
                    gtk_list_store_append(liststore, &item);
                    gtk_list_store_set(liststore, &item,
                                       STN_LIST_COL_NAME, conf.name,
                                       STN_LIST_COL_HOST, conf.host,
                                       STN_LIST_COL_PORT, conf.port,
                                       STN_LIST_COL_UTIL1, conf.util1,
                                       STN_LIST_COL_UTIL1_NAME, conf.util1_name,
                                       STN_LIST_COL_UTIL2, conf.util2,
                                       STN_LIST_COL_UTIL2_NAME, conf.util2_name,
                                       STN_LIST_COL_UTIL3, conf.util3,
                                       STN_LIST_COL_UTIL3_NAME, conf.util3_name,
                                       STN_LIST_COL_UTIL4, conf.util4,
                                       STN_LIST_COL_UTIL4_NAME, conf.util4_name,
                                       STN_LIST_COL_UTIL5, conf.util5,
                                       STN_LIST_COL_UTIL5_NAME, conf.util5_name,
                                       STN_LIST_COL_UTIL6, conf.util6,
                                       STN_LIST_COL_UTIL6_NAME, conf.util6_name,
                                       STN_LIST_COL_ANA1, conf.ana1,
                                       STN_LIST_COL_ANA1_NAME, conf.ana1_name,
                                       STN_LIST_COL_ANA2, conf.ana2,
                                       STN_LIST_COL_ANA2_NAME, conf.ana2_name,
                                       STN_LIST_COL_DIG1, conf.dig1,
                                       STN_LIST_COL_DIG1_NAME, conf.dig1_name,
                                       STN_LIST_COL_DIG2, conf.dig2,
                                       STN_LIST_COL_DIG2_NAME, conf.dig2_name,
                                       STN_LIST_COL_DIG3, conf.dig3,
                                       STN_LIST_COL_DIG3_NAME, conf.dig3_name,
                                       STN_LIST_COL_CONFIG1, conf.config1,
                                       STN_LIST_COL_CONFIG1_NAME, conf.config1_name,
                                       STN_LIST_COL_CONFIG1_ON, conf.config1_on,
                                       STN_LIST_COL_CONFIG1_OFF, conf.config1_off,
                                       STN_LIST_COL_CONFIG2, conf.config2,
                                       STN_LIST_COL_CONFIG2_NAME, conf.config2_name,
                                       STN_LIST_COL_CONFIG2_ON, conf.config2_on,
                                       STN_LIST_COL_CONFIG2_OFF, conf.config2_off,
                                       STN_LIST_COL_CONFIG3, conf.config3,
                                       STN_LIST_COL_CONFIG3_NAME, conf.config3_name,
                                       STN_LIST_COL_CONFIG3_ON, conf.config3_on,
                                       STN_LIST_COL_CONFIG3_OFF, conf.config3_off,
                                       STN_LIST_COL_CONFIG4, conf.config4,
                                       STN_LIST_COL_CONFIG4_NAME, conf.config4_name,
                                       STN_LIST_COL_CONFIG4_ON, conf.config4_on,
                                       STN_LIST_COL_CONFIG4_OFF, conf.config4_off, -1);

                    sat_log_log(SAT_LOG_LEVEL_ERROR,
                                _("%s:%d: Read %s"),
                                __FILE__, __LINE__, filename);
                    /* clean up memory */
                    if (conf.name)
                        g_free(conf.name);

                    if (conf.host)
                        g_free(conf.host);
                }
                else
                {
                    /* there was an error */
                    sat_log_log(SAT_LOG_LEVEL_ERROR,
                                _("%s:%d: Failed to read %s"),
                                __FILE__, __LINE__, conf.name);

                    g_free(conf.name);
                }
            }
        }
    }
    else
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s:%d: Failed to open hwconf dir (%s)"),
                    __FILE__, __LINE__, error->message);
        g_clear_error(&error);
    }

    g_free(dirname);
    g_dir_close(dir);

    return GTK_TREE_MODEL(liststore);
}

/**
 * Create buttons.
 * @return A button box containing the buttons.
 *
 * This function creates and initialises the three buttons below the stn list.
 * The treeview widget is needed by the buttons when they are activated.
 *
 */
static GtkWidget *create_buttons(void)
{
    GtkWidget      *box;

    addbutton = gtk_button_new_with_label(_("Add New"));
    gtk_widget_set_tooltip_text(addbutton, _("Add a new station"));
    g_signal_connect(addbutton, "clicked", G_CALLBACK(add_cb), NULL);

    editbutton = gtk_button_new_with_label(_("Edit"));
    gtk_widget_set_tooltip_text(editbutton, _("Edit selected station"));
    g_signal_connect(editbutton, "clicked", G_CALLBACK(edit_cb), NULL);

    delbutton = gtk_button_new_with_label(_("Delete"));
    gtk_widget_set_tooltip_text(delbutton, _("Delete selected station"));
    g_signal_connect(delbutton, "clicked", G_CALLBACK(delete_cb), NULL);

    box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(box), GTK_BUTTONBOX_START);

    gtk_container_add(GTK_CONTAINER(box), addbutton);
    gtk_container_add(GTK_CONTAINER(box), editbutton);
    gtk_container_add(GTK_CONTAINER(box), delbutton);

    return box;
}



static void create_stn_list()
{
    GtkTreeModel   *model;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    stnlist = gtk_tree_view_new();

    model = create_and_fill_model();
    gtk_tree_view_set_model(GTK_TREE_VIEW(stnlist), model);
    g_object_unref(model);

    /* Conf name */
    renderer = gtk_cell_renderer_text_new();
    column =
        gtk_tree_view_column_new_with_attributes(_("Station Name"), renderer,
                                                 "text", STN_LIST_COL_NAME,
                                                 NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    /* Host */
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Host"), renderer,
                                                      "text",
                                                      STN_LIST_COL_HOST, NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    /* port */
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Port"), renderer,
                                                      "text",
                                                      STN_LIST_COL_PORT, NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    /* utils */
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Util 1 Relay"), renderer,
                                                      "text",
                                                      STN_LIST_COL_UTIL1,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Util 1 Name"), renderer,
                                                      "text",
                                                      STN_LIST_COL_UTIL1_NAME,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Util 2 Relay"), renderer,
                                                      "text",
                                                      STN_LIST_COL_UTIL2,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Util 2 Name"), renderer,
                                                      "text",
                                                      STN_LIST_COL_UTIL2_NAME,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Util 3 Relay"), renderer,
                                                      "text",
                                                      STN_LIST_COL_UTIL3,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Util 3 Name"), renderer,
                                                      "text",
                                                      STN_LIST_COL_UTIL3_NAME,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Util 4 Relay"), renderer,
                                                      "text",
                                                      STN_LIST_COL_UTIL4,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Util 4 Name"), renderer,
                                                      "text",
                                                      STN_LIST_COL_UTIL4_NAME,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Util 5 Relay"), renderer,
                                                      "text",
                                                      STN_LIST_COL_UTIL5,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Util 5 Name"), renderer,
                                                      "text",
                                                      STN_LIST_COL_UTIL5_NAME,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Util 6 Relay"), renderer,
                                                      "text",
                                                      STN_LIST_COL_UTIL6,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Util 6 Name"), renderer,
                                                      "text",
                                                      STN_LIST_COL_UTIL6_NAME,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    /* analog inputs */

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Analog 1 Input"), renderer,
                                                      "text",
                                                      STN_LIST_COL_ANA1,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Analog 1 Name"), renderer,
                                                      "text",
                                                      STN_LIST_COL_ANA1_NAME,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Analog 2 Input"), renderer,
                                                      "text",
                                                      STN_LIST_COL_ANA2,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Analog 2 Name"), renderer,
                                                      "text",
                                                      STN_LIST_COL_ANA2_NAME,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    /* digital inputs */

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Digital 1 Input"), renderer,
                                                      "text",
                                                      STN_LIST_COL_DIG1,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Digital 1 Name"), renderer,
                                                      "text",
                                                      STN_LIST_COL_DIG1_NAME,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Digital 2 Input"), renderer,
                                                      "text",
                                                      STN_LIST_COL_DIG2,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Digital 2 Name"), renderer,
                                                      "text",
                                                      STN_LIST_COL_DIG2_NAME,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    /* config relays */
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Config 1 Relay"), renderer,
                                                      "text",
                                                      STN_LIST_COL_CONFIG1,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Config 1 Name"), renderer,
                                                      "text",
                                                      STN_LIST_COL_CONFIG1_NAME,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Config 1 On"), renderer,
                                                      "text",
                                                      STN_LIST_COL_CONFIG1_ON,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Config 1 Off"), renderer,
                                                      "text",
                                                      STN_LIST_COL_CONFIG1_OFF,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Config 2 Relay"), renderer,
                                                      "text",
                                                      STN_LIST_COL_CONFIG2,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Config 2 Name"), renderer,
                                                      "text",
                                                      STN_LIST_COL_CONFIG2_NAME,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Config 2 On"), renderer,
                                                      "text",
                                                      STN_LIST_COL_CONFIG2_ON,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Config 2 Off"), renderer,
                                                      "text",
                                                      STN_LIST_COL_CONFIG2_OFF,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Config 3 Relay"), renderer,
                                                      "text",
                                                      STN_LIST_COL_CONFIG3,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Config 3 Name"), renderer,
                                                      "text",
                                                      STN_LIST_COL_CONFIG3_NAME,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Config 3 On"), renderer,
                                                      "text",
                                                      STN_LIST_COL_CONFIG3_ON,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Config 3 Off"), renderer,
                                                      "text",
                                                      STN_LIST_COL_CONFIG3_OFF,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Config 4 Relay"), renderer,
                                                      "text",
                                                      STN_LIST_COL_CONFIG4,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Config 4 Name"), renderer,
                                                      "text",
                                                      STN_LIST_COL_CONFIG4_NAME,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Config 4 On"), renderer,
                                                      "text",
                                                      STN_LIST_COL_CONFIG4_ON,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Config 4 Off"), renderer,
                                                      "text",
                                                      STN_LIST_COL_CONFIG4_OFF,
                                                      NULL);
    gtk_tree_view_insert_column(GTK_TREE_VIEW(stnlist), column, -1);
    
    g_signal_connect(stnlist, "row-activated", G_CALLBACK(row_activated_cb),
                     NULL);

}

GtkWidget      *sat_pref_stn_create()
{
    GtkWidget      *vbox;       /* vbox containing the list part and the details part */
    GtkWidget      *swin;

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_set_homogeneous(GTK_BOX(vbox), FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);

    /* create stn list and pack into scrolled window */
    create_stn_list();
    swin = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(swin), stnlist);

    gtk_box_pack_start(GTK_BOX(vbox), swin, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), create_buttons(), FALSE, FALSE, 0);

    return vbox;
}

/** User pressed cancel. Any changes to config must be cancelled. */
void sat_pref_stn_cancel()
{
}

/**
 * User pressed OK. Any changes should be stored in config.
 * 
 * First, all .stn files are deleted, whereafter the station configurations in
 * the stnlist are saved one by one.
 */
void sat_pref_stn_ok()
{
    GDir           *dir = NULL; /* directory handle */
    GError         *error = NULL;       /* error flag and info */
    gchar          *buff, *dirname;
    const gchar    *filename;
    GtkTreeIter     iter;       /* new item added to the list store */
    GtkTreeModel   *model;
    guint           i, n;

    station_conf_t    conf = {
        .name = NULL,
        .host = NULL,
        .port = 4520,

        /* Toggle Utilities ON/OFF */
        .util1 = 0,
        .util1_name = NULL,
        .util2 = 0,
        .util2_name = NULL,
        .util3 = 0,
        .util3_name = NULL,
        .util4 = 0,
        .util4_name = NULL,
        .util5 = 0,
        .util5_name = NULL,
        .util6 = 0,
        .util6_name = NULL,

        /* Monitor 2 ADCs */
        .ana1 = 0,
        .ana1_name = NULL,
        .ana2 = 0,
        .ana2_name = NULL,
 
        /* Monitor 3 digital inputs */
        .dig1 = 0,
        .dig1_name = NULL,
        .dig2 = 0,
        .dig2_name = NULL,
        .dig3 = 0,
        .dig3_name = NULL,

        /* Use 4 relays for station config */
        .config1 = 0,
        .config1_name = NULL,
        .config1_on = NULL,
        .config1_off = NULL,
        .config2 = 0,
        .config2_name = NULL,
        .config2_on = NULL,
        .config2_off = NULL,
        .config3 = 0,
        .config3_name = NULL,
        .config3_on = NULL,
        .config3_off = NULL,
        .config4 = 0,
        .config4_name = NULL,
        .config4_on = NULL,
        .config4_off = NULL        
    };


    /* delete all .stn files */
    dirname = get_hwconf_dir();

    dir = g_dir_open(dirname, 0, &error);
    if (dir)
    {
        /* read each .stn file */
        while ((filename = g_dir_read_name(dir)))
        {
            if (g_str_has_suffix(filename, ".stn"))
            {

                buff = g_strconcat(dirname, G_DIR_SEPARATOR_S, filename, NULL);
                if (g_remove(buff))
                    sat_log_log(SAT_LOG_LEVEL_ERROR,
                                _("%s: Failed to remove %s"), __func__, buff);
                g_free(buff);
            }
        }
    }

    g_free(dirname);
    g_dir_close(dir);

    /* create new .stn files for the station in the stnlist */
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(stnlist));
    n = gtk_tree_model_iter_n_children(model, NULL);
    for (i = 0; i < n; i++)
    {
        /* get station conf */
        if (gtk_tree_model_iter_nth_child(model, &iter, NULL, i))
        {

            /* store conf */
            gtk_tree_model_get(model, &iter,
                               STN_LIST_COL_NAME, &conf.name,
                               STN_LIST_COL_HOST, &conf.host,
                               STN_LIST_COL_PORT, &conf.port,
                               STN_LIST_COL_UTIL1, &conf.util1,
                               STN_LIST_COL_UTIL1_NAME, &conf.util1_name,
                               STN_LIST_COL_UTIL2, &conf.util2,
                               STN_LIST_COL_UTIL2_NAME, &conf.util2_name,
                               STN_LIST_COL_UTIL3, &conf.util3,
                               STN_LIST_COL_UTIL3_NAME, &conf.util3_name,
                               STN_LIST_COL_UTIL4, &conf.util4,
                               STN_LIST_COL_UTIL4_NAME, &conf.util4_name,
                               STN_LIST_COL_UTIL5, &conf.util5,
                               STN_LIST_COL_UTIL5_NAME, &conf.util5_name,
                               STN_LIST_COL_UTIL6, &conf.util6,
                               STN_LIST_COL_UTIL6_NAME, &conf.util6_name,
                               STN_LIST_COL_ANA1, &conf.ana1,
                               STN_LIST_COL_ANA1_NAME, &conf.ana1_name,
                               STN_LIST_COL_ANA2, &conf.ana2,
                               STN_LIST_COL_ANA2_NAME, &conf.ana2_name,
                               STN_LIST_COL_DIG1, &conf.dig1,
                               STN_LIST_COL_DIG1_NAME, &conf.dig1_name,
                               STN_LIST_COL_DIG2, &conf.dig2,
                               STN_LIST_COL_DIG2_NAME, &conf.dig2_name,
                               STN_LIST_COL_DIG3, &conf.dig3,
                               STN_LIST_COL_DIG3_NAME, &conf.dig3_name,
                               STN_LIST_COL_CONFIG1, &conf.config1,
                               STN_LIST_COL_CONFIG1_NAME, &conf.config1_name,
                               STN_LIST_COL_CONFIG1_ON, &conf.config1_on,
                               STN_LIST_COL_CONFIG1_OFF, &conf.config1_off,
                               STN_LIST_COL_CONFIG2, &conf.config2,
                               STN_LIST_COL_CONFIG2_NAME, &conf.config2_name,
                               STN_LIST_COL_CONFIG2_ON, &conf.config2_on,
                               STN_LIST_COL_CONFIG2_OFF, &conf.config2_off,
                               STN_LIST_COL_CONFIG3, &conf.config3,
                               STN_LIST_COL_CONFIG3_NAME, &conf.config3_name,
                               STN_LIST_COL_CONFIG3_ON, &conf.config3_on,
                               STN_LIST_COL_CONFIG3_OFF, &conf.config3_off,
                               STN_LIST_COL_CONFIG4, &conf.config4,
                               STN_LIST_COL_CONFIG4_NAME, &conf.config4_name,
                               STN_LIST_COL_CONFIG4_ON, &conf.config4_on,
                               STN_LIST_COL_CONFIG4_OFF, &conf.config4_off, -1);
            station_conf_save(&conf);

            /* free conf buffer */
            if (conf.name)
                g_free(conf.name);

            if (conf.host)
                g_free(conf.host);

        }
        else
        {
            sat_log_log(SAT_LOG_LEVEL_ERROR,
                        _("%s: Failed to get STN %s"), __func__, i);
        }
    }
}
