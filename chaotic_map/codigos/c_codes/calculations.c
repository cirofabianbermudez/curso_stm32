#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(void){ 
    double entera = 3.0, frac = 12.0;
    printf("[%16.14lf,%16.14lf]\n",-pow(2.0,entera), pow(2.0,entera)-pow(2.0,-frac));
    printf("Combinaciones = %lf\n",pow(2.0,16.0));
    


    return 0;
}
