#!/usr/bin/perl

use Test::Harness;

@dirs = split(/ /, $ENV{SUBDIRS});

for (@dirs) {
	push @scripts, "$_/test";
}

$output = `id`;
$output =~ /uid=\d+\((\w+)\).*context=(\w+):(\w+):(\w+)/ || die ("Can't determine user's id\n");
$unix_user = $1;
$selinux_user = $2;
$selinux_role = $3;
$selinux_type = $4;

print "Running as user $unix_user with context $selinux_user:$selinux_role:$selinux_type\n\n";

if ($unix_user ne "root") {
	print "These tests are intended to be run as root\n";
	exit;
}

if (! -x "/usr/sbin/auditctl") {
	print "The auditctl tool cannot be found\n";
	exit;
}

runtests(@scripts);

