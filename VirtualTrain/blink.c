/*
 * Jaskin Kabir
 * 7/4/2023
 * Lab 3
 * https://youtu.be/C9knZ8awyS4
 */

#include <msp430.h>
#include <Four_Digit_Display.h>


typedef enum {
    STOPCCW,
    STOPCW,
    CCW,
    CW,
    MAX_STATES,
} state_t;



int trainPos=0;
int dir = 0;

void setLEDS(const int color); // Prevent function not found error

state_t runStopCCW(int inputs);
state_t runStopCW(int inputs);
state_t runCCW(int inputs);
state_t runCW(int inputs);

state_t (*stateTable[MAX_STATES])(int) = {runStopCCW, runStopCW, runCCW, runCW};


state_t runStopCCW(int inputs) {
    state_t nextState;
    dir = 0;
    setLEDS(0);

    switch (inputs) {
    case 0:
        nextState = STOPCCW;
        break;
    case 1:
        nextState = STOPCW;
        break;
    case 2:
        nextState = CCW;
        break;
    case 3:
        nextState = CW;
        break;

    }
    return nextState;
}

state_t runStopCW(int inputs) {
    state_t nextState;
    dir = 0;
    setLEDS(0);

    switch (inputs) {
    case 0:
        nextState = STOPCW;
        break;
    case 1:
        nextState = STOPCCW;
        break;
    case 2:
        nextState = CW;
        break;
    case 3:
        nextState = CCW;
        break;

    }
    return nextState;
}

state_t runCCW(int inputs) {
    state_t nextState;
    dir = 1;
    setLEDS(1);

    switch (inputs) {
    case 0:
        nextState = CCW;
        break;
    case 1:
        nextState = CW;
        break;
    case 2:
        nextState = STOPCCW;
        break;
    case 3:
        nextState = STOPCW;
        break;

    }
    return nextState;
}

state_t runCW(int inputs) {
    state_t nextState;
    dir = 2;
    setLEDS(1);

    switch (inputs) {
    case 0:
        nextState = CW;
        break;
    case 1:
        nextState = CCW;
        break;
    case 2:
        nextState = STOPCW;
        break;
    case 3:
        nextState = STOPCCW;
        break;

    }
    return nextState;
}



int sw1Pressed() {
    int edge = 0;
    static int previousState = 0;
    int currentState = !(P4IN & BIT1);

    if (!previousState && currentState) {
        edge = 1;
        setLEDS(0);
    }
    else edge = 0;

    previousState = currentState;

    return edge;

}

int sw2Pressed() {
    int edge = 0;
    static int previousState = 0;
    int currentState = !(P2IN & BIT3);

    if (!previousState && currentState)  {
        edge = 1;
        setLEDS(1);
    }
    else edge = 0;

    previousState = currentState;
    return edge;

}

void initGPIO() {
    //Switch 1 is on p4.1; Connected to GND so needs p-up
    //SW2 on 2.3
    //LED 1 (red) is on p1.0
    //LED 2 (green) is on p6.6
    four_digit_init();


    P1DIR |= BIT0;
    P6DIR |= BIT6;

    P4DIR |= BIT1;
    P4REN |= BIT1;
    P4OUT |= BIT1;

    P2DIR |= BIT3;
    P2REN |= BIT3;
    P2OUT |= BIT3;



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

void displayTrain(int pos) {
    static int prevPos = -1;

    if (prevPos == pos) return; /// Prevent flickering
    four_digit_clear();

    prevPos = pos;


    switch (pos) {
    case 0:
        display_segment(POS_2, SEG_D);
        display_segment(POS_3, SEG_D);
        display_segment(POS_4, SEG_D);
        return;
    case 1:
        display_segment(POS_1, SEG_D);
        display_segment(POS_2, SEG_D);
        display_segment(POS_3, SEG_D);
        return;
    case 2:
        display_segment(POS_2, SEG_D);
        display_segment(POS_1, SEG_D | SEG_E);
        return;
    case 3:
        display_segment(POS_1, SEG_D | SEG_E | SEG_F);
        return;
    case 4:
        display_segment(POS_1, SEG_A | SEG_E | SEG_F);
        return;
    case 5:
        display_segment(POS_1, SEG_A | SEG_F);
        display_segment(POS_2, SEG_A);
        return;
    case 6:
        display_segment(POS_1, SEG_A);
        display_segment(POS_2, SEG_A);
        display_segment(POS_3, SEG_A);
        return;
    case 7:
        display_segment(POS_2, SEG_A);
        display_segment(POS_3, SEG_A);
        display_segment(POS_4, SEG_A);
        return;
    case 8:
        display_segment(POS_3, SEG_A);
        display_segment(POS_4, SEG_A | SEG_B);
        return;
    case 9:
        display_segment(POS_4, SEG_A | SEG_B | SEG_C);
        return;
    case 10:
        display_segment(POS_4, SEG_B | SEG_C | SEG_D);
        return;
    case 11:
        display_segment(POS_4, SEG_C | SEG_D);
        display_segment(POS_3, SEG_D);
        return;
    }
}

void initACLK() {
    //SEt XT1CLK as ACLK source
    CSCTL4 |= SELA__XT1CLK;
    // Set low frequency mode
    CSCTL6 |= XT1BYPASS_1 | XTS_0;
    // Disable divider
    CSCTL6 |= DIVM_0;
}

void initTimerB0() {
    // Configure Timer B0 to be up counter using ACLK
    TB0CTL = TBSSEL__ACLK | MC__UP;
    // 32768 counts required for 1 second
    TBCCR0 = 32768;
    //Enable interrupt
    TB0CCTL0 |= CCIE;
    //Clear flag
    TB0CCTL0 &=~ CCIFG;
}

#pragma vector = TIMER0_B0_VECTOR
__interrupt void TIMER0_B0_ISR(void) {
    switch (dir) {
    case 1:
        trainPos--;
        break;
    case 2:
        trainPos++;
        break;
    }

    //Prevent out of bounds position
    if (trainPos == 12) trainPos = 0;
    if (trainPos == -1) trainPos = 11;

    //clear interrupt flag
    TB0CCTL0 &=~ CCIFG;
}



void main(void) {
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                            // to activate previously configured port settings
    initGPIO();
    initACLK();
    initTimerB0();



    displayTrain(trainPos);

    state_t currentState = STOPCCW;
    state_t nextState = STOPCCW;
    int in;

    __enable_interrupt();

    while(1)
    {
        in = (sw1Pressed() << 1 ) | sw2Pressed(); // Create input bitfield

        nextState = stateTable[currentState](in);

        currentState = nextState;


        displayTrain(trainPos);

    }
}
