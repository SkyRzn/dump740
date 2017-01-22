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

#ifndef _DUMP_740_DECODER_H_
#define _DUMP_740_DECODER_H_


#include <stdint.h>


#define TYPE_ZK1	1
#define TYPE_ZK2	2
#define TYPE_ZK3	3


void init_decoder();
void close_decoder();
int decode(uint16_t *block, int blen, uint32_t *msg, int max_mlen);
void print_message(uint32_t message);


#endif

