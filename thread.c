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

#include "thread.h"
#include "options.h"
#include "routines.h"
#include <pthread.h>
#include <stdlib.h>


static pthread_t thread;
static pthread_mutex_t end_mutex;
static pthread_barrier_t barrier;

static block_t blocks[2];
static int end_flag;


int init_thread(void *(*func)(void *))
{
	end_flag = 0;

	blocks[0].data_length = 0;
	blocks[1].data_length = 0;

	blocks[0].data = malloc(sizeof(uint16_t) * BLOCK_SIZE);
	blocks[1].data = malloc(sizeof(uint16_t) * BLOCK_SIZE);

	if (!(blocks[0].data && blocks[1].data))
		fatal("Malloc error.");

	if (pthread_mutex_init(&end_mutex, NULL) != 0)
		fatal("Pthread mutex error");

	if (pthread_barrier_init(&barrier, NULL, 2) != 0)
		fatal("Pthread barrier error");

	if (pthread_create(&thread, NULL, func, NULL) != 0)
		fatal("Pthread creating error\n");

	return 0;
}

void close_thread()
{
	pthread_barrier_destroy(&barrier);
	pthread_mutex_destroy(&end_mutex);

	free(blocks[0].data);
	free(blocks[1].data);
}


block_t *next_block(int *i)
{
	int end = 0;

	pthread_barrier_wait(&barrier);

	pthread_mutex_lock(&end_mutex);
	if (end_flag)
		end = 1;
	pthread_mutex_unlock(&end_mutex);

	if (end)
		return NULL;

	*i = (*i) ? 0 : 1;

	return &blocks[*i];
}

void set_end()
{
	pthread_mutex_lock(&end_mutex);
	end_flag = 1;
	pthread_mutex_unlock(&end_mutex);
}


