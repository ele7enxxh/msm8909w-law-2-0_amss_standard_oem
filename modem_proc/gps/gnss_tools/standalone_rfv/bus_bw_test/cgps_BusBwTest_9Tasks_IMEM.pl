# 
#$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/standalone_rfv/bus_bw_test/cgps_BusBwTest_9Tasks_IMEM.pl#1 $ $DateTime: 2016/12/13 07:59:45 $ $Author: mplcsds1 $
#*****************************************************************************
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
# *****************************************************************************
# Description:
# This file starts QXDM, sets up a connection to the phone, and starts the appropriate Bus BW
# test. Use cmd line option to stop the test. Observe output on F3 window in QXDM
# Some example functions were modified from QXDMClientConfig.pl in 
# C:\Documents and Settings\All Users\Documents\Qualcomm\QXDM\Automation Samples
#
# Usage:perl cgps_BusBwTest.pl  7 start
# perl cgps_BusBwTest.pl 7 stop
# 
#*****************************************************************************

use vars qw(%opts);
use Win32::OLE;
use Win32::OLE::Variant;
use cgps_BusBwTest;
use Getopt::Long;
use HelperFunctions;

# Global variables
my $QXDM;
my $QXDM2;

my $task_cnt = 9;


GetOptions(
'help', \$help); 

&usage() if ($help);
my $port_num = int($ARGV[0]);
my $start_stop_cntrl = $ARGV[1];

my $task_cnt = int $task_cnt;

# Launch QXDM
my $RC = Initialize();
if ($RC == false)
{
  return;
}
if( $start_stop_cntrl eq 'start' )
{
   $QXDM->LoadConfig(".\\QXDM_F3_Config.dmc");
}

# Get QXDM version78.2299
my $Version = $QXDM->{AppVersion};
printf "\nQXDM Version: $Version\n";

# Add items to the client
AddItems();
# Specify QXDM COM port 
$QXDM->{COMPort} = $port_num;

# Wait until phone is connected
while( $QXDM->{IsPhoneConnected} == 0 ){
   print "Unable to Connect to the Phone. Retrying.\n";
   sleep(1);
}

# Set the timeout to 2 seconds.
my $timeout = 2000; # Timeout for receiving ACK for injected presc dwell

# Build a diagnostic request for DWELL_CMD 
$req = new bus_bw_test;
$req->{"cmd"}                  = 75;
$req->{"subsys_id"}            = 13;
$req->{"sub_cmd"}              = 0x65;
$req->{"gps_cmd"}              = 24;
$req->{"version"}              = 1;
$req->{"task_cnt"}             = $task_cnt;
$req->{"integ_length"}         = 1;# 1-1023 0-EMEM 1-IMEM

if( $start_stop_cntrl eq 'start' )
{
   printf( "\nAttempting to inject cmd to start Bus Bandwidth Test\n" );
   $req->{"start_stop_flag"}   = 0;
}
elsif( $start_stop_cntrl eq 'stop' )
{
   printf( "Stopping Bus BW Test\n" );
   $req->{"start_stop_flag"}   = 1;
}
else
{
   printf ("Incorrect TEST FLAG. Use perl cgps_BusBwTest.pl -h for help\n");
   exit();
}

if( $start_stop_cntrl eq 'start' )
{
          if( $ARGV[2] eq 'gpsbw' )
          {
                $req->{"test_type"}            = 0; # 0=>Gps Bw; 1=>Max DMA BW; 2=>Max SYS BW
          }
          elsif( $ARGV[2] eq 'maxdma' )
          {
                $req->{"test_type"}            = 1; # 0=>Gps Bw; 1=>Max DMA BW; 2=>Max SYS BW
          }
          elsif( $ARGV[2] eq 'maxsys' )
          {
                $req->{"test_type"}            = 2; # 0=>Gps Bw; 1=>Max DMA BW; 2=>Max SYS BW
          }
          elsif( $ARGV[2] eq 'gpsbw_ubl' )
          {
                $req->{"test_type"}            = 3; # 0=>Gps Bw; 1=>Max DMA BW; 2=>Max SYS BW
          }          
          else
          {
               printf ("Incorrect TEST TYPE. Use perl cgps_BusBwTest.pl -h for help\n");
               exit();
          }
}

# Initialize the request as a Variant.
my $req_pkt = $req->SetCaptureBusBwCmd();

# Send command.
my $rsp_pkt_var = $QXDM->SendDmIcdPacketEx( $req_pkt, $timeout);

my $rsp_len_var = length($rsp_pkt_var);

printf( "Bus BW cmd response: " );
my @results = unpack("C$rsp_len_var", $rsp_pkt_var);
for( $i=0; $i<$rsp_len_var; $i++ )
{
	printf( "%d ", $results[$i] );
}
printf( "\n\n" );

if( ($rsp_len_var == 6) && ($results[2] == 0x65) &&
	($results[3] == 0) && ($results[4] == 24) )
{
	printf( "CGPS Bus BW Test Cmd accepted!\n" );
}
else
{
	printf( "CGPS Bus BW Test Cmd rejected!\n" );
}

$QXDM = NULL;
$QXDM2->UnregisterClient( $Handle );

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

GPS Bus-Bandwidth Command script.
Put CGPS in SARF mode prior to starting this test. 
For this use ..\CGPS_ModeSwitch\CGPS_ModeSwitch.pl

Syntax:   perl cgps_BusBwTest.pl <COM PORT> <START/STOP> <TEST TYPE>

          COM PORT:         QPST Connection Port Number
          START/STOP:       (start/stop) Start or stop the test.
          TEST TYPE:        (gpsbw/maxdma/maxsys/gpsbw_ubl) Three supported test types.

Options:  -help             Display this help message.

Example:  perl cgps_BusBwTest.pl 10 start maxdma
          perl cgps_BusBwTest.pl 10 stop

END

   print "$usage";
   exit (0);
}

# Initialize application
sub Initialize
{
   # Assume failure
   my $RC = false;

   # Create QXDM object
   $QXDM = new Win32::OLE 'QXDM.Application';
   if ($QXDM == null)
   {
      print "\nError launching QXDM";

      return $RC;
   }
   # Create QXDM2 interface
   $QXDM2 = $QXDM->GetIQXDM2();
   if ($QXDM2 == null)
   {
      print "\nQXDM does not support required interface";

      return $RC;
   }

   SetQXDM ( $QXDM );
   SetQXDM2 ( $QXDM2 );

   # Success
   $RC = true;
   return $RC;
}

# Add items to the client
sub AddItems
{
   use constant MSG_ID_AAGPS          => 47;
   # Get a QXDM client
   my $Handle = $QXDM2->RegisterQueueClient( 256 );
   if ($Handle == 0xFFFFFFFF)
   {
      print "\nUnable to register a client\n";

      return;
   }

   my $Client = $QXDM2->ConfigureClientByKeys( $Handle );
   if ($Client == false)
   {
      print "Unable to configure client by keys\n";

      $QXDM2->UnregisterClient( $Handle );
      return;
   }

  # Register for debug messages
  $Client->AddMessage( 47, 2 );
  $Client->AddMessage( 47, 16 );
  $Client->CommitConfig();
}

