# 
#$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/standalone_rfv/freq_xfer_test/FreqXferSummary.pl#1 $ $DateTime: 2016/12/13 07:59:45 $ $Author: mplcsds1 $
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
# 
#*****************************************************************************
#*/
#
#!/c:\utils\Cygwin\bin\perl.exe -w
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
use SingleSV_SA;
use Win32::OLE;
use Win32::OLE::Variant;
use Getopt::Long;

%opts = ();
GetOptions( \%opts, 'help');

&usage() if ($opts{'help'});
my $in_file = ($ARGV[0]);

# Open various files

&open_file( SUMMARY, 		">", "FreqXferSummary.txt" );
&open_file( INPUT, 		"<", "$in_file" );

$MinAmpI = 32767;
$MinAmpQ = 32767;
$MaxAmpI = -32767;
$MaxAmpQ = -32767;

while ( <INPUT> )
{
	$pkt_header = $_;
	chomp $pkt_header;

}

    # Processing complete. Now generate summary information.
    printf( "\nFreq Xfer records processed from %d Freq Xfer logs\n", $SumN );
if( $SumN > 1 )
{   
    printf( "\nSummary written to \"FreqXferSummary.txt\"\n\n" );
    printf( SUMMARY "Freq Xfer records processed from %d Freq Xfer logs\n", $SumN );
    printf( SUMMARY "\nPASS/FAIL CHECKS\n" );
    if( $FailFlag )
    {
    printf( SUMMARY "--------------FAIL--------------\n" );
    }
    else
    {
    printf( SUMMARY "--------------PASS--------------\n" );
    }
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


