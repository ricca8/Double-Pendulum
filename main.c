#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>
#include "Headers/init.h"
#include "Headers/graphic.h"
#include "Headers/keyboard_manager.h"
#include "Headers/elaboration.h"


/****************************************************
 ****************************************************
 ****************************************************
 ****************************************************
 ****************************************************
 ****************************************************
 ****************************************************
 ****************************************************
 ****************************************************
 *
 *            CHECK READ ME FILE (ReadMe.txt)
 *
 ****************************************************
 ****************************************************
 ****************************************************
 ****************************************************
 ****************************************************
 ****************************************************
 ****************************************************
 ****************************************************
 ****************************************************/



//---------------------------------------------------
// Main function that reads the data, starts all the
// threads after creating them, and waits until they stops.
//---------------------------------------------------

int main(int argc, char **argv) {

    int i;                                              // For cycles indexing

    read_data();

    sleep(1);

    allegro_init();

    pthread_t p, g, k;
    pthread_attr_t a;

    pthread_attr_init(&a);

    pthread_create(&g, &a, graphic, NULL);
    pthread_create(&k, &a, keyboard, NULL);

    int pend_id[sm.npend];                               // Creates an array of int (as long as the total number
                                                         // of pendulums) which will contains pendulums threads' id

    printf("I've read %d pendulums from file\n", sm.npend);

    for (i = 0; i < sm.npend; i++){
        pend_id[i] = i;
        pthread_create(&p, &a, pend, pend_id + i);
    }

    pthread_join(g, NULL);                              // Wait for threads to stop
    pthread_join(k, NULL);

    for(i = 0; i < sm.npend; i++){                      // Destroy of mutexes
        pthread_mutex_destroy(&sm.mutex[i]);
    }

    pthread_attr_destroy(&a);                           // Destroy of threads' attributes
}
