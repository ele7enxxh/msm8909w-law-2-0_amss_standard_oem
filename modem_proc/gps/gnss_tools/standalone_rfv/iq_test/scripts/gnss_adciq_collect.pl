# 
#$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/standalone_rfv/iq_test/scripts/gnss_adciq_collect.pl#1 $ $DateTime: 2016/12/13 07:59:45 $ $Author: mplcsds1 $
#*****************************************************************************
#  Copyright (C) 2004 - 2011, 2015 Qualcomm, Inc.
#  Copyright (c) 2015 Qualcomm Technologies, Inc.
#  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved. 
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
#*****************************************************************************
#
# Description:
# This script is typically used by a user to modify the iqtest cmd parameters. The parameters
# are input an exe that actually drives the iqtest. For WBIQ it has a handshaking mechanism
# to recover from dropped packets. The exe returns after the isf file has been saved. This is
# then parsed by a second exe file to produce a binary file containing the configuration info
# in the header and the iq samples in the remainder of the file. Matlab scripts can analyze the
# data.
#*****************************************************************************

use vars qw(%opts);
use Cwd;
use IqTest;
use Win32::OLE;
use Win32::OLE::Variant;
use Getopt::Long;
use HelperFunctions;
use File::Spec;


%opts = ();
GetOptions( \%opts, 'help');

&usage() if ($opts{'help'});
my $port_num = int($ARGV[0]);

#*****************************************************************************
# Create QXDM instance
#*****************************************************************************
my $qxdm_app = new Win32::OLE 'QXDM.Application';
if(!$qxdm_app)
{
   print "ERROR: Unable to invoke the QXDM application.\n";
   die;
}

$qxdm_app->{COMPort} = $port_num;

# Wait until phone is connected
while( $qxdm_app->{IsPhoneConnected} == 0 ){
   print "Unable to Connect to the Phone. Retrying.\n";
   sleep(1);
}

SetQXDM ( $QXDM );

# Get file name from script folder path
my $isf_filename = GenerateFileName( "", "" );

(my $isf_volume, my $isf_directories, my $isf_file) = File::Spec->splitpath($isf_filename);
$isf_filename = $isf_file;

my $isf_handle = $qxdm_app->CreateItemStore($isf_filename . ".isf");

# Save current item store. This is to trigger start of a fresh isf collect. Storing stale data will mean that
# the post processing of the file will take longer.

SaveISF( );
$qxdm_app->CloseItemStore($isf_handle);

$Version = 53;                 # Ver 53 - Added 4 bytes for ADC IQ saturation threshold
$TestControl = 0;              # 0/1 => Start/Stop Iq Test 

$Flag = 0x9;                     # VALID VALUES for "Flag" field:-
                               # Bit0 - Enable IQ Logging
                               # Bit1 - Enable FFT Logging (on-board FFT)
                               # Bit2 - Enable Hanning window for on-board FFT
                               # Bit3 - 1 => Disable "Dropped Log Recovery" feature
                               #        0 => Enable "Dropped Log Recovery" feature


$RFStatusLoggingRateHz = 4;    # 1~10 allowed

# GPS Center Frequency : 1575.52 MHz => +100 kHz Offset 
# GLO Center Frequency : 1602.10 MHz => +100 kHz Offset
# Signal Level Should be -120 dbm

$CollectMode = 16;              # VALID VALUES for "CollectMode":-
                               # 0   WBIQ BP1 and BP2 simultaneous (2.046 MHz sample rate)
                               # 1   WBIQ BP1 (2.046 MHz sample rate)
                               # 2   WBIQ BP2 (2.046 MHz sample rate)
                               # 3   WBIQ BP3 (1.022 MHz sample rate)
                               # 4   FBIQ BP4 (20.46 MHz sample rate)
                               # 5   NBIQ BP1 Autocenter  (1 kHz sample rate) 
                               # 6   NBIQ BP2 Autocenter  (1 kHz sample rate) 
                               # 7   NBIQ BP3 Autocenter  (1 kHz sample rate) 
                               # 8   NBIQ BP1 Manual  (1 kHz sample rate)
                               # 9   NBIQ BP2 Manual  (1 kHz sample rate)
                               # 10  NBIQ BP3 Manual  (1 kHz sample rate)
                               # 11  NBIQ BP4 Manual  (1 kHz sample rate)
                               # 12  (84 div by 5) MHz (Sample Logging ADC1)
                               #     Each 32-bit word has 3-bit I and 3-bit Q from ADC1
                               #     LSB is the most recently captured sample.
                               # 13  (84 div by 5) MHz (Sample Logging ADC2)
                               #     Each 32-bit word has 3-bit I and 3-bit Q from ADC1
                               #     LSB is the most recently captured sample.
                               # 14  (84 div by 4) MHz (Sample Logging ADC1 + ADC2)
                               #     Each 32-bit word has 2-bit I and 2-bit Q from ADC1 and 2-bit I and 2-bit Q from ADC2.
                               #     Format: ADC1:ADC2 (I:Q).
                               #     LSB is the most recently captured sample.
                               # 15  BP3 FBIQ test (10.22 MHz sample rate)
                               #     2 bytes per sample (8 bit I and 8 bit Q)
                               # 16  ADC IEQ at 81MHz
							   # 17  WBIQ_BP2 at 4MHz (Beidou)

$GLO_R1_Chan = 0;              # -7~6 allowed
$GLO_HW_Chan = 1;              # 1-12 allowed
$Num1kSamples = 256;         # 1-1024 allowed for all WB. 0-1024 allowed for NB. 0=>Collect Forever
                               # For BP3 FBIQ (Num1kSamples recommended is 512)

$Integrations = 20;            # Greater than 0
                               # For BP3 FBIQ ($Integrations recommended is 15)

$CenterFreqNBHz = 100000;      # -1 MHz to +1 MHz for BP1 and BP2
                               # -500 kHz to +500 kHz for BP3
                               # -10 MHz to +10 MHz for BP4
					
if($Version >= 53)
{					
  $AdcIqSatThresh = 9950;      # Default saturation threshold - (unit of 0.01%). 9950 indicates a saturation
                               # threshold of 99.5%						   

  print ( "..\\iq_driver\\iq_driver.exe ". $isf_filename . " " . $Version . " " . $TestControl . " " . 
                                                           $Flag . " " . $RFStatusLoggingRateHz . " " . $CollectMode . " " . 
                                                           $GLO_R1_Chan . " " . $GLO_HW_Chan . " " . $Num1kSamples . " " . 
                                                           $Integrations . " " . $CenterFreqNBHz . " " . $AdcIqSatThresh );

  system("..\\iq_driver\\iq_driver.exe ". $isf_filename . " " . $Version . " " . $TestControl . " " . 
                                                           $Flag . " " . $RFStatusLoggingRateHz . " " . $CollectMode . " " . 
                                                           $GLO_R1_Chan . " " . $GLO_HW_Chan . " " . $Num1kSamples . " " . 
                                                           $Integrations . " " . $CenterFreqNBHz . " " . $AdcIqSatThresh );
}
else
{
  print ( "..\\iq_driver\\iq_driver.exe ". $isf_filename . " " . $Version . " " . $TestControl . " " . 
                                                           $Flag . " " . $RFStatusLoggingRateHz . " " . $CollectMode . " " . 
                                                           $GLO_R1_Chan . " " . $GLO_HW_Chan . " " . $Num1kSamples . " " . 
                                                           $Integrations . " " . $CenterFreqNBHz );

  system("..\\iq_driver\\iq_driver.exe ". $isf_filename . " " . $Version . " " . $TestControl . " " . 
                                                           $Flag . " " . $RFStatusLoggingRateHz . " " . $CollectMode . " " . 
                                                           $GLO_R1_Chan . " " . $GLO_HW_Chan . " " . $Num1kSamples . " " . 
                                                           $Integrations . " " . $CenterFreqNBHz );

}

if (-e "C:\\Program Files\\Qualcomm\\QXDM\\bin\\ISFConverter.exe")
{
   print("\"C:\\Program Files\\Qualcomm\\QXDM\\bin\\ISFConverter.exe\" /pc " . $isf_filename . ".isf " . $isf_filename . ".dlf");
   system("\"C:\\Program Files\\Qualcomm\\QXDM\\bin\\ISFConverter.exe\" /pc " . $isf_filename . ".isf " . $isf_filename . ".dlf");
}
elsif (-e "C:\\Program Files (x86)\\Qualcomm\\QXDM\\bin\\ISFConverter.exe")
{
   print("\"C:\\Program Files (x86)\\Qualcomm\\QXDM\\bin\\ISFConverter.exe\" /pc " . $isf_filename . ".isf " . $isf_filename . ".dlf");
   system("\"C:\\Program Files (x86)\\Qualcomm\\QXDM\\bin\\ISFConverter.exe\" /pc " . $isf_filename . ".isf " . $isf_filename . ".dlf");
}
else
{
  # need the ISFConverter that comes with QXDM, print error if QXDM is not installed
  exit ("\n\tYou need to install QXDM to run this script\n");
}

print("\n\n");

 print("..\\iq_pkt_parser\\IqPacketParser.exe " . $isf_filename . ".dlf  " . "IQ_". $isf_filename . ".bin FFT_" . $isf_filename . ".bin");
system("..\\iq_pkt_parser\\IqPacketParser.exe " . $isf_filename . ".dlf  " . "IQ_". $isf_filename . ".bin FFT_" . $isf_filename . ".bin");

print("\n");

system( "if exist IQ_" . $isf_filename . ".bin  copy IQ_". $isf_filename . ".bin gnss_iq_collect.bin");
system( "if exist FFT_" . $isf_filename . ".bin  copy FFT_". $isf_filename . ".bin gnss_fft_collect.bin");
$qxdm_app = NULL;


# METHOD:
#   usage
#
# DESCRIPTION:
#   Display usage information.
#
# RETURN VALUE:
#   None
#

# Save the curent ISF
sub SaveISF
{
   $qxdm_app->SaveItemStore( $cwd . "\\" . $isf_filename . ".isf" );
   $RC = true;
   return $RC;
}

sub usage
{
   my $usage = <<END;

GPS Wide-Band IQ Collect

Syntax:   perl cgps_wbiq_collect.pl <Options>

Options:  -help               Display this help message.
          Option 1:           <QPST Connection Port Number>

 
Example:  perl cgps_wbiq_collect.pl 10

END

   print "$usage";
   exit (0);
}
