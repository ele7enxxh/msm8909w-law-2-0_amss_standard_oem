#========================================================================================
#$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/standalone_rfv/iq_test/scripts/gnss_rf_cmd.pl#1 $ 
#$DateTime: 2016/12/13 07:59:45 $ 
#$Author: mplcsds1 $
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
# This script starts QXDM, sets up a connection to the phone, and issues a single GNSS 
# RF control command. 
#
# Usage:
# gnss_rf_cmd.pl COM_PORT
#
#========================================================================================

use vars qw(%opts);
use Cwd;
use HelperFunctions;
use Win32::OLE;
use Win32::OLE::Variant;
use gnss_rf_cmd;
use Getopt::Long;
my $IQXDM;
my $IQXDM2;

GetOptions('help', \$help); 
&usage() if ($help);

#===========================================================================
# Create QXDM instance
#===========================================================================
my $IQXDM = new Win32::OLE 'QXDM.Application';
if(!$IQXDM)
{
   print "ERROR: Unable to invoke the QXDM application.\n";
   die;
}

#===========================================================================
# Specify QXDM COM port 
#===========================================================================
$IQXDM->{COMPort} = int $ARGV[0];

#===========================================================================
# Wait until phone is connected
#===========================================================================
while( $IQXDM->{IsPhoneConnected} == 0 ){
   print "Unable to Connect to the Phone. Port ".$ARGV[0]." Retrying.\n";
   sleep(1);
}

SetQXDM ( $QXDM );

#===========================================================================
# Timeout for receiving ACK for injected RF cmd
#===========================================================================
my $timeout = 2000; # 

printf("Sending GNSS RF Cmd \n");

$Req = new RfCmd;
$Req->{"Cmd"}                   = 75;    # Diag Cmd Code
$Req->{"SubSysId"}              = 13;    # Diag Subsystem ID
$Req->{"SubCmd"}                = 0x65;  # Diag Subsystem Cmd Code
$Req->{"GpsCmd"}                = 26;    # CGPS Cmd Code
$Req->{"Version"}               = 1;     # Version Number

$Req->{"RfMode"}                = 1;     # RF Modes available:-
                                         # 0     Ignore this field
                                         # 1     Primary NB, RxD Always OFF
                                         # 2     Primary WB, RxD Always OFF
                                         # 3     Primary WB, RxD ON. Must
                                         #       provide ON/OFF values in 
                                         #       RF_RXD_ON_TIME_SEC AND 
                                         #       RF_RXD_OFF_TIME_SEC and 
                                         #       enable the bit in CONTROL_FLAG,
                                         #       else command will not be accepted
                                         # 4     BP4 GLO Test Mode
                                         # 5~255 Reserved

$Req->{"RfLinearity"}           = 0;     # RF Linearity Modes available
                                         # (applied to Pri and RxD):-
                                         # 0       Ignore field
                                         # 1       Low Linearity
                                         # 2       High Linearity
                                         # 3       Automatic (Intelliceiver)
                                         # 4~255   Reserved

$Req->{"RfRxdOnTimeSec"}        = 0;     # On-time of RxD in seconds before
                                         # turning it ON again. To obtain
                                         # "always-on" status set OFF time to
                                         # zero.
                                         
$Req->{"RfRxdOffTimeSec"}       = 0;     # Off-time of RxD in Seconds. On-time
                                         # of RxD in seconds before turning
                                         # it ON again. 
                                         
$Req->{"Reserved"}              = 0;

#$IQXDM->SendScript("logging on");

#===========================================================================
# Initialize the request as a Variant.
#===========================================================================
my $req_pkt = $Req->SetRfCmd();

#===========================================================================
# Send command
#===========================================================================
my $rsp_pkt_var = $IQXDM->SendDmIcdPacketEx( $req_pkt, $timeout);
my $rsp_len_var = length($rsp_pkt_var);

printf( "GNSS RF Config DM Response: " );
my @results = unpack("C$rsp_len_var", $rsp_pkt_var);
for( $i=0; $i<$rsp_len_var; $i++ )
{
	printf( "%d ", $results[$i] );
}
printf( "\n\n" );

if( ($rsp_len_var == 6) && ($results[2] == 0x65) &&
	($results[3] == 0) && ($results[4] == 26) )
{
  printf( "GNSS RF Config Cmd accepted...\n" );
}
else
{
  printf( "GNSS RF Config Cmd rejected!\n" );
}


$IQXDM = NULL;

# ===========================================================================
# Functions
# ===========================================================================


#===========================================================================
# METHOD:
#   usage
#
# DESCRIPTION:
#   Display usage information.
#
# RETURN VALUE:
#   None
#
#===========================================================================
sub usage
{
   my $usage = <<END;

Issue a single GNSS RF Configuration command.

Syntax:   gnss_rf_cmd.pl COM_PORT [OPTIONS]

          COM_PORT:         QPST Connection Port Number

Options:  -help:            Display this help message.

Example:  perl gnss_rf_cmd.pl 7
Issues GNSS RF Cmd on COM port 7

END

   print "$usage";
   exit (0);
}


