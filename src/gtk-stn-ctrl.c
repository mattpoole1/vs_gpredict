
#ifdef HAVE_CONFIG_H
#include <build-config.h>
#endif

#include <errno.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <math.h>
#include <string.h>            
#include "gtk-stn-ctrl.h"
#include "sat-log.h"


static GtkVBoxClass *parent_class = NULL;

static gint stn_name_compare(const gchar * a, const gchar * b)
{
    return (gpredict_strcmp(a, b));
}

/**
 * New station selected.
 *
 * \param box Pointer to the station selector combo box.
 * \param data Pointer to the GtkStnCtrl widget.
 * 
 * This function is called when the user selects a new station controller
 * device.
 */
static void stn_selected_cb(GtkComboBox * box, gpointer data)
{
    GtkStnCtrl     *ctrl = GTK_STN_CTRL(data);

    /* free previous configuration */
    if (ctrl->conf != NULL)
    {
        g_free(ctrl->conf->name);
        g_free(ctrl->conf->host);
        g_free(ctrl->conf);
    }

    ctrl->conf = g_try_new(station_conf_t, 1);
    if (ctrl->conf == NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s:%d: Failed to allocate memory for station config"),
                    __FILE__, __LINE__);
        return;
    }

    /* load new configuration */
    ctrl->conf->name =
        gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(box));
    if (station_conf_read(ctrl->conf))
    {
        sat_log_log(SAT_LOG_LEVEL_INFO,
                    _("Loaded new station configuration %s"),
                    ctrl->conf->name);
    }
    else
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s:%d: Failed to load station configuration %s"),
                    __FILE__, __LINE__, ctrl->conf->name);

        g_free(ctrl->conf->name);
        if (ctrl->conf->host)
            g_free(ctrl->conf->host);
        g_free(ctrl->conf);
        ctrl->conf = NULL;
    }
}

/**
 * Manage toggle signals (connect)
 *
 * \param button Pointer to the GtkToggle button.
 * \param data Pointer to the GtkStnCtrl widget.
 */
static void connect_toggle_cb(GtkToggleButton * button, gpointer data)
{
    GtkStnCtrl     *ctrl = GTK_STN_CTRL(data);

    sat_log_log(SAT_LOG_LEVEL_DEBUG,
                _("%s: Connect Button Clicked"), __func__);

}

/*
 * Update station control state.
 * 
 * This function is called by the parent, i.e. GtkSatModule, indicating that
 * the satellite data has been updated. The function updates the internal state
 * of the controller and the station.
 */
void gtk_stn_ctrl_update(GtkStnCtrl * ctrl, gdouble t)
{
    
}

static gboolean have_conf()
{
    GDir           *dir = NULL; /* directory handle */
    GError         *error = NULL;       /* error flag and info */
    gchar          *dirname;    /* directory name */
    const gchar    *filename;   /* file name */
    gint            i = 0;

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
                i++;
                /*once we have one we need nothing else */
                break;
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

    return (i > 0) ? TRUE : FALSE;
}


static void gtk_stn_ctrl_init(GtkStnCtrl * ctrl)
{
    
}

static void gtk_stn_ctrl_destroy(GtkWidget * widget)
{
    GtkStnCtrl     *ctrl = GTK_STN_CTRL(widget);

    /* free configuration */
    if (ctrl->conf != NULL)
    {
        g_free(ctrl->conf->name);
        g_free(ctrl->conf->host);
        g_free(ctrl->conf);
        ctrl->conf = NULL;
    }

    (*GTK_WIDGET_CLASS(parent_class)->destroy) (widget);   


}

static void gtk_stn_ctrl_class_init(GtkStnCtrlClass * class)
{
    GtkWidgetClass *widget_class = (GtkWidgetClass *) class;

    widget_class->destroy = gtk_stn_ctrl_destroy;
    parent_class = g_type_class_peek_parent(class);
}


GType gtk_stn_ctrl_get_type()
{
    static GType    gtk_stn_ctrl_type = 0;

    if (!gtk_stn_ctrl_type)
    {
        static const GTypeInfo gtk_stn_ctrl_info = {
            sizeof(GtkStnCtrlClass),
            NULL,               /* base_init */
            NULL,               /* base_finalize */
            (GClassInitFunc) gtk_stn_ctrl_class_init,
            NULL,               /* class_finalize */
            NULL,               /* class_data */
            sizeof(GtkStnCtrl),
            5,                  /* n_preallocs */
            (GInstanceInitFunc) gtk_stn_ctrl_init,
            NULL
        };

        gtk_stn_ctrl_type = g_type_register_static(GTK_TYPE_BOX,
                                                   "GtkStnCtrl",
                                                   &gtk_stn_ctrl_info, 0);
    }

    return gtk_stn_ctrl_type;
}

static GtkWidget *create_settings(GtkStnCtrl * ctrl) {

    GtkWidget *frame, *table, *label;
    GDir           *dir = NULL; /* directory handle */
    GError         *error = NULL;       /* error flag and info */
    gchar          *dirname;    /* directory name */
    gchar         **vbuff;
    const gchar    *filename;   /* file name */
    gchar          *stnname;

    table = gtk_grid_new();
    gtk_container_set_border_width(GTK_CONTAINER(table), 5);
    gtk_grid_set_column_spacing(GTK_GRID(table), 5);
    gtk_grid_set_row_spacing(GTK_GRID(table), 5);
    
    label = gtk_label_new(_("Station Select:"));
    g_object_set(label, "xalign", 1.0f, "yalign", 0.5f, NULL);
    gtk_grid_attach(GTK_GRID(table), label, 0, 0, 1, 1);

    ctrl->stnSel = gtk_combo_box_text_new();
    gtk_widget_set_tooltip_text(ctrl->stnSel,
                                _("Select station"));

    /* open configuration directory */
    dirname = get_hwconf_dir();

    dir = g_dir_open(dirname, 0, &error);
    if (dir)
    {
        /* read each .stn file */
        GSList         *stns = NULL;
        gint            i;
        gint            n;

        while ((filename = g_dir_read_name(dir)))
        {
            if (g_str_has_suffix(filename, ".stn"))
            {
                vbuff = g_strsplit(filename, ".stn", 0);
                stns =
                    g_slist_insert_sorted(stns, g_strdup(vbuff[0]),
                                          (GCompareFunc) stn_name_compare);
                g_strfreev(vbuff);
            }
        }
        n = g_slist_length(stns);
        for (i = 0; i < n; i++)
        {
            stnname = g_slist_nth_data(stns, i);
            if (stnname)
            {
                gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT
                                               (ctrl->stnSel), stnname);
                g_free(stnname);
            }
        }
        g_slist_free(stns);
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

    gtk_combo_box_set_active(GTK_COMBO_BOX(ctrl->stnSel), 0);
    g_signal_connect(ctrl->stnSel, "changed", G_CALLBACK(stn_selected_cb),
                     ctrl);
    gtk_grid_attach(GTK_GRID(table), ctrl->stnSel, 1, 0, 1, 1);

    ctrl->connect = gtk_toggle_button_new_with_label(_("Connect"));
    gtk_widget_set_tooltip_text(ctrl->connect,
                                _
                                ("Connect to the selected station"));
    gtk_grid_attach(GTK_GRID(table), ctrl->connect, 0, 1, 2, 1);
    g_signal_connect(ctrl->connect, "toggled", G_CALLBACK(connect_toggle_cb),
                     ctrl);

    frame = gtk_frame_new(_("Settings"));
    gtk_container_add(GTK_CONTAINER(frame), table);

    return frame;
}

GtkWidget      *gtk_stn_ctrl_new(GtkSatModule * module)
{
    GtkStnCtrl     *stn_ctrl;
    GtkWidget      *table;

    /* check that we have stn conf */
    if (!have_conf())
        return NULL;

    stn_ctrl = GTK_STN_CTRL(g_object_new(GTK_TYPE_STN_CTRL, NULL));

    /* create contents */
    table = gtk_grid_new();
    gtk_grid_set_column_homogeneous(GTK_GRID(table), TRUE);
    gtk_grid_set_row_homogeneous(GTK_GRID(table), FALSE);
    gtk_grid_set_row_spacing(GTK_GRID(table), 5);
    gtk_grid_set_column_spacing(GTK_GRID(table), 5);
    gtk_container_set_border_width(GTK_CONTAINER(table), 0);
    gtk_grid_attach(GTK_GRID(table), create_settings(stn_ctrl), 0, 0, 1, 1);
    //gtk_grid_attach(GTK_GRID(table), create_monitor(stn_ctrl), 1, 0, 1, 1);
    //gtk_grid_attach(GTK_GRID(table), create_config(stn_ctrl), 0, 1, 1, 1);
    //gtk_grid_attach(GTK_GRID(table), create_utils(stn_ctrl), 1, 1, 1, 1);

    gtk_box_pack_start(GTK_BOX(stn_ctrl), table, FALSE, FALSE, 5);
    gtk_container_set_border_width(GTK_CONTAINER(stn_ctrl), 5);


    return GTK_WIDGET(stn_ctrl);
}
