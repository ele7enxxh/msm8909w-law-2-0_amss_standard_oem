################################################################################
# $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/pdapi_scripts/pd_SntpGetTime.pm#1 $
# $DateTime: 2016/12/13 07:59:45 $
# $Author: mplcsds1 $
#
# This package provides functionality to get SNTP Time.
#
# Revision History
# when       who    what, where, why
# --------   ---    ----------------------------------------------------------
# 09/23/10   ssk     Initial version.
#
################################################################################

package pd_SntpGetTime;

require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw(Obtain_NTP_Time);

  use Time::HiRes qw(time);

  use IO::Socket;

  use Getopt::Std;
  use strict;

  my ($FirstByte, $Layer, $Polling, $Precision,
      $RootDelay, $RootDelayPart, $RootDisp, $RootDispPart, $ReferenceIdent,
      $RefTime, $RefTimePart, $OriginationTime, $OriginationTimePart,
      $ReceiveTime, $ReceiveTimePart, $TransmissionTime, $TransmissionTimePart);
      
  my ($dummy, $RootDelayH, $RootDelayFH, $RootDispH, $RootDispFH, $ReferenceIdentT,
      $RefTimeH, $RefTimeFH, $OriginationTimeH, $OriginationTimeFH,
      $ReceiveTimeH, $ReceiveTimeFH, $TransmissionTimeH, $TransmissionTimeFH);
      
  my ($UnusedField, $ProtocolVersion, $Mode, $sc, $PollingT, $PrecisionV, $ReferenceT, $ReferenceIPv4);
  

  my ($SystemTime0, $SystemTime0F, $SystemTime0H, $SystemTime0FH, $SystemTime0Part);
  my ($SystemTime1, $SystemTime2);
  my ($SystemTime, $SystemTimeF, $SystemTimeT);
  my ($TotalTime, $TotalTime2, $TotFraction);
  my ($TotRoundTime, $TotalDelay, $TimeOffset);
  
  my $ServerUrl = "xtra1.gpsOneXTRA.net";

  my $IPv4server ="";

  my $responsetimer = 5;

  my $ntp_mesg_pkt;  # NTP protocol message
  
  
################################################################################
# METHOD:
#   usage
#
# DESCRIPTION:
#  Send the time request by establishing a connection to the ntp server and preparing
#  the ntp request packet
#  Stores local machine  time before and after requesting for NTP time.
# 
# RETURN VALUE:
#   None
#
################################################################################  
sub request_ntp_time 
{ 

  my ($remote);
  my $ntp_mesg_pkt;
  
  my ($RespIn, $RespOut, $ExpOut) ="";

  my @NtpSrvList=($main::XTRASNTPURL1, $main::XTRASNTPURL2,$main::XTRASNTPURL3, $main::XTRASNTPURL4,$main::XTRASNTPURL5, $main::XTRASNTPURL6);
  
foreach (@NtpSrvList)
{
   print main::LOGFILE ("\n Using  ".  sprintf("%s",$_)  . " for NTP time\n");     

  #if (gethostbyname($_)) 
  if (gethostbyname($main::XTRASNTPURL1)) 
  {
   $IPv4server = sprintf("%d.%d.%d.%d",unpack("C4",gethostbyname($_)));
  }

  # Establish connection with ntp server
  $remote = IO::Socket::INET -> new(Proto => "udp", PeerAddr => $_,
			      PeerPort => 123,
			      Timeout => $responsetimer)
			  or 
			  {
			    next
			    #die "FAILURE : Unable to connect to \"$_\"  Check INTERNT Connection\n"
      	                  };
			     
                            # next go again to the top of the loop			    			  

  # Let use store the local time 
  $SystemTime1 = time();

  #Let us convert  unix epoch time to NTP timestamp
  $SystemTime0 = $SystemTime1 + 2208988800;

  # prepare local timestamp for transmission in our request packet
  $SystemTime0F = $SystemTime0 - int($SystemTime0);
  $SystemTime0Part = FractionToBinary($SystemTime0F);
  $SystemTime0H = unpack("H8",(pack("N", int($SystemTime0))));
  $SystemTime0FH = unpack("H8",(pack("B32", $SystemTime0Part)));

  $ntp_mesg_pkt = pack("B8 C3 N10 B32", '00011011', (0)x12, int($SystemTime0), $SystemTime0Part);

  #send the ntp request to the server
  $remote -> send($ntp_mesg_pkt) or return undef;
  
  vec($RespIn, fileno($remote), 1) = 1;
  select($RespOut=$RespIn, undef, $ExpOut=$RespIn, $responsetimer)
  or do 
  {
    print ("\n ERROR No answer from $_\n"); 
    print main::LOGFILE ("\n ERROR No answer from $_\n"); 
    next;# go again to the top of the loop , exit;
  };

  # Recv  ntp message from remote server
  $remote -> recv($ntp_mesg_pkt, length($ntp_mesg_pkt))
   or do 
   {
    print main::LOGFILE ("Receive ERROR from $_ ($!)\n"); 
    print ("Receive ERROR from $_ ($!)\n");         
    next;# go again to the top of the loop and wait for next event#exit
   };

  # measure local time after the time query
  $SystemTime2 = time();

  last;
  
} # end of foreach

  $ntp_mesg_pkt;
} # End of request_ntp_time 

################################################################################
# METHOD:
#   usage
#
# DESCRIPTION:
#  Decode the ntp server responses
#
# RETURN VALUE:
#   None
#
################################################################################
sub decode_ntp_responses 
{
  # interpretations of the data

  my $ntp_mesg_pkt = shift;

  # unpack recvd msg into  integer and binary values
  ( $FirstByte, $Layer, $Polling, $Precision,
      $RootDelay, $RootDelayPart, $RootDisp, $RootDispPart, $ReferenceIdent,
      $RefTime, $RefTimePart, $OriginationTime, $OriginationTimePart,
      $ReceiveTime, $ReceiveTimePart, $TransmissionTime, $TransmissionTimePart) =
      unpack ("a C3   n B16 n B16 H8   N B32 N B32   N B32 N B32", $ntp_mesg_pkt
  );

   # Unpack recvd ntp message into hexadecimal & ASCII 
   ( $dummy, $dummy, $dummy, $dummy,
      $RootDelayH, $RootDelayFH, $RootDispH, $RootDispFH, $ReferenceIdentT,
      $RefTimeH, $RefTimeFH, $OriginationTimeH, $OriginationTimeFH,
      $ReceiveTimeH, $ReceiveTimeFH, $TransmissionTimeH, $TransmissionTimeFH) =
      unpack ("a C3   H4 H4 H4 H4 a4   H8 H8 H8 H8   H8 H8 H8 H8", $ntp_mesg_pkt
   );

   $UnusedField = unpack("C", $FirstByte & "\xC0") >> 6;
   $ProtocolVersion = unpack("C", $FirstByte & "\x38") >> 3;
   $Mode = unpack("C", $FirstByte & "\x07");
   if ($Layer < 2) 
   {
     $sc = $Layer;
   }
   else 
   {
      if ($Layer > 1) 
      {
        if ($Layer < 16) 
        {
          $sc = 2;
        } 
        else 
        {
          $sc = 16;
        }
      }
   }
   
    $PollingT = 2**($Polling);
    if ($Precision > 127) {$Precision = $Precision - 255;}
    $PrecisionV = sprintf("%1.4e",2**$Precision);
    $RootDelay += binaryToFrac($RootDelayPart);
    
    $RootDelay = sprintf("%.4f", $RootDelay);
    $RootDisp += binaryToFrac($RootDispPart);
    $RootDisp = sprintf("%.4f", $RootDisp);
    $ReferenceT = "";
    if ($Layer eq 1) {$ReferenceT = "[$ReferenceIdentT]";}
    else 
    {
      if ($Layer eq 2) 
      {
        if ($ProtocolVersion eq 3) 
        {
          $ReferenceIPv4 = sprintf("%d.%d.%d.%d",unpack("C4",$ReferenceIdentT));
          $ReferenceT = "[32bit IPv4 addr $ReferenceIPv4 of the ref src]";
        }
        else 
        {
          if ($ProtocolVersion eq 4) {$ReferenceT = "[low 32bits of latest TX times of ref src]";}
        }
      }
    }

    $RefTime += binaryToFrac($RefTimePart);
    $OriginationTime += binaryToFrac($OriginationTimePart);
    $ReceiveTime += binaryToFrac($ReceiveTimePart);
    $TransmissionTime += binaryToFrac($TransmissionTimePart);

}# End of decode_ntp_responses 


################################################################################
# METHOD:
#   usage
#
# DESCRIPTION:
#  Do the time conversion and calculations.
#
# RETURN VALUE:
#   None
#
################################################################################
sub calculate_time_data 
{
 #convert the time stamps to unix epoch and do some calculations on the time data

  my ($second, $minute, $hour, $day, $month, $year);

  #Convert the NTP time to unix epoch time
  $TransmissionTime -= 2208988800; 
  $RefTime -= 2208988800; 
  $OriginationTime -= 2208988800; 
  $ReceiveTime -= 2208988800; 


  $TotalTime = scalar(gmtime $TransmissionTime);
  
  $TotFraction = sprintf("%03.f",1000*sprintf("%.3f", $TransmissionTime - int($TransmissionTime)));
  ($second, $minute, $hour, $day, $month, $year) = gmtime($TransmissionTime);
  $TotalTime2 = sprintf("%04d-%02d-%02d %02d:%02d:%02d", $year+1900, $month+1, $day, $hour, $minute, $second);

  #Calculate the delay and difference
  $TotRoundTime = sprintf("%+.4f",($SystemTime1 - $SystemTime2));
  $TotalDelay = sprintf("%+.4f",(($SystemTime1 - $SystemTime2)/2) - ($TransmissionTime - $ReceiveTime));
  $TimeOffset = sprintf("%+.4f",(($ReceiveTime - $SystemTime1) + ($TransmissionTime - $SystemTime2))/2);

  $SystemTime = ($SystemTime1 + $SystemTime2) /2;
  $SystemTimeF = sprintf("%03.f",1000*sprintf("%.3f", $SystemTime - int($SystemTime)));
  ($second, $minute, $hour, $day, $month, $year) = gmtime($SystemTime);
  $SystemTimeT = sprintf("%04d-%02d-%02d %02d:%02d:%02d", $year+1900, $month+1, $day, $hour, $minute, $second);

}# End of calculate_time_data

################################################################################
# METHOD:
#   usage
#
# DESCRIPTION:
#  Do the time conversion and calculations.
#
# RETURN VALUE:
#   None
#
################################################################################
sub output_ntp_data 
{ 

  # Dump the data
  print main::LOGFILE ("Local Transmit Timestp : " . $SystemTime0 . "\n");
  print main::LOGFILE ("The ntp server [$main::XTRASNTPURL1 $IPv4server] sent the following data:\n");
  print main::LOGFILE ("Forst Byte                  : " . ord($FirstByte) . "\n");

  print main::LOGFILE ("Ref Time    : $RefTimeH.$RefTimeFH [" .
			    sprintf("%10.5f",$RefTime) . "]\n");
  print main::LOGFILE ("Originate Time    : $OriginationTimeH.$OriginationTimeFH [" .
			    sprintf("%10.5f",$OriginationTime) . "]\n");
  print main::LOGFILE ("Receive Time      : $ReceiveTimeH.$ReceiveTimeFH [" .
			    sprintf("%10.5f",$ReceiveTime) . "]\n");


  print main::LOGFILE ("Version number   : $ProtocolVersion [NTP/SNTP version number]\n");
  print main::LOGFILE ("Polling iterval  : $Polling [2**$Polling = $PollingT sec max interval between successive msgs]\n");
  print main::LOGFILE ("Clock precision  : $Precision [2**$Precision = $PrecisionV]\n");
  print main::LOGFILE ("Root delay       : $RootDelayH$RootDelayFH [$RootDelay sec]\n");
  print main::LOGFILE ("Root dispersion  : $RootDispH$RootDispFH [$RootDisp sec]\n");
  print main::LOGFILE ("Reference Id     : $ReferenceIdent $ReferenceT \n");
  print main::LOGFILE ("Transmit time          : $TransmissionTimeH.$TransmissionTimeFH [" .
			    sprintf("%10.5f",$TransmissionTime) . "]\n \n");

}# End of output_ntp_data


################################################################################
# METHOD:
#   usage
#
# DESCRIPTION:
# Convert from fraction to binary
#
# RETURN VALUE:
#   None
#
################################################################################
sub FractionToBinary 
{ 
  my $frac = shift;
  my $bin ="";
  while (length($bin) < 32) 
  {
    $bin = $bin . int($frac*2);
   $frac = $frac*2 - int($frac*2);
  }
  $bin;
}# End of FractionToBinary


################################################################################
# METHOD:
#   usage
#
# DESCRIPTION:
#  Cconvert  binary to fraction  
#
# RETURN VALUE:
#   None
#
################################################################################
sub binaryToFrac 
{ 
  my @bin = split '', shift;
  my $frac = 0;
  while (@bin) 
  {
   $frac = ($frac + pop @bin)/2;
  }
$frac;
}# End of binaryToFrac

################################################################################
# METHOD:
#   usage
#
# DESCRIPTION:
#   Main routine that obtains the SNTP Time from NTP Server
#
# RETURN VALUE:
#   None
#
################################################################################
sub Obtain_NTP_Time 
{
  print main::LOGFILE ("Establishing connection with NTP server :$main::XTRASNTPURL1 \n");

  $ntp_mesg_pkt = request_ntp_time;

  decode_ntp_responses($ntp_mesg_pkt);

  ## We need to verify if Recvd packet is the expected responses. If correct, if our original transmit
  # time we sent in the Transmit Time field  of our request packet shows up in the Originate Time field of the received reply.
  if (($SystemTime0H . $SystemTime0FH) ne ($OriginationTimeH . $OriginationTimeFH)) 
  {
    print main::LOGFILE "*** Response not matching with  our SNTP request\n";
    print main::LOGFILE "*** The originate time $OriginationTimeH.$OriginationTimeFH of the recvd pkt doesnt \n";
    print main::LOGFILE "*** match the transmission time $SystemTime0H.$SystemTime0FH.\n";
    
    print  "*** Response not matching with  our SNTP request\n";
    print  "*** The orginate time $OriginationTimeH.$OriginationTimeFH of the recvd pkt doesnt \n";
    print  "*** match the transmission time $SystemTime0H.$SystemTime0FH.\n";
    
    exit;
  }

  calculate_time_data;
  #$main::GlobalGPSTime = $TransmissionTime;
  
  my $SystemTime3 = time();
   # convert fm unix epoch time to NTP timestamp
  my $LocalUTCTime = $SystemTime1 + 2208988800;
  
  $main::GlobalUTCTime =  $TimeOffset + $LocalUTCTime;

  #GPS, Global Positioning System time, is the atomic time scale implemented by the
  #atomic clocks in the GPS ground control stations and the GPS satellites themselves. 
  #GPS time was zero at 0h 6-Jan-1980 and since it is not perturbed by leap seconds GPS
  #is now ahead of UTC by 17 seconds  (as of July 2015).
  $main::GlobalGPSTime =  $TimeOffset + $SystemTime1 - 315964800 + 17;
    
  print main::LOGFILE "Globaltransmittime: $main::GlobalGPSTime off : $TimeOffset, localtime : $LocalUTCTime\n";
  
  print main::LOGFILE "\n";
  
  # Dump the raw data
  #output_ntp_data; 
  
} # sub Obtain_NTP_Time 
  
############################################
#Return value for this perl module
 1;
