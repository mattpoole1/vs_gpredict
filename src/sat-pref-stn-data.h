
#ifndef SAT_PREF_STN_DATA_H
#define SAT_PREF_STN_DATA_H 1

/** Column definitions for station list. */
typedef enum {
    STN_LIST_COL_NAME = 0,      /*!< File name. */
    STN_LIST_COL_HOST,          /*!< Hostname */
    STN_LIST_COL_PORT,          /*!< Port number */

    STN_LIST_COL_UTIL1,          /*!< Relay num for Util1 */
    STN_LIST_COL_UTIL1_NAME,      /*!< Name for Util1 */
    STN_LIST_COL_UTIL2,           /*!< Relay num for Util2 */
    STN_LIST_COL_UTIL2_NAME,      /*!< Name for Util2 */
    STN_LIST_COL_UTIL3,           /*!< Relay num for Util3 */
    STN_LIST_COL_UTIL3_NAME,      /*!< Name for Util3 */
    STN_LIST_COL_UTIL4,           /*!< Relay num for Util4 */
    STN_LIST_COL_UTIL4_NAME,      /*!< Name for Util4 */
    STN_LIST_COL_UTIL5,           /*!< Relay num for Util5 */
    STN_LIST_COL_UTIL5_NAME,      /*!< Name for Util5 */
    STN_LIST_COL_UTIL6,           /*!< Relay num for Util6 */
    STN_LIST_COL_UTIL6_NAME,      /*!< Name for Util6 */

    STN_LIST_COL_ANA1,           /*!< Input num for Ana1 */
    STN_LIST_COL_ANA1_NAME,      /*!< Name for Ana1 */
    STN_LIST_COL_ANA2,           /*!< Input num for Ana2 */
    STN_LIST_COL_ANA2_NAME,      /*!< Name for Ana2 */

    STN_LIST_COL_DIG1,           /*!< Input num for Dig1 */
    STN_LIST_COL_DIG1_NAME,      /*!< Name for Dig1 */
    STN_LIST_COL_DIG2,           /*!< Input num for Dig2 */
    STN_LIST_COL_DIG2_NAME,      /*!< Name for Dig2 */
    STN_LIST_COL_DIG3,           /*!< Input num for Dig3 */
    STN_LIST_COL_DIG3_NAME,      /*!< Name for Dig3 */

    STN_LIST_COL_CONFIG1,           /*!< Relay num for Config1 */
    STN_LIST_COL_CONFIG1_NAME,      /*!< Name for Config1 */
    STN_LIST_COL_CONFIG1_ON,        /*!< Name for Config1 ON state */
    STN_LIST_COL_CONFIG1_OFF,       /*!< Name for Config1 OFF state */
    STN_LIST_COL_CONFIG2,           /*!< Relay num for Config2 */
    STN_LIST_COL_CONFIG2_NAME,      /*!< Name for Config2 */
    STN_LIST_COL_CONFIG2_ON,        /*!< Name for Config2 ON state */
    STN_LIST_COL_CONFIG2_OFF,       /*!< Name for Config2 OFF state */
    STN_LIST_COL_CONFIG3,           /*!< Relay num for Config3 */
    STN_LIST_COL_CONFIG3_NAME,      /*!< Name for Config3 */
    STN_LIST_COL_CONFIG3_ON,        /*!< Name for Config3 ON state */
    STN_LIST_COL_CONFIG3_OFF,       /*!< Name for Config3 OFF state */
    STN_LIST_COL_CONFIG4,           /*!< Relay num for Config4 */
    STN_LIST_COL_CONFIG4_NAME,      /*!< Name for Config4 */
    STN_LIST_COL_CONFIG4_ON,        /*!< Name for Config4 ON state */
    STN_LIST_COL_CONFIG4_OFF,       /*!< Name for Config4 OFF state */

    STN_LIST_COL_NUM            /*!< The number of fields in the list. */
} station_list_col_t;

#endif
