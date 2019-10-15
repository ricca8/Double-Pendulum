#ifndef calcoli_h
#define calcoli_h

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "macros.h"

void         *pend (void *arg);
void         xy2pixel(int i);
void         RUN_KUT(int i);
void         INC(int i);
void         KU_1(int i);
void         KU_2(int i);
void         KU_3(int i);
void         KU_4(int i);
double       fun1(double t1, double t2, double t1_vel, double t2_vel, int i);
double       fun2(double t1, double t2, double t1_vel, double t2_vel, int i);


#endif
