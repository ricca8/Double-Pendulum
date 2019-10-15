#ifndef RT_RICCA_INIT_H
#define RT_RICCA_INIT_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "macros.h"

void    allegro_init();
double  deg2rad(double deg);
void    search_num_pend(char *row);
void    search_theta_1(char *row);
void    search_theta_2(char *row);
void    search_mass_1(char *row);
void    search_mass_2(char *row);
void    read_data();

#endif
