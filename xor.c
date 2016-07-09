#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <inttypes.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <err.h>

// stolen from glibc's memfrob()
void *xor (void *s, size_t n, unsigned int x)
{
	unsigned char *p = (unsigned char *) s;

	while (n-- > 0) {
		if (! ((*p == x) || (*p == 0))) {
			// avoiding xor'ing null bytes, because (key ^ byte) == key.
			*p ^= x;
		}
		p++;
	}
	return s;
}

int main(int argc, char *argv[])
{
	// 4096 * 8 is fast. idk why. is it too much?
	unsigned int bif_size = 4096 * 8;
	unsigned char biffer[bif_size];
	unsigned int cipher = 0xff;
	unsigned int size = 0;

	if (argc < 2) {
		dprintf(2, "%s: no cipher provided. using 0x%02x\n", argv[0], cipher);
	}
	else {
		unsigned int e = (cipher = (unsigned int)strtol(argv[1], NULL, 16));
		if ((! e) && (cipher != 0)) {
			err(e, "could not convert %s to hex", argv[1]);
			return 1;
		}
		else if (cipher > 0xFF) {
			dprintf(2, "%s: cipher 0x%02X is out of range.\n", argv[0], cipher);
			dprintf(2, "%s: must be between 0x00 and 0xFF.\n", argv[0]);
			return 1;
		}
	}

	while ((size = read(0, biffer, bif_size)) > 0)
	{
		xor(biffer, size, cipher);
		write(1, biffer, size);
	}


	return 0;
}

