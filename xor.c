#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <inttypes.h>
#include <stdlib.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "%s: need an filename\n", argv[0]);
		return 1;
	}
	else if (argc < 3) {
		fprintf(stderr, "%s: need a byte (like ff, or 0a, or whatev)\n", argv[0]);
		return 1;
	}


	//int cipher = 0xff;
	int cipher = strtol(argv[2], NULL, 16);
	//int byte = *argv[1];
	char *filename = argv[1];
	char *outfile;
	FILE *fp = fopen(filename, "rb");

	if ((cipher < 0x00) || (cipher > 0xff)) {
		fprintf(stderr, "%s: “%#2x”: must be between 0x00 and 0xff\n", 
			argv[0], cipher);
		return 1;
	}

	if (! fp) {
		fprintf(stderr, "%s: could not open file “%s”\n", argv[0], filename);
		return 1;
	}

 	fprintf(stderr, "%s: cipher is 0x%02x\n", argv[0], cipher);

	int c;
    while ((c = fgetc(fp)) != EOF) { // standard C I/O file reading loop
       putchar(c ^ cipher);
    }

    fclose(fp);

//	printf("%.2x", cipher ^ byte);
//	putchar(0x0a);
	return 0;
}

