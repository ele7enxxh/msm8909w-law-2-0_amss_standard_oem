# /*----------------------------------------------------------------------------
# *  Copyright (c) 2015 Qualcomm Atheros, Inc.
# *  All Rights Reserved.
# *  Qualcomm Atheros Confidential and Proprietary.
# *----------------------------------------------------------------------------
# */

# 
#*****************************************************************************
#  Copyright (C) 2004 Qualcomm, Inc.
#
#                  Qualcomm, Inc.
#                  675 Campbell Technology Parkway
#                  Campbell, CA  95008
#
# This program is confidential and a trade secret of Qualcomm, Inc.  The
# receipt or possession of this program does not convey any rights to reproduce 
# or disclose its contents or to manufacture, use or sell anything that this 
# program describes in whole or in part, without the express written consent of
# Qualcomm, Inc.  The recipient and/or possessor of this program shall not 
# reproduce or adapt or disclose or use this program except as expressly 
# allowed by a written authorization from Qualcomm, Inc.
#
# *****************************************************************************
#
# Description:
# This is a support file for the multichantrack.pl.
# It should be located in the same directory.
#
#*****************************************************************************


package RFDev_MultiChanTrack;

use Win32::OLE;
use Win32::OLE::Variant;

my %SvTrack = (
       cmd => undef,
       subsys_id => undef,
       sub_cmd => undef,
       gps_cmd => undef,
       version => undef,
       test_mode => undef,
       glo_hw_chan_num => undef,
       gps_sbas_sv_id => undef,
       glo_freq_id => undef,
       gps_sbas_bitedge_setting => undef,
       glo_bitedge_setting => undef,
       command => undef,
       bds_sv_id => undef,
       bds_bitedge_setting => undef,
       gal_sv_id => undef,
       gal_bitedge_setting => undef,
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
#   SetCaptureCmd
#
# DESCRIPTION:
#   Initialize HS bitmap capture command packet.
#
# RETURN VALUE:
#   None
#
sub SetCaptureRFDev_MultiChanTrack
{
  my $self = shift;

  my $length = 17;
  my $type = ref($self) || die "$self is not an object";

  my $req_pkt = Variant(VT_ARRAY | VT_UI1, $length);

  $req_pkt->Put(0, $self->{"cmd"});
  $req_pkt->Put(1, $self->{"subsys_id"});
  $req_pkt->Put(2, 0xFF & $self->{"sub_cmd"});

  my $upper = 0xFF & ($self->{"sub_cmd"} >> 8);
  $req_pkt->Put(3, $upper);
  $req_pkt->Put(4, $self->{"gps_cmd"});
  $req_pkt->Put(5, $self->{"version"});
  $req_pkt->Put(6, $self->{"test_mode"});
  $req_pkt->Put(7, $self->{"glo_hw_chan_num"});
  $req_pkt->Put(8, $self->{"gps_sbas_sv_id"});
  $req_pkt->Put(9, $self->{"glo_freq_id"});
  $req_pkt->Put(10, $self->{"gps_sbas_bitedge_setting"});
  $req_pkt->Put(11, $self->{"glo_bitedge_setting"});
  $req_pkt->Put(12, $self->{"bds_bitedge_setting"});
  $req_pkt->Put(13, $self->{"bds_sv_id"});
  $req_pkt->Put(14, $self->{"gal_bitedge_setting"});
  $req_pkt->Put(15, $self->{"gal_sv_id"});
  $req_pkt->Put(16, $self->{"command"});

  return $req_pkt;
}
