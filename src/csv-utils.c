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
    gchar line[1024];
    gchar *token;
    // Read header
    fgets(line, 1024, stream);

    while (fgets(line, 1024, stream)) {
        token  = strtok(line, ",");

    }

    return g_strtod(token, NULL);


}

gdouble sched_v_az (gchar *filename, gdouble t)
{
    FILE* stream = fopen(filename, "r");
    gchar line[1024];
    gchar *token;
    gdouble t0;
    gdouble az0;
    gdouble t1;
    gdouble az1;
    gdouble az;
    gboolean looking = TRUE;
    // Read header
    fgets(line, 1024, stream);
    // Start with first line
    looking = fgets(line, 1024, stream);
    token = strtok(line, ",");
    t1 = g_strtod(token, NULL);
    token = strtok(NULL, ",");
    az1 = g_strtod(token, NULL);

    while (looking)
    {
        az0 = az1;
        t0 = t1;
        looking = (t > t0) && fgets(line, 1024, stream);
        token = strtok(line, ",");
        t1 = g_strtod(token, NULL);
        token = strtok(NULL, ",");
        az1 = g_strtod(token, NULL);
    }
    az = (az1-az0)/(t1-t0) * (t-t0) + az0;
        
    return az;
}
        

gdouble sched_update_az (gchar *filename, gdouble t)
{

    FILE* stream = fopen(filename, "r");
    gchar line[1024];
    gchar *token;
    gdouble s_time;
    gdouble az;
    gboolean looking = TRUE;
    // Read header
    fgets(line, 1024, stream);
    // Start with Az as first az entry in table
    fgets(line, 1024, stream);
    // Get second entry of second line
    token = strtok(line, ",");
    token = strtok(NULL, ",");
    az = g_strtod(token, NULL);
    
    while (looking)
    {
        looking = fgets(line, 1024, stream);
        token  = strtok(line, ",");
        s_time = g_strtod(token, NULL);
        if ((t <= s_time) || (!looking)) {
            looking = FALSE;
        } else {
            token = strtok(NULL, ",");
            az = g_strtod(token, NULL); 
        }

    }
    return az;


}

gdouble sched_update_el (gchar *filename, gdouble t)
{

    FILE* stream = fopen(filename, "r");
    gchar line[1024];
    gchar *token;
    gdouble s_time;
    gdouble el;
    gboolean looking = TRUE;
    // Read header
    fgets(line, 1024, stream); 
    // Start with Az as first az entry in table
    fgets(line, 1024, stream);
    // Get third entry of second line
    token = strtok(line, ",");
    token = strtok(NULL, ",");
    token = strtok(NULL, ",");
    el = g_strtod(token, NULL);

    while (looking)
    {
        looking = fgets(line, 1024, stream);
        token  = strtok(line, ",");
        s_time = g_strtod(token, NULL);
        if ((t <= s_time) || (!looking)) {
            looking = FALSE;
        } else {
            token = strtok(NULL, ",");
            token = strtok(NULL, ",");
            el = g_strtod(token, NULL);
        }
    }
    
    return el;

}

schedule_t *get_schedule (gchar *filename)
{
    gdouble start = get_current_daynum();
    gdouble az = sched_v_az(filename, 1.5);
    gdouble el = sched_update_el(filename, 0);
    gdouble end = sched_last_time(filename);
    schedule_t *sched = NULL;
    sched = g_new(schedule_t, 1);

    sched->az = az;
    sched->el = el;
    sched->start = start; 
    sched->end = end;
   
    return sched;

}



