#include <msp430.h>
#include <stdint.h>

void spi_max(uint8_t address, uint8_t data);
void init_8x8B_click(void);
void init(void);

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
uint8_t digit[] = {DIGIT0, DIGIT1, DIGIT2, DIGIT3, DIGIT4, DIGIT5, DIGIT6, DIGIT7};

/* init tetris shapes */
uint8_t kvadrat[] = {0x00, 0x00, 0x00, SEG6+SEG7, SEG6+SEG7, 0x00, 0x00, 0x00};

uint8_t shapes[7][8] = {{0x00, 0x00, 0x00, SEG6+SEG7, SEG6+SEG7, 0x00, 0x00, 0x00}, // O
                      {0x00, 0x00, SEG7, SEG7, SEG7, SEG7, 0x00, 0x00},             // I
                      {0x00, 0x00, 0x00, SEG7, SEG7, SEG7+SEG6, 0x00, 0x00},        // J
                      {0x00, 0x00, SEG7+SEG6, SEG7, SEG7, 0x00, 0x00, 0x00},        // L
                      {0x00, 0x00, 0x00, SEG6, SEG7+SEG6, SEG7, 0x00, 0x00},        // S
                      {0x00, 0x00, 0x00, SEG7, SEG7+SEG6, SEG7, 0x00, 0x00},        // T
                      {0x00, 0x00, SEG7, SEG7+SEG6, SEG6, 0x00, 0x00, 0x00},        // Z
};

uint8_t all_shapes[7][4][8] = {{{0x00, 0x00, 0x00, SEG6+SEG7, SEG6+SEG7, 0x00, 0x00, 0x00},         // O 1
                               {0x00, 0x00, 0x00, SEG6+SEG7, SEG6+SEG7, 0x00, 0x00, 0x00},          // O 2
                               {0x00, 0x00, 0x00, SEG6+SEG7, SEG6+SEG7, 0x00, 0x00, 0x00},          // O 3
                               {0x00, 0x00, 0x00, SEG6+SEG7, SEG6+SEG7, 0x00, 0x00, 0x00},},        // O 4

                               {{0x00, 0x00, SEG7, SEG7, SEG7, SEG7, 0x00, 0x00},                   // I 1
                               {0x00, 0x00, 0x00, SEG7+SEG6+SEG5+SEG4, 0x00, 0x00, 0x00, 0x00},     // I 2
                               {0x00, 0x00, SEG7, SEG7, SEG7, SEG7, 0x00, 0x00},                    // I 3
                               {0x00, 0x00, 0x00, 0x00, SEG7+SEG6+SEG5+SEG4, 0x00, 0x00, 0x00},},   // I 4

                               {{0x00, 0x00, 0x00, SEG7, SEG7, SEG7+SEG6, 0x00, 0x00},              // J 1
                               {0x00, 0x00, 0x00, SEG5, SEG7+SEG6+SEG5, 0x00, 0x00, 0x00},          // J 2
                               {0x00, 0x00, 0x00, SEG7+SEG6, SEG6, SEG6, 0x00, 0x00},               // J 3
                               {0x00, 0x00, 0x00, 0x00, SEG7+SEG6+SEG5, SEG7, 0x00, 0x00},},        // J 4

                               {{0x00, 0x00, SEG7+SEG6, SEG7, SEG7, 0x00, 0x00, 0x00},              // L 1
                               {0x00, 0x00, SEG7, SEG7+SEG6+SEG5, 0x00, 0x00, 0x00, 0x00},          // L 2
                               {0x00, 0x00, SEG6, SEG6, SEG7+SEG6, 0x00, 0x00, 0x00},               // L 3
                               {0x00, 0x00, 0x00, SEG7+SEG6+SEG5, SEG5, 0x00, 0x00, 0x00},},        // L 4

                               {{0x00, 0x00, 0x00, SEG6, SEG7+SEG6, SEG7, 0x00, 0x00},              // S 1
                               {0x00, 0x00, 0x00, 0x00, SEG7+SEG6, SEG6+SEG5, 0x00, 0x00},          // S 2
                               {0x00, 0x00, 0x00, SEG6, SEG7+SEG6, SEG7, 0x00, 0x00},               // S 3
                               {0x00, 0x00, 0x00, 0x00, SEG7+SEG6, SEG6+SEG5, 0x00, 0x00},},        // S 4

                               {{0x00, 0x00, 0x00, SEG7, SEG7+SEG6, SEG7, 0x00, 0x00},              // T 1
                               {0x00, 0x00, 0x00, SEG6, SEG7+SEG6+SEG5, 0x00, 0x00, 0x00},          // T 2
                               {0x00, 0x00, 0x00, SEG6, SEG7+SEG6, SEG6, 0x00, 0x00},               // T 3
                               {0x00, 0x00, 0x00, 0x00, SEG7+SEG6+SEG5, SEG6, 0x00, 0x00},},        // T 4

                               {{0x00, 0x00, SEG7, SEG7+SEG6, SEG6, 0x00, 0x00, 0x00},              // Z 1
                               {0x00, 0x00, SEG6+SEG5, SEG7+SEG6, 0x00, 0x00, 0x00, 0x00},          // Z 2
                               {0x00, 0x00, SEG7, SEG7+SEG6, SEG6, 0x00, 0x00, 0x00},               // Z 1
                               {0x00, 0x00, SEG6+SEG5, SEG7+SEG6, 0x00, 0x00, 0x00, 0x00},},        // Z 2

};


int main(void)
{

  init();
  init_8x8B_click();


  while (1){

      for (j = 0; j < 4; j++){                          // menja oblike
          for (k = 0; k < 8; k++){                      // pada do dna
              for (i = 0; i < 8; i++){                  // printa
                    spi_max(digit[i], all_shapes[j][0][i]);
              }

              _delay_cycles(500000);

              // shift down
              for (i = 0; i < 8; i++){                  // pomera
                  all_shapes[j][0][i] = ( all_shapes[j][0][i] >> 1 );
              }
          }
      }
      i = 0;
      j = 0;
      k = 0;
  }

}

void init(void)
{
    WDTCTL = WDTPW+WDTHOLD;                   // Stop watchdog timer

    /*  */
    P2OUT |= BIT7;                            // Set P6.4 for CS
    P2DIR |= BIT7;                            // Set P6.4 to output direction
    P3SEL |= BIT0+BIT2;                       // P3.0,2 option select


    /*  */
    UCB0CTL1 |= UCSWRST;                      // **Put state machine in reset**
    UCB0CTL0 |= UCMST+UCCKPH+UCMSB+UCSYNC;    // 3-pin, 8-bit SPI master
                                                                                                       // MSB

    UCB0CTL1 |= UCSSEL_2;                     // SMCLK
    UCB0BR0 = 0x02;                           // /2
    UCB0BR1 = 0;                              //
    UCB0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**

}

// Send 16 bits as: xxxxaaaadddddddd (ignore, address, data)
// and use active low Chip Select
void spi_max(uint8_t address, uint8_t data)
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

void init_8x8B_click(void)
{
    // Initialise MAX7219 with 8x8 led matrix
    spi_max(DISPLAYTEST, 0x00);    // NO OP (seems needed after power on)
    spi_max(SCANLIMIT, 0x07);   // Enable all digits (always needed for current/8 per row)
    spi_max(DECODEMODE, 0x00);   // Display intensity (0x00 to 0x0F)

    // Clear all rows/digits
    spi_max(DIGIT0, 0);
    spi_max(DIGIT1, 0);
    spi_max(DIGIT2, 0);
    spi_max(DIGIT3, 0);
    spi_max(DIGIT4, 0);
    spi_max(DIGIT5, 0);
    spi_max(DIGIT6, 0);
    spi_max(DIGIT7, 0);
    spi_max(SHUTDOWN, 0); // Wake oscillators/display up

    __delay_cycles(100000);
    spi_max(SHUTDOWN, 1);
    spi_max(INTENSITY, 0x3);




 }
