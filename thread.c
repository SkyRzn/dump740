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
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


static pthread_t thread;
static pthread_mutex_t end_mutex;
static pthread_barrier_t barrier;

static block_t blocks[2];
static int end_flag;


int init_thread(void *(*func)(void *))
{
	int res;

	end_flag = 0;

	blocks[0].data_length = 0;
	blocks[1].data_length = 0;

	blocks[0].data = malloc(sizeof(uint16_t) * BLOCK_SIZE);
	blocks[1].data = malloc(sizeof(uint16_t) * BLOCK_SIZE);

	if (!(blocks[0].data && blocks[1].data)) {
		fprintf(stderr, "Malloc error.\n");
		exit(-ENOMEM);
	}

	res = pthread_mutex_init(&end_mutex, NULL);
	if (res != 0) {
		fprintf(stderr, "Pthread mutex error\n");
		exit(res);
	}

	res = pthread_barrier_init(&barrier, NULL, 2);
	if (res != 0) {
		fprintf(stderr, "Pthread barrier error\n");
		exit(res);
	}

	res = pthread_create(&thread, NULL, func, NULL);
	if (res != 0) {
		fprintf(stderr, "Pthread creating error\n");
		exit(res);
	}
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


