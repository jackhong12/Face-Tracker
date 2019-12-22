/*
 * INT1: RD2
 * INT2: RD3
 * ENA: RC2 / P1A
 * 
 * variable resistance: RA0 / AN0
 */

#include <xc.h>
#include <pic18f4520.h>
#include "./include/pic_setting.h"

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


/*
 * bluetooth variable
 */
#define BUFF_SIZE 20
#define BUFF_NEW_WORD 0
#define BUFF_CON_WORD 1
#define BUFF_OVERFLOW 2
#define BUFF_ERROR 3
char buff[BUFF_SIZE];
int char_pos;
int state;
int rec_value;

void receive_from_bluetooth () {
    while(!PIR1bits.RCIF);
    /* restart a word */
    if (state == BUFF_NEW_WORD || state == BUFF_OVERFLOW || state == BUFF_ERROR) {
        char_pos = 0;
        state = BUFF_CON_WORD;
    }
    
    /* get char from bluetooth */
    buff[char_pos] = RCREG;
    
    /* end character */
    if (buff[char_pos++] == '&') {
        state = BUFF_NEW_WORD;
        char isNegative = 0;
        char_pos = 0;
        if (buff[0] == '-') {
            isNegative = 1;
            char_pos++;
        }
        
        rec_value = 0;
        while (buff[char_pos] != 0 && buff[char_pos] != '&') {
            char temp = buff[char_pos] - '0';
            if (temp < 0 || temp > 9) {
                state = BUFF_ERROR;
                rec_value = 0;
                break;
            }
            rec_value = rec_value * 10 + temp;
            ++char_pos;
        }
        
        if (isNegative)
            rec_value = -rec_value;
        
        set_dc_motor(rec_value);
    }
    
    /* buffer overflow */
    if (char_pos >= BUFF_SIZE) {
        state = BUFF_OVERFLOW;
        char_pos = 0;
    }
    buff[char_pos] = 0; /* end of word */
}

void initial_bluetooth () {
    /* enable Tx and Rx pin */
    TRISCbits.RC7 = 1;  // RX
    TRISCbits.RC6 = 1;  // TX
    TXSTAbits.TXEN = 1;
    RCSTAbits.CREN = 1;
    
    /* serial port enable */
    RCSTAbits.SPEN = 1;
    
    /*
     * BAUD rate
     */
    TXSTAbits.SYNC = 0;
    TXSTAbits.BRGH = 1;
    BAUDCONbits.BRG16 = 0;
    SPBRGH = 0;
    SPBRG = 25;
    
    /* enable Tx and Rx interrupt */
    RCIE = 1;
    //TXIE = 1;
    
    /* initial variable */
    char_pos = 0;
    state = BUFF_NEW_WORD;
    rec_value = 0;
}

void __interrupt(high_priority) high_isr () {
    if (RCSTAbits.CREN && PIR1bits.RCIF)
        receive_from_bluetooth();
}

void __interrupt(low_priority) low_isr () {
}

signed int temp = 0;
void main(void) {
    /* enable interrupt */
    RCONbits.IPEN = 1;
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;
    
    /* set Fosc = 4MHz */
    OSCCONbits.IRCF = 0b110;
    
    initial_pwm();
    initial_bluetooth();
    initial_counter();
    
    while(1) {
        set_encoder_degree();
    }
    return;
}