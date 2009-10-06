#!/bin/env perl

use strict;

use Getopt::Long;
use File::Basename;
use File::Copy qw(move);
use File::Spec::Functions;
use Cwd qw(getcwd realpath);

my $PIVOT_TOKEN = 'windows-i686-msvc8';
my $MUNGED_MARKER = '# LA FILE MUNGED';

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

   # This is the only line this function knows how to munge
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

         $mungedPart = catfile($newRootDosStyle, $PIVOT_TOKEN, $extension);

         # Was the part link library path, or a direct path to a .lib?
         $mungedPart = "-L$mungedPart" if ($root =~ /^\-L/);
      } 

      push(@newLineParts, $mungedPart);
   }
   return 'dependency_libs=\'' . join(' ', @newLineParts) . "'\n";
}

sub Usage {
   print STDERR <<__END_USAGE__;
Usage: $0 [-f] [full path to libtool .la file]

If the supplied .la file has a previouly munged marker in it, it will not be
modified unless -f is supplied.

Example: $0 /e/builds/sb-deps/flac/release/lib/libFLAC.la
__END_USAGE__
}

sub main {
   my $laFile;
   my $forceMunge = 0;

   my $optsRv = GetOptions('force|f' => \$forceMunge);

   if (!$optsRv) {
      Usage();
      return 1;
   }

   my $laFile = $ARGV[0];
   
   if ($laFile !~ /\.la$/ || !(-f $laFile)) {
      Usage();
      return 1;
   }

   my $laFileFullPath;
   my $laFileInstallDir;

   if ($laFile =~ /^\//) {
      $laFileFullPath = $laFile;
      $laFileInstallDir = dirname($laFileFullPath);
   }
   else {
      print STDERR "Need full path to .la file\n";
      Usage();
      return 1;
   }

   my $rv = open(LA_FILE, "<$laFile");
   if (!$rv) {
      print STDERR "Couldn't open .la file '$laFile': $!\n";
      Usage();
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

   print (($forceMunge ? 'Force-fixing ' : 'Fixing' ) . " $laFile...\n");

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

      # Remove old "Munged by" markers (which confuse libtool)...
      next if ($line =~ /^$MUNGED_MARKER/);

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
