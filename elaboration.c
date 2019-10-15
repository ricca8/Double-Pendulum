#include "Headers/elaboration.h"

/****************************************************
 * Here we have all the functions needed to compute
 * the new position of all parts of the pendulum.
 * We use the Runge-Kutta algorithm to solve equations,
 * as explained in this link:
 *
 *  https://www.myphysicslab.com/explain/runge-kutta-en.html
 *
 * There you can find the explanation of how the parameters
 * of the pendulums are used and what we are going to make
 * our calculations on.
 ****************************************************/

// -----------------------------------------------------------------------------
// Creation of global matrixes containing temporary data of each pendulum, later
// needed to increment all the parameters. Counter variable is used to keep trace
// of when to save the pendulum trail and the "t" one to track time.
// -----------------------------------------------------------------------------


unsigned int        counter = 1;
double              t = 1.0 / (FPS / 10) ;

double              mx1[9][4];
double              mx2[9][4];
double              mx3[9][4];
double              mx4[9][4];
double              inc[9][4];

// -----------------------------------------------------------------------------
// Global declaration of the Allegro event queue and timer.
// -----------------------------------------------------------------------------


ALLEGRO_EVENT_QUEUE *event_queue_p = NULL;
ALLEGRO_TIMER *timer_p = NULL;


// -----------------------------------------------------------------------------
// Initialize the centre for each pendulum, as well as creating a timer and an
// event queue to use that timer.
// -----------------------------------------------------------------------------

void *pend(void *arg){

    int id = *((int *) arg);

    printf("Starting pendulum thread #%d \n", id+1);

    timer_p = al_create_timer(1.0 / (FPS * TSCALE));
    if(!timer_p) {
        fprintf(stderr, "failed to create timer!\n");
    }

    event_queue_p = al_create_event_queue();
    if(!event_queue_p) {
        fprintf(stderr, "failed to create event_queue!\n");
        al_destroy_timer(timer_p);
    }

    al_register_event_source(event_queue_p, al_get_timer_event_source(timer_p));

    if(id <= 2){
        sm.pts[id].x0 = WBOX_SIXTH * (id * 2 + 1);
        sm.pts[id].y0 = HBOX_EIGHT;
    }
    else if(id >= 3 && id<= 5 ){
        sm.pts[id].x0 = WBOX_SIXTH * ((id - 3) * 2 + 1);
        sm.pts[id].y0 = HBOX_EIGHT + DWN;
    }
    else if(id >= 5 && id <= 8 ){
        sm.pts[id].x0 = WBOX_SIXTH * ((id - 6) * 2 + 1);
        sm.pts[id].y0 = HBOX_EIGHT + DWN * 2;
    }
    else
        exit(999);

    // First calculation to set pendulum based on initial data read from file
    RUN_KUT(id);
    xy2pixel(id);

    al_start_timer(timer_p);
    ALLEGRO_EVENT e;

    while(1) {
        al_wait_for_event(event_queue_p, &e);

        pthread_mutex_lock(&sm.kbmux);
        if (sm.exit) {
            pthread_mutex_unlock(&sm.kbmux);
            break;
        } else if (sm.start == true) {
            pthread_mutex_unlock(&sm.kbmux);

            // Cycling of calculations until the exit flag changes to true

            RUN_KUT(id);
            xy2pixel(id);
        } else
            pthread_mutex_unlock(&sm.kbmux);
    }
}

// -----------------------------------------------------------------------------
// Converts the real world coordinates to pixel coordinates and stores the
// pendulum trail.
// -----------------------------------------------------------------------------

void xy2pixel(int i){

    double x1 = sm.pts[i].x0 + sin(sm.pend[i].theta1) * sm.pend[i].l1;
    double y1 = sm.pts[i].y0 + cos(sm.pend[i].theta1) * sm.pend[i].l1;
    double x2 = x1 + sin(sm.pend[i].theta2) * sm.pend[i].l2;
    double y2 = y1 + cos(sm.pend[i].theta2) * sm.pend[i].l2;

    counter = (counter + 1) % 3;

    pthread_mutex_lock(&sm.mutex[i]);

    sm.pts[i].x1 = x1;
    sm.pts[i].y1 = y1;
    sm.pts[i].x2 = x2;
    sm.pts[i].y2 = y2;

    // When the counter has value zero, we save the position in that moment in
    // the circular buffer, so that we can use it to draw the trail of the pendulum.

    if(counter == 0) {

        int k = (sm.cbuf[i].first + sm.cbuf[i].size) % CB_DIM;

        sm.cbuf[i].x2[k] = x2;
        sm.cbuf[i].y2[k] = y2;

        if(sm.cbuf[i].size == CB_DIM)
            sm.cbuf[i].first = (sm.cbuf[i].first + 1) % CB_DIM;
        else if(sm.cbuf[i].size < CB_DIM)
            sm.cbuf[i].size++;
        else
            exit(30);
    }

    pthread_mutex_unlock(&sm.mutex[i]);
}

// -----------------------------------------------------------------------------
// Runge-Kutta equations. Every KU_* updates values in the correspective matrix,
// that are then used by INC function to update the final "inc" matrix that truly
// updates values in each pendulum.
// See comment at the top of this page.
// -----------------------------------------------------------------------------

void RUN_KUT(int i){
    
    INC(i);
    
    sm.pend[i].theta1     += inc[i][0];
    sm.pend[i].theta2     += inc[i][1];
    sm.pend[i].theta1_vel += inc[i][2];
    sm.pend[i].theta2_vel += inc[i][3];

}

void INC(int i){

    KU_1(i);
    KU_2(i);
    KU_3(i);
    KU_4(i);
    
    for (int j = 0; j < 4; j++){
        inc[i][j] = (mx1[i][j] + (2 * mx2[i][j]) + (2 * mx3[i][j]) +mx4[i][j]);
        inc[i][j] = inc[i][j] / 6;
    }
}

void KU_1(int i){

    double     theta1_ku_1;
    double     theta2_ku_1;
    double     theta1_vel_ku_1;
    double     theta2_vel_ku_1;
    
    theta1_ku_1     = sm.pend[i].theta1;
    theta2_ku_1     = sm.pend[i].theta2;
    theta1_vel_ku_1 = sm.pend[i].theta1_vel;
    theta2_vel_ku_1 = sm.pend[i].theta2_vel;
    
    mx1[i][0] = t * theta1_vel_ku_1;
    mx1[i][1] = t * theta2_vel_ku_1;
    mx1[i][2] = t * fun1(theta1_ku_1, theta2_ku_1, theta1_vel_ku_1, theta2_vel_ku_1, i);
    mx1[i][3] = t * fun2(theta1_ku_1, theta2_ku_1, theta1_vel_ku_1, theta2_vel_ku_1, i);
}

void KU_2(int i){

    double     theta1_ku_2;
    double     theta2_ku_2;
    double     theta1_vel_ku_2;
    double     theta2_vel_ku_2;
    
    theta1_ku_2     = sm.pend[i].theta1 + (mx1[i][0] / 2.0);
    theta2_ku_2     = sm.pend[i].theta2 + (mx1[i][1] / 2.0);
    theta1_vel_ku_2 = sm.pend[i].theta1_vel + (mx1[i][2] / 2.0);
    theta2_vel_ku_2 = sm.pend[i].theta2_vel + (mx1[i][3] / 2.0);
    
    mx2[i][0] = t * theta1_vel_ku_2;
    mx2[i][1] = t * theta2_vel_ku_2;
    mx2[i][2] = t * fun1(theta1_ku_2, theta2_ku_2, theta1_vel_ku_2, theta2_vel_ku_2, i);
    mx2[i][3] = t * fun2(theta1_ku_2, theta2_ku_2, theta1_vel_ku_2, theta2_vel_ku_2, i);
}

void KU_3(int i){

    double     theta1_ku_3;
    double     theta2_ku_3;
    double     theta1_vel_ku_3;
    double     theta2_vel_ku_3;
    
    theta1_ku_3     = sm.pend[i].theta1 + (mx2[i][0] / 2.0);
    theta2_ku_3     = sm.pend[i].theta2 + (mx2[i][1] / 2.0);
    theta1_vel_ku_3 = sm.pend[i].theta1_vel + (mx2[i][2] / 2.0);
    theta2_vel_ku_3 = sm.pend[i].theta2_vel + (mx2[i][3] / 2.0);
    
    mx2[i][0] = t * theta1_vel_ku_3;
    mx2[i][1] = t * theta2_vel_ku_3;
    mx2[i][2] = t * fun1(theta1_ku_3, theta2_ku_3, theta1_vel_ku_3, theta2_vel_ku_3, i);
    mx2[i][3] = t * fun2(theta1_ku_3, theta2_ku_3, theta1_vel_ku_3, theta2_vel_ku_3, i);
}

void KU_4(int i){

    double     theta1_ku_4;
    double     theta2_ku_4;
    double     theta1_vel_ku_4;
    double     theta2_vel_ku_4;
    
    theta1_ku_4     = sm.pend[i].theta1 + (mx3[i][0] / 2.0);
    theta2_ku_4     = sm.pend[i].theta2 + (mx3[i][1] / 2.0);
    theta1_vel_ku_4 = sm.pend[i].theta1_vel + (mx3[i][2] / 2.0);
    theta2_vel_ku_4 = sm.pend[i].theta2_vel + (mx3[i][3] / 2.0);
    
    mx2[i][0] = t * theta1_vel_ku_4;
    mx2[i][1] = t * theta2_vel_ku_4;
    mx2[i][2] = t * fun1(theta1_ku_4, theta2_ku_4, theta1_vel_ku_4, theta2_vel_ku_4, i);
    mx2[i][3] = t * fun2(theta1_ku_4, theta2_ku_4, theta1_vel_ku_4, theta2_vel_ku_4, i);
}

double fun1(double t1, double t2, double t1_v, double t2_v, int i){

    double     m1_2;               // Ratio between m_1 and m_2
    double     N1;                 // First  element of the numerator
    double     N2;                 // Second element of the numerator
    double     N3;                 // Third  element of the numerator
    double     D;
    double     r1;                 // Length of the 1st pendulum
    double     r2;                 // Length of the 2nd pendulum

    r1 = sm.pend[i].l1;
    r2 = sm.pend[i].l2;

    m1_2 = (sm.pend[i].m1 / sm.pend[i].m2);


    N1 = (G / r1) * ((sin(t2) * cos(t1 - t2)) - (sin(t1) * (m1_2 + 1)));
    N2 = t1_v * t1_v * cos(t1 - t2) * sin(t1 - t2);
    N3 = t2_v * t2_v * (r2/r1) * sin(t1 - t2);

    D = m1_2 + 1 - (cos(t1 - t2) * cos(t1 - t2));

    return ((N1 - N2 - N3) / D);
}

double fun2(double t1, double t2, double t1_v, double t2_v, int i) {
    double     t1_a;                        // Angular acceleration of theta_1
    double     N1;                          // 1st element of the numerator
    double     N2;                          // 2nd element of the numerator
    double     N3;                          // 3rd element of the numerator
    double     r1;                          // Length of the 1st pendulum
    double     r2;                          // Length of the 2nd pendulum

    r1 = sm.pend[i].l1;
    r2 = sm.pend[i].l2;

    t1_a = fun1(t1, t2, t1_v, t2_v, i);     // Acceleration of theta_1

    N1 = (r1 /r2) * t1_v * t1_v * sin(t1 - t2);
    N2 = (r1 /r2) * t1_a * cos(t1 - t2);
    N3 = (G / r2) * sin(t2);

    return N1 - N2 - N3;
}