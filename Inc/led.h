#ifndef LED_H
#define LED_H

#define GREEN_OB_LED (1<<5)
#define RED_OB_LED 	(1<<29)

void initInLED(void);
void initBoardLED(int led_mask);
void tooglesLED(int led_mask);
void setLED(int led_mask);
void clearLED(int led_mask);

void init_rled_pwm(void);

void init_testled_pwm(void);

#endif
