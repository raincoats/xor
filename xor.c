#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdint.h>

uint8_t cipher = 0xff;
bool cipher_given = false;
bool xor_nulls = false;


void usage(char *argv0)
{
	dprintf(2,
		"xor - performs an xor on stdin and a byte given with -c\n"
		"usage: %s [OPTIONS] -c byte\n"
		"   -c byte   the byte (in hex) to xor stdin with\n"
		"   -0        ignore null bytes in input\n"
		"   -V        version\n"
		"   -h        help\n"
		"\n"
		"example: cat /etc/passwd | %s -c ff\n"
		, argv0, argv0);
	exit(2);
}

void version()
{
	dprintf(2, "xor - version 0.2\n"
		       "https://github.com/raincoats/\n");
	exit(2);
}

/*
 *   this is where the magic happens
 *   adapted from glibc's memfrob()
 */
void *xor (void *s, size_t n, uint8_t x)
{
	unsigned char *p = (unsigned char *)s;

	/*
	 *   since anything xor'd by 0 is itself, you may want to ignore null bytes
	 *   when xor'ing, as it would just print the xor key. so you can pass the
	 *   -0 flag to do this.
	 */
	if (xor_nulls == true)
	{
		while (n --> 0) {
			if (! ((*p == x) || (*p == 0))) {
				*p ^= x;
			}
			p++;
		}
	}
	else
	{
		while (n --> 0)
			*p++ ^= x;
	}

	return s;
}

uint8_t arg_to_cipher(char *arg)
{
	if (arg == NULL)
		return false;

	uintmax_t c = (uintmax_t)strtol(optarg, NULL, 16);;

	if (errno == EINVAL) {
		err(errno, "could not convert '%s' to hex", arg);
		exit(1);
	}
	if (c > UINT8_MAX) {
		dprintf(2, "'0x%s': is too big. must be between 0x00 and 0xFF\n", arg);
		exit(1);
	}

	cipher = (uint8_t)c;
	dprintf(2, "using cipher '0x%02x'\n", cipher);
	cipher_given = true;
	return true;
}


// taking this out of the main loop is okay, right?
void do_getopts(int argc, char **argv)
{
	int o;

	while ((o = getopt(argc, argv, "hV0c:")) != -1) {
		switch(o) {
			case 'h': usage(argv[0]);        break;
			case 'V': version();             break;
			case '0': xor_nulls = true;      break;
			case 'c': arg_to_cipher(optarg); break;
		}
	}
}

int main(int argc, char *argv[])
{
	int bif_size = 8192;
	char biffer[bif_size];
	int size = 0;
	
	do_getopts(argc, argv);

	if (! cipher_given)
		dprintf(2, "%s: no cipher provided. using 0x%02x\n", argv[0], cipher);

	while ((size = read(0, biffer, bif_size)) > 0)
	{
		xor(biffer, size, cipher);
		if (write(1, biffer, size) == -1) {
			perror("writing to stdout");
			return 1;
		}
	}

	if (isatty(1))
		putchar('\n');

	return 0;
}

