#ifndef _TDA7313_H_
#define _TDA7313_H_

#ifdef Arduino_h
#include "Wire.h"
#endif

/* Volume:
 * -78.75dB ... 0
 * But: functions operate with positive values 0..78.75dB
 */
struct volume_ctrl
{
	unsigned int high_bits:2;
	unsigned int b:3;
	unsigned int a:3;
};

struct audio_switch_ctrl
{
	unsigned int high_bits:3;
	unsigned int gain:2;
	unsigned int loudness:1;
	unsigned int input:2;
};

struct bass_and_treble_ctrl
{
	unsigned int high_bits:4;
	unsigned int level:4;
};

struct attenuator_ctrl
{
	unsigned int high_bits: 3;
	unsigned int b: 2;
	unsigned int a: 3;
};

class TDA7313 {
	private:
		volume_ctrl vol_ctrl_data;
		audio_switch_ctrl switch_data;
		bass_and_treble_ctrl bass_data;
		bass_and_treble_ctrl treble_data;
		attenuator_ctrl lf_att_data;
		attenuator_ctrl rf_att_data;
		attenuator_ctrl lr_att_data;
		attenuator_ctrl rr_att_data;

		void write_data(unsigned char *buf, int length);
		attenuator_ctrl* get_attenuator(int input);
	public:
		TDA7313();

		/* select one of three inputs, starting from 0 */
		void set_input(int num);

		/* audio switch functions */
		void set_loudness(bool on);
		void set_gain(int num);

		/* volume related functions */
		unsigned char get_volume(void);
		void set_volume(unsigned char vol);
		void increase_volume(void);
		void decrease_volume(void);
		void mute(void);

		/* attenuators */
		void attenuator_set_value(int input, unsigned char value);
		unsigned char attenuator_get_value(int input);
		void attenuator_decrease(int input);
		void attenuator_increase(int input);

		/* apply all changes */
		void apply(void);
};

#endif
