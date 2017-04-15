#include <stdio.h>
#include <stdlib.h>

    unsigned int Raiz_Quadrada(unsigned int S){
        int inter = 15;
        float x0 = S;
        float x;
        int n;
        for(n = 0; n <= inter; n++){
            x = (x0 + (S/x0))/2;
            x0 = x;
        }
        return (unsigned int)x;
    }
   /* int Potencia(int x, int N){
        int i, P;
        P = 1;


        if(x > 1 && N < 1){
                P = 0;
            for(i = 0; i < N; i++){
                    P = P*x;
            }
        }
        if((N%2) != 0 && x < 0)
            P = -P;
        return P;
    }*/

    int Potencia(int x, int N){
	int sign = 0, p = 1;
	if(N < 0){
		N =-N;
		sign = 1;
	}
	while(N > 0){
		p = x*p;
		N--;
	}
	if(sign == 1)
        return 1/p;
	else
		return p;
}
int main()
{

unsigned int S = 120;
unsigned int saida;
int saida2;

saida = Raiz_Quadrada(S);
saida2 = Potencia(0,2);
printf("ResultadoRaiz %d ResultadoPotencia %d", saida,saida2);
return 0;
}
