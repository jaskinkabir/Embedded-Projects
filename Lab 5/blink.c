#include <msp430.h>
#include <stdint.h>

//Video : https://youtu.be/VCt6SuJNLRY

void initGPIO() {
    //Switch 1 is on p4.1; Connected to GND so needs p-up
    //SW2 on 2.3
    //LED 1 (red) is on p1.0
    //LED 2 (green) is on p6.6
    //Buzzer on 1.5
    //Ultrasonic on 1.1

    P1DIR |= BIT0;
    P6DIR |= BIT6;

    P4DIR |= BIT1;
    P4REN |= BIT1;
    P4OUT |= BIT1;

    P2DIR |= BIT3;
    P2REN |= BIT3;
    P2OUT |= BIT3;

    P4IE = BIT1;
    P2IE = BIT3;

    P4IES &=~ BIT1;
    P2IES &=~ BIT3;

}


void initClock() {
    CSCTL4 |= SELA__XT1CLK;

    CSCTL6 |= XT1BYPASS_1 | XTS_0;
}

void initUart() {

    UCA1CTLW0 = UCSWRST; //Enter Reset state


    // Set pins as secondary function
    P4SEL0 |= (BIT3 | BIT2);
    P4SEL1 &=~ (BIT3 | BIT2);

    UCA1CTLW0 |= UCSSEL__ACLK;

    // 9600BR config From datasheet
    UCA1BR0 = 3; // 32768/9600
    UCA1BR1 = 0;
    UCA1MCTLW |= 0x9200;

    UCA1CTLW0 &= ~UCSWRST; // End reset/config state
    UCA1IE |= UCRXIE; // Enable RX interrupt

}

void initAdc() {
    ADCCTL0 |= ADCON_1 | ADCSHT_2; // 16 clock cycles to allow for 8 bit resolution
    ADCCTL1 |= ADCSHP; // Internal signal
    ADCCTL2 &=~ ADCRES; // 8-bit res
    ADCMCTL0 |= ADCINCH_1 | ADCSREF_0; // Using A1 channel and 0-3.3V
}

void startADC() {
    ADCCTL0 |= ADCENC | ADCSC;
}

void setup() {
    initClock();
    initUart();
    initGPIO();
    initAdc();

    __enable_interrupt();
}

void sendByte(uint8_t byte) {
    while(!(UCA1IFG & UCTXIFG)); // Wait until previous transmission is finished

    UCA1TXBUF = byte;
}

void buttonPress() {
    volatile int i = 0;

    for (i = 0; i < 50; i++) sendByte(255);

}

//#pragma vector=USCI_A1_VECTOR
//__interrupt void uartInterrupt() {
//
//
//    if (UCA1TXBUF == 6) sendByte('c');
//    else sendByte('w');
//
//    UCA1IFG &=~ UCRXIFG;
//}

#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
UCA1TXBUF = UCA1RXBUF;
__no_operation();
// Clear flag
UCA1IFG = 0;
}

#pragma vector=PORT2_VECTOR
__interrupt void P2ISR() {
    P2IFG = 0;
    buttonPress();
}

#pragma vector=PORT4_VECTOR
__interrupt void P4ISR() {
    P4IFG = 0;
    buttonPress();
}



void main(void) {
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode

    volatile unsigned int adcResult;

    setup();

    while(1) {
        startADC();
        while (ADCCTL1 & ADCBUSY);
        adcResult = ADCMEM0;
        if (adcResult == 255) continue;
        sendByte(adcResult);

    }

}
