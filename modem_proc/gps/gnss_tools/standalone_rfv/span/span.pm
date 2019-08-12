# 
# $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/standalone_rfv/span/span.pm#1 $
# $DateTime: 2016/12/13 07:59:45 $
# $Author: mplcsds1 $
#*****************************************************************************
#  Copyright (c) 2012, 2013 by QUALCOMM INCORPORATED. All Rights Reserved.
#  Copyright (c) 2015 Qualcomm Technologies, Inc.
#  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved. 
#
# Export of this technology or software is regulated by the U.S. Government.
# Diversion contrary to U.S. law prohibited.
#
# *****************************************************************************
#
# Description:
# This is a support file for the span.pl script.
# It should be located in the same directory as those files.
#
#*****************************************************************************



package SpAn;

use Win32::OLE;
use Win32::OLE::Variant;

my %fields = (
       cmd => undef,
       id => undef,
       sub_cmd => undef,
       gps_cmd => undef,
       version => undef,
       cmd_control_flag => undef,
       scan_start_stop  => undef,
       gps_jobs_enabled => undef,
       glo_jobs_enabled  => undef,
       probe_point       => undef,
       track_mode        => undef,
       notch_filt_reset  => undef
);
# METHOD:
#   new
#
# DESCRIPTION:
#   Constructor
#
# RETURN VALUE:
#   Instance
#
sub new
{
   my $that = shift;
   my $class = ref($that) || $that;
   my $self = { %fields,
   };
   bless $self, $class;
   return $self;
}

# METHOD:
#   SpanCmd
#
# DESCRIPTION:
#   GNSS spectrum Analyzer command request packet
#
# RETURN VALUE:
#   None
#
sub SpanCmd
{
   my $self = shift;

   my $length = 13;
   my $type = ref($self) || die "$self is not an object";

   my $req_pkt = Variant(VT_ARRAY | VT_UI1, $length);

   $req_pkt->Put(0, $self->{"cmd"});
   $req_pkt->Put(1, $self->{"subsys_id"});
   $req_pkt->Put(2, 0xFF & $self->{"sub_cmd"});

   my $upper = 0xFF & ($self->{"sub_cmd"} >> 8);
   $req_pkt->Put(3, $upper);
   $req_pkt->Put(4, $self->{"gps_cmd"});
   $req_pkt->Put(5, $self->{"version"});
   $req_pkt->Put(6, $self->{"cmd_control_flag"});
   $req_pkt->Put(7, $self->{"scan_start_stop"});
   $req_pkt->Put(8, $self->{"gps_jobs_enabled"});
   $req_pkt->Put(9, $self->{"glo_jobs_enabled"});
   $req_pkt->Put(10, $self->{"probe_point"});
   $req_pkt->Put(11, $self->{"track_mode"});
   $req_pkt->Put(12, $self->{"notch_filt_reset"});
   
   return $req_pkt;
}
