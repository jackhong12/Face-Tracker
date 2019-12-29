#include <xc.h>
#include <pic18f4520.h>
#include "pic_setting.h"

signed int position, target_position = 10, velocity, target_velocity = 10;
#define P_GAIN 128
#define I_GAIN 2
#define D_GAIN 0
signed long integrator = 0;
signed int pre_vel_error = 0;

void pid () {
    signed int value, vel_error;
    
    /* velocity gap */
    if (target_velocity == 0) {
        set_dc_motor(0);
        return;
    }
    else if (target_velocity > 20)
        target_velocity = 20;
    else if (target_velocity < -20)
        target_velocity = -20;
    
    /* velocity control */
    velocity = get_encoder_velocity();
    vel_error = target_velocity - velocity;
    integrator += vel_error;
    value = P_GAIN * vel_error + I_GAIN * integrator + D_GAIN * (vel_error - pre_vel_error);
    
    /* friction compensation */
    if (value > 0)
        value += 200;
    else
        value -= 200;
    
    set_dc_motor(value);
    pre_vel_error = vel_error;
}

void __interrupt(high_priority) high_isr () { 
    if (RCSTAbits.CREN && PIR1bits.RCIF)
        receive_b7_int();

}

void __interrupt(low_priority) low_isr () { 
    if (PIE2bits.TMR3IE && PIR2bits.TMR3IF) {
        TMR3 = TIMER3_PERIOD;
        TMR3IF = 0;
        set_encoder_degree();
        
        /* new desired input */
        if (isNewInput()) {
            target_velocity = get_receive_value(); 
        }
        
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
    while(1) {
        signed int v = get_encoder_velocity();
        send_b7_int(v);
    }
    return;
}