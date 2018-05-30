
#ifndef STATION_CONF_H
#define STATION_CONF_H 1

#include <glib.h>



/** \brief Station configuration. */
typedef struct {
    gchar          *name;       /*!< Configuration file name, less .stn */
    gchar          *host;       /*!< hostname */
    gint            port;       /*!< port number */

    gint            util1;      /*!< relay num for util1 */
    gchar          *util1_name; /*!< name for util1 */
    gint            util2;      /*!< relay num for util2 */
    gchar          *util2_name; /*!< name for util2 */
    gint            util3;      /*!< relay num for util3 */
    gchar          *util3_name; /*!< name for util3 */
    gint            util4;      /*!< relay num for util4 */
    gchar          *util4_name; /*!< name for util4 */
    gint            util5;      /*!< relay num for util5 */
    gchar          *util5_name; /*!< name for util5 */
    gint            util6;      /*!< relay num for util6 */
    gchar          *util6_name; /*!< name for util6 */

    gint            ana1;      /*!< input num for ana1 */
    gchar          *ana1_name; /*!< name for ana1 */
    gint            ana2;      /*!< input num for ana2 */
    gchar          *ana2_name; /*!< name for ana2 */

    gint            dig1;      /*!< input num for dig1 */
    gchar          *dig1_name; /*!< name for dig1 */
    gint            dig2;      /*!< input num for dig2 */
    gchar          *dig2_name; /*!< name for dig2 */
    gint            dig3;      /*!< input num for dig3 */
    gchar          *dig3_name; /*!< name for dig3 */

    gint            config1;      /*!< relay num for config1 */
    gchar          *config1_name; /*!< name for config1 */
    gchar          *config1_on;   /*!< name for config1 on state */
    gchar          *config1_off;  /*!< name for config1 off state */
    gint            config2;      /*!< relay num for config2 */
    gchar          *config2_name; /*!< name for config2 */
    gchar          *config2_on;   /*!< name for config2 on state */
    gchar          *config2_off;  /*!< name for config2 off state */
    gint            config3;      /*!< relay num for config3 */
    gchar          *config3_name; /*!< name for config3 */
    gchar          *config3_on;   /*!< name for config3 on state */
    gchar          *config3_off;  /*!< name for config3 off state */
    gint            config4;      /*!< relay num for config4 */
    gchar          *config4_name; /*!< name for config4 */
    gchar          *config4_on;   /*!< name for config4 on state */
    gchar          *config4_off;  /*!< name for config4 off state */

} station_conf_t;


gboolean        station_conf_read(station_conf_t * conf);
void            station_conf_save(station_conf_t * conf);

#endif
