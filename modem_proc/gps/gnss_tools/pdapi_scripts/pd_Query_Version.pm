######################################################################
# $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/pdapi_scripts/pd_Query_Version.pm#1 $
# $DateTime: 2016/12/13 07:59:45 $
# $Author: mplcsds1 $
#
# Define class in Perl to copy display to EFS.
#
######################################################################

package pd_Query_Version;

use Win32::OLE;
use Win32::OLE::Variant;

# CGPS_QUERY_VERSION request packet field. Or response??
my %fields = (
  cmd_code           => undef, # (1 byte)
  subsys_id          => undef, # (1 byte)
  subsys_cmd_code    => undef, # (2 bytes)
  cgps_cmd_code      => undef, # (1 byte)
  version            => undef, # (1 byte)
  subsystem_type     => undef, # (4 bytes)
  subcommand         => undef, # (4 byte)
  reserved           => undef, # (8 bytes)
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
  # my $type = ref($that) || die "$self is not an object";
  my $type = ref($self) || die "$self is not an object";
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
#   Initialize DM Query Version capture request packet
#
# RETURN VALUE:
#   Request packet
#
######################################################################
sub SetCaptureCmd
{
  my $self = shift;

  my $length = 22;
  my $type = ref($self) || die "$self is not an object";

  my $req_pkt = Variant(VT_ARRAY | VT_UI1, $length);

  $req_pkt->Put(0,  $self->{"cmd_code"});
  $req_pkt->Put(1,  $self->{"subsys_id"});
  $req_pkt->Put(2,  $self->{"subsys_cmd_code"} & 0xFF);
  $req_pkt->Put(3, ($self->{"subsys_cmd_code"} >> 8) & 0xFF);
  $req_pkt->Put(4,  $self->{"cgps_cmd_code"});
  $req_pkt->Put(5,  $self->{"version"});
  
  my @arg_bytes = uint32toBytes($self->{"subsystem_type"});
  my $i = 6;
  foreach (@arg_bytes)
  {
    $req_pkt->Put($i, $_);
    $i++;
  }

  my @arg_bytes = uint32toBytes($self->{"subcommand"});
  my $i = 10;
  foreach (@arg_bytes)
  {
    $req_pkt->Put($i, $_);
    $i++;
  }

  my @arg_bytes = uint32toBytes($self->{"reserved"});
  my $i = 14;
  foreach (@arg_bytes)
  {
    $req_pkt->Put($i, $_);
    $i++;
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

  # parsing with description string 
  my (@results) = unpack("CCSCCLLLLLLLLLLca*", $data);

  $self->{"cmd_code"}                = $results[0];
  $self->{"subsys_id"}               = $results[1];
  $self->{"subsys_cmd_code"}         = $results[2];
  $self->{"cgps_cmd_code"}           = $results[3];
  $self->{"version"}                 = $results[4];
  $self->{"subsystem_type"}          = $results[5];
  $self->{"subcommand"}              = $results[6];
  $self->{"status"}                  = $results[7];
  $self->{"subcommand_version"}      = $results[8];
  $self->{"major_rev"}               = $results[9];
  $self->{"minor_rev"}               = $results[10];
  $self->{"reserved_0"}                = $results[11];
  $self->{"reserved_1"}                = $results[12];
  $self->{"reserved_2"}                = $results[13];
  $self->{"reserved_3"}                = $results[14];
  $self->{"description_length"}      = $results[15];
  $self->{"description"}             = $results[16];

  #printf("cgps_cmd_code = %d, Major Rev = %d, status = %d\n", $results[3], $results[9], $results[7]);
}

######################################################################
# METHOD:
#   GetMajorRev
#
# DESCRIPTION:
#   Subroutine to retrieve Major Rev from the response packet
#
# RETURN VALUE:
#   Major Rev
#
######################################################################
sub GetMajorRev
{
  my $self = shift;
  my $type = ref($self) || die "$self is not an object";

  if ($self->{"cmd_code"} != 75)
  {
    return -1;
  }

  return $self->{"major_rev"};
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
