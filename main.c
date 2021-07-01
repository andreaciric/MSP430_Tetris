#include <msp430.h>
#include <stdint.h>
#include <stdlib.h>

void spi_send_16(uint8_t address, uint8_t data);
void spi_send_8(uint8_t data);
void init_8x8B_click(void);
void init(void);
void thumbstick(void);

// MAX7219 Register addresses
#define NOOP        0x00
#define DIGIT0      0x01
#define DIGIT1      0x02
#define DIGIT2      0x03
#define DIGIT3      0x04
#define DIGIT4      0x05
#define DIGIT5      0x06
#define DIGIT6      0x07
#define DIGIT7      0x08
#define DECODEMODE  0x09
#define INTENSITY   0x0A
#define SCANLIMIT   0x0B
#define SHUTDOWN    0x0C
#define DISPLAYTEST 0x0F

//MAX7219 Data addresses
#define SEG0        0x01
#define SEG1        0x02
#define SEG2        0x04
#define SEG3        0x08
#define SEG4        0x10
#define SEG5        0x20
#define SEG6        0x40
#define SEG7        0x80

// variables
uint8_t i = 0, j = 0, k = 0;
const uint8_t digit[] = {DIGIT0, DIGIT1, DIGIT2, DIGIT3, DIGIT4, DIGIT5, DIGIT6, DIGIT7};
const uint8_t segments[] = {SEG0, SEG1, SEG2, SEG3, SEG4, SEG5, SEG6, SEG7};
uint8_t dont_allow = 0;
uint8_t dont_allow_next = 0;
uint8_t bottom = 0;
uint8_t end = 0;
uint8_t clear_row = 1;
uint8_t clear_bit = 0;
uint8_t start = 4;
uint8_t istart = 4;
uint8_t object_type = 0;
uint8_t rotation = 0;

uint8_t side = 1; //left = 0, static = 1, right = 2

//thumbstick
const uint8_t DinFirstByte = 0b00000110;      // Sets default Primary ADC Address register B00000110, This is a default address setting, the third LSB is set to one to start the ADC, the second LSB is to set the ADC to single ended mode, the LSB is for D2 address bit, for this ADC its a "Don't Care" bit.
const uint8_t DinSecondByte = 0b01000000;
const uint16_t DoutFirstByteMask = 0b0000000000001111;      // b00001111 isolates the 4 LSB for the value returned.
const uint16_t DoutSecondByteMask = 0b0000000011111111;      // b00001111 isolates the 4 LSB for the value returned.
uint16_t DoutFirstByte = 0;
uint16_t DoutSecondByte = 0;
uint16_t digitalValue = 0;


/* init tetris shapes */
uint8_t screen[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t endscreen[] = {0x00, SEG1+SEG4+SEG6, SEG1+SEG4+SEG5, SEG1+SEG2+SEG3+SEG4, 0x00, SEG1+SEG2+SEG3+SEG4, SEG2+SEG4, SEG1+SEG2+SEG3+SEG4};

uint8_t temp[4][16] = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
                      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
                      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
                      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
 };

uint8_t next[4][16] = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
                      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
                      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
                      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
 };

uint8_t full_row[8][8] = {{SEG0, SEG0, SEG0, SEG0, SEG0, SEG0, SEG0, SEG0},
                      {SEG1, SEG1, SEG1, SEG1, SEG1, SEG1, SEG1, SEG1},
                      {SEG2, SEG2, SEG2, SEG2, SEG2, SEG2, SEG2, SEG2},
                      {SEG3, SEG3, SEG3, SEG3, SEG3, SEG3, SEG3, SEG3},
                      {SEG4, SEG4, SEG4, SEG4, SEG4, SEG4, SEG4, SEG4},
                      {SEG5, SEG5, SEG5, SEG5, SEG5, SEG5, SEG5, SEG5},
                      {SEG6, SEG6, SEG6, SEG6, SEG6, SEG6, SEG6, SEG6},
                      {SEG7, SEG7, SEG7, SEG7, SEG7, SEG7, SEG7, SEG7},
 };

uint8_t all_shapes[7][4][16] = {{{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, SEG6+SEG7, SEG6+SEG7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},         // O 1
                               {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, SEG6+SEG7, SEG6+SEG7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},          // O 2
                               {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, SEG6+SEG7, SEG6+SEG7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},          // O 3
                               {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, SEG6+SEG7, SEG6+SEG7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},},        // O 4

                               {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, SEG7, SEG7, SEG7, SEG7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},                   // I 1
                               {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, SEG7+SEG6+SEG5+SEG4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},     // I 2
                               {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, SEG7, SEG7, SEG7, SEG7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},                    // I 3
                               {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, SEG7+SEG6+SEG5+SEG4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},},   // I 4

                               {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, SEG7, SEG7, SEG7+SEG6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},              // J 1
                               {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, SEG5, SEG7+SEG6+SEG5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},          // J 2
                               {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, SEG7+SEG6, SEG6, SEG6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},               // J 3
                               {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, SEG7+SEG6+SEG5, SEG7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},},        // J 4

                               {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, SEG7+SEG6, SEG7, SEG7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},              // L 1
                               {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, SEG7, SEG7+SEG6+SEG5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},          // L 2
                               {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, SEG6, SEG6, SEG7+SEG6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},               // L 3
                               {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, SEG7+SEG6+SEG5, SEG5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},},        // L 4

                               {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, SEG6, SEG7+SEG6, SEG7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},              // S 1
                               {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, SEG7+SEG6, SEG6+SEG5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},          // S 2
                               {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, SEG6, SEG7+SEG6, SEG7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},               // S 3
                               {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, SEG7+SEG6, SEG6+SEG5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},},        // S 4

                               {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, SEG7, SEG7+SEG6, SEG7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},              // T 1
                               {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, SEG6, SEG7+SEG6+SEG5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},          // T 2
                               {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, SEG6, SEG7+SEG6, SEG6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},               // T 3
                               {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, SEG7+SEG6+SEG5, SEG6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},},        // T 4

                               {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, SEG7, SEG7+SEG6, SEG6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},              // Z 1
                               {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, SEG6+SEG5, SEG7+SEG6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},          // Z 2
                               {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, SEG7, SEG7+SEG6, SEG6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},               // Z 1
                               {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, SEG6+SEG5, SEG7+SEG6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},},        // Z 2
};

int main(void)
{
  init();
  init_8x8B_click();

//  while(1){
//      thumbstick();
//  }

  while (!end){


      object_type = (rand() % 8) - 1;
      bottom = 0;
      dont_allow = 0;
      dont_allow_next = 0;
      start = 4;

      /* GET OBJECT */
      for (i = 0; i < 4; i++){
            for (j = 0; j < 16; j++){
                temp[i][j] = all_shapes[object_type][i][j];
            }
        }

      while (!bottom){

          thumbstick();

          _delay_cycles(500000);

          /* CHECKS FOR ROTATION */
          if (!(P1IN & BIT4))
              rotation = (rotation + 1) % 4;

          /* CHECKS FOR HORIZONTAL MOVEMENT */
          if (digitalValue > 1500)
              side = 2;
          else if (digitalValue < 500)
              side = 0;
          else
              side = 1;

          if (side == 0){
              if (start > 0)
                  start -= 1;
          } else if (side == 2){
              if (start < 8)
                  start += 1;
          }
          istart = i + start;

          /* PRINT SCREEN */
          for (i = 0; i < 8; i++){
                spi_send_16(digit[i], temp[rotation][i+start] | screen[i] );
          }

          /* SHIFT DOWN */
          for (i = 0; i < 4; i++){
              for (j = 0; j < 16; j++){
                  next[i][j] = ( temp[i][j] >> 1 );
              }
          }

          /* CHECK IF NEXT MOVE CAN BE REALISED */
          for (i = 0; i < 8; i++) {
              if ((temp[rotation][i+start] & SEG0) && !dont_allow )
                  dont_allow = 1;
              else if (((screen[i] & next[rotation][i+start]) != 0) && !dont_allow) {
                  dont_allow = 1;
              }
          }

          /* STACK OBJECT OR GO ON */
          if (dont_allow){
              for (i = 0; i < 8; i++){                  //stack object
                  screen[i] |=  temp[rotation][i+start];
                  bottom = 1;
              }
          } else{
              for (i = 0; i < 4; i++){                  //move down
                  for (j = 0; j < 16; j++){
                      temp[i][j] = next[i][j];
                  }
              }
          }
      }

      /* DELETE FULL ROWS */
      for (i = 0; i < 8; i++){
        for (j = 0; j < 8; j++){
            i = i;
            j = j;
            if ((full_row[i][j] & screen[j]) != 1)      //checks if row is (not) full
                clear_row = 0;
        }
        if (clear_row){
            clear_bit = 1 << i;
            for (k = 0; k < 8; k++){                    //clears full row
                screen[k] ^= clear_bit;
            }
            for (j = i; j < 8; j++){                    //moves down upper fields
                screen[j] = ( screen[j] >> 1 );
                spi_send_16(digit[j],  screen[j] );
            }
        }
      }

      /* CHECK IF GAME ENDS */
      for (i = 0; i < 8; i++){
          if (!end && ((screen[i] & SEG7) != 0)){
              end = 1;
          }
      }
  }
  /* PRINT SCREEN */
  for (i = 0; i < 8; i++){
      spi_send_16(digit[i], endscreen[i] );
  }

}

void init(void)
{
    WDTCTL = WDTPW+WDTHOLD;                   // Stop watchdog timer

    /* pins init */
    P2OUT |= BIT7;                            // Set P2.7 for CS (spi 8x8 B click)
    P2DIR |= BIT7;                            // Set P2.7 to output direction
    P2OUT |= BIT6;                            // Set P2.6 for CS (spi thumbstick click)
    P2DIR |= BIT6;                            // Set P2.6 to output direction
    P1OUT |= BIT4;                            // Set P1.4 for thumbstick INT
    P1DIR |= BIT4;                            // Set P1.4 to output direction

    P3SEL |= BIT0+BIT1+BIT2;                  // P3.0,2 option select
    P4SEL |= BIT1+BIT2+BIT3;                  // P3.0,2 option select

    P1IE |= BIT4;



    /* SPI init */
    UCB0CTL1 |= UCSWRST;                      // **Put state machine in reset**
    UCB0CTL0 |= UCMST+UCCKPH+UCMSB+UCSYNC;    // 3-pin, 8-bit SPI master
                                                                                                       // MSB

    UCB0CTL1 |= UCSSEL_2;                     // SMCLK
    UCB0BR0 = 0x02;                           // /2
    UCB0BR1 = 0;                              //
    UCB0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**

    /* SPI init */
    UCB1CTL1 |= UCSWRST;                      // **Put state machine in reset**
    UCB1CTL0 |= UCMST+UCCKPH+UCMSB+UCSYNC;    // 3-pin, 8-bit SPI master
                                                                                                       // MSB

    UCB1CTL1 |= UCSSEL_2;                     // SMCLK
    UCB1BR0 = 0x02;                           // /2
    UCB1BR1 = 0;                              //
    UCB1CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**

    __enable_interrupt();           // GIE
}

// Send 16 bits as: xxxxaaaadddddddd (ignore, address, data)
// and use active low Chip Select
void spi_send_16(uint8_t address, uint8_t data)
{
    P2OUT &= ~BIT7;                 // CS low
    _delay_cycles(5000);
    while (!(UCB0IFG&UCTXIFG));     // Wait until done
    UCB0TXBUF = (address & 0x0F);
    while (!(UCB0IFG&UCTXIFG));     // Wait until done
    UCB0TXBUF = (data);
    while (!(UCB0IFG&UCTXIFG));     // Wait until done
    P2OUT |= BIT7;                  // CS high
    _delay_cycles(5000);
}

void spi_send_8(uint8_t data)
{
    //P2OUT &= ~BIT6;                 // CS low
    //_delay_cycles(5000);
    //while (!(UCB0IFG&UCTXIFG));     // Wait until done
    UCB1TXBUF = (data);
    while (!(UCB1IFG&UCTXIFG));     // Wait until done
    //P2OUT |= BIT6;                  // CS high
    //_delay_cycles(5000);
}

void init_8x8B_click(void)
{
    // Initialise MAX7219 with 8x8 led matrix
    spi_send_16(DISPLAYTEST, 0x00);     // NO OP (seems needed after power on)
    spi_send_16(SCANLIMIT, 0x07);       // Enable all digits (always needed for current/8 per row)
    spi_send_16(DECODEMODE, 0x00);      // Display intensity (0x00 to 0x0F)

    // Clear all rows/digits
    spi_send_16(DIGIT0, 0);
    spi_send_16(DIGIT1, 0);
    spi_send_16(DIGIT2, 0);
    spi_send_16(DIGIT3, 0);
    spi_send_16(DIGIT4, 0);
    spi_send_16(DIGIT5, 0);
    spi_send_16(DIGIT6, 0);
    spi_send_16(DIGIT7, 0);
    spi_send_16(SHUTDOWN, 0);           // Wake oscillators/display up

    __delay_cycles(100000);
    spi_send_16(SHUTDOWN, 1);
    spi_send_16(INTENSITY, 0x3);

 }

void thumbstick(void)
{
    P2OUT &= ~BIT6;
    spi_send_8(DinFirstByte);
    spi_send_8(DinSecondByte); // read the primary byte, also sending in the secondary address byte.
    DoutFirstByte = UCB1RXBUF & DoutFirstByteMask;
    spi_send_8(0x00); // read the secondary byte, also sending 0 as this doesn't matter.
    DoutSecondByte = UCB1RXBUF & DoutSecondByteMask;
    P2OUT |= BIT6;

    digitalValue = DoutFirstByte;
    digitalValue = digitalValue << 7;
    digitalValue |= DoutSecondByte;
    //value = ((float)(digitalValue) * 2.048) / 4096.000;
}

void __attribute__ ((interrupt(PORT1_VECTOR))) P1ISR (void)
{
    P1IFG = 0;
}
