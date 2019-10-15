#include "Headers/init.h"

FILE    *fd;

// -----------------------------------------------------------------------------
// Initialize the display and flags based on the number of pendulums
// -----------------------------------------------------------------------------

void allegro_init() {

    // Setting both flags to false

    sm.start    = false;
    sm.exit     = false;

    if( !al_init() ) {
        fprintf(stderr, "failed to initialize allegro!\n");
        exit(3);
    }

    //Initializing allegro primitives, font and ttf (True Type Font) addon. It's mandatory to do so

    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();
}

// -----------------------------------------------------------------------------
// Gets the data from the .txt file
// -----------------------------------------------------------------------------

void read_data() {

    char     row[LINE];        // stores the current line read of the .txt file

    fd = fopen("Data.txt", "r");

    if (fd == NULL) {
        printf ("Error opening the file.\n");
        exit(1);
    }

    search_num_pend(row);

    do {

        fgets(row, LINE, fd);
        search_theta_1(row);
        search_theta_2(row);
        search_mass_1(row);
        search_mass_2(row);

    } while (!feof(fd));

    fclose(fd);
}

// -----------------------------------------------------------------------------
// Searches in the .txt file the number of double pendulums
// -----------------------------------------------------------------------------

void search_num_pend(char *row) {

    int  start = 1;                                // flag for the while loop
    char *ref  = "Number of Double Pendulums";    // line to be search

    // Searches the .txt file until the "ref" string is found
    while (start) {

        fgets(row, LINE, fd);

        if (strncmp(row, ref, (int) strlen(ref)) == 0)
            start = 0;
    }

    fgets(row, LINE, fd);
    sm.npend = atoi(row);

    if (sm.npend > MAX_DP) {
        printf("Error: too many double pendulums\n");
        exit(100);
    }

    sm.pend     = malloc(sm.npend * sizeof(struct pendulum));
    sm.pts      = malloc(sm.npend * sizeof(struct pend_points));
    sm.cbuf     = malloc(sm.npend * sizeof(struct circ_buf));
    sm.mutex    = malloc((sm.npend) * sizeof(pthread_mutex_t));

    for (int i = 0; i < sm.npend; i++){

        // Initialize parameteres for each pendulum

        sm.cbuf[i].first = 0;
        sm.cbuf[i].size = 0;

        sm.pend[i].l1 = P_LEN;
        sm.pend[i].l2 = P_LEN;
        sm.pend[i].theta1_vel = 0;
        sm.pend[i].theta2_vel = 0;

        if (pthread_mutex_init(&sm.mutex[i], NULL) != 0) {
            printf("Mutex #%d init failed\n", i+1);
            exit(100);
        }
        else
            printf("Mutex #%d init done\n", i+1);
    }

    if (pthread_mutex_init(&sm.kbmux, NULL) != 0) {
        printf("Keyboard mux init failed\n");
        exit(100);
    }
}

// -----------------------------------------------------------------------------
// All these functions compare the input string "row" to a
// reference string that finds the data we look for. If the data is found
// it is stored in the parameter structure of the double pendulum
// -----------------------------------------------------------------------------

void search_theta_1(char *row) {

    static int     index = 0;
    char           *ref = "Theta_1:";                     // Line we look for

    if (strncmp(row, ref, (int)strlen(ref)) == 0) {
        fgets(row, LINE, fd);
        sm.pend[index].theta1 = deg2rad(atoi(row));
        index++;
    }
    else
        return;
}

void search_theta_2(char *row) {

    static int     index = 0;
    char           *ref = "Theta_2:";        // Line we look for

    if (strncmp(row, ref, (int)strlen(ref)) == 0) {
        fgets(row, LINE, fd);
        sm.pend[index].theta2 = deg2rad(atoi(row));
        index++;
    }
    else
        return;
}

void search_mass_1(char *row){

    static int     index = 0;
    char           *ref = "Mass_1:";        // Line we look for

    if (strncmp(row, ref, (int)strlen(ref)) == 0) {
        fgets(row, LINE, fd);
        sm.pend[index].m1 = atoi(row);
        index++;
    }
    else
        return;
}

void search_mass_2(char *row){

    static int     index = 0;
    char           *ref = "Mass_2:";        // Line we look for

    if (strncmp(row, ref, (int)strlen(ref)) == 0) {
        fgets(row, LINE, fd);
        sm.pend[index].m2 = atoi(row);
        index++;
    }
    else
        return;
}

// -----------------------------------------------------------------------------
// Converts values from degree  to radiant
// -----------------------------------------------------------------------------

double deg2rad(double deg) {

    double     rad;

    rad = (deg / 180.0) * PI;

    return rad;
}


