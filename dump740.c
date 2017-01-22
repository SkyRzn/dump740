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
#include "decoder.h"
#include "thread.h"
#include "options.h"
#include <stdio.h>

#ifdef TEST
#include <time.h>
#endif


static int work()
{
	block_t *block;
	int block_index = 0;
	int cnt, i;
	uint32_t msg[MAX_MESSAGES_IN_BLOCK];

#ifdef TEST
	clock_t block_t1, block_t2;
	time_t all_t1, all_t2, all_t2_prev;
	unsigned long blocks_count = 0;
	unsigned long blocks_lost;
	unsigned long block_time;

	all_t1 = time(NULL);
#endif

	while (block = next_block(&block_index)) {
#ifdef TEST
		block_t1 = clock();
#endif

		cnt = decode(block->data, block->data_length, msg, BLOCK_SIZE);

#ifdef TEST
		block_t2 = clock();
		all_t2 = time(NULL);

		blocks_count++;

		if (all_t2 != all_t2_prev) {
			all_t2_prev = all_t2;
			blocks_lost = (unsigned long)(((double)(all_t2 - all_t1) * SAMPLE_RATE) / BLOCK_SIZE - blocks_count);
			block_time = (unsigned long)(((double)(block_t2 - block_t1))/CLOCKS_PER_SEC*1000000);

			printf("Block handling time: %lu us; blocks handled: %lu; blocks lost: %lu\n", block_time, blocks_count, blocks_lost);
		}

#endif

		for (i = 0; i < cnt; i++)
			print_message(msg[i]);
	}
}

int main(int argc, char **argv)
{
	parse_args(argc, argv);

	init_decoder();
	init_receiver();
	init_thread(reader);

	work();

	close_thread();
	close_receiver();
	close_decoder();

	return 0;
}

