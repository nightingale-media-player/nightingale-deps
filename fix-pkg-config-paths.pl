#!/usr/bin/env perl

use strict;

use Getopt::Long;
use File::Basename;
use File::Copy qw(move);
use File::Spec::Functions;
use Cwd qw(getcwd realpath);

my $MUNGED_MARKER = '# PC FILE MUNGED';

sub Usage {
   print STDERR <<__END_USAGE__;
Usage: $0 [-f] -p [TOKEN] [full path to libtool .pc file]

If the supplied .pc file has a previouly munged marker in it, it will not be
modified unless -f is supplied.

Example: $0 -p windows-i686-msvc8 /e/builds/sb-deps/flac/release/lib/libFLAC.pc
__END_USAGE__
}

sub main {
   my $pcFile;
   my $forceMunge = 0;
   my $pivotToken = '';

   my $optsRv = GetOptions('force|f' => \$forceMunge,
                           'pivot|p=s' => \$pivotToken);

   if (!$optsRv) {
      Usage();
      return 1;
   }

   my $pcFile = $ARGV[0];
   
   if (!$forceMunge && $pcFile !~ /\.pc$/) {
      Usage();
      return 1;
   }
   elsif (!(-f $pcFile)) {
      Usage();
      return 1;
   }

   my $pcFileFullPath;
   my $pcFileInstallDir;

   if ($pcFile =~ /^\//) {
      $pcFileFullPath = $pcFile;
      $pcFileInstallDir = dirname($pcFileFullPath);
   }
   else {
      print STDERR "Need full path to .pc file\n";
      Usage();
      return 1;
   }

   my $rv = open(PC_FILE, "<$pcFile");
   if (!$rv) {
      print STDERR "Couldn't open .pc file '$pcFile': $!\n";
      Usage();
      return 1;
   }

   my @currentPcFileContents = ();
   my $mungeThisFile = 1;

   while (<PC_FILE>) {
      my $line = $_;

      if ($line =~ /^$MUNGED_MARKER/ && !$forceMunge) {
         $mungeThisFile = 0;
         last;
      }

      push(@currentPcFileContents, $line);
   }

   close(PC_FILE);

   return 0 if (!$mungeThisFile);

   print (($forceMunge ? 'Force-fixing ' : 'Fixing' ) . " $pcFile...\n");

   my $newPcFile = $pcFile . '.new';
   my $newPcFileFullPath = catfile($pcFileInstallDir, basename($newPcFile));

   my ($newRoot, undef) = split(/$pivotToken/, $pcFile);

   $rv = open(NEW_PC_FILE, ">$newPcFileFullPath");

   if (!$rv) {
      print STDERR "Couldn't open temporary .pc file '$newPcFile': $!\n";
      return 1;
   }

   print NEW_PC_FILE $MUNGED_MARKER . " by $0 on " . scalar(localtime()) . "\n";

   foreach my $line (@currentPcFileContents) {
      if ($line =~ /^prefix=(.+)/) {
         my (undef, $extension) = split(/$pivotToken/, $1);
         my $newPrefix = realpath(catfile($newRoot, $pivotToken, $extension));
         $line = "prefix=$newPrefix\n";
      }

      print NEW_PC_FILE $line;
   }

   close(NEW_PC_FILE);
   return move($newPcFileFullPath, $pcFile);
}

main();
