#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <inttypes.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

// stolen from glibc
void *memfrob (void *s, size_t n, int x)
{
	char *p = (char *) s;

	while (n-- > 0)
		*p++ ^= x;

	return s;
}


int main(int argc, char *argv[])
{
	//       biffo of 32   ~ 137MiB/s
	// biffo of 4096 * 8   ~ 1.6 GiB/s
	unsigned int biffo = 4096 * 8;
	char biffer[biffo];
	int size = 0;

	while ((size = read(0, biffer, biffo)) > 0)
	{
		memfrob(biffer, size, 0xF1);
		write(1, biffer, size);
	}


	return 0;
}

