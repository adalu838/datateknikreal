#ifndef CLOCK_H
#define CLOCK_H

/* clock_init: initialise clock */ 
void init_clock(void); 

/* set_time: set current time to hours, minutes and seconds */ 
void set_time(int hours, int minutes, int seconds); 

/* set_alarm: set alarm time to hours, minutes and seconds and enable alarm */ 
void set_alarm(int hours, int minutes, int seconds); 

/* increment_time: increments the current time with 
   one second */ 
void increment_time(void);

void increment_alarm(void);

/* get_time: read time from common clock variables */ 
void get_time(int *hours, int *minutes, int *seconds);

/* get_alarm_time: read time from common alarm clock variables */ 
void get_alarm_time(int *hours, int *minutes, int *seconds);

/* time_from_set_message: extract time from set message from user interface */ 
void time_from_set_message(char message[], int *hours, int *minutes, int *seconds);

/* time_from_alarm_message: extract time from set message from user interface */ 
void time_from_alarm_message(char message[], int *hours, int *minutes, int *seconds);

/* time_ok: returns nonzero if hours, minutes and seconds represents a valid time */ 
int time_ok(int hours, int minutes, int seconds);

// Check if alarm is enabled
void check_if_enable(int *enable);

// Reset and disable alarm
void reset_alarm(void);

#endif
