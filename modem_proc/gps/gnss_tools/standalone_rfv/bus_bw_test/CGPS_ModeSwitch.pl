# 
#$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/standalone_rfv/bus_bw_test/CGPS_ModeSwitch.pl#1 $ $DateTime: 2016/12/13 07:59:45 $ $Author: mplcsds1 $
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
# This file starts QXDM, sets up a connection to the phone, and switches CGPS
# to Standalone RF Verification mode.
# Some example functions were modified from QXDMClientConfig.pl in 
# C:\Documents and Settings\All Users\Documents\Qualcomm\QXDM\Automation Samples
#
# Usage:
# perl CGPS_ModeSwitch.pl 10
# 
#*****************************************************************************


use vars qw(%opts);
use Win32::OLE;
use Win32::OLE::Variant;
use CGPS_ModeSwitch;
use Getopt::Long;
use HelperFunctions;
#use constant EVENT_GPS_LM_ENTER_SA_RF_VERIF        => "EVENT_GPS_LM_ENTER_SA_RF_VERIF";

my %eventHash = (EVENT_GPS_LM_ENTER_SA_RF_VERIF        => 1247,
                 EVENT_GPS_LM_EXIT_SA_RF_VERIF         => 1248);


# Global variables
my $QXDM;
my $QXDM2;

# This is the PD Done Event that will be received after phone has finished sending the last PD result DM pkt.
# e constant EVENT_GPS_LM_ENTER_SA_RF_VERIF        => "EVENT_GPS_LM_ENTER_SA_RF_VERIF";
# Launch QXDM
my $RC = Initialize();

if ($RC == false)
{
  return;
}

# Get QXDM version
my $Version = $QXDM->{AppVersion};
printf "\nQXDM Version: $Version\n";

# Add items to the client
AddItems();


%opts = ();
GetOptions( \%opts, 'help');
my $port_num = int($ARGV[0]);
my $on_off_ctrl = $ARGV[1];
my $flag_sarf_Mode_switched = 0;
my $mode = 0;

&usage() if ($opts{'help'});

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
$req = new SA_RF_Verif;
$req->{"cmd"}                  = 75;
$req->{"subsys_id"}            = 13;
$req->{"sub_cmd"}              = 0x65;
$req->{"gps_cmd"}              = 20;
$req->{"version"}              = 1;

if( $on_off_ctrl eq 'ON' )
{
   printf( "\nAttempting to inject cmd to enter SARF Verfication Mode\n" );
   $req->{"enter_exit_flag"}   = 0;
   $mode = $eventHash{'EVENT_GPS_LM_ENTER_SA_RF_VERIF'};
   $mode_event = EVENT_GPS_LM_ENTER_SA_RF_VERIF;
}
elsif( $on_off_ctrl eq 'OFF' )
{
   printf( "Exiting SARF Verfication Mode\n" );
   $req->{"enter_exit_flag"}   = 1;
   $mode = $eventHash{'EVENT_GPS_LM_EXIT_SA_RF_VERIF'};
   $mode_event = EVENT_GPS_LM_EXIT_SA_RF_VERIF;
}


# Enable logging or else the script wont receive the event.
#$QXDM->SendScript("logging on");
# Initialize the request as a Variant.
my $req_pkt = $req->SetCaptureRfVerifCmd();

# Display command request in Text window.

# Send string to stdout before sending cmd. This is to reduce latency
printf("    QXDM ===>  wait_event $mode_event \n");
# Send command.
my $rsp_pkt_var = $QXDM->SendDmIcdPacketEx( $req_pkt, $timeout);
eventQXDM($mode_event, 6000);

my $rsp_len_var = length($rsp_pkt_var);

printf( "CGPS Mode-Switch Response: " );
my @results = unpack("C$rsp_len_var", $rsp_pkt_var);
for( $i=0; $i<$rsp_len_var; $i++ )
{
	printf( "%d ", $results[$i] );
}
printf( "\n\n" );

if( ($rsp_len_var == 6) && ($results[2] == 0x65) &&
	($results[3] == 0) && ($results[4] == 20) )
{
	printf( "CGPS mode-switch cmd accepted!\n" );
#	eventQXDM(EVENT_GPS_LM_ENTER_SA_RF_VERIF, 6000);
}
else
{
	printf( "CGPS mode-switch cmd rejected!\n" );
}
if($flag_sarf_Mode_switched == 1)
{
      # Good news. Received event! Spread the joy.
      if( $mode == $eventHash{'EVENT_GPS_LM_ENTER_SA_RF_VERIF'} )
      {
          printf( "SARF mode entered!\n");
      }
      else
      {
          printf( "Exited SARF mode!\n");
      }
}
else
{
      printf( "Event $mode not received, timeOut.\n");
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

GPS Standalone RF Verification Mode Switch

Syntax:   perl CGPS_ModeSwitch.pl <Options>

Options:  -help               Display this help message.
          Option 1:           <QPST Connection Port Number>
          Option 2:           <ON/OFF> -- When this option is ON, the phone enters SARF Verification Mode
                                          When this is OFF, the phone exits the SARF Verification mode.

 
Example:  perl CGPS_ModeSwitch.pl 10 ON
          perl CGPS_ModeSwitch.pl 10 OFF

END

   print "$usage";
   exit (0);
}

###############################################################
# Subroutine to wait for an event for a specified timeout
################################################################
sub eventQXDM {
   my($timeEvent, $timeOut) = @_;

   $_ = $timeEvent;
   return if ($configAttributes{"offline"});

   unless(exists $eventHash{$timeEvent}) {
      printf( "Invalid event <$timeEvent>\n");
      return;
   }
   #else
   my $rspPkt = $QXDM->WaitEvent($eventHash{$timeEvent}, $timeOut);
   my $rspLen = length($rspPkt);

#print "rspPkt $rspPkt $rspLen\n";

   if ($rspLen <= 0)
   {
      return 0;
   }
   else 
   {
      $flag_sarf_Mode_switched = 1;
      return;
   }
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
#  use constant EVENT_GPS_LM_ENTER_SA_RF_VERIF = 1247;
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

# Register for ENTER and EXIT events
my ($key, $event_val) = each(%eventHash);
   $Client->AddEvent( $event_val );
my ($key, $event_val) = each(%eventHash);
   $Client->AddEvent( $event_val );

   $Client->CommitConfig();

}

