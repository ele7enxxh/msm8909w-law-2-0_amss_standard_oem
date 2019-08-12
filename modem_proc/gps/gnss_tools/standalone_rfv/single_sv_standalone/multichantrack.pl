use vars qw(%opts);
use RFDevMultiChanTrack;
use Win32::OLE;
use Win32::OLE::Variant;
use Getopt::Long;

####################################################
# METHOD:
#   usage
#
# DESCRIPTION:
#   Display usage information.
#
# RETURN VALUE:
#   None
####################################################
sub usage
{
   my $usage = <<END;


Syntax:   perl MultiChanTrack.pl [options]

Options:  -h               Display this help message
          
          OPTION 1:
          COM Port Number
          
          OPTION 2:
          0  MultiChan_Track_PRI_GLO_HBW_U
          1  MultiChan_Track_PRI_GLO_RxD
          2  MultiChan_Track_PRI_GLO
          3  MultiChan_Track_PRI_RxD_Linked
          4  MultiChan_Track_PRI
          5  MultiChan_Track_GLO
          6  MultiChan_Track_SBAS
          7  MultiChan_GPS_DPO
          8  MultiChan_Track_RxD
          9  MultiChan_Track_PRI_HBW_U
          10 MultiChan_Track_PRI_HBW_NU4
          11 MultiChan_Track_PRI_HBW_NU8
          12 MultiChan_Track_BDS
          13 MultiChan_Track_PRI_BDS
          14 MultiChan_Track_PRI_GLO_BDS
          15 MultiChan_Track_PRI_GLO_BDS_GAL
          16 MultiChan_Track_GAL
          17 MultiChan_Track_PRI_GAL
          18 MultiChan_Track_PRI_BDS_GAL
          19 MultiChan_Track_PRI_GLO_GAL
          
          
          OPTION 3:
          GPS SV ID
          
          OPTION 4:
          GLO Frequency ID
          
          OPTION 5:
          BDS SV ID

          OPTION 6:
          GAL SV ID

          OPTION 7:
          [start|stop]

          OPTION 8:
          [clear|noclear]

Example:  perl multichantrack.pl 18 15 1 0 1 1 start clear

END

   print "$usage";
   exit (0);
}

if (($ARGV[0] eq '-h') ||
    ($ARGV[0] eq '-help')
   )
{
  &usage();
  exit(0);
}

my $qxdm_app = new Win32::OLE 'QXDM.Application';
if(!$qxdm_app)
{
   print "ERROR: Unable to invoke the QXDM application.\n";
   exit(0);
}

# Specify QXDM COM port 
my $port_num = int($ARGV[0]);
my $test_mode = $ARGV[1];
my $gpsSvId = $ARGV[2];
my $gloFqId = $ARGV[3];
my $bdsSvId = $ARGV[4];
my $galSvId = $ARGV[5];
my $dm_cmd = $ARGV[6];
my $dm_opt = $ARGV[7];
my $cmd = 0;

# Sanity check on user input
if (($dm_cmd ne 'start') &&
    ($dm_cmd ne 'START') && 
    ($dm_cmd ne 'stop') &&
    ($dm_cmd ne 'STOP') &&
    ($dm_opt ne 'clear') &&
    ($dm_opt ne 'CLEAR') &&
    ($dm_opt ne 'noclear') &&
    ($dm_opt ne 'NOCLEAR')
   )
{
  print "Invalid input entered!\n";
  &usage();
  exit(0);
}

chomp($dm_cmd);
chomp($dm_opt);

# Process User input
if (($dm_cmd eq 'start')&&($dm_opt eq 'clear'))
{
  $cmd = 0;
  #$cmd = 1;
}
elsif (($dm_cmd eq 'start')&&($dm_opt eq 'noclear'))
{
  $cmd = 1;
  #$cmd = 2;
}
elsif (($dm_cmd eq 'stop')&&($dm_opt eq 'clear'))
{
  $cmd = 2;
  #$cmd = 3;
}
elsif (($dm_cmd eq 'stop')&&($dm_opt eq 'noclear'))
{
  $cmd = 3;
  #$cmd = 4;
}
else
{
  print "Invalid user input.\n";
  &usage();
  exit(0);
}
  
$qxdm_app->{COMPort} = $port_num;

# Wait until phone is connected
while( $qxdm_app->{IsPhoneConnected} == 0 ){
   print "Unable to Connect to the Phone. Retrying.\n";
   sleep(1);
}

# Set the timeout to 2 seconds.
my $timeout = 2000; # Timeout for receiving ACK for injected pkt

printf("Sending RF Dev Multi-Chan tracking cmd \n");
$test_ctrl = 1;
$req = new RFDev_MultiChanTrack;
$req->{"cmd"}                  = 75;
$req->{"subsys_id"}            = 13;
$req->{"sub_cmd"}              = 0x65;
$req->{"gps_cmd"}              = 27;
$req->{"version"}              = 1;

$req->{"test_mode"}            = int($test_mode); 
$req->{"glo_hw_chan_num"}      = 0;
$req->{"gps_sbas_sv_id"}       = int($gpsSvId);
$req->{"glo_freq_id"}          = int($gloFqId);
$req->{"gps_sbas_bitedge_setting"} = 1;
$req->{"glo_bitedge_setting"}  = 1;
$req->{"command"}              = int($cmd);
$req->{"bds_sv_id"}            = int($bdsSvId);
$req->{"bds_bitedge_setting"}  = 1;
$req->{"gal_sv_id"}            = int($galSvId);
$req->{"gal_bitedge_setting"}  = 1;

# Enable the logs that we are interested in

# Initialize the request as a Variant.
my $req_pkt = $req->SetCaptureRFDev_MultiChanTrack();

# Send command.
my $rsp_pkt_var = $qxdm_app->SendDmIcdPacketEx( $req_pkt, $timeout);
my $rsp_len_var = length($rsp_pkt_var);

printf( "Standalone/Single-SV Response: " );
my @results = unpack("C$rsp_len_var", $rsp_pkt_var);
for( $i=0; $i<$rsp_len_var; $i++ )
{
        printf( "%d ", $results[$i] );
}
printf( "\n\n" );
