/* This file is part of Simple_OS, a real-time operating system  */
/* designed for research and education */
/* Copyright (c) 2003-2013 Ola Dahl */

/* The software accompanies the book Into Realtime, available at  */
/* http://theintobooks.com */

/* Simple_OS is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* This program is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include "tcb_list.h"
#include "tcb.h"
#include "task_id_list.h"

void tcb_list_reset(task_control_block tcb_list[], int length)
{
    int i; 
    /* reset all elements of the list */ 
    for (i = 0; i < length; i++) 
    {
        tcb_reset(&tcb_list[i]); 
    }
}

/* tcb_list_insert: inserts tcb in tcb_list, and returns 
   the task_id for the task to which the tcb belongs */ 
int tcb_list_insert(
    task_control_block tcb_list[], int length,
    task_control_block tcb)
{
    int i; /* loop counter */ 
    int found; /* flag to indicate a found element */ 
    int found_index; /* index for found element */ 
 
    /* no element is found */ 
    found = 0; 
    /* look in tcb_list for first invalid element */ 
    for (i = 0; i < length && !found; i++)
    {
        if (!tcb_is_valid(&tcb_list[i]))
        {
            /* we have found an invalid element */ 
            found = 1; 
            /* copy task control block */ 
            tcb_list[i] = tcb;
            /* the element is now valid */ 
            tcb_set_valid(&tcb_list[i]); 
            /* set task_id equal to index */ 
            tcb_set_task_id(&tcb_list[i], i); 
            /* the index found */ 
            found_index = i; 
        }
    }
    if (found) /* an index was found, return it */ 
    {
        return found_index; 
    }
    else /* no index was found, return invalid TASK_ID */ 
    {
        return TASK_ID_INVALID; 
    }
}

/* change the dependence on this number! TBD */ 
#define MAX_PRIORITY_VALUE 10000

int tcb_list_get_task_id_highest_prio(
    task_control_block tcb_list[], int tcb_list_length, 
    int task_id_list[], int task_id_list_length)
{
    /* loop counter for task_id_list */  
    int task_id_index; 
    
    /* current priority and task_id */ 
    int current_priority; 
    int current_task_id; 

    /* smallest priority value found */ 
    int min_priority_value = MAX_PRIORITY_VALUE; 

    /* task_id for process with smallest priority value */
    int task_id_min_priority_value; 

    /* set to true if no valid task_id and TCB are found */ 
    int no_valid_task_id_tcb = 1; 
 
    /* go through task_id_list */ 
    for (task_id_index = 0; task_id_index < task_id_list_length; task_id_index++) 
    {
        /* get the task_id */ 
        current_task_id = task_id_list[task_id_index];

        /* check if valid task_id */  
        if (current_task_id != TASK_ID_INVALID)
        {
            /* check if valid tcb */   
            if (tcb_is_valid(&tcb_list[current_task_id]))
            {
                /* one valid task_id and TCB combination is found */ 
                if (no_valid_task_id_tcb)
                {
                    no_valid_task_id_tcb = 0; 
                }
                /* get the priority */ 
                current_priority = tcb_list[current_task_id].priority;
                /* compare */ 
                if (current_priority < min_priority_value)
                {
                    /* a new minumun value is found */ 
                    min_priority_value = current_priority; 
                    /* record current task_id */ 
                    task_id_min_priority_value = current_task_id; 
                }
            }
        }
    }
    /* return the value found */ 
    if (no_valid_task_id_tcb)
    {
        return TASK_ID_INVALID; 
    }
    else
    {
        return task_id_min_priority_value; 
    }
}

int tcb_list_get_task_id_highest_prio_not_quantum(
	task_control_block tcb_list[], int tcb_list_length, 
    int task_id_list[], int task_id_list_length)
{
	/* loop counter for task_id_list */  
    int task_id_index; 
    
    /* current priority and task_id */ 
    int current_priority; 
    int current_task_id;

	/* current run time */
	int run_time;

    /* smallest priority value found */ 
    int min_priority_value = MAX_PRIORITY_VALUE; 

    /* task_id for process with smallest priority value */
    int task_id_min_priority_value; 

    /* set to true if no valid task_id and TCB are found */ 
    int no_valid_task_id_tcb = 1; 
 
	/* check if all TS tasks exceeded quantum, if so reset run_time */
	if (!tcb_list_check_ts_all_quantum(tcb_list, tcb_list_length, task_id_list, task_id_list_length))
	{
		tcb_list_reset_run_time(tcb_list, tcb_list_length, task_id_list, task_id_list_length);
	}
 
    /* go through task_id_list */ 
    for (task_id_index = 0; task_id_index < task_id_list_length; task_id_index++) 
    {
        /* get the task_id */ 
        current_task_id = task_id_list[task_id_index];

        /* check if valid task_id */  
        if (current_task_id != TASK_ID_INVALID)
        {
            /* check if valid tcb */   
            if (tcb_is_valid(&tcb_list[current_task_id]))
            {
                /* one valid task_id and TCB combination is found */ 
                if (no_valid_task_id_tcb)
                {
                    no_valid_task_id_tcb = 0; 
                }
                /* get the priority and run time*/ 
                current_priority = tcb_list[current_task_id].priority;
				run_time = tcb_list[current_task_id].run_ticks;
                /* compare priority and run time*/ 
                if (current_priority < min_priority_value && run_time < MAX_RUN_TICKS)
                {
                    /* a new minumun value is found */ 
                    min_priority_value = current_priority; 
                    /* record current task_id */ 
                    task_id_min_priority_value = current_task_id; 
                }
            }
        }
    }
    /* return the value found */ 
    if (no_valid_task_id_tcb)
    {
        return TASK_ID_INVALID; 
    }
    else
    {
        return task_id_min_priority_value; 
    }
}

#include "console.h"

void tcb_list_decrement_timers(
    task_control_block tcb_list[], int tcb_list_length, 
    int task_id_list[], int task_id_list_length, 
    int *n_timers_set_to_zero, int task_ids_set_to_zero[])
{
    /* index into task_id_list */ 
    int task_id_index; 
    
    /* current task_id */ 
    int current_task_id; 

    int n_set_to_zero = 0; 

    /* go through task_id_list */ 
    for (task_id_index = 0; task_id_index < task_id_list_length; task_id_index++) 
    {
        /* get the task_id */ 
        current_task_id = task_id_list[task_id_index];
        /* check if valid task_id */  
        if (current_task_id != TASK_ID_INVALID)
        {
            /* check if valid tcb */   
            if (tcb_is_valid(&tcb_list[current_task_id]))
            {
                if (tcb_list[current_task_id].wait_ticks > 0)
                {
                    tcb_list[current_task_id].wait_ticks--; 

                    //console_put_string("tid: ");
                    //console_put_hex(current_task_id); 
                    //console_put_string("new ticks: ");  
                    //console_put_hex(tcb_list[current_task_id].wait_ticks);

                    if (tcb_list[current_task_id].wait_ticks == 0)
                    {
                        //console_put_string("END WAIT for tid: "); 
                        //console_put_hex(current_task_id);  
                        task_ids_set_to_zero[n_set_to_zero] = current_task_id; 
                        n_set_to_zero++; 
                    }
                }
            }
        }
    }
    *n_timers_set_to_zero = n_set_to_zero; 
}

void tcb_list_increment_run_timer(task_control_block tcb_list[], int tcb_list_length, 
    int task_id_list[], int task_id_list_length, 
    int *n_timers_set_to_zero, int task_ids_set_to_zero[])
{
	/* current task_id */ 
    int current_task_id;

	/* get the task_id */ 
    current_task_id = task_get_task_id_running();
	
    /* check if valid task_id */  
    if (current_task_id != TASK_ID_INVALID)
    {
    	/* check if valid tcb */   
        if (tcb_is_valid(&tcb_list[current_task_id]))
        {	
			/* increment run timer */
        	tcb_list[current_task_id].run_ticks++;
      	}
	}
}

int tcb_list_has_real_time_task(
    task_control_block tcb_list[], int tcb_list_length, 
    int task_id_list[], int task_id_list_length)
	{
   		int id_index; 
    	int task_id; 
    	int real_time_task_found = 0; 
		
		/* iterate through the task id list */
    	for (id_index = 0; id_index < task_id_list_length; id_index++) 
    	{
        	task_id = task_id_list[id_index];
			/* check if valid id */
        	if (task_id != TASK_ID_INVALID)
        	{
            	if (tcb_is_valid(&tcb_list[task_id]))
            	{	
					/* check if real time task, if so return true */
                	if (!real_time_task_found)
                	{
                    	real_time_task_found = 
                    	tcb_is_real_time_task(&tcb_list[task_id]); 
                	}
            	}
        	}
    	}
    return real_time_task_found; 
}

int tcb_list_check_ts_all_quantum(
    task_control_block tcb_list[], int tcb_list_length, 
    int task_id_list[], int task_id_list_length)
{
	int id_index; 
	int task_id; 
    int quantum_time_shared_task_found = 0; 
 
	/* iterate through the task id list */
    for (id_index = 1; id_index < task_id_list_length; id_index++) 
    {
		task_id = task_id_list[id_index];
		/* ignore the real time tasks */
		if (!tcb_is_real_time_task(&tcb_list[task_id]))
		{
			/* check if valid id */
			if (task_id != TASK_ID_INVALID)
			{
				if (tcb_is_valid(&tcb_list[task_id]))
				{
					/* check if run time exceeded quantum time */
					if (tcb_list[task_id].run_ticks < MAX_RUN_TICKS)
					{
						quantum_time_shared_task_found++;
					}
				}	
			}
		}
    }
	/* return number of ts tasks that have not exceeded quantum time */
    return quantum_time_shared_task_found; 
}

void tcb_list_reset_run_time(
    task_control_block tcb_list[], int tcb_list_length, 
    int task_id_list[], int task_id_list_length)
{
	int id_index; 
	int task_id; 
	/* iterate through the task id list */
	for (id_index = 0; id_index < task_id_list_length; id_index++) 
	{
		task_id = task_id_list[id_index];
		/* ignore the real time tasks */
		if (!tcb_is_real_time_task(&tcb_list[task_id]))
		{
			/* check if valid id */
			if (task_id != TASK_ID_INVALID)
			{
				if (tcb_is_valid(&tcb_list[task_id]))
				{	
					/* reset run time for ts tasks */
					tcb_list[task_id].run_ticks = 0;
				}	
			}
		}
	}
}
