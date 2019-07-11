#!/usr/bin/perl
use strict;
use FileHandle;

my %file_handles;
my $line;

my %shipnums;
my %linetypes;
my %aggs;
my %strategies;

my %results;
my $outfile;

my %lowsel_ln_to_sel = (2 => 9.04544e-05, 3 => 0.000167439,  4 => 0.000230987, 5 => 0.000282049,
						6 => 0.000320024, 7 => 0.000346647, 8 => 0.000360117);
my %hisel_ln_to_sel = (2 => 0.248377, 3 => 0.461258, 4 => 0.638624, 5 => 0.780526, 6 => 0.886923,
	 					7 => 0.957854, 8 => 0.993286);
my %comptype_to_str = ("RLE" => "RLE", INT => "uncompressed", T2DECOMP => "bit-vector");
my %strat_to_str = ("PosAndEM-smart" => "Early", "PosAndLM-inmemory" => "LM-hot", "PosAndLM-reread" => "LM-cold",
					PosAndMC => "MultiColumn", "PosAndMC_t2special" => "MultiColumn-T2Special");

# Split the data into files for each graph
# Graphs are split by high/low sel, agg/no agg, col types --> 12 graphs
# This produces a total of 36 files  (12 graphs * 3 files per graph, one for each strat)

die "Need at least one outfile" unless (@ARGV > 0);

while($outfile = shift) {
	open(OUTFILE, "grep took $outfile |") || die "Could not open $outfile: $!";
	while (chomp($line = <OUTFILE>)) {
		next if ($line =~ /^\s*#/);
		$line =~ /(PosAnd[A-Za-z\-0-9_]+) test ([0-9]) shipnum < ([0-9]+) linenum \< ([0-9]+) type ([A-Z2]+) aggregate ([0-9]) took ([0-9]+) ms read ([0-9]+) selectivity = ([0-9\.e-]+)/;
		my ($strategy, $testnum, $shipnum, $linenum, $linetype, $agg, $time, $count, $sel) = ($1, $2, $3, $4, $5, $6, $7, $8, $9);
		unless ($strategy && defined $testnum && $shipnum && 
			$linenum && $linetype && defined $agg &&
			$time && $count && $sel) {
				die "Bad line $line";
		}
		print STDERR "$strategy:$testnum:$shipnum:$linenum:$linetype:$agg:$time:$count:$sel\n";
		#my $fh = get_filehandle($strategy, $shipnum, $linetype, $agg);
		#die "bad fh" unless (defined $fh);
		#print $fh "$linenum $time $sel\n";
		$shipnums{$shipnum} = 1;
		$linetypes{$linetype} = 1;
		$aggs{$agg} = 1;
		$strategies{$strategy} = 1;

		# Yikes. Store this result in one huge multi-dimensional hashtable.

		# Fixup aggregation selectivities, which are incorrect in the file
		if ($agg && $shipnum == 8050) {
			$sel = $lowsel_ln_to_sel{$linenum};
		}
		if ($agg && $shipnum == 10500) {
			$sel = $hisel_ln_to_sel{$linenum};
		}

		# Truncate selectivity a bit
		#$sel = sprintf("%.5f", $sel);
		push @{$results{$strategy}->{$agg}->{$linetype}->{$shipnum}->{$linenum}}, {time => $time, sel => $sel};
	}
	close OUTFILE;
}

foreach my $strategy(keys %results) {
	foreach my $agg(keys %{$results{$strategy}}) {
		foreach my $linetype(keys %{$results{$strategy}->{$agg}}) {
			foreach my $shipnum(keys %{$results{$strategy}->{$agg}->{$linetype}}) {
				foreach my $linenum(sort {$a <=> $b} keys %{$results{$strategy}->{$agg}->{$linetype}->{$shipnum}}) {
					# Now, loop over all the associative hashes I tossed in here before.
					my @times;
					foreach my $res (@{$results{$strategy}->{$agg}->{$linetype}->{$shipnum}->{$linenum}}) {
						push @times, $res->{"time"};
					}
					my $sel = ${$results{$strategy}->{$agg}->{$linetype}->{$shipnum}->{$linenum}}[0]->{"sel"};
					my $mean = mean(@times);
					my $stdev = std_dev(@times);
					my $fh = get_filehandle($strategy, $shipnum, $linetype, $agg);
					print $fh "$linenum $mean $sel $stdev, ", scalar @times, "\n";
				}
			}
		}
	}
}
					

foreach my $shipnum(keys %shipnums) {
	foreach my $linetype(keys %linetypes) {
		foreach my $agg(keys %aggs) {
			print STDERR "Graph ship $shipnum, $linetype, agg = $agg\n";
			graph_test($shipnum, $linetype, $agg);
		}
	}
}

sub graph_test{
	my ($shipnum, $linetype, $agg) = @_;

	my $selstr = ($shipnum == 10500 ? "High" : "Low") . " selectivity";
	my $aggstr = ($agg ? "with " : "without ") . "aggregation";
	my $compstr = "using " . $comptype_to_str{$linetype} . " encoding";
	my $title = "$selstr $aggstr $compstr";

	open (GP_OUT, ">gnp.in") || die $!;
	print GP_OUT "set terminal postscript enhanced color\n";
	print GP_OUT "set output 'graph-ship$shipnum-line$linetype-agg$agg.ps'\n";
#	print GP_OUT "set size square\n";
	print GP_OUT "set size ratio 1.2\n";
	print GP_OUT "set title '$title'\n";
#	print GP_OUT "set key outside\n";
	print GP_OUT "set xlab 'Selectivity (fraction)'\n";
	if ($shipnum == 8050 && $linetype eq "RLE") { # Catch special case
		print GP_OUT "set yrange [0:2000]\n";
	} else {
		print GP_OUT "set yrange [0:*]\n";
	}
	print GP_OUT "set ylab 'Runtime (ms)'\n";
	print GP_OUT "plot ";
	my $plotline;
	foreach my $strat(keys %strategies) {
		next if ($strat eq "PosAndEM-dumb"); # Not graphing this one anymore
		next if ($strat eq "PosAndMC_t2special" && ($linetype ne "T2DECOMP" || $shipnum != 8050));
		warn "'$strat' '$linetype' '$shipnum'";
		my $fname = make_fname($strat, $shipnum, $linetype, $agg);
		my $stratname = $strat_to_str{$strat};
		die "$strat" unless ($stratname);
		my $xcol = 3;
		$plotline .= "'$fname' using $xcol:2:4 title '$stratname' with yerrorlines, "
	}
	chop($plotline); # discard last ,
	chop($plotline); # discard last ,
	print GP_OUT $plotline, "\n";
	close GP_OUT;
	system("gnuplot < gnp.in");
	system("ps2pdf -DEPSCrop graph-ship$shipnum-line$linetype-agg$agg.ps");
}

sub make_fname {
	my ($strategy, $shipnum, $linetype, $agg) = @_;
	my $fname = "graphin-shipno$shipnum-ltype$linetype-agg$agg-$strategy";
}

sub get_filehandle {
	my ($strategy, $shipnum, $linetype, $agg) = @_;
	die unless ($strategy, $shipnum && $linetype && defined $agg);
	my $fname = make_fname($strategy, $shipnum, $linetype, $agg);
	if ($file_handles{$fname}) {
		return $file_handles{$fname};
	}
	# Otherwise, open file
	my $fh = new FileHandle $fname, "w";
	print $fh "#File for strategy = $strategy shipnum = $shipnum linetype = $linetype agg = $agg\n";
	print $fh "#linenum mean_exec_time_ms selectivity stdev nresults\n";
	$file_handles{$fname} = $fh;
	return $fh;
}

sub mean {
  my $result;
  foreach (@_) { $result += $_ }
  return $result / @_;
}
sub std_dev {
  my $mean = mean(@_);
  my @elem_squared;
  foreach (@_) {
    push (@elem_squared, ($_ **2));
  }
  return sqrt( mean(@elem_squared) - ($mean ** 2));
}

