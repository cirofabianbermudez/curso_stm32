#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* Variables globales */
char _a;        // parte entera
char _b;        // parte fraccionaria
char _power;

void inicializa( char a, char b ){      
    _a = a;             // A(a,b) representacion en punto fijo, cargamos variables globables 
    _b = b;
    _power = (char)1<< _b;      // Calculamos el factor para la conversion
}

char setNumber( double v ){     // Convierte a punto fijo con truncamiento 
    return ( (char)(v*_power) );
}

double getNumber( char r ){     // Convierte de vuelta a double
    return ( (double)r/_power);
}

char multTrunc( char x, char y, short *res){    // Multiplicacion con truncamiento
    short r;
    short a=0;
    short b=0;
    a = x;
    b = y;
    r = a*b;
    *res = r;
    r = r >> _b;
    return r;
}

char multTrunc2(char u, char v, short *res){
    char u1 = (u & 0xf);
    char v1 = (v & 0xf);
    char t = (u1 * v1);
    char w3 = (t & 0xf);
    char k = ((unsigned char)t >> 4);
    char h;
    char l;
    char result = 0;
    char frac = 3;
        
    u >>= 4;
    t = (u * v1) + k;
    k = (t & 0xf);
    char w1 = (t >> 4);
    
    v >>= 4;
    t = (u1 * v) + k;
    k = (t >> 4);
    
    h = (u * v) + w1 + k;
    l = (t << 4) + w3;

    //printf("check = %2.2hhx, %2.2hhx\n",h,l);
    result = ((unsigned char)l >> frac ) | (unsigned char)(h << 8 - frac);
    *res = ( (short)h << 8 ) | ( (short)l & 0x00ff );
    return result;
}

int main(void){
    inicializa(4,3);
    printf("Rango = [%f,%f]\n\n",-pow(2.0,4), pow(2.0,4)-pow(2.0,-3));
    char a = 0;
    char b = 0;
    char r1 = 0;
    char r2 = 0;
    short res1 = 0;
    short res2 = 0;
    short *dir1 = &res1;
    short *dir2 = &res2;
    int check;

    a = setNumber( 0.0 );
    b = setNumber( 0.0 );

    
    printf("a_hex, a,      b_hex, b, r1_hex, r1, res1 ,r2_hex, r2, res2, equal\n");
    for(int i = 0; i<=255; i++){
        for(int j = 0; j<=255; j++){
            r1 = multTrunc(a,b, dir1);
            r2 = multTrunc2(a,b, dir2);
            check = (res1 == res2) ? 1 : 0;
            if(1){
                printf("%2.2hhx, %12.8lf, ", a, getNumber(a) );
                printf("%2.2hhx, %12.8lf, ", b, getNumber(b) );
                printf("%2.2hhx, %12.8lf, %4.4hx, ",r1, getNumber(r1), res1 );
                printf("%2.2hhx, %12.8lf, %4.4hx, ",r2, getNumber(r2), res2 );
                printf("%d\n",check);
            }
            a++;
        }
        b++;
    }
    return 0;
}

