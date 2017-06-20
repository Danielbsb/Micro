#include <msp430.h> 

// ---- Portas MSP utilizadas e sua função ----  //
//#define BTN1 BIT3   // move up
//#define BTN2 BIT7   // move down
//#define BTN3 BIT1   // move left
//#define BTN4 BIT0   // move right
#define CLK BIT2    // pino 13 do MAX7219
#define LOAD BIT4   // pino 12 do MAX7219
#define DIN BIT5    // pino 1 do MAX7219
// -------------------------------------------  //
// ----  Coordenadas iniciais do LED  ----   //
    #define x0 0b00010000
    #define y0 4
//  --------------------------------------   //
//---- Variáveis utilizadas para acessar o endereço de cada função do MAX7219 ----//
#define max7219_reg_digit0 0x01
#define max7219_reg_digit1 0x02
#define max7219_reg_digit2 0x03
#define max7219_reg_digit3 0x04
#define max7219_reg_digit4 0x05
#define max7219_reg_digit5 0x06
#define max7219_reg_digit6 0x07
#define max7219_reg_digit7 0x08
#define max7219_reg_decodeMode 0x09
#define max7219_reg_intensity 0x0a
#define max7219_reg_scanLimit 0x0b
#define max7219_reg_shutdown 0x0c
#define max7219_reg_displayTest 0x0f
//--------------------------------------------------------------------------------//
#define PWR_MGMT_1 0x6B      // MPU-6050 register address
#define ACCEL_XOUT_H 0x3B    // MPU-6050 register address
#define ACCEL_XOUT_L 0x3C    // MPU-6050 register address
#define ACCEL_YOUT_H 0x3D    // MPU-6050 register address
#define ACCEL_YOUT_L 0x3E    // MPU-6050 register address
#define ACCEL_ZOUT_H 0x3F    // MPU-6050 register address
#define ACCEL_ZOUT_L 0x40    // MPU-6050 register address
//--------------------------------------------------------------------------------//
#define limite_superior 500
#define limite_medio 250
#define limite_analise 300
//--------------------------------------------------------------------------------//
unsigned char RX_Data[6];
unsigned char TX_Data[2];
unsigned char RX_ByteCtr;
unsigned char TX_ByteCtr;
unsigned char slaveAddress = 0x68;
int xAccel;
int yAccel;

volatile unsigned int up = 0,down = 0, left = 0, right = 0; // Variáveis para deslocar o LED nas coordenadas x e y

const int maxInUse = 1;

int buffer[8];
int limits[8] = {0xff,0x81,0x81,0x81,0x81,0x81,0x80,0xff};
int limits0[8];

int x1;
unsigned int portalIndex = 0;
unsigned int portalX = 0b10000000;
unsigned int portalY = 0b00000010;
//unsigned int zero = 1;
//unsigned int eight = 0;
unsigned int linha = 0; // Coordenadas de movimentação do LED.
unsigned int coluna = 0;
// Teste 2

//Funções drivers para MAX7219 e funções globais do programa:
/*
void move_LED(int direction, int axis){
    int hold = 1;
    if(direction & hold){  // se pressionarmos o botão, o led deve andar uma posição para cima, logo, ele deve saltar de linha.
        axis ++;   // incrementando uma linha;
        hold = 0;   // segurando o hold, necessario pois o ciclo de clock incrementaria mais de uma linha.
    }
    if(up == 0)
        hold = 1;
}
*/

long map(long Accel, long in_min, long in_max, long out_min, long out_max)
{
  return (Accel - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void i2cInit(void)
{
    // set up I2C module
    UCB0CTL1 |= UCSWRST;                // Enable SW reset
    UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;           // I2C Master, synchronous mode
    UCB0CTL1 = UCSSEL_2 + UCSWRST;          // Use SMCLK, keep SW reset
    UCB0BR0 = 10;                   // fSCL = SMCLK/12 = ~100kHz
    UCB0BR1 = 0;
    UCB0CTL1 &= ~UCSWRST;               // Clear SW reset, resume operation
}

void i2cWrite(unsigned char address)
{
    //__disable_interrupt();
    IE2 &= ~UCB0TXIE;                   // desabilita o interrupt
    UCB0I2CSA = address;                // Load slave address
    IE2 |= UCB0TXIE;                // Enable TX interrupt
    while(UCB0CTL1 & UCTXSTP);          // Ensure stop condition sent
    UCB0CTL1 |= UCTR + UCTXSTT;         // TX mode and START condition
    //__bis_SR_register(GIE);
    //while((IFG2 & UCB0TXIFG) == 0);
    __bis_SR_register( LPM0_bits + GIE);        // sleep until UCB0TXIFG is set ...
}

void i2cRead(unsigned char address)
{
    //__disable_interrupt();
    IE2 &= ~UCB0RXIE;                   // desabilita o interrupt
    UCB0I2CSA = address;                // Load slave address
    IE2 |= UCB0RXIE;                // Enable RX interrupt
    while(UCB0CTL1 & UCTXSTP);          // Ensure stop condition sent
    UCB0CTL1 &= ~UCTR;              // RX mode
    UCB0CTL1 |= UCTXSTT;                // Start Condition
    //__bis_SR_register(GIE);
    //while((IFG2 & UCB0RXIFG) == 0);
    __bis_SR_register(LPM0_bits + GIE);        // sleep until UCB0RXIFG is set ...
}

int Debounce_button(int BIT)
{
    unsigned int counter = 1000; // 1Mhz / 1000 = 1ms de debouncing

        while((P1IN & BIT) == 0){  //0000 1000 and 0000 1000
            if(counter == 0)
                return  1;
            else
                counter --;
        }
        if(P1IN & BIT){
            return  0;
        }
            counter = 1000;
}

void delay(volatile unsigned int x)
{

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

void scanByte(unsigned int sDATA)   // Função para ler 1 byte de cada vez e enviar ao MAX7219.
{
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

void sendCoord( unsigned int dig, unsigned int seg)  // carrega 2 bytes e os envia para o registrador do MAX7219.
//             | LINHA(D15:D8)   |  COLUNA (D7:D0)|
{
/*
  int col = 1;
  int a = 0;
    if(seg == 0){
        col = 0;
    }else{
        for(a = 1; a < seg; a++)
            col *= 2;
    }
*/
  P1OUT &= ~LOAD;   // LOAD inicializa como zero
  // Os bits são enviados do MSB para o LSB ou seja (D15,D14,D13 ... D0)
  scanByte(dig);    //scaneia os bits referentes às linhas ou ao endereço do REGISTRADOR (D15 - D8)
  scanByte(seg);    //scaneia os segmentos a serem ligados (D7 - D0)
  P1OUT |= LOAD;    // carrega os dados (D0-D15) para o registrador.
} // end sendCoord;

void setReg(unsigned int reg, int col)    // initialize  all  MAX7219's in the system
{
  int c = 0;
  P1OUT &= ~LOAD;   // begin
  for ( c = 1; c <= maxInUse; c++) {
      scanByte(reg);    // specify register
      scanByte(col);    //((data & 0x01) * 256) + data >> 1); // put data
  }
  P1OUT |= LOAD;
}

void screenUpdate()
{
   unsigned int row;
   for (row = 0; row < 8; row++)
   {
      sendCoord(row+1, buffer[row]);
   }
}

void clearDisplay()
{
    unsigned int x;
    for (x = 0; x < 8; x++)
    {
       buffer[x] = 0b00000000;
    }
    screenUpdate();
}
void changeMap()
{
int i = 0;

    for(i = 0; i < 8; i++){
        if(i == portalIndex) // Se a coluna mover-se
        {
            if((portalIndex > 0) && (portalIndex < 7))
                limits[portalIndex] = portalX;
            else
                //if((portalY != 128) && (portalY != 1))
                    limits[portalIndex] = ~portalY;
        }
        else
            limits[i] = limits0[i];
    }
}
//
int main()
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    BCSCTL1 = CALBC1_1MHZ;  // seleciona o Master clock de 1Mhz
    DCOCTL = CALDCO_1MHZ;   // seleciona o clock de 1Mhz

    P1OUT |= CLK + DIN + LOAD;   // Valor inicial da porta P1
    P1DIR |= DIN + CLK + LOAD;  // Define os botões como entrada

    //P1REN = (BTN1 + BTN2 + BTN3 + BTN4);        // Habilita os resistores (Pull-up)
    //P1OUT |= (BTN1 + BTN2 + BTN3 + BTN4);       // Define os resistores dos botões como Pull-up

    //TA0CTL = MC_1 + ID_0 + TASSEL_2 + TAIE;     // Modo de contagem contínua / sem divisão de clock / Master Clock(1Mhz)
    //TA0CCR0 = 9999; // A cada 10 ms o mapa é mudado com a interrupção do timer0
    //__bis_SR_register(GIE);

    P1SEL |= BIT6 + BIT7;                   // Assign I2C pins to USCI_B0
    P1SEL2|= BIT6 + BIT7;                   // Assign I2C pins to USCI_B0
    i2cInit();
    slaveAddress = 0x68;                    // MPU-6050 address
    TX_Data[1] = 0x6B;                      // address of PWR_MGMT_1 register
    TX_Data[0] = 0x00;                      // set register to zero (wakes up the MPU-6050)
    TX_ByteCtr = 2;
    i2cWrite(slaveAddress);

    setReg(max7219_reg_scanLimit, 0x07);        // Define o valor máximo de colunas a ser escaneada
    setReg(max7219_reg_decodeMode, 0x00);       // Cada segmento receberá o valor enviado ao registrador sem decodificação
    setReg(max7219_reg_shutdown, 0x01);         // "Liga" o MAX7219 (Normal operation)
    setReg(max7219_reg_displayTest, 0x00);      // Desliga o modo teste.
    setReg(max7219_reg_intensity, 0x0F & 0x0F); // Define a intensidade de brilho dos leds(0x00-0x0f)


    unsigned int hold = 1; // Variável para travar a condição quando o botão for pressionado
    unsigned int i = 0; // index
    clearDisplay(); // Limpa o display
    linha = x0; // define as coordenadas iniciais dos leds (0b00010000)
    coluna = y0; // define o índice inicial do buffer[i] (4)
    x1 = x0;

    limits0[0] = 0b11111111;
    limits0[1] = 0b10000001;
    limits0[2] = 0b10000001;
    limits0[3] = 0b10000001;
    limits0[4] = 0b10000001;
    limits0[5] = 0b10000001;
    limits0[6] = 0b10000001;
    limits0[7] = 0b11111111;

    for(;;){

        // Point to the ACCEL_ZOUT_H register in the MPU-6050
        slaveAddress = 0x68;                    // MPU-6050 address
        TX_Data[0] = 0x3B;                  // register address
        TX_ByteCtr = 1;
        i2cWrite(slaveAddress);

        // Read the two bytes of data and store them in zAccel
        slaveAddress = 0x68;                    // MPU-6050 address
        RX_ByteCtr = 6;
        i2cRead(slaveAddress);

        xAccel  = RX_Data[5] << 8;              // MSB
        xAccel |= RX_Data[4];                   // LSB
        yAccel  = RX_Data[3] << 8;              // MSB
        yAccel |= RX_Data[2];                   // LSB

        //zAccel  = RX_Data[1] << 8;              // MSB
        //zAccel |= RX_Data[0];                   // LSB

        left = map(xAccel,0,20000,limite_medio,limite_superior);
        right = map(xAccel,0,-20000,limite_medio,limite_superior);
        up = map(yAccel,0,20000,limite_medio,limite_superior);
        down = map(yAccel,0,-20000,limite_medio,limite_superior);

        // Laço responsável por enviar as coordenadas certas ao LED
        // Ele atualiza o valor enviado para o buffer[8] com as coordenadas atuais
        for(i = 0; i < 8; i++){
            if(i == coluna) // Se a coluna mover-se
                buffer[coluna] = limits[i] | linha; // Analise feita a partir da coluna, essa posição que receberá a nova posição
            else                                    // da linha ... isto é, será enviado o par (linha,coluna) para a matriz.
                buffer[i] = limits[i];  // Nas demais colunas, a matriz será preenchida com as bordas do jogo.
        }

        screenUpdate();

        /*up    = Debounce_button(BTN1);   // Analisa o estado dos botões (esse processo pode levar 5 ms)
        down  = Debounce_button(BTN2);   // Para mudar essa lógica, basta colocar a variável do acelerômetro no lugar do "Debouce ..."
        left  = Debounce_button(BTN3);
        right = Debounce_button(BTN4);
         */
        if((up > limite_analise) && (down < limite_analise) && (left < limite_analise) && (right < limite_analise))  // se pressionarmos o botão 1, o led deve andar uma posição para cima, logo, ele deve saltar de linha.
        {
            x1 = (x1 >> 1); // desloca o bit referente ao LED, cuja coordenada inicial é "00010000"

            if((x1 & 0xFF) != 1)    // Condição para colisão com a borda
                linha = x1;
            else
            {
                if(limits[coluna] == 0x80)
                {
                    changeMap();
                    x1 = x0;
                    linha = x0;
                    coluna = y0;
                }else{
                    x1 = 0x02;
                    linha = 0x02;
                }
                    //coluna = y0;
            }
            delay(500 - up);
            // segurando o hold, necessario pois o ciclo de clock incrementaria mais de uma linha.
        }
        else if((down > limite_analise) && (left < limite_analise) && (right < limite_analise) && (up < limite_analise))  // processo se repete para os outros comandos
        {
            x1 = (x1 << 1);
            if((x1 & 0xFF) != 128)  // Condição para colisão com a borda
                linha = x1;
            else
            {
                if(limits[coluna] == 0x01)
                {
                    changeMap();
                    x1 = x0;
                    linha = x0;
                    coluna = y0;
                }else{
                    x1 = 0x40;
                    linha = 0x40;
                }
                    //coluna = y0;
            }

                //coluna = y0;
            delay(500 - down);
         //   hold = 0;
        }
        else if((left > limite_analise) && (right < limite_analise) && (down < limite_analise) && (up < limite_analise))
        {
            coluna ++;  // Para a coluna, basta incrementar ou decrementar o índice no qual o led irá acender (buffer[i])
            if(coluna == 7) // Condição para colisão com a borda
            {
                if((limits[coluna] &  (~linha)) == limits[coluna])
                {
                    changeMap();
                    x1 = x0;
                    linha = x0;
                    coluna = y0;
                }else{
                    coluna = 6;//y0;
                }
                //linha = x0;
            }
            delay(500 - left);

          //  hold = 0;
        }
        else if ((right > limite_analise) && (left < limite_analise) && (down < limite_analise) && (up < limite_analise))
        {
            coluna --;
            if(coluna == 0) // Condição para colisão com a borda
            {
                if((limits[coluna] & (~linha)) == limits[coluna])
                {
                    changeMap();
                    x1 = x0;
                    linha = x0;
                    coluna = y0;
                }else{
                    coluna = 1;// y0;//1;
                }
                    //linha = x0;
            }
            delay(500 - right);
         //   hold = 0;
        }
        //if((up + down + left + right) == 0) // Destrava o hold
           // hold = 1;

    }
    return 0;
}

// Interrupção do Timer 0 para trocar de mapa "aleatoriamente".
/*#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer_A0(void){

    portalIndex ++;
    if(portalIndex == 8)
    {
        portalIndex = 0;
    }

    portalY = (portalY << 1);
    if(portalY == 128)
        portalY = 0x02;
    portalX ^= 0b10000001;
    TA0CTL &= ~TAIFG;
}
*/
// USCIAB0TX_ISR
#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void)
{

    if(UCB0CTL1 & UCTR)                 // TX mode (UCTR == 1)
    {
        if (TX_ByteCtr)                     // TRUE if more bytes remain
        {
            TX_ByteCtr--;               // Decrement TX byte counter
            UCB0TXBUF = TX_Data[TX_ByteCtr];    // Load TX buffer
        }
        else                        // no more bytes to send
        {
            UCB0CTL1 |= UCTXSTP;            // I2C stop condition
            IFG2 &= ~UCB0TXIFG;         // Clear USCI_B0 TX int flag

            __bic_SR_register_on_exit(CPUOFF);  // Exit LPM0

            //-------------change_map-------------//
            portalIndex ++;
            if(portalIndex == 8)
            {
                portalIndex = 0;
            }
            portalY = (portalY << 1);
            if(portalY == 128)
                portalY = 0x02;
            portalX ^= 0b10000001;
            //------------------------------------//
        }
    }
    else // (UCTR == 0)                 // RX mode
    {
        RX_ByteCtr--;                       // Decrement RX byte counter
        if (RX_ByteCtr)                     // RxByteCtr != 0
        {
            RX_Data[RX_ByteCtr] = UCB0RXBUF;    // Get received byte
            if (RX_ByteCtr == 1)            // Only one byte left?
            UCB0CTL1 |= UCTXSTP;            // Generate I2C stop condition
        }
        else                        // RxByteCtr == 0
        {
            RX_Data[RX_ByteCtr] = UCB0RXBUF;    // Get final received byte
            __bic_SR_register_on_exit(CPUOFF);  // Exit LPM0
        }
    }
}

