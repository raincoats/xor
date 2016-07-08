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

// stolen from glibc
void *memfrob (void *s, size_t n, int x)
{
	char *p = (char *) s;

	while (n-- > 0) {
		// avoiding xor'ing null bytes, because (key ^ byte) == key.
		// TODO: make this more efficient..
		if ((*p != x) && (*p != 0)) {
			*p ^= x;
		}
		p++;
	}
	return s;
}

int main(int argc, char *argv[])
{
	//       biffo of 32   ~ 137MiB/s
	// biffo of 4096 * 8   ~ 1.6 GiB/s
	unsigned int biffo = 4096 * 8;
	char biffer[biffo];
	unsigned int cipher = 0xff;
	int size = 0;

	if (argc < 2) {
		dprintf(2, "%s: no cipher provided. using 0x%02x\n", argv[0], cipher);
	}
	else {
		int e = (cipher = (int)strtol(argv[1], NULL, 16));
		if ((! e) && (cipher != 0)) {
			err(e, "could not convert %s to hex", argv[1]);
			return 1;
		}
		if (cipher < 0x100) {
			dprintf(2, "%s: using 0x%02X\n", argv[0], cipher);
		}
		else {
			dprintf(2, "%s: cipher 0x%02X is out of range.\n", argv[0], cipher);
			dprintf(2, "%s: must be between 0x00 and 0xFF.\n", argv[0]);
			return 1;
		}
	}

	while ((size = read(0, biffer, biffo)) > 0)
	{
		memfrob(biffer, size, cipher);
		write(1, biffer, size);
	}


	return 0;
}

