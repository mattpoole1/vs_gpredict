
#ifdef HAVE_CONFIG_H
#include <build-config.h>
#endif

#include <errno.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <math.h>
#include <string.h>   

/* NETWORK */
#ifndef WIN32
#include <arpa/inet.h>          /* htons() */
#include <netdb.h>              /* gethostbyname() */
#include <netinet/in.h>         /* struct sockaddr_in */
#include <sys/socket.h>         /* socket(), connect(), send() */
#else
#include <winsock2.h>
#endif

         
#include "gtk-stn-ctrl.h"
#include "sat-log.h"

#define DELAY 3000

/* Stnctld commands */

#define WRITE_COIL "s"
#define READ_ANALOG "m"
#define READ_DIGITAL "r"
#define READ_COIL "g"


static GtkVBoxClass *parent_class = NULL;

/* Open the stncld socket. Returns file descriptor or -1 if an error occurs */
static gint stnctld_socket_open(const gchar * host, gint port)
{
    struct sockaddr_in ServAddr;
    struct hostent *h;
    gint            sock;
    gint            status;

    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == -1)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("Failed to create stnctl socket: %s"), strerror(errno));
        return sock;
    }

    sat_log_log(SAT_LOG_LEVEL_DEBUG,
                _("%s: Network socket created successfully"), __func__);

    memset(&ServAddr, 0, sizeof(ServAddr));
    ServAddr.sin_family = AF_INET;      /* Internet address family */
    h = gethostbyname(host);
    if (h == NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("Name resolution of stnctld server %s failed."), host);
#ifdef WIN32
        closesocket(sock);
#else
        close(sock);
#endif
        return -1;
    }

    memcpy((char *)&ServAddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
    ServAddr.sin_port = htons(port);    /* Server port */

    /* establish connection */
    status = connect(sock, (struct sockaddr *)&ServAddr, sizeof(ServAddr));
    if (status == -1)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("Connection to stnctld server at %s:%d failed: %s"),
                    host, port, strerror(errno));

#ifdef WIN32
        closesocket(sock);
#else
        close(sock);
#endif
        return -1;
    }

    sat_log_log(SAT_LOG_LEVEL_DEBUG, _("%s: Connection opened to %s:%d"),
                __func__, host, port);

    return sock;
}

/* Close a stncld socket. First send a q command to cleanly shut down stnctld */
static void stnctld_socket_close(gint * sock)
{
    gint            written;

    /*shutdown the stnctld connect */
    written = send(*sock, "q\x0a", 2, 0);
    if (written != 2)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s:%s: Sent 2 bytes but sent %d."),
                    __FILE__, __func__, written);
    }

#ifndef WIN32
    shutdown(*sock, SHUT_RDWR);
    close(*sock);
#else
    shutdown(*sock, SD_BOTH);
    closesocket(*sock);
#endif

    *sock = -1;
}

/*
 * Send a command to stnctld and read the response.
 *
 * Inputs are the socket, a string command, and a buffer and length for
 * returning the output from stnctld.
 */
static gboolean stnctld_socket_rw(gint sock, gchar * buff, gchar * buffout,
                                  gint sizeout)
{
    gint            written;
    gint            size;

#ifdef WIN32
    size = strlen(buff) - 1;
#else
    size = strlen(buff);
#endif

    /* send command */
    written = send(sock, buff, size, 0);
    if (written != size)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: SIZE ERROR %d / %d"), __func__, written, size);
    }
    if (written == -1)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: stnctld Socket Down"), __func__);
        return FALSE;
    }

    /* try to read answer */
    size = recv(sock, buffout, sizeout, 0);

    if (size == -1)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: stnctld Socket Down"), __func__);
        return FALSE;
    }

    buffout[size] = '\0';
    if (size == 0)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s:%s: Got 0 bytes from stnctld"), __FILE__, __func__);
    }



    return TRUE;
}

static gboolean analog_read(GtkStnCtrl * ctrl, gint a_num, gdouble * ana)
{
    gchar          *buff, **vbuff;
    gchar           buffback[128];
    gboolean        retcode;

    if (ana == NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s:%d: NULL storage."), __FILE__, __LINE__);
        return FALSE;
    }

    /* send command */
    buff = g_strdup_printf("m%d\x0a", a_num);
    retcode = stnctld_socket_rw(ctrl->client.socket, buff, buffback, 128);
    
    /* try to parse answer */
    if (retcode)
    {
        if (strncmp(buffback, "RPRT", 4) == 0)
        {
            g_strstrip(buffback);
            sat_log_log(SAT_LOG_LEVEL_ERROR,
                        _("%s:%d: stnctld returned error (%s)"),
                        __FILE__, __LINE__, buffback);
        }
        else
        {
            vbuff = g_strsplit(buffback, "\n", 2);
            if (vbuff[0] != NULL)
            {
                *ana = g_strtod(vbuff[0], NULL);

            }
            else
            {
                g_strstrip(buffback);
                sat_log_log(SAT_LOG_LEVEL_ERROR,
                            _("%s:%d: stnctld returned bad response (%s)"),
                            __FILE__, __LINE__, vbuff[0]);
            }

        } 
        g_strfreev(vbuff);
    } else {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Read/write failed, retcode = %i"),
                    __func__, retcode);     
    }

    g_free(buff);

    return retcode;
}

static gboolean digital_read(GtkStnCtrl * ctrl, gint d_num, gboolean * dig)
{
    gchar          *buff;
    gchar           buffback[128];

    gboolean        retcode;
    gboolean        bad_resp = FALSE;

    if (dig == NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s:%d: NULL storage."), __FILE__, __LINE__);
        return FALSE;
    }

    /* send command */
    buff = g_strdup_printf("r%d\x0a", d_num);
    retcode = stnctld_socket_rw(ctrl->client.socket, buff, buffback, 128);
    
    /* try to parse answer */
    if (retcode)
    {
        if (strncmp(buffback, "RPRT", 4) == 0)
        {
            g_strstrip(buffback);
            sat_log_log(SAT_LOG_LEVEL_ERROR,
                        _("%s:%d: stnctld returned error (%s)"),
                        __FILE__, __LINE__, buffback);
        }
        else
        {

            if (buffback != NULL)
            {
                if (!strncmp(buffback, "0", 1)) {
                    *dig = FALSE;
                } else if (!strncmp(buffback, "1", 1)) {
                    *dig = TRUE;
                } else {
                    bad_resp = TRUE;
                }
            }
            else {
                bad_resp = TRUE;
            }
            if (bad_resp)
            {
                g_strstrip(buffback);
                sat_log_log(SAT_LOG_LEVEL_ERROR,
                            _("%s:%d: stnctld returned bad response (%s)"),
                            __FILE__, __LINE__, buffback);
                retcode = FALSE;
            }

        }
    }

    g_free(buff);

    return retcode;
}

static gboolean coil_read(GtkStnCtrl * ctrl, gint c_num)
{
    gchar          *buff;
    gchar           buffback[128];

    gboolean        state;
    gboolean        retcode;
    gboolean        bad_resp = FALSE;

    /* send command */
    buff = g_strdup_printf("g%d\x0a", c_num);
    retcode = stnctld_socket_rw(ctrl->client.socket, buff, buffback, 128);
    
    /* try to parse answer */
    if (retcode)
    {
        if (strncmp(buffback, "RPRT", 4) == 0)
        {
            g_strstrip(buffback);
            sat_log_log(SAT_LOG_LEVEL_ERROR,
                        _("%s:%d: stnctld returned error (%s)"),
                        __FILE__, __LINE__, buffback);
        }
        else
        {

            if (buffback != NULL)
            {
                if (!strncmp(buffback, "0", 1)) {
                    state = FALSE;
                } else if (!strncmp(buffback, "1", 1)) {
                    state = TRUE;
                } else {
                    bad_resp = TRUE;
                }
            }
            else {
                bad_resp = TRUE;
            }
            if (bad_resp)
            {
                g_strstrip(buffback);
                sat_log_log(SAT_LOG_LEVEL_ERROR,
                            _("%s:%d: stnctld returned bad response (%s)"),
                            __FILE__, __LINE__, buffback);
                retcode = FALSE;
            }

        }
    }
    state = (((state) && (retcode)) && (!bad_resp)); 
    g_free(buff);

    return state;
}


static gboolean get_readings(GtkStnCtrl * ctrl, gdouble * a1, gdouble * a2,
                               gboolean * dig1_s, gboolean * dig2_s, gboolean * dig3_s)
{

    gboolean retcode = TRUE;
    retcode = retcode && analog_read(ctrl, ctrl->conf->ana1, a1);
    retcode = retcode && analog_read(ctrl, ctrl->conf->ana2, a2);
    retcode = retcode && digital_read(ctrl, ctrl->conf->dig1, dig1_s);
    retcode = retcode && digital_read(ctrl, ctrl->conf->dig2, dig2_s);
    retcode = retcode && digital_read(ctrl, ctrl->conf->dig3, dig3_s);

    return retcode;    

}


/* Stnctl client thread */
static gpointer stnctld_client_thread(gpointer data)
{
    gdouble         elapsed_time;
    gdouble         a1s = 0.0;
    gdouble         a2s = 0.0;
    gboolean        dig1_s = FALSE;
    gboolean        dig2_s = FALSE;
    gboolean        dig3_s = FALSE;
    gboolean        new_trg = FALSE;
    gboolean        io_error = FALSE;
    GtkStnCtrl     *ctrl = GTK_STN_CTRL(data);
  

    sat_log_log(SAT_LOG_LEVEL_ERROR,
                           _("Starting stnctld client thread"));

    ctrl->client.socket = stnctld_socket_open(ctrl->conf->host,
                                              ctrl->conf->port);

    sat_log_log(SAT_LOG_LEVEL_ERROR,
                _("Socket opened at %s:%d"), ctrl->conf->host, ctrl->conf->port);

    if (ctrl->client.socket == -1)
        return GINT_TO_POINTER(-1);

    ctrl->client.timer = g_timer_new();
    ctrl->client.new_trg = FALSE;
    ctrl->client.running = TRUE;

    while (ctrl->client.running)
    {
        g_timer_start(ctrl->client.timer);
        io_error = FALSE;
        g_mutex_lock(&ctrl->client.mutex);

        if (!get_readings(ctrl, &a1s, &a2s, &dig1_s, &dig2_s, &dig3_s))
        {
            ctrl->client.io_error = TRUE; 
        } else {
            ctrl->client.io_error = FALSE;
        }


        ctrl->client.ana1_s = a1s;
        ctrl->client.ana2_s = a2s;
        ctrl->client.dig1_s = dig1_s;
        ctrl->client.dig2_s = dig2_s;
        ctrl->client.dig3_s = dig3_s;
        ctrl->client.new_trg = new_trg;

        sat_log_log(SAT_LOG_LEVEL_ERROR ,_("%s: Ana 1 val is now %f"), __func__, ctrl->client.ana1_s);
        a1s = 0.0;
        a2s = 0.0;

        g_mutex_unlock(&ctrl->client.mutex);

        /* ensure stnctl duty cycle stays below 50%, but wait at least 700 ms (TBC) */
        elapsed_time = MAX(g_timer_elapsed(ctrl->client.timer, NULL), 0.7);
        g_usleep(elapsed_time * 1e6);
    }

    sat_log_log(SAT_LOG_LEVEL_ERROR ,_("Stopping stnctld client thread\n"));
    g_timer_destroy(ctrl->client.timer);
    stnctld_socket_close(&ctrl->client.socket);

    return GINT_TO_POINTER(0);
}
static gint stn_name_compare(const gchar * a, const gchar * b)
{
    return (gpredict_strcmp(a, b));
}

static gboolean stn_ctrl_timeout_cb(gpointer data)
{

    GtkStnCtrl   *ctrl = GTK_STN_CTRL(data);
    gchar       *buff;
    gchar       *text;
    gdouble      ana1;
    gdouble      ana2;
    gboolean     dig1;
    gboolean     dig2;
    gboolean     dig3;
    gboolean     error = TRUE;
    gboolean     retcode = TRUE;
    
    if (ctrl->connected) 
    {
        if (g_mutex_trylock(&ctrl->client.mutex))
        {
            error = ctrl->client.io_error;
            ana1 = ctrl->client.ana1_s;
            ana2 = ctrl->client.ana2_s;
            dig1 = ctrl->client.dig1_s;
            dig2 = ctrl->client.dig2_s;
            dig3 = ctrl->client.dig3_s;
            g_mutex_unlock(&ctrl->client.mutex);

            if (error)
            {
                gtk_label_set_text(GTK_LABEL(ctrl->ana1), _("ERROR"));
                gtk_label_set_text(GTK_LABEL(ctrl->ana2), _("ERROR"));
                gtk_label_set_text(GTK_LABEL(ctrl->dig1), _("ERROR"));
                gtk_label_set_text(GTK_LABEL(ctrl->dig2), _("ERROR"));
                gtk_label_set_text(GTK_LABEL(ctrl->dig3), _("ERROR"));

            }
            else
            {
                /* update display widgets */
                text = g_strdup_printf("%.2f\302\260", ana1);
                gtk_label_set_text(GTK_LABEL(ctrl->ana1), text);
                g_free(text);
                text = g_strdup_printf("%.2f\302\260", ana2);
                gtk_label_set_text(GTK_LABEL(ctrl->ana2), text);
                g_free(text);

                if (dig1) {
                    text = "ON";
                } else {
                    text = "OFF";
                }
                gtk_label_set_text(GTK_LABEL(ctrl->dig1), _(text));
                g_free(text);

                if (dig2) {
                    text = "ON";
                } else {
                    text = "OFF";
                }
                gtk_label_set_text(GTK_LABEL(ctrl->dig2), _(text));
                g_free(text);

                if (dig3) {
                    text = "ON";
                } else {
                    text = "OFF";
                }
                gtk_label_set_text(GTK_LABEL(ctrl->dig3), _(text));
                g_free(text);
                
            }
        } else {
            retcode = FALSE;
        }
    } 
    return retcode;

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

    if (!gtk_toggle_button_get_active(button))
    {
        ctrl->connected = FALSE;
        //deactivate_relays();
        //clear_readings();
        //disable_relays();
        if (!ctrl->client.running) {
            return; }
        ctrl->client.running = FALSE;
        g_thread_join(ctrl->client.thread); 
    } else {

        if (ctrl->conf == NULL)
        {
            /* we don't have a working configuration */
            sat_log_log(SAT_LOG_LEVEL_ERROR,
                        _
                        ("%s: Controller does not have a valid configuration"),
                        __func__);
            return;
        }
        ctrl->client.thread =
            g_thread_new("gpredict_stnctl", stnctld_client_thread, ctrl);
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                _("%s: Client thread begun"), __func__);
        ctrl->connected = TRUE;
        //update_relay_states();
        //enable_all_relays();
    }

    sat_log_log(SAT_LOG_LEVEL_DEBUG,
                _("%s: Connect Button Clicked"), __func__);

}

/**
 * Manage toggle signals (Config1On)
 *
 * \param button Pointer to the GtkToggle button.
 * \param data Pointer to the GtkStnCtrl widget.
 */
static void config1On_toggle_cb(GtkToggleButton * button, gpointer data)
{
    GtkStnCtrl    *ctrl = GTK_STN_CTRL(data);
    gchar          *buff;
    gchar           buffback[128];
    gboolean        retcode;
    gint            retval;

    if (gtk_toggle_button_get_active(button))
    {
        
        /* send command */
        buff = g_strdup_printf("s%d %d\x0a", ctrl->conf->config1, 1);
        retcode = stnctld_socket_rw(ctrl->client.socket, buff, buffback, 128);
        g_free(buff);
        if (retcode) {
            retval = (gint) g_strtod(buffback + 4, NULL);
            if (retval == 0) {

                /* Only switch if successful */
                gtk_toggle_button_set_active(ctrl->config1Off, FALSE);
                gtk_widget_set_sensitive(button, FALSE);
                gtk_widget_set_sensitive(ctrl->config1Off, TRUE);
            } else {
                g_strstrip(buffback);
                sat_log_log(SAT_LOG_LEVEL_ERROR,_
                        ("%s:%d: stnctld returned error %s with %s"),
                        __FILE__, __LINE__, retval, buffback);
            }
        }
    }

    sat_log_log(SAT_LOG_LEVEL_DEBUG,
                _("%s: Config 1 On Button Clicked"), __func__);

}

/**
 * Manage toggle signals (Config1Off)
 *
 * \param button Pointer to the GtkToggle button.
 * \param data Pointer to the GtkStnCtrl widget.
 */
static void config1Off_toggle_cb(GtkToggleButton * button, gpointer data)
{
    GtkStnCtrl    *ctrl = GTK_STN_CTRL(data);
    gchar          *buff;
    gchar           buffback[128];
    gboolean        retcode;
    gint            retval;

    if (gtk_toggle_button_get_active(button))
    {
        
        /* send command */
        buff = g_strdup_printf("s%d %d\x0a", ctrl->conf->config1, 0);
        retcode = stnctld_socket_rw(ctrl->client.socket, buff, buffback, 128);
        g_free(buff);
        if (retcode) {
            retval = (gint) g_strtod(buffback + 4, NULL);
            if (retval == 0) {

                /* Only switch if successful */
                gtk_toggle_button_set_active(ctrl->config1On, FALSE);
                gtk_widget_set_sensitive(button, FALSE);
                gtk_widget_set_sensitive(ctrl->config1On, TRUE);
                
            } else {
                g_strstrip(buffback);
                sat_log_log(SAT_LOG_LEVEL_ERROR,_
                        ("%s:%d: stnctld returned error %s with %s"),
                        __FILE__, __LINE__, retval, buffback);
            }
        }
    }

    sat_log_log(SAT_LOG_LEVEL_DEBUG,
                _("%s: Config 1 Off Button Clicked"), __func__);

}

/**
 * Manage toggle signals (Config2On)
 *
 * \param button Pointer to the GtkToggle button.
 * \param data Pointer to the GtkStnCtrl widget.
 */
static void config2On_toggle_cb(GtkToggleButton * button, gpointer data)
{
    GtkStnCtrl    *ctrl = GTK_STN_CTRL(data);
    gchar          *buff;
    gchar           buffback[128];
    gboolean        retcode;
    gint            retval;

    if (gtk_toggle_button_get_active(button))
    {
        
        /* send command */
        buff = g_strdup_printf("s%d %d\x0a", ctrl->conf->config2, 1);
        retcode = stnctld_socket_rw(ctrl->client.socket, buff, buffback, 128);
        g_free(buff);
        if (retcode) {
            retval = (gint) g_strtod(buffback + 4, NULL);
            if (retval == 0) {

                /* Only switch if successful */
                gtk_toggle_button_set_active(ctrl->config2Off, FALSE);
                gtk_widget_set_sensitive(button, FALSE);
                gtk_widget_set_sensitive(ctrl->config2Off, TRUE);
            } else {
                g_strstrip(buffback);
                sat_log_log(SAT_LOG_LEVEL_ERROR,_
                        ("%s:%d: stnctld returned error %s with %s"),
                        __FILE__, __LINE__, retval, buffback);
            }
        }
    }

    sat_log_log(SAT_LOG_LEVEL_DEBUG,
                _("%s: Config 2 On Button Clicked"), __func__);

}

/**
 * Manage toggle signals (Config2Off)
 *
 * \param button Pointer to the GtkToggle button.
 * \param data Pointer to the GtkStnCtrl widget.
 */
static void config2Off_toggle_cb(GtkToggleButton * button, gpointer data)
{
    GtkStnCtrl    *ctrl = GTK_STN_CTRL(data);
    gchar          *buff;
    gchar           buffback[128];
    gboolean        retcode;
    gint            retval;

    if (gtk_toggle_button_get_active(button))
    {
        
        /* send command */
        buff = g_strdup_printf("s%d %d\x0a", ctrl->conf->config2, 0);
        retcode = stnctld_socket_rw(ctrl->client.socket, buff, buffback, 128);
        g_free(buff);
        if (retcode) {
            retval = (gint) g_strtod(buffback + 4, NULL);
            if (retval == 0) {

                /* Only switch if successful */
                gtk_toggle_button_set_active(ctrl->config2On, FALSE);
                gtk_widget_set_sensitive(button, FALSE);
                gtk_widget_set_sensitive(ctrl->config2On, TRUE);
                
            } else {
                g_strstrip(buffback);
                sat_log_log(SAT_LOG_LEVEL_ERROR,_
                        ("%s:%d: stnctld returned error %s with %s"),
                        __FILE__, __LINE__, retval, buffback);
            }
        }
    }

    sat_log_log(SAT_LOG_LEVEL_DEBUG,
                _("%s: Config 2 Off Button Clicked"), __func__);

}

/**
 * Manage toggle signals (Config3On)
 *
 * \param button Pointer to the GtkToggle button.
 * \param data Pointer to the GtkStnCtrl widget.
 */
static void config3On_toggle_cb(GtkToggleButton * button, gpointer data)
{
    GtkStnCtrl    *ctrl = GTK_STN_CTRL(data);
    gchar          *buff;
    gchar           buffback[128];
    gboolean        retcode;
    gint            retval;

    if (gtk_toggle_button_get_active(button))
    {
        
        /* send command */
        buff = g_strdup_printf("s%d %d\x0a", ctrl->conf->config3, 1);
        retcode = stnctld_socket_rw(ctrl->client.socket, buff, buffback, 128);
        g_free(buff);
        if (retcode) {
            retval = (gint) g_strtod(buffback + 4, NULL);
            if (retval == 0) {

                /* Only switch if successful */
                gtk_toggle_button_set_active(ctrl->config3Off, FALSE);
                gtk_widget_set_sensitive(button, FALSE);
                gtk_widget_set_sensitive(ctrl->config3Off, TRUE);
            } else {
                g_strstrip(buffback);
                sat_log_log(SAT_LOG_LEVEL_ERROR,_
                        ("%s:%d: stnctld returned error %s with %s"),
                        __FILE__, __LINE__, retval, buffback);
            }
        }
    }

    sat_log_log(SAT_LOG_LEVEL_DEBUG,
                _("%s: Config 3 On Button Clicked"), __func__);

}

/**
 * Manage toggle signals (Config3Off)
 *
 * \param button Pointer to the GtkToggle button.
 * \param data Pointer to the GtkStnCtrl widget.
 */
static void config3Off_toggle_cb(GtkToggleButton * button, gpointer data)
{
    GtkStnCtrl    *ctrl = GTK_STN_CTRL(data);
    gchar          *buff;
    gchar           buffback[128];
    gboolean        retcode;
    gint            retval;

    if (gtk_toggle_button_get_active(button))
    {
        
        /* send command */
        buff = g_strdup_printf("s%d %d\x0a", ctrl->conf->config3, 0);
        retcode = stnctld_socket_rw(ctrl->client.socket, buff, buffback, 128);
        g_free(buff);
        if (retcode) {
            retval = (gint) g_strtod(buffback + 4, NULL);
            if (retval == 0) {

                /* Only switch if successful */
                gtk_toggle_button_set_active(ctrl->config3On, FALSE);
                gtk_widget_set_sensitive(button, FALSE);
                gtk_widget_set_sensitive(ctrl->config3On, TRUE);
                
            } else {
                g_strstrip(buffback);
                sat_log_log(SAT_LOG_LEVEL_ERROR,_
                        ("%s:%d: stnctld returned error %s with %s"),
                        __FILE__, __LINE__, retval, buffback);
            }
        }
    }

    sat_log_log(SAT_LOG_LEVEL_DEBUG,
                _("%s: Config 3 Off Button Clicked"), __func__);

}

/**
 * Manage toggle signals (Config4On)
 *
 * \param button Pointer to the GtkToggle button.
 * \param data Pointer to the GtkStnCtrl widget.
 */
static void config4On_toggle_cb(GtkToggleButton * button, gpointer data)
{
    GtkStnCtrl    *ctrl = GTK_STN_CTRL(data);
    gchar          *buff;
    gchar           buffback[128];
    gboolean        retcode;
    gint            retval;

    if (gtk_toggle_button_get_active(button))
    {
        
        /* send command */
        buff = g_strdup_printf("s%d %d\x0a", ctrl->conf->config4, 1);
        retcode = stnctld_socket_rw(ctrl->client.socket, buff, buffback, 128);
        g_free(buff);
        if (retcode) {
            retval = (gint) g_strtod(buffback + 4, NULL);
            if (retval == 0) {

                /* Only switch if successful */
                gtk_toggle_button_set_active(ctrl->config4Off, FALSE);
                gtk_widget_set_sensitive(button, FALSE);
                gtk_widget_set_sensitive(ctrl->config4Off, TRUE);
            } else {
                g_strstrip(buffback);
                sat_log_log(SAT_LOG_LEVEL_ERROR,_
                        ("%s:%d: stnctld returned error %s with %s"),
                        __FILE__, __LINE__, retval, buffback);
            }
        }
    }

    sat_log_log(SAT_LOG_LEVEL_DEBUG,
                _("%s: Config 4 On Button Clicked"), __func__);

}

/**
 * Manage toggle signals (Config4Off)
 *
 * \param button Pointer to the GtkToggle button.
 * \param data Pointer to the GtkStnCtrl widget.
 */
static void config4Off_toggle_cb(GtkToggleButton * button, gpointer data)
{
    GtkStnCtrl    *ctrl = GTK_STN_CTRL(data);
    gchar          *buff;
    gchar           buffback[128];
    gboolean        retcode;
    gint            retval;

    if (gtk_toggle_button_get_active(button))
    {
        
        /* send command */
        buff = g_strdup_printf("s%d %d\x0a", ctrl->conf->config4, 0);
        retcode = stnctld_socket_rw(ctrl->client.socket, buff, buffback, 128);
        g_free(buff);
        if (retcode) {
            retval = (gint) g_strtod(buffback + 4, NULL);
            if (retval == 0) {

                /* Only switch if successful */
                gtk_toggle_button_set_active(ctrl->config4On, FALSE);
                gtk_widget_set_sensitive(button, FALSE);
                gtk_widget_set_sensitive(ctrl->config4On, TRUE);
                
            } else {
                g_strstrip(buffback);
                sat_log_log(SAT_LOG_LEVEL_ERROR,_
                        ("%s:%d: stnctld returned error %s with %s"),
                        __FILE__, __LINE__, retval, buffback);
            }
        }
    }

    sat_log_log(SAT_LOG_LEVEL_DEBUG,
                _("%s: Config 4 Off Button Clicked"), __func__);
}

/**
 * Manage toggle signals (Util1)
 *
 * \param button Pointer to the GtkToggle button.
 * \param data Pointer to the GtkStnCtrl widget.
 */
static void util1_toggle_cb(GtkToggleButton * button, gpointer data)
{
    GtkStnCtrl    *ctrl = GTK_STN_CTRL(data);
    gchar          *buff;
    gchar           buffback[128];
    gboolean        retcode;
    gint            retval;

    if (gtk_toggle_button_get_active(button))
    {
        
        /* send command */
        buff = g_strdup_printf("s%d %d\x0a", ctrl->conf->util1, 1);
        retcode = stnctld_socket_rw(ctrl->client.socket, buff, buffback, 128);
        g_free(buff);

    } else {
        /* send command */
        buff = g_strdup_printf("s%d %d\x0a", ctrl->conf->util1, 0);
        retcode = stnctld_socket_rw(ctrl->client.socket, buff, buffback, 128);
        g_free(buff);


    sat_log_log(SAT_LOG_LEVEL_DEBUG,
                _("%s: Util1 Button Toggled"), __func__);
    }
    if (retcode) {
        retval = (gint) g_strtod(buffback + 4, NULL);
        if (retval != 0) {
            g_strstrip(buffback);
            sat_log_log(SAT_LOG_LEVEL_ERROR,_
                    ("%s:%d: stnctld returned error %s with %s"),
                    __FILE__, __LINE__, retval, buffback);
        }
    }

}

/**
 * Manage toggle signals (Util2)
 *
 * \param button Pointer to the GtkToggle button.
 * \param data Pointer to the GtkStnCtrl widget.
 */
static void util2_toggle_cb(GtkToggleButton * button, gpointer data)
{
    GtkStnCtrl    *ctrl = GTK_STN_CTRL(data);
    gchar          *buff;
    gchar           buffback[128];
    gboolean        retcode;
    gint            retval;

    if (gtk_toggle_button_get_active(button))
    {
        
        /* send command */
        buff = g_strdup_printf("s%d %d\x0a", ctrl->conf->util2, 1);
        retcode = stnctld_socket_rw(ctrl->client.socket, buff, buffback, 128);
        g_free(buff);

    } else {
        /* send command */
        buff = g_strdup_printf("s%d %d\x0a", ctrl->conf->util2, 0);
        retcode = stnctld_socket_rw(ctrl->client.socket, buff, buffback, 128);
        g_free(buff);


    sat_log_log(SAT_LOG_LEVEL_DEBUG,
                _("%s: Util2 Button Toggled"), __func__);
    }
    if (retcode) {
        retval = (gint) g_strtod(buffback + 4, NULL);
        if (retval != 0) {
            g_strstrip(buffback);
            sat_log_log(SAT_LOG_LEVEL_ERROR,_
                    ("%s:%d: stnctld returned error %s with %s"),
                    __FILE__, __LINE__, retval, buffback);
        }
    }



}

/**
 * Manage toggle signals (Util3)
 *
 * \param button Pointer to the GtkToggle button.
 * \param data Pointer to the GtkStnCtrl widget.
 */
static void util3_toggle_cb(GtkToggleButton * button, gpointer data)
{
    GtkStnCtrl    *ctrl = GTK_STN_CTRL(data);
    gchar          *buff;
    gchar           buffback[128];
    gboolean        retcode;
    gint            retval;

    if (gtk_toggle_button_get_active(button))
    {
        
        /* send command */
        buff = g_strdup_printf("s%d %d\x0a", ctrl->conf->util3, 1);
        retcode = stnctld_socket_rw(ctrl->client.socket, buff, buffback, 128);
        g_free(buff);

    } else {
        /* send command */
        buff = g_strdup_printf("s%d %d\x0a", ctrl->conf->util3, 0);
        retcode = stnctld_socket_rw(ctrl->client.socket, buff, buffback, 128);
        g_free(buff);


    sat_log_log(SAT_LOG_LEVEL_DEBUG,
                _("%s: Util3 Button Toggled"), __func__);
    }
    if (retcode) {
        retval = (gint) g_strtod(buffback + 4, NULL);
        if (retval != 0) {
            g_strstrip(buffback);
            sat_log_log(SAT_LOG_LEVEL_ERROR,_
                    ("%s:%d: stnctld returned error %s with %s"),
                    __FILE__, __LINE__, retval, buffback);
        }
    }



}

/**
 * Manage toggle signals (Util4)
 *
 * \param button Pointer to the GtkToggle button.
 * \param data Pointer to the GtkStnCtrl widget.
 */
static void util4_toggle_cb(GtkToggleButton * button, gpointer data)
{
    GtkStnCtrl    *ctrl = GTK_STN_CTRL(data);
    gchar          *buff;
    gchar           buffback[128];
    gboolean        retcode;
    gint            retval;

    if (gtk_toggle_button_get_active(button))
    {
        
        /* send command */
        buff = g_strdup_printf("s%d %d\x0a", ctrl->conf->util4, 1);
        retcode = stnctld_socket_rw(ctrl->client.socket, buff, buffback, 128);
        g_free(buff);

    } else {
        /* send command */
        buff = g_strdup_printf("s%d %d\x0a", ctrl->conf->util4, 0);
        retcode = stnctld_socket_rw(ctrl->client.socket, buff, buffback, 128);
        g_free(buff);


    sat_log_log(SAT_LOG_LEVEL_DEBUG,
                _("%s: Util4 Button Toggled"), __func__);
    }
    if (retcode) {
        retval = (gint) g_strtod(buffback + 4, NULL);
        if (retval != 0) {
            g_strstrip(buffback);
            sat_log_log(SAT_LOG_LEVEL_ERROR,_
                    ("%s:%d: stnctld returned error %s with %s"),
                    __FILE__, __LINE__, retval, buffback);
        }
    }



}

/**
 * Manage toggle signals (Util5)
 *
 * \param button Pointer to the GtkToggle button.
 * \param data Pointer to the GtkStnCtrl widget.
 */
static void util5_toggle_cb(GtkToggleButton * button, gpointer data)
{
    GtkStnCtrl    *ctrl = GTK_STN_CTRL(data);
    gchar          *buff;
    gchar           buffback[128];
    gboolean        retcode;
    gint            retval;

    if (gtk_toggle_button_get_active(button))
    {
        
        /* send command */
        buff = g_strdup_printf("s%d %d\x0a", ctrl->conf->util5, 1);
        retcode = stnctld_socket_rw(ctrl->client.socket, buff, buffback, 128);
        g_free(buff);

    } else {
        /* send command */
        buff = g_strdup_printf("s%d %d\x0a", ctrl->conf->util5, 0);
        retcode = stnctld_socket_rw(ctrl->client.socket, buff, buffback, 128);
        g_free(buff);


    sat_log_log(SAT_LOG_LEVEL_DEBUG,
                _("%s: Util5 Button Toggled"), __func__);
    }
    if (retcode) {
        retval = (gint) g_strtod(buffback + 4, NULL);
        if (retval != 0) {
            g_strstrip(buffback);
            sat_log_log(SAT_LOG_LEVEL_ERROR,_
                    ("%s:%d: stnctld returned error %s with %s"),
                    __FILE__, __LINE__, retval, buffback);
        }
    }



}

/**
 * Manage toggle signals (Util6)
 *
 * \param button Pointer to the GtkToggle button.
 * \param data Pointer to the GtkStnCtrl widget.
 */
static void util6_toggle_cb(GtkToggleButton * button, gpointer data)
{
    GtkStnCtrl    *ctrl = GTK_STN_CTRL(data);
    gchar          *buff;
    gchar           buffback[128];
    gboolean        retcode;
    gint            retval;

    if (gtk_toggle_button_get_active(button))
    {
        
        /* send command */
        buff = g_strdup_printf("s%d %d\x0a", ctrl->conf->util6, 1);
        retcode = stnctld_socket_rw(ctrl->client.socket, buff, buffback, 128);
        g_free(buff);

    } else {
        /* send command */
        buff = g_strdup_printf("s%d %d\x0a", ctrl->conf->util6, 0);
        retcode = stnctld_socket_rw(ctrl->client.socket, buff, buffback, 128);
        g_free(buff);


    sat_log_log(SAT_LOG_LEVEL_DEBUG,
                _("%s: Util6 Button Toggled"), __func__);
    }
    if (retcode) {
        retval = (gint) g_strtod(buffback + 4, NULL);
        if (retval != 0) {
            g_strstrip(buffback);
            sat_log_log(SAT_LOG_LEVEL_ERROR,_
                    ("%s:%d: stnctld returned error %s with %s"),
                    __FILE__, __LINE__, retval, buffback);
        }
    }



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

    gchar          *buff;
    gchar           buffback[128];
    gboolean        retcode;
    gboolean        state;
    gint            retval;

    if (ctrl->connected) {
        /* Read coil for Utils */
        state = coil_read(ctrl, ctrl->conf->util1);
        gtk_toggle_button_set_active(ctrl->util1, state);
        state = coil_read(ctrl, ctrl->conf->util2);
        gtk_toggle_button_set_active(ctrl->util2, state);
        state = coil_read(ctrl, ctrl->conf->util3);
        gtk_toggle_button_set_active(ctrl->util3, state);
        state = coil_read(ctrl, ctrl->conf->util4);
        gtk_toggle_button_set_active(ctrl->util4, state);
        state = coil_read(ctrl, ctrl->conf->util5);
        gtk_toggle_button_set_active(ctrl->util5, state);
        state = coil_read(ctrl, ctrl->conf->util6);
        gtk_toggle_button_set_active(ctrl->util6, state);

        /* Update config states */
        state = coil_read(ctrl, ctrl->conf->config1);
        if (state) {
            gtk_toggle_button_set_active(ctrl->config1On, state);
        } else {
            gtk_toggle_button_set_active(ctrl->config1Off, !state);
        }
        state = coil_read(ctrl, ctrl->conf->config2);
        if (state) {
            gtk_toggle_button_set_active(ctrl->config2On, state);
        } else {
            gtk_toggle_button_set_active(ctrl->config2Off, !state);
        }
        state = coil_read(ctrl, ctrl->conf->config3);
        if (state) {
            gtk_toggle_button_set_active(ctrl->config3On, state);
        } else {
            gtk_toggle_button_set_active(ctrl->config3Off, !state);
        }
        state = coil_read(ctrl, ctrl->conf->config4);
        if (state) {
            gtk_toggle_button_set_active(ctrl->config4On, state);
        } else {
            gtk_toggle_button_set_active(ctrl->config4Off, !state);
        }
    }
    
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

    ctrl->connected = FALSE;
    ctrl->conf = NULL;
    ctrl->timerid = 0;

    g_mutex_init(&ctrl->client.mutex);
    ctrl->client.thread = NULL;
    ctrl->client.socket = -1;
    ctrl->client.running = FALSE;    
}

static void gtk_stn_ctrl_destroy(GtkWidget * widget)
{
    GtkStnCtrl     *ctrl = GTK_STN_CTRL(widget);
    
    if (ctrl->timerid > 0)
    {
        g_source_remove(ctrl->timerid);
    }

    /* free configuration */
    if (ctrl->conf != NULL)
    {
        g_free(ctrl->conf->name);
        g_free(ctrl->conf->host);
        g_free(ctrl->conf);
        ctrl->conf = NULL;
    }

    if (ctrl->client.running) 
    {
        ctrl->client.running = FALSE;
        g_thread_join(ctrl->client.thread);
    }

    g_mutex_clear(&ctrl->client.mutex);

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
    station_conf_t *conf = NULL;

    ctrl->conf = g_try_new(station_conf_t, 1);
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

    /* load new configuration */
    ctrl->conf->name =
        gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(ctrl->stnSel));
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
    }
    g_signal_connect(ctrl->stnSel, "changed", G_CALLBACK(stn_selected_cb),
                     ctrl);
    gtk_grid_attach(GTK_GRID(table), ctrl->stnSel, 1, 0, 2, 1);

    ctrl->connect = gtk_toggle_button_new_with_label(_("Connect"));
    gtk_widget_set_tooltip_text(ctrl->connect,
                                _
                                ("Connect to the selected station"));
    gtk_grid_attach(GTK_GRID(table), ctrl->connect, 0, 1, 3, 1);
    g_signal_connect(ctrl->connect, "toggled", G_CALLBACK(connect_toggle_cb),
                     ctrl);

    frame = gtk_frame_new(_("Settings"));
    gtk_container_add(GTK_CONTAINER(frame), table);

    return frame;
}

static GtkWidget *create_monitor(GtkStnCtrl * ctrl)
{
    GtkWidget *frame, *table;
    GtkWidget *ana1_label, *ana2_label;
    GtkWidget *dig1_label, *dig2_label, *dig3_label;
    GtkWidget *dig1_val, *dig2_val, *dig3_val;
    gint a_row = 0;
    gint a_col = 0;
    gint d_row = 0;
    gint d_col = 0;
    gboolean as = FALSE;

    table = gtk_grid_new();
    gtk_container_set_border_width(GTK_CONTAINER(table), 5);
    gtk_grid_set_column_spacing(GTK_GRID(table), 5);
    gtk_grid_set_row_spacing(GTK_GRID(table), 5);

    ana1_label = gtk_label_new(_(ctrl->conf->ana1_name));
    g_object_set(ana1_label, "xalign", 0.0f, "yalign", 0.5f, NULL);
    ctrl->ana1 = gtk_label_new(_("---"));
    g_object_set(ctrl->ana1, "xalign", 0.0f, "yalign", 0.5f, NULL);
    ana2_label = gtk_label_new(_(ctrl->conf->ana2_name));
    g_object_set(ana2_label, "xalign", 0.0f, "yalign", 0.5f, NULL);
    ctrl->ana2 = gtk_label_new(_("---"));
    g_object_set(ctrl->ana2, "xalign", 0.0f, "yalign", 0.5f, NULL);

    dig1_label = gtk_label_new(_(ctrl->conf->dig1_name));
    g_object_set(dig1_label, "xalign", 0.0f, "yalign", 0.5f, NULL);
    ctrl->dig1 = gtk_label_new(_("---"));
    g_object_set(ctrl->dig1, "xalign", 0.0f, "yalign", 0.5f, NULL);
    dig2_label = gtk_label_new(_(ctrl->conf->dig2_name));
    g_object_set(dig2_label, "xalign", 0.0f, "yalign", 0.5f, NULL);
    ctrl->dig2 = gtk_label_new(_("---"));
    g_object_set(ctrl->dig2, "xalign", 0.0f, "yalign", 0.5f, NULL);
    dig3_label = gtk_label_new(_(ctrl->conf->dig3_name));
    g_object_set(dig3_label, "xalign", 0.0f, "yalign", 0.5f, NULL);
    ctrl->dig3 = gtk_label_new(_("---"));
    g_object_set(ctrl->dig3, "xalign", 0.0f, "yalign", 0.5f, NULL);

    /* All labels made, now pack ones that are configured */

    if (strncmp(ctrl->conf->ana1_name, "", 1)) 
    {
        gtk_grid_attach(GTK_GRID(table), ana1_label, a_col, a_row, 1, 1);
        gtk_grid_attach(GTK_GRID(table), ctrl->ana1, a_col+1, a_row, 1, 1);
        a_row++;
        as = TRUE;
    }
    if (strncmp(ctrl->conf->ana2_name, "", 1)) 
    {
        gtk_grid_attach(GTK_GRID(table), ana2_label, a_col, a_row, 1, 1);
        gtk_grid_attach(GTK_GRID(table), ctrl->ana2, a_col+1, a_row, 1, 1);
        as = TRUE;
    }
    if (as) {d_col+=2;}
    if (strncmp(ctrl->conf->dig1_name, "", 1)) 
    {
        gtk_grid_attach(GTK_GRID(table), dig1_label, d_col, d_row, 1, 1);
        gtk_grid_attach(GTK_GRID(table), ctrl->dig1, d_col+1, d_row, 1, 1);
        d_row++;
    }
    if (strncmp(ctrl->conf->dig2_name, "", 1)) 
    {
        gtk_grid_attach(GTK_GRID(table), dig2_label, d_col, d_row, 1, 1);
        gtk_grid_attach(GTK_GRID(table), ctrl->dig2, d_col+1, d_row, 1, 1);
        d_row++;
    }
    if (strncmp(ctrl->conf->dig3_name, "", 1)) 
    {
        gtk_grid_attach(GTK_GRID(table), dig3_label, d_col, d_row, 1, 1);
        gtk_grid_attach(GTK_GRID(table), ctrl->dig3, d_col+1, d_row, 1, 1);
    }


    frame = gtk_frame_new(_("Monitor"));
    gtk_container_add(GTK_CONTAINER(frame), table);

    return frame;
}

static GtkWidget *create_config(GtkStnCtrl * ctrl)
{
    GtkWidget *frame, *table;
    GtkWidget *config1_label, *config2_label, *config3_label, *config4_label;

    gint c_row = 0;
    gint c_col = 0;

    table = gtk_grid_new();
    gtk_container_set_border_width(GTK_CONTAINER(table), 5);
    gtk_grid_set_column_spacing(GTK_GRID(table), 5);
    gtk_grid_set_row_spacing(GTK_GRID(table), 5);

    config1_label = gtk_label_new(_(ctrl->conf->config1_name));
    g_object_set(config1_label, "xalign", 0.0f, "yalign", 0.5f, NULL);
    ctrl->config1On = gtk_toggle_button_new_with_label(_(ctrl->conf->config1_on));
    gtk_widget_set_tooltip_text(ctrl->config1On, _("Activate this station state"));
    g_signal_connect(ctrl->config1On, "toggled", G_CALLBACK(config1On_toggle_cb), ctrl);  
    ctrl->config1Off = gtk_toggle_button_new_with_label(_(ctrl->conf->config1_off));
    gtk_widget_set_tooltip_text(ctrl->config1Off, _("Activate this station state"));
    g_signal_connect(ctrl->config1Off, "toggled", G_CALLBACK(config1Off_toggle_cb), ctrl); 

    config2_label = gtk_label_new(_(ctrl->conf->config2_name));
    g_object_set(config2_label, "xalign", 0.0f, "yalign", 0.5f, NULL);
    ctrl->config2On = gtk_toggle_button_new_with_label(_(ctrl->conf->config2_on));
    gtk_widget_set_tooltip_text(ctrl->config2On, _("Activate this station state"));
    g_signal_connect(ctrl->config2On, "toggled", G_CALLBACK(config2On_toggle_cb), ctrl);  
    ctrl->config2Off = gtk_toggle_button_new_with_label(_(ctrl->conf->config2_off));
    gtk_widget_set_tooltip_text(ctrl->config2Off, _("Activate this station state"));
    g_signal_connect(ctrl->config2Off, "toggled", G_CALLBACK(config2Off_toggle_cb), ctrl); 

    config3_label = gtk_label_new(_(ctrl->conf->config3_name));
    g_object_set(config3_label, "xalign", 0.0f, "yalign", 0.5f, NULL);
    ctrl->config3On = gtk_toggle_button_new_with_label(_(ctrl->conf->config3_on));
    gtk_widget_set_tooltip_text(ctrl->config3On, _("Activate this station state"));
    g_signal_connect(ctrl->config3On, "toggled", G_CALLBACK(config3On_toggle_cb), ctrl);  
    ctrl->config3Off = gtk_toggle_button_new_with_label(_(ctrl->conf->config3_off));
    gtk_widget_set_tooltip_text(ctrl->config3Off, _("Activate this station state"));
    g_signal_connect(ctrl->config3Off, "toggled", G_CALLBACK(config3Off_toggle_cb), ctrl); 

    config4_label = gtk_label_new(_(ctrl->conf->config4_name));
    g_object_set(config4_label, "xalign", 0.0f, "yalign", 0.5f, NULL);
    ctrl->config4On = gtk_toggle_button_new_with_label(_(ctrl->conf->config4_on));
    gtk_widget_set_tooltip_text(ctrl->config4On, _("Activate this station state"));
    g_signal_connect(ctrl->config4On, "toggled", G_CALLBACK(config4On_toggle_cb), ctrl);  
    ctrl->config4Off = gtk_toggle_button_new_with_label(_(ctrl->conf->config4_off));
    gtk_widget_set_tooltip_text(ctrl->config4Off, _("Activate this station state"));
    g_signal_connect(ctrl->config4Off, "toggled", G_CALLBACK(config4Off_toggle_cb), ctrl);  


    /* All labels made, now pack ones that are configured */

    if (strncmp(ctrl->conf->config1_name, "", 1)) 
    {
        gtk_grid_attach(GTK_GRID(table), config1_label, c_col, c_row, 1, 1);
        gtk_grid_attach(GTK_GRID(table), ctrl->config1On, c_col+1, c_row, 1, 1);
        gtk_grid_attach(GTK_GRID(table), ctrl->config1Off, c_col+2, c_row, 1, 1);
        c_row++;
    }
    if (strncmp(ctrl->conf->config2_name, "", 1)) 
    {
        gtk_grid_attach(GTK_GRID(table), config2_label, c_col, c_row, 1, 1);
        gtk_grid_attach(GTK_GRID(table), ctrl->config2On, c_col+1, c_row, 1, 1);
        gtk_grid_attach(GTK_GRID(table), ctrl->config2Off, c_col+2, c_row, 1, 1);
        c_row++;
    }
    if (strncmp(ctrl->conf->config3_name, "", 1)) 
    {
        gtk_grid_attach(GTK_GRID(table), config3_label, c_col, c_row, 1, 1);
        gtk_grid_attach(GTK_GRID(table), ctrl->config3On, c_col+1, c_row, 1, 1);
        gtk_grid_attach(GTK_GRID(table), ctrl->config3Off, c_col+2, c_row, 1, 1);
        c_row++;
    }
    if (strncmp(ctrl->conf->config4_name, "", 1)) 
    {
        gtk_grid_attach(GTK_GRID(table), config4_label, c_col, c_row, 1, 1);
        gtk_grid_attach(GTK_GRID(table), ctrl->config4On, c_col+1, c_row, 1, 1);
        gtk_grid_attach(GTK_GRID(table), ctrl->config4Off, c_col+2, c_row, 1, 1);
        c_row++;
    }


    frame = gtk_frame_new(_("Configure"));
    gtk_container_add(GTK_CONTAINER(frame), table);

    return frame;
}

static GtkWidget *create_utils(GtkStnCtrl * ctrl)
{
    GtkWidget *frame, *table;

    gint u_row = 0;
    gint u_col = 0;

    table = gtk_grid_new();
    gtk_container_set_border_width(GTK_CONTAINER(table), 5);
    gtk_grid_set_column_spacing(GTK_GRID(table), 5);
    gtk_grid_set_row_spacing(GTK_GRID(table), 5);

    ctrl->util1 = gtk_toggle_button_new_with_label(_(ctrl->conf->util1_name));
    gtk_widget_set_tooltip_text(ctrl->util1, _("Toggle power state of this utility"));
    g_signal_connect(ctrl->util1, "toggled", G_CALLBACK(util1_toggle_cb), ctrl);  

    ctrl->util2 = gtk_toggle_button_new_with_label(_(ctrl->conf->util2_name));
    gtk_widget_set_tooltip_text(ctrl->util2, _("Toggle power state of this utility"));
    g_signal_connect(ctrl->util2, "toggled", G_CALLBACK(util2_toggle_cb), ctrl); 

    ctrl->util3 = gtk_toggle_button_new_with_label(_(ctrl->conf->util3_name));
    gtk_widget_set_tooltip_text(ctrl->util3, _("Toggle power state of this utility"));
    g_signal_connect(ctrl->util3, "toggled", G_CALLBACK(util3_toggle_cb), ctrl); 

    ctrl->util4 = gtk_toggle_button_new_with_label(_(ctrl->conf->util4_name));
    gtk_widget_set_tooltip_text(ctrl->util4, _("Toggle power state of this utility"));
    g_signal_connect(ctrl->util4, "toggled", G_CALLBACK(util4_toggle_cb), ctrl); 

    ctrl->util5 = gtk_toggle_button_new_with_label(_(ctrl->conf->util5_name));
    gtk_widget_set_tooltip_text(ctrl->util5, _("Toggle power state of this utility"));
    g_signal_connect(ctrl->util5, "toggled", G_CALLBACK(util5_toggle_cb), ctrl); 

    ctrl->util6 = gtk_toggle_button_new_with_label(_(ctrl->conf->util6_name));
    gtk_widget_set_tooltip_text(ctrl->util6, _("Toggle power state of this utility"));
    g_signal_connect(ctrl->util6, "toggled", G_CALLBACK(util6_toggle_cb), ctrl); 


    /* All labels made, now pack ones that are configured */

    if (strncmp(ctrl->conf->util1_name, "", 1)) 
    {
        gtk_grid_attach(GTK_GRID(table), ctrl->util1, u_col, u_row, 1, 1);
        u_row++;
    }
    if (strncmp(ctrl->conf->util2_name, "", 1)) 
    {
        gtk_grid_attach(GTK_GRID(table), ctrl->util2, u_col, u_row, 1, 1);
        u_row++;
    }
    if (strncmp(ctrl->conf->util3_name, "", 1)) 
    {
        gtk_grid_attach(GTK_GRID(table), ctrl->util3, u_col, u_row, 1, 1);
        u_row++;
    }
    if (strncmp(ctrl->conf->util4_name, "", 1)) 
    {
        gtk_grid_attach(GTK_GRID(table), ctrl->util4, u_col, u_row, 1, 1);
        u_row++;
    }
    if (strncmp(ctrl->conf->util5_name, "", 1)) 
    {
        gtk_grid_attach(GTK_GRID(table), ctrl->util5, u_col, u_row, 1, 1);
        u_row++;
    }
    if (strncmp(ctrl->conf->util6_name, "", 1)) 
    {
        gtk_grid_attach(GTK_GRID(table), ctrl->util6, u_col, u_row, 1, 1);
        u_row++;
    }


    frame = gtk_frame_new(_("Utilities"));
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
    gtk_grid_attach(GTK_GRID(table), create_monitor(stn_ctrl), 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(table), create_utils(stn_ctrl), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(table), create_config(stn_ctrl), 1, 1, 1, 1);


    gtk_box_pack_start(GTK_BOX(stn_ctrl), table, FALSE, FALSE, 5);
    gtk_container_set_border_width(GTK_CONTAINER(stn_ctrl), 5);

    stn_ctrl->timerid = g_timeout_add(DELAY,
                                      stn_ctrl_timeout_cb, stn_ctrl);


    return GTK_WIDGET(stn_ctrl);
}
