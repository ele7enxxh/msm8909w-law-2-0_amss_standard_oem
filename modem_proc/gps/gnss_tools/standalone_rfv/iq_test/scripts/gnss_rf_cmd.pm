#===========================================================================
#$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/standalone_rfv/iq_test/scripts/gnss_rf_cmd.pm#1 $ 
#$DateTime: 2016/12/13 07:59:45 $ 
#$Author: mplcsds1 $
#===========================================================================
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
#===========================================================================
#
# Description:
# This is a support file for the gnss_rf_cmd.pl script.
# It should be located in the same directory as that file.
#
#===========================================================================

package RfCmd;

use Win32::OLE;
use Win32::OLE::Variant;

my %RfCmd = (
       Cmd => undef,
       SubSysId => undef,
       SubCmd => undef,
       GpsCmd => undef,
       Version => undef,
       RfMode => undef,
       RfLinearity => undef,
       RfRxdOnTimeSec => undef,
       RfRxdOffTimeSec => undef,
       Reserved => undef,
);

#===========================================================================
# METHOD:
#   new
#
# DESCRIPTION:
#   Constructor
#
# RETURN VALUE:
#   Instance
#
#===========================================================================
sub new
{
   my $that = shift;
   my $class = ref($that) || $that;
   my $self = { %fields,
   };
   bless $self, $class;
   return $self;
}

#===========================================================================
# METHOD:
#   AUTOLOAD
#
# DESCRIPTION:
#   Performs object validation.
#
# RETURN VALUE:
#   Reference object
#
#===========================================================================
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

#===========================================================================
# METHOD:
#   uint32toBytes
#
# DESCRIPTION:
#   Breaks uint32 to individual bytes.
#
# RETURN VALUE:
#   Byte values
#
#===========================================================================
sub uint32toBytes {
   my ($arg) = @_;
   my($byte1, $byte2, $byte3, $byte4);

   # printf ("%d", $arg);
    
   $byte1 = $arg & 0xFF; $arg = $arg >> 8;
   $byte2 = $arg & 0xFF; $arg = $arg >> 8;
   $byte3 = $arg & 0xFF; $arg = $arg >> 8;
   $byte4 = $arg & 0xFF;

   return($byte1, $byte2, $byte3, $byte4);
}

#===========================================================================
# METHOD:
#   SetRfCmd
#
# DESCRIPTION:
#   Initialize RF command bitmap capture packet.
#
# RETURN VALUE:
#   None
#
#===========================================================================
sub SetRfCmd
{
   my $self = shift;

   my $length = 14;
   my $type = ref($self) || die "$self is not an object";

   my $req_pkt = Variant(VT_ARRAY | VT_UI1, $length);

   $req_pkt->Put(0, $self->{"Cmd"});
   $req_pkt->Put(1, $self->{"SubSysId"});
   $req_pkt->Put(2, 0xFF & $self->{"SubCmd"});

   my $upper = 0xFF & ($self->{"SubCmd"} >> 8);
   $req_pkt->Put(3, $upper);
   $req_pkt->Put(4, $self->{"GpsCmd"});
   $req_pkt->Put(5, $self->{"Version"});
   $req_pkt->Put(6, $self->{"RfMode"});
   $req_pkt->Put(7, $self->{"RfLinearity"});
   $req_pkt->Put(8, $self->{"RfRxdOnTimeSec"});
   $req_pkt->Put(9, $self->{"RfRxdOffTimeSec"});

   my @argsTopass = uint32toBytes( $self->{"Reserved"});

   my $i = 10;
   foreach (@argsTopass)
   {
      $req_pkt->Put($i, $_);
      $i++;
   }

   $req_pkt->Put(14, 0);
   $req_pkt->Put(15, 0);
   $req_pkt->Put(16, 0);
   $req_pkt->Put(17, 0);

   return $req_pkt;
}

