#include <stdio.h>
#include <stdlib.h>
#define CLK BIT2
#define LOAD BIT4
#define DIN BIT5

// First Try
/*	    for(i = 0; i < 16; i++){
	        P1OUT &= ~((sDATA >> i)^(0x01));  // 0000 0100
	        printf("P1OUT %d\n", P1OUT);
	        if(i == 15)
	            P1OUT |= BIT4;
            printf("P1OUT2 %d", P1OUT2);
	    }
	    P1OUT2 &= ~BIT4;
*/

/* second try
//Global variables
int P1OUT = 0x10;
int P1OUT2 = 0x00;
int BIT2 = 0b00000100,BIT4 = 0b00010000,BIT5 = 0b00100000;


//Funções drivers para MAX7219;
void scanByte(unsigned int sDATA){ // Função para ler 1 byte de cada vez e enviar ao MAX7219.
  int i = 8;
  unsigned mask;
  while(i > 0) {
    mask = 0x01 << (i - 1); // fazendo a mascara para varrer "DATA".
    P1OUT &= ~CLK;  // set clock to zero => XXXX X0XX
    if (sDATA & mask){   // verifica o bit a ser enviado para o Reg
    P1OUT |= DIN;   // Envia 1 para o registrador através de DIN => XXX1 XXXX
    }else{
    P1OUT &= ~DIN;  // Envia 0 ao DIN
    }
    P1OUT |= CLK;   // Ao fim da leitura de DATA, setar clock para 1
    --i;            // mover para próximo bit.
}
} // end scanByte;
void sendCoord( unsigned int dig, unsigned int seg) { // carrega 2 bytes e os envia para o registrador.
//                           LINHA             COLUNA
//maxSingle is the "easy"  function to use for a single max7219
  int col = 1;
  int a = 0;
    if(seg == 0){
        col = 1;
    }else{
        for(a = 1; a < seg; a++)
            col *= 2;
    }

  P1OUT &= ~LOAD; // LOAD inicializa como zero.
  scanByte(dig);//scaneia os bits referentes às linhas ou ao endereço do REGISTRADOR
  scanByte(col);//scaneia os segmentos a serem ligados.((data & 0x01) * 256) + data >> 1); // put data
  P1OUT &= ~LOAD; // espera 1 ciclo de clk
  P1OUT |= LOAD; // carrega os dados (D0-D15) para o registrador.
} // end sendWord;

int main()
{
//    int i = 0;
    int linha = 8;
    int coluna= 1;
    sendCoord(8,1);
    printf("coluna %d", P1OUT);

//Second Try

	return 0;
}
*/
const int max7219_reg_noop        = 0x00;
const int max7219_reg_digit0      = 0x01;
const int max7219_reg_digit1      = 0x02;
const int max7219_reg_digit2      = 0x03;
const int max7219_reg_digit3      = 0x04;
const int max7219_reg_digit4      = 0x05;
const int max7219_reg_digit5      = 0x06;
const int max7219_reg_digit6      = 0x07;
const int max7219_reg_digit7      = 0x08;
const int max7219_reg_decodeMode  = 0x09;
const int max7219_reg_intensity   = 0x0a;
const int max7219_reg_scanLimit   = 0x0b;
const int max7219_reg_shutdown    = 0x0c;
const int max7219_reg_displayTest = 0x0f;
int P1OUT = 0x00;
int BIT2 = 0b00000100,BIT4 = 0b00010000,BIT5 = 0b00100000;
const int maxInUse = 1;
//Funções drivers para MAX7219;
void scanByte(unsigned int sDATA){ // Função para ler 1 byte de cada vez e enviar ao MAX7219.
  int i = 8;
  unsigned mask;
  while(i > 0) {
    mask = 0x01 << (i - 1); // fazendo a mascara para varrer "DATA".
    P1OUT &= ~CLK;  // set clock to zero => XXXX X0XX
    printf(" CLOCK FALLING EVENT %d\n", P1OUT);
    if (sDATA & mask){   // verifica o bit a ser enviado para o Reg
    P1OUT |= DIN;   // Envia 1 para o registrador através de DIN => XXX1 XXXX
    printf(" DIN EVENT %d\n", P1OUT);
    }else{
    P1OUT &= ~DIN;  // Envia 0 ao DIN
    printf(" DIN EVENT %d\n", P1OUT);
    }
    P1OUT |= CLK;   // Ao fim da leitura de DATA, setar clock para 1
    --i;            // mover para próximo bit.
    printf(" CLOCK RISING EVENT %d\n", P1OUT);
}
} // end scanByte;
void sendCoord( unsigned int dig, unsigned int seg) { // carrega 2 bytes e os envia para o registrador.
//                           LINHA             COLUNA
//maxSingle is the "easy"  function to use for a single max7219
  int col = 1;
  int a = 0;
    if(seg == 0){
        col = 1;
    }else{
        for(a = 1; a < seg; a++)
            col *= 2;
    }

  P1OUT &= ~LOAD; // LOAD inicializa como zero.
  printf(" LOAD FALLING EVENT %d\n", P1OUT);
  scanByte(dig);//scaneia os bits referentes às linhas ou ao endereço do REGISTRADOR
  scanByte(col);//scaneia os segmentos a serem ligados.((data & 0x01) * 256) + data >> 1); // put data
  P1OUT &= ~LOAD; // espera 1 ciclo de clk
  P1OUT |= LOAD; // carrega os dados (D0-D15) para o registrador.
  printf(" LOAD RISING EVENT %d\n", P1OUT);
} // end sendWord;

void setReg(unsigned int reg, unsigned int col) {    // initialize  all  MAX7219's in the system
  int c = 0;
  P1OUT &= ~LOAD;  // begin
  printf(" LOAD FALLING EVENT %d\n", P1OUT);
  for ( c = 1; c <= maxInUse; c++) {
  scanByte(reg);  // specify register
  scanByte(col);//((data & 0x01) * 256) + data >> 1); // put data
    }
  P1OUT &= ~LOAD;

  P1OUT |= LOAD;
  printf(" LOAD RISING EVENT %d\n", P1OUT);
}
int main()
{
//    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    P1OUT = 0x00;
//    P1DIR = (0xFF & ~BIT3);
    setReg(max7219_reg_scanLimit, 0x07);
    setReg(max7219_reg_decodeMode, 0x00);
    setReg(max7219_reg_shutdown, 0x01);
    setReg(max7219_reg_displayTest, 0x00);
    setReg(max7219_reg_intensity, 0x0f & 0x0f);
//    int i = 0;
//    int linha = 8;
//    int coluna= 1;
    //for(;;){

        sendCoord(8,1);
    //}
//  printf("coluna %d", P1OUT);

//Second Try

    return 0;
}
