# 
#
#*****************************************************************************
#  Copyright (C) 2004, 2015 Qualcomm, Inc.
#  Copyright (c) 2015 Qualcomm Technologies, Inc.
#  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved. 
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
# This is a support file for StartIqTest.pl.
# They should be in the same directory.
#
#*****************************************************************************


package IqTest;

use Win32::OLE;
use Win32::OLE::Variant;

my %IqTest = (
       cmd => undef,
       id => undef,
       sub_cmd => undef,
       gps_cmd => undef,
       version => undef,
       requests => undef,
       fcount => undef,
       tsinfo => undef,
       ts_signal_power => undef,
       ts_doppler => undef,
       sample_cnt_1k => undef,
       fft_integs => undef,
       sm_collect => undef,
       collect_point => undef,
       scale => undef,
       start_delay => undef,
       wb_center_freq => undef,
       nb_center_freq => undef,
       num_wb_peaks => undef,
       num_wb_adj_samples => undef,
       num_nb_peaks => undef,
       num_nb_adj_samples => undef,
       adciq_sat_percent => undef,
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
sub SetCaptureIqTestCmd
{
   my $self = shift;

   my $length = 51;
   my $type = ref($self) || die "$self is not an object";

   my $req_pkt = Variant(VT_ARRAY | VT_UI1, $length);

   $req_pkt->Put(0, $self->{"cmd"});
   $req_pkt->Put(1, $self->{"subsys_id"});
   $req_pkt->Put(2, 0xFF & $self->{"sub_cmd"});

   my $upper = 0xFF & ($self->{"sub_cmd"} >> 8);
   $req_pkt->Put(3, $upper);
   $req_pkt->Put(4, $self->{"gps_cmd"});
   $req_pkt->Put(5, $self->{"version"});
   $req_pkt->Put(6, $self->{"requests"});

   my @argsTopass = uint32toBytes( $self->{"fcount"});

   my $i = 7;
   foreach (@argsTopass)
   {
      $req_pkt->Put($i, $_);
      $i++;
   }

   $req_pkt->Put(11, $self->{"tsinfo"});
   my $upper = 0xFF & ($self->{"tsinfo"} >> 8);
   $req_pkt->Put(12, $upper);

   $req_pkt->Put(13, $self->{"ts_signal_power"});

   my @argsTopass = uint32toBytes( $self->{"ts_doppler"});

   my $i = 14;
   foreach (@argsTopass)
   {
      $req_pkt->Put($i, $_);
      $i++;
   }

   $req_pkt->Put(18, $self->{"sample_cnt_1k"});
   my $upper = 0xFF & ($self->{"sample_cnt_1k"} >> 8);
   $req_pkt->Put(19, $upper);

   $req_pkt->Put(20, $self->{"fft_integs"});
   my $upper = 0xFF & ($self->{"fft_integs"} >> 8);
   $req_pkt->Put(21, $upper);

   $req_pkt->Put(22, $self->{"sm_point"});
   $req_pkt->Put(23, $self->{"collect_point"});
   $req_pkt->Put(24, $self->{"scale"});

   $req_pkt->Put(25, $self->{"start_delay"});
   my $upper = 0xFF & ($self->{"start_delay"} >> 8);
   $req_pkt->Put(26, $upper);

   my @argsTopass = uint32toBytes( $self->{"wb_center_freq"});

   my $i = 27;
   foreach (@argsTopass)
   {
      $req_pkt->Put($i, $_);
      $i++;
   }

   my @argsTopass = uint32toBytes( $self->{"nb_center_freq"});

   my $i = 31;
   foreach (@argsTopass)
   {
      $req_pkt->Put($i, $_);
      $i++;
   }

   $req_pkt->Put(35, $self->{"num_wb_peaks"});
   $req_pkt->Put(36, $self->{"num_wb_adj_samples"});
   $req_pkt->Put(37, 0xFF & $self->{"num_nb_peaks"});
   $req_pkt->Put(38, 0xFF & $self->{"num_nb_adj_samples"});

   my @argsTopass = uint32toBytes( $self->{"adciq_sat_percent"});

   my $i = 39;
   foreach (@argsTopass)
   {
      $req_pkt->Put($i, $_);
      $i++;
   }
   
   $req_pkt->Put(43, 0);
   $req_pkt->Put(44, 0);
   
   return $req_pkt;
}
