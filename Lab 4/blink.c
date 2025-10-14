#include <msp430.h>

// Video : https://youtube.com/shorts/DeTFYkjo4lo?feature=share

void initGPIO() {
    //Switch 1 is on p4.1; Connected to GND so needs p-up
    //SW2 on 2.3
    //LED 1 (red) is on p1.0
    //LED 2 (green) is on p6.6
    //Buzzer on 1.5
    //Ultrasonic on 1.1

    P1DIR |= BIT0 | BIT1 | BIT5;
    P6DIR |= BIT6;

    P4DIR |= BIT1;
    P4REN |= BIT1;
    P4OUT |= BIT1;

    P2DIR |= BIT3;
    P2REN |= BIT3;
    P2OUT |= BIT3;

    P1OUT &=~ BIT5;

    P1IES = 0;



}


void setLEDS(int color) {
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

    case 3:
        P1OUT &=~ BIT0;
        P6OUT &=~ BIT6;
        break;
    }

}

void pollUSS() {

    P1DIR |= BIT1; //Set uss to output
    P1OUT |= BIT1; //Pull uss high
    __delay_cycles(10); //Wait 10 us
    P1OUT &=~ BIT1; //Pull uss low
    P1DIR &=~ BIT1; //Set uss to input

    P1IES &=~ BIT1; // Set to rising edge
    P1IE |= BIT1; // Enable interrupt


}

int timerVal = 0;

#pragma vector=PORT1_VECTOR
__interrupt void ussISR() {
    P1IFG &=~ BIT1; // Clear flag
    if (!(P1IES & BIT1)) {
        // Rising edge
        TB0CTL = TBSSEL__SMCLK | MC__CONTINUOUS; // Set timer to continuous
        P1IES |= BIT1; // Set to falling edge
        timerVal = 0;
    }

    else {
        // Falling Edge
        timerVal = TB0R;
        TB0CTL = TBSSEL__SMCLK | MC__STOP; // Stop Timer
        P1IE &=~ BIT1; // Disable interrupt
        TB0R = 0; // Reset timer

    }
}

float getDistance() {

    const float speedOfSound = 0.0343; // Cm/us

    pollUSS();
    while (P1IE & BIT1) continue; // Wait until polling is complete

    return timerVal * speedOfSound * .5;
}

void setup(){
    initGPIO();

    __enable_interrupt();

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

void setBuzzer(int state) {
    switch (state) {
    case 0:
        P1OUT &=~ BIT5;
        break;
    case 1:
        P1OUT |= BIT5;
        break;
    }
}


typedef struct {
    int sw;
    int trip;
} inputs_t;

typedef enum {
    IDLE,
    ARMED,
    TRIGGERED,
    MAX_STATES

} state_t;

int initArmed = 1;

state_t runIDLE(inputs_t inputs) {
    state_t nextState;
    setLEDS(3);
    setBuzzer(0);

    if (!inputs.sw) nextState = IDLE;
    else {
        nextState = ARMED;
        initArmed = 1;
    }

    return nextState;

}

state_t runARMED(inputs_t inputs) {
    state_t nextState;


    const float threshold = 40;
    const float cutoff = 200;
    float curDist = getDistance();



    static float lastDist;

    if (initArmed) {
        lastDist = curDist;
        initArmed = 0;
        return ARMED;
    }


    int trip = (abs(curDist - lastDist) > threshold);



    setLEDS(1);
    setBuzzer(0);

    if (inputs.sw) nextState = IDLE;
    else {
        if (curDist > cutoff) return ARMED; // Throw out extraneous readings

        if (trip) {
            nextState = TRIGGERED;
        }
        else nextState = ARMED;
    }

    lastDist = curDist;
    return nextState;
}

state_t runTRIGGERED(inputs_t inputs) {
    state_t nextState;
    setLEDS(0);
    setBuzzer(1);

    if (inputs.sw) nextState = IDLE;
    else nextState = TRIGGERED;

    return nextState;
}

state_t (*stateTable[MAX_STATES])(inputs_t) = {runIDLE, runARMED, runTRIGGERED};





void main(void) {
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                            // to activate previously configured port settings

    setup();


    state_t currentState = IDLE;
    state_t nextState = currentState;



    inputs_t inputs;

    while (1) {

        inputs.sw = sw1Pressed() || sw2Pressed();

        //lastDist = curDist;



        nextState = stateTable[currentState](inputs);
        currentState = nextState;





//        if (dist > 6) setLEDS(2);

   }
}

void delay_ms(int milliseconds) {
    // Assuming the MSP430 runs at 1 MHz, each loop iteration takes approximately 1 ms
    while (milliseconds) {
        __delay_cycles(1000); // 1000 cycles at 1 MHz is approximately 1 ms
        milliseconds--;
    }
}

