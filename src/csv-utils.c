#ifdef HAVE_CONFIG_H
#include <build-config.h>
#endif

#include "csv-utils.h"
#include <glib.h>
#include <glib/gi18n.h>
#include "time-tools.h"


gdouble sched_last_time (gchar *filename)
{

    FILE* stream = fopen(filename, "r");
    gchar[1024] line;
    gboolean done = FALSE;
    // Read header
    fgets(line, 1024, stream);
    while (fgets(line, 1024, stream)) {
        token  = strtok(line, ',');

    }

    return g_strtod(token);

}

gdouble sched_update_az (gchar *filename, gdouble t)
{

    FILE* stream = fopen(filename, "r");
    gchar[1024] line;
    gdouble c_time;
    gdouble az;
    gboolean done = FALSE;
    // Read header
    fgets(line, 1024, stream);
    while (!done) {
        done = !fgets(line, 1024, stream);
        token  = strtok(line, ',');
        s_time = g_strtod(token);
        if (t >= s_time) {
            token = strtok(NULL, ',');
            az = g_strtod(token);
            done = TRUE;
        }
    }
    return az;

}

gdouble sched_update_el (gchar *filename, gdouble t)
{

    FILE* stream = fopen(filename, "r");
    gchar[1024] line;
    gdouble c_time;
    gdouble el;
    gboolean done = FALSE;
    // Read header
    fgets(line, 1024, stream);
    while (!done) {
        fgets(line, 1024, stream);
        token  = strtok(line, ',');
        s_time = g_strtod(token);
        if (t >= s_time) {
            token = strtok(NULL, ',');
            token = strtok(NULL, ',');
            el = g_strtod(token);
            done = TRUE;
        }

    }
    return el;

}


schedule_t *get_schedule (gchar *filename)
{
    gdouble start = g_get_current_time();
    gdouble az = sched_update_az(filename, start);
    gdouble el = sched_update_el(filename, start);
    gdouble end = sched_last_time(filename);

    schedule_t *sched = NULL;

    sched->az = az;
    sched->el = el;
    sched->start = start; 
    sched->end = end;
   
    return sched;

}



