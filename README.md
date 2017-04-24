[![Build Status](https://travis-ci.org/ildus/TDA7313.svg?branch=master)](https://travis-ci.org/ildus/TDA7313)

# TDA7313 library for Arduino

## About

TDA7313 is digital controlled stereo audio processor. Features:

* 3 stereo inputs
* 4 speaker attenuators
* Treble and bass control
* Loudness function
* Other features

This library implements all features of TDA7313 by I2C protocol.

## Tests

	mkdir build && cd build
	cmake ../
	make && ./test

## Dependencies

* cmake for testing

* https://github.com/rpavlik/StandardCplusplus

## PCB

I've also included PCB footprints in Eagle format (https://cadsoft.io). It includes all elements from datasheet and voltage regulator to 9V.

![PCB]
(https://habrastorage.org/files/fc1/a0f/32b/fc1a0f32b7794bdb8be0d9e7b7a40666.png)
