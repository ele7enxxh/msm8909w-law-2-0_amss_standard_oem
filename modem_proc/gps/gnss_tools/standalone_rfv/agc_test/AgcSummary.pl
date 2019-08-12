# 
#$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/standalone_rfv/agc_test/AgcSummary.pl#1 $ $DateTime: 2016/12/13 07:59:45 $ $Author: mplcsds1 $
#*****************************************************************************
#  Copyright (C) 2003 Qualcomm, Inc.
#
#                  Qualcomm, Inc.
#                  675 Campbell Technology Parkway
#                  Campbell, CA  95008
#
# This program is confidential and a trade secret of SnapTrack, Inc.  The
# receipt or possession of this program does not convey any rights to reproduce 
# or disclose its contents or to manufacture, use or sell anything that this 
# program describes in whole or in part, without the express written consent of
# SnapTrack, Inc.  The recipient and/or possessor of this program shall not 
# reproduce or adapt or disclose or use this program except as expressly 
# allowed by a written authorization from SnapTrack, Inc.
#
# *****************************************************************************
# Description:
# This file contains standalone verification test support for the Converged GPS 
#
# Usage:
# >perl AgcSummary.pl <file name> 
#
# History:
# 09/06/07    jb  Version 1.4
#                 Correct display of mean of MeanQ on summary
# 06/29/07    jb  Version 1.3
#                 Amplitude pass/fail 100-500
#                 Print DC Mean for information
# 06/07/07    jb  Version 1.2
# 05/21/07    jb  Version 1.1
#                 Print min/max/STD for Mean I/Q, pass/fail optional argument,
#                 default +/-500
#                 Print min/max DC Offset I/Q, pass/fail 1-254
#	
# 
#*****************************************************************************
#*/
#
# sigma subroutine extract the standard deviation from N samples using the
# sum(x) and sum(x*x)

sub sigma
{
	my( $n, $x, $x2 ) = @_;
	my( $mean ) = $x/$n;
  my( $diff ) = $x2/$n - $mean*$mean;
  if( $diff < 0.000001 )
  {
    $sigma = 0.0;
  }
  else
  {
	  $sigma = sqrt( $diff );
  }

}

# open_file performs the generic file open and checking logic

sub open_file
{
	my( $Handle, $Attrib, $Name ) = @_;
	my( $File ) = sprintf "%s %s", $Attrib, $Name;

	unless( open $Handle, $File )
	{
		die( "Cannot open $Name: $!");
	}
}

# subroutine to read N lines from a specified file

sub read_lines
{
	my( $N_Lines ) = @_;

	$read_lines = '';

	foreach( 1..$N_Lines )
	{
		$line = <INPUT>;
		chomp $line;
		$read_lines .= $line . " ";
	}

	# Return the composite line
	$read_lines;
}

use vars qw(%opts);
use Win32::OLE;
use Win32::OLE::Variant;
use Getopt::Long;

$Version = 1.4;

%opts = ();
GetOptions( \%opts, 'help');

&usage() if ($opts{'help'});
my $in_file = ($ARGV[0]);

# Open various files

&open_file( SUMMARY, 		">", "AgcSummary.txt" );
&open_file( INPUT, 		"<", "$in_file" );

$MinAmpI = 65535;
$MinAmpQ = 65535;
$MaxAmpI = 0;
$MaxAmpQ = 0;

$MinMeanI = 32767;
$MinMeanQ = 32767;
$MaxMeanI = -32767;
$MaxMeanQ = -32767;

while ( <INPUT> )
{
	$pkt_header = $_;
	chomp $pkt_header;

#1980 Jan  6 	01:18:35.684 [02]	Converged GPS RF Status Report
#Version: 1
#PLL Locked  = Yes
#PLL Chan    = 1
#BP Mean I   = 4241
#BP Mean Q   = 4241
#BP Ampl I   = 50
#BP Ampl Q   = 53
#BP Rotator Freq (Hz)        = 4294962950 
#BP Rotator Delta Freq (Hz)  = 0

#GPF Micro DC Offset disabled 
#GPF Gain of Amplitude-adjusting IIR = No left or right shift 
#GPF GPF Notch Filter disabled 
#GPF Notch2 Filter Coeff = (13, 0, 0, 0) 
#MND Counter, M = 14 
#MND Counter, N = 15 
#MND Counter, D = 16 

#RF DC Offset I = 18 
#RF DC Offset Q = 19 
#RF DC Loop Time disabled 
#RF ADC gain setting disabled 
#RF chip = RTR6285

	# Pull the FCount off the first measurement report string
	if( $pkt_header =~ /Converged\s+GPS\s+RF\s+Status\s+Report$/)
	{
    # Verify that the next line is "Version = ...."
		$line = read_lines(1);
		chomp( $line );
		@line = split (' ', $line);
    if( $line[0] =~ /Version/ )
    {
      # Skip over the line "PLL Locked ...."
      # Skip over the line "PLL Chan ...."
		  $line = read_lines(2); 

      # Read the line "BP Mean I   = ...."
      $line = read_lines(1); 
      chomp( $line );
		  @line = split (' ', $line);
      $MeanI = $line[4];

      # Read the line "BP Mean Q   = ...."
      $line = read_lines(1); 
      chomp( $line );
		  @line = split (' ', $line);
      $MeanQ = $line[4];

      # Read the line "BP Ampl I   = ...."
      $line = read_lines(1); 
      chomp( $line );
		  @line = split (' ', $line);
      $AmpI = $line[4];

      # Read the line "BP Ampl Q   = ...."
      $line = read_lines(1); 
      chomp( $line );
		  @line = split (' ', $line);
      $AmpQ = $line[4];


      if( $MeanI > 32767 )
      {
        $MeanI -= 65536;
      }
      if( $MeanQ > 32767 )
      {
        $MeanQ -= 65536;
      }

			$MaxMeanI = $MeanI if $MeanI > $MaxMeanI;
			$MinMeanI = $MeanI if $MeanI < $MinMeanI;
			$MaxMeanQ = $MeanQ if $MeanQ > $MaxMeanQ;
			$MinMeanQ = $MeanQ if $MeanQ < $MinMeanQ;
      $SumMeanI += $MeanI;
      $SumMeanQ += $MeanQ;

			$MaxAmpI = $AmpI if $AmpI > $MaxAmpI;
			$MinAmpI = $AmpI if $AmpI < $MinAmpI;
			$MaxAmpQ = $AmpQ if $AmpQ > $MaxAmpQ;
			$MinAmpQ = $AmpQ if $AmpQ < $MinAmpQ;
      $SumAmpI += $AmpI;
      $SumAmpQ += $AmpQ;
      $SumN++;
    }
  }
}

    # Processing complete. Now generate summary information.
    printf( "\nAGC Amplitudes processed from %d RF logs\n", $SumN );
if( $SumN > 1 )
{   
    $MeanAmpI = $SumAmpI / $SumN;
    $MeanAmpQ = $SumAmpQ / $SumN;
    $MeanMeanI = $SumMeanI / $SumN;
    $MeanMeanQ = $SumMeanQ / $SumN;
    printf( "AGC Ampl I (Min/Mean/Max)  %6.0f %6.0f %6.0f\n",
            $MinAmpI,
            $MeanAmpI,
            $MaxAmpI );
    printf( "AGC Ampl Q (Min/Mean/Max)  %6.0f %6.0f %6.0f\n",
            $MinAmpQ,
            $MeanAmpQ,
            $MaxAmpQ );

    printf( " DC Mean I (Min/Mean/Max)  %6.0f %6.0f %6.0f\n",
            $MinMeanI,
            $MeanMeanI,
            $MaxMeanI );
    printf( " DC Mean Q (Min/Mean/Max)  %6.0f %6.0f %6.0f\n",
            $MinMeanQ,
            $MeanMeanQ,
            $MaxMeanQ );

    printf( "\nSummary written to \"AgcSummary.txt\"\n\n" );
    printf( "AgcSummary.pl Version $Version\n" );

    printf( SUMMARY "AGC Amplitudes processed from %d RF logs\n", $SumN );
    printf( SUMMARY "AGC Ampl I (Min/Mean/Max)  %6.0f %6.0f %6.0f\n",
            $MinAmpI,
            $MeanAmpI,
            $MaxAmpI );
    printf( SUMMARY "AGC Ampl Q (Min/Mean/Max)  %6.0f %6.0f %6.0f\n",
            $MinAmpQ,
            $MeanAmpQ,
            $MaxAmpQ );

    printf( SUMMARY " DC Mean I (Min/Mean/Max)  %6.0f %6.0f %6.0f\n",
            $MinMeanI,
            $MeanMeanI,
            $MaxMeanI );
    printf( SUMMARY " DC Mean Q (Min/Mean/Max)  %6.0f %6.0f %6.0f\n",
            $MinMeanQ,
            $MeanMeanI,
            $MaxMeanQ );

    printf( SUMMARY "\nPASS/FAIL CHECKS\n" );
    $FailFlag = 0;
    if( ($MinAmpI >= 100) && ($MaxAmpI <= 500) && 
        ($MinAmpQ >= 100) && ($MaxAmpQ <= 500) ) 
    {
    printf( SUMMARY "PASS Ampl I/Q Range   | 100-500\n" );
    }
    else
    {
    printf( SUMMARY "FAIL Ampl I/Q Range   | 100-500\n" );
    $FailFlag = 1;
    }
    if( $FailFlag )
    {
    printf( SUMMARY "--------------FAIL--------------\n" );
    }
    else
    {
    printf( SUMMARY "--------------PASS--------------\n" );
    }
    printf( SUMMARY "\nAgcSummary.pl Version $Version\n" );
}

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


Syntax:   Single_SV_SA [options]

Options:  -help               Display this help message.
          OPTION 1:
          <Input File>
          
          OPTION 2:
          <STD Specified Limit>

Example:  AgcSummary.pl <ParsedData.txt> 3.8

END

   print "$usage";
   exit (0);
}


