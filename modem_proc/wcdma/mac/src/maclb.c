/*==========================================================================================

       M A C  D U M M Y  S I G N A L I N G  H A N D L E R

GENERAL DESCRIPTION

  This module implements dummy signaling message transmission on DCCH 
  (intended for uplink RF testing for which Reference Measurement 
  Channels are used) described in section 5.3.2.8 of TS34.109. 

  The pre-conditions for enabling the dummy signaling messages on DCCH : 

   - UE should be in test loop mode 1 or 2
   - DCCH dummy mode is enabled in UE (by SS entity)

  When dummy signaling is enabled and there is no DCCH data to be sent, 
  UE sets all bits in the uplink DCH transport associated with a DCCH 
  to 1.

  Please refer to TS34.109 section 5.3.2.8 for more information on dummy 
  signaling on DCCH with RMC.

  This module contains functions to :

  - Initialize internal dummy signaling info maintained by MAC
  - Reselect a suitable TFC for transmission of dummy signaling messages 
  - Build UL DCH transport block with dummy data 

Copyright (c) 2001,2002 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2003, 2005,2008-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

============================================================================================*/

/*==========================================================================================

            EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when      who   what, where, why
--------  ---   -------------------------------------------------------------
07/26/14   rkmk    Reduce image size by using right macro depending upon number of parameters
06/28/10  mrg  MAC-R99 ZI memory reduction changes
05/07/09  ssg   Updated Copyright Information
04/29/09  av    Fixed compiler warnings in L2 
03/09/05  sk    Fixed all lint errors.
03-07-16  tjw   Prevent compiler warning C3040E: no external declaration in translation unit
05/11/02  mks   Created the file
08/13/02  mks   Removed the check that disallows transmission of dummy signaling 
                messages when aleast one transport channel is ready

============================================================================================
INCLUDE FILES
============================================================================================*/
#include "wcdma_variation.h"
#include  "maclb.h"
#ifdef FEATURE_DUMMY_SIGNALING
#error code not present
#else
void feature_dummy_signaling_dummy(void) {}    /* To prevent compiler warning C3040E: no external declaration in translation unit */
#endif
