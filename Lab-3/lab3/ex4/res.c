#include "restaurant.h"
#include <stdio.h>
#include <semaphore.h>
#include <sys/queue.h>

/* 

create array for each table, queue
once tables full, add to said queue

how to maintain order in which thread woken up?

*/


// You can declare global variables here

/* Queue */

// struct tailq_entry {
// 	int value;

// 	/*
// 	 * This holds the pointers to the next and previous entries in
// 	 * the tail queue.
// 	 */
// 	TAILQ_ENTRY(tailq_entry) entries;
// };


// TAILQ_HEAD(, tailq_entry) queue_head;

// TAILQ_INIT(&queue_head);


// initialization of queues
// struct tailq_entry {
// 	int value;

// 	/*
// 	 * This holds the pointers to the next and previous entries in
// 	 * the tail queue.
// 	 */
// 	TAILQ_ENTRY(tailq_entry) entries;
// };

int table_availability[5];

// table numbers array
int table_num[5][1000];
// tables occupied
int table_occupied_status[5][1000];

// semaphore declaration
sem_t table_capacity_semaphore[5][4];

// table queue number for given capacity
int table_queues[5];
// int table_queue_exists_status[5];

// current queue num
int next_expected_queue_number[5];
sem_t global_mutex;


void restaurant_init(int num_tables[5]) {
    // Write initialization code here (called once at the start of the program).
    // It is guaranteed that num_tables is an array of length 5.
    // TODO

    // init array of table numbers to -1
    sem_init(&global_mutex, 0, 1);
    for (int i = 0; i < 5; i++) {
        for (int a = 0; a < 1000; a++) {
            table_num[i][a] = -1;
            table_occupied_status[i][a] = -1;
        }
    }

    for (int i = 0; i < 5; i++) {
        table_availability[i] = num_tables[i];
        // table_queue_exists_status[i] = 0;
        table_queues[i] = 0;
        next_expected_queue_number[i] = 0;

    }

    // init semaphores 
    for (int i = 0; i < 5; i++) {

        // int max_tables = num_tables[i];

        sem_init(&table_capacity_semaphore[i][0], 0, 1);
        sem_init(&table_capacity_semaphore[i][1], 0, 0);
        sem_init(&table_capacity_semaphore[i][2], 0, 1);
        sem_init(&table_capacity_semaphore[i][3], 0, 0);
    }

    // init table numbers
    int table_counter = 0;
    for (int i = 0; i < 5; i++) {
        for (int c = 0; c < num_tables[i]; c++) {
            table_num[i][c] = table_counter;
            table_occupied_status[i][c] = 0;
            table_counter++;
        }
    }

}

void restaurant_destroy(void) {
    // Write deinitialization code here (called once at the end of the program).
    // TODO
}

int request_for_table(group_state *state, int num_people) {
    // Write your code here.
    // Return the id of the table you want this group to sit at.
    // TODO

    // just to make it easier to indexr
    num_people--;
    state->num_people = num_people;

    sem_wait(&table_capacity_semaphore[num_people][0]);
    on_enqueue();

    int tmp_curr_queue_num = table_queues[num_people];
    state->position = tmp_curr_queue_num;
        
    table_queues[num_people]++;
    sem_post(&table_capacity_semaphore[num_people][0]); 
    /* NO TABLE AVAIL OR ppl in queue */
    sem_wait(&table_capacity_semaphore[num_people][2]);
    printf("%d %d\n", num_people, table_availability[num_people]);

    if (table_availability[num_people] <= 0) {
         
        // sem_post(&table_capacity_semaphore[num_people][0]);
        // BLOCK HERE
        sem_post(&table_capacity_semaphore[num_people][2]);
        wait:
        sem_wait(&table_capacity_semaphore[num_people][1]);
        if (state->position != next_expected_queue_number[num_people]) {
            sem_post(&table_capacity_semaphore[num_people][1]);
            goto wait;
        } else {
            sem_post(&table_capacity_semaphore[num_people][1]);
        }
        printf("Ticket and expected queue number:%d %d\n", state->position, next_expected_queue_number[num_people]);
        /*
        while (state->position != next_expected_queue_number[num_people]) {
            sem_post(&table_capacity_semaphore[num_people][1]);
            sem_wait(&table_capacity_semaphore[num_people][1]);
        }*/
    } else {
        sem_post(&table_capacity_semaphore[num_people][2]);
    }
    sem_wait(&global_mutex);
    // reduce table availability
    table_availability[num_people]--;


    int counter = 0;
    while (table_occupied_status[num_people][counter] != 0) {
        counter++;
    }

    // occupy this table
    table_occupied_status[num_people][counter] = 1;

    int table_num_occupied = table_num[num_people][counter];
    sem_post(&global_mutex);

    state->table_number = table_num_occupied;
    state->table_index_for_capacity = counter;

    // sem_post(&table_capacity_semaphore[num_people][0]);
   // sem_post(&table_capacity_semaphore[num_people][2]);
    
    return table_num_occupied;
}

void leave_table(group_state *state) {
    // Write your code here.
    // TODO
    sem_wait(&table_capacity_semaphore[state->num_people][2]);
    // free table
    table_availability[state->num_people]++; // UNBLOCK TABLES  ==========
    table_occupied_status[state->num_people][state->table_index_for_capacity] = 0;
    next_expected_queue_number[state->num_people]++;
    sem_post(&table_capacity_semaphore[state->num_people][2]);
    sem_post(&table_capacity_semaphore[state->num_people][1]);
 
    // check for next avail to be served
        // if queue not empty
    

}
