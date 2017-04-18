#include <stdio.h>
#include <stdlib.h>
#include <math.h>
int Primalidade(unsigned int x){
	    unsigned int y = 0;
	    int i;
            for(i = 1; i <= x; i++){
                if((x%i)== 0){
                    y++;
                }
            }
            if(y == 2)
                return 1;
            else
                return 0;
	}
int main()
{
int z;

z = Primalidade(163);
printf("%d", z);

return 0;
}
