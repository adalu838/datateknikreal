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

/* message types for lift simulation */ 

/* a move message is sent to the lift task when the 
   lift shall move to the next floor */ 
#define MOVE_MESSAGE 0

/* a travel message is sent to the lift task when a 
   person would like to make a lift travel */ 
#define TRAVEL_MESSAGE 1

/* a travel done message is sent to a person task when 
   a lift travel is finished */ 
#define TRAVEL_DONE_MESSAGE 2

/* set id of first passenger */
#define TASK_ID_FIRST_PERSON 4

/* fig_end lift_h_defs */ 

/* data structure for messages */ 
typedef struct
{
    /* message type */
    int type; 
    /* person id */ 
    int id; 
    /* source and destination of lift travel */ 
    int from_floor; 
    int to_floor; 
} message_data_type;

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

/* special numbers, to define no identity and no destination */ 
#define NO_ID -1
#define NO_FLOOR -1

/* fig_begin lift_mes_type */ 
/* definition data type for lift */

typedef struct
{
    /* the floor where the lift is positioned */ 
    int floor; 
	
    /* variable to indicate if the lift is travelling in the up 
       direction, which is defined as the direction where the 
       floor number is increasing */
    int up;

    /* persons on each floor waiting to enter */ 
    person_data_type persons_to_enter[N_FLOORS][MAX_N_PERSONS];

    /* passengers in the lift */
    person_data_type passengers_in_lift[MAX_N_PASSENGERS];

} lift_data_type;

typedef lift_data_type* lift_type;
/* fig_end lift_mes_type */ 

/* lift_create: creates and initialises a variable of type lift_type */
lift_type lift_create(void); 

/* lift_delete: deallocates memory for lift */
void lift_delete(lift_type lift); 

/* fig_begin mes_functions */ 
/* lift_next_floor: computes the floor to which 
   the lift shall travel. The parameter *change_direction 
   indicates if the direction shall be changed */
void lift_next_floor(lift_type lift, int *next_floor, int *change_direction); 

/* lift_move: makes the lift move from its current 
   floor to next_floor. The parameter change_direction indicates if 
   the move includes a change of direction. */
void lift_move(lift_type lift, int next_floor, int change_direction); 

/* get_current_floor: returns the floor on which the lift is positioned */ 
int get_current_floor(lift_type lift);

/* enter_floor: makes the person with id id and destination to_floor stand 
   at floor floor */
void enter_floor(lift_type lift, int id, int floor, int to_floor);

/* leave_floor: makes a person with id id at enter_floor leave 
   enter_floor */ 
void leave_floor(lift_type lift, int id, int enter_floor);

/* enter_lift: makes the person with id id and destination to_floor 
   enter the lift */ 
void enter_lift(lift_type lift); 

/* leave_lift: makes a person, standing inside the lift and having 
   destination floor equal to floor, leave the lift. The id of the 
   person is returned in the parameter *id */ 
void leave_lift(lift_type lift);

/* n_passengers_to_leave: returns the number of passengers in the 
   lift having the destination floor equal to floor */
int n_passengers_to_leave(lift_type lift, int floor); 

/* n_persons_to_enter: returns the number of persons standing on 
   floor floor */ 
int n_persons_to_enter(lift_type lift, int floor);

/* lift_is_full: returns nonzero if the lift is full, returns zero 
   otherwise */ 
int lift_is_full(lift_type lift); 

int id_to_task_id(int id);

#endif
