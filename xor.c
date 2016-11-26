#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

unsigned char *cipher;
long cipher_len = 1;
bool cipher_given = false;
bool xor_nulls = false;


void usage(char *argv0)
{
	dprintf(2,
		"xor - performs an xor on stdin and a byte given with -c\n"
		"usage: %s [OPTIONS] -c byte\n"
		// "   -c byte   the byte (in hex) to xor stdin with\n"
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
	dprintf(2, "xor - version 0.2.1\n"
		       "https://github.com/raincoats/\n");
	exit(2);
}

/*
 *   this is where the magic happens
 *   adapted from glibc's memfrob()
 */
void *xor (void *s, size_t n, void *x, size_t y)
{
	unsigned char *p = (unsigned char *)s;
	unsigned char *o = (unsigned char *)x;
	size_t c = y;

		while (n --> 0)
		{
			dprintf(2, "\033[38;5;225mc=%ld o=%02x y=%02x\033[m\n", c, (unsigned int)*o, (unsigned int)y);
			*p++ ^= *o++;
			
			if (--c == 0)
			{
				c = y;
				o -= c;
			}
		}

	return s;
}

/*
             DISASTER BELOW

bool arg_to_byte(char *arg)
{
	assert(arg != NULL);

	uintmax_t c = (uintmax_t)strtol(arg, NULL, 16);

	if (errno == EINVAL) {
		err(errno, "could not convert '%s' to hex", arg);
		exit(1);
	}
	if (c > UINT8_MAX) {
		dprintf(2, "'0x%s': is too big. must be between 0x00 and 0xFF\n", arg);
		exit(1);
	}

	// memset(cipher, (uint8_t)strtol(arg, NULL, 16), 1);
	char *e = (char *)strtol(arg, NULL, 16);
	// unsigned char e = (unsigned char)strtol(arg, NULL, 16);
	// uint8_t *p = &e;
	// cipher = (unsigned char *)p;
	// cipher = &e;
	strncpy((char *)cipher, e, 1);
	// *cipher = (unsigned char *)e;
	// strncpy(cipher, (char)&c, 1);
	cipher_len = 1;
	dprintf(2, "\033[38;5;215mCIPHER: %s (0x%x), len=%ld\033[m\n",
		cipher, (uint8_t)*cipher, cipher_len);
	cipher_given = true;
	return true;
}
*/

bool arg_to_cipher(char *arg)
{
	assert(arg != NULL);

	cipher = (unsigned char *)arg;
	cipher_len = strlen(arg);
	// dprintf(2, "\033[38;5;196mCIPHER: %s (0x%x)\033[m, len=%ld\n", cipher, (int)*cipher, cipher_len);
	cipher_given = true;
	return true;
}


// taking this out of the main loop is okay, right?
void do_getopts(int argc, char **argv)
{
	int o;

	while ((o = getopt(argc, argv, "hV0c:s:")) != -1) {
		switch(o) {
			case 'h': usage(argv[0]);        break;
			case 'V': version();             break;
			case '0': xor_nulls = true;      break;
			// case 'c': arg_to_byte(optarg);   break;
			case 's': arg_to_cipher(optarg); break;
		}
	}
}

int main(int argc, char *argv[])
{
	int bif_size = 8192;
	char biffer[bif_size];
	int size = 0;
	
	do_getopts(argc, argv);

	if (cipher_given == false) {
		cipher[0] = '\xff';
		dprintf(2, "%s: no cipher provided. using 0x%02x\n", argv[0], cipher[0]);
	}

	while ((size = read(0, biffer, bif_size)) > 0)
	{
		dprintf(2, "\033[38;5;2mCIPHER: %s (0x%x), len=%ld\033[m\n", cipher, (uint8_t)*cipher, cipher_len);
		xor(biffer, size, cipher, cipher_len);
		if (write(1, biffer, size) == -1) {
			perror("writing to stdout");
			return 1;
		}
	}

	if (isatty(1))
		putchar('\n');

	return 0;
}

