#include "restaurant.h"
#include <stdio.h>
#include <semaphore.h>
#include <sys/queue.h>


// You can declare global variables here


int next_expected[5];
int queue_counter[5];

// table numbers array
int table_num[5][1000];
// tables occupied
int table_occupied_status[5][1000];

// semaphore declaration
sem_t table_capacity_semaphore[5][4];

// table queue number for given capacity
int table_queue_number[5];

// current queue num
int table_quantity[5];
sem_t global_mutex;


void restaurant_init(int num_tables[5]) {
    // Write initialization code here (called once at the start of the program).
    // It is guaranteed that num_tables is an array of length 5.
    // TODO

    // init array of table numbers to -1

    for (int i = 0; i < 5; i++) {
        for (int a = 0; a < 1000; a++) {
            table_num[i][a] = -1;
            table_occupied_status[i][a] = -1;
        }
    }

    for (int i = 0; i < 5; i++) {
        next_expected[i] = num_tables[i];
        table_queue_number[i] = 0;
        table_quantity[i] = num_tables[i];
    }

    // init semaphores 
    for (int i = 0; i < 5; i++) {

        // int max_tables = num_tables[i];

        sem_init(&table_capacity_semaphore[i][0], 0, 1);
        sem_init(&table_capacity_semaphore[i][1], 0, num_tables[i]);
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

    on_enqueue();
    // get queue number
    sem_wait(&table_capacity_semaphore[num_people][0]);
    state->queue_num = table_queue_number[num_people];
    state->num_people = num_people;
    table_queue_number[num_people]++;
    sem_post(&table_capacity_semaphore[num_people][0]);


    start:
    sem_wait(&table_capacity_semaphore[num_people][1]);

    sem_wait(&table_capacity_semaphore[num_people][2]);
    if (next_expected[num_people] > (state->queue_num)) {
        sem_post(&table_capacity_semaphore[num_people][2]);
    } else {
        sem_post(&table_capacity_semaphore[num_people][2]);
        sem_post(&table_capacity_semaphore[num_people][1]);
        goto start;
    }

    int result_table_num = 0;
    for (int i = 0; i < table_quantity[num_people]; i++) {
        if (table_occupied_status[num_people][i] == 0) {
            table_occupied_status[num_people][i] = 1;
            result_table_num = table_num[num_people][i];
            state->table_index = i;
            break;
        }
    }

    sem_post(&table_capacity_semaphore[num_people][1]);

    return result_table_num;
}

void leave_table(group_state *state) {
    // Write your code here.
    // TODO

    sem_wait(&table_capacity_semaphore[state->num_people][2]);

    next_expected[state->num_people]++;
    table_occupied_status[state->num_people][state->table_index] = 0;

    sem_post(&table_capacity_semaphore[state->num_people][2]);

    sem_post(&table_capacity_semaphore[state->num_people][1]);

}
