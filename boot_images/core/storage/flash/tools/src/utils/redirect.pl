#!/usr/bin/perl
#===================================================================
# FILE: redirect.pl
#
# Copyright (c) 2008-2009 Qualcomm Incorporated. 
#  All Rights Reserved.
#  Qualcomm Confidential and Proprietary
#===================================================================
#===================================================================
#                          EDIT HISTORY FOR MODULE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
#  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/utils/redirect.pl#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
#  
# when       who     what, where, why
# --------   ---     -------------------------------------------------
# 10/29/08    mm      Initial version
#=====================================================================
use POSIX qw(uname);

my $BuildPlatform = "WIN32";

my ($uname_s, $uname_r)  = (POSIX::uname())[0,2];

if ($uname_s eq "Linux")
{
   $BuildPlatform = "LINUX";
}
 
open (FILEHANDLE, "> $ARGV[0]");
foreach $i (1..$#ARGV)
{
  $_=$ARGV[$i];

  if( $BuildPlatform eq "WIN32" )
  {
    s#\/#\\#g;
  }

  print FILEHANDLE "$_ ";
}
close FILEHANDLE;

