#ifndef _TDA7313_H_
#define _TDA7313_H_

#include "Wire.h"

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

const double	MAX_VOLUME = 0,
				MIN_VOLUME = -78.75;

class TDA7313 {
	private:
		volume_ctrl vol_ctrl_data;
		void write_data(unsigned char *buf, int length);
	public:
		TDA7313();
		void set_input(int number);
		void increase_volume(void);
		void decrease_volume(void);
		void apply(void);
};

#endif
