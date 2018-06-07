
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include "sat-log.h"
#include "compat.h"

#include "station-conf.h"
#include "gpredict-utils.h"

#define GROUP           "Station"
#define KEY_NAME        "Name"
#define KEY_HOST        "Host"
#define KEY_PORT        "Port"

#define KEY_UTIL1       "Util1"
#define KEY_UTIL1_NAME  "Util1 Name"
#define KEY_UTIL2       "Util2"
#define KEY_UTIL2_NAME  "Util2 Name"
#define KEY_UTIL3       "Util3"
#define KEY_UTIL3_NAME  "Util3 Name"
#define KEY_UTIL4       "Util4"
#define KEY_UTIL4_NAME  "Util4 Name"
#define KEY_UTIL5       "Util5"
#define KEY_UTIL5_NAME  "Util5 Name"
#define KEY_UTIL6       "Util6"
#define KEY_UTIL6_NAME  "Util6 Name"

#define KEY_ANA1       "Ana1"
#define KEY_ANA1_NAME  "Ana1 Name"
#define KEY_ANA2       "Ana2"
#define KEY_ANA2_NAME  "Ana2 Name"

#define KEY_DIG1       "Dig1"
#define KEY_DIG1_NAME  "Dig1 Name"
#define KEY_DIG2       "Dig2"
#define KEY_DIG2_NAME  "Dig2 Name"
#define KEY_DIG3       "Dig3"
#define KEY_DIG3_NAME  "Dig3 Name"

#define KEY_CONFIG1       "Config1"
#define KEY_CONFIG1_NAME  "Config1 Name"
#define KEY_CONFIG1_ON    "Config1 On"
#define KEY_CONFIG1_OFF   "Config1 Off"
#define KEY_CONFIG2       "Config2"
#define KEY_CONFIG2_NAME  "Config2 Name"
#define KEY_CONFIG2_ON    "Config2 On"
#define KEY_CONFIG2_OFF   "Config2 Off"
#define KEY_CONFIG3       "Config3"
#define KEY_CONFIG3_NAME  "Config3 Name"
#define KEY_CONFIG3_ON    "Config3 On"
#define KEY_CONFIG3_OFF   "Config3 Off"
#define KEY_CONFIG4       "Config4"
#define KEY_CONFIG4_NAME  "Config4 Name"
#define KEY_CONFIG4_ON    "Config4 On"
#define KEY_CONFIG4_OFF   "Config4 Off"



/**
 * \brief Read station configuration.
 * \param conf Pointer to a station_conf_t structure where the data will be
 *             stored.
 * 
 * This function reads a station configuration from a .stn file into conf.
 * conf->name must contain the file name of the configuration (no path, just
 * file name and without the .stn extension).
 */
gboolean station_conf_read(station_conf_t * conf)
{
    GKeyFile       *cfg = NULL;
    gchar          *confdir;
    gchar          *fname;
    GError         *error = NULL;

    if (conf->name == NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: NULL configuration name!"), __func__);
        return FALSE;
    }

    confdir = get_hwconf_dir();
    fname = g_strconcat(confdir, G_DIR_SEPARATOR_S, conf->name, ".stn", NULL);
    g_free(confdir);

    /* open .stn file */
    cfg = g_key_file_new();
    g_key_file_load_from_file(cfg, fname, 0, NULL);

    if (cfg == NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Could not load file %s\n"), __func__, fname);
        g_free(fname);

        return FALSE;
    }

    g_free(fname);

    /* read parameters */
    conf->host = g_key_file_get_string(cfg, GROUP, KEY_HOST, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Error reading station conf from %s (%s)."),
                    __func__, conf->name, error->message);
        g_clear_error(&error);
        g_key_file_free(cfg);
        return FALSE;
    }

    conf->port = g_key_file_get_integer(cfg, GROUP, KEY_PORT, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Error reading station conf from %s (%s)."),
                    __func__, conf->name, error->message);
        g_clear_error(&error);
        g_key_file_free(cfg);
        return FALSE;
    }

    conf->util1 = g_key_file_get_integer(cfg, GROUP, KEY_UTIL1, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Util1 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->util1 = 0;
        return FALSE;
    }

    conf->util1_name = g_key_file_get_string(cfg, GROUP, KEY_UTIL1_NAME, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Util1 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->util1_name = NULL;
        return FALSE;
    }

    conf->util2 = g_key_file_get_integer(cfg, GROUP, KEY_UTIL2, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Util2 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->util2 = 0;
        return FALSE;
    }

    conf->util2_name = g_key_file_get_string(cfg, GROUP, KEY_UTIL2_NAME, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Util2 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->util2_name = NULL;
        return FALSE;
    }

    conf->util3 = g_key_file_get_integer(cfg, GROUP, KEY_UTIL3, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Util3 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->util3 = 0;
        return FALSE;
    }

    conf->util3_name = g_key_file_get_string(cfg, GROUP, KEY_UTIL3_NAME, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Util3 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->util3_name = NULL;
        return FALSE;
    }

    conf->util4 = g_key_file_get_integer(cfg, GROUP, KEY_UTIL4, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_INFO,
                    _("%s: Util4 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->util4 = 0;
        return FALSE;
    }

    conf->util4_name = g_key_file_get_string(cfg, GROUP, KEY_UTIL4_NAME, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Util4 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->util4_name = NULL;
        return FALSE;
    }

    conf->util5 = g_key_file_get_integer(cfg, GROUP, KEY_UTIL5, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Util5 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->util5 = 0;
        return FALSE;
    }

    conf->util5_name = g_key_file_get_string(cfg, GROUP, KEY_UTIL5_NAME, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Util5 name not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->util5_name = NULL;
        return FALSE;
    }

    conf->util6 = g_key_file_get_integer(cfg, GROUP, KEY_UTIL6, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Util6 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->util6 = 0;
        return FALSE;
    }

    conf->util6_name = g_key_file_get_string(cfg, GROUP, KEY_UTIL6_NAME, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Util6 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->util6_name = NULL;
        return FALSE;
    }

    conf->ana1 = g_key_file_get_integer(cfg, GROUP, KEY_ANA1, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Ana1 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->ana1 = 0;
        return FALSE;
    }

    conf->ana1_name = g_key_file_get_string(cfg, GROUP, KEY_ANA1_NAME, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Ana1 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->ana1_name = NULL;
        return FALSE;
    }

    conf->ana2 = g_key_file_get_integer(cfg, GROUP, KEY_ANA2, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Ana2 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->ana2 = 0;
        return FALSE;
    }

    conf->ana2_name = g_key_file_get_string(cfg, GROUP, KEY_ANA2_NAME, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Ana2 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->ana2_name = NULL;
        return FALSE;
    }

    conf->dig1 = g_key_file_get_integer(cfg, GROUP, KEY_DIG1, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Dig1 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->dig1 = 0;
        return FALSE;
    }

    conf->dig1_name = g_key_file_get_string(cfg, GROUP, KEY_DIG1_NAME, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Dig1 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->dig1_name = NULL;
        return FALSE;
    }

    conf->dig2 = g_key_file_get_integer(cfg, GROUP, KEY_DIG2, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Dig2 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->dig2 = 0;
        return FALSE;
    }

    conf->dig2_name = g_key_file_get_string(cfg, GROUP, KEY_DIG2_NAME, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Dig2 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->dig2_name = NULL;
        return FALSE;
    }

    conf->dig3 = g_key_file_get_integer(cfg, GROUP, KEY_DIG3, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Dig3 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->dig3 = 0;
        return FALSE;
    }

    conf->dig3_name = g_key_file_get_string(cfg, GROUP, KEY_DIG3_NAME, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Dig3 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->dig3_name = NULL;
        return FALSE;
    }

    conf->config1 = g_key_file_get_integer(cfg, GROUP, KEY_CONFIG1, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Config1 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->config1 = 0;
        return FALSE;
    }

    conf->config1_name = g_key_file_get_string(cfg, GROUP, KEY_CONFIG1_NAME, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Config1 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->config1_name = NULL;
        return FALSE;
    }

    conf->config1_on = g_key_file_get_string(cfg, GROUP, KEY_CONFIG1_ON, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Config1 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->config1_on = NULL;
        return FALSE;
    }

    conf->config1_off = g_key_file_get_string(cfg, GROUP, KEY_CONFIG1_OFF, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Config1 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->config1_off = NULL;
        return FALSE;
    }

    conf->config2 = g_key_file_get_integer(cfg, GROUP, KEY_CONFIG2, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Config2 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->config2 = 0;
        return FALSE;
    }

    conf->config2_name = g_key_file_get_string(cfg, GROUP, KEY_CONFIG2_NAME, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Config2 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->config2_name = NULL;
        return FALSE;
    }

    conf->config2_on = g_key_file_get_string(cfg, GROUP, KEY_CONFIG2_ON, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Config2 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->config2_on = NULL;
        return FALSE;
    }

    conf->config2_off = g_key_file_get_string(cfg, GROUP, KEY_CONFIG2_OFF, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Config2 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->config2_off = NULL;
        return FALSE;
    }

    conf->config3 = g_key_file_get_integer(cfg, GROUP, KEY_CONFIG3, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Config3 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->config3 = 0;
        return FALSE;
    }

    conf->config3_name = g_key_file_get_string(cfg, GROUP, KEY_CONFIG3_NAME, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Config3 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->config3_name = NULL;
        return FALSE;
    }

    conf->config3_on = g_key_file_get_string(cfg, GROUP, KEY_CONFIG3_ON, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Config3 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->config3_on = NULL;
        return FALSE;
    }

    conf->config3_off = g_key_file_get_string(cfg, GROUP, KEY_CONFIG3_OFF, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Config3 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->config3_off = NULL;
        return FALSE;
    }

    conf->config4 = g_key_file_get_integer(cfg, GROUP, KEY_CONFIG4, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Config4 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->config4 = 0;
        return FALSE;
    }

    conf->config4_name = g_key_file_get_string(cfg, GROUP, KEY_CONFIG4_NAME, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Config4 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->config4_name = NULL;
        return FALSE;
    }

    conf->config4_on = g_key_file_get_string(cfg, GROUP, KEY_CONFIG4_ON, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Config4 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->config4_on = NULL;
        return FALSE;
    }

    conf->config4_off = g_key_file_get_string(cfg, GROUP, KEY_CONFIG4_OFF, &error);
    if (error != NULL)
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: Config4 not defined for %s."),
                    __func__, conf->name);
        g_clear_error(&error);
        conf->config4_off = NULL;
        return FALSE;
    }

    g_key_file_free(cfg);

    return TRUE;
}

/**
 * \brief Save station configuration.
 * \param conf Pointer to the station configuration.
 * 
 * This function saves the station configuration stored in conf to a
 * .stn file. conf->name must contain the file name of the configuration
 * (no path, just file name and without the .stn extension).
 */
void station_conf_save(station_conf_t * conf)
{
    GKeyFile       *cfg = NULL;
    gchar          *confdir;
    gchar          *fname;
   
    sat_log_log(SAT_LOG_LEVEL_ERROR,
                _("%s: Beginning Save Function to %s"), __func__, conf->name);

    sat_log_log(SAT_LOG_LEVEL_ERROR,
                _("%s: Successfully interpreted conf->util1_name as %s"), __func__, conf->util1_name);

    if (conf->name == NULL) {
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s: NULL configuration name!"), __func__);
        return;
    }

    /* create a config structure */
    cfg = g_key_file_new();



    g_key_file_set_string(cfg, GROUP, KEY_NAME, conf->name);
    g_key_file_set_string(cfg, GROUP, KEY_HOST, conf->host);
    g_key_file_set_integer(cfg, GROUP, KEY_PORT, conf->port);

    g_key_file_set_integer(cfg, GROUP, KEY_UTIL1, conf->util1);
    g_key_file_set_string(cfg, GROUP, KEY_UTIL1_NAME, conf->util1_name);
    g_key_file_set_integer(cfg, GROUP, KEY_UTIL2, conf->util2);
    g_key_file_set_string(cfg, GROUP, KEY_UTIL2_NAME, conf->util2_name);
    g_key_file_set_integer(cfg, GROUP, KEY_UTIL3, conf->util3);
    g_key_file_set_string(cfg, GROUP, KEY_UTIL3_NAME, conf->util3_name);
    g_key_file_set_integer(cfg, GROUP, KEY_UTIL4, conf->util4);
    g_key_file_set_string(cfg, GROUP, KEY_UTIL4_NAME, conf->util4_name);
    g_key_file_set_integer(cfg, GROUP, KEY_UTIL5, conf->util5);
    g_key_file_set_string(cfg, GROUP, KEY_UTIL5_NAME, conf->util5_name);
    g_key_file_set_integer(cfg, GROUP, KEY_UTIL6, conf->util6);
    g_key_file_set_string(cfg, GROUP, KEY_UTIL6_NAME, conf->util6_name);

    g_key_file_set_integer(cfg, GROUP, KEY_ANA1, conf->ana1);
    g_key_file_set_string(cfg, GROUP, KEY_ANA1_NAME, conf->ana1_name);
    g_key_file_set_integer(cfg, GROUP, KEY_ANA2, conf->ana2);
    g_key_file_set_string(cfg, GROUP, KEY_ANA2_NAME, conf->ana2_name);

    g_key_file_set_integer(cfg, GROUP, KEY_DIG1, conf->dig1);
    g_key_file_set_string(cfg, GROUP, KEY_DIG1_NAME, conf->dig1_name);
    g_key_file_set_integer(cfg, GROUP, KEY_DIG2, conf->dig2);
    g_key_file_set_string(cfg, GROUP, KEY_DIG2_NAME, conf->dig2_name);
    g_key_file_set_integer(cfg, GROUP, KEY_DIG3, conf->dig3);
    g_key_file_set_string(cfg, GROUP, KEY_DIG3_NAME, conf->dig3_name);

    g_key_file_set_integer(cfg, GROUP, KEY_CONFIG1, conf->config1);
    g_key_file_set_string(cfg, GROUP, KEY_CONFIG1_NAME, conf->config1_name);
    g_key_file_set_string(cfg, GROUP, KEY_CONFIG1_ON, conf->config1_on);
    g_key_file_set_string(cfg, GROUP, KEY_CONFIG1_OFF, conf->config1_off);
    g_key_file_set_integer(cfg, GROUP, KEY_CONFIG2, conf->config2);
    g_key_file_set_string(cfg, GROUP, KEY_CONFIG2_NAME, conf->config2_name);
    g_key_file_set_string(cfg, GROUP, KEY_CONFIG2_ON, conf->config2_on);
    g_key_file_set_string(cfg, GROUP, KEY_CONFIG2_OFF, conf->config2_off);
    g_key_file_set_integer(cfg, GROUP, KEY_CONFIG3, conf->config3);
    g_key_file_set_string(cfg, GROUP, KEY_CONFIG3_NAME, conf->config3_name);
    g_key_file_set_string(cfg, GROUP, KEY_CONFIG3_ON, conf->config3_on);
    g_key_file_set_string(cfg, GROUP, KEY_CONFIG3_OFF, conf->config3_off);
    g_key_file_set_integer(cfg, GROUP, KEY_CONFIG4, conf->config4);
    g_key_file_set_string(cfg, GROUP, KEY_CONFIG4_NAME, conf->config4_name);
    g_key_file_set_string(cfg, GROUP, KEY_CONFIG4_ON, conf->config4_on);
    g_key_file_set_string(cfg, GROUP, KEY_CONFIG4_OFF, conf->config4_off);


    /* build filename */
    confdir = get_hwconf_dir();
    sat_log_log(SAT_LOG_LEVEL_ERROR,
                _("%s: Confdir is: %s"), __func__, confdir);
    fname = g_strconcat(confdir, G_DIR_SEPARATOR_S, conf->name, ".stn", NULL);
    g_free(confdir);

    /* save information */
    gpredict_save_key_file(cfg, fname);

    /* cleanup */
    g_free(fname);
    g_key_file_free(cfg);
    
    
}
