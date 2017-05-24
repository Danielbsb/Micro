
#include "io430.h"

// função para enviar bits serialmente.
/*void serial_to_paralell(void){
  
int m = BIT0;
int b;
for(b = 0; b<8; b++)
{
  if(P1IN & m) 
    P2OUT |= BIT0;
  else 
    P2OUT &= ~BIT0;
  m *= 2;
}
  P2OUT &= ~BIT0;
  Atraso(1);
}
*/
// função que calcula delay de "X" ms
  void Atraso(volatile unsigned int x){
    
    int i = 0;
    TA0CTL = MC_1 + ID_0 + TASSEL_2;
    TA0CCR0 = 999;
    
    while(i < x){
      if(TA0CTL & TAIFG){
          i ++;
          TA0CTL &= ~TAIFG;
      }
    }
    TA0CTL = MC_0;  
    }

int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  P1OUT &= ~BIT0;
  P1DIR |= BIT0;
  //TACCR0 = 62500;
  //TACTL = TASSEL_2 + ID_3 + MC_3 +TAIE;
 // __bit_SR(GIE + LPM0_bits);
  ;for(;;)
  {
    P1OUT |= BIT0;
    Atraso(1000);
    P1OUT &= ~BIT0;
  }
 /* 
  // Para testar a função atraso.
   int j = 10;
   Atraso(j);
 */ 
  return 0;
}
// interrupção para piscar led com interrupção do timerA. 

/*interrupt (TA0IFG) TAISR(void)
{
  P1OUT ^= BIT0;
  TACTL &= ~TAIFG;
}´*/

