#include "restaurant.h"
#include <stdio.h>
#include <semaphore.h>
#include <sys/queue.h>


// You can declare global variables here

int total_tables = 0;
int table_index = 0;
int group_queue_number = 0;
int table_quantity[5];

sem_t table_semaphores[5];
sem_t group_semaphores[1000];

// table numbers array
int table_num[5][1000];
// tables occupied
int table_occupied_status[5][1000];

struct group_state {
	int value;
	TAILQ_ENTRY(group_state) entries;
}; 

TAILQ_HEAD(, group_state) my_tailq_head;



void restaurant_init(int num_tables[5]) {
    // Write initialization code here (called once at the start of the program).
    // It is guaranteed that num_tables is an array of length 5.
    // TODO


    TAILQ_INIT(&my_tailq_head); 

    // init 1000 cell arrays to -1
    for (int i = 0; i < 5; i++) {
        for (int a = 0; a < 1000; a++) {
            table_num[i][a] = -1;
            table_occupied_status[i][a] = -1;
        }
    }

    
    for (int i = 0; i < 5; i++) {
        for (int c = 0; c < num_tables[i]; c++) {
            table_num[i][c] = total_tables;
            table_occupied_status[i][c] = 0;
            total_tables++;
        }
    }

    for (int i = 0; i < 5; i++) {
        table_quantity[i] = num_tables[i];
    }

    // init semaphores
    for (int i = 0; i < 1000; i++) {
        sem_init(&group_semaphores[i], 0, 0);
    }

    // group semaphores
    sem_init(&table_semaphores[0], 0, 1);
    sem_init(&table_semaphores[1], 0, 1);
    sem_init(&table_semaphores[2], 0, 1);
    sem_init(&table_semaphores[3], 0, 1);


}

void restaurant_destroy(void) {
    // Write deinitialization code here (called once at the end of the program).
    // TODO

    for (int i = 0; i < 1000; i++) {
        sem_destroy(&group_semaphores[i]);
    }

    sem_destroy(&table_semaphores[0]);
    sem_destroy(&table_semaphores[1]);
    sem_destroy(&table_semaphores[2]);
    sem_destroy(&table_semaphores[3]);
}

int request_for_table(group_state *state, int num_people) {
    // Write your code here.
    // Return the id of the table you want this group to sit at.
    // TODO

    num_people--;
    on_enqueue();

    sem_wait(&table_semaphores[0]);
    state->num_people = num_people;
    state->queue_num = group_queue_number;
    state->internal_sem = &group_semaphores[group_queue_number];
    
    group_queue_number++;

    int result_table_num = 0;
    for (int t = state->num_people; t < 5; t++) {
        for (int i = 0; i < table_quantity[t]; i++) {
            if (table_occupied_status[t][i] == 0) {
                table_occupied_status[t][i] = 1;
                result_table_num = table_num[t][i];

                state->table_index = i;
                state->table_num = result_table_num;
                state->occupied_table_capacity = t;

                sem_post(&table_semaphores[0]);

                return result_table_num;

            }
        }
    }

    TAILQ_INSERT_TAIL(&my_tailq_head, state, entries); 
    // printf("entered queue %d\n", state->num_people);
    
    sem_post(&table_semaphores[0]);



    sem_wait(state->internal_sem);

    // table_occupied_status[state->occupied_table_capacity][state->table_index] = 1;

    return state->table_num;
}

void leave_table(group_state *state) {
    // Write your code here.
    // TODO
    
    // extract out current state semaphore
    sem_t *tmp_sem_ptr = &state->internal_sem;

    sem_wait(&table_semaphores[1]);
    // table_occupied_status[state->occupied_table_capacity][state->table_index] = 0;


    // int result_table_num = 0;
    // TAILQ_FOREACH(tmp_state, &my_tailq_head, entries) {
    
    // struct group_state *curr_state;
    group_state *curr_state;
    group_state *next_state;
    for (curr_state = TAILQ_FIRST(&my_tailq_head); curr_state != NULL; curr_state = next_state) {
        next_state = TAILQ_NEXT(curr_state, entries);


        // printf("%d\n", state->occupied_table_capacity);
        // printf("%d\n", curr_state->num_people);
        if (curr_state->num_people <= state->occupied_table_capacity) {
            printf("HEREEEEE %d\n", curr_state->num_people);
            printf("SIZEEEE %d\n", curr_state->queue_num); 

            curr_state->table_num = state->table_num;
            curr_state->table_index = state->table_index;
            curr_state->occupied_table_capacity = state->occupied_table_capacity;

            table_occupied_status[state->occupied_table_capacity][state->table_index] = 0;
            TAILQ_REMOVE(&my_tailq_head, curr_state, entries);

            sem_post(curr_state->internal_sem);
            sem_post(&table_semaphores[1]);
            // TAILQ_REMOVE(&my_tailq_head, curr_state, entries);

            return;
        }
    }

    printf("DIDNT ENTER HTE LOOP\n");
    table_occupied_status[state->occupied_table_capacity][state->table_index] = 0;
    sem_post(&table_semaphores[1]);

}
