#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//Função que calcula duplo fatorial.
int DuploFatorial(unsigned int x){
    unsigned int Result = 1;
    int i;
    for(i = x; i >= 0; i--){
        if((((x%2) == 0)&& ((i%2) == 0))||(((x%2)!= 0)&& ((i%2) != 0))){
            Result = Result*i;
        }
    }
    return Result;
}
int main()
{
    int x;
    x = DuploFatorial(2);
    printf("%d",x);
    return 0;
}
