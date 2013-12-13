#ifndef LIFT_H
#define LIFT_H

#include <simple_os.h>

/* fig_begin lift_h_defs */ 
/* size of building */ 
#define N_FLOORS 5

/* maximum number of persons in the lift system */ 
#define MAX_N_PERSONS 10

/* maximum number of passengers in lift */ 
#define MAX_N_PASSENGERS 5

/* special numbers, to define no identity and no destination */ 
#define NO_ID -1
#define NO_FLOOR -1

/* fig_end lift_h_defs */ 

/* fig_begin person_data_type */ 
/* data structure for person information */ 
typedef struct
{
    /* identity */ 
    int id; 
    /* destination floor */ 
    int to_floor; 

} person_data_type; 
/* fig_end person_data_type */ 

/* fig_begin lift_mon_type */ 
/* definition of monitor data type for lift */
typedef struct
{
    /* the floor where the lift is positioned */ 
    int floor; 

    /* a flag to indicate if the lift is moving */ 
    int moving; 

    /* variable to indicate if the lift is travelling in the up 
       direction, which is defined as the direction where the 
       floor number is increasing */
    int up;
	
	/* start writing at first position */ 
	int in_pos;
    /* start reading at first position */ 
    int out_pos;
    /* no elements are stored in the buffer */ 
    int count;

    /* persons on each floor waiting to enter */ 
    person_data_type persons_to_enter[N_FLOORS][MAX_N_PERSONS];

    /* passengers in the lift */
    person_data_type passengers_in_lift[MAX_N_PASSENGERS];

    /* semaphore for mutual exclusion */
    si_semaphore mutex; 

    /* condition variable, to indicate that something has happend */ 
    si_condvar change; 

} lift_data_type;

typedef lift_data_type* lift_type;
/* fig_end lift_mon_type */ 

/* lift_create: creates and initialises a variable of type lift_type */
//lift_type lift_create(void); 

void init_lift(lift_type lift);

/* lift_delete: deallocates memory for lift */
void lift_delete(lift_type lift); 

/* fig_begin mon_functions */ 
/* MONITOR function lift_next_floor: computes the floor to which 
   the lift shall travel. The parameter *change_direction 
   indicates if the direction shall be changed */
void lift_next_floor(lift_type lift, int *next_floor, int *change_direction); 

/* MONITOR function lift_move: makes the lift move from its current 
   floor to next_floor. The parameter change_direction indicates if 
   the move includes a change of direction. This function shall be 
   called by the lift process when the lift shall move */ 
void lift_move(lift_type lift, int next_floor, int change_direction); 

/* MONITOR function lift_has_arrived: shall be called by the lift 
   process when the lift has arrived at the next floor. This function 
   indicates to other processes that the lift has arrived, and then waits 
   until the lift shall move again. */
void lift_has_arrived(lift_type lift); 

int passenger_wait_for_lift(lift_type lift, int wait_floor);

/* enter_floor: makes a person with id id stand at floor floor */ 
void enter_floor(lift_type lift, int id, int floor);

/* leave_floor: makes a person with id id at enter_floor leave 
   enter_floor */ 
void leave_floor(lift_type lift, int id, int enter_floor);

/* MONITOR function lift_travel: makes the person with id id perform 
   a journey with the lift, starting at from_floor and ending 
   at to_floor */ 
void lift_travel(lift_type lift, int id, int from_floor, int to_floor);
/* fig_end mon_functions */ 

/* enter_lift: makes the person with id id and destination to_floor 
   enter the lift */ 
void enter_lift(lift_type lift, int id, int to_floor); 

/* leave_lift: makes a person, standing inside the lift and having 
   destination floor equal to floor, leave the lift. The id of the 
   person is returned in the parameter *id */ 
void leave_lift(lift_type lift, int floor, int *id);

/* passengers_exit: returns number of passengers waiting to exit lift */ 
int passengers_exit(lift_type lift);

/* passengers_enter: returns number of passengers waiting to enter lift */ 
int passengers_enter(lift_type lift);


#endif
