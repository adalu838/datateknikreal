/*


*/

/* Simple_OS include */ 
#include <simple_os.h>
#include <stdio.h>

/* Standard library includes */ 
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Data structs for time keeping */

typedef struct
{
	unsigned int hours;
	unsigned int minutes;
	unsigned int seconds;

}time_data_type;

typedef struct
{

    /* the current time */ 
    time_data_type time; 
    /* alarm time */ 
    time_data_type alarm_time; 
    /* alarm enabled flag */ 
    int alarm_enabled; 

    /* semaphore for mutual exclusion */ 
    si_semaphore Mutex; 

    /* semaphore for alarm activation */ 
    si_semaphore start_alarm;

}clock_data_type;

static clock_data_type Clock;

void init_clock(void)
{
    /* initialise time and alarm time */ 

    Clock.time.hours = 0; 
    Clock.time.minutes = 0; 
    Clock.time.seconds = 0; 

    Clock.alarm_time.hours = 0; 
    Clock.alarm_time.minutes = 0; 
    Clock.alarm_time.seconds = 0; 
    
    /* alarm is not enabled */ 
    Clock.alarm_enabled = 0; 

    /* initialise semaphores */ 
    si_sem_init(&Clock.Mutex, 1); 
    si_sem_init(&Clock.start_alarm, 0); 
}

void set_time(int hours, int minutes, int seconds)
{
    si_sem_wait(&Clock.Mutex); 

    Clock.time.hours = hours; 
    Clock.time.minutes = minutes; 
    Clock.time.seconds = seconds; 

    si_sem_signal(&Clock.Mutex); 
}

void set_alarm(int hours, int minutes, int seconds)
{
    si_sem_wait(&Clock.Mutex);
 
    Clock.alarm_time.hours = hours; 
    Clock.alarm_time.minutes = minutes; 
    Clock.alarm_time.seconds = seconds; 

    si_sem_signal(&Clock.Mutex);

    si_sem_signal(&Clock.start_alarm);
	Clock.alarm_enabled = 1;
} 

/* increment_time: increments the current time with 
   one second */ 
void increment_time(void)
{
    /* reserve clock variables */
    si_sem_wait(&Clock.Mutex); 

    /* increment time */ 
    Clock.time.seconds++; 
    if (Clock.time.seconds > 59)
    {
        Clock.time.seconds = 0; 
        Clock.time.minutes++; 
        if (Clock.time.minutes > 59)
        {
            Clock.time.minutes = 0; 
            Clock.time.hours++; 
            if (Clock.time.hours > 11)
            {
                Clock.time.hours = 0; 
            }
        }
    }

    /* release clock variables */
    si_sem_signal(&Clock.Mutex); 
}

/* increment_alarm: increments the current alarm time with 
   ten seconds */
void increment_alarm(void)
{
    /* reserve clock variables */
    si_sem_wait(&Clock.Mutex); 

    /* Wait ten seconds before next notification*/ 
    Clock.alarm_time.seconds =  Clock.alarm_time.seconds + 10;
    if (Clock.alarm_time.seconds > 59)
    {
        Clock.alarm_time.seconds = Clock.alarm_time.seconds - 60; 
        Clock.alarm_time.minutes++; 
        if (Clock.alarm_time.minutes > 59)
        {
            Clock.alarm_time.minutes = 0; 
            Clock.alarm_time.hours++; 
            if (Clock.alarm_time.hours > 11)
            {
                Clock.alarm_time.hours = 0; 
            }
        }
    }

    /* release clock variables */
    si_sem_signal(&Clock.Mutex); 
}

/* get_time: read time from common clock variables */ 
void get_time(int *hours, int *minutes, int *seconds)
{
    /* reserve clock variables  */
    si_sem_wait(&Clock.Mutex); 

    /* read values */ 
    *hours = Clock.time.hours; 
    *minutes = Clock.time.minutes; 
    *seconds = Clock.time.seconds;
        
    /* release clock variables */ 
    si_sem_signal(&Clock.Mutex); 
}

/* get_time: read time from common clock variables */ 
void get_alarm_time(int *hours, int *minutes, int *seconds)
{
    /* reserve clock variables  */
    si_sem_wait(&Clock.Mutex); 

    /* read values */ 
    *hours = Clock.alarm_time.hours; 
    *minutes = Clock.alarm_time.minutes; 
    *seconds = Clock.alarm_time.seconds;
        
    /* release clock variables */ 
    si_sem_signal(&Clock.Mutex); 
}

/* time_from_set_message: extract time from set message from user interface */ 
void time_from_set_message(char message[], int *hours, int *minutes, int *seconds)
{
    sscanf(message,"set:%d:%d:%d",hours, minutes, seconds); 
}

/* time_from_alarm_message: extract time from set message from user interface */ 
void time_from_alarm_message(char message[], int *hours, int *minutes, int *seconds)
{
    sscanf(message,"alarm:%d:%d:%d",hours, minutes, seconds); 
}

/* time_ok: returns nonzero if hours, minutes and seconds represents a valid time */ 
int time_ok(int hours, int minutes, int seconds)
{
    return hours >= 0 && hours <= 11 && minutes >= 0 && minutes <= 59 && 
        seconds >= 0 && seconds <= 59; 
}

// Check if alarm enabled
void check_if_enable(int *enable)
{
	si_sem_wait(&Clock.start_alarm);
	*enable = Clock.alarm_enabled;
	si_sem_signal(&Clock.start_alarm);
}

// Reset and disable alarm
void reset_alarm(void)
{
	si_sem_wait(&Clock.start_alarm);
	Clock.alarm_enabled = 0;
}
