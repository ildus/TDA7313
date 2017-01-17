# TDA7313 library for Arduino

## About

TDA7313 is digital controlled stereo audio processor. Features:

1. 3 stereo inputs
2. 4 speaker attenuators
3. Treble and bass control
4. Loudness function
5. Other features

This library implements all features of TDA7313 by I2C protocol.

## Tests

	mkdir build && cd build
	cmake ../
	make && ./test

## Dependencies

1. cmake for testing

2. https://github.com/rpavlik/StandardCplusplus

## PCB

I've also included PCB footprints in Eagle format (https://cadsoft.io). It includes all elements from datasheet and voltage regulator to 9V.

![PCB]
(http://dl.getdropbox.com/u/930627/images/eznnkqfjjwqgkpcoryel.png)
