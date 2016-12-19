#include "xor.h"

void usage(void)
{
	printf(
		"xor - performs an xor with stdin, and a string given with -s.\n"
		"usage: %s [OPTIONS] -s string\n"
		"   -s        string to xor stdin with\n"
		"   -A        perform a bitwise AND instead of an XOR\n"
		"   -O        perform a bitwise OR instead of an XOR\n"
		"   -L        perform a shift left  (not fully implemented yet!)\n"
		"   -R        perform a shift right (not fully implemented yet!)\n"
		"   -V        version\n"
		"   -h        help\n"
		"\n"
		"example: cat /etc/passwd | %s -s $'\\x01\\x02\\xDE\\xAD'\n"
		"         cat /etc/passwd | %s -s 'hello, world'\n"
		"         cat /etc/passwd | %s -s $'\\xff'\n"
		, argv0, argv0, argv0, argv0);
	exit(0);
}

void version(void)
{
	printf("xor v%s <https://github.com/raincoats/xor>\n", XOR_VERSION);
	exit(0);
}

/* all of the operations: xor, and, or, shift left, shift right */

inline char do_xor(char p, char o) { return p ^= o; }
inline char do_and(char p, char o) { return p &= o; }
inline char do_or (char p, char o) { return p |= o; }
inline char do_shl(char p, char o) { return (char)do_shift(p, o, true);  }
inline char do_shr(char p, char o) { return (char)do_shift(p, o, false); }
inline uint8_t do_shift(char p, char o, bool left_shift)
{
	return left_shift? (uint8_t)(p << o) : (uint8_t)(p >> o);
}


/*
 *   this is where the magic happens
 *   adapted from glibc's memfrob()
 */
void *xor (void *s, ssize_t n, void *x, ssize_t y)
{
	char *p = (char *)s;
	char *o = (char *)x;
	char c = (char)y;

		while (n --> 0)
		{
			/*
			 *   i'd love to do just *p = op(*p++, *o++) but apparently
			 *   that would be ~ undefined ~
			 */
			char t = operation(*p++, *o++);

			/* since we p++'d, we have to go back one character,
			   to assign the xor result */
			*(p-1) = t;
			
			/* this little bit makes the xor string passed on the cmd line
			   wrap around. */
			if (--c == 0)
			{
			   /* TODO: could probably use a modulo here instead */
				c = (char)y;
				o -= c;
			}
		}

	return s;
}

bool arg_to_cipher(char *arg)
{
	assert(arg != NULL);

	cipher_str = (char *)arg;
	cipher_len = (ssize_t)strlen(arg);
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

	while ((o = getopt(argc, argv, "hVs:c:XAOLR")) != -1) {
		switch(o) {
			case 'h': usage();               break;
			case 'V': version();             break;
			case 's': arg_to_cipher(optarg); break;
			case 'c': arg_to_byte(optarg);   break;
			case 'X': operation = &do_xor;   break;
			case 'A': operation = &do_and;   break;
			case 'O': operation = &do_or;    break;
			case 'L': operation = &do_shl;   break;
			case 'R': operation = &do_shr;   break;
		}
	}
}

int main(int argc, char **argv)
{
	argv0 = (argv0 == NULL)? "xor" : argv[0];
	size_t bif_size = 8192;
	char biffer[bif_size];
	ssize_t size = 0;
	
	do_getopts(argc, argv);

	if (cipher_given == false)
		usage();

	void *cipher;

	if (cipher_str != NULL) {
		cipher = cipher_str;
		cipher_len = (ssize_t)strlen((char *)cipher_str);
	}
	else {
		cipher = &cipher_byte;
		cipher_len = 1;
	}

	while (2+2 == 4)
	{
		size = read(0, biffer, bif_size);
		if (size == 0) break;
		if (size == -1) {
			perror("reading from stdin");
			return 1;
		}

		xor(biffer, size, cipher, cipher_len);
		if (write(1, biffer, (size_t)size) == -1) {
			perror("writing to stdout");
			return 1;
		}
	}

	if (isatty(1))
		putchar('\n');

	return 0;
}

