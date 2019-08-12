#! /usr/bin/perl -w
#===========================================================================
#
#                  N V _ I T E M S _ M E T A . P L
#
# DESCRIPTION
#
#  This script is used during HtoRPC build time to insert meta-comments in
#  the generated nv_items.h. This is done because it is not possible to
#  insert these meta-comments using NVC.
#
# USAGE:
#   nv_items_meta.pl <input> <output>
#
# METHODOLOGY:
#   Read input file and insert meta-comments in designated locations and save
#   to output.
#
# Copyright (c)  2007 by QUALCOMM, Incorporated.  All Rights Reserved.
#============================================================================
#============================================================================
#
#                            EDIT HISTORY FOR MODULE
#
#  $Id: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/rapi/services/src/nv_items_meta.pl#1 $
#
# when        who    what, where, why
# --------    ---    --------------------------------------------------------
# 02/16/07    hn     Created this file
#============================================================================

#============================================================================
# GLOBAL DECLARATIONS
#============================================================================
my %METACOMMENTS;
my $PATTERN_MATCH;
my $INPUT;
my $OUTPUT;
my $LINE;

my $USAGE =<<"USAGE";
Usage: $0 <input> <output>

USAGE

#============================================================================
#
#                     I N I T I A L I Z A T I O N S
#
#============================================================================

#----------------------------------------------------------------------------
# Maintain metacomments here:
#----------------------------------------------------------------------------
%METACOMMENTS =
(
 #---------------------------------------------------------------------------
 # Metacomments for nv_rtsp_proxy_server_addr
 #---------------------------------------------------------------------------
 "nv_rtsp_proxy_server_addr" =>
   "/*~ CASE NV_ADDR_INVALID nv_rtsp_proxy_server_addr.void */\n" .
   "/*~ CASE NV_IPV4_ADDR_MASK nv_rtsp_proxy_server_addr.v4 */\n" .
   "/*~ CASE NV_IPV6_ADDR_MASK nv_rtsp_proxy_server_addr.v6 */\n" .
   "/*~ DEFAULT nv_rtsp_proxy_server_addr.void */\n",

 #---------------------------------------------------------------------------
 # Metacomments for nv_rtsp_proxy_server_addr_type
 #---------------------------------------------------------------------------
 "nv_rtsp_proxy_server_addr_type" =>
   "/*~ FIELD nv_rtsp_proxy_server_addr_type.addr DISC " .
   "nv_rtsp_proxy_server_addr_type.flags */\n"
);

#----------------------------------------------------------------------------
# Build PATTERN_MATCH
#----------------------------------------------------------------------------
$PATTERN_MATCH = "";
for my $type ( sort keys %METACOMMENTS )
{
  $PATTERN_MATCH .= $type . "|";
}
chop($PATTERN_MATCH);

#----------------------------------------------------------------------------
# Check arguments
#----------------------------------------------------------------------------
if ( @ARGV < 2 )
{
  die $USAGE;
}

#----------------------------------------------------------------------------
# Initialize NV path
#----------------------------------------------------------------------------
$INPUT = $ARGV[0];
$OUTPUT = $ARGV[1];

#============================================================================
#
#                         M E T H O D O L O G Y
#
#============================================================================

#----------------------------------------------------------------------------
# Insert meta-comments in nv_PPExpand.h and save as nv_PPExpand.htorpc.h
#----------------------------------------------------------------------------
if ( ! open(INPUT, $INPUT) )
{
  die "Could not open $INPUT for reading\n";
}

if ( ! open(OUTPUT, ">", $OUTPUT) )
{
  die "Could not open $OUTPUT for writing\n";
}

while ( $LINE = <INPUT> )
{
  chomp $LINE;
  $LINE =~ s/\r$//;
  print OUTPUT $LINE, "\n";
  if ( $LINE =~ /\b($PATTERN_MATCH);/ )
  {
    my $key = $1;
    print OUTPUT $METACOMMENTS{$key};    
  }
}

close(OUTPUT);
close(INPUT);
