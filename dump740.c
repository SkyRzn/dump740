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
#include "routines.h"
#include <stdio.h>

#ifdef TEST
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#endif


static int work()
{
	block_t *block;
	int block_index = 0;
	int cnt, i;
	uint32_t msg[MAX_MESSAGES_IN_BLOCK];

#ifdef TEST
	clock_t total_c0, block_c1, block_c2;
	time_t total_t1, total_t2, total_t2_prev = 0;
	unsigned long blocks_count = 0;
	unsigned long block_cime;
	long blocks_lost;
	int fd;

	total_t1 = time(NULL);

	if (options.dump) {
		time_t rawtime;
		struct tm *timeinfo;
		char tbuf[32];

		timeinfo = localtime(&total_t1);

		strftime(tbuf, sizeof(tbuf), "%Y-%m-%d-%H-%M-%S-dump740.raw", timeinfo);
		print("Dump file: %s\n", tbuf);
		fd = open(tbuf, O_WRONLY|O_CREAT|O_TRUNC, 0666);
		if (fd < 0)
			fatal("Dump file opening error: %s", strerror(errno));
	}

	total_c0 = clock();

#endif

	while (block = next_block(&block_index)) {
#ifdef TEST
		if (options.bstat)
			block_c1 = clock();


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
		if (options.dump)
			write(fd, block->data, block->data_length);
#pragma GCC diagnostic pop
#endif

		cnt = decode(block->data, block->data_length, msg, BLOCK_SIZE);

#ifdef TEST
		if (options.bstat) {
			block_c2 = clock();
			total_t2 = time(NULL);

			blocks_count++;

			if (total_t2 != total_t2_prev) {
				if (total_t2_prev > 0) {
					blocks_lost = (long)(((double)(total_t2 - total_t1) * SAMPLE_RATE) / BLOCK_SIZE - blocks_count);
					if (blocks_lost < 0)
						blocks_lost = 0;
					block_cime = (unsigned long)(((double)(block_c2 - block_c1))/CLOCKS_PER_SEC*1000000);

					debug("block time = %lu us; blocks handled = %lu; blocks lost = %lu",
						  block_cime, blocks_count, blocks_lost);
				}
				total_t2_prev = total_t2;
			}
		}
#endif
		for (i = 0; i < cnt; i++)
			print_message(stdout, msg[i]);
	}
#ifdef TEST
	if (options.bstat)
		info("total time = %.3f; total blocks handled = %lu; total blocks lost = %lu",
			 (double)(clock()-total_c0)/CLOCKS_PER_SEC, blocks_count, blocks_lost);

	if (options.dump)
		close(fd);
#endif
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

