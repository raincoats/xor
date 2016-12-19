# xor

little program to xor things, or perform a few other bitwise operations (and, or, shifts). reads from stdin, writes to stdout.

## usage
    cat /etc/passwd | ./xor -s $'\x01\x02\xDE\xAD'

there's also a `test.php` that makes sure it works (as i'm really bad at c).

special thanks to whoever made glibc's `memfrob()` function, even though that `*p++ ^= x;` stuff ruxed me for like 2 hours.

