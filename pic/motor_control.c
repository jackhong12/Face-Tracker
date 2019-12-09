/*
 * INT1: RD2
 * INT2: RD3
 * ENA: RC2 / P1A
 * 
 * variable resistance: RA0 / AN0
 */

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

int value;
int power;
void __interrupt(high_priority) high_isr () {
    if (PIE1bits.ADIE && PIR1bits.ADIF) {
        value = ADRESL;
        value += ADRESH << 8;
        
        if (value > 1023) value = 1023;
        if (value < 0) value = 0;
        
        power = value - 511;
        if (power == 0) {
            LATDbits.LD3 = 0;
            LATDbits.LD2 = 0;
        }
        else if (power < 0) {
            LATDbits.LD3 = 0;
            LATDbits.LD2 = 1;
            power = -power;
            
            CCPR1L = power / 2;
        }
        else {
            LATDbits.LD3 = 1;
            LATDbits.LD2 = 0;
            CCPR1L = power / 2;
        }
        
        PIR1bits.ADIF = 0;
        ADCON0bits.GO = 1;  // start to read A/D converter pin
    }
}

void __interrupt(low_priority) low_isr () {
}

void main(void) {
    /* enable interrupt */
    RCONbits.IPEN = 1;
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;
    
    /* set Fosc = 4MHz */
    OSCCONbits.IRCF = 0b110;
    
    /* timer2 prescaler 1:16 */
    T2CON = 0b00000111;
    TMR2 = 0;
    
    /*
     * single output P1A
     * DC1B : 0 0
     * PWM mode P1A, P1C active-high
     */
    CCP1CON = 0b00001100;
    
    PR2 = 255;
    
    TRISD = 0;
    LATDbits.LD3 = 0;
    LATDbits.LD2 = 1;
    CCPR1L = 40;
    
    /* P1A: RC2 output */
    TRISCbits.RC2 = 0;
    
    
    ADCON1bits.VCFG = 0;    // Vss and Vdd as voltage reference
    ADCON1bits.PCFG = 0b1110;   // AN0 as A/D ports
    ADCON0bits.CHS = 0; // select AN0 pin
    TRISAbits.RA0 = 1;  // RA0 as input
    TRISC = 0;
    TRISD = 0;
    
    
    
    /*
     * TAD = 64 / 4 MHz = 16 mu second
     * TACQ = 2.4 mu second
     * TAD / TACQ = 6.667
     */
    ADCON2bits.ADFM = 1; // right justified
    ADCON2bits.ADCS = 0b110;    // Fosc/64
    ADCON2bits.ACQT = 0b101;  // 8 Tad
    
    /* A/D interrupt */
    PIR1bits.ADIF = 0;
    PIE1bits.ADIE = 1;
    IPR1bits.ADIP = 1;
    
    ADCON0bits.ADON = 1;    // enable A/D Converter module
    ADCON0bits.GO = 1;  // start to read A/D converter pin
    while(1);
    return;
}
