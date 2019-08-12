#----------------------------------------------------------------------------
#  Copyright (c) 2014 Qualcomm Atheros, Inc.
#  All Rights Reserved.
#  Qualcomm Atheros Confidential and Proprietary.
#----------------------------------------------------------------------------
#

# 
#$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/standalone_rfv/presc_dwells/dwell.pm#1 $ $DateTime: 2016/12/13 07:59:45 $ $Author: mplcsds1 $
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
# This is a support file for the StartPrescDwell.pl scripts.
# It should be located in the same directory as that file.
#
#*****************************************************************************


# Define class in Perl to copy display to EFS.

package Dwell;

use Win32::OLE;
use Win32::OLE::Variant;

my %dwell = (
       cmd => undef,
       id => undef,
       sub_cmd => undef,
       gps_cmd => undef,
       version => undef,
       cmd_ctrl_flag => undef,
       seq_num => undef,
       gps_rxd_flag => undef,
       gps_ctrl_flag => undef,
       gps_sv => undef,
       gps_dopp_mps => undef,
       gps_dopp_win_hz => undef,
       gps_code_phase_center_c32 => undef,
       gps_code_phase_win_c1 => undef,
       gps_srch_mode => undef,
       gps_num_postd => undef,
       gps_pfa => undef,
       gps_reserved => undef,
       glo_ctrl_flag => undef,
       glo_freq_k => undef,
       glo_dopp_mps => undef,
       glo_dopp_win_hz => undef,
       glo_code_phase_center_c32 => undef,
       glo_code_phase_win_c1 => undef,
       glo_srch_mode => undef,
       glo_num_postd => undef,
       glo_hw_chan => undef,
       glo_pfa => undef,
       glo_reserved => undef,
       dwell_cnt => undef,
       total_num_dwells => undef,
       pass_dwell_cnt => undef,
       reserved => undef,

       bds_ctrl_flag => undef,
       bds_sv => undef,
       bds_dopp_mps => undef,
       bds_dopp_win_hz => undef,
       bds_code_phase_center_c40 => undef,
       bds_code_phase_win_c1 => undef,
       bds_srch_mode => undef,
       bds_num_postd => undef,
       bds_pfa => undef,
       bds_reserved => undef,

       max_sv_dwell => undef,

       gal_ctrl_flag => undef,
       gal_sv => undef,
       gal_dopp_mps => undef,
       gal_dopp_win_hz => undef,
       gal_code_phase_center_c40 => undef,
       gal_code_phase_win_c1 => undef,
       gal_srch_mode => undef,
       gal_num_postd => undef,
       gal_pfa => undef,
       gal_reserved => undef,
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
#   AUTOLOAD
#
# DESCRIPTION:
#   Performs object validataion.
#
# RETURN VALUE:
#   reference object
#
sub AUTOLOAD
{
   my $self = shift;
   my $type = ref($that) || die "$self is not an object";
   my $name = $AUTOLOAD;
   $name =~ s/.*://;   #strip fully-qualified portion
   unless( exists $self->{$name} ) {
      die "Can't access '$name' field in object of class $type";
   }
   if( @_) {
      return $self->{$name} = shift;
   }else{
      return $self->{$name};
   }
}

sub uint32toBytes {
   my ($arg) = @_;
   my($byte1, $byte2, $byte3, $byte4);

   # printf ("%d", $arg);
    
   $byte1 = $arg & 0xFF; $arg = $arg >> 8;
   $byte2 = $arg & 0xFF; $arg = $arg >> 8;
   $byte3 = $arg & 0xFF; $arg = $arg >> 8;
   $byte4 = $arg & 0xFF;

   # printf ("  %d %d %d %d\n", $byte1, $byte2, $byte3, $byte4);

   return($byte1, $byte2, $byte3, $byte4);

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
sub SetCaptureDwellCmd
{
   my $self = shift;
   my $i = 0;

   my $length = 137;
   my $type = ref($self) || die "$self is not an object";

   my $req_pkt = Variant(VT_ARRAY | VT_UI1, $length);

   $req_pkt->Put($i++, $self->{"cmd"});
   $req_pkt->Put($i++, $self->{"subsys_id"});
   $req_pkt->Put($i++, 0xFF & $self->{"sub_cmd"});

   my $upper = 0xFF & ($self->{"sub_cmd"} >> 8);
   $req_pkt->Put($i++, $upper);
   $req_pkt->Put($i++, $self->{"gps_cmd"});
   $req_pkt->Put($i++, $self->{"version"});
   $req_pkt->Put($i++, $self->{"cmd_ctrl_flag"});

   my @argsTopass = uint32toBytes( $self->{"seq_num"});

   foreach (@argsTopass)
   {
      $req_pkt->Put($i++, $_);
   }

   $req_pkt->Put($i++, $self->{"gps_rxd_flag"});
   $req_pkt->Put($i++, $self->{"gps_ctrl_flag"});
   $req_pkt->Put($i++, $self->{"gps_sv"});
   my @argsTopass = uint32toBytes( $self->{"gps_dopp_mps"});

   foreach (@argsTopass)
   {
      $req_pkt->Put($i++, $_);
   }

   my @argsTopass = uint32toBytes( $self->{"gps_dopp_win_hz"});

   foreach (@argsTopass)
   {
      $req_pkt->Put($i++, $_);
   }

   my @argsTopass = uint32toBytes( $self->{"gps_code_phase_center_c32"});

   foreach (@argsTopass)
   {
      $req_pkt->Put($i++, $_);
   }

   my @argsTopass = uint32toBytes( $self->{"gps_code_phase_win_c1"});

   foreach (@argsTopass)
   {
      $req_pkt->Put($i++, $_);
   }

   $req_pkt->Put($i++, $self->{"gps_srch_mode"});

   $req_pkt->Put($i++, 0xFF & $self->{"gps_num_postd"});
   my $upper = 0xFF & ($self->{"gps_num_postd"} >> 8);
   $req_pkt->Put($i++, $upper);
   $req_pkt->Put($i++, $self->{"gps_pfa"});

   my @argsTopass = uint32toBytes( $self->{"gps_reserved"});

   foreach (@argsTopass)
   {
      $req_pkt->Put($i++, $_);
   }

   $req_pkt->Put($i++, $self->{"glo_ctrl_flag"});
   $req_pkt->Put($i++, $self->{"glo_freq_k"});
   my @argsTopass = uint32toBytes( $self->{"glo_dopp_mps"});

   foreach (@argsTopass)
   {
      $req_pkt->Put($i++, $_);
   }

   my @argsTopass = uint32toBytes( $self->{"glo_dopp_win_hz"});

   foreach (@argsTopass)
   {
      $req_pkt->Put($i++, $_);
   }

   my @argsTopass = uint32toBytes( $self->{"glo_code_phase_center_c32"});

   foreach (@argsTopass)
   {
      $req_pkt->Put($i++, $_);
   }

   my @argsTopass = uint32toBytes( $self->{"glo_code_phase_win_c1"});

   foreach (@argsTopass)
   {
      $req_pkt->Put($i++, $_);
   }

   $req_pkt->Put($i++, $self->{"glo_srch_mode"});

   $req_pkt->Put($i++, 0xFF & $self->{"glo_num_postd"});
   my $upper = 0xFF & ($self->{"glo_num_postd"} >> 8);
   $req_pkt->Put($i++, $upper);

   $req_pkt->Put($i++, $self->{"glo_hw_chan"});
   $req_pkt->Put($i++, $self->{"glo_pfa"});

   my @argsTopass = uint32toBytes( $self->{"glo_reserved"});

   foreach (@argsTopass)
   {
      $req_pkt->Put($i++, $_);
   }

   $req_pkt->Put($i++, 0xFF & $self->{"dwell_cnt"});
   my $upper = 0xFF & ($self->{"dwell_cnt"} >> 8);
   $req_pkt->Put($i++, $upper);

   $req_pkt->Put($i++, 0xFF & $self->{"total_num_dwells"});
   my $upper = 0xFF & ($self->{"total_num_dwells"} >> 8);
   $req_pkt->Put($i++, $upper);

   $req_pkt->Put($i++, 0xFF & $self->{"pass_dwell_cnt"});
   my $upper = 0xFF & ($self->{"pass_dwell_cnt"} >> 8);
   $req_pkt->Put($i++, $upper);


   my @argsTopass = uint32toBytes($self->{"reserved"}); 

   foreach (@argsTopass)
   {
      $req_pkt->Put($i++, $_);
   }

  $req_pkt->Put($i++, $self->{"bds_ctrl_flag"});
  $req_pkt->Put($i++, $self->{"bds_sv"});
  my @argsTopass = uint32toBytes( $self->{"bds_dopp_mps"});

  foreach (@argsTopass)
  {
    $req_pkt->Put($i++, $_);
  }

  my @argsTopass = uint32toBytes( $self->{"bds_dopp_win_hz"});

  foreach (@argsTopass)
  {
    $req_pkt->Put($i++, $_);
  }

  my @argsTopass = uint32toBytes( $self->{"bds_code_phase_center_c40"});

  foreach (@argsTopass)
  {
    $req_pkt->Put($i++, $_);
  }

  my @argsTopass = uint32toBytes( $self->{"bds_code_phase_win_c1"});

  foreach (@argsTopass)
  {
    $req_pkt->Put($i++, $_);
  }

  $req_pkt->Put($i++, $self->{"bds_srch_mode"});

  $req_pkt->Put($i++, 0xFF & $self->{"bds_num_postd"});
  my $upper = 0xFF & ($self->{"bds_num_postd"} >> 8);
  $req_pkt->Put($i++, $upper);
  $req_pkt->Put($i++, $self->{"bds_pfa"});

  my @argsTopass = uint32toBytes( $self->{"bds_reserved"});

  foreach (@argsTopass)
  {
    $req_pkt->Put($i++, $_);
  }
  #<-BDS

  #Max SV Dwell parameter
  $req_pkt->Put($i++, $self->{"max_sv_dwell"});

  # GAL ->
  $req_pkt->Put($i++, $self->{"gal_ctrl_flag"});
  $req_pkt->Put($i++, $self->{"gal_sv"});
  my @argsTopass = uint32toBytes( $self->{"gal_dopp_mps"});

  foreach (@argsTopass)
  {
    $req_pkt->Put($i++, $_);
  }

  my @argsTopass = uint32toBytes( $self->{"gal_dopp_win_hz"});

  foreach (@argsTopass)
  {
    $req_pkt->Put($i++, $_);
  }

  my @argsTopass = uint32toBytes( $self->{"gal_code_phase_center_c40"});

  foreach (@argsTopass)
  {
    $req_pkt->Put($i++, $_);
  }

  my @argsTopass = uint32toBytes( $self->{"gal_code_phase_win_c1"});

  foreach (@argsTopass)
  {
    $req_pkt->Put($i++, $_);
  }

  $req_pkt->Put($i++, $self->{"gal_srch_mode"});

  $req_pkt->Put($i++, 0xFF & $self->{"gal_num_postd"});
  my $upper = 0xFF & ($self->{"gal_num_postd"} >> 8);
  $req_pkt->Put($i++, $upper);
  $req_pkt->Put($i++, $self->{"gal_pfa"});

  my @argsTopass = uint32toBytes( $self->{"gal_reserved"});

  foreach (@argsTopass)
  {
    $req_pkt->Put($i++, $_);
  }
  #<-GAL
   $req_pkt->Put($i++, 0);
   $req_pkt->Put($i++, 0);
   $req_pkt->Put($i++, 0);
   $req_pkt->Put($i++, 0);

   return $req_pkt;
}
