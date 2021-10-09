#include "packer.h"
#include <stdio.h>
#include <semaphore.h>

// You can declare global variables here

// array semaphores
sem_t arr_sems[3][4];

// array of count so far
int arr_id_count[3];
int arr_id[3][64];

// current count
int arr_limit[3];

int balls_count = 0;
int write_so_far[3] = {0, 0, 0};


void packer_init(int balls_per_pack) {
    // Write initialization code here (called once at the start of the program).
    // It is guaranteed that balls_per_pack >= 2.

    balls_count = balls_per_pack;

    for (int i = 0; i < 4; i++) {
        
        sem_init(&arr_sems[i][0], 0, 1);
        sem_init(&arr_sems[i][1], 0, 0);
        sem_init(&arr_sems[i][2], 0, 1);
        sem_init(&arr_sems[i][3], 0, 0);

        arr_limit[i] = 0;

        // arr_id[i][0] = -1;
        // arr_id[i][1] = -1;
    }
}

void packer_destroy(void) {
    // Write deinitialization code here (called once at the end of the program).

    for (int i = 0; i < 3; i++) {
        sem_destroy(&arr_sems[i][0]);
        sem_destroy(&arr_sems[i][1]);
        sem_destroy(&arr_sems[i][2]);
    }

}

void write_to_arr(int colour, int id, int *other_ids) {

    sem_wait(&arr_sems[colour][2]);
    int counter = 0;
    for (int i = 0; i < balls_count; i++) {
        int tmp = arr_id[colour][i];
        if (tmp != id) {
            other_ids[counter] = tmp;
            counter ++;
        } 
    }
    write_so_far[colour] ++;

    if (write_so_far[colour] == balls_count) {
        write_so_far[colour] = 0;
        for (int a = 0; a < 64; a++) {
            arr_id[colour][a] = 0;
        }

        sem_post(&arr_sems[colour][3]);
    }
    sem_post(&arr_sems[colour][2]);

    // sem_wait(&arr_sems[colour][2]);

}

void pack_ball(int colour, int id, int *other_ids) {
    // Write your code here.
    // Remember to populate the array `other_ids` with the (balls_per_pack-1) other balls.

    colour = colour - 1;

    sem_wait(&arr_sems[colour][0]);
    arr_limit[colour] = arr_limit[colour] + 1;

    if (arr_limit[colour] == balls_count) {

        arr_id[colour][arr_limit[colour] - 1] = id;
        arr_limit[colour] = 0;

        write_to_arr(colour, id, other_ids);

        // loop posting sems
        for (int i = 0; i < balls_count - 1; i++) {
          sem_post(&arr_sems[colour][1]);
        }

        sem_wait(&arr_sems[colour][3]);

        sem_post(&arr_sems[colour][0]);
        return;
    }

    arr_id[colour][arr_limit[colour] - 1] = id;
    sem_post(&arr_sems[colour][0]);

    // one by one will get freed by n'th
    sem_wait(&arr_sems[colour][1]);

    write_to_arr(colour, id, other_ids);
    return;

}


// sem does whole process
// last one, allows them all to one by one write
// last one waits for all to write, then delete, and open new sem
