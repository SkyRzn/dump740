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

#ifndef _DUMP_740_ROUTINES_H_
#define _DUMP_740_ROUTINES_H_


#include <string.h>


#define LOG_DEBUG	0
#define LOG_INFO	1
#define LOG_WARNING	2
#define LOG_ALERT	3
#define LOG_FATAL	4



#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)


void print(const char *fmt, ...);
void log_func(unsigned char level, const char *file, const char *func, const char *fmt, ...);
#define debug(...)		do {log_func(LOG_DEBUG, __FILENAME__, __FUNCTION__, __VA_ARGS__);}  while(0)
#define info(...)		do {log_func(LOG_INFO, __FILENAME__, __FUNCTION__, __VA_ARGS__);}  while(0)
#define warning(...)	do {log_func(LOG_WARNING, __FILENAME__, __FUNCTION__, __VA_ARGS__);}  while(0)
#define alert(...)		do {log_func(LOG_ALERT, __FILENAME__, __FUNCTION__, __VA_ARGS__);}  while(0)
#define fatal(...)		do {log_func(LOG_FATAL, __FILENAME__, __FUNCTION__, __VA_ARGS__);}  while(0)


#endif

