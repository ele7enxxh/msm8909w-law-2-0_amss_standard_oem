################################################################################
# $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/pdapi_scripts/pd_XtraBinFileInject.pm#1 $
# $DateTime: 2016/12/13 07:59:45 $
# $Author: mplcsds1 $
#
# This script injects the XTRA bin file
#
#
# Revision History
# when       who    what, where, why
# --------   ---    ----------------------------------------------------------
# 08/17/10   ssk     Initial version.
#
################################################################################

package pd_XtraBinFileInject;

require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw(GpsXtraBinFileSend);

use Time::HiRes qw( usleep  );
use strict;

use vars qw(%opts);

use Win32::OLE;
use Win32::OLE::Variant;

use Getopt::Long;
use Cwd;

use pd_XtraDataPkt;

# Set the timeout to 2 seconds.
use constant TIMEOUT => 2000;

# PDSM_CLIENT_TYPE_TEST2
use constant  CLIENT_TYPE_TEST2 => 8; 
#PDSM_CLIENT_TYPE_XTRA
use constant CLIENT_TYPE_XTRA => 11; 

#  Miscellaneous constants
use constant null                => 0;
use constant false               => 0;
use constant true                => 1;

# Set the timeout to 2 seconds.
use constant TIMEOUT => 2000;
my $ReqHandle;
my $XtraPartLen;
my $XtraPartCount;
my $TotalXtraPkts;
### how big should the new file be

my $XtraPartBuf = '';  


################################################################################
# METHOD:
#   usage
#
# DESCRIPTION:
#   Send the XTRA data segment
#
# RETURN VALUE:
#   None
#
################################################################################
sub XtraPktSend 
{ 
  #printf ("Sending CGPS_XTRA_INJECT_DATA_CMD: Initialize XTRA client type id %d tot:%d id:%d\n", $main::client_id_xtra,$TotalXtraPkts, $main::client_id_test2 );
  printf main::LOGFILE ("Sending CGPS_XTRA_INJECT_DATA_CMD:  XTRA client type id %d tot:%d id:%d\n", $main::client_id_xtra,$TotalXtraPkts, $main::client_id_test2 );  
  
  my $buf_size = $main::MAXSEG_SIZE;   
 
  print main::LOGFILE ("XtraPktSend Len=" . sprintf("%d",$XtraPartLen). ",Cnt =". sprintf("%d",$XtraPartCount) . ",Size =". sprintf("%d",$buf_size) ."\n"); 
 
  ######################################################################
  # Build a diagnostic request for CGPS_XTRA_INJECT_DATA_CMD
  ######################################################################
 
  # $FileNameLength = length($FileNameAndData);  
  # print main::LOGFILE ("FileLength %d\n", $FileLen);

  my $req_xtra_data_send = new pd_XtraDataPkt;

  $req_xtra_data_send->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_xtra_data_send->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_xtra_data_send->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_xtra_data_send->{"cgps_cmd_code"}   = 204;  # CGPS Cmd Code CGPS_XTRA_INJECT_DATA_CMD
  $req_xtra_data_send->{"version"}         = 1;   # Version Number
  $req_xtra_data_send->{"client_id"}       = $main::client_id_xtra;  # XTRA Client ID

  $req_xtra_data_send->{"file_size"}        = $main::XTRAFILESIZE;
  $req_xtra_data_send->{"data_part_number"} = $XtraPartCount;
  $req_xtra_data_send->{"xtra_data_payload"}= $XtraPartBuf;
  $req_xtra_data_send->{"xtra_data_len"}    = $XtraPartLen; 
  $req_xtra_data_send->{"client_data_ptr"}   = $main::client_id_xtra;
  $req_xtra_data_send->{"data_total_parts"}  = $TotalXtraPkts;
  

  ######################################################################
  # Construct the DM CGPS_XTRA_INJECT_DATA_CMD capture request packet
  ######################################################################
   my $req_setnmeaconfig_pkt = $req_xtra_data_send->SetCaptureCmd();

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  my $rsp_setnmeaconfig_pkt = $main::qxdm_app->SendDmIcdPacketEx($req_setnmeaconfig_pkt, TIMEOUT);

  my $rsp_setnmeaconfig_len = length($rsp_setnmeaconfig_pkt);
  print main::LOGFILE ("DM CGPS_XTRA_INJECT_DATA_CMD Response Packet Length=" . sprintf("%d",$rsp_setnmeaconfig_len) ."\n");   

  $req_xtra_data_send->ParseData($rsp_setnmeaconfig_pkt);

  my $status    = $req_xtra_data_send->GetStatus();
  my $client_id = $req_xtra_data_send->GetClientId();

  print main::LOGFILE ("Status  =" . sprintf("%d",$status). ",Client Id =". sprintf("%d", $client_id) . "\n");   
 
  print main::LOGFILE ("Packet segment sent\n");
  
} # End  of sub XtraPktSend


################################################################################
# METHOD:
#   usage
#
# DESCRIPTION:
#   Starts injection Xtra Bin File to Diag
#
# RETURN VALUE:
#   None
#
################################################################################
sub GpsXtraBinFileSend
{

  print main::LOGFILE ("\n =============GpsXtraBinFileSend START =========\n\n");  
  ################################################################################
  # Start Xtra file segmenation and send
  ################################################################################
  
  use integer;
  #open the input file
  my $savefile = "xtra.bin";
  my $timeslept = 0;    
  my $XtraInjectCmdRsp = 0;
  
  open (MYBINFILE, "xtra.bin") or die "\n", $!, "Does Xtra file exist\?\n\n";

  binmode(MYBINFILE);

  my $buf_size = $main::MAXSEG_SIZE; 
  ### ready a buffer for the data
  ### for as many parts as there are, read
  ### the amount of data, then write it to
  ### the appropriate DIAG Pkt.
  $XtraPartCount = 0;
  $TotalXtraPkts = $main::XTRAFILESIZE  / $main::MAXSEG_SIZE ;
  if ($main::MAXSEG_SIZE *  $TotalXtraPkts  != $main::XTRAFILESIZE )
  {
    $TotalXtraPkts = $TotalXtraPkts + 1; #Plus one packet for the last fractional packet
  }
  my $bytesread;
  
  #No need to WAIT FOR THE XTRA End event GPSONEXTRA_START_DOWNLOAD Event(01366)
  
  #printf  main::LOGFILE  ("\n GpsXtraBinFileSend readbytes:%d filesize:%d  lfilezize:%d \n", $BytesReadFromFile,$main::XTRAFILESIZE,$lFileSize);
  #while(<MYBINFILE>)  
  while(1) 
  {           
    if ( ($bytesread = read  MYBINFILE, $XtraPartBuf, $buf_size ) != 0 )
    {
     print main::LOGFILE "\n$bytesread bytes read\n";
    }      
    else #if ( $bytesread == 0 )    
    {
      last; ## break out of the loop
    }
    $XtraPartLen = $bytesread ; 
    $XtraPartCount++;     


    $main::PrevIndex = $main::qxdm_app2->GetClientItemCount( $main::ReqHandle ) - 1;        
    
    # Send out this framgent 
    &XtraPktSend; 
    
    #reset the wait timer for Data Ack event
    $timeslept = 0;    
    
    while(1)
    {
      #let us wait for the DATA ACK Event  EVENT_GPSONEXTRA_DATA_ACK = 0x0840,         
      usleep(100000); # lets wait for 100 milliseconds
        
      $timeslept = $timeslept + 100000;
    
      #if we have waited for  more than 5 seconds, then let us abort Saying that
      # Xtra injection is not supported on this target.
      if ( $timeslept == 5000000 )
      {
        printf  main::LOGFILE  ("\nABORTING Test due to TIMEOUT of Inject Xtra Data Cmd, This build doesn't support XTRA diag Cmds!!\n\n");	                


        print ("\n\n\n~~~~~~~~~~~~~~~~~~~~~~ERROR~~~~~~~~~~~~~~~~~~~~~\n\n\n");                
        print ("\nABORTING Test due to TIMEOUT of Inject Xtra Data Cmd, This build doesn't support XTRA diag Cmds!!\n\n");	                        
        print ("\n\n\n~~~~~~~~~~~~~~~~~~~~~~ERROR~~~~~~~~~~~~~~~~~~~~~\n\n\n");                
        $XtraInjectCmdRsp = -1;
        last;
      }

      $main::CurrIndex = $main::qxdm_app2->GetClientItemCount( $main::ReqHandle ) - 1; # Get index of last item in client
      #printf  main::LOGFILE  ("Waiting for XTRA Data ACK 1:%d 2:%d\n",$main::CurrIndex,$main::PrevIndex);          
      
      if( $main::CurrIndex  == $main::PrevIndex)
      {
        #printf  main::LOGFILE  ("Not Received Event still  when waiting for Data Ack Event 1:%d 2:%d \n\n",$main::CurrIndex ,$main::PrevIndex);   	
        next;# go again to the top of the loop and wait for next event
      }
      #else proces the new item

      my $Item = $main::qxdm_app2->GetClientItem($main::ReqHandle, $main::CurrIndex ); # Get Item
      my $ItemKey = $Item->GetItemKeyText();
               
      if ($ItemKey eq "[01927]") # 0x787
      {
	printf main::LOGFILE  ("Received Event GPS_PD_XTRA_DATA_ACK_RECEIVED!!\n\n");
   	$main::PrevIndex = $main::CurrIndex;	    
        last; ## break out of the loop
      }
      else
      {
        printf  main::LOGFILE  ("Received Event:%d when waiting for Data Ack Event !!\n\n",$ItemKey);   	
        $main::PrevIndex = $main::CurrIndex;
        next; #This was not the event we are waiting for, go again to the top of loop to wait for next event.
      }      	 
    } # end of waiting for data ack event
    
      if ( $XtraInjectCmdRsp == -1)
      {
        # lets break out of the test since the Targetr is not responding to XTRA DATA injections.
        last;
      }
  } # end of processing complete bin file

  print main::LOGFILE ("\n =============GpsXtraBinFileSend END =========\n\n");  
  ### we're done spliting the input file
  close MYBINFILE;

  ## Start of wait for Download event      
  #LETS WAIT FOR THE XTRA End event EVENT_GPSONEXTRA_END_DOWNLOAD = 01367
  my $timeslept = 0;
  while(1)
  {
    usleep(100000); # lets wait for 100 milliseconds

    $timeslept = $timeslept + 100000;
    
    #if we have waited for  more than 1.5 seconds, then let us abort.
    if ( $timeslept == 5000000 )
    {
      printf  main::LOGFILE  ("TIMEOUT : Failed to Receive EVENT_GPSONEXTRA_END_DOWNLOAD!!\n\n");	                
      last;
    }
    #wait for the next event.  
    $main::CurrIndex = $main::qxdm_app2->GetClientItemCount( $main::ReqHandle ) - 1; # Get index of last item in client

    #printf  main::LOGFILE  ("Waiting for XTRA Data ACK 1:%d 2:%d\n",$main::CurrIndex,$main::PrevIndex);          
    if( $main::CurrIndex  == $main::PrevIndex)
    {
      next;# go again to the top of the loop and wait for next event
    }
    #else proces the new item

    my $Item = $main::qxdm_app2->GetClientItem($main::ReqHandle, $main::CurrIndex ); # Get Item
    my $ItemKey = $Item->GetItemKeyText();

    if ($ItemKey eq "[01377]")
    {
      if(IsErrorResponse( $Item ) == 0)
      {
        printf  main::LOGFILE  ("Received Event EVENT_GPSONEXTRA_END_DOWNLOAD with success!!\n\n");	      
      }
      else
      {
	 printf  main::LOGFILE  ("Received Event EVENT_GPSONEXTRA_END_DOWNLOAD with failure!!\n\n");	      	      
	 my $Txt = "Error response received - " . $Item->GetItemName();
	 my $Status = GetSubsysV2ErrorCode( $Item );
	 if ($Status != 0)
	 {
	    $Txt .= " [" . $Status . "]";
	 }
         print main::LOGFILE ( "\n  Xtra Final Download status is : " . sprintf("%s",$Txt) . " \n\n" );
       } # End of error  check
       last; ## break out of the loop
    }
    else
    {
      $main::PrevIndex = $main::CurrIndex;
      next; #This was not the event we are waiting for, go again to the top of loop to wait for next event.
    }      	 
    
  }# End of waiting for  End event EVENT_GPSONEXTRA_END_DOWNLOAD
  
} # End of GpsXtraBinFileSend 
    

# ===========================================================================
# METHOD:
#    IsErrorResponse
# 
# DESCRIPTION:
#    Is the item an error response?
#    
# PARAMETERS:
#    $Item        [ I ] - $Item to check
#
# RETURN VALUE:
#    bool
# ===========================================================================
sub IsErrorResponse
{
  my $Item = shift;
  my $Status = 0;

  # Assume failure
  my $RC = 0;
   
  if ($Item == 0)
  {
    return $RC;
  }

  my $ItemKey = $Item->GetItemKeyText();
  my $ItemType = $Item->GetItemType();

  $Status = GetSubsysV2ErrorCode( $Item );
  if ($Status != 0)
  {
   $RC = 1;
  }
  return $RC;
}

# ===========================================================================
# METHOD:
#    GetSubsysV2ErrorCode
# 
# DESCRIPTION:
#    Return the error code from a subsystem dispatch V2 response
#    
# PARAMETERS:
#    $Item        [ I ] - $Item to extract error code from
#       
# RETURN VALUE:
#    Error code (as per the standard 0 means no error)
# ===========================================================================
sub GetSubsysV2ErrorCode
{
  my $Item = shift;
   
  # Assume failure
  my $RC = 0;

  if ($Item == 0)
  {
    return $RC;
  }

  my $ItemKey = $Item->GetItemKeyText();
  my $ItemType = $Item->GetItemType();
   
  # Parse out the status (UINT32, 32 bits, from bit offset [header] 32)
  #$RC = $Item->GetItemFieldValue( 6, 32, 32, true );
  $RC = $Item->GetItemFieldValue( 4, 32, 32, true );
   
  my $RC1 = $Item->GetItemFieldValue( 6, 32, 32, true );
  my $RC2 = $Item->GetItemFieldValue( 1, 32, 32, true );
  my $RC3 = $Item->GetItemFieldValue( 2, 32, 32, true );
  my $RC4 = $Item->GetItemFieldValue( 3, 32, 32, true );
  my $RC5=  $Item->GetItemFieldValue( 4, 32, 32, true );
  my $RC6 = $Item->GetItemFieldValue( 5, 32, 32, true );
  my $RC7 = $Item->GetItemFieldValue( 7, 32, 32, true );
  my $RC8 = $Item->GetItemFieldValue( 8, 32, 32, true );
  my $RC9 = $Item->GetItemFieldValue( 9, 32, 32, true );
  
   
  #printf  main::LOGFILE  ("GetSubsysV2ErrorCode err:%d !!\n\n",$RC );	      	      
  #printf  main::LOGFILE  ("GetSubsysV2ErrorCode 1:%d 2:%d  3:%d!!\n\n",$RC1,$RC2,$RC3 );	      	      
  #printf  main::LOGFILE  ("GetSubsysV2ErrorCode 4:%d 5:%d  6:%d!!\n\n",$RC4,$RC5,$RC6 );	      	         
  #printf  main::LOGFILE  ("GetSubsysV2ErrorCode 7:%d 8:%d  9:%d!!\n\n",$RC7,$RC8,$RC9 );	      	           
   
  return $RC;
}

############################################
#Return value for this perl module
1;
