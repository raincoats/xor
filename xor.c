/*
 * xor.c
 * ~~~~~
 *
 * tiny string xor'er
 * by @reptar_xl
 *
 * 7 mar 2016
 *
 * i don't know c very well, as you can tell...
 *
*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	if (argc < 3) {
		fprintf(stderr, 
			"usage: %s <word> <word>\n"
			"simple xor calculator by @reptar_xl\n"
			"note: strings must be the same length... :/\n"
			, argv[0]);
		return 4;
	}

	char *one = argv[1];
	char *two = argv[2];

	while (*one && *two) {
		putc(*one++ ^ *two++, stdout);
	}

	if (isatty(1) && isatty(2) && isatty(0))
		putchar(0x0a);

	return 0;
}