#include <Wire.h>
#include "TDA7313.h"

TDA7313::TDA7313() {
	Wire.begin();

	//initizalize volume, set to -78.75dB
	vol_ctrl_data.high_bits = 0;
	vol_ctrl_data.b = 0b111;
	vol_ctrl_data.a = 0b111;
}

/* I2C communication */
void TDA7313::write_data(unsigned char *buf, int length) {
	Wire.beginTransmission(0x44); // 01000100
	for (int i = 0; i < length; i++) {
		Wire.write(buf[i]);
	}
	Wire.endTransmission();
}

/*
 * Increase volume for 1.25dB
 */
void TDA7313::increase_volume(void) {
	if ((vol_ctrl_data.b == 0) && (vol_ctrl_data.a == 0))
		return;

	vol_ctrl_data.a -= 1;
	if (vol_ctrl_data.a == 0b111) { //overflow
		vol_ctrl_data.b -= 1;
	}
}

/*
 * Decrease volume for 1.25dB
 */
void TDA7313::decrease_volume(void) {
	if ((vol_ctrl_data.b == 0b111) && (vol_ctrl_data.a == 0b111))
		return;

	vol_ctrl_data.a += 1;
	if (vol_ctrl_data.a == 0b000) { //overflow
		vol_ctrl_data.b += 1;
	}
}

void TDA7313::apply(void) {
	unsigned char buf[] = {0};

	memcpy(buf, &vol_ctrl_data, 1);
	this->write_data(buf, 1);
}
