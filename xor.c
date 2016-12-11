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

uintmax_t cipher_len = 1;
uint8_t cipher_byte;
unsigned char *cipher_str = NULL;
bool cipher_given = false;


void usage(char *argv0)
{
	dprintf(2,
		"xor - performs an xor on stdin and a byte given with -c\n"
		"usage: %s [OPTIONS] -s string\n"
		"   -s        string to xor stdin with\n"
		"   -s        byte to xor stdin with\n"
		"   -V        version\n"
		"   -h        help\n"
		"\n"
		"example: cat /etc/passwd | %s -s hello\n"
		"         cat /etc/passwd | %s -c 41\n"
		, argv0, argv0, argv0);
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
			*p++ ^= *o++;
			
			if (--c == 0)
			{
				c = y;
				o -= c;
			}
		}

	return s;
}

bool arg_to_cipher(char *arg)
{
	assert(arg != NULL);

	cipher_str = (unsigned char *)arg;
	cipher_len = strlen(arg);
	cipher_given = true;
	return true;
}

bool arg_to_byte(char *arg)
{
	assert(arg != NULL);

	cipher_byte = (uint8_t)strtol(arg, NULL, 16);
	cipher_given = true;
	return true;
}


// taking this out of the main loop is okay, right?
void do_getopts(int argc, char **argv)
{
	int o;

	while ((o = getopt(argc, argv, "hVs:c:")) != -1) {
		switch(o) {
			case 'h': usage(argv[0]);        break;
			case 'V': version();             break;
			case 's': arg_to_cipher(optarg); break;
			case 'c': arg_to_byte(optarg);   break;
		}
	}
}

int main(int argc, char *argv[])
{
	int bif_size = 8192;
	char biffer[bif_size];
	int size = 0;
	
	do_getopts(argc, argv);

	if (cipher_given == false)
		usage(argv[0]);

	void *cipher;

	if (cipher_str != NULL) {
		cipher = cipher_str;
		cipher_len = strlen((char *)cipher_str);
	}
	else {
		cipher = &cipher_byte;
		cipher_len = 1;
	}

	while ((size = read(0, biffer, bif_size)) > 0)
	{
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

