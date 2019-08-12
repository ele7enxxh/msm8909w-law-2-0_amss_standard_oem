use vars qw(%opts);

use Win32::OLE;
use Win32::OLE::Variant;
use PD_FILE_DLOAD;
use Getopt::Long;

%opts = ();
GetOptions( \%opts, 'help');

&usage() if ($opts{'help'});
my $qxdm_app = new Win32::OLE 'QXDM.Application';
if(!$qxdm_app)
{
   print "ERROR: Unable to invoke the QXDM application.\n";
   die;
}

$comport     = $ARGV[1];
# Specify QXDM COM port 
$qxdm_app->{COMPort} = $comport;

# Wait until phone is connected
while( $qxdm_app->{IsPhoneConnected} == 0 ){
   print "Unable to Connect to the Phone. Retrying.\n";
   sleep(1);
}

# Create QXDM2 interface
$QXDM2 = $qxdm_app->GetIQXDM2();
if ($QXDM2 == null)
{
  print "\nQXDM does not support required interface\n";
  die;
}
# Register a QXDM client
#my $Handle = $QXDM2->RegisterQueueClient( 256 );
my $Handle = $QXDM2->RegisterClient( "Filtered", 1 );
if ($Handle == 0xFFFFFFFF)
{
  print "\nUnable to register a client\n";
  die;
}

my $Client = $QXDM2->ConfigureClientByKeys( $Handle );
if ($Client == false)
{
  print "Unable to configure client by keys\n";
  $QXDM2->UnregisterClient( $Handle );
  die;
}

# Register for subsystem dispatch response
# "CGPS EFS FILE Handling Immediate/Delayed Response"
$Client->AddSubsysResponse( 13, 105 );
$Client->SetSubsysV2DelayedResponsesOnly(1);
$Client->CommitConfig();
   
my $timeout = 2000;
#################################################
# Open File for reading
#################################################
open(FILE, $ARGV[0]) || die "Cannot open file";
if ($ARGV[0] !~ /txt$/)
{
  printf("Opening File in Bin Mode\n");
  binmode FILE;
}
else
{
  printf("Opening File in Text Mode\n");
}
$FileData = "";
$FileNameAndData = "";
$PathName = "/CGPS_PE/"; 
$FileNameAndData = join "", $FileNameAndData, $PathName;
$FileNameAndData = join "", $FileNameAndData, $ARGV[0];
$FileNameLength = length($FileNameAndData);
printf("FileNameLength %d\n", $FileNameLength);
$FileLen = 0;
while($line = <FILE>)
{
  foreach (split(//, $line)) 
  {
    $FileLen++;
  }
  $FileData = join "", $FileData, $line;
}
printf("Size of file %d\n", $FileLen);

$FileNameAndData = join "", $FileNameAndData, $FileData;

#print $FileNameAndData;
# Build a diagnostic request for FILE_DLOAD 
$req = new PD_FILE_DLOAD;
$req->{"cmd"}                  = 128; # DIAG_SUBSYS_CDM_VER_2_F
$req->{"subsys_id"}            = 13;
$req->{"sub_cmd"}              = 105;
$req->{"fileoperation"}        = 0; # 0 - Write; 1 - Append; 2 - Delete; 3 - Read 
$req->{"filenamelength"}       = $FileNameLength; 
$req->{"reserved"}             = 0;
$req->{"filesize"}             = $FileLen;
$req->{"filepayload"}          = $FileNameAndData;

# Initialize the request as a Variant.
my $req_pkt = $req->SetCaptureCmd();
$pkt_len = 14 + length($req->{"filepayload"});
for($i = 0; $i < $pkt_len; $i++)
{
  #printf("%x\n", $req_pkt->Get($i));
}

$StartIndex = $QXDM2->GetItemCount() - 1; # Get index of last item in client
# Send command.
$qxdm_app->SendDmIcdPacketEx( $req_pkt, $timeout);

$EndIndex = $QXDM2->GetItemCount() - 1; # Get index of last item in client
print "\n Start: $StartIndex, End: $EndIndex";
my $Got_Del_Resp = 0;
while( $Got_Del_Resp == 0 )
{
  # Process all new items
  for ( my $i2 = $StartIndex; $i2 <= $EndIndex; $i2++ )
  {
    my $Item = $QXDM2->GetItem( $i2 ); # Get Item
    if ($Item == null)
    {
      print "\nUnable to retrieve client item\n";
      $QXDM2->UnregisterClient( $Handle );
      die;
    }  
    my $type = $Item->GetItemType();
	my $name = $Item->GetItemName();

	# $type = 9 ensures that it is a subsytem dispatch response
    if ( ($type == 9) && ($name eq "CGPS EFS File Handling Delayed Response"))
#    if ( ($type == 9) )
	{
      print "\n $name: $type\n";
	  if($Got_Del_Resp == 0)
	  {
	    print "Got Imm Rsp\n";
	    $imm_response = $Item->GetItemBuffer(1);
        $Got_Del_Resp = 1;
	  }
	  else
	  {
	    print "Got Delayed Rsp\n";
	    $del_response = $Item->GetItemBuffer(1);
	  }
	}
  }
  $StartIndex = $EndIndex + 1;
  $EndIndex = $QXDM2->GetItemCount() - 1; # Get index of last item in client
}


my $imm_rsp_len = length($imm_response);
my @imm_rsp_results = unpack("C$imm_rsp_len", $imm_response);
print @imm_rsp_results;

my $del_rsp_len = length($del_response);
my @del_rsp_results = unpack("C$del_rsp_len", $del_response);
print @del_rsp_results;

$QXDM2->UnregisterClient( $Handle );
$qxdm_app = NULL;

# Display command request in Text window.

# METHOD:
#   usage
#
# DESCRIPTION:
#   Display usage information.
#
# RETURN VALUE:
#   None
#
sub usage
{
   my $usage = <<END;

HSDisplayToEFS

Syntax:   Dwell [options]

Options:  -help               Display this help message.
 
Example:  Dwell

END

   print "$usage";
   exit (0);
}
