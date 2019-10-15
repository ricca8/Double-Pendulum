
#include "Headers/keyboard_manager.h"

//---------------------------------------------------
// Creation of an allegro event queue needed to use the keyboard
//---------------------------------------------------

ALLEGRO_EVENT_QUEUE *event_queue = NULL;

//---------------------------------------------------
// Initializing the keyboard, so that allegro can use it, linking it to the event queue
//---------------------------------------------------

void init_keyboard(){


    if(!al_install_keyboard()) {
        fprintf(stderr, "failed to initialize the keyboard!\n");
        exit(1);
    }

    event_queue = al_create_event_queue();
    if(!event_queue) {
        fprintf(stderr, "failed to create event_queue!\n");
        exit(2);
    }

    al_register_event_source(event_queue, al_get_keyboard_event_source());
}

//---------------------------------------------------
// Keyboard thread
//---------------------------------------------------

void *keyboard(void *arg){

    printf("Starting keyboard thread \n");

    init_keyboard();

    while(!sm.exit){                                        // Runs until exit flag is false
        
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {        //If the windows is closed, it stops
                                                            // the simulation putting the exit flag to true
            pthread_mutex_lock(&sm.kbmux);
            sm.exit = true;
            pthread_mutex_unlock(&sm.kbmux);
            break;
        }

        else if(ev.type == ALLEGRO_EVENT_KEY_UP) {          // When a button is pressed:
            switch (ev.keyboard.keycode) {
                case ALLEGRO_KEY_ESCAPE:                    // - if it's escape, exit flag switches to true
                    pthread_mutex_lock(&sm.kbmux);
                    sm.exit = true;
                    pthread_mutex_unlock(&sm.kbmux);
                    break;
                case ALLEGRO_KEY_SPACE:                     // - if it's space, start flag switches to true
                    pthread_mutex_lock(&sm.kbmux);
                    sm.start = true;
                    pthread_mutex_unlock(&sm.kbmux);
                    break;
            }
        }
    }

    al_destroy_event_queue(event_queue);        // Once the thread is finished, we destroy the event queue
}
