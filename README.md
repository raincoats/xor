# xor

little program to xor things. reads from stdin, writes to stdout. only supports single byte xor ciphers at the moment.

## usage examples

    $ head -2 /proc/cpuinfo
    processor	: 0
    vendor_id	: GenuineIntel

    $ head -2 /proc/cpuinfo | xor 0e | xxd
    00000000: 7e7c 616d 6b7d 7d61 7c07 342e 3e04 786b  ~|amk}}a|.4.>.xk
    00000010: 606a 617c 5167 6a07 342e 496b 607b 6760  `ja|Qgj.4.Ik`{g`
    00000020: 6b47 607a 6b62 04                        kG`zkb.

    $ head -2 /proc/cpuinfo | xor 0e | xor 0x0e
    processor	: 0
    vendor_id	: GenuineIntel

there's also a `test.sh` that makes sure it works (as i'm really bad at c).

special thanks to whoever made glibc's `memfrob()` function, even though that `*p++ ^= x;` stuff ruxed me for like 2 hours.

