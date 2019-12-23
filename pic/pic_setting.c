#include "pic_setting.h"

/*============================================================================
 * Timer3 
 *============================================================================*/
void initial_timer3 () {
    T3CONbits.RD16 = 1; /* 16 bits */
    /* cpature/compare clock source for CCP2 */
    T3CONbits.T3CCP2 = 0; 
    T3CONbits.T3CCP1 = 1;
    T3CONbits.T3CKPS = 0b00;    /* 1:1 prescale value */
    T3CONbits.TMR3CS = 0;   /* Internal clock (Fosc/4) */
    T3CONbits.TMR3ON = 1;   /* Timer 3 ON bit */
    
    TMR3 = TIMER3_PERIOD;
    
    PIR2bits.TMR3IF = 0;
    IPR2bits.TMR3IP = 1;    /* high priority */
    PIE2bits.TMR3IE = 1;    /* enable timer3 interrupt */
}

/*============================================================================
 * Counter 
 *============================================================================*/
unsigned char _encoder_ptr = 0;
signed int _encoder_degree[ENCODER_BUFFER_SIZE];

void initial_counter () {
    T0CONbits.TMR0ON = 1;
    T0CONbits.T08BIT = 0;   /* 16 bits */
    T0CONbits.T0CS = 1; /* T0CKI pin */
    T0CONbits.T0SE = 1; /* high-to-low */
    T0CONbits.PSA = 1;  /* don't use prescaler */
    T0CONbits.T0PS = 0b000;
  
    T1CONbits.RD16 = 1; /* 16 bits */
    T1CONbits.T1RD16 = 1;
    T1CONbits.T1CKPS = 0b00;    /* 1:1 prescale value */
    T1CONbits.T1OSCEN = 1;
    T1CONbits.T1SYNC = 0;   /* synchronize external clock rate */
    /* external clock from pin RC0/T1OSO/T13CKI (on the rising edge) */
    T1CONbits.TMR1CS = 1;   
    T1CONbits.TMR1ON = 1;
    
    TMR0 = 0;
    TMR1 = 0;
    for (int i = 0; i < ENCODER_BUFFER_SIZE; i++)
        _encoder_degree[i] = 0;
}

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
    unsigned char pre_encoder_ptr = _encoder_ptr - ENCODER_CONTORL_SIZE;
    if (pre_encoder_ptr < 0)
        pre_encoder_ptr += ENCODER_BUFFER_SIZE;
    return _encoder_degree[_encoder_ptr] - _encoder_degree[pre_encoder_ptr];
}

unsigned int get_encoder_avg_velocity () {
    unsigned char pre_encoder_ptr = _encoder_ptr + 1;
    if (pre_encoder_ptr >= ENCODER_BUFFER_SIZE)
        pre_encoder_ptr = 0;
    return _encoder_degree[_encoder_ptr] - _encoder_degree[pre_encoder_ptr];
}

/*============================================================================
 * PWM (motor control)
 *
 *============================================================================*/
void initial_pwm () {
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
    
    /*
     * INT1 INT2 
     */
    TRISD = 0;
    TRISC = 0;
    LATDbits.LD3 = 0;
    LATDbits.LD2 = 0;
    CCPR1L = 0;
}

/* 
 * - (2^10 - 1) = -1023 <= pwm <= 1023 = 2^10 -1
 * LD3
 * LD2
 * 
 */
void set_dc_motor (int pwm) {
    /* pwm == 0 */
    if (pwm == 0) {
        LATDbits.LD3 = 0;
        LATDbits.LD2 = 0;
        CCPR1L = 0;
        CCP1CONbits.DC1B = 0;
        return;
    }
    /* pwm > 0 */
    else if (pwm > 0) {
        LATDbits.LD3 = 1;
        LATDbits.LD2 = 0;
    }
    /* pwm < 0 */
    else {
        pwm = -pwm;
        LATDbits.LD3 = 0;
        LATDbits.LD2 = 1;    
    }
    if (pwm > 1023)
        pwm = 1023;
    
    CCPR1L = pwm / 4;
    CCP1CONbits.DC1B = pwm % 4;
}

/*============================================================================
 * 
 *============================================================================*/
void initial_buad () {
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
}

void send_char (char one_char) {
    if (!TXSTAbits.TXEN)
        return;
    while (!PIR1bits.TXIF);
    TXREG = one_char;
}
