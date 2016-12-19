#define _XOR_H 1
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
#include <libgen.h>

#define XOR_VERSION "0.3"

char do_xor(char, char);
char do_and(char, char);
char do_or (char, char);
char do_shl(char, char);
char do_shr(char, char);
uint8_t do_shift(char, char, bool);

char *argv0;
ssize_t cipher_len = 1;
uint8_t cipher_byte;
char *cipher_str = NULL;
bool cipher_given = false;
char (*operation)(char, char) = &do_xor;
