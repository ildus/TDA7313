/*
 *	TDA7313.cpp - TDA7313 audoprocessor library.
 *	Author - Ildus Kurbangaliev, 2017-01-17.
 *  Released into the public domain.
 */

using namespace std;

#include "TDA7313.h"
#include <cassert>
#include <cstring>

TDA7313::TDA7313() {
#ifdef ARDUINO
	Wire.begin();
#endif

	//initizalize volume, set to -78.75dB (lowest level)
	vol_ctrl_data = 0b00111111;

	//initialize audio switch, gain 11.25dB, loudness on, input 1
	switch_data = 0b01000000;

	//initialize bass, 0dB
	bass_data = 0b01100111;

	//initialize treble, 0dB
	treble_data = 0b01110111;

	//initialize attenuators
	lf_att_data = 0b10000000;
	rf_att_data = 0b10100000;
	lr_att_data = 0b11000000;
	rr_att_data = 0b11100000;
}

/* I2C communication */
void TDA7313::write(int options) {
#ifdef ARDUINO
	Wire.beginTransmission(0x44); // 01000100
	std::vector<unsigned char> *v = get_i2c_sequence(options);
	for (int i = 0; i < v->size(); i++) {
		Wire.write((*v)[i]);
	}
	delete v;
	Wire.endTransmission();
#endif
}

void TDA7313::set_volume(unsigned char vol) {
	vol_ctrl_data = vol & 0b00111111;
}

unsigned char TDA7313::get_volume(void) {
	return vol_ctrl_data; // high two bits are always 0, no need to clean
}

bool TDA7313::is_volume_at_min(void) {
	return vol_ctrl_data == 0b00111111;
}

bool TDA7313::is_volume_at_max(void) {
	return vol_ctrl_data == 0;
}

/*
 * Increase volume for 1.25dB
 */
void TDA7313::increase_volume(void) {
	if (is_volume_at_max())
		return;

	vol_ctrl_data -= 1;
}

/*
 * Decrease volume for 1.25dB
 */
void TDA7313::decrease_volume(void) {
	if (is_volume_at_min())
		return;

	vol_ctrl_data += 1;
}

std::vector<unsigned char>* TDA7313::get_i2c_sequence(int options) {
	std::vector<unsigned char> *v = new std::vector<unsigned char>();

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
	for (int i = 0; i < 4; i++) {
		temp_memory[i] = attenuator_get_value(i);
		attenuator_set_value(i, 0xFF);
	}
}

void TDA7313::unmute(void) {
	for (int i = 0; i < 4; i++) {
		attenuator_set_value(i, temp_memory[i]);
		temp_memory[i] = 0;
	}
}

unsigned char *TDA7313::get_attenuator(int input) {
	switch (input) {
		case 0: return &lf_att_data;
		case 1: return &rf_att_data;
		case 2: return &lr_att_data;
	};

	return &rr_att_data;
}

void TDA7313::attenuator_set_value(int input, unsigned char value) {
	unsigned char *att_data = get_attenuator(input);
	value &= 0b00011111;
	*att_data = (*att_data & 0b11100000) + value;
}

unsigned char TDA7313::attenuator_get_value(int input) {
	unsigned char *att_data = get_attenuator(input);
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

/* these functions are for bass and treble, and operate with low 4 bits */
static inline unsigned char get_value(unsigned char data) {
	return data & 0b1111;
}

static void set_value(unsigned char *data, unsigned char val) {
	val &= 0b00001111; // clear high 4 bits
	*data = (*data & 0b11110000) + val;
}

//14dB
static inline bool is_max(unsigned char data) {
	return (data & 0b1111) == 0b1000;
}

static inline bool is_min(unsigned char data) {
	return (data & 0b1111) == 0;
}

static void increase_value(unsigned char *data) {
	unsigned char val = get_value(*data);
	if (is_max(val))
		return;

	if (val == 0b0111) {
		val = 0b1111;
	} else if ((1 << 3) & val) { //checking sign
		val -= 1;
	} else {
		val += 1;
	}
	set_value(data, val);
}

static void decrease_value(unsigned char *data) {
	unsigned char val = get_value(*data);
	if (is_min(val))
		return;

	if (val == 0b1111) {
		val = 0b0111;
	} else if ((1 << 3) & val) { //checking sign
		val += 1;
	} else {
		val -= 1;
	}
	set_value(data, val);
}

/* bass */
unsigned char TDA7313::get_bass_value(void) {
	return get_value(bass_data);
}

void TDA7313::set_bass_value(unsigned char val) {
	set_value(&bass_data, val);
}

bool TDA7313::is_bass_at_max(void) {
	return is_max(bass_data);
}

bool TDA7313::is_bass_at_min(void) {
	return is_min(bass_data);
}

void TDA7313::increase_bass(void) {
	increase_value(&bass_data);
}

void TDA7313::decrease_bass(void) {
	decrease_value(&bass_data);
}

/* treble */
unsigned char TDA7313::get_treble_value(void) {
	return get_value(treble_data);
}

void TDA7313::set_treble_value(unsigned char val) {
	set_value(&treble_data, val);
}

bool TDA7313::is_treble_at_max(void) {
	return is_max(treble_data);
}

bool TDA7313::is_treble_at_min(void) {
	return is_min(treble_data);
}

void TDA7313::increase_treble(void) {
	increase_value(&treble_data);
}

void TDA7313::decrease_treble(void) {
	decrease_value(&treble_data);
}
