######################################################################
# $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/pdapi_scripts/pd_ExternCoarsePosInj.pm#1 $
# $DateTime: 2016/12/13 07:59:45 $
# $Author: mplcsds1 $
#
# Define class in Perl to inject time information.
#
######################################################################

package pd_ExternCoarsePosInj;

use Win32::OLE;
use Win32::OLE::Variant;

my %fields = (

  cmd_code          => undef, # (1 byte)
  subsys_id         => undef, # (1 byte)
  subsys_cmd_code   => undef, # (2 bytes)
  cgps_cmd_code     => undef, # (1 byte)
  version           => undef, # (1 byte)
  unused            => undef, # (2 bytes)
  client_id         => undef, # (4 bytes)	
  opt_field_mask    => undef, # (4 bytes)
  time_info_type    => undef, # (4 bytes)
  timestamp_sec     => undef, # (4 bytes)
  alt_def           => undef, # (4 bytes)
  source            => undef, # (4 bytes)
  latitude          => undef, # (4 bytes)
  longitude         => undef, # (4 bytes)
  loc_unc_hor       => undef, # (4 bytes)
  altitude			=> undef, # (2 bytes)
  loc_unc_vert		=> undef, # (2 bytes)
  confidence_hor	=> undef, # (1 bytes)
  confidence_vert	=> undef, # (1 bytes)
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
#   Initialize DM CGPS_INJECT_TIME_CMD capture request packet
#
# RETURN VALUE:
#   Request packet
#
######################################################################
sub SetCaptureCmd
{
  my $self = shift;

  my $length = 50;
  my $type = ref($self) || die "$self is not an object";

  my $req_pkt = Variant(VT_ARRAY | VT_UI1, $length);

  $req_pkt->Put(0,  $self->{"cmd_code"});
  $req_pkt->Put(1,  $self->{"subsys_id"});
  $req_pkt->Put(2,  $self->{"subsys_cmd_code"} & 0xFF);
  $req_pkt->Put(3, ($self->{"subsys_cmd_code"} >> 8) & 0xFF);
  $req_pkt->Put(4,  $self->{"cgps_cmd_code"});
  $req_pkt->Put(5,  $self->{"version"});
  $req_pkt->Put(6, 0);
  $req_pkt->Put(7, 0);

  my @argsTopass = uint32toBytes($self->{"client_id"});
  my $i = 8;
  foreach (@argsTopass)
  {
    $req_pkt->Put($i, $_);
    $i++;
  }

  my @argsTopass = uint32toBytes($self->{"opt_field_mask"});
  my $i = 12;
  foreach (@argsTopass)
  {
    $req_pkt->Put($i, $_);
    $i++;
  }


  my @argsTopass = uint32toBytes($self->{"time_info_type"});
  my $i = 16;
  foreach (@argsTopass)
  {
    $req_pkt->Put($i, $_);
    $i++;
  }

  my @argsTopass = uint32toBytes($self->{"timestamp_sec"});
  my $i = 20;
  foreach (@argsTopass)
  {
    $req_pkt->Put($i, $_);
    $i++;
  }

  my @argsTopass = uint32toBytes($self->{"alt_def"});
  my $i = 24;
  foreach (@argsTopass)
  {
    $req_pkt->Put($i, $_);
    $i++;
  }

  my @argsTopass = uint32toBytes($self->{"source"});
  my $i = 28;
  foreach (@argsTopass)
  {
    $req_pkt->Put($i, $_);
    $i++;
  }

  my @argsTopass = uint32toBytes($self->{"latitude"});
  my $i = 32;
  foreach (@argsTopass)
  {
    $req_pkt->Put($i, $_);
    $i++;
  }

  my @argsTopass = uint32toBytes($self->{"longitude"});
  my $i = 36;
  foreach (@argsTopass)
  {
    $req_pkt->Put($i, $_);
    $i++;
  }

  my @argsTopass = uint32toBytes($self->{"loc_unc_hor"});
  my $i = 40;
  foreach (@argsTopass)
  {
    $req_pkt->Put($i, $_);
    $i++;
  }

  $req_pkt->Put(44, $self->{"altitude"});
  $req_pkt->Put(45, ($self->{"altitude"} >> 8) & 0xFF);
  $req_pkt->Put(46, $self->{"loc_unc_vert"});
  $req_pkt->Put(47, ($self->{"loc_unc_vert"} >> 8) & 0xFF);
  $req_pkt->Put(48, $self->{"confidence_hor"});
  $req_pkt->Put(49, $self->{"confidence_vert"});

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

  printf main::LOGFILE  ("cgps_cmd_code = %d, client_id = %d, status = %d\n", $results[3], $results[5], $results[6]);
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
