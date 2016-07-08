#!/bin/bash
set -e

function good {
	printf -- "\033[38;5;034m["'$'"]\033[m %s\n" "$*"
}
function pain {
	printf -- "\033[38;5;196m["'!'"]\033[m %s\n" "$*"
}

function random_byte {
	if hash xxd; then
		xxd -p -len 1 /dev/urandom
	else
		seq 0 255 | xargs printf -- "%02x\n" | shuf -n1
	fi
}
function random_file {
	echo $testfiles | tr ' ' '\n' | shuf -n1
}

function cleanup {
	[ ! -z "$tmp1" ] && rm -f "${tmp1:?}"
	[ ! -z "$tmp2" ] && rm -f "${tmp2:?}"
}
trap 'cleanup' SIGINT
trap 'cleanup' SIGTRAP
trap 'cleanup' SIGQUIT
trap 'cleanup' SIGHUP
trap 'cleanup' SIGTERM



########## test start ##########

make clean
make

tmp1=$(mktemp /tmp/xor-test-XXXXXXXX)
tmp2=$(mktemp /tmp/xor-test-XXXXXXXX)
tests=${1:-255}
testfiles=(
	/etc/passwd
	/bin/bash
	/etc/group
	/usr/include/sys/syscall.h
	/proc/cpuinfo
	$0
	./xor
	/bin/cat
	/usr/bin/env
	/usr/sbin/sshd
	/sbin/mount
	/sbin/ping
)
hashers=(
	md5sum
	sha1sum
	sha256sum
	sha512sum
)
for prog in $hashers; do
	hash $prog && hasher=$prog
done

good "doing ${tests:?} tests"
good "hasher is $hasher"

for n in {0..$tests}
do
	b=$(random_byte)
	f=$(random_file)
	
	[ ! -f "$f" ] && f=$0

	./xor $b 2>/dev/null < $f    > $tmp1
	./xor $b 2>/dev/null < $tmp1 > $tmp2

	hash1=$($hasher < $f    | cut -d' ' -f1)
	hash2=$($hasher < $tmp2 | cut -d' ' -f1)

	if [ "$hash1" = "$hash2" ]; then
		good "[$n/$tests] PASS:     0x$b $f"

		if [ ! -z $VERBOSE ]; then
			good "$b < $f > $tmp1"
			good "$b < $tmp1 > $tmp2"
			good "[$n/$tests] EXPECTED: $hash1"
			good "[$n/$tests] GOT:      $hash2"
			printf "\n"
		fi

	else
		pain "FAIL:     0x$b $f"
		pain "EXPECTED: $hash1"
		pain "GOT:      $hash2"
		exit 1
	fi
	# bsd sleep doesnt support 0.1s
	sleep 0.1s || sleep 1
	:>$tmp1
	:>$tmp2
done

