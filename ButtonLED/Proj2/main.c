#include <msp430.h>


/**
 * main.c
 */

int buttonPressed() {
    int edge = 0;
    static int previousState = 0;
    int currentState = !(P4IN & BIT1);

    if (!previousState && currentState) edge = 1;
    else edge = 0;

    previousState = currentState;
    return edge;

}

void setupGPIO() {
//Switch 1 is on p4.1; Connected to GND so needs p-up
//LED 1 (red) is on p1.0
//LED 2 (green) is on p6.6


P1DIR |= BIT0;
P6DIR |= BIT6;

P4DIR |= BIT1;
P4REN |= BIT1;
P4OUT |= BIT1;

}

void setLEDS(const int color) {
    switch (color) {
    case 0: //red
        P1OUT |= BIT0;
        P6OUT &= ~BIT6;
        break;
    case 1: // green
        P1OUT &= ~BIT0;
        P6OUT |= BIT6;
        break;
    case 2: // red and green
        P1OUT |= BIT0;
        P6OUT |= BIT6;
        break;
    }
}

int main(void)
{
    PM5CTL0 &= ~LOCKLPM5; // Disable the GPIO power-on default high-impedance mode
	WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer
	setupGPIO();
	setLEDS(0);

	int color = 0;

	while (1) {
	    if (buttonPressed()) {
	        color++;
	        if (color == 3) color = 0;

	        setLEDS(color);
	    }
	}
	
}
