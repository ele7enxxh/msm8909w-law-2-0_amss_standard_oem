# 
#*****************************************************************************
# Description:
# This is a support file for the SvNoExist.pl.
# It should be located in the same directory.
#
#*****************************************************************************

package SvNoExist;

use Win32::OLE;
use Win32::OLE::Variant;

my %SvNoExist =
(
  cmd => undef,
  subsys_id => undef,
  sub_cmd => undef,
  no_exist_mask => undef,
  action => undef,
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
  my $self =
  {
    %fields,
  };
  bless $self, $class;
  return $self;
}

# METHOD:
#   BuildSvNoExistPkt
#
# DESCRIPTION:
#   Initialize the Sv No Exist command packet.
#
# RETURN VALUE:
#   The Sv No Exist command packet
#
sub BuildSvNoExistPkt
{
   my $self = shift;

   my $length = 9;
   my $type = ref($self) || die "$self is not an object";
   my $sv_mask = $self->{"no_exist_mask"};

   my $req_pkt = Variant(VT_ARRAY | VT_UI1, $length);

   $req_pkt->Put(0, 128);
   $req_pkt->Put(1, 13);
   $req_pkt->Put(2, 106);

   $req_pkt->Put(3, 0);

   for($i = 4; $i < 8; $i++)
   {
     $req_pkt->Put($i, $sv_mask & 0xFF);
     $sv_mask >>= 8;
   }

   $req_pkt->Put(8, $self->{"action"});
   
   return $req_pkt;
}

