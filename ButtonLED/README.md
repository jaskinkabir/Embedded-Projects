# ButtonLED: Project Description
This project explores GPIO control, button debouncing, and software state machine implementation on a microcontroller. The assignment was to use a button on the development board to cycle the color of the onboard RGB LED through red, green, yellow states. The assignment also required some sort of debouncing mechanism to ensure reliable button presses.

# Implementation Details
The button is read through polling in the main loop, with debouncing handled via a simple state machine that detects a rising edge on the button's GPIO line. After the button press is read, the RGB LED color is changed by updating the appropriate GPIO output lines.