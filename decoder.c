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

#include "decoder.h"
#include <stdlib.h>
#include <math.h>


#define MAX_MSG_SIZE	728 // (44 us header + 8*20*2 us data) * 2 (1 item = 0.5 us)
#define MAG_TABLE_SIZE	65536


static const int timing_zk1[] = {28, 45, 61, 85, -1}; //us: 14 + 8.5 + 8 + 12
static const int timing_zk2[] = {22, 50, 66, 82, -1}; //us: 11 + 14 + 8 + 8
static const int timing_zk3[] = {36, 56, 80, 88, -1}; //us: 18 + 10 + 12 + 4

static uint16_t *mag_table = NULL;


static inline int pulse(uint16_t *m, int i)
{
	uint16_t high;

	high = ((uint32_t)m[i] + (uint32_t)m[i+1])>>2;

	if (m[i-1] < high && m[i+2] < high) {
		if (m[i] > m[i+1])
			return i;
		return i + 1;
	}
	return 0;
}

static inline int read_data(uint16_t *m)
{
	int i, shift = 0, r1 = 0, r2 = 0;

	m += 8;

	for (i = 0; i < 320; i+=16) {
		if (m[i] > m[i+8])
			r1 += 1 << shift;

		if (m[i+320] > m[i+8+320])
			r2 += 1 << shift;

		shift++;
	}

	if (r1 != r2)
		return -1;

	return r1;
}

static inline int check_timing(const int *timing, uint16_t *m, int i)
{
	int p;
	const int *t;

	for (t = timing; *t != -1; t++) {
		p = pulse(m, i + *t);
		if (!p)
			return 0;

	}
	return p;
}

void init_decoder()
{
	int x, y;
	float i, q;

	mag_table = malloc(sizeof(uint16_t) * MAG_TABLE_SIZE);
	if (!mag_table)
		fatal("Malloc error.");

	for (x = 0; x < 256; x++) {
		for (y = 0; y < 256; y++) {
			i = (float)x - 127.5;
			q = (float)y - 127.5;
			mag_table[x*256 + y] = lround(sqrt(i*i + q*q) * 363.45);
		}
	}
}

void close_decoder()
{
	free(mag_table);
}

int decode(uint16_t *block,
		   int blen,
#ifdef TEST
		   uint64_t *msg,
#else
		   uint32_t *msg,
#endif
		   int max_mlen)
{
	int type, i, last_pulse, mlen = 0;
	uint32_t data;

	for (i = 0; i < blen; i++)
		block[i] = mag_table[block[i]];

	for (i = 1; i < (blen - MAX_MSG_SIZE); i++) {
		if (!pulse(block, i))
			continue;

		type = TYPE_ZK1;
		last_pulse = check_timing(timing_zk1, block, i);
		if (last_pulse)
			goto read_msg_data;

		type = TYPE_ZK2;
		last_pulse = check_timing(timing_zk2, block, i);
		if (last_pulse)
			goto read_msg_data;

		type = TYPE_ZK3;
		last_pulse = check_timing(timing_zk3, block, i);
		if (last_pulse)
			goto read_msg_data;

		continue;

read_msg_data:
		data = read_data(block + last_pulse);
		if (data != -1) {
			if (mlen >= max_mlen)
				return mlen;
			msg[mlen] = data + (type << 24);
#ifdef TEST
			msg[mlen] += (uint64_t)i << 32;
#endif

			mlen++;

			i = last_pulse + 640;
		}

	}
	return mlen;
}

static int data2dec(uint32_t data)
{
	int i = 1, res = 0;

	while (data) {
		res += (data & 0xf) * i;
		data >>= 4;
		i *= 10;
	}

	return res;
}

void print_message(FILE *f,
#ifdef TEST
					uint64_t tmessage,
					unsigned long block_n
#else
					uint32_t message
#endif
)
{

	int type, data;
	int fuel, altitude_type, altitude;
	int speed, angle;

#ifdef TEST
	uint32_t message;
	message = tmessage & 0xffffffff;
	tmessage >>= 32;
	tmessage += (uint64_t)block_n * (BLOCK_SIZE >> 1);
	fprintf(f, "offset = %lu\n", tmessage);
#endif

	fprintf(f, "*%08x;\n", message);

	if (options.raw)
		return;

	type = (message >> 24);
	data = message & 0xfffff;

	switch (type) {
		case TYPE_ZK1:
			fprintf(f, "  Code: %05x\n", data);
			break;

		case TYPE_ZK2:
			fuel = (data >> 16) & 0xf;
			if (fuel <= 10)
				fuel *= 5;
			else
				fuel = (fuel - 10) * 10 + 50;

			altitude_type = (data >> 14) & 1; // 1 - abs, 0 - rel

			altitude = data2dec(data & 0x3fff) * 10; // TODO negative altitude

			fprintf(f, "  Altitude: %d m (%s)\n", altitude, (altitude_type) ? "absolute" : "relative");
			fprintf(f, "  Fuel: %d%%\n", fuel);

			break;

		case TYPE_ZK3:
			speed = data2dec(data & 0x3ff) * 10;
			angle = data2dec((data >> 10) & 0x3ff);
			fprintf(f, "  Speed: %d km/h\n", speed);
			fprintf(f, "  Angle: %d°\n", angle);
			break;
	}
	fprintf(f, "\n");
}
