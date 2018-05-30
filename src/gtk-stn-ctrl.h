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
    gboolean        pos;
    GtkWidget       *connect;

    station_conf_t  *conf;

};

struct _GtkStnCtrlClass {
    GtkBoxClass     parent_class;
};

GType           gtk_stn_ctrl_get_type(void);
GtkWidget      *gtk_stn_ctrl_new(GtkSatModule * module);
void            gtk_stn_ctrl_update(GtkStnCtrl * ctrl, gdouble t);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __GTK_STN_CTRL_H__ */
