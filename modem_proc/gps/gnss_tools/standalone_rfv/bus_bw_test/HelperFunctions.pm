# 
#$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/standalone_rfv/bus_bw_test/HelperFunctions.pm#1 $ $DateTime: 2016/12/13 07:59:45 $ $Author: mplcsds1 $
######################################################################
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
######################################################################
#
# Description:
# This is a support file for the StartPrescDwell.pl scripts.
# It should be located in the same directory as that file.
#
######################################################################

package HelperFunctions;
require Exporter;

@ISA     = qw(Exporter);
@EXPORT  = qw( GetPathFromScript
               GenerateFileName
               SetQXDM
               SetQXDM2
               DumpItemDetails
               Connect
               Disconnect
               GetSubsysV2ErrorCode
               IsErrorResponse
               SendRequestAndReturnResponse
               PhoneOffline
               ResetPhone
               null
               false
               true
               ITEM_TYPE_DIAG_ERR
               ITEM_TYPE_DIAG_RX
               ITEM_TYPE_DIAG_TX
               ITEM_TYPE_GPS
               ITEM_TYPE_EVENT
               ITEM_TYPE_LOG
               ITEM_TYPE_MESSAGE
               ITEM_TYPE_STRING
               ITEM_TYPE_OTA_LOG
               ITEM_TYPE_SUBSYS_RX
               ITEM_TYPE_SUBSYS_TX
               SERVER_DISCONNECTED
               SERVER_PRECONNECT
               SERVER_CONNECTED
               SERVER_PREDISCONNECT
               SERVER_PLAYBACK
               FIELD_TYPE_BOOL
               FIELD_TYPE_INT8
               FIELD_TYPE_UINT8
               FIELD_TYPE_INT16
               FIELD_TYPE_UINT16
               FIELD_TYPE_INT32
               FIELD_TYPE_UINT32
               FIELD_TYPE_INT64
               FIELD_TYPE_UINT64
               FIELD_TYPE_STRING_A
               FIELD_TYPE_STRING_U
               FIELD_TYPE_STRING_ANT
               FIELD_TYPE_STRING_UNT
               FIELD_TYPE_FLOAT32
               FIELD_TYPE_FLOAT64 );

use strict;
use Win32::OLE;
use File::Spec;
use Cwd 'abs_path';
use Win32::OLE::Variant;

# ===========================================================================
# FILE: 
#    HelperFunctions.pm
#
# DESCRIPTION:
#    Implementation of functions used in myious automation script samples.
#
# Copyright (C) 2006 QUALCOMM Incorporated. All rights reserved.
#                    QUALCOMM Proprietary/GTDR
#
# All data and information contained in or disclosed by this document is
# confidential and proprietary information of QUALCOMM Incorporated and all
# rights therein are expressly reserved.  By accepting this material the
# recipient agrees that this material and the information contained therein
# is held in confidence and in trust and will not be used, copied, reproduced
# in whole or in part, nor its contents revealed in any manner to others
# without the express written permission of QUALCOMM Incorporated.
# ===========================================================================
#
# ---------------------------------------------------------------------------
#  Definitions
# ---------------------------------------------------------------------------

#  Miscellaneous constants
use constant null                => 0;
use constant false               => 0;
use constant true                => 1;

# $Item type constants
use constant ITEM_TYPE_DIAG_ERR  => 0;
use constant ITEM_TYPE_DIAG_RX   => 1;
use constant ITEM_TYPE_DIAG_TX   => 2;
use constant ITEM_TYPE_GPS       => 3;
use constant ITEM_TYPE_EVENT     => 4;
use constant ITEM_TYPE_LOG       => 5;
use constant ITEM_TYPE_MESSAGE   => 6;
use constant ITEM_TYPE_STRING    => 7;
use constant ITEM_TYPE_OTA_LOG   => 8;
use constant ITEM_TYPE_SUBSYS_RX => 9;
use constant ITEM_TYPE_SUBSYS_TX => 10;

# DIAG server states
use constant SERVER_DISCONNECTED  => 0;
use constant SERVER_PRECONNECT    => 1;
use constant SERVER_CONNECTED     => 2;
use constant SERVER_PREDISCONNECT => 3;
use constant SERVER_PLAYBACK      => 4;

# Field type constants
use constant FIELD_TYPE_BOOL       => 0;
use constant FIELD_TYPE_INT8       => 1;
use constant FIELD_TYPE_UINT8      => 2;
use constant FIELD_TYPE_INT16      => 3;
use constant FIELD_TYPE_UINT16     => 4;
use constant FIELD_TYPE_INT32      => 5;
use constant FIELD_TYPE_UINT32     => 6;
use constant FIELD_TYPE_INT64      => 7;
use constant FIELD_TYPE_UINT64     => 8;
use constant FIELD_TYPE_STRING_A   => 9;
use constant FIELD_TYPE_STRING_U   => 10;
use constant FIELD_TYPE_STRING_ANT => 11;
use constant FIELD_TYPE_STRING_UNT => 12;
use constant FIELD_TYPE_FLOAT32    => 13;
use constant FIELD_TYPE_FLOAT64    => 14;

# IQXDM/IQXDM2 interfaces used by function in this file (SetQXDM/SetQXDM2
# must be called prior to using any function in this file that requires
# each interface)
my $IQXDM = null;
my $IQXDM2 = null;

# ---------------------------------------------------------------------------
# Functions
# ---------------------------------------------------------------------------

# ===========================================================================
# METHOD:
#    SetQXDM
#
# DESCRIPTION:
#    Set IQXDM interface object being used
#
# PARAMETERS:
#    QXDM       [ I ] - QXDM interface object to use
#
# RETURN VALUE:
#    None
# ===========================================================================
sub SetQXDM
{
   my $QXDM = shift;

   $IQXDM = $QXDM;
} 

# ===========================================================================
# METHOD:
#    SetQXDM2
#
# DESCRIPTION:
#    Set IQXDM2 interface object being used
# 
# PARAMETERS:
#    QXDM2       [ I ] - QXDM2 interface object to use
#
# RETURN VALUE:
#    None
# ===========================================================================
sub SetQXDM2
{
   my $QXDM2 = shift;

   $IQXDM2 = $QXDM2;
}

# ===========================================================================
# METHOD:
#   GetPathFromScript
#
# DESCRIPTION:
#   Get path from current script file name
#
# RETURN VALUE:
#   String ("" upon error)
# ===========================================================================
sub GetPathFromScript()
{
   # Assume failure
   my $Path = "";
   my $Txt = "";

   $Path = abs_path( $ENV{'PWD'} );
   $Path = File::Spec->rel2abs( $Path ) ;
   if (length $Path <= 0)
   {
      $Txt = "Unable to get folder name";
      print "\n$Txt\n";
   }
   else
   {
      if (!($Path =~ /\\$/))
      {
         $Path .= "\\";
      }
   }

   return $Path;
}

# ===========================================================================
# METHOD:
#    GenerateFileName
#
# DESCRIPTION:
#    Generate a unique file name
#
# PARAMETERS:
#    Path        [ I ] - Path to use ("" means use QXDM automation folder)
#    Extension   [ I ] - Desired file extension (leading "." must be included)
#
# RETURN VALUE:
#    String ("" upon error)
# ===========================================================================
sub GenerateFileName
{
   my ( $Path, $Extension ) = @_;
   my $FileName = "";

   if (length( $Path ) <= 0)
   {
      # Use script path
      $Path = GetPathFromScript();
   }

   #  Get GM time string (Wed May 31 03:03:22 2006)
   my $tm = gmtime();

   # Replace ':' with '.'
   $tm =~ s/\:/\./g;

   # Remove whitespace
   my @a = split( / /, $tm );

#  print ("\n", 

   # Rearrange date
   my $TodaysDate = "$a[0]_$a[3]_$a[1]$a[2]_$a[5]_$a[4]_UTC";#$qxdm_app->LogMaskOn(0x36f, 1);
   print("$a[0] $a[3] $a[1] $a[2] $a[5] $a[4]\n\n");
   $FileName = $TodaysDate . $Extension;

   return $FileName;
}

# ===========================================================================
# METHOD:
#    DumpItemDetails
#
# DESCRIPTION:
#    Dump out item details for the given item
#
# PARAMETERS:
#    $Item        [ I ] - $Item to dump out details for
#    NamePadding  [ I ] - Pad field name output length to X characters
#
# RETURN VALUE:
#    None (Text written to StdOut)
# ===========================================================================
sub DumpItemDetails
{
   my ( $Item, $NamePadding ) = @_;

   if ($Item == null)
   {
      return;
   }

   my $ItemTS =  $Item->GetItemSpecificTimestampText( false, true );
   my $ItemKey = $Item->GetItemKeyText();
   my $ItemName = $Item->GetItemName();

   my $Txt = $ItemTS . " " . $ItemKey . " " . $ItemName;
   print( "$Txt\n" );

   my $ItemFields = $Item->GetItemFields();
   if ($ItemFields != null)
   {
      my $FieldCount = $ItemFields->GetFieldCount();
      for (my $f = 0; $f < $FieldCount; $f++)
      {
         my $Name = $ItemFields->GetFieldName( $f, true );
         my $Value = $ItemFields->GetFieldValueText( $f );

         my $format = "   %-${NamePadding}s = %s\n";
         printf( $format, $Name, $Value );
      }
   }
}

# ===========================================================================
# METHOD:
#    Connect
# 
# DESCRIPTION:
#    Connect to the given port, waiting for the connection come up
#
#    NOTE: Requires IQXDM be set via SetQXDM() call
#
# PARAMETERS:
#    Port        [ I ] - Port to connect to (1, 2, 3, etc.)
#
# RETURN VALUE:
#    bool
# ===========================================================================
sub Connect
{
   my $Port = shift;

   # Assume failure
   my $RC = false;
   my $Txt = "";

   if ($Port == 0)
   {
      $Txt = "Invalid COM port specified" . $Port;
      print( "$Txt\n" );

      return $RC;
   }

   # Connect to our desired COM port
   $IQXDM->{COMPort} = $Port;

   # Wait until DIAG server state transitions to connected
   # (we do this for up to five seconds)
   my $WaitCount = 0;
   my $ServerState = SERVER_DISCONNECTED;
   while ($ServerState != SERVER_CONNECTED && $WaitCount < 5)
   {
      sleep( 1 );

      $ServerState = $IQXDM2->GetServerState();
      $WaitCount++;
   }

   if ($ServerState == SERVER_CONNECTED)
   {
      # Success!
      $Txt = "QXDM connected to COM" . $Port;
      print( "$Txt\n" );
      $RC = true;
   }
   else
   {
      $Txt = "QXDM unable to connect to COM" . $Port;
      print( "$Txt\n" );
   }

   return $RC;
}


# ===========================================================================
# METHOD:
#    Disconnect
#
# DESCRIPTION:
#    Disconnect, waiting for the action to complete
#
#    NOTE: Requires IQXDM be set via SetQXDM() call
#
# RETURN VALUE:
#    bool
# ===========================================================================
sub Disconnect
{
   # Assume failure
   my $RC = false;

   # Disconnect
   $IQXDM->{COMPort} = 0;

   # Wait until DIAG server state transitions to disconnected
   # (we do this for up to five seconds)
   my $WaitCount = 0;
   my $ServerState = SERVER_CONNECTED;
   while ($ServerState != SERVER_DISCONNECTED && $WaitCount < 5)
   {
      sleep( 1 );

      $ServerState = $IQXDM2->GetServerState();
      $WaitCount++;
   }

   my $Txt = "";
   if ($ServerState == SERVER_DISCONNECTED)
   {
      # Success!
      $Txt = "QXDM successfully disconnected";
      print( "$Txt\n" );
      $RC = true;
   }
   else
   {
      $Txt = "QXDM unable to disconnect";
      print( "$Txt\n" );
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
   my $RC = false;

   if ($Item == null)
   {
      return $RC;
   }

   my $ItemKey = $Item->GetItemKeyText();
   my $ItemType = $Item->GetItemType();
   if ($ItemType == ITEM_TYPE_SUBSYS_RX)
   {
      if ($ItemKey eq "[128]")
      {
         # Parse out the status (UINT32, 32 bits, from bit offset [header] 32)
         $RC = $Item->GetItemFieldValue( 6, 32, 32, true );
      }
   }

   return $RC;
}

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

   # Assume failure
   my $RC = false;
   
   if ($Item == null)
   {
      return $RC;
   }

   my $ItemKey = $Item->GetItemKeyText();
   my $ItemType = $Item->GetItemType();
   if ($ItemType == ITEM_TYPE_DIAG_RX)
   {
      if ($ItemKey eq "[019]" ||
          $ItemKey eq "[020]" ||
          $ItemKey eq "[021]" ||
          $ItemKey eq "[022]" ||
          $ItemKey eq "[023]" ||
          $ItemKey eq "[024]" ||
          $ItemKey eq "[066]" ||
          $ItemKey eq "[071]")
      {
         $RC = true;
      }
   }
   elsif ($ItemType == ITEM_TYPE_SUBSYS_RX)
   {
      my $Status = GetSubsysV2ErrorCode( $Item );
      if ($Status != 0)
      {
         $RC = true;
      }
   }

   return $RC;
}

# ===========================================================================
# METHOD:
#    SendRequestAndReturnResponse
# 
# DESCRIPTION:
#    Send a request/wait for and return the response
#    
#    NOTE: Requires IQXDM2 be set via SetQXDM2() call
#
# PARAMETERS:
#    RequestName [ I ] - DIAG entity name of the request
#    RequestArgs [ I ] - Arguments used to populate the request (may be "")
#    DumpItems   [ I ] - Dump out item details to StdOut?  If so then set 
#                        this to the field name width, if not then set to
#                        zero
#
# RETURN VALUE:
#    The response item (null upon error)
# ===========================================================================
sub SendRequestAndReturnResponse
{
   my ( $RequestName, $RequestArgs, $DumpItems ) = @_;
   my $Txt = "";

   # We need to be connected
   my $ServerState = $IQXDM2->GetServerState();
   if ($ServerState != 2)
   {
      $Txt = "Unable to send request - " . $RequestName;
      print( "$Txt\n" );
      return null; 
   }

   # Create a client
   my $ReqHandle = $IQXDM2->RegisterQueueClient( 256 );
   if ($ReqHandle == 0xFFFFFFFF)
   {
      $Txt = "Unable to create client to send request - " . $RequestName;
      print( "$Txt\n" );

      return null;
   }

   # One request with a 1s timeout
   my $ReqID = $IQXDM2->ClientRequestItem( $ReqHandle,
                                           $RequestName,
                                           $RequestArgs,
                                           1,
                                           1000,
                                           1,
                                           1 );

   if ($ReqID == 0)
   {
      $Txt = "Unable to schedule request - " . $RequestName;
      print( "$Txt\n" );
      
      $IQXDM2->UnregisterClient( $ReqHandle );
      return null;
   }

   my $Secs = 0;
   my $Items = 0;

   # Wait for the response (we know the response has arrived when the number
   # of items in our client goes to two)
   for ($Secs = 0; $Secs < 5; $Secs++)
   {
      # Sleep for 1s
      sleep( 1 );

      # How many items do we have?
      $Items = $IQXDM2->GetClientItemCount( $ReqHandle );
      if ($Items == 2)
      {
         last;
      }
   }

   # Dump out everything in the client?
   if ($DumpItems > 0)
   {
      for (my $Index = 0; $Index < $Items; $Index++)
      {
         my $Item = $IQXDM2->GetClientItem( $ReqHandle, $Index );
         if ($Item != null)
         {
            DumpItemDetails( $Item, $DumpItems );
         }
      }
   }

   if ($Items == 2)
   {
      my $Item = $IQXDM2->GetClientItem( $ReqHandle, 1 );
      if (IsErrorResponse( $Item ) == false)
      {
         $IQXDM2->UnregisterClient( $ReqHandle );
         return $Item;
      }
      else
      {
         $Txt = "Error response received - " . $Item->GetItemName();
         my $Status = GetSubsysV2ErrorCode( $Item );
         if ($Status != 0)
         {
            $Txt .= " [" . $Status . "]";
         }

         print( "$Txt\n" );
         
         $IQXDM2->UnregisterClient( $ReqHandle );
         return null;
      }
   }
   elsif ($Items == 1)
   {
      $Txt = "Timeout waiting for response to request - " . $RequestName;
      print( "$Txt\n" );

      $IQXDM2->UnregisterClient( $ReqHandle );
      return null;
   }

   $Txt = "Error sending request - " . $RequestName;
   print( "$Txt\n" );

   $IQXDM2->UnregisterClient( $ReqHandle );
   return null;
}

# ===========================================================================
# METHOD:
#    PhoneOffline
# 
# DESCRIPTION:
#    Set phone offline
#    
#    NOTE: Requires IQXDM be set via SetQXDM() call
#    
# RETURN VALUE:
#    bool
# ===========================================================================
sub PhoneOffline
{
   # Assume failure
   my $RC = false;

   print( "Set phone offline ... " );

   # We need to be connected for this to work
   my $ServerState = $IQXDM2->GetServerState();
   if ($ServerState != SERVER_CONNECTED)
   {
      print( "failed (not connected)\n" );
      return $RC;
   }

   my $Status = $IQXDM->OfflineDigital();
   if ($Status == 1)
   {
      print( "succeeded\n" );
      $RC = true;
   }
   else
   {
      print( "failed\n" );
   }

   return $RC;
}

# ===========================================================================
# METHOD:
#    ResetPhone
# 
# DESCRIPTION:
#    Attempt to reset the phone and wait for it to come back up
#    
#    NOTE: Requires IQXDM/IQXDM2 be set via SetQXDM/SetQXDM2() call
#    
# RETURN VALUE:
#    bool
# ===========================================================================
sub ResetPhone
{
   # Assume failure
   my $RC = false;
   my $Txt = "";

   print( "Reset phone ... " );
   
   # We need to be connected for this to work
   my $ServerState = $IQXDM2->GetServerState();
   if ($ServerState != SERVER_CONNECTED)
   {
      print( "failed (not connected)\n" );
      return $RC;
   }

   # Reset the phone
   my $Status = $IQXDM->ResetPhone();
   if ($Status == 1)
   {
      print( "succeeded\n" );
      $RC = true;
   }
   else
   {
      print( "failed\n" );
      return $RC;
   }

   print( "Waiting for phone to restart ... " );

   # The phone should first disconnect
   my $WaitCount = 0;
   $ServerState = SERVER_CONNECTED;
   while ($ServerState != SERVER_DISCONNECTED && $WaitCount < 5)
   {
      sleep( 1 );

      $ServerState = $IQXDM2->GetServerState();
      $WaitCount++;
   }

   if ($ServerState != SERVER_DISCONNECTED)
   {
      print( "failed\n" );
      return $RC;
   }

   # Now wait until DIAG server state transitions back to connected
   # (we do this for up to twenty seconds)
   $WaitCount = 0;
   $ServerState = SERVER_DISCONNECTED;
   while ($ServerState != SERVER_CONNECTED && $WaitCount < 20)
   {
      sleep( 1 );

      $ServerState = $IQXDM2->GetServerState();
      $WaitCount++;
   }

   if ($ServerState == SERVER_CONNECTED)
   {
      $Txt = "succeeded";
      print( "$Txt\n" );
      $RC = true;
   }
   else
   {
      $Txt = "failed";
      print( "$Txt\n" );
   }

   return $RC;
}

1;
