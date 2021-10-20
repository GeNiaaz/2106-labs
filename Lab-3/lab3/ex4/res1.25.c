#include "restaurant.h"
#include <stdio.h>
#include <semaphore.h>
#include <sys/queue.h>


// You can declare global variables here


// table numbers array
int table_num[5][1000];
// tables occupied
int table_occupied_status[5][1000];

// general semaphore declaration
sem_t table_capacity_semaphore[5];

// table queue number for given capacity
int tables[5];


void restaurant_init(int num_tables[5]) {
    // Write initialization code here (called once at the start of the program).
    // It is guaranteed that num_tables is an array of length 5.
    // TODO

    // init array of table numbers to -1


    // sem_init(&global_mutex, 0, 1);

    // init to 1000
    for (int i = 0; i < 5; i++) {
        for (int a = 0; a < 1000; a++) {
            table_num[i][a] = -1;
            table_occupied_status[i][a] = -1;
        }
    }

    for (int i = 0; i < 5; i++) {
        tables[i] = num_tables[i];
    }

    // init semaphores 
    for (int i = 0; i < 5; i++) {
        sem_init(&table_capacity_semaphore[i], 0, num_tables[i]);
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

    sem_wait(&table_capacity_semaphore[num_people]);
    for ()

    
    return 0;
}

void leave_table(group_state *state) {
    // Write your code here.
    // TODO

    

}
