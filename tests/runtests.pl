#!/usr/bin/perl

use strict;

use Test::Harness;

my @dirs = split(/ /, $ENV{SUBDIRS});
my @scripts;
for (@dirs) {
	push @scripts, "$_/test";
}

my $output = `id`;
$output =~ /uid=\d+\((\w+)\).*context=(\w+):(\w+):(\w+)/ || die ("Can't determine user's id\n");
my $unix_user = $1;
my $selinux_user = $2;
my $selinux_role = $3;
my $selinux_type = $4;

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

