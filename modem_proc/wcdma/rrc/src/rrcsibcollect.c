/*===========================================================================
             R R C   S I B   C O L L E C T I O N   M O D U L E  

DESCRIPTION
  
  This module is part of RRC SIB Processing procedure. This module performs
  the sib collection part of the RRC procedure. SIB collection process includes
  the following - 
    1) receiving the System Information Message 
    2) Saving the SIB segments in the temporary SIBs holder.
    3) Concatenating multiple segments belonging to the same SIB if needed.
     
  This module does not interact directly with other RRC modules except
  the SIB processing modules. 
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000, 2001, 2003, 2004, 2006-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcsibcollect.c_v   1.13   28 Sep 2001 13:44:50   krishnan  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcsibcollect.c#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
09/16/14   rmn     Resolved compilation errors in WRRC for LTE removal flavor
07/21/14   sn      f3 reduction changes
06/23/14   vi      Made changes to skip SIB7 reading if SIB7 expiration timer is stil running
05/23/13   sr      Added support for SIB18/SIB19 deferring
01/03/13   ad      Made changes to reduce the redundant F3s
10/08/12   sn      Changes to decode SIB7 only once
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
05/02/12   sn      Changes to build and send SIB schedule change indication after 
                   receiving SIB7 in FACH state
03/20/12   mn      Corrected bad format log prints
02/16/12   gv      Fixed compilation issue
02/15/12   gv      Added support to log new event when complete SIB is received
05/03/11   rl      Fixed the lint error
04/25/11   rl      Fixed the KW error
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
08/03/10   ss      Fixed KW issues
07/01/10   rm      Fixed compilation issues
06/29/10   rm      Added code to support LTE<->WCDMA Inter RAT
06/18/10   gkg     Fixed Compilation warnings.
03/12/10   dm      Added code for supporting Deferred SIB reading feature.
01/21/09   dm      Made changes to not process a SIB segment if it's scheduling info
                   is not yet received.
10/23/09   dm      Fixed Klocwork warnings
07/29/09   dm      Added code to not drop a valid SIB segment whose scheduling info 
                   is present in SB1/2 and comes along with it in the same BCCH-BCH block.
06/30/09   dm      Added support for updating UL Interference value with the latest
                   received SIB7 content.
05/11/09   dm      Feature to dynamically allocate sib_event ptr
05/08/09   ss      Updated Copyright Information
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
10/07/08   ns      Added code to store SIB segments received before the scheduling block. 
                   This appears as part of the feature FEATURE_RRC_TSIB_OPTIMIZATION.
04/24/08   ns      Fixed klockwork errors
07/09/07   kp      Added support for Tsib optimization db. With this optimization 
                   SIB will be able to store & use the SIB segments received out of
                   sync. Current optimization will only take care of  store & use the
                   SIB segments received out of sync after receiving MIB & Scheduling block’s.
04/26/06   vr      Support for SIB5bis
10/26/04   kc      Optimization to read SIB-segment from the last received segment rather
                   than start from first-segment if we receive first segment during a
                   subsequent sib-repetition-cycle.
04/29/03   ram     Linted file and fixed lint errors.
09/24/01   ram     Additional debug information for SIBs - we will print
                   the SFN in which a SIB was completely received. This is
                   featurized under FEATURE_RRC_SIB_DEBUG.
09/05/01   ram     Improved debug messages for segment processing. Fixed
                   the use the constant byte and bit lenghts of complete sibs 
                   in process_complete_sib. Updated comments.
07/27/01   ram     In process_sys_info_msg when more than one complete SIB
                   is received in a frame, made it possible for each 
                   completed SIB to be decoded right away.
06/11/01   ram     Some debug messages were displayed each BCCH frame. Removed
                   some of these messages and changed others to MSG_LOW so that
                   there aren't too many debug messages every frame.
04/09/01   ram     The maximum seg_index for a segmented SIB has been clarified
                   in the December version of the spec. Added code to verify
                   the maximum seg_index for a segmented SIB.
03/23/01   ram     Updated file to work with new LED ASN.1 header files.
02/20/01   ram     When a last segment is processed in the function process_sys
                   info_msg, we were not indicating that a SIB has been
                   received completely. This has been fixed now.
02/08/01   ram     Added support for concatenating SIBs that have been segmented
                   by the UTRAN. Added support for processing all the segment
                   types specified in the spec. Enhanced error detection for
                   segment processing.
01/25/01   ram     In process_sys_info_msg, added cases for one complete SIB 
                   and one last segment in the check for segment combinations.
                   Added function process_short_complete_sib to process a 
                   system information message that has a "Complete SIB Short"
                   segment type. Expanded seg_proc_stat_e_type to include 
                   SEG_PROC_NONE and SEG_PROC_DUPLICATE. Updated copyright.
12/15/00   ram     Added functionality to determine when a SIB has been completely
                   received (i.e. all segments are received). This info is returned
                   to the sib event controller.
12/01/00   ram     Removed tabs introduced by editor. Added sib_events_index_e_type
                   as a parameter to process_sys_info_msg since this function
                   writes sib segments into the sib events database.
11/06/00   ram     Added comments in all the function headers. Created
                   a segment status enum type for the segment processing
                   functions.
10/30/00   ram     Created file.

===========================================================================*/


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "bit.h"
#include "comdef.h"
#include "err.h"
#include "msg.h"
#include "rrcasn1.h"
#include "rrcsibproci.h"




/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */



/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
#ifdef FEATURE_WCDMA_HS_FACH_DRX
/* Keeps track of sfn prime of latest received BCH block */
uint32 curr_sfn = 0;
#endif
  /*tsib_optimization_db used to hold sib segments received in out of order*/
rrc_temp_sib_optimization_holder_type tsib_optimization_db[MAX_TEMP_SIB_SEGMENTS_TO_HOLD];

  /*Look up table used to indicate whether a sib segment 
    of a sib tpye is presnt in temp sib optimization db or not*/
boolean tsib_opt_db_look_up_table[rrc_MAX_SIB];

#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
/*Look up table for holding the value tag and segments of the deferred SIBs.*/
rrc_deferred_sib_info_type deferred_sib_info;
#endif
/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */



/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */


/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/*===========================================================================

FUNCTION PROCESS_FIRST_SEG

DESCRIPTION

  This function processes a "First Segment" of a SIB. The data from the segment
  is copied to the appropriate place in the temp sib holder based on the SIB
  type.
  
DEPENDENCIES

  None.

RETURN VALUE

  A value of type seg_proc_stat_e_type indicating the status of the
  processing.

SIDE EFFECTS

  None.

===========================================================================*/
static seg_proc_stat_e_type process_first_seg
(
  rrc_FirstSegment *seg, /* Pointer to the received First Segment */ 
  sib_events_index_e_type index,    /* Index into the SIB events database
                                   for storing SIBs received in this msg */
  uint32 sfn_prime  /*SFN prime at which this sib segment received*/

)
{
  rrc_SIB_type sib;  /* To store the received SIBs type */
  void *src;         /* Source pointer for the b_copy function */
  void *dest;        /* Destination pointer for the b_copy function */
  seg_proc_stat_e_type status = SEG_PROC_SUCCEEDED;/*This will say status of processing*/

  if(EVENT_INDEX_VALID(index) ==FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    status = SEG_PROC_FAILED;
    return status;
  }
  if(seg->sib_Type == (uint32)rrc_SIB_Type_extensionType)
  {
    sib = rrcsib_map_extension_sib_to_internal_sibtype(0,sfn_prime);
  }
  else
  {
    /* Copy the received sib type. */
    sib = rrcsib_map_asn1_sibtype_to_int_sibtype(seg->sib_Type);
  }

  if(sib == rrc_MAX_SIB)
  {
    status = SEG_PROC_FAILED;
    return status;
  }
  /* If this is SIB5bis, typecast it to SIB5 */
  if(sib == rrc_SIB5bis)
  {
    sib = rrc_SIB5;
  }

  /* First check if this SIB is needed. */
  if((sib < (rrc_MAX_SIB - 1)) && (sib_events_ptr->event[index].tsib[sib].sib_needed))
  {

    /* Then check if the SIB has already been received */
    if(sib_events_ptr->event[index].tsib[sib].sib_received && (sib != rrc_SIB7))
    {
      /* Nothing to do here, this is a duplicate */
      return(SEG_PROC_DUPLICATE);
    }

    //honor the first segment, only first time.. this check is added to concatenate
    //SIB's from the last hole where it was left off..  if we received a duplicate first
    //segment then we were un-necessarily reading from first all over again even though
    //we may have received 1,2 earlier..
    if(sib_events_ptr->event[index].tsib[sib].seg_index != (MAX_SEG_INDEX_FOR_A_SIB + 1))
    {
      WRRC_MSG1_HIGH("Dup. 1st seg. for sib %d", sib);
      return(SEG_PROC_DUPLICATE);
    }

    /* We have received the first segment of a SIB. We need to copy
    the bits into the SIB events database. First store 
    the number of segments for this SIB. */
    MSG_LOW("Rxed First seg of SIB %d", seg->sib_Type, 0, 0);
    sib_events_ptr->event[index].tsib[sib].seg_count = seg->seg_Count;

    /* Now copy the actual data bits. Use the bit copy function.
    Destination is the sib data pointer inside the sib events database.
    Source is the received segment's data pointer. */
    dest = (void *)sib_events_ptr->event[index].tsib[sib].sib_data_ptr;
    src = (void *)seg->sib_Data_fixed.data;

    b_copy( src,  /* Source Pointer */
            0,    /* Source Offset is always 0 since we copy all the received bits */
            dest, /* Destination Pointer */
            0,    /* Destination Offset is 0 since this is the first segment */
            FIXED_SIB_DATA_LENGTH_BITS  /* Length of the first segment is fixed */
          );

    /* Note the current length of this SIB */
    sib_events_ptr->event[index].tsib[sib].sib_tot_length = FIXED_SIB_DATA_LENGTH_BITS;

    /* Since a first segment has been received, we can now make the segment
    index 0. */
    sib_events_ptr->event[index].tsib[sib].seg_index = 0;

    status = rrcsib_process_sib_seg_in_tsib_optim_db(sib,index);
  
    if(status == SEG_PROC_COMPLETE)
    {
      /*Do nothing here & return same value from this function*/
    }
    else
    {
      status = SEG_PROC_SUCCEEDED;
    }

    return( status );
  }
  else
  {
    /* Nothing to do here since this SIB is not required */
    return(SEG_PROC_NONE);
  }
}

/*===========================================================================

FUNCTION PROCESS_SHORT_FIRST_SEG

DESCRIPTION

  This function processes a "First Segment(Short)" of a SIB. The data from the
  segment is copied to the appropriate place in the temp sib holder based on
  the SIB type.
  
DEPENDENCIES

  None.

RETURN VALUE

  A value of type seg_proc_stat_e_type indicating the status of the
  processing.

SIDE EFFECTS

  None.

===========================================================================*/
static seg_proc_stat_e_type process_short_first_seg
(
  rrc_FirstSegmentShort *seg, /* Pointer to the received First Segment */ 
  sib_events_index_e_type index,    /* Index into the SIB events database
                                   for storing SIBs received in this msg */
  uint32 sfn_prime  /*SFN prime at which this sib segment received*/

)
{
  rrc_SIB_type sib;  /* To store the received SIBs type */
  void *src;         /* Source pointer for the b_copy function */
  void *dest;        /* Destination pointer for the b_copy function */
  seg_proc_stat_e_type status = SEG_PROC_SUCCEEDED;/*This will say status of processing*/

  if(EVENT_INDEX_VALID(index) ==FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    status = SEG_PROC_FAILED;
    return status;
  }

  if(seg->sib_Type == (uint32)rrc_SIB_Type_extensionType)
  {
    sib = rrcsib_map_extension_sib_to_internal_sibtype(0,sfn_prime);
  }
  else
  {
/* Copy the received sib type. */
    sib = rrcsib_map_asn1_sibtype_to_int_sibtype(seg->sib_Type);
  }

  if(sib == rrc_MAX_SIB)
  {
    status = SEG_PROC_FAILED;
    return status;
  }

  /* If this is SIB5bis, typecast it to SIB5 */
  if(sib == rrc_SIB5bis)
  {
    sib = rrc_SIB5;
  }

  /* First check if the scheduling info for this SIB has been received. This leg might be hit when a
     BCCH-BCH block is received with some segments in it being scheduled in MIB while some are 
     in SB1/SB2 and SB1/SB2 are yet to be received.
  */
  if((sib < (rrc_MAX_SIB - 1)) && (sib_events_ptr->sib_sched[sib].repitition_rate == INVALID_SIB_SCHEDULE_REP))
  {
   /* Nothing to do here since this SIB is not required */
    return(SEG_PROC_NONE);
  }

    /* First check if this SIB is needed. */
  if((sib < (rrc_MAX_SIB - 1)) && (sib_events_ptr->event[index].tsib[sib].sib_needed))
  {

    /* Then check if the SIB has already been received */
    if(sib_events_ptr->event[index].tsib[sib].sib_received && (sib != rrc_SIB7))
    {
      /* Nothing to do here, this is a duplicate */
      return(SEG_PROC_DUPLICATE);
    }

    /* We have received the first segment of a SIB. We need to copy
    the bits into the SIB events database. First store
    the number of segments for this SIB. */
    MSG_LOW("Rxed Short First seg of SIB %d", seg->sib_Type, 0, 0);
    sib_events_ptr->event[index].tsib[sib].seg_count = seg->seg_Count;

    /* Now copy the actual data bits. Use the bit copy function.
    Destination is the sib data pointer inside the sib events database.
    Source is the received segment's data pointer. */
    dest = (void *)sib_events_ptr->event[index].tsib[sib].sib_data_ptr;
    src = (void *)seg->sib_Data_variable.data;

    b_copy( src,  /* Source Pointer */
            0,    /* Source Offset is always 0 since we copy all the received bits */
            dest, /* Destination Pointer */
            0,    /* Destination Offset is 0 since this is the first segment */
            (word)(seg->sib_Data_variable.numbits) /* Length of the short first segment */
                  /* This is defined as unsigned int, but the maximum value is only
                  214 */
          );

    /* Note the current length of this SIB */
    sib_events_ptr->event[index].tsib[sib].sib_tot_length = seg->sib_Data_variable.numbits;

    /* Since a first segment has been received, we can now make the segment
    index 0. */
    sib_events_ptr->event[index].tsib[sib].seg_index = 0;

    status = rrcsib_process_sib_seg_in_tsib_optim_db(sib,index);

    if(status == SEG_PROC_COMPLETE)
    {
      /*Do nothing here & return same value from this function*/
    }
    else
    {
      status = SEG_PROC_SUCCEEDED;
    }
    return( status );
  }
  else
  { 
    /* Nothing to do here since this SIB is not required */
    return(SEG_PROC_NONE);
  }
}



/*===========================================================================

FUNCTION PROCESS_SUBSEQUENT_SEG

DESCRIPTION

  This function processes a "Subsequent Segment" of a SIB. The data from the segment
  is copied to the appropriate place in the temp sib holder based on the SIB
  type. Since other segments of this SIB have been copied before, this data
  is concatenated to the last bit of data received in the previous segment.
  
DEPENDENCIES

  None.

RETURN VALUE

  A value of type seg_proc_stat_e_type indicating the status of the
  processing.

SIDE EFFECTS

  None.

===========================================================================*/
static seg_proc_stat_e_type process_subsequent_seg
(
  rrc_SubsequentSegment *seg,  /* Pointer to the received Subsequent Segment */
  sib_events_index_e_type index,    /* Index into the SIB events database
                                   for storing SIBs received in this msg */
  uint32 sfn_prime  /*SFN prime at which this sib segment received*/
)
{
  rrc_SIB_type sib;  /* To store the received SIBs type */
  void *src;         /* Source pointer for the b_copy function */
  void *dest;        /* Destination pointer for the b_copy function */
  word dest_offset;  /* Destination offset for the b_copy function */
  seg_proc_stat_e_type status = SEG_PROC_SUCCEEDED;/*This will say status of processing*/


  if(EVENT_INDEX_VALID(index) ==FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    status = SEG_PROC_FAILED;
    return status;  
  }

  if(seg->sib_Type == (uint32)rrc_SIB_Type_extensionType)
  {
    sib = rrcsib_map_extension_sib_to_internal_sibtype(seg->segmentIndex, sfn_prime);
  }
  else
  {
    /* Copy the received sib type. */
    sib = rrcsib_map_asn1_sibtype_to_int_sibtype(seg->sib_Type);
  }

  if(sib == rrc_MAX_SIB)
  {
    status = SEG_PROC_FAILED;
    return status;
  }

  /* If this is SIB5bis, typecast it to SIB5 */
  if(sib == rrc_SIB5bis)
  {
    sib = rrc_SIB5;
  }

  /* First check if this SIB is needed. */
  if((sib < (rrc_MAX_SIB - 1)) && (sib_events_ptr->event[index].tsib[sib].sib_needed))
  {

    /* Then check if the SIB has already been received */
    if(sib_events_ptr->event[index].tsib[sib].sib_received && (sib != rrc_SIB7))
    {
      /* Nothing to do here, this is a duplicate */
      return(SEG_PROC_DUPLICATE);
    }

    /* We have received a subsequent segment of a SIB. We need to 
    concatenate the received bits to the end of the already received
    bits stored in the SIB events database. */
    MSG_LOW("Rxed Subs Seg of SIB %d", seg->sib_Type, 0, 0);

    /* Check if we've received all the segments before this segment for this
    SIB. This is done by checking if the received index is one more than the
    stored index. */
    if( sib_events_ptr->event[index].tsib[sib].seg_index != (seg->segmentIndex - 1))
    {
      status = rrcsib_store_sib_seg_in_tsib_optim_db(sib,
                                          sfn_prime,
                                          seg->segmentIndex,
                                          subsequentSegment_chosen,
                                          FIXED_SIB_DATA_LENGTH_BITS,
                                          (void *)seg->sib_Data_fixed.data,
                                          index);
      MSG_HIGH("Gap in Seg Seq sib# %d seg# %d status %d", rrcsib_map_int_sibtype_to_asn1_sibtype(sib), seg->segmentIndex,status);
      if(status != SEG_PROC_STORED)
      {
        status = SEG_PROC_FAILED;
      }
      return( status );
    }
    if( seg->segmentIndex > MAX_SEG_INDEX_FOR_A_SIB )
    {
      WRRC_MSG1_HIGH("Seg count exceeded! %d", seg->segmentIndex);
      return( SEG_PROC_FAILED );
    }
    sib_events_ptr->event[index].tsib[sib].seg_index = seg->segmentIndex;

    /* Just for safety check if the segment will fit into the maximum
      allowed length for the SIB */
    if((sib_events_ptr->event[index].tsib[sib].sib_tot_length + 
        FIXED_SIB_DATA_LENGTH_BITS) > MAX_SIB_LENGTH_BITS )
    {
      WRRC_MSG0_HIGH("Maximum SIB length exceeded!");
      return( SEG_PROC_FAILED );
    }

    /* Now copy the actual data bits. Use the bit copy function.
      Destination is the sib data pointer inside the sib events database.
      Source is the received segment's data pointer.  Destination offset
      is the next bit after the total received bits so far. */
    dest = (void *)sib_events_ptr->event[index].tsib[sib].sib_data_ptr;
    src = (void *)seg->sib_Data_fixed.data;
    /* Ok to typecast to word here since maximum value for length is 3552 */
    dest_offset = (word)(sib_events_ptr->event[index].tsib[sib].sib_tot_length);

    b_copy( src,  /* Source Pointer */
            0,    /* Source Offset is always 0 since we copy all the received bits */
            dest, /* Destination Pointer */
            dest_offset,  /* Destination Offset */
            FIXED_SIB_DATA_LENGTH_BITS  /* Length of the subsequent segment is fixed */
          );

   /* Note the current length of this SIB. Add this segments length to the 
    previous total. */
    sib_events_ptr->event[index].tsib[sib].sib_tot_length = 
      (sib_events_ptr->event[index].tsib[sib].sib_tot_length + 
        FIXED_SIB_DATA_LENGTH_BITS);
  
    status = rrcsib_process_sib_seg_in_tsib_optim_db(sib,index);

    if(status == SEG_PROC_COMPLETE)
    {
      /*Do nothing here & return same value from this function*/
    }
    else
    {
      status = SEG_PROC_SUCCEEDED;
    }
    return( status );    
  }
  else
  { 
    /* Nothing to do here since this SIB is not required */
    return(SEG_PROC_NONE);
  }
}





/*===========================================================================

FUNCTION PROCESS_LAST_SEG

DESCRIPTION

  This function processes a "Last Segment" of a SIB. The data from the segment
  is copied to the appropriate place in the temp sib holder based on the SIB
  type. Since other segments of this SIB have been copied before, this data
  is concatenated to the last bit of data received in the previous segment.
  
DEPENDENCIES

  None.

RETURN VALUE

  A value of type seg_proc_stat_e_type indicating the status of the
  processing.

SIDE EFFECTS

  None.

===========================================================================*/
static seg_proc_stat_e_type process_last_seg
(
  rrc_LastSegment *seg,  /* Pointer to the received Last Segment */
  sib_events_index_e_type index,    /* Index into the SIB events database
                                   for storing SIBs received in this msg */
  uint32 sfn_prime  /*SFN prime at which this sib segment received*/
)
{
  rrc_SIB_type sib;  /* To store the received SIBs type */
  void *src;         /* Source pointer for the b_copy function */
  void *dest;        /* Destination pointer for the b_copy function */
  word dest_offset;  /* Destination offset for the b_copy function */
  seg_proc_stat_e_type status = SEG_PROC_SUCCEEDED;/*This will say status of processing*/

  if(EVENT_INDEX_VALID(index) ==FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    status = SEG_PROC_FAILED;
    return status;
  }

  if(seg->sib_Type == (uint32)rrc_SIB_Type_extensionType)
  {
    sib = rrcsib_map_extension_sib_to_internal_sibtype(seg->segmentIndex, sfn_prime);
  }
  else
  {
    /* Copy the received sib type. */
    sib = rrcsib_map_asn1_sibtype_to_int_sibtype(seg->sib_Type);
  }

  if(sib == rrc_MAX_SIB)
  {
    status = SEG_PROC_FAILED;
    return status;
  }

  /* If this is SIB5bis, typecast it to SIB5 */
  if(sib == rrc_SIB5bis)
  {
    sib = rrc_SIB5;
  }

  /* First check if this SIB is needed. */
  if((sib < (rrc_MAX_SIB - 1)) && (sib_events_ptr->event[index].tsib[sib].sib_needed))
  {

    /* Then check if the SIB has already been received */
    if(sib_events_ptr->event[index].tsib[sib].sib_received && (sib != rrc_SIB7))
    {
      /* Nothing to do here, this is a duplicate */
      return(SEG_PROC_DUPLICATE);
    }

    /* We have received the last segment of a SIB. We need to 
    concatenate the received bits to the end of the already received
    bits stored in the SIB events database. */
    MSG_LOW("Rxed Last Seg of SIB %d", seg->sib_Type, 0, 0);

    /* Check if we've received all the segments before this segment for this
    SIB. This is done by checking if the received index is one more than the
    stored index. */
    if( sib_events_ptr->event[index].tsib[sib].seg_index != (seg->segmentIndex - 1))
    {
      status = rrcsib_store_sib_seg_in_tsib_optim_db(sib,
                                            sfn_prime,
                                            seg->segmentIndex,
                                            lastSegment_chosen,
                                            FIXED_SIB_DATA_LENGTH_BITS,
                                            (void *)seg->sib_Data_fixed.data,
                                            index);
      MSG_HIGH("Gap in Seg Seq sib# %d seg# %d status %d", rrcsib_map_int_sibtype_to_asn1_sibtype(sib), seg->segmentIndex,status);
      if(status != SEG_PROC_STORED)
      {
        status = SEG_PROC_FAILED;
      }
      return( status );
    }
    if( seg->segmentIndex > MAX_SEG_INDEX_FOR_A_SIB )
    {
      WRRC_MSG1_HIGH("Seg count exceeded! - %d", seg->segmentIndex);
      return( SEG_PROC_FAILED );
    }
    sib_events_ptr->event[index].tsib[sib].seg_index = seg->segmentIndex;

    /* Just for safety check if the segment will fit into the maximum
    allowed length for the SIB */
    if((sib_events_ptr->event[index].tsib[sib].sib_tot_length + 
      FIXED_SIB_DATA_LENGTH_BITS) > MAX_SIB_LENGTH_BITS )
    {
      WRRC_MSG0_HIGH("Maximum SIB length exceeded!");
      return( SEG_PROC_FAILED );
    }

    /* Now copy the actual data bits. Use the bit copy function.
    Destination is the sib data pointer inside the sib events database.
    Source is the received segment's data pointer.  Destination offset
    is the next bit after the total received bits so far. */
    dest = (void *)sib_events_ptr->event[index].tsib[sib].sib_data_ptr;
    src = (void *)seg->sib_Data_fixed.data;
    /* Ok to typecast to word here since maximum value for length is 3552 */
    dest_offset = (word)(sib_events_ptr->event[index].tsib[sib].sib_tot_length);

    b_copy( src,  /* Source Pointer */
            0,    /* Source Offset is always 0 since we copy all the received bits */
            dest, /* Destination Pointer */
            dest_offset,  /* Destination Offset */
            FIXED_SIB_DATA_LENGTH_BITS  /* Length of the subsequent segment is fixed */
          );

    /* Note the current length of this SIB. Add this segments length to the 
    previous total. */
    sib_events_ptr->event[index].tsib[sib].sib_tot_length = 
      (sib_events_ptr->event[index].tsib[sib].sib_tot_length + 
       FIXED_SIB_DATA_LENGTH_BITS);

    /* Since this is the last segment, set the "sib_received" indicator
    to TRUE so that we won't process this SIB again. */
    sib_events_ptr->event[index].tsib[sib].sib_received = TRUE;
  
    status = rrcsib_process_sib_seg_in_tsib_optim_db(sib,index);

    if(status == SEG_PROC_COMPLETE)
    {
      /*Do nothing here & return same value from this function*/
    }
    else
    {
      status = SEG_PROC_SUCCEEDED;
    }
    return( status );    
  }
  else
  { 
    /* Nothing to do here since this SIB is not required */
    return(SEG_PROC_NONE);
  }
}




/*===========================================================================

FUNCTION PROCESS_SHORT_LAST_SEG

DESCRIPTION

  This function processes a "Last Segment(Short)" of a SIB. The data from the segment
  is copied to the appropriate place in the temp sib holder based on the SIB
  type. Since other segments of this SIB have been copied before, this data
  is concatenated to the last bit of data received in the previous segment.
  
DEPENDENCIES

  None.

RETURN VALUE

  A value of type seg_proc_stat_e_type indicating the status of the
  processing.

SIDE EFFECTS

  None.

===========================================================================*/
static seg_proc_stat_e_type process_short_last_seg
(
  rrc_LastSegmentShort *seg,  /* Pointer to the received Last Segment */
  sib_events_index_e_type index,    /* Index into the SIB events database
                                   for storing SIBs received in this msg */
  uint32 sfn_prime  /*SFN prime at which this sib segment received*/
)
{
  rrc_SIB_type sib;  /* To store the received SIBs type */
  void *src;         /* Source pointer for the b_copy function */
  void *dest;        /* Destination pointer for the b_copy function */
  word dest_offset;  /* Destination offset for the b_copy function */
  seg_proc_stat_e_type status = SEG_PROC_SUCCEEDED;/*This will say status of processing*/

  if(EVENT_INDEX_VALID(index) ==FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    status = SEG_PROC_FAILED;
    return status;
  }

  if(seg->sib_Type == (uint32)rrc_SIB_Type_extensionType)
  { 
    sib = rrcsib_map_extension_sib_to_internal_sibtype(seg->segmentIndex, sfn_prime);
  }
  else
  {
  /* Copy the received sib type. */
    sib = rrcsib_map_asn1_sibtype_to_int_sibtype(seg->sib_Type);
  }

  if(sib == rrc_MAX_SIB)
  {
    status = SEG_PROC_FAILED;
    return status;
  }

  /* If this is SIB5bis, typecast it to SIB5 */
  if(sib == rrc_SIB5bis)
  {
    sib = rrc_SIB5;
  }

    /* First check if the scheduling info for this SIB has been received. This leg might be hit when a
          BCCH-BCH block is received with some segments in it being scheduled in MIB while some are 
          in SB1/SB2 and SB1/SB2 are yet to be received.
          */
  if((sib < (rrc_MAX_SIB - 1)) && (sib_events_ptr->sib_sched[sib].repitition_rate == INVALID_SIB_SCHEDULE_REP))
  {
    /* Nothing to do here since this SIB is not required */
    return(SEG_PROC_NONE);
  }
  /* First check if this SIB is needed. */
  if((sib < (rrc_MAX_SIB - 1)) && (sib_events_ptr->event[index].tsib[sib].sib_needed))
  {

    /* Then check if the SIB has already been received */
    if(sib_events_ptr->event[index].tsib[sib].sib_received && (sib != rrc_SIB7))
    {
      /* Nothing to do here, this is a duplicate */
      return(SEG_PROC_DUPLICATE);
    }

    /* We have received the last segment(short) of a SIB. We need to 
    concatenate the received bits to the end of the already received
    bits stored in the SIB events database.  */
    MSG_LOW("Received Short Last Segment of SIB %d", seg->sib_Type, 0, 0);

    /* Check if we've received all the segments before this segment for this
    SIB. This is done by checking if the received index is one more than the
    stored index. */
    if( sib_events_ptr->event[index].tsib[sib].seg_index != (seg->segmentIndex - 1))
    {
      status = rrcsib_store_sib_seg_in_tsib_optim_db(sib,
                                            sfn_prime,
                                            seg->segmentIndex,
                                            lastSegmentShort_chosen,
                                            seg->sib_Data_variable.numbits,
                                            (void *)seg->sib_Data_variable.data,
                                            index);
      MSG_HIGH("Gap in Seg Seq sib# %d seg# %d status %d", rrcsib_map_int_sibtype_to_asn1_sibtype(sib), seg->segmentIndex,status);
      if(status != SEG_PROC_STORED)
      {
        status = SEG_PROC_FAILED;
      }

      return( status );
    }
    if( seg->segmentIndex > MAX_SEG_INDEX_FOR_A_SIB )
    {
      WRRC_MSG1_HIGH("Segment count exceeded! - %d", seg->segmentIndex);
      return( SEG_PROC_FAILED );
    }
    sib_events_ptr->event[index].tsib[sib].seg_index = seg->segmentIndex;

    /* Just for safety check if the segment will fit into the maximum
    allowed length for the SIB */
    if((sib_events_ptr->event[index].tsib[sib].sib_tot_length + 
        seg->sib_Data_variable.numbits) > MAX_SIB_LENGTH_BITS )
    {
      WRRC_MSG0_HIGH("Maximum SIB length exceeded!");
      return( SEG_PROC_FAILED );
    }

    /* Now copy the actual data bits. Use the bit copy function.
    Destination is the sib data pointer inside the sib events database.
    Source is the received segment's data pointer.  Destination offset
    is the next bit after the total received bits so far. */
    dest = (void *)sib_events_ptr->event[index].tsib[sib].sib_data_ptr;
    src = (void *)seg->sib_Data_variable.data;
    /* Ok to typecast to word here since maximum value for length is 3552 */
    dest_offset = (word)(sib_events_ptr->event[index].tsib[sib].sib_tot_length);

    b_copy( src,  /* Source Pointer */
            0,    /* Source Offset is always 0 since we copy all the received bits */
            dest, /* Destination Pointer */
            dest_offset,  /* Destination Offset */
            (word)(seg->sib_Data_variable.numbits) /* Length of the subsequent segment is fixed */
                  /* This is defined as unsigned int, but the maximum value is only
                  214 */
          );

    /* Note the current length of this SIB. Add this segments length to the 
    previous total. */
    sib_events_ptr->event[index].tsib[sib].sib_tot_length = 
      (sib_events_ptr->event[index].tsib[sib].sib_tot_length + 
        seg->sib_Data_variable.numbits);

    /* Since this is the last segment, set the "sib_received" indicator
    to TRUE so that we won't process this SIB again. */
    sib_events_ptr->event[index].tsib[sib].sib_received = TRUE;
  
    status = rrcsib_process_sib_seg_in_tsib_optim_db(sib,index);

    if(status == SEG_PROC_COMPLETE)
    {
      /*Do nothing here & return same value from this function*/
    }
    else
    {
      status = SEG_PROC_SUCCEEDED;
    }
    return( status );    
  }
  else
  { 
    /* Nothing to do here since this SIB is not required */
    return(SEG_PROC_NONE);
  }
}



/*===========================================================================

FUNCTION PROCESS_SHORT_COMPLETE_SIB

DESCRIPTION

  This function processes a short complete SIB. The SIB data
  is copied to the appropriate place in the temp sib holder 
  based on the SIB type.

DEPENDENCIES

  None.

RETURN VALUE

  A value of type seg_proc_stat_e_type indicating the status of the
  processing.

SIDE EFFECTS

  None.

===========================================================================*/
static seg_proc_stat_e_type process_short_complete_sib
(
  rrc_CompleteSIBshort *seg,       /* Pointer to the received Complete SIB */
  sib_events_index_e_type index,    /* Index into the SIB events database
                                   for storing SIBs received in this msg */
  uint32 sfn_prime  /*SFN prime at which this sib segment received*/

)
{
  rrc_SIB_type sib; /* The received SIB */
  uint32 length;    /* Store length for copying */
  uint32 count;     /* Local counter    */

  if(EVENT_INDEX_VALID(index) ==FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    return SEG_PROC_FAILED;
  }

  if(seg->sib_Type == (uint32)rrc_SIB_Type_extensionType)
  {
    sib = rrcsib_map_extension_sib_to_internal_sibtype(0, sfn_prime);
  }
  else
  {
   /* Copy the received sib type. */
     sib = rrcsib_map_asn1_sibtype_to_int_sibtype(seg->sib_Type);
  }

  if(sib == rrc_MAX_SIB)
  {
    return SEG_PROC_FAILED;
  }

  /* If this is SIB5bis, typecast it to SIB5 */
  if(sib == rrc_SIB5bis)
  {
    sib = rrc_SIB5;
  }

    /* First check if the scheduling info for this SIB has been received. This leg might be hit when a
          BCCH-BCH block is received with some segments in it being scheduled in MIB while some are 
          in SB1/SB2 and SB1/SB2 are yet to be received.
          */
  if((sib < (rrc_MAX_SIB - 1)) && (sib_events_ptr->sib_sched[sib].repitition_rate == INVALID_SIB_SCHEDULE_REP))
  {
    /* Nothing to do here since this SIB is not required */
    return(SEG_PROC_NONE);
  }
  if(sib < (rrc_MAX_SIB - 1))
  {
    /* First check if this SIB is needed. */
    if(!(sib_events_ptr->event[index].tsib[sib].sib_needed))
    {
      /* Nothing to do here since this SIB is not required */
      return(SEG_PROC_NONE);
    }

    /* Then check if the SIB has already been received */
    if(sib_events_ptr->event[index].tsib[sib].sib_received && (sib != rrc_SIB7))
    {
      /* Nothing to do here, this is a duplicate */
      return(SEG_PROC_DUPLICATE);
    }

    /* Store the length to be copied, in bytes */
    length = GET_PDU_LENGTH_IN_BYTES( seg->sib_Data_variable.numbits );

    /* We need to copy the entire SIB to our temporary buffer. */
    for( count = 0; count <= length; count++ )
    {
      sib_events_ptr->event[index].tsib[sib].sib_data_ptr[count]
        = seg->sib_Data_variable.data[count];
    }

    /* Store the length in bits */
    sib_events_ptr->event[index].tsib[sib].sib_tot_length 
      = seg->sib_Data_variable.numbits;

  /* Since this is a complete SIB, set the "sib_received" indicator
  to TRUE so that we won't process this SIB again. */
    sib_events_ptr->event[index].tsib[sib].sib_received = TRUE;
    return( SEG_PROC_SUCCEEDED );
  }
  else
  {
    /* Nothing to do here since this SIB is not required */
    return(SEG_PROC_NONE);
  }
}




/*===========================================================================

FUNCTION PROCESS_COMPLETE_SIB

DESCRIPTION

  This function processes a complete SIB. The SIB data
  is copied to the appropriate place in the temp sib holder 
  based on the SIB type.
  
DEPENDENCIES

  None.

RETURN VALUE

  A value of type seg_proc_stat_e_type indicating the status of the
  processing.

SIDE EFFECTS

  None.

===========================================================================*/
static seg_proc_stat_e_type process_complete_sib
(
  rrc_CompleteSIB *seg,            /* Pointer to the received Complete SIB */
  sib_events_index_e_type index,    /* Index into the SIB events database
                                   for storing SIBs received in this msg */
  uint32 sfn_prime  /*SFN prime at which this sib segment received*/

)
{
  rrc_SIB_type sib; /* The received SIB */
  word count;       /* Local counter    */

  if(EVENT_INDEX_VALID(index) ==FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    return SEG_PROC_FAILED;
  }

  if(seg->sib_Type == (uint32)rrc_SIB_Type_extensionType)
  {
    sib = rrcsib_map_extension_sib_to_internal_sibtype(0, sfn_prime);
  }
  else
  {
  /* Copy the received sib type. */
    sib = rrcsib_map_asn1_sibtype_to_int_sibtype(seg->sib_Type);
  }

  if(sib == rrc_MAX_SIB)
  {
    return SEG_PROC_FAILED;
  }

  /* If this is SIB5bis, typecast it to SIB5 */
  if(sib == rrc_SIB5bis)
  {
    sib = rrc_SIB5;
  }

  if(sib < (rrc_MAX_SIB - 1))
  {
    /* First check if this SIB is needed. */
    if(!(sib_events_ptr->event[index].tsib[sib].sib_needed))
    {
      /* Nothing to do here since this SIB is not required */
      return(SEG_PROC_NONE);
    }

    /* Then check if the SIB has already been received */
    if(sib_events_ptr->event[index].tsib[sib].sib_received && (sib != rrc_SIB7))
    {
      /* Nothing to do here, this is a duplicate */
      return(SEG_PROC_DUPLICATE);
    } 

    /* We need to copy the entire SIB to our temporary buffer. */
    for(count=0; count<FIXED_COMPLETE_SIB_DATA_LENGTH_BYTES; count++)
    {
      sib_events_ptr->event[index].tsib[sib].sib_data_ptr[count] =
        seg->sib_Data_fixed.data[count];
    }

    /* Store the length in bits */
    sib_events_ptr->event[index].tsib[sib].sib_tot_length = 
      FIXED_COMPLETE_SIB_DATA_LENGTH_BITS;

    /* Since this is a complete SIB, set the "sib_received" indicator
    to TRUE so that we won't process this SIB again. */
    sib_events_ptr->event[index].tsib[sib].sib_received = TRUE;
    return( SEG_PROC_SUCCEEDED );
  }
  else
  {
    /* Nothing to do here since this SIB is not required */
    return(SEG_PROC_NONE);
  }
}


/*===========================================================================

FUNCTION PROCESS_SYS_INFO_MSG

DESCRIPTION
 
  This function processes a received System Information Message. The
  System Information Message must have already been ASN.1 decoded before
  passing it to this function. Both FACH System Information Messages
  and BCH System Information Messages are processed here.
  
  The function first determines what combination of SIBs or SIB segments
  are included in the message. Then based on the combination, each SIB
  or SIB segment in the message is processed by calling the appropriate
  function.
  
  This function returns a status value which indicates whether any SIB
  has been completely received or not. If a SIB has been completely 
  received, the calling function is responsible to ASN.1 decode the SIB
  and save the decoded SIB in the SIB database.

DEPENDENCIES

  None.

RETURN VALUE

  NO_SIBS_COMPLETED if none of the SIBs were completely received in this
  system information message.
  SIBS_COMPLETED if any SIB was completely received in this message. It's
  not necessary that the entire SIB was received in this message. If the
  last semgment of a SIB was received in this message, SIBS_COMPLETED will
  be returned by this function. 

SIDE EFFECTS

===========================================================================*/
rrc_sib_collect_e_type process_sys_info_msg
(
  //NEED TO MODIFY LATER (FACH msg also needed)
  rrc_SystemInformation_BCH *msg,  /* Pointer to Sys Info Message BCH */

  sib_events_index_e_type index    /* Index into the SIB events database
                                   for storing SIBs received in this msg */
)
{
  /* Status to be returned to the calling function.
  Assume no SIBs have been completed in this round. */
  rrc_sib_collect_e_type status = NO_SIBS_COMPLETED;
  /* For saving the status of segment processing    */
  seg_proc_stat_e_type seg_status = SEG_PROC_NONE;
  OSRTDListNode *temp_node = NULL;

  /* Pointer to a complete SIB list needed in case a
  complete SIB list is included in the message */
  rrc_CompleteSIB_List *list_ptr = NULL;
  
  if(EVENT_INDEX_VALID(index) ==FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    status = NO_SIBS_COMPLETED;
    return status;
  }
  //Need a way to send sfn prime only once for a cell.
  //probably hook with event controller module for that
  //function to send sfn is TBD

  /* Switch based on the combination type present in
  the message */
#ifdef FEATURE_WCDMA_HS_FACH_DRX
  curr_sfn = msg->sfn_Prime;
#endif
  switch( msg->payload.t )
  {
    case( T_rrc_SystemInformation_BCH_payload_noSegment ):
      /* No SIB segments included in this message */
      WRRC_MSG0_HIGH("No Segment chosen");
      /* Nothing to do here, just break. */
      break;

    case( T_rrc_SystemInformation_BCH_payload_firstSegment ):
      /* First segment included in this message   */
      WRRC_MSG0_HIGH("First Segment chosen");
      /* Process the First Segment */
      seg_status = process_first_seg( &(msg->payload.u.firstSegment), index, msg->sfn_Prime );
      if(seg_status == SEG_PROC_COMPLETE)
      {
        status = SIBS_COMPLETED;
      }
      break;


    case( T_rrc_SystemInformation_BCH_payload_subsequentSegment ):
      /* Subsequent segment included in this message */
      MSG_LOW("Subsequent Segment chosen", 0, 0, 0);
      /* Process the Subsequent Segment */
      seg_status = process_subsequent_seg( &(msg->payload.u.subsequentSegment), 
                                           index, msg->sfn_Prime );
      if(seg_status == SEG_PROC_COMPLETE)
      {
        status = SIBS_COMPLETED;
      }
      break;


    case( T_rrc_SystemInformation_BCH_payload_lastSegmentShort ):
      /* Last segment included in this message */
      MSG_LOW(" Last Segment chosen", 0, 0, 0);
      /* Process the Last Segment */
      seg_status = process_short_last_seg( &(msg->payload.u.lastSegmentShort),
                                           index, msg->sfn_Prime );
      /* Since this is the last segment, if segment processing is succesful,
      a SIB has been completely received. */
      if((seg_status == SEG_PROC_SUCCEEDED )
         ||(seg_status == SEG_PROC_COMPLETE)
         )
      {
#ifdef FEATURE_RRC_SIB_DEBUG
        WRRC_MSG2_HIGH("Received SIB %d, Sfn prime %d", msg->payload.u.lastSegmentShort.sib_Type, msg->sfn_Prime);
#endif /* FEATURE_RRC_SIB_DEBUG */
        rrc_log_complete_sib(msg->payload.u.lastSegmentShort.sib_Type,
                             msg->payload.u.lastSegmentShort.segmentIndex,
                             msg->sfn_Prime);
        status = SIBS_COMPLETED;
      }
      break;


    case( T_rrc_SystemInformation_BCH_payload_lastAndFirst ):
      /* 1 last segment and 1 first segment 
      included in this message */
      MSG_LOW("Last and First Segment chosen", 0, 0, 0);
      /* First process the last segment */
      seg_status = process_short_last_seg( &(msg->payload.u.lastAndFirst.lastSegmentShort), 
                                           index, msg->sfn_Prime );
      /* Since this is the last segment, if segment processing is succesful,
      a SIB has been completely received. */
      if((seg_status == SEG_PROC_SUCCEEDED )
         ||(seg_status == SEG_PROC_COMPLETE)
         )
      {
#ifdef FEATURE_RRC_SIB_DEBUG
        WRRC_MSG2_HIGH("Received SIB %d, Sfn prime %d",msg->payload.u.lastAndFirst.lastSegmentShort.sib_Type, msg->sfn_Prime);
#endif /* FEATURE_RRC_SIB_DEBUG */
        rrc_log_complete_sib(msg->payload.u.lastAndFirst.lastSegmentShort.sib_Type,
                             msg->payload.u.lastAndFirst.lastSegmentShort.segmentIndex,
                             msg->sfn_Prime);
        status = SIBS_COMPLETED;
      }
      /* Then process the First Segment */
      seg_status = process_short_first_seg( &(msg->payload.u.lastAndFirst.firstSegment),
                                            index, msg->sfn_Prime );
      if(seg_status == SEG_PROC_COMPLETE)
      {
        status = SIBS_COMPLETED;
      }
      break;


    case( T_rrc_SystemInformation_BCH_payload_lastAndComplete ):
      /* Last segment and one or more complete SIBS
      included in this message */
      MSG_LOW("Last Segment and complete SIBs chosen", 0, 0, 0);
      /* First process the last segment */
      seg_status = process_short_last_seg( &(msg->payload.u.lastAndComplete.lastSegmentShort),
                                           index, msg->sfn_Prime );
      /* Since a last semgent is received,
      Indicate that SIBs have been completed. */
      if((seg_status == SEG_PROC_SUCCEEDED )
         ||(seg_status == SEG_PROC_COMPLETE)
        )
      {
#ifdef FEATURE_RRC_SIB_DEBUG
        WRRC_MSG2_HIGH("Received SIB %d, Sfn prime %d",msg->payload.u.lastAndComplete.lastSegmentShort.sib_Type, msg->sfn_Prime);
#endif /* FEATURE_RRC_SIB_DEBUG */
        rrc_log_complete_sib(msg->payload.u.lastAndComplete.lastSegmentShort.sib_Type,
                             msg->payload.u.lastAndComplete.lastSegmentShort.segmentIndex,
                             msg->sfn_Prime);
        status = SIBS_COMPLETED;
      }
      /* Then process each one of the complete SIBs given
      by the message. Copy the first complete SIB pointer to
      the local pointer and then traverse through subsequent 
      SIB pointers. */
      list_ptr = &(msg->payload.u.lastAndComplete.completeSIB_List);
      temp_node = list_ptr->head;
      
      if(temp_node == NULL)
      {
        WRRC_MSG0_ERROR("Invalid SIB pointer received");
        return FALSE;
      }

      while(temp_node != NULL )
      {
        seg_status = process_short_complete_sib( temp_node->data, index, msg->sfn_Prime );
        /* Update status for each SIB since we're in a while loop */
        if((seg_status == SEG_PROC_SUCCEEDED )
          ||(seg_status == SEG_PROC_COMPLETE)
          )
        {
#ifdef FEATURE_RRC_SIB_DEBUG
          WRRC_MSG2_HIGH("Received SIB %d, Sfn prime %d", 
                 ((rrc_CompleteSIBshort*)(temp_node->data))->sib_Type,msg->sfn_Prime);
#endif /* FEATURE_RRC_SIB_DEBUG */
          rrc_log_complete_sib(((rrc_CompleteSIBshort*)(temp_node->data))->sib_Type,0,msg->sfn_Prime);
          status = SIBS_COMPLETED;
        }
        if( seg_status == SEG_PROC_FAILED )
        {
          WRRC_MSG1_HIGH("Seg proc failed on seg %d",msg->payload.t);
        }
        temp_node = temp_node->next;
      }
      temp_node = NULL;
      list_ptr = NULL;
      break;


    case( T_rrc_SystemInformation_BCH_payload_lastAndCompleteAndFirst ):
    /* Last segment, one or more complete SIBS
    and a First segment included in this message */
      /* No SIB segments included in this message */
      MSG_LOW("Last Segment, complete SIBs and First Segment chosen", 0, 0, 0);
      /* First process the last segment */
      seg_status = process_short_last_seg( &(msg->payload.u.lastAndCompleteAndFirst.lastSegmentShort),
                                     index, msg->sfn_Prime );
      
      /* Since a last semgent is received,
      Indicate that SIBs have been completed. */
      if((seg_status == SEG_PROC_SUCCEEDED )
        ||(seg_status == SEG_PROC_COMPLETE)
        )
      {
#ifdef FEATURE_RRC_SIB_DEBUG
        WRRC_MSG2_HIGH("Received SIB %d, Sfn prime %d", 
                 msg->payload.u.lastAndCompleteAndFirst.lastSegmentShort.sib_Type,msg->sfn_Prime);
#endif /* FEATURE_RRC_SIB_DEBUG */
        rrc_log_complete_sib(msg->payload.u.lastAndCompleteAndFirst.lastSegmentShort.sib_Type,
                             msg->payload.u.lastAndCompleteAndFirst.lastSegmentShort.segmentIndex,
                             msg->sfn_Prime);
        status = SIBS_COMPLETED;
      }
      /* Then process each one of the complete SIBs given
      by the message. Copy the first complete SIB pointer to
      the local pointer and then traverse through subsequent 
      SIB pointers. */
      list_ptr = &(msg->payload.u.lastAndComplete.completeSIB_List);
      temp_node = list_ptr->head;
      
      if(temp_node == NULL)
      {
        WRRC_MSG0_ERROR("Invalid SIB pointer received");
        return FALSE;
      }

      while( temp_node != NULL )
      {
        seg_status = process_short_complete_sib( temp_node->data, index, msg->sfn_Prime );
        /* Update status for each SIB since we're in a while loop */
        if((seg_status == SEG_PROC_SUCCEEDED )
          ||(seg_status == SEG_PROC_COMPLETE)
          )
        {
#ifdef FEATURE_RRC_SIB_DEBUG
          WRRC_MSG2_HIGH("Received SIB %d, Sfn prime %d", 
                ((rrc_CompleteSIBshort*)(temp_node->data))->sib_Type,msg->sfn_Prime);
#endif /* FEATURE_RRC_SIB_DEBUG */
          rrc_log_complete_sib(((rrc_CompleteSIBshort*)(temp_node->data))->sib_Type,0,msg->sfn_Prime);
          status = SIBS_COMPLETED;
        }
        temp_node = temp_node->next;
        if( seg_status == SEG_PROC_FAILED )
        {
          WRRC_MSG1_HIGH("Seg proc failed on seg %d",msg->payload.t);
        }
      }
      /* Then Process the First segment */
      seg_status = process_short_first_seg( &(msg->payload.u.lastAndCompleteAndFirst.firstSegment),
                                            index, msg->sfn_Prime);
      if(seg_status == SEG_PROC_COMPLETE)
      {
        status = SIBS_COMPLETED;
      }
      temp_node = NULL;
      list_ptr = NULL;
      break;


    case( T_rrc_SystemInformation_BCH_payload_completeSIB_List ):
      /* One or more complete SIBs included in this message */
      MSG_LOW("Complete SIB List chosen", 0, 0, 0);
      /* Process each one of the complete SIBs given
      by the message. Copy the first complete SIB pointer to
      the local pointer and then traverse through subsequent 
      SIB pointers. */
      list_ptr = &(msg->payload.u.completeSIB_List);
      temp_node = list_ptr->head;

      if(temp_node == NULL)
      {
        WRRC_MSG0_ERROR("Invalid SIB pointer received");
        return FALSE;
      }

      while( temp_node != NULL )
      {
        seg_status = process_short_complete_sib( temp_node->data, index ,msg->sfn_Prime);
        /* Update status for each SIB since we're in a while loop */
        if((seg_status == SEG_PROC_SUCCEEDED )
           ||(seg_status == SEG_PROC_COMPLETE)
           )
        {
#ifdef FEATURE_RRC_SIB_DEBUG
          WRRC_MSG2_HIGH("Received SIB %d, Sfn prime %d", 
                   ((rrc_CompleteSIBshort*)(temp_node->data))->sib_Type,msg->sfn_Prime);
#endif /* FEATURE_RRC_SIB_DEBUG */
          rrc_log_complete_sib(((rrc_CompleteSIBshort*)(temp_node->data))->sib_Type,0,msg->sfn_Prime);
          status = SIBS_COMPLETED;
        }
        temp_node = temp_node->next;
        if( seg_status == SEG_PROC_FAILED )
        {
          WRRC_MSG1_HIGH("Seg proc failed on seg %d",msg->payload.t);
        }
      }
      temp_node = NULL;
      list_ptr = NULL;
      break;
      


    case( T_rrc_SystemInformation_BCH_payload_completeAndFirst ):
      /* One or more complete SIBs and a First segment
      included in this message */
      MSG_LOW("Complete SIBs and First Segment chosen", 0, 0, 0);
      /* Process each one of the complete SIBs given
      by the message. Copy the first complete SIB pointer to
      the local pointer and then traverse through subsequent 
      SIB pointers. */
      list_ptr = &(msg->payload.u.completeAndFirst.completeSIB_List);
      temp_node = list_ptr->head;
      
      if(temp_node == NULL)
      {
        WRRC_MSG0_ERROR("Invalid SIB pointer received");
        return FALSE;
      }

      while( temp_node != NULL )
      {
        seg_status = process_short_complete_sib(  temp_node->data, index, msg->sfn_Prime );
        /* Update status for each SIB since we're in a while loop */
        if((seg_status == SEG_PROC_SUCCEEDED )
          ||(seg_status == SEG_PROC_COMPLETE)
          )
        {
#ifdef FEATURE_RRC_SIB_DEBUG
          WRRC_MSG2_HIGH("Received SIB %d, Sfn prime %d", 
                   ((rrc_CompleteSIBshort*)(temp_node->data))->sib_Type, msg->sfn_Prime);
#endif /* FEATURE_RRC_SIB_DEBUG */
          rrc_log_complete_sib(((rrc_CompleteSIBshort*)(temp_node->data))->sib_Type,0,msg->sfn_Prime);
          status = SIBS_COMPLETED;
        }
        temp_node = temp_node->next;
        if( seg_status == SEG_PROC_FAILED )
        {
          WRRC_MSG1_HIGH("Seg proc failed on seg %d",msg->payload.t);
        }
      }
      /* Then Process the First segment */
      seg_status = process_short_first_seg( &(msg->payload.u.completeAndFirst.firstSegment),
                                            index, msg->sfn_Prime);
      if(seg_status == SEG_PROC_COMPLETE)
      {
        status = SIBS_COMPLETED;
      }
      temp_node = NULL;
      list_ptr = NULL;
      break;

    case( T_rrc_SystemInformation_BCH_payload_completeSIB ):
      /* A single complete SIB is included in this message */
      MSG_LOW("Single Complete SIB chosen", 0, 0, 0);
      seg_status = process_complete_sib( &(msg->payload.u.completeSIB), index, msg->sfn_Prime);
      /* Since 1 complete SIB is received,
      Indicate that SIBs have been completed. */
      if((seg_status == SEG_PROC_SUCCEEDED )
        ||(seg_status == SEG_PROC_COMPLETE)
        )
      {
#ifdef FEATURE_RRC_SIB_DEBUG
        WRRC_MSG2_HIGH("Received SIB %d, Sfn prime %d", 
                 msg->payload.u.completeSIB.sib_Type, msg->sfn_Prime);
#endif /* FEATURE_RRC_SIB_DEBUG */
        rrc_log_complete_sib(msg->payload.u.completeSIB.sib_Type,0,msg->sfn_Prime);
        status = SIBS_COMPLETED;
      }
      break;

    case( T_rrc_SystemInformation_BCH_payload_lastSegment ):
      MSG_LOW("Last Segment chosen", 0, 0, 0);
      /* A last segment is included in this message */
      seg_status = process_last_seg( &(msg->payload.u.lastSegment), index, msg->sfn_Prime);
      /* Since a last segment is received,
      Indicate that SIBs have been completed. */
      if((seg_status == SEG_PROC_SUCCEEDED )
        ||(seg_status == SEG_PROC_COMPLETE)
        )
      {
#ifdef FEATURE_RRC_SIB_DEBUG
        WRRC_MSG2_HIGH("Received SIB %d, Sfn prime %d", 
                 msg->payload.u.lastAndCompleteAndFirst.lastSegmentShort.sib_Type, msg->sfn_Prime);
#endif /* FEATURE_RRC_SIB_DEBUG */
        rrc_log_complete_sib(msg->payload.u.lastAndCompleteAndFirst.lastSegmentShort.sib_Type,
                             msg->payload.u.lastAndCompleteAndFirst.lastSegmentShort.segmentIndex,
                             msg->sfn_Prime);
        status = SIBS_COMPLETED;
      }
      break;

    default:
      /* Not a known choice value. Nothing to do here, note the
      status and move on. */
      WRRC_MSG0_HIGH("Unknown Segment choice");
      break;

  }/* End switch */

  /* Check for incorrect segment processing */
  if( seg_status  == SEG_PROC_FAILED )
  {
    WRRC_MSG1_HIGH("Seg proc failed on combination %d",msg->payload.t);
  }
  return( status ); /* Indicate whether any SIB has been completed or not. */
}/* End function process_sys_info_msg */
/*===========================================================================

FUNCTION rrcsib_clear_tsib_optimization_db

DESCRIPTION
  This function will clear tsib optimization data base.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rrcsib_clear_tsib_optimization_db
(
  void
)
{
  uint8 i;
  /*Initialize tsib optimization db here.
    Note:Dont use memset here as Zero value for some of the fields
     e.g sib_type a value of zero is valid & it say MIB, so best is to loop 
     through & init each member of structure*/
  for(i = 0;i<MAX_TEMP_SIB_SEGMENTS_TO_HOLD;i++)
  {
    tsib_optimization_db[i].index_in_use = FALSE;
    tsib_optimization_db[i].seg_data_length = 0;
    tsib_optimization_db[i].seg_index = 0;
    tsib_optimization_db[i].sfn_prime = 0;
    tsib_optimization_db[i].sib_seg_type = noSegment_chosen;
    tsib_optimization_db[i].sib_type = rrc_MIB;
    memset(tsib_optimization_db[i].seg_data_ptr,0x00, FIXED_SIB_DATA_LENGTH_BYTES);
  }
  /*Initialize tsib look up table*/
  memset(tsib_opt_db_look_up_table,FALSE,rrc_MAX_SIB);
}
/*===========================================================================

FUNCTION rrcsib_clear_a_sib_from_tsib_optim_db

DESCRIPTION
  This function will clear any segment(if its stored) of 
  a sib type from tsib optimization data base.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rrcsib_clear_a_sib_from_tsib_optim_db
(
  rrc_SIB_type rrc_sib_type
)
{
  uint8 i;
  /*Initialize tsib optimization db for a sib which is passed as input to this function
   */
  for(i = 0;i<MAX_TEMP_SIB_SEGMENTS_TO_HOLD;i++)
  {
    if((tsib_optimization_db[i].sib_type == rrc_sib_type)&&
       (tsib_optimization_db[i].index_in_use == TRUE))
    {
      tsib_optimization_db[i].index_in_use = FALSE;
      tsib_optimization_db[i].seg_data_length = 0;
      tsib_optimization_db[i].seg_index = 0;
      tsib_optimization_db[i].sfn_prime = 0;
      tsib_optimization_db[i].sib_seg_type = noSegment_chosen;
      /*We dont save MIB, as its processed the moment its rexd, 
       *so use it as default value to init 
       */
      tsib_optimization_db[i].sib_type = rrc_MIB;
      memset(tsib_optimization_db[i].seg_data_ptr,0x00, FIXED_SIB_DATA_LENGTH_BYTES);
    }
  }
  /*Re-set this sib type in tsib look up table*/
  tsib_opt_db_look_up_table[rrc_sib_type] = FALSE;
}
/*===========================================================================

FUNCTION rrcsib_find_free_index_in_tsib_optim_db

DESCRIPTION
  This function will return first free index in tsib optimization db.
  or the index which already has this sib segment
  
DEPENDENCIES
  None.

RETURN VALUE
  Free index into tsib optimization db.
  else MAX_TEMP_SIB_SEGMENTS_TO_HOLD i.e.
  non of the index are free.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 rrcsib_find_free_index_in_tsib_optim_db
(
  rrc_SIB_type rrc_sib_type,  /*This will indicate type of SIB,e.g MIB,SIB1...etc*/
  uint8 seg_index,  /*SIB segment index*/
  sib_segment_e_type sib_seg_type/*Used to indicate type of sib segment*/
)
{
  uint8 i;
  uint8 first_free_indx = MAX_TEMP_SIB_SEGMENTS_TO_HOLD;/*Init this value to invalid*/
  boolean index_found = FALSE;
  uint8 return_index = MAX_TEMP_SIB_SEGMENTS_TO_HOLD;
  
  for(i = 0;i<MAX_TEMP_SIB_SEGMENTS_TO_HOLD;i++)
  {
    if((tsib_optimization_db[i].index_in_use == TRUE)&&
        (tsib_optimization_db[i].sib_type == rrc_sib_type)&&
        (tsib_optimization_db[i].seg_index == seg_index)&&
        (tsib_optimization_db[i].sib_seg_type == sib_seg_type)
      )
    {
      /*If this segment is already present in tsib optimization db then return the
        *same index so that it will be overwritten with the new segment data
        */
      index_found = TRUE;
      break;
    }
    else if((tsib_optimization_db[i].index_in_use == FALSE)&&
            (first_free_indx == MAX_TEMP_SIB_SEGMENTS_TO_HOLD)
           )
    {
      first_free_indx = i;
    }
  }
  
  if(index_found == TRUE)
  {
    return_index = i;
  }
  else
  {
    return_index = first_free_indx;
  }

  return return_index;
}
/*===========================================================================

FUNCTION rrcsib_store_sib_seg_in_tsib_optim_db

DESCRIPTION
  This function will store received sib segment in tsib
  optimization db only if a index in tsib optimization db
  is free.
  
DEPENDENCIES
  None.

RETURN VALUE
  SEG_PROC_STORED = if sib segment is stored in tsib opt db
  SEG_PROC_FAILED  =  if sib segment could not be stored in tsib opt db
  
SIDE EFFECTS
  None.
===========================================================================*/
seg_proc_stat_e_type rrcsib_store_sib_seg_in_tsib_optim_db
(
  rrc_SIB_type rrc_sib_type,  /*This will indicate type of SIB,e.g MIB,SIB1...etc*/
  uint32 sfn_prime,  /*SFN prime at which this sib segment received*/
  uint8 seg_index,  /*SIB segment index*/
  sib_segment_e_type sib_seg_type,/*Used to indicate type of sib segment*/
  uint32 seg_data_length,  /*SIB segment data lenght*/
  void *seg_data_ptr,  /*pointer to sib segment data*/
  sib_events_index_e_type event_index    /* Index into the SIB events database
                                   for storing SIBs received in this msg */
)
{
  seg_proc_stat_e_type process_status = SEG_PROC_FAILED;

  if(EVENT_INDEX_VALID(event_index) ==FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    return process_status;
  }

  /* Rules for storing SIB segment in Tsib optimization db.
   1) If the event is UPDATE SIB then we should not store SIB segments till
       a MIB with updated value tag is received. This will ensure that we are not 
       storing any SIB segment which belongs to the repetition cycle just before 
       new sib's are  broadcasted.
  */
  
  if((sib_events_ptr->event[event_index].event_name == RRCSIB_UPDATE_SIBS)&&
       (sib_events_ptr->event[event_index].tempholder.mib_decoded == FALSE))
  {
    WRRC_MSG0_HIGH("New MIB not received for Update SIBs event");
    process_status = SEG_PROC_FAILED;
  }
  else
  {
    uint8 index;
    void *src;         /* Source pointer for the b_copy function */
    void *dest;        /* Destination pointer for the b_copy function */

    index = rrcsib_find_free_index_in_tsib_optim_db(rrc_sib_type,
                                            seg_index,
                                            sib_seg_type);
    if(index >= MAX_TEMP_SIB_SEGMENTS_TO_HOLD)
    {
      WRRC_MSG0_ERROR("Tsib optimization db is full,trashing rexd sib seg");
      process_status = SEG_PROC_FAILED;
    }
    else
    {
      process_status = SEG_PROC_STORED;
      WRRC_MSG2_HIGH("Store sib seg=%d in tsib opti db index=%d",rrcsib_map_int_sibtype_to_asn1_sibtype(rrc_sib_type),index);
      /*Found a free index in tsib optimization db
        continue storing rexd sib segment in tsib 
        optimization db*/
      tsib_optimization_db[index].index_in_use = TRUE;
      tsib_optimization_db[index].sib_type = rrc_sib_type;
      tsib_optimization_db[index].sib_seg_type = sib_seg_type;
      tsib_optimization_db[index].seg_data_length = seg_data_length;
      tsib_optimization_db[index].sfn_prime = sfn_prime;
      tsib_optimization_db[index].seg_index = seg_index;
  
      /* Now copy the actual data bits. Use the bit copy function.
         Destination is the sib data pointer inside the sib events database.
         Source is the received segment's data pointer.  Destination offset
         is the next bit after the total received bits so far. */
      dest = (void *) tsib_optimization_db[index].seg_data_ptr;
      src = seg_data_ptr;
    
      b_copy( src,  /* Source Pointer */
              0,    /* Source Offset is always 0 since we copy all the received bits */
              dest, /* Destination Pointer */
              0,  /* Destination Offset */
              seg_data_length  /* Length of the subsequent segment is fixed */
            );
  
      tsib_opt_db_look_up_table[rrc_sib_type] = TRUE;
    }
  }
  return(process_status);
}
/*===========================================================================

FUNCTION rrcsib_is_sib_seg_in_tsib_optim_db

DESCRIPTION
  This function will say whether a sib segment 
  of a sib type is stored in tsib optimization db
  by looking at look up table..
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE : if a sib segment is stored in tsib optimization db
  FASLE: otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
boolean rrcsib_is_sib_seg_in_tsib_optim_db
(
  rrc_SIB_type rrc_sib_type
)
{
  return(tsib_opt_db_look_up_table[rrc_sib_type]);
}
/*===========================================================================

FUNCTION rrcsib_process_sib_seg_in_tsib_optim_db

DESCRIPTION
  This function will process stored sib sements 
  in tsib optimization db if any by looking at
  look up table..
  
DEPENDENCIES
  None.

RETURN VALUE
  If a complete sib is constructed at the end of this function then returns = SEG_PROC_COMPLETE.
  else
  Return SEG_PROC_NONE 
  
SIDE EFFECTS
  None.
===========================================================================*/
seg_proc_stat_e_type rrcsib_process_sib_seg_in_tsib_optim_db
(
  rrc_SIB_type rrc_sib_type,   /*Sib type*/
  sib_events_index_e_type index    /* Index into the SIB events database
                                   for storing SIBs received in this msg */
)
{
  /*Stores seg processing result, which will be used to return from this function*/
  seg_proc_stat_e_type seg_process_status = SEG_PROC_NONE;

  if(EVENT_INDEX_VALID(index) ==FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    seg_process_status = SEG_PROC_FAILED;
    return seg_process_status;
  }

  if(rrc_sib_type >= rrc_ExtensionType)
  {
    WRRC_MSG0_ERROR("Invalid SIB Type");
    seg_process_status = SEG_PROC_FAILED;
    return seg_process_status;
  }

  if(rrcsib_is_sib_seg_in_tsib_optim_db(rrc_sib_type) == TRUE)
  {
    void *src;         /* Source pointer for the b_copy function */
    void *dest;        /* Destination pointer for the b_copy function */
    uint8 i;              /*Loop count index*/
    word dest_offset;  /* Destination offset for the b_copy function */
    boolean search_continue_flag = FALSE;

    WRRC_MSG1_HIGH("Sib segment for sib type =%d, found in tbib optim db ",rrcsib_map_int_sibtype_to_asn1_sibtype(rrc_sib_type));
    /*Start processing sib segments from tsib optimization db*/
    do
    {
      /*search_continue_flag is used to loop through when ever a 
        subsequent sib segment is received.We need to re-set this 
        flag here so that in this iteration, if we dont find a sib segment
        in tsib optimization db then we have tostop searching*/
      search_continue_flag = FALSE;
      for(i = 0;i<MAX_TEMP_SIB_SEGMENTS_TO_HOLD;i++)
      {
        if((tsib_optimization_db[i].index_in_use == TRUE)&&
           (tsib_optimization_db[i].sib_type == rrc_sib_type))
        {
      
          switch(tsib_optimization_db[i].sib_seg_type)
          {
            case noSegment_chosen:
              /*Do nothing here*/
              break;

            case firstSegment_chosen:
              WRRC_MSG1_HIGH("Concatinating first segment for sib %d ", rrcsib_map_int_sibtype_to_asn1_sibtype(rrc_sib_type));
              search_continue_flag = TRUE;
              /* Now copy the actual data bits. Use the bit copy function.
              Destination is the sib data pointer inside the sib events database.
              Source is the received segment's data pointer. */
              dest = (void *)sib_events_ptr->event[index].tsib[rrc_sib_type].sib_data_ptr;
              src = (void *)tsib_optimization_db[i].seg_data_ptr;
            
              b_copy( src,  /* Source Pointer */
                      0,    /* Source Offset is always 0 since we copy all the received bits */
                      dest, /* Destination Pointer */
                      0,    /* Destination Offset is 0 since this is the first segment */
                      tsib_optimization_db[i].seg_data_length/* Length of the first segment is fixed */
                    );
            
              /* Note the current length of this SIB */
              sib_events_ptr->event[index].tsib[rrc_sib_type].sib_tot_length = tsib_optimization_db[i].seg_data_length;
            
              /* Since a first segment has been received, we can now make the segment
              index 0. */
              sib_events_ptr->event[index].tsib[rrc_sib_type].seg_index = 0;

                /*Clear Tsib db index right here*/
              tsib_optimization_db[i].index_in_use = FALSE;
              tsib_optimization_db[i].seg_data_length = 0;
              tsib_optimization_db[i].seg_index = 0;
              tsib_optimization_db[i].sfn_prime = 0;
              tsib_optimization_db[i].sib_seg_type = noSegment_chosen;
              /*We dont save MIB, as its processed the moment its rexd, 
               *so use it as default value to init 
               */
              tsib_optimization_db[i].sib_type = rrc_MIB;
              memset(tsib_optimization_db[i].seg_data_ptr,0x00, FIXED_SIB_DATA_LENGTH_BYTES);
  
              break;

            case subsequentSegment_chosen:
              if( sib_events_ptr->event[index].tsib[rrc_sib_type].seg_index == 
                  (uint32)(tsib_optimization_db[i].seg_index - 1))
              {
                WRRC_MSG2_HIGH("Concatinating Subsequent segment index=%d for SIB type=%d",tsib_optimization_db[i].seg_index,rrcsib_map_int_sibtype_to_asn1_sibtype(rrc_sib_type));
                search_continue_flag = TRUE;
                dest = (void *)sib_events_ptr->event[index].tsib[rrc_sib_type].sib_data_ptr;
                src = (void *)tsib_optimization_db[i].seg_data_ptr;
                /* Ok to typecast to word here since maximum value for length is 3552 */
                dest_offset = (word)(sib_events_ptr->event[index].tsib[rrc_sib_type].sib_tot_length);
        
                b_copy( src,  /* Source Pointer */
                        0,    /* Source Offset is always 0 since we copy all the received bits */
                        dest, /* Destination Pointer */
                        dest_offset,  /* Destination Offset */
                        FIXED_SIB_DATA_LENGTH_BITS  /* Length of the subsequent segment is fixed */
                      );
        
                /* Note the current length of this SIB. Add this segments length to the 
                 previous total. */
                sib_events_ptr->event[index].tsib[rrc_sib_type].sib_tot_length = 
                 (sib_events_ptr->event[index].tsib[rrc_sib_type].sib_tot_length + 
                 FIXED_SIB_DATA_LENGTH_BITS);
                sib_events_ptr->event[index].tsib[rrc_sib_type].seg_index = tsib_optimization_db[i].seg_index;
                /*Clear Tsib db index right here*/
                tsib_optimization_db[i].index_in_use = FALSE;
                tsib_optimization_db[i].seg_data_length = 0;
                tsib_optimization_db[i].seg_index = 0;
                tsib_optimization_db[i].sfn_prime = 0;
                tsib_optimization_db[i].sib_seg_type = noSegment_chosen;
                /*We dont save MIB, as its processed the moment its rexd, 
                 *so use it as default value to init 
                 */
                tsib_optimization_db[i].sib_type = rrc_MIB;
                memset(tsib_optimization_db[i].seg_data_ptr,0x00, FIXED_SIB_DATA_LENGTH_BYTES);
              }
              break;
              
              case lastSegment_chosen:
              case lastSegmentShort_chosen:
                if(sib_events_ptr->event[index].tsib[rrc_sib_type].seg_index == 
                  (uint32)(tsib_optimization_db[i].seg_index - 1))
                {
                  WRRC_MSG2_HIGH("Concatinating Last segment index=%d for SIB type=%d",tsib_optimization_db[i].seg_index,rrcsib_map_int_sibtype_to_asn1_sibtype(rrc_sib_type));
                  search_continue_flag = FALSE;
                  dest = (void *)sib_events_ptr->event[index].tsib[rrc_sib_type].sib_data_ptr;
                  src = (void *)tsib_optimization_db[i].seg_data_ptr;
                  /* Ok to typecast to word here since maximum value for length is 3552 */
                  dest_offset = (word)(sib_events_ptr->event[index].tsib[rrc_sib_type].sib_tot_length);
                       
                  b_copy( src,  /* Source Pointer */
                          0,    /* Source Offset is always 0 since we copy all the received bits */
                          dest, /* Destination Pointer */
                          dest_offset,  /* Destination Offset */
                          tsib_optimization_db[i].seg_data_length/* Length of the subsequent segment is fixed */
                        );

                  /* Note the current length of this SIB. Add this segments length to the 
                   previous total. */
                  sib_events_ptr->event[index].tsib[rrc_sib_type].sib_tot_length = 
                   (sib_events_ptr->event[index].tsib[rrc_sib_type].sib_tot_length + 
                   tsib_optimization_db[i].seg_data_length);

                  sib_events_ptr->event[index].tsib[rrc_sib_type].seg_index = tsib_optimization_db[i].seg_index;
                  /* Since this is the last segment, set the "sib_received" indicator
                     to TRUE so that we won't process this SIB again. */
                  sib_events_ptr->event[index].tsib[rrc_sib_type].sib_received = TRUE;
                  seg_process_status = SEG_PROC_COMPLETE;
                  WRRC_MSG1_HIGH("Received SIB %d",rrcsib_map_int_sibtype_to_asn1_sibtype(rrc_sib_type));
                  rrcsib_clear_a_sib_from_tsib_optim_db(rrc_sib_type);
                }
                break;

              case completeSIB_chosen:
                search_continue_flag = FALSE;
                /* Now copy the actual data bits. Use the bit copy function.
                Destination is the sib data pointer inside the sib events database.
                Source is the received segment's data pointer. */
                dest = (void *)sib_events_ptr->event[index].tsib[rrc_sib_type].sib_data_ptr;
                src = (void *)tsib_optimization_db[i].seg_data_ptr;

                b_copy( src,  /* Source Pointer */
                        0,    /* Source Offset is always 0 since we copy all the received bits */
                        dest, /* Destination Pointer */
                        0,    /* Destination Offset is 0 since this is the first segment */
                        tsib_optimization_db[i].seg_data_length/* Length of the first segment is fixed */
                      );
                sib_events_ptr->event[index].tsib[rrc_sib_type].sib_tot_length = 
                  tsib_optimization_db[i].seg_data_length;
                /* Since this is a complete SIB, set the "sib_received" indicator
                to TRUE so that we won't process this SIB again. */
                sib_events_ptr->event[index].tsib[rrc_sib_type].sib_received = TRUE;

                seg_process_status = SEG_PROC_COMPLETE;
                rrcsib_clear_a_sib_from_tsib_optim_db(rrc_sib_type);

                break;

              default:
                WRRC_MSG1_HIGH("Invalid seg type =%d",tsib_optimization_db[i].sib_seg_type);
                break;
          }
        }
      }
    }while(search_continue_flag == TRUE);
    
  }
  else
  {
    /*Do nothing here*/
  }
  return(seg_process_status);
}

/*===========================================================================

FUNCTION rrcsib_map_extension_sib_to_internal_sibtype

DESCRIPTION
  This function will return the SIB type in internal format for extension SIBs. It will find out the SIB type based on
  the scheduling info and SFN at which the SIB arrives.
  
DEPENDENCIES
  None.

RETURN VALUE
  Returns the SIB type based on the SFN at which it arrives. 
  
SIDE EFFECTS
  None.
===========================================================================*/
rrc_SIB_type rrcsib_map_extension_sib_to_internal_sibtype
(
  rrc_SegmentIndex seg_index,
  uint32 sfn_prime
)
{
  rrc_SIB_type sib = rrc_MAX_SIB;

  if(sib_events_ptr->sib_sched[rrc_SIB11bis].sfn_offset[seg_index] == 
             ((sfn_prime<<1) % sib_events_ptr->sib_sched[rrc_SIB11bis].repitition_rate))
  {
    sib = rrc_SIB11bis;
  }
#ifdef FEATURE_WCDMA_TO_LTE
  else if(sib_events_ptr->sib_sched[rrc_SIB19].sfn_offset[seg_index] == 
           ((sfn_prime<<1) % sib_events_ptr->sib_sched[rrc_SIB19].repitition_rate))
  {
    sib = rrc_SIB19;
  }
#endif
  return sib;
}


#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
/*===========================================================================

FUNCTION rrcsib_store_sib_seg_in_deferred_sib_db

DESCRIPTION
  This function will clear all SIBs in tsib optimization data base except SIB11/11bis/12.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rrcsib_store_sib_seg_in_deferred_sib_db
(
  void
)
{
  uint8 i, j=0;
  WRRC_MSG0_HIGH("Deferred SIB: Copying TSIB to Deferred SIB DB");
  
  for(i = 0;i < MAX_DEFERRED_SIB_SEGMENTS_TO_HOLD;i++)
  {
    deferred_sib_info.deferred_sib_seg_db[i].index_in_use = FALSE;
    deferred_sib_info.deferred_sib_seg_db[i].seg_data_length = 0;
    deferred_sib_info.deferred_sib_seg_db[i].seg_index = 0;
    deferred_sib_info.deferred_sib_seg_db[i].sfn_prime = 0;
    deferred_sib_info.deferred_sib_seg_db[i].sib_seg_type = noSegment_chosen;
    deferred_sib_info.deferred_sib_seg_db[i].sib_type = rrc_MAX_SIB;
    memset(deferred_sib_info.deferred_sib_seg_db[i].seg_data_ptr,0x00, FIXED_SIB_DATA_LENGTH_BYTES);
  }
  
  for(i = 0;i < MAX_TEMP_SIB_SEGMENTS_TO_HOLD;i++)
   {
     if((tsib_optimization_db[i].sib_type == rrc_SIB11)||
        (tsib_optimization_db[i].sib_type == rrc_SIB12)||
        (tsib_optimization_db[i].sib_type == rrc_SIB11bis) ||
        (tsib_optimization_db[i].sib_type == rrc_SIB18)
#ifdef FEATURE_WCDMA_TO_LTE
        ||(tsib_optimization_db[i].sib_type == rrc_SIB19)
#endif
        )
     {
       deferred_sib_info.deferred_sib_seg_db[j] = tsib_optimization_db[i];
       WRRC_MSG2_HIGH("Deferred SIB: Copying TSIB index %d to Deferred SIB index %d", i ,j);
       j = j+1;
     }
   }
  return;
}

#endif


