using namespace std;

#include "TDA7313.h"
#include <cassert>
#include <cstring>

TDA7313::TDA7313() {
#ifdef Arduino_h
	Wire.begin();
#endif

	//initizalize volume, set to -78.75dB (lowest level)
	vol_ctrl_data = 0b00111111;

	//initialize audio switch, gain 11.25dB, loudness on, input 1
	switch_data = 0b01000000;

	//initialize bass, -14dB
	bass_data = 0b01100000;

	//initialize treble, -14dB
	treble_data = 0b01110000;

	//initialize attenuators
	lf_att_data = 0b10000000;
	rf_att_data = 0b10100000;
	lr_att_data = 0b11000000;
	rr_att_data = 0b11100000;
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
	vol_ctrl_data = vol & 0b00111111;
}

unsigned char TDA7313::get_volume(void) {
	return vol_ctrl_data; // high two bits are always 0, no need to clean
}

/*
 * Increase volume for 1.25dB
 */
void TDA7313::increase_volume(void) {
	if (vol_ctrl_data == 0)
		return;

	vol_ctrl_data -= 1;
}

/*
 * Decrease volume for 1.25dB
 */
void TDA7313::decrease_volume(void) {
	if (vol_ctrl_data == 0b00111111)
		return;

	vol_ctrl_data += 1;
}

std::vector<unsigned char>* TDA7313::get_i2c_sequence(int options) {
	auto v = new std::vector<unsigned char>();

	if (options & OPT_VOLUME)
		v->push_back(vol_ctrl_data);

	if (options & OPT_SWITCH)
		v->push_back(switch_data);

	if (options & OPT_BASS)
		v->push_back(bass_data);

	if (options & OPT_TREBLE)
		v->push_back(treble_data);

	//attenuators
	if (options & OPT_ATTENUATORS) {
		for (int i; i < 4; i++) {
			unsigned char *att_data = get_attenuator(i);
			assert(att_data != nullptr);
			v->push_back(*att_data);
		}
	}

	return v;
}

/* Set audio source, values 0..2 (3 inputs), bits - 000000XX */
void TDA7313::set_input(int num) {
	if (!(num >= 0 && num <= 2))
		return;

	switch_data = (switch_data & 0b11111100) + num;
}

int TDA7313::get_input(void) {
	return switch_data & 0b11;
}

/* Loudness, bits - 00000X00 */
void TDA7313::set_loudness(bool on) {
	switch_data ^= (-((int)(!on)) ^ switch_data) & (1 << 2);
}

bool TDA7313::get_loudness(void) {
	return !((bool)((switch_data >> 2) & 1));
}

/*
 * Set gain. Bits 000XX000
 * 0	11.25dB
 * 1	7.5dB
 * 2	3.75dB
 * 3	0dB
 */
void TDA7313::set_gain(int num) {
	assert(num >= 0 && num <= 3);
	switch_data &= 0b11100111;
	switch_data |= num << 3;
}

int TDA7313::get_gain(void) {
	return switch_data >> 3 & 0b11;
}

void TDA7313::mute(void) {
	for (int i; i < 4; i++) {
		attenuator_set_value(i, 0xFF);
	}
}

unsigned char *TDA7313::get_attenuator(int input) {
	switch (input) {
		case 0: return &lf_att_data;
		case 1: return &rf_att_data;
		case 2: return &lr_att_data;
		case 3: return &rr_att_data;
	}
	return nullptr;
}

void TDA7313::attenuator_set_value(int input, unsigned char value) {
	unsigned char *att_data = get_attenuator(input);
	if (att_data == nullptr)
		return;

	value &= 0b00011111;
	*att_data = (*att_data & 0b11100000) + value;
}

unsigned char TDA7313::attenuator_get_value(int input) {
	unsigned char *att_data = get_attenuator(input);
	assert(att_data != nullptr);
	return *att_data & 0b00011111;
}

/* minimum: -38.75dB, means it's muted */
void TDA7313::attenuator_decrease(int input) {
	unsigned char val = attenuator_get_value(input);
	if (val == 0b11111)
		return;

	val += 1;
	attenuator_set_value(input, val);
}

/* maximum: 0 */
void TDA7313::attenuator_increase(int input) {
	unsigned char val = attenuator_get_value(input);
	if (val == 0)
		return;

	attenuator_set_value(input, --val);
}
