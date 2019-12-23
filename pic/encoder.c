#include <xc.h>
#include <pic18f4520.h>
#include "pic_setting.h"

signed int position, target_position = 1;
#define P_GAIN 100
#define I_GAIN 0.1
#define D_GAIN 0
signed long integrator = 0;

void send_int (signed int value) {
    if (value < 0) {
        send_char('-');
        value = -value;
    }
    char temp = value / 100;
    send_char(temp % 10 + '0');
    
    temp = value / 10;
    send_char(temp % 10 + '0');
    
    send_char(value % 10 + '0');
    send_char('\n');
}

void pid () {
    position = get_encoder_avg_velocity();
    signed int value, error = target_position - position;
    integrator += error;
    value = P_GAIN * error + I_GAIN * integrator;
    if (value > 0)
        value += 200;
    else
        value -= 200;
    set_dc_motor(value);
}

char send_flag = 0;
void __interrupt(high_priority) high_isr () { 
    if (PIE2bits.TMR3IE && PIR2bits.TMR3IF) {
        TMR3 = TIMER3_PERIOD;
        TMR3IF = 0;
        set_encoder_degree();
        pid();
        
    }
}

void main(void) {
    /* enable interrupt */
    RCONbits.IPEN = 1;
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;
    
    /* set Fosc = 4MHz */
    OSCCONbits.IRCF = 0b110;
    
    initial_counter();
    initial_timer3();
    initial_pwm();
    initial_buad();
    set_encoder_degree();
    pid();
    signed int v = get_encoder_avg_velocity();
    send_int(v);
    while(1) {
        
    }
    return;
}
