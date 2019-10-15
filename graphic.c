#include "Headers/graphic.h"

// -----------------------------------------------------------------------------
// Global declaration of the Allegro display, event queue and timer.
// -----------------------------------------------------------------------------

ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue_g = NULL;
ALLEGRO_TIMER *timer = NULL;

// -----------------------------------------------------------------------------
// Initialize the display as well as creating a timer and an event
// queue to use that timer.
// -----------------------------------------------------------------------------

void init_graphic(){

    display = al_create_display(WIN_W, WIN_H);

    if(!display) {
        fprintf(stderr, "failed to create display!\n");
        al_destroy_timer(timer);
        exit(120);
    }

    timer = al_create_timer(1.0 / FPS );
    if(!timer) {
        fprintf(stderr, "failed to create timer!\n");
        exit(22);
    }

    event_queue_g = al_create_event_queue();
    if(!event_queue_g) {
        fprintf(stderr, "failed to create event_queue!\n");
        al_destroy_display(display);
        al_destroy_timer(timer);
        exit(33);
    }

    al_register_event_source(event_queue_g, al_get_timer_event_source(timer));

}

// -----------------------------------------------------------------------------
// Graphic thread. Synchronizing with a timer, calls the function to draw the pendulums
// -----------------------------------------------------------------------------

void *graphic(void *arg){

    printf("Starting graphic thread \n");

    init_graphic();
    init_menu();

    ALLEGRO_EVENT e;

    al_start_timer(timer);

    while(1){

        al_wait_for_event(event_queue_g, &e);

        pthread_mutex_lock(&sm.kbmux);
        if(sm.exit){
            pthread_mutex_unlock(&sm.kbmux);
            break;
        }
        else if(sm.start == true){
            pthread_mutex_unlock(&sm.kbmux);
            draw_pend();
        }
        else
            pthread_mutex_unlock(&sm.kbmux);

    }

    printf("Stopping graphic thread \n");

    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue_g);
}

// -----------------------------------------------------------------------------
// Sets colors and fonts. Then we draw and write strings of the menu on the display.
// -----------------------------------------------------------------------------

void init_menu(){

    ALLEGRO_COLOR cream = al_map_rgb(255, 253, 208);
    ALLEGRO_COLOR black = al_map_rgb(0, 0, 0);
    ALLEGRO_FONT *font1 = al_load_ttf_font("INC.ttf", WIN_W / 16, 100);
    ALLEGRO_FONT *font2 = al_load_ttf_font("INC.ttf", WIN_W / 32, 0);
    ALLEGRO_FONT *font3 = al_load_ttf_font("INC.ttf", WIN_W / 48, 0);
    ALLEGRO_FONT *font4 = al_load_ttf_font("INC.ttf", WIN_W / 54, 0);

    char *s = "            ";
    char *s1 = "      ";

    al_clear_to_color(cream);

    al_draw_text(font1, black, WIN_W/2, WIN_H/10, ALLEGRO_ALIGN_CENTRE, "DOUBLE PENDULUM SIMULATOR");
    al_draw_text(font2, black, WIN_W/2, WIN_H/4, ALLEGRO_ALIGN_CENTRE, "Press SPACE to start the simulation or ESC to stop it");
    al_draw_textf(font3, black, WIN_W/(WIN_W * 0.7), WIN_H/2, ALLEGRO_ALIGN_LEFT, "PENDULUM INITIAL DATA:%sTheta 1%sTheta 2%s Mass 1%s Mass 2", s1, s1, s1, s1);

    // Loop to print all data of each pendulum. IF controls are used to align by column even when we have
    // triple or double digit numbers (we add or remove spaces based on this).

    for(int i = 0; i < sm.npend; i++) {

        int container  = (int) rad2deg(sm.pend[i].theta1);
        int container2 = (int) rad2deg(sm.pend[i].theta2);
        int container3 = (int) sm.pend[i].m1;
        int container4 = (int) sm.pend[i].m2;

        if(container > 99 || container < 10) {
            if(container2 > 99)
                al_draw_textf(font4, black, WIN_W / (WIN_W * 0.7), WIN_H / 2 + ((i + 2) * (BOX_H / 10)), ALLEGRO_ALIGN_LEFT,
                              "%s%sPEND %d%s%d°%s%d°%s%d%s%d", s1, s, i + 1,
                              s,container,s,container2,s,container3,s,container4);
            else if(container2 > 9 && container2 < 100)
                al_draw_textf(font4, black, WIN_W / (WIN_W * 0.7), WIN_H / 2 + ((i + 2) * (BOX_H / 10)), ALLEGRO_ALIGN_LEFT,
                              "%s%sPEND %d%s%d°%s%d°%s%d%s%d", s1, s, i + 1,
                              s,container,s,container2,s,container3,s,container4);
            else
                al_draw_textf(font4, black, WIN_W / (WIN_W * 0.7), WIN_H / 2 + ((i + 2) * (BOX_H / 10)), ALLEGRO_ALIGN_LEFT,
                              "%s%sPEND %d%s%d°%s%d°%s%d%s%d", s1, s, i + 1,
                              s,container,s,container2,s,container3,s,container4);
        }
        else{
            if(container2 > 99)
                al_draw_textf(font4, black, WIN_W / (WIN_W * 0.7), WIN_H / 2 + ((i + 2) * (BOX_H / 10)), ALLEGRO_ALIGN_LEFT,
                              "%s%sPEND %d%s%d°%s%d°%s%d%s%d", s1, s, i + 1,
                              s,container,s,container2,s,container3,s,container4);
            else if(container2 > 9 && container2 < 100)
                al_draw_textf(font4, black, WIN_W / (WIN_W * 0.7), WIN_H / 2 + ((i + 2) * (BOX_H / 10)), ALLEGRO_ALIGN_LEFT,
                              "%s%sPEND %d%s%d°%s %d°%s%d%s%d", s1, s, i + 1,
                              s,container,s,container2,s,container3,s,container4);
            else
                al_draw_textf(font4, black, WIN_W / (WIN_W * 0.7), WIN_H / 2 + ((i + 2) * (BOX_H / 10)), ALLEGRO_ALIGN_LEFT,
                              "%s%sPEND %d%s%d°%s%d°%s%d%s%d", s1, s, i + 1,
                              s,container,s,container2,s,container3,s,container4);
        }
    }
    al_flip_display();
}

// -----------------------------------------------------------------------------
// - Define colors
// - Draw lines to delimit the boxes
// - Save trail points
// - When it's time, draws the trail calling the function under this one
// - Draws the pendulum
// -----------------------------------------------------------------------------

void draw_pend(){

    int     size;                   // Quantity of elements currently in circular buffer
    double  x[CB_DIM], y[CB_DIM];   // Coordinates of trail points

    ALLEGRO_COLOR black = al_map_rgb(0, 0, 0);
    ALLEGRO_COLOR orange = al_map_rgb(253, 165, 15);
    ALLEGRO_COLOR cream = al_map_rgb(255, 253, 208);
    ALLEGRO_COLOR dark_orange = al_map_rgb(240, 94, 35);

    al_clear_to_color(cream);

    // Horizontal lines of the boxes
    al_draw_line(0.0, BOX_H, WIN_W, BOX_H, black, 8.0);
    al_draw_line(0.0, BOX_H * 2, WIN_W, BOX_H * 2, black, 8.0);

    // Vertical lines of the boxes
    al_draw_line(BOX_W, 0.0, BOX_W, WIN_H, black, 8.0);
    al_draw_line(BOX_W * 2, 0.0, BOX_W * 2, WIN_H, black, 8.0);

    for(int i = 0; i < sm.npend; i++){

        pthread_mutex_lock(&sm.mutex[i]);

        double x0 = sm.pts[i].x0;
        double y0 = sm.pts[i].y0;
        double x1 = sm.pts[i].x1;
        double y1 = sm.pts[i].y1;
        double x2 = sm.pts[i].x2;
        double y2 = sm.pts[i].y2;

        size = sm.cbuf[i].size;

        // Looping on the circular buffer

        for (int j = 0; j < size; j++) {

            int k1 = (sm.cbuf[i].first + j) % CB_DIM;

            x[j] = sm.cbuf[i].x2[k1];
            y[j] = sm.cbuf[i].y2[k1];
        }

        pthread_mutex_unlock(&sm.mutex[i]);

        al_draw_filled_circle(x0, y0, 3, black);
        al_draw_line(x0, y0, x1, y1, black, 4);

        if (size)
            draw_trail(x, y, size);

        al_draw_line(x1, y1, x2, y2, black, 4);
        al_draw_filled_circle(x1, y1, P_RAD, orange);
        al_draw_filled_circle(x2, y2, P_RAD, dark_orange);
    }

    al_flip_display();
}

// -----------------------------------------------------------------------------
// Simply draw the trail on the display. Points are drawn as very small filled circles
// -----------------------------------------------------------------------------

void draw_trail(double *x, double *y, int size){ // draw w past values

    ALLEGRO_COLOR red = al_map_rgb(200, 0, 0);

    for (int j = 0; j + 1 < size; j++)
        al_draw_filled_circle(x[j], y[j], 2, red);
}

// -----------------------------------------------------------------------------
// Converts values from radiant to degree
// -----------------------------------------------------------------------------

float rad2deg(double rad) {
    float 	deg;

    deg = (rad / PI) * 180.0;
    return deg;
}
