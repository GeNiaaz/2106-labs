#include "packer.h"
#include <stdio.h>
#include <semaphore.h>


/* help 

sem_init ===== initialise semaphore
sem_wait / sem_trywait (return error, instead block) / sem_timedwait (wait for timeout) ===== decrement semaphore pointed to if > 0;
sem_post =====  increment semaphore and return so another can acquire
sem_destroy ===== clean up function
*/

// You can declare global variables here

#define red 0
#define green 1
#define blue 2


sem_t arr_sems[3][3];
int arr_id[3][2];
int arr_limit[3];

void packer_init(void) {
    // Write initialization code here (called once at the start of the program).

    for (int i = 0; i < 3; i++) {
        
        sem_init(&arr_sems[i][0], 0, 1);
        sem_init(&arr_sems[i][1], 0, 0);
        sem_init(&arr_sems[i][2], 0, 1);

        arr_limit[i] = 0;

        arr_id[i][0] = -1;
        arr_id[i][1] = -1;
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

int pack_ball(int colour, int id) {
    // Write your code here.
    colour = colour - 1;

    sem_wait(&arr_sems[colour][0]);
    arr_limit[colour] = arr_limit[colour] + 1;

    if (arr_limit[colour] == 2) {
        arr_limit[colour] = 0;

        arr_id[colour][1] = id;
        sem_post(&arr_sems[colour][1]);
        sem_post(&arr_sems[colour][0]);

        return arr_id[colour][0];

    } else {
        sem_wait(&arr_sems[colour][2]);

        arr_id[colour][0] = id;
        sem_post(&arr_sems[colour][0]);
        sem_wait(&arr_sems[colour][1]);

        sem_post(&arr_sems[colour][2]);
        return arr_id[colour][1];
    }

    return 0;
}
