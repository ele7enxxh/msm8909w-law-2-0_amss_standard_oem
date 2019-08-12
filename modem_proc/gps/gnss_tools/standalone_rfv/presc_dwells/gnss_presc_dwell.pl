#----------------------------------------------------------------------------
#  Copyright (c) 2014 Qualcomm Atheros, Inc.
#  All Rights Reserved.
#  Qualcomm Atheros Confidential and Proprietary.
#
#  Copyright (c) 2015 Qualcomm Technologies, Inc. 
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#----------------------------------------------------------------------------
#

#========================================================================================
#$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/standalone_rfv/presc_dwells/gnss_presc_dwell.pl#1 $ $DateTime: 2016/12/13 07:59:45 $ $Author: mplcsds1 $
#========================================================================================
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
#========================================================================================
# Description:
# This script starts QXDM, sets up a connection to the phone, and issues a single dwell
# command. Dwell result can be viewed at command prompt if enabled in command line option. 
#
# Usage:
# perl gnss_presc_dwell.pl <Com Port Number> <0/1 Disable/Enable Command Prompt Display> <Options>
#
#========================================================================================

use vars qw(%opts);
use Cwd;
use HelperFunctions;
use Win32::OLE;
use Win32::OLE::Variant;
use Dwell;
use Getopt::Long;
use constant SECONDS_TO_WAIT        => 30;
use constant SECONDS_TO_SLEEP       => 5;
my $IQXDM;
my $IQXDM2;
my $Disp_Dwell_Result = int $ARGV[1];
my $preD;

# Default dwell params
my $gps_enable = 1;
my $gps_control_flag = 0;
my $gps_dopp_mps = 900; # m/s
my $gps_cp_win_sz_chips = 1023;
my $gps_srch_mode = 3;
my $gps_postD = 600;
my $gps_rxd_flag = 0;

my $glo_enable = 1;
my $glo_control_flag = 0;
my $glo_dopp_mps = 900; # m/s
my $glo_cp_win_sz_chips = 511;
my $glo_srch_mode = 17;
my $glo_postD = 600;

GetOptions('gps_enable=i' => \$gps_enable,
           'gps_rxd_flag=i' => \$gps_rxd_flag,
           'gps_ctrl_flag=i' => \$gps_ctrl_flag,
           'gps_dopp_mps=f' => \$gps_dopp_mps,
           'gps_cp_win=i' => \$gps_cp_win_sz_chips,
           'gps_srch_mode=i' => \$gps_srch_mode,
           'gps_postd=i' => \$gps_postD,

           'glo_enable=i' => \$glo_enable,
           'glo_ctrl_flag=i' => \$glo_ctrl_flag,
           'glo_dopp_mps=f' => \$glo_dopp_mps,
           'glo_cp_win=i' => \$glo_cp_win_sz_chips,
           'glo_srch_mode=i' => \$glo_srch_mode,
           'glo_postd=i' => \$glo_postD,

           'help', \$help); 

if($gps_enable > 0)
{
  $gps_enable = 1;
}

if($gps_rxd_flag > 2)
{
  printf "*****Error: RxD flag incorrect. Must be between 0 and 2!";
  return;
}

if($glo_enable > 0)
{
  $glo_enable = 1;
}

if($gps_cp_win_sz_chips > 1023)
{
  $gps_cp_win_sz_chips = 1023;
}

if($glo_cp_win_sz_chips > 511)
{
  $glo_cp_win_sz_chips = 511;
}

&usage() if ($help);

# This is the PD Done Event that will be received after phone has finished sending the PD result DM pkt.
use constant     EVENT_GNSS_PRESC_DWELL_UPDATE        => "EVENT_GNSS_PRESC_DWELL_UPDATE";
my %eventHash = (EVENT_GNSS_PRESC_DWELL_UPDATE        => 1717);

my %gps_event = (
                 GPS_PRESC_DWELL_NOT_APPLICABLE => 0,
                 GPS_PRESC_DWELL_REJECTED       => 1,
                 GPS_PRESC_DWELL_DONE           => 2,
                 GPS_PRESC_DWELL_TUNC_TOO_BIG   => 3,
                );

my %glo_event = (
                 GLO_PRESC_DWELL_NOT_APPLICABLE => 0,
                 GLO_PRESC_DWELL_REJECTED       => 1,
                 GLO_PRESC_DWELL_DONE           => 2,
                 GLO_PRESC_DWELL_TUNC_TOO_BIG   => 3,
                );

my %bds_event = (
                 BDS_PRESC_DWELL_NOT_APPLICABLE => 0,
                 BDS_PRESC_DWELL_REJECTED       => 1,
                 BDS_PRESC_DWELL_DONE           => 2,
                 BDS_PRESC_DWELL_TUNC_TOO_BIG   => 3,
                );

my %gal_event = (
                 GAL_PRESC_DWELL_NOT_APPLICABLE => 0,
                 GAL_PRESC_DWELL_REJECTED       => 1,
                 GAL_PRESC_DWELL_DONE           => 2,
                 GAL_PRESC_DWELL_TUNC_TOO_BIG   => 3,
                );



# Start-up the QXDM Application
&Register();

# Specify QXDM COM port 
$IQXDM->{COMPort} = int $ARGV[0];

# Wait until phone is connected
while( $IQXDM->{IsPhoneConnected} == 0 ){
   print "Unable to Connect to the Phone. Retrying.\n";
   sleep(1);
}

# Set the timeout to 2 seconds.
my $timeout = 2000; # Timeout for receiving ACK for injected presc dwell

printf("Sending Dwell Cmd \n");

# Build a diagnostic request for DWELL_CMD 
$req = new Dwell;
$req->{"cmd"}                       = 75;
$req->{"subsys_id"}                 = 13;
$req->{"sub_cmd"}                   = 0x65;
$req->{"gps_cmd"}                   = 21;
$req->{"version"}                   = 53;

#$req->{"cmd_ctrl_flag"}             = $gps_enable | ($glo_enable << 1);
$req->{"cmd_ctrl_flag"}             = 15;
                                      # Cmd Control Flag:-
                                      # BIT#  DESCRIPTION
                                      # 0     0/1 => Enable GPS Dwell cmd
                                      # 1     0/1 => Enable GLO Dwell cmd
                                      # 2     0/1 => Enable BDS Dwell cmd
                                      # 3     0/1 => Enable GAL Dwell cmd
 

$req->{"seq_num"}                   = 0;
$req->{"gps_rxd_flag"}              = $gps_rxd_flag;
                                      # ENUM  DESCRIPTION
                                      # 0     No RxD
                                      # 1     Primary + RxD Linked
                                      
$req->{"gps_ctrl_flag"}             = $gps_ctrl_flag & 0x3;
                                      # GPS Control Flag:-
                                      # BIT#  DESCRIPTION
                                      # 0     0/1 => As-prescribed/Autocenter
                                      # 1     0/1 => No on-board truncation of code
                                      #              phase center/Truncate
                                      #              code phase center on-board. 
                                      #              Applicable to autocenter
                                      #              dwells ONLY.
                                      # 2     0/1 => No on-board randomization
                                      #              of doppler/Apply randomization of
                                      #              doppler on-board. (+/- 12.5 Hz)
                                      # 3-7   Reserved

$req->{"gps_sv"}                    = 1;
$req->{"gps_dopp_mps"}               = $gps_dopp_mps;
$req->{"gps_dopp_win_hz"}           = 200;
$req->{"gps_code_phase_center_c32"} = 0;
$req->{"gps_code_phase_win_c1"}     = $gps_cp_win_sz_chips;
$req->{"gps_srch_mode"}             = $gps_srch_mode;
$req->{"gps_num_postd"}             = $gps_postD;
$req->{"gps_pfa"}                   = 4;
$req->{"gps_reserved"}              = 0; # Not used

$req->{"glo_ctrl_flag"}             = $glo_ctrl_flag & 0x3;
$req->{"glo_freq_k"}                = 8; # 1 ~ 14
$req->{"glo_dopp_mps"}               = $glo_dopp_mps;
$req->{"glo_dopp_win_hz"}           = 200;
$req->{"glo_code_phase_center_c32"} = 0;
$req->{"glo_code_phase_win_c1"}     = $glo_cp_win_sz_chips;
$req->{"glo_srch_mode"}             = $glo_srch_mode;
$req->{"glo_num_postd"}             = $glo_postD;
$req->{"glo_hw_chan"}               = 1; # 1 ~ 12
$req->{"glo_pfa"}                   = 4;
$req->{"glo_reserved"}              = 0; # Not used yet

$req->{"dwell_cnt"}                 = 0; # Not used yet
$req->{"total_num_dwells"}          = 0; # Not used yet
$req->{"pass_dwell_cnt"}            = 0; # Not used yet
$req->{"reserved"}                  = 0; # Not used yet

$req->{"bds_sv"}                    = 2;
$req->{"bds_dopp_mps"}              = 68;
$req->{"bds_dopp_win_hz"}           = 500;
$req->{"bds_code_phase_center_c40"} = 0;
$req->{"bds_code_phase_win_c1"}     = 2046;
$req->{"bds_srch_mode"}             = 31;
$req->{"bds_num_postd"}             = 80;
$req->{"bds_pfa"}                   = 4;
$req->{"bds_reserved"}              = 0; # Not used

$req->{"max_sv_dwell"}              = 0; # Enable max Sv dwell
                                         #    ** 0 for disable
                                         #    ** 1 for enable
                                         #    ** Bit0: Enable for GPS
                                         #    ** Bit1: Enable for GLO
                                         #    ** Bit2: Enable for BDS
                                         #    ** Bit3: Enable for GAL

$req->{"gal_sv"}                    = 1;
$req->{"gal_dopp_mps"}              = 0;
$req->{"gal_dopp_win_hz"}           = 250;
$req->{"gal_code_phase_center_c40"} = 100;
$req->{"gal_code_phase_win_c1"}     = 4096;
$req->{"gal_srch_mode"}             = 38;
$req->{"gal_num_postd"}             = 50;
$req->{"gal_pfa"}                   = 6;
$req->{"gal_reserved"}              = 0; # Not used

#========================================================================================
# Determine the longer of the two dwell durations. This will dictate the timeout value
# waiting for the dwell done EVENT.
#========================================================================================

if($gps_srch_mode == 3 || $gps_srch_mode == 10)
{
	$gps_preD = 20;
}
elsif($gps_srch_mode == 4)
{
	$gps_preD = 160;
}
else
{
	$gps_preD = 20;
}

if( $gps_enable == 0 )
{
	$gps_preD = 0;
}

if( $glo_enable == 0 )
{
	$glo_preD = 0;
}
else
{
	$glo_preD = 20;
}

if( $gps_preD > $glo_preD )
{
  $max_preD = $gps_preD;
}
else
{
  $max_preD = $glo_preD;
}

if( $gps_postD > $glo_postD )
{
  $max_postD = $gps_postD;
}
else
{
  $max_postD = $glo_postD;
}


# Enable the logs that we are interested in
$IQXDM->LogMaskOn(0x1374, 1);

$IQXDM->SendScript("logging on");

# Initialize the request as a Variant.
my $req_pkt = $req->SetCaptureDwellCmd();

# Send command.
my $rsp_pkt_var = $IQXDM->SendDmIcdPacketEx( $req_pkt, $timeout);
my $rsp_len_var = length($rsp_pkt_var);

printf( "Dwell DM Response: " );
my @results = unpack("C$rsp_len_var", $rsp_pkt_var);
for( $i=0; $i<$rsp_len_var; $i++ )
{
	printf( "%d ", $results[$i] );
}
printf( "\n\n" );

if( ($rsp_len_var == 6) && ($results[2] == 0x65) &&
	($results[3] == 0) && ($results[4] == 21) )
{
  printf( "Prescribed Dwell Started...\n" );
  if( $Disp_Dwell_Result eq 1 )
  {
    while (1)
    {
      # Process any items received.
      &ProcessItems();
      sleep( SECONDS_TO_SLEEP );
      $gSecondsElapsed += SECONDS_TO_SLEEP;
              
                 
      if ($gSecondsElapsed > SECONDS_TO_WAIT)
      {
        # Done waiting, quit application.
        printf "Finished waiting, %d seconds expired\n", SECONDS_TO_WAIT;
        close HANDLE;
        exit( 0 );
      }
    }
  }
  else
  {
    $timeout_ms = ( $max_preD * $max_postD );
    $timeout_ms+=2000; # Add 2 secs more before the routine times out.
    eventQXDM(EVENT_GNSS_PRESC_DWELL_UPDATE, $timeout_ms);
  }
}
else
{
	printf( "Prescribed Dwell Not Started!\n" );
}


$IQXDM = NULL;

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

Issue a single Prescribed Dwell command.

Syntax:   gnss_presc_dwell.pl COM_PORT CMD_LINE_DISPLAY [OPTIONS]

          COM_PORT:         QPST Connection Port Number
          CMD_LINE_DISPLAY: 0/1 Turn command line display OFF/ON (Not supported yet)

Options:  -help:                           Display this help message.
          -gps_rxd_flag:                   Enable/Disable RxD dwell. Default Disabled
          -gps_enable/-glo_enable:         (1/0 => Enable/Disable). Default 1/0
          -gps_ctrl_flag/-glo_ctrl_flag:   Bit0 => 0/1 As_prescribed/Autocenter
                                           (Note: Bit1 and Bit2 apply to Autocenter dwells ONLY)
                                           Bit1 => 1   Enable on-board truncation CP center
                                           Bit2 => 1   Enable on-board randomization of
                                                       doppler (+/- 12.5 Hz)
          -gps_dopp_mps/-glo_dopp_mps:       Doppler (m/s). Default 900/900 m/s
          -gps_cp_win/-glo_cp_win:         Code Phase win size (Chips). Default 1023/511
          -gps_srch_mode/-glo_srch_mode:   GPS/GLO Search Mode. Default 3/18
          -gps_postd/-glo_postd:           PostDets. Default = 600/600

Default values from perl script take effect when value is not provided.
 
Example:  perl gnss_presc_dwell.pl 25 1 -gps_enable 0 -glo_enable 1 -glo_ctrl_flag 3 -glo_postd 50
This does a presc dwell on GLO ONLY. Defaults are over-riden for "GLO control flag" and "GLO PostD".

END

   print "$usage";
   exit (0);
}


sub ProcessItems
{
   my $itemIndex = $IQXDM2->GetClientItemCount( $gHandle ) - 1;
   
   if ($gItemIndex > $itemIndex)
   {
      #Item Store has been cleared.  Resetting index.
      $gItemIndex = -1;
   }
   
   if ($itemIndex <= $gItemIndex)
   {
      my $timeOfDay = localtime( time() );
      print "Prescribed Dwell Result. Waiting... $timeOfDay\n";
      return;
   }
   
   # Process any items received.
   my $Collect_Mode = 0;
   while ($gItemIndex < $itemIndex)
   {
      $gItemIndex++;

      my $Item = $IQXDM2->GetClientItem( $gHandle, $gItemIndex );
      if (!$Item)
      {
         print "Unable to access response at index $gItemIndex\n";
         return;
      }   

	my $ItemKey = $Item->GetItemKeyText();
	if ($ItemKey ne "[0x1374]")
	{
	   next;
	}   

      my $Fields = $Item->GetConfiguredItemFields( "", false, false );
      if (!$Fields)
      {
         print "Unable to access Fields\n";
         return;
      }

	my $Resp_Type = $Fields->GetFieldValue( 1 );
	
	if($Resp_Type == 0)
	{
	  $Collect_Mode = $Resp_Type;
   	  &item_unpack($Fields);
       }
      exit( 0 );
   }
}


#function to read NV Value from phone and write to file
sub item_unpack{
  my( $Fields ) = @_;
  my $preD;
  my $index = $Fields->GetFieldCount() - 4;
  my $counter = 4;

  my $sv = $Fields->GetFieldValue( 4 );
  my $gps_rtc = $Fields->GetFieldValue( 5 );
  my $valid_peak_flag = $Fields->GetFieldValue( 6 );

# Index is decided by arrangement in QXDM DB (See DB format for 0x1374 using DBEditor.exe in bin directory of QXDM)
  my $multipath_flag = $Fields->GetFieldValue( 7 );  
  my $srch_mode = $Fields->GetFieldValue( 8 );
  my $postD = $Fields->GetFieldValue( 9 );
  my $cno_db = $Fields->GetFieldValue( 10 );
  my $code_phase = $Fields->GetFieldValue( 11 );
  my $doppler = $Fields->GetFieldValue( 12 );
  my $multipath_est = $Fields->GetFieldValue( 13 );

if($srch_mode == 0)
{
$preD = 10;
}
if($srch_mode == 1)
{
$preD = 20;
}
if($srch_mode == 2)
{
$preD = 160;
}

my $temp = $valid_peak_flag & 1;

  if(($valid_peak_flag & 1) == 1)
  {
    print "Valid Peak.\n";
    print "Rtc $gps_rtc SV $sv preD $preD PostD $postD Cno $cno_db";
    printf(" Pk %.4f Dopp %.2f m/s\n", $code_phase, $doppler/5.25);
  }
  else
  {
    print "InValid Peak.\n";
    print "Rtc $gps_rtc SV $sv preD $preD PostD $postD Cno NA Pk NA Dopp NA\n";
  }
  return;
}
# Perform startup registration.
sub Register
{
   # Start-up the QXDM Application
   $IQXDM = new Win32::OLE 'QXDM.Application';
   if (!$IQXDM)
   {
      print "ERROR: Unable to invoke the QXDM application.\n";
      die;
   }

   # Obtain the new automation interface
   $IQXDM2 = $IQXDM->GetIQXDM2();
   if (!$IQXDM2)
   {
      print "ERROR: QXDM application does not support IQXDM2 interface\n";
      die;
   }

   # We start by registering as a client
   $gHandle = $IQXDM2->RegisterClient( "Waiting for Items", 1 );
   if ($gHandle == 0xFFFFFFFF)
   {
      print "ERROR: Unable to register client view\n";
      die;
   }
   
   # Get a configuration object
   my $clientObject = $IQXDM2->ConfigureClientByKeys( $gHandle );
   if (!$clientObject)
   {
      print "Failed to get client interface pointer\n";
      die;
   }
   
   # Register for some logs
   $clientObject->AddLog( 0x1374 );
   $clientObject->CommitConfig();
}

#========================================================================================
# Subroutine to wait for an event for a specified timeout
#========================================================================================
sub eventQXDM {
   my($timeEvent, $timeOut) = @_;

   $_ = $timeEvent;
   printf("    QXDM ===>  wait_event $timeEvent \n");
   return if ($configAttributes{"offline"});

   unless(exists $eventHash{$timeEvent}) {
      printf( "Invalid event <$timeEvent>\n");
      return;
   }

   my $rspPkt = $IQXDM->WaitEvent($eventHash{$timeEvent}, $timeOut);
   my $rspLen = length($rspPkt);

   if ($rspLen <= 0)
   {
      printf( "Event <$timeEvent> not received, timeOut.\n");
      return 0;
   }
   else
   {
     # Good news. Received event! Spread the joy.
     # Unpack the binary data.
     my (@results) = unpack("c$rspLen", $rspPkt);
     printf ("Event name = $results[18], Event data = $results[19] respLen = $rspLen\n");
     printf ("Received event <$timeEvent> Length: <$rspLen> @results\n");
     
      if( $results[14] == $gps_event{GPS_PRESC_DWELL_DONE} )
      {
      		if($gps_cp_win_sz_chips == 1023)
      		{
      			printf( "Prescribed dwell status: GPS_PRESC_DWELL_DONE\n" ); #GPS_PRESC_DWELL_DONE
      		}
      		else
      		{
      			printf( "Prescribed dwell status: GPS_PRESC_DWELL_TUNC_TOO_BIG\n" ); #GPS_PRESC_DWELL_DONE      		
      		}
      }
      elsif( $results[14] == $gps_event{GPS_PRESC_DWELL_TUNC_TOO_BIG} )
      {
        printf( "Prescribed dwell status: GPS_PRESC_DWELL_TUNC_TOO_BIG\n" );
      }
      elsif( $results[14] == $gps_event{GPS_PRESC_DWELL_NOT_APPLICABLE} )
      {
        printf( "Prescribed dwell status: GPS_PRESC_DWELL_NOT_APPLICABLE\n" );
      }
      elsif( $results[14] == $gps_event{GPS_PRESC_DWELL_REJECTED} )
      {
        printf( "Prescribed dwell status: GPS_PRESC_DWELL_REJECTED\n" );
      }
      else
      {
        printf( "Prescribed dwell status: Unknown payload %u\n", $results[14] );
      }

      if( $results[15] == $glo_event{GLO_PRESC_DWELL_DONE} )
      {
        printf( "Prescribed dwell status: GLO_PRESC_DWELL_DONE\n" );
      }
      elsif( $results[15] == $glo_event{GLO_PRESC_DWELL_TUNC_TOO_BIG} )
      {
        printf( "Prescribed dwell status: GLO_PRESC_DWELL_TUNC_TOO_BIG\n" );
      }
      elsif( $results[15] == $glo_event{GLO_PRESC_DWELL_NOT_APPLICABLE} )
      {
        printf( "Prescribed dwell status: GLO_PRESC_DWELL_NOT_APPLICABLE\n" );
      }
      elsif( $results[15] == $glo_event{GLO_PRESC_DWELL_REJECTED} )
      {
        printf( "Prescribed dwell status: GLO_PRESC_DWELL_REJECTED\n" );
      }
      else
      {
        printf( "Prescribed dwell status: Unknown payload %u\n", $results[15] );
      }

      if( $results[16] == $bds_event{BDS_PRESC_DWELL_DONE} )
      {
        printf( "Prescribed dwell status: BDS_PRESC_DWELL_DONE\n" );
      }
      elsif( $results[16] == $bds_event{BDS_PRESC_DWELL_TUNC_TOO_BIG} )
      {
        printf( "Prescribed dwell status: BDS_PRESC_DWELL_TUNC_TOO_BIG\n" );
      }
      elsif( $results[16] == $bds_event{BDS_PRESC_DWELL_NOT_APPLICABLE} )
      {
        printf( "Prescribed dwell status: BDS_PRESC_DWELL_NOT_APPLICABLE\n" );
      }
      elsif( $results[16] == $bds_event{BDS_PRESC_DWELL_REJECTED} )
      {
        printf( "Prescribed dwell status: BDS_PRESC_DWELL_REJECTED\n" );
      }
      else
      {
        printf( "Prescribed dwell status: Unknown payload %u\n", $results[16] );
      }


      if( $results[17] == $gal_event{GAL_PRESC_DWELL_DONE} )
      {
        printf( "Prescribed dwell status: GAL_PRESC_DWELL_DONE\n" );
      }
      elsif( $results[17] == $gal_event{GAL_PRESC_DWELL_TUNC_TOO_BIG} )
      {
        printf( "Prescribed dwell status: GAL_PRESC_DWELL_TUNC_TOO_BIG\n" );
      }
      elsif( $results[17] == $gal_event{GAL_PRESC_DWELL_NOT_APPLICABLE} )
      {
        printf( "Prescribed dwell status: GAL_PRESC_DWELL_NOT_APPLICABLE\n" );
      }
      elsif( $results[17] == $gal_event{GAL_PRESC_DWELL_REJECTED} )
      {
        printf( "Prescribed dwell status: GAL_PRESC_DWELL_REJECTED\n" );
      }
      else
      {
        printf( "Prescribed dwell status: Unknown payload %u\n", $results[17] );
      }

      return;

   }
}

