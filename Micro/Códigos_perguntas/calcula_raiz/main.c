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
        printf("A raiz e %f", x );
    }
int main()
{

unsigned int S = 25;

Raiz_Quadrada(S);

return 0;
}
