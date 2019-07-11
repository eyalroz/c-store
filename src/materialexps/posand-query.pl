#!/usr/bin/perl
use strict;

if (@ARGV != 4) {
	print STDERR "Usage: shipnum linenum aggregate file\n";
	exit(1);
}

my $shipnum_wanted = shift;
my $linenum_wanted = shift;
my $aggregate = shift;
my $count = 0;
my %agg;

open(DB, shift) || die $!;
my $line;
while (chomp($line = <DB>)) {
	my ($shipnum, $line, $qty, $rflag) = split(/\s+/, $line);
	if ($line < $linenum_wanted && $shipnum < $shipnum_wanted ) {
		++$count;
		$agg{$shipnum} += $line;
		if (!$aggregate) {
			print "$shipnum,$line\n";
		}
	}
	#warn "$shipnum:$line:$qty:$rflag";
}

if ($aggregate) {
	foreach my $snum(sort keys %agg) {
		print "$snum,",$agg{$snum}, "\n";
	}
}

close DB;
