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

#include "options.h"
#include <getopt.h>
#include <stdlib.h>


options_t options;

static const char *opts = "d:f:c:g:i:rl:Dh?";

static const struct option long_opts[] = {
	{"device-index",	required_argument,	NULL,	'd'},
	{"freq",			required_argument,	NULL,	'f'},
	{"freq-correction",	required_argument,	NULL,	'c'},
	{"gain",			required_argument,	NULL,	'g'},
	{"ifile",			required_argument,	NULL,	'i'},
	{"raw",				no_argument,		NULL,	'r'},
	{"log-level",		required_argument,	NULL,	'l'},
	{"dump",			no_argument,		NULL,	'D'},
	{"help",			required_argument,	NULL,	'h'},
	{NULL,				no_argument,		NULL,	0}
};

void parse_args(int argc, char **argv)
{
	int opt, len, mult, ind;

	options.dev_index = 0;
	options.freq = DEFAULT_FREQUENCY;
	options.freq_correction = 0;
	options.gain = ARG_GAIN_MAX;
	options.ifile = NULL;
	options.raw = 0;
	options.log_level = DEFAULT_LOG_LEVEL;
	options.dump = 0;

	opt = getopt_long( argc, argv, opts, long_opts, &ind);

	while (opt != -1) {
		switch (opt) {
			case 'd':
				options.dev_index = atoi(optarg);
				break;
			case 'f':
				len = strlen(optarg);
				if (len < 1)
					fatal("Incorrect argument for freq-correction: '%s'\n", optarg);
				mult = 1;
				if (optarg[len-1] == 'M')
					mult = 1000000;
				else if (optarg[len-1] == 'K')
					mult = 1000;
				options.freq = atoi(optarg) * mult;
				break;
			case 'c':
				options.freq_correction = atoi(optarg);
				break;
			case 'g':
				if (strcmp(optarg, "A") == 0)
					options.gain = ARG_GAIN_AUTO;
				else
					options.gain = atof(optarg) * 10;
				break;
			case 'i':
				options.ifile = optarg;
				break;
			case 'r':
				options.raw = 1;
				break;
			case 'l':
				options.log_level = atoi(optarg);
				break;
			case 'D':
				options.dump = 1;
				break;
			case 'h':
			case '?':
				print_usage();
				exit(0);
			default:
				break;
		}
		opt = getopt_long( argc, argv, opts, long_opts, &ind);
	}
}

void print_usage()
{
	print(	"Usage: dump740 [OPTION...]\n"
			"Program for receiving and decoding UVD protocol.\n"
			"Version: %d.%d.%d.\n"
			"Example: ./dump740 -d 1 -f 730M -g 42.1 -a -r\n"
			"\n"
			"\t-c, --freq-correction=<ppm>  Set frequency correction (default: 0)\n"
			"\t-d, --device-index=<index>   Select RTLSDR device\n"
			"\t-D, --dump                   Dump raw blocks data to file.\n"
			"\t-f, --freq=<hz>              Set frequency (default: 740 MHz);\n"
			"\t                             You can use 'M' and 'K' as reduction for million\n"
			"\t                               and thousand respectively\n"
			"\t-g, --gain=<db>              Set gain (default: max gain, use 'A' for auto-gain)\n"
			"\t-i, --ifile=<filename>       Read data from file\n"
			"\t-l, --log-level=<0-4>        Minimum log level: 0 - DEBUG, 1 - INFO, 2 - WARNING, 3 - FATAL\n"
			"\t-r, --raw                    Show only raw messages\n"
			"\t-?, --help                   Give this help list\n"
			"\t--usage                      Give a short usage message\n",
		VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
}

