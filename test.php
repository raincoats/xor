#!/usr/bin/env php
<?php

// define('TEST_STRING', 'hello, world. 123, 456. lots of words here');
define('TEST_STRING', 'hello, world');

	// 0: xor
	// 1: and
	// 2: or
	// 3: shl
	// 4: shr

function op_to_str(int $operation): string
{
	switch ($operation) {
		case 0:  return 'xor';
		case 1:  return 'and';
		case 2:  return 'or';
		default: die("wtf");
	}
}

function shell_escape(string $s): string
{
	$r = '';
	for ($i=0; $i<strlen($s); $i++) {
		$c = substr($s, $i, 1);
		$r .= sprintf("\\x%02x", ord($c));
	}

	return sprintf("\$'%s'", $r);
}


function make_cmd(string $a, string $flag, string $b): string
{
	$b = shell_escape($b);
	return sprintf("printf -- \"%%s\" '%s' | ./xor %s -s %s", $a, $flag, $b);
}


function make_test(string $a, string $b, int $operation): array
{
	// make $b big enough for $a, otherwise the xor/whatever will be short.
	while (strlen($b) < strlen($a))
		$b .= $b;
	if (strlen($b) > strlen($a))
		$b = substr($b, 0, strlen($a));

	switch ($operation) {
		case 0:  $result = $a ^ $b; $flag='-X'; break;
		case 1:  $result = $a & $b; $flag='-A'; break;
		case 2:  $result = $a | $b; $flag='-O'; break;
		default: die("fuck");
	}

	$cmd = make_cmd($a, $flag, $b);

	return array(
		'str' => $a,
		'cipher' => $b,
		'result' => $result,
		'sha1' => sha1($result),
		'cmd' => $cmd,
	);
}


function random_cipher(): string
{
	// php doesn't handle nulls well...
	$f = file_get_contents('/dev/urandom', false, NULL, 0, random_int(1, 8));
	return strtr($f, array("\0" => chr(random_int(1, 255))));
}


function run_test(int $times=10, string $cipher=NULL, bool $all=true)
{
	for ($i=0; $i<$times; $i++)
	{
		if ($all) {
			$operation = random_int(0, 2);
		}
		else {
			$operation = 0;
		}
		if ($cipher === NULL)
			$cipher = random_cipher();
		
		$a = make_test(TEST_STRING, random_cipher(), $operation);

		$r = shell_exec($a['cmd']);
		if ($a['sha1'] === sha1($r))
			printf("\033[2k\r[%6d] op=%s ", $i, op_to_str($operation));
			// printf("\033[38;5;076m[+]\033[m OK!\t\t%s\n", $a['cmd']);
		else {
			printf("\033[38;5;196m[+]\033[m NO!\t\t%s\n", $a['cmd']);
			printf("%-10s: %s\n", 'expected', shell_escape($a['result']));
			printf("%-10s: %s\n\n", 'got',    shell_escape($r));
			die;
		}
	}
}


// run_test(256, NULL, false);
run_test(65535, NULL, true);
die("\n");
$a=make_test('hello, world. 123, 456. lots of words here', "\x17\x00\x89\xa6\x85\xf2", 0);
var_dump($a);
die(shell_exec($a['cmd']));

// var_dump($a);

// echo $a['cmd']."\n";
// echo $a['sha1']."\n";


// pcntl_exec("/bin/sh", array('-c', $a['cmd']));


