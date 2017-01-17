#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "TDA7313.h"

#ifndef ARDUINO
#define DEBUG
#endif


TEST_CASE("TDA7313 I2C", "[i2c]" ) {
	TDA7313 *tda = new TDA7313();

	SECTION("checking volume functions") {
		REQUIRE(tda->get_volume() == 0b111111); //lowest level
		auto seq = tda->get_i2c_sequence(OPT_VOLUME);
		REQUIRE((*seq)[0] == 0b00111111);
		delete seq;

		tda->increase_volume();
		seq = tda->get_i2c_sequence(OPT_VOLUME);

		REQUIRE(seq->size() == 1);
		REQUIRE((*seq)[0] == 0b00111110);
		delete seq;

		tda->set_volume(0b11000001);
		REQUIRE(tda->get_volume() == 0b00000001);

		tda->increase_volume();
		REQUIRE(tda->get_volume() == 0b00000000); //highest level
		REQUIRE(tda->is_volume_at_max() == true);
		tda->increase_volume();
		REQUIRE(tda->get_volume() == 0b00000000); //same

		tda->decrease_volume();
		REQUIRE(tda->get_volume() == 0b00000001);

		tda->set_volume(0b00111110);
		tda->decrease_volume();
		REQUIRE(tda->get_volume() == 0b00111111); //lowest level
		REQUIRE(tda->is_volume_at_min() == true);
		tda->decrease_volume();
		REQUIRE(tda->get_volume() == 0b00111111); //same
	}

	SECTION("checking audio switch functions") {
		REQUIRE(tda->get_input() == 0);
		REQUIRE(tda->get_gain() == 0);
		REQUIRE(tda->get_loudness() == true);

		auto seq = tda->get_i2c_sequence(OPT_SWITCH);
		REQUIRE((*seq)[0] == 0b01000000);
		delete seq;

		tda->set_input(2);
		REQUIRE(tda->get_input() == 2);

		tda->set_loudness(false);
		REQUIRE(tda->get_loudness() == false);

		tda->set_gain(3);
		REQUIRE(tda->get_gain() == 3);

		tda->set_gain(1);
		REQUIRE(tda->get_gain() == 1);

		seq = tda->get_i2c_sequence(OPT_SWITCH);
		REQUIRE((*seq)[0] == 0b01001110);
		delete seq;
	}

	SECTION("checking attenuators") {
		for (int i = 0; i < 4; i++)
			REQUIRE(tda->attenuator_get_value(i) == 0);

		tda->attenuator_set_value(0, 0b11110);
		REQUIRE(tda->attenuator_get_value(0) == 0b11110);
		tda->attenuator_decrease(0);
		REQUIRE(tda->attenuator_get_value(0) == 0b11111); //lowest level
		tda->attenuator_decrease(0);
		REQUIRE(tda->attenuator_get_value(0) == 0b11111); //same

		tda->attenuator_set_value(1, 1);
		REQUIRE(tda->attenuator_get_value(1) == 1);
		tda->attenuator_increase(1);
		REQUIRE(tda->attenuator_get_value(1) == 0); //highest level
		tda->attenuator_increase(1);
		REQUIRE(tda->attenuator_get_value(1) == 0); //same

		tda->attenuator_set_value(2, 1);
		tda->attenuator_set_value(3, 2);

		auto seq = tda->get_i2c_sequence(OPT_ATTENUATORS);
		REQUIRE((*seq)[0] == 0b10011111);
		REQUIRE((*seq)[1] == 0b10100000);
		REQUIRE((*seq)[2] == 0b11000001);
		REQUIRE((*seq)[3] == 0b11100010);
		delete seq;

		// all to lowest level
		tda->mute();
		seq = tda->get_i2c_sequence(OPT_ATTENUATORS);
		REQUIRE((*seq)[0] == 0b10011111);
		REQUIRE((*seq)[1] == 0b10111111);
		REQUIRE((*seq)[2] == 0b11011111);
		REQUIRE((*seq)[3] == 0b11111111);
		delete seq;

		// restore values
		tda->unmute();
		seq = tda->get_i2c_sequence(OPT_ATTENUATORS);
		REQUIRE((*seq)[0] == 0b10011111);
		REQUIRE((*seq)[1] == 0b10100000);
		REQUIRE((*seq)[2] == 0b11000001);
		REQUIRE((*seq)[3] == 0b11100010);
		delete seq;
	}

	SECTION("checking bass") {
		auto seq = tda->get_i2c_sequence(OPT_BASS);
		REQUIRE((*seq)[0] == 0b01100111);
		delete seq;

		REQUIRE(tda->get_bass_value() == 0b0111);
		for (int i = 0; i < 7; i++) {
			tda->decrease_bass();
		}
		REQUIRE(tda->get_bass_value() == 0);
		tda->decrease_bass();
		REQUIRE(tda->get_bass_value() == 0);
		REQUIRE(tda->is_bass_at_min() == true);

		for (int i = 0; i < 7; i++) {
			tda->increase_bass();
		}
		REQUIRE(tda->get_bass_value() == 0b0111);
		tda->increase_bass();
		REQUIRE(tda->get_bass_value() == 0b1111);

		for (int i = 0; i < 7; i++) {
			tda->increase_bass();
		}
		REQUIRE(tda->get_bass_value() == 0b1000);
		REQUIRE(tda->is_bass_at_max() == true);

		for (int i = 0; i < 15; i++) {
			tda->decrease_bass();
		}
		REQUIRE(tda->is_bass_at_min() == true);

		tda->set_bass_value(2);
		REQUIRE(tda->get_bass_value() == 2);

		seq = tda->get_i2c_sequence(OPT_BASS);
		REQUIRE((*seq)[0] == 0b01100010);
		delete seq;
	}

	SECTION("checking treble") {
		auto seq = tda->get_i2c_sequence(OPT_TREBLE);
		REQUIRE((*seq)[0] == 0b01110111);
		delete seq;

		REQUIRE(tda->get_treble_value() == 0b0111);
		for (int i = 0; i < 7; i++) {
			tda->decrease_treble();
		}
		REQUIRE(tda->get_treble_value() == 0);
		tda->decrease_treble();
		REQUIRE(tda->get_treble_value() == 0);
		REQUIRE(tda->is_treble_at_min() == true);

		for (int i = 0; i < 7; i++) {
			tda->increase_treble();
		}
		REQUIRE(tda->get_treble_value() == 0b0111);
		tda->increase_treble();
		REQUIRE(tda->get_treble_value() == 0b1111);

		for (int i = 0; i < 7; i++) {
			tda->increase_treble();
		}
		REQUIRE(tda->get_treble_value() == 0b1000);
		REQUIRE(tda->is_treble_at_max() == true);

		for (int i = 0; i < 15; i++) {
			tda->decrease_treble();
		}
		REQUIRE(tda->is_treble_at_min() == true);

		tda->set_treble_value(2);
		REQUIRE(tda->get_treble_value() == 2);

		seq = tda->get_i2c_sequence(OPT_TREBLE);
		REQUIRE((*seq)[0] == 0b01110010);
		delete seq;
	}
	delete tda;
}
