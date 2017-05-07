#include <stdio.h>
#include <stdlib.h>

int main()
{
    int i = 0;
    int P1OUT = 0x00;
    int P1OUT2 = 0x00;
    int BIT2 = 0x04,BIT4 = 0x10;
    int sDATA = 0b00110011001100110;
	    for(i = 0; i < 16; i++){
	        P1OUT = (0xFF &(BIT2 & (sDATA >> i)));  // 0000 0100
	        printf("P1OUT %d\n", P1OUT);
	        if(i == 15)
	            P1OUT2 |= BIT4;
            printf("P1OUT2 %d", P1OUT2);
	    }
	    P1OUT2 &= ~BIT4;

	return 0;
}
