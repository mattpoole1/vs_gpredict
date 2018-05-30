
#ifdef HAVE_CONFIG_H
#include <build-config.h>
#endif
#include <glib/gi18n.h>
#include <glib/gstdio.h>
#include <gtk/gtk.h>
#include <math.h>

#include "gpredict-utils.h"
#include "station-conf.h"
#include "sat-cfg.h"
#include "sat-log.h"
#include "sat-pref-stn-editor.h"


extern GtkWidget *window;       /* dialog window defined in sat-pref.c */
static GtkWidget *dialog;       /* dialog window */
static GtkWidget *name;         /* Configuration name */
static GtkWidget *host;         /* host name or IP */
static GtkWidget *port;         /* port number */

static GtkWidget *util1;
static GtkWidget *util1_name;
static GtkWidget *util2;
static GtkWidget *util2_name;
static GtkWidget *util3;
static GtkWidget *util3_name;
static GtkWidget *util4;
static GtkWidget *util4_name;
static GtkWidget *util5;
static GtkWidget *util5_name;
static GtkWidget *util6;
static GtkWidget *util6_name;

static GtkWidget *ana1;
static GtkWidget *ana1_name;
static GtkWidget *ana2;
static GtkWidget *ana2_name;

static GtkWidget *dig1;
static GtkWidget *dig1_name;
static GtkWidget *dig2;
static GtkWidget *dig2_name;
static GtkWidget *dig3;
static GtkWidget *dig3_name;

static GtkWidget *config1;
static GtkWidget *config1_name;
static GtkWidget *config1_on;
static GtkWidget *config1_off;
static GtkWidget *config2;
static GtkWidget *config2_name;
static GtkWidget *config2_on;
static GtkWidget *config2_off;
static GtkWidget *config3;
static GtkWidget *config3_name;
static GtkWidget *config3_on;
static GtkWidget *config3_off;
static GtkWidget *config4;
static GtkWidget *config4_name;
static GtkWidget *config4_on;
static GtkWidget *config4_off;


/* Update widgets from the currently selected row in the treeview */
static void update_widgets(station_conf_t * conf)
{
    /* configuration name */
    gtk_entry_set_text(GTK_ENTRY(name), conf->name);

    /* host */
    if (conf->host)
        gtk_entry_set_text(GTK_ENTRY(host), conf->host);

    /* port */
    if (conf->port > 1023)
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(port), conf->port);
    else
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(port), 4520); 

    /* Update name entries */
    if (conf->util1_name)
        gtk_entry_set_text(GTK_ENTRY(util1_name), conf->util1_name);
    if (conf->util2_name)
        gtk_entry_set_text(GTK_ENTRY(util2_name), conf->util2_name);
    if (conf->util3_name)
        gtk_entry_set_text(GTK_ENTRY(util3_name), conf->util3_name);
    if (conf->util4_name)
        gtk_entry_set_text(GTK_ENTRY(util4_name), conf->util4_name);
    if (conf->util5_name)
        gtk_entry_set_text(GTK_ENTRY(util5_name), conf->util5_name);
    if (conf->util6_name)
        gtk_entry_set_text(GTK_ENTRY(util6_name), conf->util6_name);
    if (conf->ana1_name)
        gtk_entry_set_text(GTK_ENTRY(ana1_name), conf->ana1_name);
    if (conf->ana2_name)
        gtk_entry_set_text(GTK_ENTRY(ana2_name), conf->ana2_name);
    if (conf->dig1_name)
        gtk_entry_set_text(GTK_ENTRY(dig1_name), conf->dig1_name);
    if (conf->dig2_name)
        gtk_entry_set_text(GTK_ENTRY(dig2_name), conf->dig2_name);
    if (conf->dig3_name)
        gtk_entry_set_text(GTK_ENTRY(dig3_name), conf->dig3_name);
    if (conf->config1_name)
        gtk_entry_set_text(GTK_ENTRY(config1_name), conf->config1_name);
    if (conf->config2_name)
        gtk_entry_set_text(GTK_ENTRY(config2_name), conf->config2_name);
    if (conf->config3_name)
        gtk_entry_set_text(GTK_ENTRY(config3_name), conf->config3_name);
    if (conf->config4_name)
        gtk_entry_set_text(GTK_ENTRY(config4_name), conf->config4_name);
    if (conf->config1_on)
        gtk_entry_set_text(GTK_ENTRY(config1_on), conf->config1_on);
    if (conf->config2_on)
        gtk_entry_set_text(GTK_ENTRY(config2_on), conf->config2_on);
    if (conf->config3_on)
        gtk_entry_set_text(GTK_ENTRY(config3_on), conf->config3_on);
    if (conf->config4_on)
        gtk_entry_set_text(GTK_ENTRY(config4_on), conf->config4_on);
    if (conf->config1_off)
        gtk_entry_set_text(GTK_ENTRY(config1_off), conf->config1_off);
    if (conf->config2_off)
        gtk_entry_set_text(GTK_ENTRY(config2_off), conf->config2_off);
    if (conf->config3_off)
        gtk_entry_set_text(GTK_ENTRY(config3_off), conf->config3_off);
    if (conf->config4_off)
        gtk_entry_set_text(GTK_ENTRY(config4_off), conf->config4_off);


    /* Update numeric values */
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(util1), conf->util1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(util2), conf->util2);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(util3), conf->util3);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(util4), conf->util4);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(util5), conf->util5);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(util6), conf->util6);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(ana1), conf->ana1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(ana2), conf->ana2);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(dig1), conf->dig1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(dig2), conf->dig2);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(config1), conf->config1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(config2), conf->config2);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(config3), conf->config3);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(config4), conf->config4);

}

/* called when the user clicks on the CLEAR button */
static void clear_widgets()
{
    gtk_entry_set_text(GTK_ENTRY(name), "");
    gtk_entry_set_text(GTK_ENTRY(host), "localhost");
    gtk_entry_set_text(GTK_ENTRY(util1_name), "");
    gtk_entry_set_text(GTK_ENTRY(util2_name), "");
    gtk_entry_set_text(GTK_ENTRY(util3_name), "");
    gtk_entry_set_text(GTK_ENTRY(util4_name), "");
    gtk_entry_set_text(GTK_ENTRY(util5_name), "");
    gtk_entry_set_text(GTK_ENTRY(util6_name), "");
    gtk_entry_set_text(GTK_ENTRY(ana1_name), "");
    gtk_entry_set_text(GTK_ENTRY(ana2_name), "");
    gtk_entry_set_text(GTK_ENTRY(dig1_name), "");
    gtk_entry_set_text(GTK_ENTRY(dig2_name), "");
    gtk_entry_set_text(GTK_ENTRY(dig3_name), "");
    gtk_entry_set_text(GTK_ENTRY(config1_name), "");
    gtk_entry_set_text(GTK_ENTRY(config1_on), "");
    gtk_entry_set_text(GTK_ENTRY(config1_off), "");
    gtk_entry_set_text(GTK_ENTRY(config2_name), "");
    gtk_entry_set_text(GTK_ENTRY(config2_on), "");
    gtk_entry_set_text(GTK_ENTRY(config2_off), "");
    gtk_entry_set_text(GTK_ENTRY(config3_name), "");
    gtk_entry_set_text(GTK_ENTRY(config3_on), "");
    gtk_entry_set_text(GTK_ENTRY(config3_off), "");
    gtk_entry_set_text(GTK_ENTRY(config4_name), "");
    gtk_entry_set_text(GTK_ENTRY(config4_on), "");
    gtk_entry_set_text(GTK_ENTRY(config4_off), "");
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(port), 4520);    
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(util1), 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(util2), 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(util3), 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(util4), 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(util5), 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(util6), 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(ana1), 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(ana2), 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(dig1), 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(dig2), 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(config1), 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(config2), 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(config3), 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(config4), 0);
}

/*
 * This function is called when the contents of the name entry changes.
 * The primary purpose of this function is to check whether the char length
 * of the name is greater than zero, if yes enable the OK button of the dialog.
 */
static void name_changed(GtkWidget * widget, gpointer data)
{
    const gchar    *text;
    gchar          *entry, *end, *j;
    gint            len, pos;

    (void)data;

    /* step 1: ensure that only valid characters are entered
       (stolen from xlog, tnx pg4i)
     */
    entry = gtk_editable_get_chars(GTK_EDITABLE(widget), 0, -1);
    if ((len = g_utf8_strlen(entry, -1)) > 0)
    {
        end = entry + g_utf8_strlen(entry, -1);
        for (j = entry; j < end; ++j)
        {
            if (!gpredict_legal_char(*j))
            {
                gdk_beep();
                pos = gtk_editable_get_position(GTK_EDITABLE(widget));
                gtk_editable_delete_text(GTK_EDITABLE(widget), pos, pos + 1);
            }
        }
    }

    /* step 2: if name seems all right, enable OK button */
    text = gtk_entry_get_text(GTK_ENTRY(widget));

    if (g_utf8_strlen(text, -1) > 0)
    {
        gtk_dialog_set_response_sensitive(GTK_DIALOG(dialog),
                                          GTK_RESPONSE_OK, TRUE);
    }
    else
    {
        gtk_dialog_set_response_sensitive(GTK_DIALOG(dialog),
                                          GTK_RESPONSE_OK, FALSE);
    }
}

static GtkWidget *create_editor_widgets(station_conf_t * conf)
{
    GtkWidget      *table;
    GtkWidget      *info;
    GtkWidget      *utilities;
    GtkWidget      *analogs;
    GtkWidget      *digitals;
    GtkWidget      *configs;
    GtkWidget      *label;

    GtkWidget      *info_frame;
    GtkWidget      *utilities_frame;
    GtkWidget      *analogs_frame;
    GtkWidget      *digitals_frame;
    GtkWidget      *configs_frame;



    /* Info */
    /*----------------------------------------------------------------*/
    
    info = gtk_grid_new();
    gtk_container_set_border_width(GTK_CONTAINER(info), 5);
    gtk_grid_set_column_spacing(GTK_GRID(info), 5);
    gtk_grid_set_row_spacing(GTK_GRID(info), 5);

    /* Config name */
    label = gtk_label_new(_("Name"));
    g_object_set(label, "xalign", 1.0, "yalign", 0.5, NULL);
    gtk_grid_attach(GTK_GRID(info), label, 0, 0, 1, 1);

    name = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(name), 25);
    gtk_widget_set_tooltip_text(name,
                                _("Enter a short name for this configuration, "
                                  " e.g. STATION-1.\n"
                                  "Allowed characters: 0..9, a..z, A..Z, - and _"));
    gtk_grid_attach(GTK_GRID(info), name, 1, 0, 3, 1);

    /* attach changed signal so that we can enable OK button when
       a proper name has been entered
     */
    g_signal_connect(name, "changed", G_CALLBACK(name_changed), NULL);

    /* Host */
    label = gtk_label_new(_("Host"));
    g_object_set(label, "xalign", 1.0, "yalign", 0.5, NULL);
    gtk_grid_attach(GTK_GRID(info), label, 0, 1, 1, 1);

    host = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(host), 50);
    gtk_entry_set_text(GTK_ENTRY(host), "localhost");
    gtk_widget_set_tooltip_text(host,
                                _("Enter the host where Modbus interpreter is running. "
                                  "You can use both host name and IP address, "
                                  "e.g. 192.168.1.100\n\n"
                                  "If gpredict and Modbus interpreter are running on the "
                                  "same computer, use localhost"));
    gtk_grid_attach(GTK_GRID(info), host, 1, 1, 3, 1); 

    /* port */
    label = gtk_label_new(_("Port"));
    g_object_set(label, "xalign", 1.0, "yalign", 0.5, NULL);
    gtk_grid_attach(GTK_GRID(info), label, 0, 2, 1, 1);

    port = gtk_spin_button_new_with_range(1024, 65535, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(port), 4520);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(port), 0);
    gtk_widget_set_tooltip_text(port,
                                _("Enter the port number where Modbus interpreter is "
                                  "listening. Default is 4520."));
    gtk_grid_attach(GTK_GRID(info), port, 1, 2, 1, 1); 

    /* Utilities */
    /*--------------------------------------------------------------*/

    utilities = gtk_grid_new();
    gtk_container_set_border_width(GTK_CONTAINER(utilities), 5);
    gtk_grid_set_column_spacing(GTK_GRID(utilities), 5);
    gtk_grid_set_row_spacing(GTK_GRID(utilities), 5);

    /* Top Labels */

    label = gtk_label_new(_("Utility Name"));
    g_object_set(label, "xalign", 1.0, "yalign", 0.5, NULL);
    gtk_grid_attach(GTK_GRID(utilities), label, 0, 0, 3, 1);
    
    label = gtk_label_new(_("Utility Num"));
    g_object_set(label, "xalign", 1.0, "yalign", 0.5, NULL);
    gtk_grid_attach(GTK_GRID(utilities), label, 3, 0, 2, 1);

    /* Entries */

    util1_name = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(util1_name), 20);
    gtk_entry_set_text(GTK_ENTRY(util1_name), "");
    gtk_widget_set_tooltip_text(util1_name,
                                _("Enter the designator for this utility"));
    gtk_grid_attach(GTK_GRID(utilities), util1_name, 0, 1, 3, 1); 

    util2_name = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(util2_name), 20);
    gtk_entry_set_text(GTK_ENTRY(util2_name), "");
    gtk_widget_set_tooltip_text(util2_name,
                                _("Enter the designator for this utility"));
    gtk_grid_attach(GTK_GRID(utilities), util2_name, 0, 2, 3, 1); 

    util3_name = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(util3_name), 20);
    gtk_entry_set_text(GTK_ENTRY(util3_name), "");
    gtk_widget_set_tooltip_text(util3_name,
                                _("Enter the designator for this utility"));
    gtk_grid_attach(GTK_GRID(utilities), util3_name, 0, 3, 3, 1); 

    util4_name = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(util4_name), 20);
    gtk_entry_set_text(GTK_ENTRY(util4_name), "");
    gtk_widget_set_tooltip_text(util4_name,
                                _("Enter the designator for this utility"));
    gtk_grid_attach(GTK_GRID(utilities), util4_name, 0, 4, 3, 1); 

    util5_name = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(util5_name), 20);
    gtk_entry_set_text(GTK_ENTRY(util5_name), "");
    gtk_widget_set_tooltip_text(util5_name,
                                _("Enter the designator for this utility"));
    gtk_grid_attach(GTK_GRID(utilities), util5_name, 0, 5, 3, 1); 

    util6_name = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(util6_name), 20);
    gtk_entry_set_text(GTK_ENTRY(util6_name), "");
    gtk_widget_set_tooltip_text(util6_name,
                                _("Enter the designator for this utility"));
    gtk_grid_attach(GTK_GRID(utilities), util6_name, 0, 6, 3, 1); 

    /* Spin Buttons */

    util1 = gtk_spin_button_new_with_range(0, 16, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(util1), 0);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(util1), 0);
    gtk_widget_set_tooltip_text(util1,
                                _("Select the relay number that controls the "
                                  "this utility. Note this is the hardware labelled "
                                  "relay number that starts at 1, not the Modbus "
                                  "number that starts at 0. 0 means no utility attached."));
    gtk_grid_attach(GTK_GRID(utilities), util1, 3, 1, 2, 1);

    util2 = gtk_spin_button_new_with_range(0, 16, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(util2), 0);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(util2), 0);
    gtk_widget_set_tooltip_text(util2,
                                _("Select the relay number that controls the "
                                  "this utility. Note this is the hardware labelled "
                                  "relay number that starts at 1, not the Modbus "
                                  "number that starts at 0. 0 means no utility attached."));
    gtk_grid_attach(GTK_GRID(utilities), util2, 3, 2, 2, 1);

    util3 = gtk_spin_button_new_with_range(0, 16, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(util3), 0);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(util3), 0);
    gtk_widget_set_tooltip_text(util3,
                                _("Select the relay number that controls the "
                                  "this utility. Note this is the hardware labelled "
                                  "relay number that starts at 1, not the Modbus "
                                  "number that starts at 0. 0 means no utility attached."));
    gtk_grid_attach(GTK_GRID(utilities), util3, 3, 3, 2, 1);

    util4 = gtk_spin_button_new_with_range(0, 16, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(util4), 0);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(util4), 0);
    gtk_widget_set_tooltip_text(util4,
                                _("Select the relay number that controls the "
                                  "this utility. Note this is the hardware labelled "
                                  "relay number that starts at 1, not the Modbus "
                                  "number that starts at 0. 0 means no utility attached."));
    gtk_grid_attach(GTK_GRID(utilities), util4, 3, 4, 2, 1);

    util5 = gtk_spin_button_new_with_range(0, 16, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(util5), 0);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(util5), 0);
    gtk_widget_set_tooltip_text(util5,
                                _("Select the relay number that controls the "
                                  "this utility. Note this is the hardware labelled "
                                  "relay number that starts at 1, not the Modbus "
                                  "number that starts at 0. 0 means no utility attached."));
    gtk_grid_attach(GTK_GRID(utilities), util5, 3, 5, 2, 1);

    util6 = gtk_spin_button_new_with_range(0, 16, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(util6), 0);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(util6), 0);
    gtk_widget_set_tooltip_text(util6,
                                _("Select the relay number that controls the "
                                  "this utility. Note this is the hardware labelled "
                                  "relay number that starts at 1, not the Modbus "
                                  "number that starts at 0. 0 means no utility attached."));
    gtk_grid_attach(GTK_GRID(utilities), util6, 3, 6, 2, 1);

    /* Analog Inputs */
    /*--------------------------------------------------------------------*/
    analogs = gtk_grid_new();
    gtk_container_set_border_width(GTK_CONTAINER(analogs), 5);
    gtk_grid_set_column_spacing(GTK_GRID(analogs), 5);
    gtk_grid_set_row_spacing(GTK_GRID(analogs), 5);
    
    /* Top Labels */

    label = gtk_label_new(_("Analog Input Name"));
    g_object_set(label, "xalign", 1.0, "yalign", 0.5, NULL);
    gtk_grid_attach(GTK_GRID(analogs), label, 0, 0, 3, 1);
    
    label = gtk_label_new(_("Analog Input Num"));
    g_object_set(label, "xalign", 1.0, "yalign", 0.5, NULL);
    gtk_grid_attach(GTK_GRID(analogs), label, 3, 0, 2, 1);

    /* Entries */

    ana1_name = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(ana1_name), 20);
    gtk_entry_set_text(GTK_ENTRY(ana1_name), "");
    gtk_widget_set_tooltip_text(ana1_name,
                                _("Enter the designator for this analog input"));
    gtk_grid_attach(GTK_GRID(analogs), ana1_name, 0, 1, 3, 1); 

    ana2_name = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(ana2_name), 20);
    gtk_entry_set_text(GTK_ENTRY(ana2_name), "");
    gtk_widget_set_tooltip_text(ana2_name,
                                _("Enter the designator for this analog input"));
    gtk_grid_attach(GTK_GRID(analogs), ana2_name, 0, 2, 3, 1); 

    /* Spin Buttons */

    ana1 = gtk_spin_button_new_with_range(0, 16, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(ana1), 0);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(ana1), 0);
    gtk_widget_set_tooltip_text(ana1,
                                _("Select the input number that controls the "
                                  "this analog input. Note this is the hardware labelled "
                                  "input number that starts at 1, not the Modbus "
                                  "number that starts at 0. 0 means no input attached."));
    gtk_grid_attach(GTK_GRID(analogs), ana1, 3, 1, 2, 1);

    ana2 = gtk_spin_button_new_with_range(0, 16, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(ana2), 0);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(ana2), 0);
    gtk_widget_set_tooltip_text(ana2,
                                _("Select the input number that controls the "
                                  "this analog input. Note this is the hardware labelled "
                                  "input number that starts at 1, not the Modbus "
                                  "number that starts at 0. 0 means no input attached."));
    gtk_grid_attach(GTK_GRID(analogs), ana2, 3, 2, 2, 1);

    /* Digital Inputs */
    /*----------------------------------------------------------------*/

    digitals = gtk_grid_new();
    gtk_container_set_border_width(GTK_CONTAINER(digitals), 5);
    gtk_grid_set_column_spacing(GTK_GRID(digitals), 5);
    gtk_grid_set_row_spacing(GTK_GRID(digitals), 5);
    
    /* Top Labels */

    label = gtk_label_new(_("Digital Input Name"));
    g_object_set(label, "xalign", 1.0, "yalign", 0.5, NULL);
    gtk_grid_attach(GTK_GRID(digitals), label, 0, 0, 3, 1);
    
    label = gtk_label_new(_("Digital Input Num"));
    g_object_set(label, "xalign", 1.0, "yalign", 0.5, NULL);
    gtk_grid_attach(GTK_GRID(digitals), label, 3, 0, 2, 1);

    /* Entries */

    dig1_name = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(dig1_name), 20);
    gtk_entry_set_text(GTK_ENTRY(dig1_name), "");
    gtk_widget_set_tooltip_text(dig1_name,
                                _("Enter the designator for this digital input"));
    gtk_grid_attach(GTK_GRID(digitals), dig1_name, 0, 1, 3, 1); 

    dig2_name = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(dig2_name), 20);
    gtk_entry_set_text(GTK_ENTRY(dig2_name), "");
    gtk_widget_set_tooltip_text(dig2_name,
                                _("Enter the designator for this digital input"));
    gtk_grid_attach(GTK_GRID(digitals), dig2_name, 0, 2, 3, 1); 

    dig3_name = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(dig3_name), 20);
    gtk_entry_set_text(GTK_ENTRY(dig3_name), "");
    gtk_widget_set_tooltip_text(dig3_name,
                                _("Enter the designator for this digital input"));
    gtk_grid_attach(GTK_GRID(digitals), dig3_name, 0, 3, 3, 1); 

    /* Spin Buttons */

    dig1 = gtk_spin_button_new_with_range(0, 16, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(dig1), 0);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(dig1), 0);
    gtk_widget_set_tooltip_text(dig1,
                                _("Select the input number that controls the "
                                  "this digital input. Note this is the hardware labelled "
                                  "input number that starts at 1, not the Modbus "
                                  "number that starts at 0. 0 means no input attached."));
    gtk_grid_attach(GTK_GRID(digitals), dig1, 3, 1, 2, 1);

    dig2 = gtk_spin_button_new_with_range(0, 16, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(dig2), 0);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(dig2), 0);
    gtk_widget_set_tooltip_text(dig2,
                                _("Select the input number that controls the "
                                  "this digital input. Note this is the hardware labelled "
                                  "input number that starts at 1, not the Modbus "
                                  "number that starts at 0. 0 means no input attached."));
    gtk_grid_attach(GTK_GRID(digitals), dig2, 3, 2, 2, 1);

    dig3 = gtk_spin_button_new_with_range(0, 16, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(dig3), 0);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(dig3), 0);
    gtk_widget_set_tooltip_text(dig3,
                                _("Select the input number that controls the "
                                  "this digital input. Note this is the hardware labelled "
                                  "input number that starts at 1, not the Modbus "
                                  "number that starts at 0. 0 means no input attached."));
    gtk_grid_attach(GTK_GRID(digitals), dig3, 3, 3, 2, 1);

    /* Config Relays */
    /*--------------------------------------------------------------------------*/

    configs = gtk_grid_new();
    gtk_container_set_border_width(GTK_CONTAINER(configs), 5);
    gtk_grid_set_column_spacing(GTK_GRID(configs), 5);
    gtk_grid_set_row_spacing(GTK_GRID(configs), 5);

    /* Top Labels */

    label = gtk_label_new(_("Config Var Name"));
    g_object_set(label, "xalign", 1.0, "yalign", 0.5, NULL);
    gtk_grid_attach(GTK_GRID(configs), label, 0, 0, 3, 1);
    
    label = gtk_label_new(_("Config Num"));
    g_object_set(label, "xalign", 1.0, "yalign", 0.5, NULL);
    gtk_grid_attach(GTK_GRID(configs), label, 3, 0, 2, 1);

    label = gtk_label_new(_("Config ON State Name"));
    g_object_set(label, "xalign", 1.0, "yalign", 0.5, NULL);
    gtk_grid_attach(GTK_GRID(configs), label, 5, 0, 3, 1);

    label = gtk_label_new(_("Config OFF State Name"));
    g_object_set(label, "xalign", 1.0, "yalign", 0.5, NULL);
    gtk_grid_attach(GTK_GRID(configs), label, 8, 0, 3, 1);

    /* Entries */
    config1_name = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(config1_name), 20);
    gtk_entry_set_text(GTK_ENTRY(config1_name), "");
    gtk_widget_set_tooltip_text(config1_name,
                                _("Enter the designator for this configuration variable"));
    gtk_grid_attach(GTK_GRID(configs), config1_name, 0, 1, 3, 1); 

    config2_name = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(config2_name), 20);
    gtk_entry_set_text(GTK_ENTRY(config2_name), "");
    gtk_widget_set_tooltip_text(config2_name,
                                _("Enter the designator for this configuration variable"));
    gtk_grid_attach(GTK_GRID(configs), config2_name, 0, 2, 3, 1); 

    config3_name = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(config3_name), 20);
    gtk_entry_set_text(GTK_ENTRY(config3_name), "");
    gtk_widget_set_tooltip_text(config3_name,
                                _("Enter the designator for this configuration variable"));
    gtk_grid_attach(GTK_GRID(configs), config3_name, 0, 3, 3, 1); 

    config4_name = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(config4_name), 20);
    gtk_entry_set_text(GTK_ENTRY(config4_name), "");
    gtk_widget_set_tooltip_text(config4_name,
                                _("Enter the designator for this configuration variable"));
    gtk_grid_attach(GTK_GRID(configs), config4_name, 0, 4, 3, 1); 

    config1_on = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(config1_on), 20);
    gtk_entry_set_text(GTK_ENTRY(config1_on), "");
    gtk_widget_set_tooltip_text(config1_on,
                                _("Enter the descriptor for when this config var is ON"));
    gtk_grid_attach(GTK_GRID(configs), config1_on, 5, 1, 3, 1); 

    config2_on = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(config2_on), 20);
    gtk_entry_set_text(GTK_ENTRY(config2_on), "");
    gtk_widget_set_tooltip_text(config2_on,
                                _("Enter the descriptor for when this config var is ON"));
    gtk_grid_attach(GTK_GRID(configs), config2_on, 5, 2, 3, 1); 

    config3_on = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(config3_on), 20);
    gtk_entry_set_text(GTK_ENTRY(config3_on), "");
    gtk_widget_set_tooltip_text(config3_on,
                                _("Enter the descriptor for when this config var is ON"));
    gtk_grid_attach(GTK_GRID(configs), config3_on, 5, 3, 3, 1); 

    config4_on = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(config4_on), 20);
    gtk_entry_set_text(GTK_ENTRY(config4_on), "");
    gtk_widget_set_tooltip_text(config4_on,
                                _("Enter the descriptor for when this config var is ON"));
    gtk_grid_attach(GTK_GRID(configs), config4_on, 5, 4, 3, 1); 

    config1_off = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(config1_off), 20);
    gtk_entry_set_text(GTK_ENTRY(config1_off), "");
    gtk_widget_set_tooltip_text(config1_off,
                                _("Enter the descriptor for when this config var is OFF"));
    gtk_grid_attach(GTK_GRID(configs), config1_off, 8, 1, 3, 1); 

    config2_off = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(config2_off), 20);
    gtk_entry_set_text(GTK_ENTRY(config2_off), "");
    gtk_widget_set_tooltip_text(config2_off,
                                _("Enter the descriptor for when this config var is OFF"));
    gtk_grid_attach(GTK_GRID(configs), config2_off, 8, 2, 3, 1); 

    config3_off = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(config3_off), 20);
    gtk_entry_set_text(GTK_ENTRY(config3_off), "");
    gtk_widget_set_tooltip_text(config3_off,
                                _("Enter the descriptor for when this config var is OFF"));
    gtk_grid_attach(GTK_GRID(configs), config3_off, 8, 3, 3, 1); 

    config4_off = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(config4_off), 20);
    gtk_entry_set_text(GTK_ENTRY(config4_off), "");
    gtk_widget_set_tooltip_text(config4_off,
                                _("Enter the descriptor for when this config var is OFF"));
    gtk_grid_attach(GTK_GRID(configs), config4_off, 8, 4, 3, 1); 

    /* Spin Buttons */

    config1 = gtk_spin_button_new_with_range(0, 16, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(config1), 0);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(config1), 0);
    gtk_widget_set_tooltip_text(config1,
                                _("Select the relay number that controls the "
                                  "this config variable. Note this is the hardware labelled "
                                  "relay number that starts at 1, not the Modbus "
                                  "number that starts at 0. 0 means no config device attached."));
    gtk_grid_attach(GTK_GRID(configs), config1, 3, 1, 2, 1);

    config2 = gtk_spin_button_new_with_range(0, 16, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(config2), 0);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(config2), 0);
    gtk_widget_set_tooltip_text(config2,
                                _("Select the relay number that controls the "
                                  "this config variable. Note this is the hardware labelled "
                                  "relay number that starts at 1, not the Modbus "
                                  "number that starts at 0. 0 means no config device attached."));
    gtk_grid_attach(GTK_GRID(configs), config2, 3, 2, 2, 1);

    config3 = gtk_spin_button_new_with_range(0, 16, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(config3), 0);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(config3), 0);
    gtk_widget_set_tooltip_text(config3,
                                _("Select the relay number that controls the "
                                  "this config variable. Note this is the hardware labelled "
                                  "relay number that starts at 1, not the Modbus "
                                  "number that starts at 0. 0 means no config device attached."));
    gtk_grid_attach(GTK_GRID(configs), config3, 3, 3, 2, 1);

    config4 = gtk_spin_button_new_with_range(0, 16, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(config4), 0);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(config4), 0);
    gtk_widget_set_tooltip_text(config4,
                                _("Select the relay number that controls the "
                                  "this config variable. Note this is the hardware labelled "
                                  "relay number that starts at 1, not the Modbus "
                                  "number that starts at 0. 0 means no config device attached."));
    gtk_grid_attach(GTK_GRID(configs), config4, 3, 4, 2, 1);

    /* Put windows together */
    /*---------------------------------------------------*/

    info_frame = gtk_frame_new(_("Network"));
    gtk_container_add(GTK_CONTAINER(info_frame), info);
    
    utilities_frame = gtk_frame_new(_("Utilities"));
    gtk_container_add(GTK_CONTAINER(utilities_frame), utilities);

    analogs_frame = gtk_frame_new(_("Analog Inputs"));
    gtk_container_add(GTK_CONTAINER(analogs_frame), analogs);
  
    digitals_frame = gtk_frame_new(_("Digital Inputs"));
    gtk_container_add(GTK_CONTAINER(digitals_frame), digitals);

    configs_frame = gtk_frame_new(_("Configuration Variables"));
    gtk_container_add(GTK_CONTAINER(configs_frame), configs);

    table = gtk_grid_new();
    gtk_container_set_border_width(GTK_CONTAINER(table), 5);
    gtk_grid_set_column_spacing(GTK_GRID(table), 5);
    gtk_grid_set_row_spacing(GTK_GRID(table), 5);

    gtk_grid_attach(GTK_GRID(table), info_frame, 0, 0, 2, 1);
    gtk_grid_attach(GTK_GRID(table), utilities_frame, 0, 1, 1, 2);
    gtk_grid_attach(GTK_GRID(table), analogs_frame, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(table), digitals_frame, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(table), configs_frame, 0, 3, 2, 1);

    if (conf->name != NULL)
        update_widgets(conf);

    gtk_widget_show_all(table);

    return table;
}

/* Called when the user clicks the OK button */
static gboolean apply_changes(station_conf_t * conf)
{
    /* name */
    if (conf->name)
        g_free(conf->name);

    conf->name = g_strdup(gtk_entry_get_text(GTK_ENTRY(name)));

    /* host */
    if (conf->host)
        g_free(conf->host);

    conf->host = g_strdup(gtk_entry_get_text(GTK_ENTRY(host)));

    /* port */
    conf->port = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(port));

    /* Utility Names */
    if (conf->util1_name)
        g_free(conf->util1_name);

    conf->util1_name = g_strdup(gtk_entry_get_text(GTK_ENTRY(util1_name)));

    if (conf->util2_name)
        g_free(conf->util2_name);

    conf->util2_name = g_strdup(gtk_entry_get_text(GTK_ENTRY(util2_name)));

    if (conf->util3_name)
        g_free(conf->util3_name);

    conf->util3_name = g_strdup(gtk_entry_get_text(GTK_ENTRY(util3_name)));

    if (conf->util4_name)
        g_free(conf->util4_name);

    conf->util4_name = g_strdup(gtk_entry_get_text(GTK_ENTRY(util4_name)));

    if (conf->util5_name)
        g_free(conf->util5_name);

    conf->util5_name = g_strdup(gtk_entry_get_text(GTK_ENTRY(util5_name)));

    if (conf->util6_name)
        g_free(conf->util6_name);

    conf->util6_name = g_strdup(gtk_entry_get_text(GTK_ENTRY(util6_name)));

    /* Analog Input Names */
    if (conf->ana1_name)
        g_free(conf->ana1_name);

    conf->ana1_name = g_strdup(gtk_entry_get_text(GTK_ENTRY(ana1_name)));

    if (conf->ana2_name)
        g_free(conf->ana2_name);

    conf->ana2_name = g_strdup(gtk_entry_get_text(GTK_ENTRY(ana2_name)));

    /* Digital Input Names */
    if (conf->dig1_name)
        g_free(conf->dig1_name);

    conf->dig1_name = g_strdup(gtk_entry_get_text(GTK_ENTRY(dig1_name)));

    if (conf->dig2_name)
        g_free(conf->dig2_name);

    conf->dig2_name = g_strdup(gtk_entry_get_text(GTK_ENTRY(dig2_name)));

    if (conf->dig3_name)
        g_free(conf->dig3_name);

    conf->dig3_name = g_strdup(gtk_entry_get_text(GTK_ENTRY(dig3_name)));

    /* Config Variable Names + States */
    if (conf->config1_name)
        g_free(conf->config1_name);

    conf->config1_name = g_strdup(gtk_entry_get_text(GTK_ENTRY(config1_name)));

    if (conf->config2_name)
        g_free(conf->config2_name);

    conf->config2_name = g_strdup(gtk_entry_get_text(GTK_ENTRY(config2_name)));

    if (conf->config3_name)
        g_free(conf->config3_name);

    conf->config3_name = g_strdup(gtk_entry_get_text(GTK_ENTRY(config3_name)));

    if (conf->config4_name)
        g_free(conf->config4_name);

    conf->config4_name = g_strdup(gtk_entry_get_text(GTK_ENTRY(config4_name)));

    if (conf->config1_on)
        g_free(conf->config1_on);

    conf->config1_on = g_strdup(gtk_entry_get_text(GTK_ENTRY(config1_on)));

    if (conf->config2_on)
        g_free(conf->config2_on);

    conf->config2_on = g_strdup(gtk_entry_get_text(GTK_ENTRY(config2_on)));

    if (conf->config3_on)
        g_free(conf->config3_on);

    conf->config3_on = g_strdup(gtk_entry_get_text(GTK_ENTRY(config3_on)));

    if (conf->config4_on)
        g_free(conf->config4_on);

    conf->config4_on = g_strdup(gtk_entry_get_text(GTK_ENTRY(config4_on)));

    if (conf->config1_off)
        g_free(conf->config1_off);

    conf->config1_off = g_strdup(gtk_entry_get_text(GTK_ENTRY(config1_off)));

    if (conf->config2_off)
        g_free(conf->config2_off);

    conf->config2_off = g_strdup(gtk_entry_get_text(GTK_ENTRY(config2_off)));

    if (conf->config3_off)
        g_free(conf->config3_off);

    conf->config3_off = g_strdup(gtk_entry_get_text(GTK_ENTRY(config3_off)));

    if (conf->config4_off)
        g_free(conf->config4_off);

    conf->config4_off = g_strdup(gtk_entry_get_text(GTK_ENTRY(config4_off)));

    /* Utility Relays */
    conf->util1 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(util1));
    conf->util2 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(util2));
    conf->util3 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(util3));
    conf->util4 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(util4));
    conf->util5 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(util5));
    conf->util6 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(util6));

    /* Analog Inputs */
    conf->ana1 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(ana1));
    conf->ana2 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(ana2));

    /* Digital Inputs */
    conf->dig1 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(dig1));
    conf->dig2 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(dig2));
    conf->dig3 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(dig3));

    /* Config Relays */
    conf->config1 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(config1));
    conf->config2 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(config2));
    conf->config3 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(config3));
    conf->config4 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(config4));


    return TRUE;
}

/**
 * Add or edit a station configuration.
 *
 * @param conf Pointer to a station configuration.
 *
 * If conf->name is not NULL the widgets will be populated with the data.
 */
void sat_pref_stn_editor_run(station_conf_t * conf)
{
    gint            response;
    gboolean        finished = FALSE;

    /* crate dialog and add contents */
    dialog = gtk_dialog_new_with_buttons(_("Edit station configuration"),
                                         GTK_WINDOW(window),
                                         GTK_DIALOG_MODAL |
                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                         "_Clear", GTK_RESPONSE_REJECT,
                                         "_Cancel", GTK_RESPONSE_CANCEL,
                                         "_Ok", GTK_RESPONSE_OK,
                                         NULL);

    /* disable OK button to begin with */
    gtk_dialog_set_response_sensitive(GTK_DIALOG(dialog),
                                      GTK_RESPONSE_OK, FALSE);

    gtk_container_add(GTK_CONTAINER
                      (gtk_dialog_get_content_area(GTK_DIALOG(dialog))),
                      create_editor_widgets(conf));

    /* this hacky-thing is to keep the dialog running in case the
       CLEAR button is plressed. OK and CANCEL will exit the loop
     */
    while (!finished)
    {
        response = gtk_dialog_run(GTK_DIALOG(dialog));

        switch (response)
        {
        case GTK_RESPONSE_OK:
            if (apply_changes(conf))
                finished = TRUE;
            else
                finished = FALSE;
            break;

        case GTK_RESPONSE_REJECT:
            /* CLEAR */
            clear_widgets();
            break;

        default:
            /* Everything else is considered CANCEL */
            finished = TRUE;
            break;
        }
    }

    gtk_widget_destroy(dialog);
}
