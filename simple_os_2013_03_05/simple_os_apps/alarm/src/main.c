/*Che
eee
ese
*/

/* Simple_OS include */ 
#include <simple_os.h>
#include <stdio.h>

/* Standard library includes */ 
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "clock.h"
#include "display.h"

/* stack size */ 
#define STACK_SIZE 5000 

/* stack for clock_task */ 
stack_item Clock_Stack[STACK_SIZE]; 

/* stack for set_task */ 
stack_item Set_Stack[STACK_SIZE]; 

/* stack for alarm_task */ 
stack_item Alarm_Stack[STACK_SIZE];

/* tasks */ 

/* clock_task: clock task */ 
void clock_task(void) 
{

    si_time next_time;
    /* local copies of the current time */ 
    int alarm_hours, alarm_minutes, alarm_seconds, hours, minutes, seconds, enable; 
    si_get_current_time(&next_time);
    /* infinite loop */ 
    while (1)
    {
        /* increment time */
		//printf("%d\n",1);
		increment_time();

        /* read and display current time */ 
		get_time(&hours, &minutes, &seconds, &enable);
		get_alarm_time(&alarm_hours, &alarm_minutes, &alarm_seconds);
		display_time(hours, minutes, seconds);
		
		if (enable && alarm_hours == hours && alarm_minutes == minutes && alarm_seconds == seconds)
		{
			enable_alarm();
		}
		si_time_add_n_ms(&next_time,1000);
        /* wait one second */ 
		si_wait_until_time(&next_time);
    }
}

/* alarm_task: alarm task */ 
void alarm_task(void) 
{
    /* infinite loop */ 
    while (1)
    {	
	alarm_wait();

    display_alarm_text();
	increment_alarm();
    /* wait one second */ 
	si_wait_n_ms(1000);
    }
}

/* set_task: reads messages from the user interface... */ 
void set_task(void)
{
    /* message array */ 
    char message[SI_UI_MAX_MESSAGE_SIZE]; 

    /* time read from user interface */ 
    int hours = 0, minutes = 0, seconds = 0; 

    /* set GUI size */ 
    si_ui_set_size(400, 200); 

    while(1)
    {
        /* read a message */ 
        si_ui_receive(message); 
        /* check if it is a set message */ 
        if (strncmp(message, "set:", 4) == 0)
        {
            time_from_set_message(message, &hours, &minutes, &seconds); 
            if (time_ok(hours, minutes, seconds))
            {
                set_time(hours, minutes, seconds); 
            }
            else
            {
                si_ui_show_error("Illegal value for hours, minutes or seconds"); 
            }
        }
        /* check if it is an alarm message */ 
	else if (strncmp(message, "alarm:", 6) == 0)
	{
	    time_from_alarm_message(message, &hours, &minutes, &seconds);
	    if (time_ok(hours, minutes, seconds))
            {
                set_alarm(hours, minutes, seconds);
				display_alarm_time(hours,minutes,seconds);
            }
            else
            {
                si_ui_show_error("Illegal value for hours, minutes or seconds"); 
            }
	}
	/* check if it is an reset message */ 
	else if (strncmp(message, "reset", 5) == 0)
	{	
		erase_alarm_time();
		erase_alarm_text();
		reset_alarm();
	}
	/* check if it is an exit message */ 
        else if (strcmp(message, "exit") == 0)
        {
            exit(0); 
        }
        /* not a legal message */ 
        else 
        {
            si_ui_show_error("unexpected message type"); 
        }
    }
}

/* main */ 
int main(void)
{
    /* initialise kernel */ 
    si_kernel_init(); 
    
    /* initialise UI channel */ 
    si_ui_init(); 

    /* initialise variables */         
    init_clock();

    /* initialise display */
    display_init();

    /* create tasks */ 
    si_task_create(clock_task, &Clock_Stack[STACK_SIZE-1], 20); 
    si_task_create(set_task, &Set_Stack[STACK_SIZE-1], 15);
    si_task_create(alarm_task, &Alarm_Stack[STACK_SIZE-1], 10);

    /* start the kernel */ 
    si_kernel_start(); 

    /* will never be here! */ 
    return 0; 
}
