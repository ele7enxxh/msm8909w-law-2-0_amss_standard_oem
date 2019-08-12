#! /usr/bin/perl
#******************************************************************************
#* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg/mcfg_tools/mcfg_setup.pl#1 $
#* $DateTime: 2016/12/13 07:58:21 $
#*
#* 
#******************************************************************************
#*
#* Copyright (c) 2015 Qualcomm Technologies, Inc.
#* All rights reserved.
#* Qualcomm Technologies, Inc. Confidential and Proprietary.
#*
#******************************************************************************
#        
# Purpose: 
#   Copy files from build to local machine for MCFG config generation       
# 
#   Script prompts user for a destination if not provided
#   as an argument on the command line
#
# Usage: 
#   perl mcfg_setup.pl [ destination_directory ], 
#   e.g.   perl mcfg_setup.pl c:\temp\mcfg_eng_creation
#        
# Note: 
#        
use strict;
use warnings;
use Cwd 'abs_path';
use File::Spec::Functions qw( catdir canonpath );
use File::Basename;
use File::Path;
#use File::Copy::Recursive qw( rocpy ); #rcopy not available in perl 5.6. 

my $user_dir = shift @ARGV; # allow dest dir to be supplied via command line
my $BUILD_ROOT = catdir( GetMCFGRoot(), "../..");
my @BUILD_SRC = ( 
  'modem_proc/mcfg/mcfg_gen',
  'modem_proc/mcfg/build', 
  'modem_proc/mcfg/configs', 
  'modem_proc/mmcp/policyman/configurations',
);

# ===================================
#   Main
# ===================================
print "Build root: \n  " . $BUILD_ROOT . "\n";
$user_dir = SetPath( $user_dir );   # set the destination path if not passed on cmd line
$user_dir = canonpath( $user_dir ); # make destination path platform independent

#copy files from build to user_dir
for my $dir (@BUILD_SRC)
{
  MCFGcopy( $user_dir, $dir);
}

my $os = $^O; 
if ($os =~ /Win/) 
{
  my @sys_cmd = qq/explorer.exe $user_dir/;
  system(@sys_cmd) or die "error: $!";
  print STDOUT "MCFG copy complete\n";
}


# ===================================
#   Subroutines
# ===================================
sub MCFGcopy
{
  my ($user_dir, $build_src) = @_;
  my $src = catdir($BUILD_ROOT, $build_src);
  my $dst = catdir($user_dir, $build_src);
  my $sys_cmd = "xcopy $src $dst /I /E /R /Y /Q";

  #rcopy not available in perl 5.6. will need in house implementation
  #rcopy ($src, $dst) || die "Copy error - src: $src, dst: $dst: $!";

  die if (!IsDirValid($src));
  print "issuing cmd: \n   $sys_cmd\n";
  print "   Copying files from $build_src...";
  system($sys_cmd); 
  #check return value from system call
  if ($? == -1) 
  {
    die "Failed to execute system call: $!\n";
  }
  elsif ($? & 127) 
  {
    printf  "Copy error - \n" . "src: $src \n" . "dst: $dst\n";
    die "Error: %d, %s coredump\n", ($? & 127),  ($? & 128) ? 'with' : 'without';
  }
}

sub IsDirValid
{
  my ($dir) = @_;

  if (! -e $dir)
  {
    print "Dir '$dir' does not exist. \n";
    return 0;
  }
  elsif (! -d $dir)
  {
    print "Destination '$dir' not a dir. \n";
    return 0;
  }
  else
  {
    return 1;
  }
}
# GetMCFGRoot
# 
# Parameters:
#   None
#
# Purpose: 
#   Finds the MCFG folder and returns its absolute path. This script 
#   may be run from 2 different relative levels:
#       a) MCFG folder = cwd()/.. if running build_mcfgs.pl from command line
#       b) MCFG folder = cwd()/../../../.. if build_mcfgs.exe is called by a spreadsheet.
#   This wasn't needed before since both build_mcfgs.pl and the spreadsheet were
#   one directory level away from the MCFG folder
#
# Returns:
#   Absolute path to MCFG folder
sub GetMCFGRoot {
   my $mcfg_dir = "mcfg";
   # my $curr_dir = getcwd();
   my ($filename, $curr_dir, $suffix) = fileparse(abs_path($0));
   $curr_dir =~ s#\\#/#g; #change windows style paths to unix

   do {
      # Traverse up the directory string by removing the last directory separator and foldername
      $curr_dir =~ s#^(.*)/[^/]+/?$#$1#;
   } while (($curr_dir !~ m#.*$mcfg_dir$#) && $curr_dir ne '');

   die ("MCFG folder not found in parent path.") if ($curr_dir eq '');

   return abs_path("$curr_dir");
}

sub SetPath
{
  my ($dest_dir) = @_;
  my $def_dest = 'c:\temp\mcfg_sandbox';

  # Prompt for destination if not provided via command line
  if (! $dest_dir)
  {
    print "Enter MCFG destination directory [dflt: $def_dest]: \n";
    chomp($dest_dir = <STDIN>);
  }
  # Remove trailing quotes
  $dest_dir =~ s/"//g;
  if ($dest_dir eq '')
  {
    print "MCFG destination: $def_dest \n";
    $dest_dir = $def_dest;
  }
  elsif (! -e $dest_dir)
  {
    print "Destination dir '$dest_dir' does not exist. Defaulting to $def_dest \n";
    $dest_dir = $def_dest;
  }
  elsif (! -d $dest_dir)
  {
    print "Destination '$dest_dir' not a dir. Defaulting to $def_dest \n";
    $dest_dir = $def_dest;
  }

  return  $dest_dir; 
}
