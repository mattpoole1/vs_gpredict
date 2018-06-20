#ifndef __GTK_STN_CTRL_H__
#define __GTK_STN_CTRL_H__ 1

#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#include "gtk-sat-module.h"
#include "station-conf.h"
#include "sgpsdp/sgp4sdp4.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define GTK_TYPE_STN_CTRL          (gtk_stn_ctrl_get_type ())
#define GTK_STN_CTRL(obj)          G_TYPE_CHECK_INSTANCE_CAST (obj,\
                                   gtk_stn_ctrl_get_type (),\
                                   GtkStnCtrl)

#define GTK_STN_CTRL_CLASS(klass)  G_TYPE_CHECK_CLASS_CAST (klass,\
                             gtk_stn_ctrl_get_type (),\
                             GtkStnCtrlClass)

#define IS_GTK_STN_CTRL(obj)       G_TYPE_CHECK_INSTANCE_TYPE (obj, gtk_stn_ctrl_get_type ())

typedef struct _gtk_stn_ctrl GtkStnCtrl;
typedef struct _GtkStnCtrlClass GtkStnCtrlClass;

struct _gtk_stn_ctrl {
    GtkBox          box;

    GtkWidget       *stnSel;
    GtkWidget       *load;
    GtkWidget       *connect;

    GtkWidget       *ana1, *ana2, *dig1, *dig2, *dig3;

    GtkWidget       *config1On, *config1Off, *config2On, *config2Off;
    GtkWidget       *config3On, *config3Off, *config4On, *config4Off;

    GtkWidget       *util1, *util2, *util3, *util4, *util5, *util6;

    gboolean        connected;
    guint           timerid;

    station_conf_t  *conf;

    /* TCP client to stnctld */
    struct {
        GThread    *thread;
        GTimer     *timer;
        GMutex      mutex;
        gint        socket;     /* network socket to stnctld */
        gdouble     ana1_s;
        gdouble     ana2_s;
        gboolean    dig1_s;
        gboolean    dig2_s;
        gboolean    dig3_s;
        gboolean    running;
        gboolean    io_error;
        gboolean    new_trg;
    } client;

};

struct _GtkStnCtrlClass {
    GtkBoxClass     parent_class;
};

GType           gtk_stn_ctrl_get_type(void);
GtkWidget      *gtk_stn_ctrl_new(void);
void            gtk_stn_ctrl_update(GtkStnCtrl * ctrl);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __GTK_STN_CTRL_H__ */
