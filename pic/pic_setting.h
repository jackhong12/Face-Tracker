#ifndef PIC_SETTING_H
#define PIC_SETTING_H

#include <xc.h>
#include <pic18f4520.h>

/* 
 * Counter 
 *     Use timer0 and timer1 as encoder counter.
 *     - timer0 pin: T0CKI
 *     - timer1 pin: RC0/T1OSO/T13CKI
 *
 * ENCODER_BUFFER_SIZE
 *     For calculating velocity.
 *     v = (position[t] - position[t - size]) / (size * timer_period)
 */
#define ENCODER_BUFFER_SIZE 5
void initial_counter(void);
void set_encoder_degree(void);
unsigned int get_encoder_degree(void);
unsigned int get_encoder_velocity(void);

/* 
 * Timer 
 * 
 * Fosc = 4 MHz
 * 0.001(s) =  1 KHz / 1MHz
 * timer3_period = 2 ^ 16 - 1000 = 64536 
 */
#define TIMER3_PERIOD 64536
void initial_timer3(void);

/*
 * PWM
 *     - timer2
 *     - LD3
 *     - LD2
 *     - RC2/P1A
 */
void initial_pwm(void);
void set_dc_motor(int pwm); /* -1023 <= pwm <= 1023 */

#endif
