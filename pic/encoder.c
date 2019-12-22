#include <xc.h>
#include <pic18f4520.h>

#pragma config OSC = INTIO67    // Oscillator Selection bits (Internal oscillator block, port function on RA6 and RA7)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bits (Brown-out Reset enabled and controlled by software (SBOREN is enabled))
//#pragma config BOREN = SBORDIS       // Brown-out Reset enabled in hardware only (SBOREN is disabled)
#pragma config BORV = 3         // Brown Out Reset Voltage bits (Minimum setting)

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC   // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
//#pragma config PBADEN = ON      // PORTB A/D Enable bit (PORTB<4:0> pins are configured as analog input channels on Reset)
#pragma config PBADEN = OFF      // PORTB<4:0> pins are configured as digital I/O on Reset
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-001FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (002000-003FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (004000-005FFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (006000-007FFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-001FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (002000-003FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (004000-005FFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (006000-007FFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-001FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (002000-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (004000-005FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (006000-007FFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot block (000000-0007FFh) not protected from table reads executed in other blocks)

void initial_counter () {
    T0CONbits.TMR0ON = 1;
    T0CONbits.T08BIT = 0;   /* 16 bits */
    T0CONbits.T0CS = 1; /* T0CKI pin */
    T0CONbits.T0SE = 1; /* high-to-low */
    T0CONbits.PSA = 0b000;
  
    T1CONbits.RD16 = 1; /* 16 bits */
    T1CONbits.T1RD16 = 1;
    T1CONbits.T1CKPS = 0b00;    /* 1:1 prescale value */
    T1CONbits.T1OSCEN = 1;
    T1CONbits.T1SYNC = 0;   /* synchronize external clock rate */
    T1CONbits.TMR1CS = 1;   /* external clock from pin RC0/T1OSO/T13CKI (on the rising edge */
    T1CONbits.TMR1ON = 1;
    
    TMR0 = 0;
    TMR1 = 0;
}
#define ENCODER_BUFFER_SIZE 5
unsigned char _encoder_ptr = 0;
signed int _encoder_degree[ENCODER_BUFFER_SIZE];

void set_encoder_degree () {
    ++_encoder_ptr;
    if (_encoder_ptr >= ENCODER_BUFFER_SIZE)
        _encoder_ptr = 0;
    _encoder_degree[_encoder_ptr] = TMR1 - TMR0;
}

unsigned int get_encoder_degree () {
    return _encoder_degree[_encoder_ptr];
}

unsigned int get_encoder_velocity () {
    unsigned char pre_encoder_ptr = _encoder_ptr + 1;
    if (pre_encoder_ptr >= ENCODER_BUFFER_SIZE)
        pre_encoder_ptr = 0;
    return _encoder_degree[_encoder_ptr] - _encoder_degree[pre_encoder_ptr];
}

volatile signed long deg, gap, temp;


void main(void) {
//    T0CON = 0b10111000;
//    T1CON = 0b11001011;
    initial_counter();
    
    
    while(1) {
        set_encoder_degree();
        gap = TMR1;
        gap -= TMR0;
        deg = gap * 360;
        temp = deg / 616;
    }
    return;
}
