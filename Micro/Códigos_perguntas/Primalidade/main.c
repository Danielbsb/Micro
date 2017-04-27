#include <stdio.h>
#include <stdlib.h>
    Resto(int A, int B){
    int i = A;
    while(i >= B){
        i -= B;
    }
    return i;
    }
int main()
{
    int j = Resto(10,7);
    printf("%d",j);
}
