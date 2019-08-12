######################################################################
#$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/pdapi_scripts/pd_NotifyVerify.pm#1 $
# $DateTime: 2016/12/13 07:59:45 $
# $Author: mplcsds1 $
#
# Define class in Perl to copy display to EFS.
#
######################################################################

package pd_NotifyVerify;

use Win32::OLE;
use Win32::OLE::Variant;

# CGPS_NTFY_VRFY_SND_USR_ACTN_CMD request packet field. Or response??
my %fields = (
  cmd_code          => undef, # (1 byte)
  subsys_id         => undef, # (1 byte)
  subsys_cmd_code   => undef, # (2 bytes)
  cgps_cmd_code     => undef, # (1 byte)
  version           => undef, # (1 byte)
  accept_deny       => undef, # (1 byte)
  client_id         => undef, # (4 bytes)
  cmd_type          => undef, # (1 byte)
  resp_data_len     => undef, # (2 bytes)

  # SUPL-related data
  notif_priv_type   => undef, # (1 byte)
  lcs_flags         => undef, # (2 bytes)
  presence          => undef, # (1 byte)
  session_id_0      => undef, # (1 byte)
  session_id_1      => undef, # (1 byte)
  session_id_2      => undef, # (1 byte)
  session_id_3      => undef, # (1 byte)
  slp_addr_type     => undef, # (1 byte)
  url_len           => undef, # (1 byte)
  url               => undef, # (1-255 bytes)
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
#   Initialize the DM CGPS_NTFY_VRFY_SND_USR_ACTN_CMD capture request
#   packet
#
# RETURN VALUE:
#   Request packet
#
######################################################################
sub SetCaptureCmd
{
  my $self = shift;

  my $length = 24 + $self->{"url_len"};
  my $type = ref($self) || die "$self is not an object";

  my $req_pkt = Variant(VT_ARRAY | VT_UI1, $length);

  $req_pkt->Put(0,  $self->{"cmd_code"});
  $req_pkt->Put(1,  $self->{"subsys_id"});
  $req_pkt->Put(2,  $self->{"subsys_cmd_code"} & 0xFF);
  $req_pkt->Put(3, ($self->{"subsys_cmd_code"} >> 8) & 0xFF);
  $req_pkt->Put(4,  $self->{"cgps_cmd_code"});
  $req_pkt->Put(5,  $self->{"version"});
  $req_pkt->Put(6,  $self->{"accept_deny"});

  my @arg_bytes = uint32toBytes($self->{"client_id"});
  my $i = 7;
  foreach (@arg_bytes)
  {
    $req_pkt->Put($i, $_);
    $i++;
  }

  $req_pkt->Put(11,  $self->{"cmd_type"});
  $req_pkt->Put(12,  $self->{"resp_data_len"} & 0xFF);
  $req_pkt->Put(13, ($self->{"resp_data_len"} >> 8) & 0xFF);

  # SUPL-related data

  $req_pkt->Put(14,  $self->{"notif_priv_type"});
  $req_pkt->Put(15,  $self->{"lcs_flags"} & 0xFF);
  $req_pkt->Put(16, ($self->{"lcs_flags"} >> 8) & 0xFF);
  $req_pkt->Put(17,  $self->{"presence"});
  $req_pkt->Put(18,  $self->{"session_id_0"});
  $req_pkt->Put(19,  $self->{"session_id_1"});
  $req_pkt->Put(20,  $self->{"session_id_2"});
  $req_pkt->Put(21,  $self->{"session_id_3"});
  $req_pkt->Put(22,  $self->{"slp_addr_type"});
  $req_pkt->Put(23,  $self->{"url_len"});

  if ($self->{"url_len"} > 0)
  {
    my @arg_bytes = unpack("U*", $self->{"url"});
    my $i = 24;
    foreach (@arg_bytes)
    {
      $req_pkt->Put($i, $_);
      $i++;
    }
  }
  else
  {
    $req_pkt->Put(23, 0);
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
