# Implementación de mapa caótico en STM32 con arquitectura de punto fijo

El nombre del articulo es **Designing an authenticated Hash function with a 2D chaotic map**, 2021, de los autores Luis Gerardo De la Fraga, Cuauhtemoc Mancillas-López y Esteban Tlelo-Cuautle.
$$
\begin{array}{ccl}
		x_{n+1} & = &  a_{1} + a_{2}x_{n} + a_{3}x_{n}^{2} + a_{4}x_{n}y_{n} + a_{5}y_{n} + a_{6}y_{n}^{2}\\
		y_{n+1} & = &  a_{7} + a_{8}x_{n} + a_{9}x_{n}^{2} + a_{10}x_{n}y_{n} + a_{11}y_{n} + a_{12}y_{n}^{2}
		\end{array}
$$
donde los parámetros $\{a_{1}, a_{2}, \ldots a_{12}\}$ pueden tomar diferentes valores, sin embargo para este trabajo se usaron los siguientes:
$$
M_{1} = \{ −0.6,−0.1,1.1,0.2, −0.8, 0.6, −0.7, 0.7, 0.7, 0.3, 0.6, 0.9\}
$$
y las condiciones iniciales:
$$
x_{0} = 0.1 , \qquad y_{0} = 0.2
$$

## 1. Simulación en MATLAB

Lo primero que se tiene que hacer es analizar el comportamiento del sistema en punto flotante y ver la forma del atractor del sistema, en el siguiente código esta la simulación del atractor.

```matlab
clear; close all; clc;
% Iteraciones
iter = 100000;

% Parametros
a = [-0.6, -0.1, 1.1, 0.2, -0.8, 0.6, -0.7, 0.7, 0.7, 0.3, 0.6, 0.9]; % M1
% a = [-1.0, 0.9, 0.4, -0.2, -0.6, -0.5,0.4, 0.7, 0.3, -0.5, 0.7, -0.8]; % M2
x0 = 0.1; y0 = 0.2; % Condiciones iniciales

% Creacion de vectores iniciales
xn = zeros(1,iter+1); yn = zeros(1,iter+1);

% Asignacion de condicion inicial
xn(1) = x0; yn(1) = y0;

for i = 2:iter+1
    xn(i) = a(1) + a(2)*xn(i-1) + a(3)*xn(i-1)^(2) + a(4)*xn(i-1)*yn(i-1) + a(5)*yn(i-1) + a(6)*yn(i-1)^(2);
    yn(i) = a(7) + a(8)*xn(i-1) + a(9)*xn(i-1)^(2) + a(10)*xn(i-1)*yn(i-1) + a(11)*yn(i-1) + a(12)*yn(i-1)^(2);
end

plot(xn,yn,'.k','MarkerSize',0.5); 
grid on; grid minor;
xlabel("xn"); 
ylabel("yn"); 
axis equal;
```

<img src="imagenes\simulacion.png" alt="simulacion" style="zoom:80%;" />

## 2. Representación de punto fijo

La representación de punto fijo de un número $X$ es $X(a,b)$ donde $a$ es la parte entera y $b$ es la parte fraccionaria. De manera que el número de bits en la representación es $a + b + 1$, es decir la parte entera más la parte fraccionaria más el bit de signo. El rango de valores que puede tomar es   $[-2^{a},2^{a}-2^{-b}]$. Por ejemplo:

| Número  | Conversión | Formato $X(a,b)$ | Rango $[-2^{a},2^{a}-2^{-b}]$ |
| ------- | ---------- | ---------------- | ----------------------------- |
| $01110$ | $3.5$      | $X(2,2)$         | $[-4,3.75]$                   |
| $10010$ | $-3.5$     | $X(2,2)$         | $[-4,3.75]$                   |
| $00011$ | $0.75$     | $X(2,2)$         | $[-4,3.75]$                   |





## 3. Algoritmo en punto fijo en C

La idea fundamental detrás de simular los diseños digitales en C antes de realizar su implementación en STM32 es poder comprobar que las arquitecturas, ya sean de 16, 32 o 64 bits utilizando la metodología de punto fijo funcionen correctamente desde el punto de vista de diseño.

### 3.1 Tipos de dato en C

El tipo de dato es el primer punto fundamental para la simulación. Existen diversos tipos de datos en lenguaje C sin embargo para arquitecturas con tamaños de bits definidos podemos reducirlo a cuatro: `__int128`, `long`, `int`, `short`. Cada uno de estos tipos dato tiene una cantidad definida de bytes asociados dependiendo de la computadora y el compilador, para revisar la cantidad de bytes podemos utilizar el siguiente código:

```c
/*
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
```

El formato `%lu` es para ver la cantidad de bytes también conocida como representación unsigned long.

Para compilar y después ejecutar el programa utilizamos las siguientes lineas de código en consola:

```she
gcc -o check_data_types check_data_types.c 
./check_data_types
```

En el caso particular del sistema que estamos utilizado, tenemos un procesador Ryzen 5 3600 corriendo en el sistema operativo MX Linux y la salida del programa es la siguiente:

```shell
float is 4 bytes.
double is 8 bytes.
__int128 is 16 bytes.
long is 8 bytes.
int is 4 bytes.
short is 2 bytes.
char is 1 bytes.
```



| Tipo                                   | Bytes | Bits | Caracteres en HEX |
| -------------------------------------- | ----- | ---- | ----------------- |
| `char`                                 | 1     | 8    | 2                 |
| `short`                                | 2     | 16   | 4                 |
| `int`                                  | 4     | 32   | 8                 |
| `long`                                 | 8     | 64   | 16                |
| `__int64` o `long long` **IMPORTANTE** | 8     | 64   | 16                |
| `__int128`                             | 16    | 128  | 32                |

**NOTA IMPORTANTE:** si se esta trabajando con windows y gcc es importante que todas los `long` se cambien por `__int64` o `long long`, por alguna razón extraña los `long` en windows son de 4 bytes .



Lo primero es definir variables globales para la parte entera y la parte fraccionaria para la representación de punto fijo, además una variable que almacene el factor de conversión entre punto flotante y punto fijo con el método de truncamiento. Las primeras dos variables son de tipo `int` por facilidad y el factor de conversión debe ser del tamaño de la arquitectura que será de $64$ bits.

```c
/* Variables globales */
int _a;			// parte entera
int _b;			// parte fraccionaria
long _power;	// factor de conversion
```

En lenguaje C, un corrimiento a la izquierda (Left Shift) se realiza con el operador `<<`, por ejemplo para realizar un corrimiento del entero `x` dos bits a la izquierda se escribe `x << 2`.

Después creamos una función para inicializar las variables globales y el factor de conversión.

```c
// A(a,b) representacion en punto fijo
void inicializa( int a, int b ){        
	_a = a;					// _a: parte entera
    _b = b;					// _b: parte fraccionaria
    _power = (long)1 << _b;	// calculo de factor de conversion
}
```

Para convertir un número de punto flotante a uno de punto fijo unicamente es necesario multiplicarlo por el factor de conversión, para entenderlo analicemos el siguiente ejemplo:

Si tuviéramos disponibles unicamente $4$ bits y seleccionamos una representación de $A(1,2)$ tenemos un rango de $[-2,1.75]$, si elegimos el número $1.5$ para convertir y lo multiplicamos por $2^{2}$, es decir por $2^{b}$ donde $b$ es la parte fraccionaria, $1.5\times2^2 = 6$, que en binario es $0110$, el cual es precisamente $1.5$ en punto fijo. El código anterior hace el corrimiento del número $1$ a la izquierda `_b` bits lo cual resulta ser una operación muy eficiente, alternativamente se podría cambiar por `_power = (long)pow(2,_b)`. Algo de suma importancia es que $a+b+1 = 4$ para este ejemplo de lo contrario la conversión no se realizará de manera correcta. Para arquitecturas de una cantidad mayor de bits se tiene que sustituir el $4$ por el número de bits de la arquitectura y se debe cumplir que $a+b+1=\text{Bits}_{\text{arq}}$, no deben faltar ni sobrar bits.

Por lo tanto la función para convertir a punto fijo con truncamiento es la siguiente:

```c
// Convierte a punto fijo con truncamiento 
long setNumber( double v ){
    return ( (long)(v*_power) );
}
```

Para convertir de vuelta a de punto fijo a punto flotante teniendo en cuenta que en el primer proceso de conversión de punto flotante a punto fijo existe una perdida de información por el truncamiento y por la cantidad de bits disponibles. unicamente es necesario dividir el número en punto fijo por el factor de conversión. Por ejemplo si tenemos el número binario de $4$ bits $0110$ el cual es $1.5$ en punto fijo en el formato $A(1,2)$ y $6$ en decimal sin signo y lo dividimos por el factor de conversión, $6/2^2 = 1.5$.

Por lo tanto la función para convertir de punto fijo a punto flotante es la siguiente:

```c
// Convierte de vuelta a punto flotante
double getNumber( long r ){
    return ( (double)r/_power);
}
```

La suma y la resta ya están definidas en las operaciones básicas de C, como el tipo `long` es un entero no es necesario definir una nueva función para estas operaciones en punto fijo ya que estas operan de la misma manera en ambos dominios, sin embargo para realizar la multiplicación en punto fijo es necesario truncar el resultado. En aritmética binaria la multiplicación requiere el doble de bits, por lo tanto es necesario utilizar un tipo de dato con el doble de bits para almacenar el resultado y después truncar el resultado en el lugar correcto. Como ejemplo consideremos el número b inario de $4$ bits $0010$ el cual es $0.5$ en punto fijo en el formato $A(1,2)$ y $2$ en decimal sin signo, si multiplicamos $0.5\times 0.5 = 0.25$ y en decimal sin signo $2 \times 2 = 4$, sabemos de antemano que si multiplicamos dos números de $4$ bits se requieren $8$ bits para el resultado, convirtiendo a binario la representación decimal sin signo resulta $0000\:\:0100$, y para que este número binario represente $0.25$ el formato resultante de la multiplicación en punto fijo debe ser $M(3,4)$ o de manera general $M(2a+1,2b)$, finalmente como unicamente debemos tener $4$ bits debemos realizar un corrimiento a la derecha $b$ veces, en este caso $2$ veces y obtenemos $0000\;\;0001$, tomando unicamente los primeros $4$ bits resulta $0001$ el cual representa $0.25$ en el formato de punto fijo $A(1,2)$ original.

El tipo de dato `__int128` es lo que utilizaremos para almacenar el resultado y después utilizando un corrimiento a la derecha $b$ veces y convirtiendo el resultado a `long` truncamos el resultado.

Utilizado lo anterior describimos una función para multiplicación de punto fijo con truncamiento de la siguiente manera:

```c
// Multiplicacion de punto fijo con truncamiento
long multTrunc( long x, long y ){
    __int128 r;
    __int128 a=0;
    __int128 b=0;
    a = x;
    b = y;
    r = a*b;
    r = r >> _b;
    return( r );
}
```

Finalmente para mostrar en pantalla información de la simulación y poder guardar el resultado de esta en un archivo de texto utilizamos los siguientes comandos:

```c
printf(" Representacion A(a,b) = A(%d,%d)\n a: entera\tb: fraccionaria\n",entera,frac);
```

```c
// Archivo de texto
FILE *fpointer = fopen("salida.txt","w");
	fprintf(fpointer,"%2.20f\t%lx\n", getNumber( x ),x);
fclose(fpointer);
```

La primera línea crea un apuntador para guardar en  `salida.txt` los resultados de la simulación, `%lx` es el formato de en hexadecimal de un tipo `long` y finalmente hay que cerrar el archivo de texto en el que se esta escribiendo.

Una vez teniendo todas las herramientas anteriores podemos hacer cualquier tipo de simulación de punto fijo en las arquitecturas de 16, 32 y 64 bits, únicamente hay que tener presente que:

| Arquitectura | Tipo    | Multiplicación |
| ------------ | ------- | -------------- |
| 16           | `short` | `int`          |
| 32           | `int`   | `long`         |
| 64           | `long`  | `__int128`     |

Para ejecutar el código de las plantillas se 

```shell
gcc -o simulacion simulacion.c -lm
./simulacion
```



## 4. Simulación en C

```c
/*
	Nombre:	Simulador de diseños en VHDL de 64 bits en punto fijo
*/

#include <stdio.h>
#include <stdlib.h>

/* Variables globales */
int _a;						// parte entera
int _b;						// parte fraccionaria
long long _power;

/*
	Para un Ryzen 5 3600 en MxLinux
	
	Data type <long long> tiene 8 bytes, es decir 64 bits.
	printf("long long is %lu bytes.\n",sizeof(long long));
	
	Data type <__int128> tiene 16 bytes, es decir 128 bits.
	printf("__int128 is %lu bytes.\n",sizeof(__int128));
	
	Nota1: Se hizo la prueba en una Raspberry pi 4 y no es compatible __int128
	Nota2: De preferencia trabajar con gcc de linux o mac, falta hacer prueba en windows
	
	Para compilar ejecutar:
		gcc -o map1 map1.c
*/

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
	r = r >> _b;
	return( r );
}

int main(int argc, char *argv[]){

	// Archivo de texto
  	FILE *fpointer = fopen("salida.txt","w");
  	
	long long ai[12];
	long long xi, yi, xni, yni, x2i, y2i, xyi;
	double x0;
	double y0;
	int entera;
	int frac;
	int i;
	
	// Parametros de punto fijo
	entera = 3;
	frac = 60; 
	inicializa( entera, frac );
	printf(" Representacion A(a,b) = A(%d,%d)\n a: entera\tb: fraccionaria\n",entera,frac);

	// Valores iniciales
	x0 = 0.1;
	y0 = 0.2;
	printf(" # x0: %f\n", x0 );
	printf(" # y0: %f\n", y0 );
	
	// Conversion a punto fijo
	xi = setNumber( x0 );
	yi = setNumber( y0 );
	printf(" # x0 real: %2.10f\n",getNumber(xi) );
	printf(" # y0 real: %2.10f\n",getNumber(yi) );
	
	// Parametros de mapa 1
	ai[0] = setNumber( -0.6 );
	ai[1] = setNumber( -0.1 );
	ai[2] = setNumber(  1.1 );

	ai[3] = setNumber(  0.2 );
	ai[4] = setNumber( -0.8 );
	ai[5] = setNumber(  0.6 );

	ai[6] = setNumber( -0.7 );
	ai[7] = setNumber(  0.7 );
	ai[8] = setNumber(  0.7 );

	ai[9]  = setNumber( 0.3 );
	ai[10] = setNumber( 0.6 );
	ai[11] = setNumber( 0.9 );
	
	fprintf(fpointer,"%2.20f,%2.20f\n",getNumber( xi ), getNumber( yi ));
	for(i = 0; i<100000; i++){
	    xni = ai[0] + multTrunc( multTrunc( ai[2] , xi )  + ai[1] , xi) + multTrunc(multTrunc(xi,yi), ai[3]) +
	           multTrunc( multTrunc( ai[5] , yi )  + ai[4] , yi);
	    
	   yni = ai[6] + multTrunc( multTrunc( ai[8] , xi )  + ai[7] , xi) + multTrunc(multTrunc(xi,yi), ai[9]) +  
                 multTrunc( multTrunc( ai[11] , yi )  + ai[10] , yi);
		
		xi = xni;			 
		yi = yni;
		fprintf(fpointer,"%2.20f,%2.20f\n",getNumber( xi ), getNumber( yi ));
	}
	
	fclose(fpointer);
	return 0;
}
// gcc -o simulacion simulacion_windows.c
// ./simulation 
```



### 4.1. Simulación en C con salida hexadecimal

 ```c
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
 
 /*
 	Para un Ryzen 5 3600 en MxLinux
 	
 	Data type <long long> tiene 8 bytes, es decir 64 bits.
 	printf("long long is %lu bytes.\n",sizeof(long long));
 	
 	Data type <__int128> tiene 16 bytes, es decir 128 bits.
 	printf("__int128 is %lu bytes.\n",sizeof(__int128));
 	
 	Nota1: Se hizo la prueba en una Raspberry pi 4 y no es compatible __int128
 	Nota2: De preferencia trabajar con gcc de linux o mac, falta hacer prueba en windows
 	
 	Para compilar ejecutar:
 		gcc -o map1 map1.c
 */
 
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
 	r = r >> _b;
 	return( r );
 }
 
 int main(int argc, char *argv[]){
 
 	// Archivo de texto
   	FILE *fpointer = fopen("salida.txt","w");
   	
 	long long ai[12];
 	long long xi, yi, xni, yni, x2i, y2i, xyi;
 	double x0;
 	double y0;
 	int entera;
 	int frac;
 	int i;
 	
 	// Parametros de punto fijo
 	entera = 3;
 	frac = 60; 
 	inicializa( entera, frac );
 	printf(" Representacion A(a,b) = A(%d,%d)\n a: entera\tb: fraccionaria\n",entera,frac);
 
 	// Valores iniciales
 	x0 = 0.1;
 	y0 = 0.2;
 	printf(" # x0: %f\n", x0 );
 	printf(" # y0: %f\n", y0 );
 	
 	// Conversion a punto fijo
 	xi = setNumber( x0 );
 	yi = setNumber( y0 );
 	printf(" # x0 real: %2.10f\n",getNumber(xi) );
 	printf(" # y0 real: %2.10f\n",getNumber(yi) );
 	
 	// Parametros de mapa 1
 	ai[0] = setNumber( -0.6 );
 	ai[1] = setNumber( -0.1 );
 	ai[2] = setNumber(  1.1 );
 
 	ai[3] = setNumber(  0.2 );
 	ai[4] = setNumber( -0.8 );
 	ai[5] = setNumber(  0.6 );
 
 	ai[6] = setNumber( -0.7 );
 	ai[7] = setNumber(  0.7 );
 	ai[8] = setNumber(  0.7 );
 
 	ai[9]  = setNumber( 0.3 );
 	ai[10] = setNumber( 0.6 );
 	ai[11] = setNumber( 0.9 );
 	
 	for(i = 0; i<12; i++ ){
 		printf("ai[%d] = %16.16llx\n", i,ai[i] );
 	}
 	
 	for(i = 0; i<12; i++ ){
 		printf("ai[%d] = %lld\n", i, ai[i] );
 	}
 	
 	printf("x0 = %lld\ny0 = %lld\n", xi, yi );
 	
 	printf("x0 = %16.16llx\ny0 = %16.16llx\n", xi, yi );
 
 	
 	fprintf(fpointer,"%16.16llx,%16.16llx\n", xi , yi );
 	for(i = 0; i<10; i++){
 	    xni = ai[0] + multTrunc( multTrunc( ai[2] , xi )  + ai[1] , xi) + multTrunc(multTrunc(xi,yi), ai[3]) +
 	           multTrunc( multTrunc( ai[5] , yi )  + ai[4] , yi);
 	    
 	   yni = ai[6] + multTrunc( multTrunc( ai[8] , xi )  + ai[7] , xi) + multTrunc(multTrunc(xi,yi), ai[9]) +  
                  multTrunc( multTrunc( ai[11] , yi )  + ai[10] , yi);
 		
 		xi = xni;			 
 		yi = yni;
 		fprintf(fpointer,"%16.16llx,%16.16llx\n", xi , yi );
 	}
 	
 	fclose(fpointer);
 	return 0;
 }
 // gcc -o simulation simulation.c
 // ./simulation 
 ```



```
01999999999999a0,0333333333333340
f44dd2f1a9fbe751,f89fbe76c8b4398a
09fe64da752bfc39,f2d9933424caba9a
0ba108d479e4658e,ff89ba6ba3133c5c
fed84cafb67c409c,027dac705657a578
f4cfabb9a90370a6,f5d6e4af6a61ede0
0d8be94ceab09f8e,f44985fbd6ed3692
0e316c30d427c546,040678eda7597da5
00ee710dc6d1bf9f,0bf230b7b7f6217f
f24ce074cafab937,04e4bf243ec09161
00d1799937f25dd2,f67289e3e8a7f860
```



### 4.2. Comprobación de simulación

Para comprobar la simulación en C generamos un archivo de texto que después será leído y graficado por MATLAB.

```matlab
clear; close all; clc;

data = load("../c_codes/salida.txt");
xn = data(:,1);
yn = data(:,2);

plot(xn,yn,'.k','MarkerSize',0.5); 
grid on; grid minor;
xlabel("xn"); 
ylabel("yn"); 
axis equal;
```

<img src="imagenes\simulacion_c.png" alt="simulacion_c" style="zoom:67%;" />

## 5. Implementación en STM32

Lo primero que hay que hacer es poder generar una comunicación entre el STM32 y la computadora, esto lo hacemos utilizando la comunicación SWO (Single Wire), 

### 5.1 Creación de un nuevo proyecto

1. `File/New/STM32 Proyect`, en la pestaña de Board Selector seleccionar la tarjeta Nucleo correspondiente y dar clic en Next.
2. Configuraciones
   1. Target Language: C
   2. Target Binary Type: Executable
   3. Target Projet Type: Empty
3. Para quitar el error `"FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."`, es necesario cambiar las configuraciones de projecto. 
   1. Floting point unit: None
   2. Floating point ABI: Software Implementation

### 5.2 Comunicación SWO

Para activar la comunicación por SWO es necesario modificar el archivo `syscalls.c` y entre los `Includes` y `Variables` pegar el siguiente código:

```c

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//					Implementation of printf like feature using ARM Cortex M3/M4/ ITM functionality
//					This function will not work for ARM Cortex M0/M0+
//					If you are using Cortex M0, then you can use semihosting feature of openOCD
/////////////////////////////////////////////////////////////////////////////////////////////////////////


//Debug Exception and Monitor Control Register base address
#define DEMCR        			*((volatile uint32_t*) 0xE000EDFCU )

/* ITM register addresses */
#define ITM_STIMULUS_PORT0   	*((volatile uint32_t*) 0xE0000000 )
#define ITM_TRACE_EN          	*((volatile uint32_t*) 0xE0000E00 )

void ITM_SendChar(uint8_t ch)
{

	//Enable TRCENA
	DEMCR |= ( 1 << 24);

	//enable stimulus port 0
	ITM_TRACE_EN |= ( 1 << 0);

	// read FIFO status in bit [0]:
	while(!(ITM_STIMULUS_PORT0 & 1));

	//Write to ITM stimulus port0
	ITM_STIMULUS_PORT0 = ch;
}
```

despues es necesario modificar la función `write`como se muestra acontinuación:

```c
__attribute__((weak)) int _write(int file, char *ptr, int len)
{
	int DataIdx;

	for (DataIdx = 0; DataIdx < len; DataIdx++)
	{
		//__io_putchar(*ptr++);
        ITM_SendChar(*ptr++);
	}
	return len;
}
```



FInalmente para saber que la comunicación es correcta, debemos correr el microcontrolador en modo debug, pero antes es necesario configurarla:

1. `Debug as -> Debug configurations -> (Doble clic)STM32 -> Pestaña Debuger -> Serial Wire Viewer Enable `
2. Abrir la ventana SWV, `windows -> Show View -> SWV -> Data console`
3. Comenzar el debug, dar clic en switch, y configurar la SWV data console activanlo el port 0, dar clic en `Start Trace`

### 5.3 Tamaño de las variables

Una vez realizado los cambios anteriores podemos ejecutar un programa para ver el tamaño que tienen las variables en nuestro STM32. Es necesario conocer el tamaño de las variables dentro del STM32 para asegurarnos que la arquitectura funcione correctamente, utilizando el siguiente código en C podemos hacer esto:



```c
#include <stdint.h>
#include <stdio.h>

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

int main(void)
{
	// Data type <float> tiene 4 bytes, es decir 32 bits.
	printf("float is %u bytes.\n",sizeof(float));

	// Data type <double> tiene 8 bytes, es decir 64 bits.
	printf("double is %u bytes.\n",sizeof(double));

	// Data type <long long> tiene 8 bytes, es decir 64 bits.
	printf("long long is %u bytes.\n",sizeof(long long));

	// Data type <int> tiene 4 bytes, es decir 32 bits.
	printf("int is %u bytes.\n",sizeof(int));

	// Data type <short> tiene 2 bytes, es decir 16 bits.
	printf("short is %u bytes.\n",sizeof(short));

	// Data type <char> tiene 1 bytes, es decir 8 bits.
	printf("char is %u bytes.\n",sizeof(char));
    /* Loop forever */
	for(;;);
}
```

De lo cual resulta

```
float is 4 bytes.
double is 8 bytes.
long long is 8 bytes.
int is 4 bytes.
short is 2 bytes.
char is 1 bytes.
```





### 5.4 Multiplicador de 64 bits con truncamiento

```c
long long multTrunc( long long u, long long v){		// Multiplicacion con truncamiento
	long long u1 = (u & 0xffffffff);
	long long v1 = (v & 0xffffffff);
	long long t = (u1 * v1);
	long long w3 = (t & 0xffffffff);
	long long k = ((unsigned long long)t >> 32);
	long long h;
	long long l;
	long long result = 0;
	int frac = 60;

	u >>= 32;
	t = (u * v1) + k;
	k = (t & 0xffffffff);
	long long w1 = (t >> 32);

	v >>= 32;
	t = (u1 * v) + k;
	k = (t >> 32);

	h = (u * v) + w1 + k;
	l = (t << 32) + w3;

	result = ((unsigned long long)l >> frac ) | (unsigned long long)(h << 64 - frac);
	return result;
}
```



```c
#include <stdint.h>
#include <stdio.h>

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

*/
long long multTrunc( long long u, long long v){		// Multiplicacion con truncamiento
	long long u1 = (u & 0xffffffff);
	long long v1 = (v & 0xffffffff);
	long long t = (u1 * v1);
	long long w3 = (t & 0xffffffff);
	long long k = ((unsigned long long)t >> 32);
	long long h;
	long long l;
	long long result = 0;
	int frac = 60;

	u >>= 32;
	t = (u * v1) + k;
	k = (t & 0xffffffff);
	long long w1 = (t >> 32);

	v >>= 32;
	t = (u1 * v) + k;
	k = (t >> 32);

	h = (u * v) + w1 + k;
	l = (t << 32) + w3;

	result = ((unsigned long long)l >> frac ) | (unsigned long long)(h << 64 - frac);
	return result;
}


int main(void)
{
	/*Parameters*/
	long long ai[12] = {0xf666666666666680,0xfe66666666666660,0x1199999999999a00,0x0333333333333340,0xf333333333333300,0x0999999999999980,0xf4cccccccccccd00,0x0b33333333333300,0x0b33333333333300,0x04ccccccccccccc0,0x0999999999999980,0x0e66666666666680};

	/*Initial conditions*/
	long long xi = 0x01999999999999a0;
	long long yi = 0x0333333333333340;

	/*Iteration variables*/
	long long xni = 0;
	long long yni = 0;

	/*Fixed point format*/
	//int a = 3;
	//int b = 60;

    /* Loop forever */
	//for(;;);
		int cont = 0;
	/* Loop forever */
	while(1){
		xni = ai[0] + multTrunc( multTrunc( ai[2] , xi )  + ai[1] , xi) + multTrunc(multTrunc(xi,yi), ai[3]) + multTrunc( multTrunc( ai[5] , yi )  + ai[4] , yi);
		yni = ai[6] + multTrunc( multTrunc( ai[8] , xi )  + ai[7] , xi) + multTrunc(multTrunc(xi,yi), ai[9]) + multTrunc( multTrunc( ai[11] , yi )  + ai[10] , yi);
		xi = xni;
		yi = yni;
		for(int i = 0; i<1000000; i++); // Delay por software
		cont++;
		printf("Iteracion %d\n",cont);
		printf("xi = %8.8x%8.8x\t",(int)(xi >> 32),(int)xi);
		printf("yi = %8.8x%8.8x\n",(int)(yi >> 32),(int)yi);
	}
}
```

NOTA IMPORTANTE: Ya se comprobo que funciona exactamente igual tanto en C como en C embebido. El multiplicador custom ya funciona perfectamente. Ya hay copia del proyecto completo en google drive

Códigos importantes:

- simulacion_windows_mult_custom.c
- simulacion_windows.c
