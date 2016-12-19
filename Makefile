CC           = gcc
CFLAGS       = -fstack-protector-strong -O2
CFLAGS_DEBUG = -fbuiltin -O0 -g3 -ggdb3
CFLAGS_PAIN  = -fstack-protector-strong -D_FORTIFY_SOURCE=1 -march=native -mtune=native -O2 -fPIE \
               -fno-omit-frame-pointer -fsanitize=address -Wformat -Werror=format-security \
               -Wconversion -Wsign-conversion -Wall -Wextra -Wstack-protector --param ssp-buffer-size=4
CPPFLAGS     = -Wdate-time -D_FORTIFY_SOURCE=2
LDFLAGS      = -pie -Wl,-Bsymbolic-functions,-z,nodlopen,nodump,noexecstack,noexecheap,relro,now

all:
	$(CC) $(CFLAGS) -o xor xor.c

debug:
	$(CC) $(CFLAGS_DEBUG) -o xor xor.c

pain:
	LDFLAGS="$(LDFLAGS)" CPPFLAGS="$(CPPFLAGS)" $(CC) $(CFLAGS_PAIN) -o xor xor.c
