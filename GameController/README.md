# Project Description

This project explores ADC control, clock configuration, and UART communication. The assignment was to implement a simple game controller that reads the position of a potentiometer to control the horizontal position of a fighter jet in a Top Gun game displayed on a connected computer via UART. The controller also has a button that, when pressed, sends a command to the game to shoot missiles.

# Implementation Details
The game expects each command to be sent as a single byte over UART. The value of the byte indicates the horizontal position of the jet, with 0 representing the far left and 254 representing the far right. When the button is pressed, a special command byte (0xFF) is sent to indicate that a missile should be fired. If the potentiometer is at the far right position, which would also correspond to the byte value 0xFF, the controller instead sends the value 0xFE to avoid confusion. The main loop polls the ADCBUSY flag to wait for the ADC to finish conversion and then sends the appropriate byte over UART. The fire command is issued in the button's interrupt service routine (ISR) to ensure immediate response when the button is pressed.

The onboard UART module requires proper configuration of the clock system to generate the desired baud rate. In this project, the clock divider is configured to generate a baud rate of 9600bps.




# Demonstration Video
[![Controller Demo](https://img.youtube.com/vi/VCt6SuJNLRY/hqdefault.jpg)](https://www.youtube.com/shorts/VCt6SuJNLRY)