#include "TDA7313.h"

TDA7313 *tda;

void setup() {
	tda = new TDA7313();
	Serial.begin(9600);
}

void loop() {
	Serial.println("volume goes up");
	while (!tda->is_max_volume()) {
		tda->increase_volume();
		delay(100);
	}

	Serial.println("volume goes down");
	while (!tda->is_min_volume()) {
		tda->decrease_volume();
		delay(100);
	}
}
