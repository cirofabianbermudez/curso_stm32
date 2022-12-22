/*
	Autor:	Ciro Fabian Bermudez Marquez
	Nombre:	Simulador de diseños en VHDL de 64 bits en punto fijo
	Agracecimientos: Dr. Luis Gerardo de la Fraga y Dr. Cuauhtemoc Mancillas López
*/

#include <stdio.h>
#include <stdlib.h>

/* Variables globales */
int _a;						// parte entera
int _b;						// parte fraccionaria
long long _power;

void inicializa( int a, int b ){		
	_a = a;								// A(a,b) representacion en punto fijo, cargamos variables globables 
	_b = b;
	_power = (long long)1<< _b;					// Calculamos el factor para la conversion
}

long long setNumber( double v ){				// Convierte a punto fijo con truncamiento 
	return ( (long long)(v*_power) );
}

double getNumber( long long r ){				// Convierte de vuelta a double
	return ( (double)r/_power);
}


long long multTrunc( long long x, long long y ){		// Multiplicacion con truncamiento
	__int128 r;
	__int128 a=0;
	__int128 b=0;
	a = x;
	b = y;
	r = a*b;
	printf("Mult antes  : %16.16llx %16.16llx \n",(long long)(r >> 64), (long long)r);
	r = r >> _b;
	printf("Mult despues: %16.16llx %16.16llx \n",(long long)(r >> 64), (long long)r );
	return( r );
}


long long multTrunc2(long long u, long long v, int frac){
    long long u1 = (u & 0xffffffff);
    long long v1 = (v & 0xffffffff);
    long long t = (u1 * v1);
    long long w3 = (t & 0xffffffff);
    long long k = (t >> 32);
	long long h;
	long long l;
	long long result = 0;
		
    u >>= 32;
    t = (u * v1) + k;
    k = (t & 0xffffffff);
    long long w1 = (t >> 32);
    
    v >>= 32;
    t = (u1 * v) + k;
    k = (t >> 32);
    
    h = (u * v) + w1 + k;
    l = (t << 32) + w3;
	printf("h = %16.16llx, l = %16.16llx \n",h, l);
	printf("left = %16.16llx, right = %16.16llx \n", (unsigned long long)(h << 64 - frac), ((unsigned long long)l >> frac) );
	result = ((unsigned long long)l >> frac ) | (unsigned long long)(h << 64 - frac);
	//printf("result = %16.16llx \n",result);
	//printf("h = %16.16llx, l = %16.16llx \n",h, l);
	return result;
}


int main(int argc, char *argv[]){

	long long xi, yi, z1, z2;
	int entera;
	int frac;
	
	// Parametros de punto fijo
	entera = 10;
	frac = 64 - entera - 1;  // 53
	inicializa( entera, frac );
	printf(" Representacion A(a,b) = A(%d,%d)\n a: entera\tb: fraccionaria\n",entera,frac);
	
	// Conversion a punto fijo
	xi = setNumber( -3.2354334523452324534 );
	yi = setNumber( -2.23452433452323452345 );
	printf(" # x0 real: %16.13f\t %16.16llx\n",getNumber(xi), xi );
	printf(" # y0 real: %16.13f\t %16.16llx\n",getNumber(yi), yi );
	
	z1 = multTrunc(xi,yi);
	printf("Result1 = %16.13f\t %16.16llx\n", getNumber(z1), z1 );
	
	z2 = multTrunc2(xi,yi,frac);
	printf("Result2 = %16.13f\t %16.16llx\n", getNumber(z2), z2 );
	
	return 0;
}
// gcc -o simulation simulation.c
// ./simulation 