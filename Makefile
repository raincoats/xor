all:
	gcc -Wall -fstack-protector-strong -O2 -march=native xor.c -o xor

install:
	cp xor /usr/local/bin/xor

uninstall:
	[ -f 'xor' ] && rm -f /usr/local/bin/xor

clean:
	[ -f 'xor' ] && rm -f xor

