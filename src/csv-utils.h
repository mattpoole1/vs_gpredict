/*

csv-utils.h

This module provides utility functions and data types for performing a controlled rotator sweep with Az / El and time data provided in a csv file.

The csv file should be in the format:

Time (begin at 0:00)  |   Az   |   El
        0:00            100.0     45.0
        0:20            115.0     50.0

etc.

Author: Matt Poole
Date: 18th May 2018
Contact: matt.poole1@gmail.com

*/

#ifndef CSV_UTILS_H
#define CSV_UTILS_H 1

/** \brief Brief manual pass info .*/
typedef struct {
    gdouble   az;
    gdouble   el;
    gdouble   start;
    gdouble   end;
} schedule_t;

gdouble sched_last_time (gchar *filename);
gdouble sched_update_az (gchar *filename, gdouble t);
gdouble sched_update_el (gchar *filename, gdouble t);
schedule_t *get_schedule (gchar *filename);


#endif
 
