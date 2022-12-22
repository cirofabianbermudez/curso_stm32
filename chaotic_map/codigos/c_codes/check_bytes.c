/*
	Autor:  		Ciro Fabian Bermudez Marquez
    Descripcion: 	Checar cantidad de bytes en data types del sistema
*/
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
	// Data type <float> tiene 4 bytes, es decir 32 bits.
	printf("float is %lu bytes.\n",sizeof(float));
	
	// Data type <double> tiene 8 bytes, es decir 64 bits.
	printf("double is %lu bytes.\n",sizeof(double));
	
	// Data type <__int128> tiene 16 bytes, es decir 128 bits.
	printf("__int128 is %lu bytes.\n",sizeof(__int128));

	// Data type <long> tiene 8 bytes, es decir 64 bits.
	printf("long is %lu bytes.\n",sizeof(long));

	// Data type <int> tiene 4 bytes, es decir 32 bits.
	printf("int is %lu bytes.\n",sizeof(int));

	// Data type <short> tiene 2 bytes, es decir 16 bits.
	printf("short is %lu bytes.\n",sizeof(short));
	
	// Data type <char> tiene 1 bytes, es decir 8 bits.
	printf("char is %lu bytes.\n",sizeof(char));
	
	return 0;
}