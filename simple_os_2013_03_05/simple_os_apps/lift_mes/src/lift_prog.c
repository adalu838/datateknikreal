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
#include <time.h>
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

int User_Task_Id; 
int Lift_Task_Id; 
int Move_Lift_Task_Id; 
int Lowest_Passenger_Task_Id;


/* sets task id to passenger */
int id_to_task_id(int id)
{
    return id + TASK_ID_FIRST_PERSON; 
}

/* creates random level */
int random_level(void)
{
    /* return random number between 0 and N_FLOORS-1 */
    return rand() % N_FLOORS;
}

/* tasks */ 

/* task for each passenger */
void passenger_task(void)
{
	int id, length, send_task_id;	

	si_message_receive((char *) &id, &length, &send_task_id); 

	while(1)
	{
		srand(time(NULL));
		int from_floor = random_level();
		int to_floor;
		do
		{
			to_floor = random_level();
		}
		while(from_floor == to_floor);
		lift_travel(lift, id, from_floor, to_floor);
		si_wait_n_ms(5000);
	}
}

/* task for the lift */
void lift_task(void)
{
	int next_floor, change_direction;
    
	lift = lift_create();

	draw_lift(lift);

	while(1)
	{
		lift_next_floor(lift, &next_floor, &change_direction);
		lift_move(lift, next_floor, change_direction);
		lift_has_arrived(lift);
	}
}

/* task for moving the lift */
 void move_lift_task(void)
 
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

        if (strcmp(message,"new") == 0)
        {
			
			if (n_persons < MAX_N_PERSONS)
			{
				si_task_create(passenger_task, &Passenger_Stack[n_persons][STACK_SIZE-1], 10 + n_persons);
				si_message_send((char *) &n_persons, sizeof(int), id_to_task_id(n_persons));
				n_persons = n_persons + 1;
			}
			else
			{
				si_ui_show_error("building is full");
			}
		
        }

		/* check if it is an exit message */
        else if (strcmp(message, "exit") == 0)
        {
        	exit(0);
		}
                
    	else
		{
 			si_ui_show_error("unexpected message type"); 
		}
		si_wait_n_ms(500); 
	}
}

/* main */                          //ÄNDRAD!!!!!!!!!!
int main(void)
{
    /* initialise kernel */ 
    si_kernel_init();

	/* initialise message handling */ 
    si_message_init();
    
    /* initialise UI channel */ 
    si_ui_init(); 
	
	/* initialise random number generator */
    initialise_random();

    /* create tasks */ 

    si_task_create(lift_task, &Lift_Stack[STACK_SIZE - 1], LIFT_PRIORITY);

    Lift_Task_Id = 1; 

    si_task_create(move_lift_task, &Move_Lift_Stack[STACK_SIZE - 1], MOVE_LIFT_PRIORITY);

    Move_Lift_Task_Id = 2; 

    si_task_create(user_task, &User_Stack[STACK_SIZE - 1], USER_PRIORITY);

    User_Task_Id = 3; 

    Lowest_Passenger_Task_Id = 4; 

    /* start the kernel */ 
    si_kernel_start(); 

    /* will never be here! */ 
    return 0; 
}
