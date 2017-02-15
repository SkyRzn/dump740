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

#include "routines.h"
#include "options.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <stdlib.h> //FIXME найти в других файлах и выпилить


static const char *log_strings[] = {"DEBUG", "INFO", "WARNING", "ALERT", "FATAL"};
static time_t t0 = 0;


void print(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
}

void log_func(unsigned char level, const char *file, const char *func, const char *fmt, ...)
{
	char buf[1024];
	va_list args;

	if (t0 == 0)
		t0 = time(NULL);

	if (level < options.log_level)
		return;

	if (level > 4)
		level = 4;

	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);

	fprintf(stderr, "%lu: %s:%s() [%s] %s\n", time(NULL) - t0, file, func, log_strings[level], buf);

	if (level == LOG_FATAL)
		exit(-1);
}

