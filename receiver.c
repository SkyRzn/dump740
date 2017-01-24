/* Part of dump740, a UVD protocol decoder for RTLSDR devices.
 *
 * Copyright (c) 2016 Alexandr Ivanov <alexandr.sky@gmail.com>
 *
 * This file is free software: you may copy, redistribute and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "receiver.h"
#include "thread.h"
#include "options.h"
#include "routines.h"
#include <rtl-sdr.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>


static rtlsdr_dev_t *dev = NULL;
static int fd = -1;


static void init_rtlsdr()
{
	int cnt, i;
	int gains[64];
	char vend[256], prod[256], sn[256];

	cnt = rtlsdr_get_device_count();
	if (!cnt)
		fatal("RTLSDR devices not found.");

	fprintf(stderr, "Found %d device(s):\n", cnt);
	for (i = 0; i < cnt; i++) {
		rtlsdr_get_device_usb_strings(i, vend, prod, sn);
		print("\t%d: %s, %s, %s\n", i, vend, prod, sn);
	}

	if (rtlsdr_open(&dev, options.dev_index) < 0)
		fatal("RTLSDR device open error: %s", strerror(errno));

	rtlsdr_set_freq_correction(dev, options.freq_correction);
	rtlsdr_set_center_freq(dev, options.freq);

	rtlsdr_set_tuner_gain_mode(dev, (options.gain != ARG_GAIN_AUTO));
	if (options.gain != ARG_GAIN_AUTO) {
		if (options.gain == ARG_GAIN_MAX) {
			cnt = rtlsdr_get_tuner_gains(dev, gains);
			if (cnt <= 0)
				fatal("Gains not available");

			options.gain = gains[cnt - 1];
			print("Max available gain is: %.2f db\n", options.gain/10.0);
		}
		rtlsdr_set_tuner_gain(dev, options.gain);
		print("Setting gain to: %.2f db\n", options.gain/10.0);
	} else {
		print("Using AGC\n");
		rtlsdr_set_agc_mode(dev, 1);
	}

	rtlsdr_set_sample_rate(dev, SAMPLE_RATE);

	rtlsdr_reset_buffer(dev);

	print("Frequency set to: %d Hz\n", rtlsdr_get_center_freq(dev));
	print("Frequency correction set to: %d ppm\n", rtlsdr_get_freq_correction(dev));
	print("Gain reported by device: %.2f db\n", rtlsdr_get_tuner_gain(dev)/10.0);
}

void init_receiver()
{
	if (options.ifile) {
		fd = open(options.ifile, O_RDONLY);
		if (fd < 0)
			fatal("File error: %s", strerror(errno));
	} else
		init_rtlsdr();
}

void close_receiver()
{
	if (dev)
		rtlsdr_close(dev);
	if (fd >= 0)
		close(fd);
}

void *reader(void *ptr)
{
	block_t *block;
	int block_index = 1;
	int mlen, res;

	while (block = next_block(&block_index)) {
		if (dev) {
			res = rtlsdr_read_sync(dev, block->data, sizeof(uint16_t)*BLOCK_SIZE, &mlen);
		} else if (fd >= 0) {
			mlen = read(fd, block->data, BLOCK_SIZE);
			res = (mlen) ? 0 : -EINVAL;
		}

		if (res != 0) {
			set_end();
			continue;
		}
		block->data_length = mlen >> 1;
	}
}

