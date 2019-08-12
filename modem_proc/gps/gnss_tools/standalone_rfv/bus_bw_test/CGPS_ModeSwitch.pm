# 
#$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/standalone_rfv/bus_bw_test/CGPS_ModeSwitch.pm#1 $ $DateTime: 2016/12/13 07:59:45 $ $Author: mplcsds1 $
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
# This is a support file for the start_sens_dwell.pl and start_pfa_dwell.pl
# scripts.  It should be located in the same directory as those files.
#
#*****************************************************************************


# Define class in Perl to copy display to EFS.

package SA_RF_Verif;

use Win32::OLE;
use Win32::OLE::Variant;

my %fields = (
       cmd => undef,
       id => undef,
       sub_cmd => undef,
       gps_cmd => undef,
       version => undef,
       enter_exit_flag => undef,
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
sub SetCaptureRfVerifCmd
{
   my $self = shift;

   my $length = 10;
   my $type = ref($self) || die "$self is not an object";

   my $req_pkt = Variant(VT_ARRAY | VT_UI1, $length);

   $req_pkt->Put(0, $self->{"cmd"});
   $req_pkt->Put(1, $self->{"subsys_id"});
   $req_pkt->Put(2, 0xFF & $self->{"sub_cmd"});

   my $upper = 0xFF & ($self->{"sub_cmd"} >> 8);
   $req_pkt->Put(3, $upper);
   $req_pkt->Put(4, $self->{"gps_cmd"});
   $req_pkt->Put(5, $self->{"version"});
   $req_pkt->Put(6, $self->{"enter_exit_flag"});
   $req_pkt->Put(7, 0);
   $req_pkt->Put(8, 0);
   $req_pkt->Put(9, 0);
   
   return $req_pkt;
}
