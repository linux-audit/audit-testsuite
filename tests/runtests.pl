#!/usr/bin/perl

use strict;

use Test::Harness;
use File::Which;

# Gather the list of tests to execute.
my @dirs = split( / /, $ENV{TESTS} );
my @scripts;
for (@dirs) {
    push @scripts, "$_/test";
}

my $output = `id`;
$output =~ /uid=\d+\((\w+)\).*/;
my $unix_user = $1;
$output =~ /context=(\w+):(\w+):(\w+):([\w:\.,-]*)/;
my $selinux_user = $1;
my $selinux_role = $2;
my $selinux_type = $3;
my $selinux_mls  = $4;

# Sanity checks prior to test execution.
die("These tests are intended to be run as root\n") unless $unix_user eq "root";
die("The auditctl tool cannot be found\n")          unless which "auditctl";

print "Running as   user    $unix_user\n";
print
"        with context $selinux_user:$selinux_role:$selinux_type:$selinux_mls\n";
print "        on   system  $ENV{DISTRO}\n\n";

# Execute tests.
runtests(@scripts);
