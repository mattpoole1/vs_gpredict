
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
    ctrl->pos = NULL;
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

    gtk_box_pack_start(GTK_BOX(stn_ctrl), table, FALSE, FALSE, 5);
    gtk_container_set_border_width(GTK_CONTAINER(stn_ctrl), 5);


    return GTK_WIDGET(stn_ctrl);
}
