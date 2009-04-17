#!/bin/env perl

use strict;

use File::Basename;
use File::Copy qw(move);
use File::Spec::Functions;
use Cwd qw(getcwd realpath);

sub main {
   my $laFile = $ARGV[0];

   if (! -f $laFile || $laFile !~ /\.la$/) {
      print STDERR "Usage: $0 [foo.la]\n";
      print STDERR "Example: $0 flac/release/lib/libFLAC.la\n";
      return 1;
   }

   my $laFileFullPath = ($laFile =~ /^\//) ? $laFile :
    catfile(getcwd(), $laFile);

   my $laFileInstallDir = dirname(realpath($laFileFullPath));

   my $newLaFile = $laFile . '.new';
   my $newLaFileFullPath = catfile($laFileInstallDir, basename($newLaFile));

   my $rv = open(LA_FILE, "<$laFile");
   if (!$rv) {
      print STDERR "Couldn't open .la file '$laFile': $!\n";
      return 1;
   }

   print "Fixing $laFile...\n";

   $rv = open(NEW_LA_FILE, ">$newLaFileFullPath");

   if (!$rv) {
      print STDERR "Couldn't open temporary .la file '$newLaFile': $!\n";
      return 1;
   }

   while (<LA_FILE>) {
      my $line = $_;
      if ($_ =~ /^libdir=/) {
         $line = "libdir='$laFileInstallDir'\n";
      }

      print NEW_LA_FILE $line;
   }

   close(LA_FILE);
   close(NEW_LA_FILE);

   return move($newLaFileFullPath, $laFile);
}

main();
