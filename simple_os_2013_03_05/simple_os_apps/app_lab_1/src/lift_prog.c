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
#include "lift.h"
#include "draw.h"

/* stack size */ 
#define STACK_SIZE 5000

/* set id of first passenger */
#define TASK_ID_FIRST_PERSON 3

/* stacks */

/* stack for lift_task */ 
stack_item Lift_Stack[STACK_SIZE]; 

/* stack for user_task */ 
stack_item User_Stack[STACK_SIZE];

/* stacks for passenger tasks */
stack_item Passenger_Stack[MAX_N_PERSONS][STACK_SIZE];

/* Init lift */
lift_type lift;


/* sets task id to passenger */
int id_to_task_id(int id)
{
    return id + TASK_ID_FIRST_PERSON; 
}

/* tasks */ 

/* task for each passenger */
void passenger_task(void)
{
	int id, length, send_task_id, n_travels = 0;	

	si_message_receive((char *) &id, &length, &send_task_id); 

	while(n_travels < 2)
	{
		int from_floor = 0;
		int to_floor = 3;
		
		/* make the journey */
		lift_travel(lift, id, from_floor, to_floor);
		
		n_travels++; 

        /* sleep for a while */
        si_wait_n_ms(5000); 
	}
}

/* create_passenger: create a person task */ 
void create_passenger(int id, int priority) 
{
    int task_id; 

    /* create task */ 
    task_id = si_task_create_task_id(
        passenger_task, &Passenger_Stack[id][STACK_SIZE - 1], priority);

    /* send id message to created task */ 
    si_message_send((char *) &id, sizeof(int), task_id); 
}

/* task for the lift */
void lift_task(void)
{
	int next_floor, change_direction;

	draw_lift(lift);

	while(1)
	{
		lift_next_floor(lift, &next_floor, &change_direction);
		lift_move(lift, next_floor, change_direction);
		lift_has_arrived(lift);
	}
}

/* task for user communication */
void user_task(void)
{

	/* message array */ 
    char message[SI_UI_MAX_MESSAGE_SIZE];
	int n_persons = 0;

    /* set size of GUI window */ 
    si_ui_set_size(670, 700);

	while(1)
	{
		/* read a message */ 
        si_ui_receive(message); 

        if (si_string_compare(message,"new") == 0)
        {
			
			if (n_persons < MAX_N_PERSONS)
			{
				create_passenger(id_to_task_id(n_persons), 10 + n_persons);
				n_persons = n_persons + 1;
			}
			else
			{
				si_ui_show_error("building is full");
			}
		
        }

		/* check if it is an exit message */
        else if (si_string_compare(message, "exit") == 0)
        {
        	//Här ska det avslutas
		}
                
    	else
		{
 			si_ui_show_error("unexpected message type"); 
		}
		si_wait_n_ms(500); 
	}
}

/* main */ 
int main(void)
{
    /* initialise kernel */ 
    si_kernel_init();

	/* initialise message handling */ 
    si_message_init();
    
    /* initialise UI channel */ 
    si_ui_init(); 

	/* create lift */ 
	lift = lift_create();
	
	/* init lift */
	init_lift(&lift);

    /* create tasks */ 
    si_task_create(lift_task, &Lift_Stack[STACK_SIZE-1], 30); 
    si_task_create(user_task, &User_Stack[STACK_SIZE-1], 5);

    /* start the kernel */ 
    si_kernel_start(); 

    /* will never be here! */ 
    return 0; 
}
