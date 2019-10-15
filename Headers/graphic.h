#ifndef graphic_h
#define graphic_h

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include "macros.h"
#include "elaboration.h"

void draw_pend();
void init_menu();
void *graphic(void *arg);
void draw_trail(double *x, double *y, int size);
float rad2deg(double rad);


#endif
