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

static const char *opts = "d:f:c:g:i:rl:DBh?";

static const struct option long_opts[] = {
	{"device-index",	required_argument,	NULL,	'd'},
	{"freq",			required_argument,	NULL,	'f'},
	{"freq-correction",	required_argument,	NULL,	'c'},
	{"gain",			required_argument,	NULL,	'g'},
	{"ifile",			required_argument,	NULL,	'i'},
	{"raw",				no_argument,		NULL,	'r'},
	{"log-level",		required_argument,	NULL,	'l'},
#ifdef TEST
	{"dump",			no_argument,		NULL,	'D'},
	{"blocks-statistic",no_argument,		NULL,	'B'},
#endif
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
#ifdef TEST
	options.dump = 0;
	options.bstat = 0;
#endif

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
#ifdef TEST
			case 'D':
				options.dump = 1;
				break;
			case 'B':
				options.bstat = 1;
				break;
#endif
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
			"\t-B, --blocks-statistic       Show blocks statistic.\n"
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


/*static struct argp_option argp_options[] = {
	{"device-index",	'd', "<index>",		0, "Select RTLSDR device" },
	{"freq",			'f', "<hz>",		0, "Set frequency (default: 740 MHz);\nYou can use 'M' and 'K' as reduction for million and thousand respectively" },
	{"freq-correction",	'c', "<ppm>",		0, "Set frequency correction (default: 0)" },
	{"gain",			'g', "<db>",		0, "Set gain (default: max gain, use 'A' for auto-gain)"},
	{"ifile",			'i', "<filename>",	0, "Read data from file"},
	{"raw",				'r', 0,				0, "Show only raw messages"},
	{"log-level",		'l', "<0-4>",		0, "Minimum log level: 0 - DEBUG, 1 - INFO, 2 - WARNING, 3 - FATAL"},
#ifdef TEST
	{"dump",			'D', 0,				0, "Dump raw blocks data to file."},
	{"blocks-statistic",'B', 0,				0, "Show blocks statistic."},
#endif
	{ 0 }
};


options_t options;


static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
	options_t *options = state.input;
	int len, mult = 1;

	switch (key) {
		case 'd':
			options.dev_index = atoi(arg);
			break;
		case 'f':
			len = strlen(arg);
			if (len < 1)
				fatal("Incorrect argument for freq-correction: '%s'\n", arg);
			if (arg[len-1] == 'M')
				mult = 1000000;
			else if (arg[len-1] == 'K')
				mult = 1000;
			options.freq = atoi(arg) * mult;
			break;
		case 'c':
			options.freq_correction = atoi(arg);
			break;
		case 'g':
			if (strcmp(arg, "A") == 0)
				options.gain = ARG_GAIN_AUTO;
			else
				options.gain = atof(arg) * 10;
			break;
		case 'i':
			options.ifile = arg;
			break;
		case 'r':
			options.raw = 1;
			break;
		case 'l':
			options.log_level = atoi(arg);
			break;
#ifdef TEST
		case 'D':
			options.dump = 1;
			break;
		case 'B':
			options.bstat = 1;
			break;
#endif
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

int parse_args(int argc, char **argv)
{
	char program_desc[128];

	snprintf(program_desc, sizeof(program_desc),
				"Program for receiving and decoding UVD protocol.\n"
				"Version: %d.%d.%d.\n"
				"Example: %s -d 1 -f 730M -g 42.1 -a -r",
				VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH,
				argv[0]);

	options.dev_index = 0;
	options.freq = DEFAULT_FREQUENCY;
	options.freq_correction = 0;
	options.gain = ARG_GAIN_MAX;
	options.ifile = NULL;
	options.raw = 0;
	options.log_level = DEFAULT_LOG_LEVEL;
#ifdef TEST
	options.dump = 0;
	options.bstat = 0;
#endif

	struct argp argp = {argp_options, parse_opt, "", program_desc};

	argp_parse(&argp, argc, argv, 0, 0, &options);
}*/

