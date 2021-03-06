# Arduino Radio Code

This projects team consisted of:
James Hessell-Bowman
Sam Wass
Chitesh Munraj

This code is based upon 2 arduinos using nRF24L01+ radio modules to communicate with each other. 

Arduino A has these components:
- one TC1602A 16x2 LCD using a PCF8574 I2C interface 
- one Joystick Module with button
- one nRF24L01 radio module with power converter

Arduino B has these components:
- one SG90 servo
- one B5k Potentiometer
- one 2 wire motor we found in a box
- one green led with 1k resistor
- one nRF24L01 radio module with power converter

Requirements: 
- Reads X and Y analog signals and a single digital input from a joystick.
- The joystick should be connected to the first μCU.
- The joystick’s X-axis signal should control the position of a servo.
- The joystick’s Y-axis should increment/decrement the speed of a DC motor.
- The joystick’s button press should toggle whether the X- and Y-signals are enabled or not using an interrupt.
- Disabling the joystick inputs should reset the the servo position, switch off the DC motor and toggle an indicator LED.
- The value of a trim potentiometer should determine the default reset position of the servo.
- The servo, DC motor and potentiometer should be connected to the second μCU.
- Communication between the two μCUs should be achieved wirelessly by using NRF24L01 transceiver modules.
- X, Y, status and trim information should be displayed on a TC1602A 16x2 LCD using a PCF8574 I2C interface connected to the first μCU.
- The LCD display updates should be triggered at a 100 ms interval using a timer interrupt.

All of these requirements have been met as far as we can tell and the whole project is working

## Block Diagram:

![Picture of diagrams](https://media.discordapp.net/attachments/202257159428374528/772292638320623647/IMG_20201101_155325.jpg?width=507&height=676)

## Diagrams and Pictures

Arduino A Picture

![Picture of  Arduino A](https://media.discordapp.net/attachments/202257159428374528/772335714191933440/20201101_184617.jpg?width=507&height=676)


Arduino B Picture

![Picture of Arduino B](https://media.discordapp.net/attachments/202257159428374528/772335661947944980/20201101_184603.jpg?width=720&height=540)


Arduino A Diagram

![Diagram for Arduino A](https://media.discordapp.net/attachments/715520138442702848/772367218800984084/dis.png?width=720&height=402)

Arduino B Diagram

![Diagram for Arduino B](https://media.discordapp.net/attachments/580265362654887956/772342024232894485/unknown.png?width=720&height=417)

## States

On.
Off.
Disabled.
Sending.
Receiving.

## Notes and after thoughts 
This is where I say how painful this whole thing was. Most of the components connected properly straight away but working out how interrupts work and working out how to get the radio modules to do two way communication was a pain. Finally after like 6 hours of research (and a bunch of post 12 hours at the office breaking of the code) we managed to work out what an AckPayload was and how to use it to send the trim data back to in the response to a received message thus not requiring to switch the communications back the other way in-between which was locking the whole sketch up.
Otherwise this has been a fun and interesting challenge to complete and we've had a lot of fun doing it.


## Contributions

|Name|Preparation|Participation|Reliability|Comments|
|---|---|---|---|---|
|James|5|5|5|Coding master made communication work in the end|
|Chitesh|5|5|5|Research, documentation and support|
|Sam|5|5|5|Hardware and bug fixing (aka why the hell doesn't this work)|