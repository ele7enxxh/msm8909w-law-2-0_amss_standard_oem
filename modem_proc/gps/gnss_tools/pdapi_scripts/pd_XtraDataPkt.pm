######################################################################
# $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/pdapi_scripts/pd_XtraDataPkt.pm#1 $
# $DateTime: 2016/12/13 07:59:45 $
# $Author: mplcsds1 $
#
# Define class in Perl to inject time information.
#
######################################################################

#use strict;

package pd_XtraDataPkt;

use Win32::OLE;
use Win32::OLE::Variant;

my %fields = (

  ########diag pkt subsys header_type########
  cmd_code          => undef, # (1 byte) Command Code
  subsys_id         => undef, # (1 byte) Subsystem Id
  subsys_cmd_code   => undef, # (2 bytes) Subsystem Command Code
  cgps_cmd_code     => undef, # (1 byte) CGPS Command Code
  version           => undef, # (1 byte) Request Message Version Number
  client_id         => undef, # (4 bytes)
  
  ########Diag pkt Body########  
  file_size         => undef, # (4 bytes)
  data_part_number  => undef, # (4 bytes) data part number
  data_total_parts  => undef, # (4 byte)  total data parts
  xtra_data_len     => undef, # (4 bytes) Length of XTRA data
  xtra_data_payload => undef, # (2018 C_GPSDIAG_XTRA_DATA_MAX_PAYLOAD bytes) Length of XTRA data   
  client_data_ptr   => undef, # (4 bytes) 1
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
   my($byte1, $byte2, $byte3, $byte4);

   # printf ("%d", $arg);
    
   $byte1 = $arg & 0xFF; $arg = $arg >> 8;
   $byte2 = $arg & 0xFF; $arg = $arg >> 8;
   $byte3 = $arg & 0xFF; $arg = $arg >> 8;
   $byte4 = $arg & 0xFF;

   #printf ("  XTRA convert %d %d %d %d\n", $byte1, $byte2, $byte3, $byte4);

   return($byte1, $byte2, $byte3, $byte4);

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

  my $length = $main::MAXSEG_SIZE + $main::DIAGHDR_SIZE;
  my $type = ref($self) || die "$self is not an object";

  my $req_pkt = Variant(VT_ARRAY | VT_UI1, $length);

  $req_pkt->Put(0,  $self->{"cmd_code"});
  $req_pkt->Put(1,  $self->{"subsys_id"});
  $req_pkt->Put(2,  $self->{"subsys_cmd_code"} & 0xFF);
  $req_pkt->Put(3, ($self->{"subsys_cmd_code"} >> 8) & 0xFF);
  $req_pkt->Put(4,  $self->{"cgps_cmd_code"});
  $req_pkt->Put(5,  $self->{"version"});

  my @argsTopass = uint32toBytes($self->{"client_id"});
  
  #printf main::LOGFILE ("Setcapturecmd  Client = %d  args:%s\n", $self->{"client_id"},$_,0);  
  
  my $i = 6;
  foreach (@argsTopass)
  {
    $req_pkt->Put($i, $_);
    #printf main::LOGFILE ("Setcapturecmd  1 = %d, 2 = %d \n", $i, $_,0);
    $i++;
  }
  
  
  my @argsTopass = uint32toBytes($self->{"file_size"});
  my $i = 10;
  foreach (@argsTopass)
  {
    $req_pkt->Put($i, $_);
    $i++;
  }

  my @argsTopass = uint32toBytes($self->{"data_part_number"});
  my $i = 14;
  foreach (@argsTopass)
  {
    $req_pkt->Put($i, $_);
    $i++;
  }

  my @argsTopass = uint32toBytes($self->{"data_total_parts"});
  my $i = 18;
  foreach (@argsTopass)
  {
    $req_pkt->Put($i, $_);
    $i++;
  }

  my @argsTopass = uint32toBytes($self->{"xtra_data_len"});
  my $i = 22;
  foreach (@argsTopass)
  {
    $req_pkt->Put($i, $_);
    $i++;
  }

  #printf main::LOGFILE ("\n***** Start of Payload parsing = %d",i);    
  my @argsTopass = uint32toBytes($self->{"xtra_data_payload"});    
  my $i = 26;
  
  foreach (split(//, $self->{"xtra_data_payload"})) 
  {
     $req_pkt->Put($i, ord($_));
     $i++;
     #printf main::LOGFILE ("%d ",ord($_));       
  }  
    
  #printf main::LOGFILE ("\n***** Endof Payload parsing = %d",i);    

  
  my @argsTopass = uint32toBytes($self->{"client_data_ptr"});
  my $i =  $main::MAXSEG_SIZE + $main::DIAGHDR_SIZE - 4;
  foreach (@argsTopass)
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
  my (@results) = unpack("ccScLL", $data);

  $self->{"cmd_code"}        = $results[0];
  $self->{"subsys_id"}       = $results[1];
  $self->{"subsys_cmd_code"} = $results[2];

  $self->{"cgps_cmd_code"}   = $results[4];
  $self->{"version"}         = $results[5];  
  $self->{"client_id"}       = $results[9];

  $self->{"q_Status"}        = $results[6];
  $self->{"xtra_data_len"}   = $results[7];
  $self->{"part_number"}     = $results[8];
  $self->{"total_parts"}     = $results[9];
  
  printf main::LOGFILE ("cgps_cmd_code = %d, client_id = %d, status = %d\n", $results[9], $results[13], $results[17] );
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
