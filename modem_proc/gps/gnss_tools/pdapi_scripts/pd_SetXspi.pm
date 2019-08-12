######################################################################
# $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/pdapi_scripts/pd_SetXspi.pm#1 $
# $DateTime: 2016/12/13 07:59:45 $
# $Author: mplcsds1 $
#
# Define class in Perl to copy display to EFS.
#
######################################################################

package pd_SetXspi;

use Win32::OLE;
use Win32::OLE::Variant;
use strict;
use warnings;

# CGPS_SET_PARAM_CMD request packet field.
my %fields = (
  cmd_code           => undef, # (1 byte)
  subsys_id          => undef, # (1 byte)
  subsys_cmd_code    => undef, # (2 bytes)
  cgps_cmd_code      => undef, # (1 byte)
  version            => undef, # (1 byte)
  client_id          => undef, # (4 bytes)
  param_type         => undef, # (1 byte)
  param_length       => undef, # (2 bytes)
  mode               => undef, # (1 bytes)
  reserved_0         => undef, # (4 bytes)
  reserved_1         => undef, # (4 bytes)
);

#length = 22 bytes

######################################################################
# METHOD:
#   new
#
# DESCRIPTION:
#   Constructor
#
# RETURN VALUE:
#   Instance
#
######################################################################
sub new
{
  my $that = shift;
  my $class = ref($that) || $that;
  my $self = { %fields };
  bless $self, $class;
  return $self;
}

######################################################################
# METHOD:
#   AUTOLOAD
#
# DESCRIPTION:
#   Performs object validation
#
# RETURN VALUE:
#   reference object
#
######################################################################
sub test_AUTOLOAD
{
  my $self = shift;
  # my $type = ref($that) || die "$self is not an object";
  my $type = ref($self) || die "$self is not an object";
  my $name = "AUTOLOAD"; # $AUTOLOAD;
  $name =~ s/.*://;   # strip fully-qualified portion
  unless (exists $self->{$name})
  {
    die "Can't access '$name' field in object of class $type";
  }
  if (@_)
  {
    return $self->{$name} = shift;
  }
  else
  {
    return $self->{$name};
  }
}

######################################################################
# METHOD:
#   uint32toBytes
#
# DESCRIPTION:
#   Subroutine to convert 32-bit integer to array of 4 bytes
#
# RETURN VALUE:
#   Array of 4 individual bytes representing uint32
#
######################################################################
sub uint32toBytes
{
  my ($arg) = @_;
  my ($byte1, $byte2, $byte3, $byte4);

  # not sure of perl's bitwise operators - change code later
  $byte1 = $arg%256; $arg = $arg/256;
  $byte2 = $arg%256; $arg = $arg/256;
  $byte3 = $arg%256; $arg = $arg/256;
  $byte4 = $arg%256;

  return ($byte1, $byte2, $byte3, $byte4);
}

######################################################################
# METHOD:
#   SetCaptureCmd
#
# DESCRIPTION:
#   Initialize DM CGPS_SET_PARAM_CMD capture request packet
#
# RETURN VALUE:
#   Request packet
#
######################################################################
sub SetCaptureCmd
{
  my $self = shift;

  my $length = 22;
  my $i = 0;

  my $type = ref($self) || die "$self is not an object";

  my $req_pkt = Variant(VT_ARRAY | VT_UI1, $length);

  $req_pkt->Put($i,  $self->{"cmd_code"});
  $i++;

  $req_pkt->Put($i,  $self->{"subsys_id"});
  $i++;

  $req_pkt->Put($i,  $self->{"subsys_cmd_code"} & 0xFF);
  $i++;

  $req_pkt->Put($i, ($self->{"subsys_cmd_code"} >> 8) & 0xFF);
  $i++;

  $req_pkt->Put($i,  $self->{"cgps_cmd_code"});
  $i++;

  $req_pkt->Put($i,  $self->{"version"});
  $i++;

  my @arg_bytes = uint32toBytes($self->{"client_id"});
  foreach (@arg_bytes)
  {
    $req_pkt->Put($i, $_);
    $i++;
  }


  $req_pkt->Put($i,  $self->{"param_type"});
  $i++;

  # param_length       => undef, # (2 bytes)
  $req_pkt->Put($i,  $self->{"param_length"} & 0xFF);
  $i++;

  $req_pkt->Put($i, ($self->{"param_length"} >> 8) & 0xFF);
  $i++;

  # mode               => undef, # (1 bytes)
  $req_pkt->Put($i,  $self->{"mode"});
  $i++;

  @arg_bytes = uint32toBytes($self->{"reserved_0"});
  foreach (@arg_bytes)
  {
    $req_pkt->Put($i, $_);
    $i++;
  }

  @arg_bytes = uint32toBytes($self->{"reserved_1"});
  foreach (@arg_bytes)
  {
    $req_pkt->Put($i, $_);
    $i++;
  }

  die("number of bytes sent [$i] does not match packet size [$length] !") if($i != $length) ;

  return $req_pkt;
}

######################################################################
# METHOD:
#   ParseData
#
# DESCRIPTION:
#   Subroutine to parse the response packet
#
# RETURN VALUE:
#   None
#
######################################################################
sub ParseData
{
  my $self = shift;
  my $type = ref($self) || die "$self is not an object";
  my $data = shift;

  my (@results) = unpack("ccSccLL", $data);

  $self->{"cmd_code"}        = $results[0];
  $self->{"subsys_id"}       = $results[1];
  $self->{"subsys_cmd_code"} = $results[2];
  $self->{"cgps_cmd_code"}   = $results[3];
  $self->{"version"}         = $results[4];
  $self->{"client_id"}       = $results[5];
  $self->{"status"}          = $results[6];

  printf("cgps_cmd_code = %d, client_id = %d, status = %d\n", $results[3], $results[5], $results[6]);
}

######################################################################
# METHOD:
#   GetClientId
#
# DESCRIPTION:
#   Subroutine to retrieve Client ID from the response packet
#
# RETURN VALUE:
#   Client ID
#
######################################################################
sub GetClientId
{
  my $self = shift;
  my $type = ref($self) || die "$self is not an object";

  if ($self->{"cmd_code"} != 75)
  {
    return -1;
  }

  return $self->{"client_id"};
}

######################################################################
# METHOD:
#   GetStatus
#
# DESCRIPTION:
#   Subroutine to retrieve status from the response packet
#
# RETURN VALUE:
#   Status
#
######################################################################
sub GetStatus
{
  my $self = shift;
  my $type = ref($self) || die "$self is not an object";

  if ($self->{"cmd_code"} != 75)
  {
    return -1;
  }

  return $self->{"status"};
}
