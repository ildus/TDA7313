#include "TDA7313.h"

TDA7313 *tda;

bool volume_checked = false;

void setup() {
	tda = new TDA7313();
	Serial.begin(9600);
	tda->set_input(1);
	tda->write(OPT_ALL);
}

void loop() {
	if (!volume_checked) {
		Serial.println("volume goes up");
		while (!tda->is_volume_at_max()) {
			tda->increase_volume();
			tda->write(OPT_VOLUME);
			delay(100);
		}

		Serial.println("volume goes down");
		while (!tda->is_volume_at_min()) {
			tda->decrease_volume();
			tda->write(OPT_VOLUME);
			delay(100);
		}

		tda->set_volume(0b010000); //-20dB
		tda->write(OPT_VOLUME);
		delay(2000);
		tda->mute();
		Serial.println("muted");
		tda->write(OPT_ATTENUATORS);
		delay(2000);
		tda->unmute();
		tda->write(OPT_ATTENUATORS);
		Serial.println("unmuted");

		volume_checked = true;
		Serial.println("volume checked");
	}

	delay(1000);
}
