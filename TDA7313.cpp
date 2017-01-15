#include "TDA7313.h"
#include <cassert>

TDA7313::TDA7313() {
#ifdef Arduino_h
	Wire.begin();
#endif

	//initizalize volume, set to -78.75dB
	vol_ctrl_data.high_bits = 0;
	vol_ctrl_data.b = 0b111; //lowest level
	vol_ctrl_data.a = 0b111;

	//initialize audio switch
	switch_data.high_bits = 0b010;
	switch_data.gain = 0; // 11.25dB
	switch_data.loudness = 0; // loudness on
	switch_data.input = 0; // input 1

	//initialize bass
	bass_data.high_bits = 0b0110;
	bass_data.level = 0; // -14dB

	//initialize treble
	treble_data.high_bits = 0b0111;
	treble_data.level = 0; // -14dB

	//initialize attenuators
	lf_att_data.high_bits = 0b100;
	lf_att_data.b = 0;
	lf_att_data.a = 0;

	rf_att_data.high_bits = 0b101;
	rf_att_data.b = 0;
	rf_att_data.a = 0;

	lr_att_data.high_bits = 0b110;
	lr_att_data.b = 0;
	lr_att_data.a = 0;

	rr_att_data.high_bits = 0b111;
	rr_att_data.b = 0;
	rr_att_data.a = 0;
}

/* I2C communication */
void TDA7313::write_data(unsigned char *buf, int length) {
#ifdef Arduino_h
	Wire.beginTransmission(0x44); // 01000100
	for (int i = 0; i < length; i++) {
		Wire.write(buf[i]);
	}
	Wire.endTransmission();
#endif
}

void TDA7313::set_volume(unsigned char vol) {
	vol_ctrl_data.b = vol >> 3;
	vol_ctrl_data.a = vol & 0b00000111;
}

unsigned char TDA7313::get_volume(void) {
	return (vol_ctrl_data.b << 3) + vol_ctrl_data.a;
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
	unsigned char buf[] = {0, 0, 0, 0, 0, 0, 0, 0};

	memcpy(buf, &vol_ctrl_data, 1);
	memcpy(buf + 1, &switch_data, 1);
	memcpy(buf + 2, &bass_data, 1);
	memcpy(buf + 3, &treble_data, 1);

	//attenuators
	for (int i; i < 4; i++) {
		attenuator_ctrl *att_data = get_attenuator(i);
		assert(att_data != NULL);
		memcpy(buf + 4 + i, att_data, 1);
	}
	this->write_data(buf, sizeof(buf));
}

void TDA7313::set_input(int num) {
	assert(num >= 0 && num <= 2);
	switch_data.input = num;
}

void TDA7313::set_loudness(bool on) {
	switch_data.loudness = int(!on);
}

/*
 * Set gain
 * 0	11.25dB
 * 1	7.5dB
 * 2	3.75dB
 * 3	0dB
 */
void TDA7313::set_gain(int num) {
	assert(num >= 0 && num <= 3);
	switch_data.gain = num;
}

void TDA7313::mute(void) {
	for (int i; i < 4; i++) {
		attenuator_ctrl *att_data = get_attenuator(i);
		assert(att_data != NULL);
		att_data->b = 0b11;
		att_data->a = 0b111;
	}
}

attenuator_ctrl *TDA7313::get_attenuator(int input) {
	switch (input) {
		case 0: return &lf_att_data;
		case 1: return &rf_att_data;
		case 2: return &lr_att_data;
		case 3: return &rr_att_data;
	}
	return NULL;
}

void TDA7313::attenuator_set_value(int input, unsigned char value) {
	attenuator_ctrl *att_data = get_attenuator(input);
	if (att_data == NULL)
		return;

	value &= 0b00011111;
	att_data->b = value >> 3;
	att_data->a = value & 0b00111;
}

unsigned char TDA7313::attenuator_get_value(int input) {
	attenuator_ctrl *att_data = get_attenuator(input);
	assert(att_data != NULL);
	return (att_data->b << 3) + att_data->a;
}

/* minimum: -38.75dB, means it's muted */
void TDA7313::attenuator_decrease(int input) {
	attenuator_ctrl *att_data = get_attenuator(input);
	if (att_data == NULL)
		return;

	if ((att_data->b == 0b11) && (att_data->a == 0b111))
		/* nothing to decrease */
		return;

	att_data->a += 1;
	if (att_data->a == 0) { // overflow
		att_data->b += 1;
	}
}

/* maximum: 0 */
void TDA7313::attenuator_increase(int input) {
	attenuator_ctrl *att_data = get_attenuator(input);
	if (att_data == NULL)
		return;

	if ((att_data->b == 0) && (att_data->a == 0))
		/* nothing to increase */
		return;

	att_data->a -= 1;
	if (att_data->a == 0b111) { // overflow
		att_data->b -= 1;
	}
}
