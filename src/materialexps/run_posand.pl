#!/usr/bin/perl
use strict;

my $NTESTS = 3;

#my @shipnums = (8050, 10500);
#my @linenums = (2, 3, 4, 5, 6, 7, 8);
my @linenums = (7);
#my @line_num_types = ("RLE", "INT", "T2DECOMP");
#my @line_num_types = ("T2DECOMP");
my @tests = ("PosAndMC", "PosAndEM");
#my @tests = ("PosAndMC");
#my @aggregation = (0, 1);

my @shipnums = (8050, 8408, 8720, 9032, 9345, 9655, 9965, 10279, 10550);
#my @shipnums = (8050);
my @line_num_types = ("INT");
my @aggregation = (1);

if (-f "posand.out") {
	print STDERR "Error: will not overwrite existing posand.out\n";
	exit(1);
}

for (my $i = 3; $i <= $NTESTS; $i++) {
	foreach my $shipnum(@shipnums) {
		foreach my $linenum(@linenums) {
			foreach my $line_num_type(@line_num_types) {
				foreach my $agg(@aggregation) {
					foreach my $test(@tests) {
						my @args = ($shipnum, $linenum, $line_num_type, $agg);

						# PosAndMC just gets run once
						#if ($test =~ /PosAndMC/) {
						#	run_test($test, @args, $i); # 1 is test number
						#} 
		
						# For PosAnd{LM,EM} we run them twice--once with args[4]==1 and
						# once with it zero. This corresponds to the back_to_disk and smart
						# test parameters for LM/EM.
						#elsif ($test eq "PosAndEM") {
							#run_test($test, @args, 1, $i);
						#} else { # LM
						# run test twice, once pipelined, once parallel
							run_test($test, @args, 0, $i); # 0 = extra parameter, i = test number
							run_test($test, @args, 1, $i);
						#}
					}
				}
			}
		}
	}
}

system("cp posand.out posand.scriptbak");
#system("grep 'shipnum = ' posand.out");


sub run_test {
	my ($test, @args) = @_;
	my $nargs = scalar @args;
	my $argstr = join("\t", @args);
	open(CNF, ">posand.cnf") || die $!;
	print CNF "$test\t$nargs\t$argstr\n";	
	close CNF;
	system("./cstoreqptestOpt 0 posand.cnf global.conf 2>&1 | tee -a posand.out");
#	my $testout = `grep took posand.out | tail -n 1`;
#	$testout =~ /read ([0-9]+)/;
#	my $nread = $1;

#	my $right_ans = perlComputeAnswer(@args);
#	if ($nread != $right_ans) {
#		`echo "$test $argstr FAILED: wrong answer. Got $nread should be $right_ans" >> posand.out`;
#	} else {
#		`echo "$test $argstr PASSED: $nread equals $right_ans" >> posand.out`
#	}
}

# Returns number of records that should match
sub perlComputeAnswer {
	my ($shipnum, $linenum, $line_num_type, $agg) = @_;
	if (! -f "materialexps/answers/shipno$shipnum-linenum$linenum-agg$agg.txt") {
		print STDERR "Comptuing missing answer for shipnum < $shipnum, linenum < $linenum\n";
		system("materialexps/posand-query.pl $shipnum $linenum $agg ../D1.data.retshipsortnumquant > materialexps/answers/shipno$shipnum-linenum$linenum-agg$agg.txt");
	}
	my $answer = `wc -l materialexps/answers/shipno$shipnum-linenum$linenum-agg$agg.txt`;	
	$answer =~ /^([0-9]+) /;
	return $1;
}


