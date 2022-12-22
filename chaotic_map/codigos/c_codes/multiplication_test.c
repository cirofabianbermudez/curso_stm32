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
short _power;

void inicializa( int a, int b ){		
	_a = a;								// A(a,b) representacion en punto fijo, cargamos variables globables 
	_b = b;
	_power = (short)1<< _b;					// Calculamos el factor para la conversion
}

short setNumber( double v ){				// Convierte a punto fijo con truncamiento 
	return ( (short)(v*_power) );
}

double getNumber( short r ){				// Convierte de vuelta a double
	return ( (double)r/_power);
}

short multTrunc( short x, short y ){		// Multiplicacion con truncamiento
	int r;
	int a=0;
	int b=0;
	a = x;
	b = y;
	r = a*b;
	printf("Mult antes  : %8.8x\n",r);
	r = r >> _b;
	printf("Mult despues: %8.8x\n",r);
	return( r );
}

short multTrunc2(short u, short v, int frac)
{
    short u1 = (u & 0xff);
    short v1 = (v & 0xff);
    short t = (u1 * v1);
    short w3 = (t & 0xff);
    short k = (t >> 8);
	short h;
	short l;
	//short place = 0;
	//short temp = 0;
	short result = 0;
	//int i = 0;
    
	//printf("u1 = %4.4hx\n",u1);
	//printf("v1 = %4.4hx\n",v1);
	//printf("t = %4.4hx\n",t);
	//printf("w3 = %4.4hx\n",w3);
	//printf("k = %4.4hx\n",k);
	
	
    u >>= 8;
    t = (u * v1) + k;
    k = (t & 0xff);
    short w1 = (t >> 8);
    
    v >>= 8;
    t = (u1 * v) + k;
    k = (t >> 8);
    
    h = (u * v) + w1 + k;
    l = (t << 8) + w3;

    /*	
	for(i = 12; i<16; i++){
		place = (short)1<< i;
		temp = (unsigned short)(place & l) >> 12;
		result += temp; 
		//printf("placelo = %4.4hx, %4.4hx, %4.4hx\n",place, temp, result);
	}
	
	for(i = 0; i<12; i++){
		place = (short)1<< i;
		temp = (unsigned short)(place & h) << 4;
		result += temp; 
		//printf("placehi = %4.4hx, %4.4hx, %4.4hx\n",place, temp, result);
	}
	*/
	result = (unsigned short)(l >> frac) + (unsigned short)(h << 16 - frac);
	printf("result = %4.4hx \n",result);
	printf("h = %4.4hx, l = %4.4hx \n",h, l);
	return result;
}


int main(int argc, char *argv[]){

	short xi, yi, z1, z2;
	int entera;
	int frac;
	
	// Parametros de punto fijo
	entera = 3;
	frac = 12; 
	inicializa( entera, frac );
	printf(" Representacion A(a,b) = A(%d,%d)\n a: entera\tb: fraccionaria\n",entera,frac);
	
	// Conversion a punto fijo
	xi = setNumber( -1.126523454354 );
	yi = setNumber( 0.63453454565 );
	printf(" # x0 real: %16.13f\t %4.4hx\n",getNumber(xi), xi );
	printf(" # y0 real: %16.13f\t %4.4hx\n",getNumber(yi), yi );
	
	z1 = multTrunc(xi,yi);
	printf("Result1 = %16.13f\t %4.4hx\n", getNumber(z1), z1 );
	
	z2 = multTrunc2(xi,yi,frac);
	printf("Result2 = %16.13f\t %4.4hx\n", getNumber(z2), z2 );
	
	return 0;
}
// gcc -o simulation simulation.c
// ./simulation 