######################################################################
# $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/pdapi_scripts/pd_GetPos.pm#1 $
# $DateTime: 2016/12/13 07:59:45 $
# $Author: mplcsds1 $
#
# Define class in Perl to copy display to EFS.
#
######################################################################

package pd_GetPos;

use Win32::OLE;
use Win32::OLE::Variant;

# CGPS_START_CMD request packet field. Or response??
my %fields = (
  cmd_code          => undef, # (1 byte)
  subsys_id         => undef, # (1 byte)
  subsys_cmd_code   => undef, # (2 bytes)
  cgps_cmd_code     => undef, # (1 byte)
  version           => undef, # (1 byte)
  session_type      => undef, # (1 byte)
  operating_type    => undef, # (1 byte)
  server_option     => undef, # (1 byte)
  server_addr_type  => undef, # (1 byte)
  class_id          => undef, # (4 bytes)
  qos_gps_sess_tout => undef, # (1 byte)
  qos_acc_thrshld   => undef, # (4 bytes)
  time_bw_fixes     => undef, # (4 bytes)
  num_fixes         => undef, # (4 bytes)
  client_id         => undef, # (4 bytes)
  reserved          => undef, # (8 bytes)
  server_ipv4_adrs  => undef, # (4 bytes)
  server_ipv4_port  => undef, # (4 bytes)
);


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
sub AUTOLOAD
{
  my $self = shift;
  my $type = ref($that) || die "$self is not an object";
  my $name = $AUTOLOAD;
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
#   Initialize the DM CGPS_START_CMD capture request packet
#
# RETURN VALUE:
#   Request packet
#
######################################################################
sub SetCaptureCmd
{
  my $self = shift;

  my $length = 47;
  my $type = ref($self) || die "$self is not an object";

  my $req_pkt = Variant(VT_ARRAY | VT_UI1, $length);

  $req_pkt->Put(0,  $self->{"cmd_code"});
  $req_pkt->Put(1,  $self->{"subsys_id"});
  $req_pkt->Put(2,  $self->{"subsys_cmd_code"} & 0xFF);
  $req_pkt->Put(3, ($self->{"subsys_cmd_code"} >> 8) & 0xFF);
  $req_pkt->Put(4,  $self->{"cgps_cmd_code"});
  $req_pkt->Put(5,  $self->{"version"});
  $req_pkt->Put(6,  $self->{"session_type"});
  $req_pkt->Put(7,  $self->{"operating_type"});
  $req_pkt->Put(8,  $self->{"server_option"});
  $req_pkt->Put(9,  $self->{"server_addr_type"});

  my @arg_bytes = uint32toBytes($self->{"class_id"});
  my $i = 10;
  foreach (@arg_bytes)
  {
    $req_pkt->Put($i, $_);
    $i++;
  }

  $req_pkt->Put(14, $self->{"qos_gps_sess_tout"});

  my @arg_bytes = uint32toBytes($self->{"qos_acc_thrshld"});
  my $i = 15;
  foreach (@arg_bytes)
  {
    $req_pkt->Put($i, $_);
    $i++;
  }

  my @arg_bytes = uint32toBytes($self->{"time_bw_fixes"});
  my $i = 19;
  foreach (@arg_bytes)
  {
    $req_pkt->Put($i, $_);
    $i++;
  }

  my @arg_bytes = uint32toBytes($self->{"num_fixes"});
  my $i = 23;
  foreach (@arg_bytes)
  {
    $req_pkt->Put($i, $_);
    $i++;
  }

  my @arg_bytes = uint32toBytes($self->{"client_id"});
  my $i = 27;
  foreach (@arg_bytes)
  {
    $req_pkt->Put($i, $_);
    $i++;
  }

  # Reserved[0] is used to pass the BypassMask for testing purposes.
  $req_pkt->Put(31,  $self->{"reserved"});

  # All 7 bytes of $self->{"reserved"} should be set to 0
  my $i = 32;
  while ($i <= 38)
  {
    $req_pkt->Put($i, 0);
    $i++;
  }

  my @arg_bytes = split(/\./, $self->{"server_ipv4_adrs"});
  my $i = 39;
  my $j = scalar(@arg_bytes)-1;
  while ($j >= 0)
  {
    printf("i = %d, j = %d, arg = %d\n", $i, $j, $arg_bytes[$j]);
    $req_pkt->Put($i, $arg_bytes[$j]);
    $i++;
    $j--;
  }

  my @arg_bytes = uint32toBytes($self->{"server_ipv4_port"});
  my $i = 43;
  foreach (@arg_bytes)
  {
    $req_pkt->Put($i, $_);
    $i++;
  }

  # Print out DM Request packet for each byte/line
  for ($j = 0; $j < $length; $j++)
  {
    printf("j = %d, byte = %d\n", $j, $req_pkt->Get($j));
  }

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
