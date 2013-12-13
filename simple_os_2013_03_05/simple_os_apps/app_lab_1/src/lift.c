#include "lift.h"

/* Simple_OS include */ 
#include <simple_os.h>

/* drawing module */ 
#include "draw.h"

/* standard includes */ 
#include <stdlib.h>
#include <stdio.h>

/* panic function, to be called when fatal errors occur  
static void lift_panic(const char message[])
{
    printf("LIFT_PANIC!!! "); 
    printf("%s", message); 
    printf("\n"); 
    exit(0); 
}*/

/* --- monitor data type for lift and operations for create and delete START --- */



void init_lift(lift_type lift)
{
    /* initialise variables */
	
	/* start writing at first position */ 
    lift->in_pos = 0;
    /* start reading at first position */ 
    lift->out_pos = 0;
    /* no elements are stored in the buffer */ 
    lift->count = 0; 
	
    /* initialise floor */
    lift->floor = 0; 
    
    /* set direction of lift travel to up */
    lift->up = 1;

    /* the lift is not moving */ 
    lift->moving = 0; 

	/* loop counter */
    int i;

    /* floor counter */ 
    int floor; 

    /* initialise person information */
    for (floor = 0; floor < N_FLOORS; floor++)
    {
        for (i = 0; i < MAX_N_PERSONS; i++)
        {
            lift->persons_to_enter[floor][i].id = NO_ID; 
            lift->persons_to_enter[floor][i].to_floor = NO_FLOOR; 
        }
    }
	
    /* initialise passenger information */
    for (i = 0; i < MAX_N_PASSENGERS; i++) 
    {
        lift->passengers_in_lift[i].id = NO_ID; 
        lift->passengers_in_lift[i].to_floor = NO_FLOOR; 
    }

    /* initialise semaphore and event variable */
    si_sem_init(&lift->mutex, 1); 
    si_cv_init(&lift->change, &lift->mutex); 

}

/* lift_delete: deallocates memory for lift */
void lift_delete(lift_type lift)
{
  //Här ska minne frigöras
}


/* --- monitor data type for lift and operations for create and delete END --- */


/* --- functions related to lift task START --- */

/* MONITOR function lift_next_floor: computes the floor to which the lift 
   shall travel. The parameter *change_direction indicates if the direction 
   shall be changed */
void lift_next_floor(lift_type lift, int *next_floor, int *change_direction)
{
	si_sem_wait(&lift->mutex);
	//printf("%s\n","Next");
	
	*change_direction = 0;

	if (lift->floor == 0 && lift->up)
	{
		*change_direction = 1;
		*next_floor = 1;
	}
	else if (lift->floor == 4 && !lift->up)
	{
		*change_direction = 1;
		*next_floor = 3;	
	}
	else if (lift->up)
	{
		*next_floor = lift->floor - 1;
	}
	else if (!lift->up)
	{
		*next_floor = lift->floor + 1;
	}

	si_sem_signal(&lift->mutex);
}

void lift_move(lift_type lift, int next_floor, int change_direction)
{
    /* reserve lift */ 
    si_sem_wait(&lift->mutex); 

    /* the lift is moving */ 
    lift->moving = 1; 
        
    /* release lift */ 
    si_sem_signal(&lift->mutex); 
        
    /* it takes two seconds to move to the next floor */ 
    si_wait_n_ms(2000);
	//printf("%s\n","Arrive1"); 
        
    /* reserve lift */ 
    si_sem_wait(&lift->mutex);

	//printf("%s\n","Arrive2");  
        
    /* the lift is not moving */ 
    lift->moving = 0; 

    /* the lift has arrived at next_floor */ 
    lift->floor = next_floor; 

    /* check if direction shall be changed */ 
    if (change_direction)
    {
        lift->up = !lift->up; 
    }

    /* draw, since a change has occurred */ 
    draw_lift(lift); 

    /* release lift */ 
    si_sem_signal(&lift->mutex); 
}

/* this function is used also by the person tasks */ 
static int n_passengers_in_lift(lift_type lift)
{
    int n_passengers = 0; 
    int i; 
        
    for (i = 0; i < MAX_N_PASSENGERS; i++)
    {
        if (lift->passengers_in_lift[i].id != NO_ID)
        {
            n_passengers++; 
        }
    }
    return n_passengers; 
}

/* MONITOR function lift_has_arrived: shall be called by the lift task
   when the lift has arrived at the next floor. This function indicates
   to other tasks that the lift has arrived, and then waits until the lift
   shall move again. */
void lift_has_arrived(lift_type lift)
{
	si_sem_wait(&lift->mutex);

	si_cv_broadcast(&lift->change);
	
	while(passengers_enter(lift) || passengers_exit(lift))
	{
		si_cv_wait(&lift->change); 
	}

	si_sem_signal(&lift->mutex);
}

/* --- functions related to lift task END --- */


/* --- functions related to person task START --- */

/* passenger_wait_for_lift: returns true if passenger shall
   wait, otherwise false */
int passenger_wait_for_lift(lift_type lift, int wait_floor)
{
	return lift->moving || lift->floor != wait_floor || n_passengers_in_lift(lift) == MAX_N_PASSENGERS;
}

/* enter_floor: makes a person with id id stand at floor floor */ 
void enter_floor(lift_type lift, int id, int floor)
{
    int i; 
    int floor_index; 
    int found; 

    /* stand at floor */ 
    found = 0; 
    for (i = 0; i < MAX_N_PERSONS && !found; i++)
    {
        if (lift->persons_to_enter[floor][i].id == NO_ID)
        {
            found = 1; 
            floor_index = i; 
        }
    }
        
    if (!found)
    {
        //lift_panic("cannot enter floor"); 
    }

    /* enter floor at index floor_index */ 
    lift->persons_to_enter[floor][floor_index].id = id; 
    lift->persons_to_enter[floor][floor_index].to_floor = NO_FLOOR; 
}

/* leave_floor: makes a person with id id at enter_floor leave 
   enter_floor */ 
void leave_floor(lift_type lift, int id, int enter_floor)
{
    int i; 
    int floor_index; 
    int found; 

    /* leave the floor */
    found = 0; 
    for (i = 0; i < MAX_N_PERSONS && !found; i++)
    {
        if (lift->persons_to_enter[enter_floor][i].id == id)
        {
            found = 1; 
            floor_index = i; 
        }
    }
        
    if (!found)
    {
        //lift_panic("cannot leave floor"); 
    }

    /* leave floor at index floor_index */ 
    lift->persons_to_enter[enter_floor][floor_index].id = NO_ID; 
    lift->persons_to_enter[enter_floor][floor_index].to_floor = NO_FLOOR; 
}

/* MONITOR function lift_travel: performs a journey with the lift
   starting at from_floor, and ending at to_floor */ 
void lift_travel(lift_type lift, int id, int from_floor, int to_floor)
{
	si_sem_wait(&lift->mutex);
	enter_floor(lift, id, from_floor);
	draw_lift(lift);

	while(passenger_wait_for_lift(lift, from_floor))
	{
		si_cv_wait(&lift->change);
	}

	leave_floor(lift, id, from_floor);

	enter_lift(lift, id, to_floor);
	
	draw_lift(lift);
	
	si_cv_broadcast(&lift->change);

	while(lift->moving || lift->floor != to_floor)
	{
		si_cv_wait(&lift->change);
	}

	leave_lift(lift, to_floor, &id);
	
	draw_lift(lift);
	
	si_cv_broadcast(&lift->change);

	si_sem_signal(&lift->mutex);
}

/* enter_lift: makes the person with id id and destination to_floor 
   enter the lift */ 
void enter_lift(lift_type lift, int id, int to_floor)
{
	int i;

	for (i = 0; i < MAX_N_PERSONS; i++)
    {
        if (lift->passengers_in_lift[i].id == NO_ID)
        {
			lift->passengers_in_lift[i].id = id;
			lift->passengers_in_lift[i].to_floor = to_floor;
			break;
        }
    }
} 

/* leave_lift: makes a person, standing inside the lift and having 
   destination floor equal to floor, leave the lift. The id of the 
   person is returned in the parameter *id */ 
void leave_lift(lift_type lift, int floor, int *id)
{
	int i;

	for (i = 0; i < MAX_N_PERSONS; i++)
    {
        if (lift->passengers_in_lift[i].id == *id && lift->passengers_in_lift[i].to_floor == floor)
        {
			lift->passengers_in_lift[i].id = NO_ID;
			lift->passengers_in_lift[i].to_floor = NO_FLOOR;
        }
    }
}

/* passengers_exit: returns number of passengers waiting to exit lift 
	INTE KLAR?*/ 
int passengers_exit(lift_type lift)
{
	int n_passengers = 0, floor, i;
	floor = lift->floor;
	
	for(i = 0; i < MAX_N_PASSENGERS; i++)
	{
		if (lift->passengers_in_lift[i].to_floor == floor)
		{
			n_passengers++;
		}
	}
	
	return n_passengers;
}

/* passengers_enter: returns number of passengers waiting to enter lift */ 
int passengers_enter(lift_type lift)
{
	int n_passengers = 0, floor, i;
	floor = lift->floor;

	for(i = 0; i < MAX_N_PERSONS; i++)
	{
		if (lift->persons_to_enter[floor][i].id != NO_ID && MAX_N_PASSENGERS != n_passengers_in_lift(lift))
		{
			n_passengers++;
		}
	}
	return n_passengers;
}

/* --- functions related to person task END --- */
