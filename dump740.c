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


static int work()
{
	block_t *block;
	int block_index = 0;
	int cnt, i;
	uint32_t msg[MAX_MESSAGES_IN_BLOCK];

	while (block = next_block(&block_index)) {
		cnt = decode(block->data, block->data_length, msg, BLOCK_SIZE);

		for (i = 0; i < cnt; i++)
			print_package(msg[i]);
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

