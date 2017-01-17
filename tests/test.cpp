#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "TDA7313.h"


TEST_CASE("TDA7313 I2C", "[i2c]" ) {
	TDA7313 *tda = new TDA7313();

	SECTION("checking volume functions") {
		REQUIRE(tda->get_volume() == 0b111111); //lowest level
		auto seq = tda->get_i2c_sequence(OPT_VOLUME);
		REQUIRE((*seq)[0] == 0b00111111);
		free(seq);

		tda->increase_volume();
		seq = tda->get_i2c_sequence(OPT_VOLUME);

		REQUIRE(seq->size() == 1);
		REQUIRE((*seq)[0] == 0b00111110);
		free(seq);

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
		free(seq);

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
		free(seq);
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
		free(seq);
	}

	free(tda);
}
