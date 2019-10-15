#ifndef macros_h
#define macros_h

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <pthread.h>

//---------------------------------------------------
// Text file constants
//---------------------------------------------------

#define LINE        80          // Max length of a line of the .txt file
#define MAX_DP      9           // Max number of pendulums

//---------------------------------------------------
// Math constants
//---------------------------------------------------

#define PI          3.14        // Greek Pi
#define G           9.81        // Gravity

//---------------------------------------------------
// Window constants
//---------------------------------------------------

#define WIN_W       2400        // Window width
#define WIN_H       1800        // Window height
#define BOX_W       WIN_W / 3   // Width of each box that contains a pendulum
#define BOX_H       WIN_H / 3   // Height of each box that contains a pendulum
#define HBOX_EIGHT  WIN_H / 8   // An eights of a box height
#define WBOX_SIXTH  WIN_W / 6   // A sixths of a box width
#define DWN     WIN_H / 3       // Factor for moving drawing down

//---------------------------------------------------
// Constants for graphic and timing
//---------------------------------------------------

#define P_LEN   6 * WIN_W / 200 + WIN_W / 100           // Length of pendulum's rods
#define P_RAD   1600 / WIN_W * 2 + WIN_W / 200          // Radius of pendulum's balls
#define FPS     60                                      // Frame per second
#define TSCALE  10                                      // Time scale
#define CB_DIM  200                                     // Circular buffer dimension

//---------------------------------------------------
// Structure that contains all pendulum's values
//---------------------------------------------------

struct pendulum {
    float     l1;               // Length of the 1st pendulum
    float     m1;               // Mass of the 1st pendulum
    double    theta1;           // Angle of the 1st pendulum
    float     theta1_vel;       // Angular velocity of the 1st pendulum
    float     l2;               // Length of the 2st pendulum
    float     m2;               // Mass of the 2st pendulum
    double    theta2;           // Angle of the 2st pendulum
    float     theta2_vel;       // Angular velocity of the 2st pendulum
};

//---------------------------------------------------
// Structure that contains pendulum's coordinates once converted to the window coordinates
//---------------------------------------------------

struct pend_points{

    double  x0;
    double  y0;
    double  x1;
    double  y1;
    double  x2;
    double  y2;
};

//---------------------------------------------------
// Circular buffer to store trail points
//---------------------------------------------------

struct circ_buf{
    int     first;          // Stores the index of the last element of the buffer
    int     size;           // Quantity of elements currently in circular buffer
    double  x2[CB_DIM];
    double  y2[CB_DIM];
};

//---------------------------------------------------
// Piece of memory shared among threads
//---------------------------------------------------

struct shared_memory{
    
    struct pendulum     *pend;
    struct pend_points  *pts;
    struct circ_buf     *cbuf;
    bool                start;      // Flag to start the simulation
    bool                exit;       // Flag to exit the simulation
    int                 npend;      // Total number of pendulums
    pthread_mutex_t     *mutex;     // One mutex for each pendulum
    pthread_mutex_t     kbmux;      // One mutex for the keyboard

};

struct shared_memory sm;


#endif
