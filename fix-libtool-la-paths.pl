#!/bin/env perl

use strict;

use File::Basename;
use File::Copy qw(move);
use File::Spec::Functions;
use Cwd qw(getcwd realpath);

my $PIVOT_TOKEN = 'windows-i686-msvc8';
my $MUNGED_MARKER = '# LA FILE MUNGED';
my $forceMunge = 0;

sub IsDosStylePath {
   my %args = @_;
   die "ASSERT: IsDosStylePath() passed empty path" if (!exists($args{'path'}));
   my $path = $args{'path'};
   return ($path =~ /:/);
}

sub ConvertToDosStylePath {
   my %args = @_;
   die "ASSERT: ConvertToDosStylePath() passed empty path" if (!exists($args{'path'}));
   my $path = $args{'path'};
   $path =~ s|^/(\w)|$1:|;
   return $path;
}

sub MungeDependencyLibsLine {
   my %args = @_;
   my $line = $args{'line'};
   my $newRoot = $args{'newRoot'};
   my $newRootDosStyle = ConvertToDosStylePath(path => $args{'newRoot'});

   # This is the only line we know how to munge; if it doesn't match, return
   # the old line
   return $line if ($line !~ /^dependency_libs='/);

   # Get just the value of the variable
   $line =~ s/^dependency_libs='//;
   $line =~ s/'\s*$//;

   my @lineParts = split(/\s+/, $line);
   my @newLineParts = ();

   foreach my $part (@lineParts) {
      my $mungedPart = $part; 
      if ($part =~ /$PIVOT_TOKEN/) {
         my ($root, $extension) = split(/$PIVOT_TOKEN/, $mungedPart);
         $extension =~ s|^/||;

         # Non-MSYS-style path check
         #my $partNewRoot = (IsDosStylePath(path => $root) ? 
         # $newRootDosStyle : $newRoot);

         #$mungedPart = catfile($partNewRoot, $PIVOT_TOKEN, $extension);
         $mungedPart = catfile($newRootDosStyle, $PIVOT_TOKEN, $extension);

         # Was the part link library path, or a direct path to a .lib?
         $mungedPart = "-L$mungedPart" if ($root =~ /^\-L/);
      } 

      push(@newLineParts, $mungedPart);
   }
   return 'dependency_libs=\'' . join(' ', @newLineParts) . "'\n";
}

sub main {
   my $laFile = $ARGV[0];

   if (! -f $laFile || $laFile !~ /\.la$/) {
      print STDERR "Usage: $0 [foo.la]\n";
      print STDERR "Example: $0 flac/release/lib/libFLAC.la\n";
      return 1;
   }

   my $laFileFullPath;
   my $laFileInstallDir;

   if ($laFile =~ /^\//) {
      $laFileFullPath = $laFile;
      $laFileInstallDir = dirname($laFileFullPath);
   }
   else {
      $laFileFullPath = catfile(getcwd(), $laFile);
      $laFileInstallDir = dirname(realpath($laFileFullPath));
   }

   my $rv = open(LA_FILE, "<$laFile");
   if (!$rv) {
      print STDERR "Couldn't open .la file '$laFile': $!\n";
      return 1;
   }

   my @currentLaFileContents = ();
   my $mungeThisFile = 1;

   while (<LA_FILE>) {
      my $line = $_;

      if ($line =~ /^$MUNGED_MARKER/ && !$forceMunge) {
         $mungeThisFile = 0;
         last;
      }

      push(@currentLaFileContents, $line);
   }

   close(LA_FILE);

   return 0 if (!$mungeThisFile);

   print "Fixing $laFile...\n";

   my $newLaFile = $laFile . '.new';
   my $newLaFileFullPath = catfile($laFileInstallDir, basename($newLaFile));

   my ($newRoot, $extension) = split(/$PIVOT_TOKEN/, $laFileInstallDir);

   $rv = open(NEW_LA_FILE, ">$newLaFileFullPath");

   if (!$rv) {
      print STDERR "Couldn't open temporary .la file '$newLaFile': $!\n";
      return 1;
   }

   print NEW_LA_FILE $MUNGED_MARKER . " by $0 on " . scalar(localtime()) . "\n";

   foreach my $l (@currentLaFileContents) {
      my $line = $l;

      if ($line =~ /^libdir=(.+)/) {
         #if (IsDosStylePath(path => $1)) {
            $line = "libdir='" . 
             ConvertToDosStylePath(path => $laFileInstallDir) . "'\n";
         #}
         #else {
         #   $line = "libdir='$laFileInstallDir'\n";
         #}
      }
      elsif ($line =~ /^dependency_libs=/) {
         $line = MungeDependencyLibsLine(line => $line,
                                         newRoot => $newRoot);
      }

      print NEW_LA_FILE $line;
   }

   close(NEW_LA_FILE);
   return move($newLaFileFullPath, $laFile);
}

main();
