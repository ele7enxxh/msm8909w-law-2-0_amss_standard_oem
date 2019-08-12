#ifndef MAC_DL_HS_H
#define MAC_DL_HS_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              D O W N L I N K   M A C   H S D P A   H E A D E R    F I L E

DESCRIPTION
   MAC HS specific type declarations.


Copyright (c) 2001, 2002, 2003 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2005-2006, 2008-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/src/macdlhs.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who      what, where, why
--------   -------- --------------------------------------------------------
01/30/14   kv       0x421E/0x4220 Logging Fixes for 3C and WM debug prints
12/09/13   as       Code change to EDRX full sleep if there pending partials at MAC. 
10/30/13   scv      Enhancement for 3C-HSDPA
04/02/13   kc       Fix to restart T1 timer only if there exists a TSN that cannot be delivered to RLC
01/15/13   ksr      When EFACH configured let the TSN 0 experience T1 timer delay as per the spec
                    (Remove the open window mechanism which is out of the spec implementation)
07/13/12   grk      MBMS feature cleanup at L2(MBMS_DEBUG_1, FEATURE_MODEM_MBMS)
01/02/12   grk      Feature cleanup
04/26/11   vp       Created the defintion of MAC_HS_FACH_OR_PCH_STATE() when
                    HS feature is not defined
03/09/11   ssg      MAC-ehs support on Apollo.
01/31/11   grk      Fixed compiler warning.
01/31/11   mrg      Fixed compilation error.
12/20/10   grk      WCDMA_INTLOCK() cleanup.
12/16/10   mrg      In case of RACH abort and persistency failure MAC informs RLC to re-send status report
11/11/10   mrg      MAC DL HS ZI memory optimization changes
10/19/10   kvk      Added support to flush the TSNs when HS is stopped
09/19/1 0  av       HS_FACH and Enhanced 0x4220 log changes
09/01/10   grk      Fixed compiler warning.
06/14/10   grk      Fixed compilation errors/warnings.
06/08/10   av       Increasing the number of complete PDU structure 
                    in Mac_ehs_tsn_log_bug to prevent out of bound access
                    in MAC-HS
06/01/10   grk      Fixed compilation error for silver MOB.
05/06/10   grk      Fixed compiler warning
04/30/10   prk      Changes for MAC QTF.
02/11/10   prk      MAC-hs timer optimization for CSoHS call.
02/08/10   prk      Defined an intermediate queue in MAC. In DL data ISR 
                    received TSNs are put into this queue; MAC-hs dequeue
                    TSN items from this queue in task context and put them 
                    into reordering buffer.
01/20/10   prk      Fixing compilation error due to featurization.
01/20/10   prk      Added structure type declarations,used in 0x421E logging.
07/30/09   prk      Macros MAC_HS_IS_TSN_INSIDE_WIN and MAC_HS_IS_TSN_OUTSIDE_WIN
                    are added to detect the TSN inside or outside of window.
07/29/09   prk      IS_TSN_NOT_WITHIN_BUF is added which checks whether TSN 
                    Memory is within mcalwcdma_dec_tsn_info_buf memory
05/07/09   ssg      Updated Copyright Information
04/30/09   av       Fixed MOB errors 
04/29/09   av       Fixed compiler warnings in L2 
10/17/08   grk      Merged taxis changes to mainline.
04/25/08   mrg      Check for received flag is added in MAC_HS_IS_DATA_RECEIVED macro 
03/24/08   pj       Added support for data path optimizations under 
                    FEATURE_WCDMA_UL_DATA_PATH_OPT & FEATURE_WCDMA_DL_DATA_PATH_OPT
           ksr      1. MAC DL Changes for Data path optimisations
02/26/08   sk       Restore the next expected TSN values and the winddow up edge and low edge
                    if the restore flag is set to true. This is under
                    FEATURE_HSDPA_MAC_HS_DL_RESTORE_TSN
08/24/06   gsc      Increased Max. # of TB sets in 10 ms to 32. Increased max.
                    # of TSNs in 10 ms to (32-UE_MAX_TRCH) = 24.
05/24/06   gsc      Increased the double buffer space to 32.
04/26/06   gsc      Added support for HSDPA re-config optimizations
04/03/06   gsc      Added support to just perform flush and no reset for IRAT
                    handover cases (CCO/HHO).
03/15/06   gsc      MAC_HS_DL_MAX_MAC_D_PDUS_PER_SUB_FRAME now refers to
                    UE_MAX_DL_RLC_PDUS instead of being hard-coded to 70.
11/04/05   sk/gsc   Added declaration for function mac_hs_dl_reset_local_config_data
09/09/05   gsc      Fix for CR 75199
03/08/05   gsc      Added tsn field to mac_hs_tsn_info_type for debugging.
03/07/05   gsc      Lint Fixes.
02/17/05   gsc      Completed implementation of mac_hs_reset as per discussions
                    with L1 folks.
01/25/05   gsc      Added new macros MAC_HS_IS_TSN_BEFORE_OR_EQ;
                    MAC_HS_IS_TSN_LATER_OR_EQ. Cleaned up other macros.
01/12/05   gsc      T1 timer related fixed observed during IOT. Added 1 sec timer
                    for reporting status log pkt.
12/23/04   gsc      Fixes for issues found during HSDPA feature integration in the lab.
11/17/04   gsc      Changed MAC_HS_DL_MAC_D_FLOW_ID_IS_INVALID() to use the
                    mac_dflow_id range of 0-7.
11/16/04   gsc      Changed mac_dflow_id range from 1-8 to 0-7. Added logging
                    support.
11/01/04   gsc      Updated MAC_HS_DL_NUM_QUEUE_IS_INVALID macro.
10/26/04   gsc      Added description in function headers.
10/11/04   gsc      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"

#ifdef CUST_H
#include "customer.h"
#endif

#include "macrrcif_v.h"
#include "macinternal.h"
#include "rex.h"
/*===========================================================================

                     DEFINES AND MACROS FOR MODULE

===========================================================================*/
extern uint8 mac_hs_dl_tsn_bits;
#define MAC_HS_DL_MIN_MAC_D_FLOWS                  1
#define MAC_HS_DL_MAX_MAC_D_FLOWS                  UE_MAX_MAC_D_FLOW
#define MAC_HS_DL_MIN_MAC_D_FLOW_ID                0
#define MAC_HS_DL_MAX_MAC_D_FLOW_ID                (UE_MAX_MAC_D_FLOW - 1)
#define MAC_HS_DL_MIN_QUEUES                       1
#define MAC_HS_DL_MAX_QUEUES                       UE_MAX_HS_QUEUE
#define MAC_HS_DL_MIN_LC_PER_FLOW                  1
#define MAC_HS_DL_MAX_LC_PER_FLOW                  UE_MAX_LOGCHAN_PER_DFLOW
#define MAC_HS_DL_MIN_QUEUE_ID                     0
#define MAC_HS_DL_MAX_QUEUE_ID                     (UE_MAX_HS_QUEUE - 1)
#define MAC_HS_DL_MIN_SIDS                         1
#define MAC_HS_DL_MAX_SIDS                         UE_MAX_HS_NO_OF_PDU_SIZE_PER_QUEUE
#define MAC_HS_DL_MIN_PDU_SIZE                     1
#define MAC_HS_DL_MAX_PDU_SIZE                     5000
#define MAC_HS_DL_MIN_PDU_SIZE_INDEX               0
#define MAC_HS_DL_MAX_PDU_SIZE_INDEX               (UE_MAX_HS_NO_OF_PDU_SIZE_PER_QUEUE - 1)
#define MAC_HS_DL_MAX_MAC_D_PDUS_PER_SUB_FRAME     UE_MAX_DL_RLC_PDUS
#ifdef FEATURE_WCDMA_3C_HSDPA
#define MAC_EHS_DL_MAX_PDUS_PER_SUB_FRAME          44
#define MAC_HS_DL_MAX_WINDOW_SIZE                  128
#else
#define MAC_EHS_DL_MAX_PDUS_PER_SUB_FRAME          26
#define MAC_HS_DL_MAX_WINDOW_SIZE                  32
#endif
#define MAC_HS_DL_MAX_NUM_TSN                      (1<<mac_hs_dl_tsn_bits)
#define MAC_HS_DL_MAX_TSN                          (MAC_HS_DL_MAX_NUM_TSN - 1) 
#define MAC_HS_DL_MIN_REL_TIMER_VAL                10
#define MAC_HS_DL_MAX_REL_TIMER_VAL                400
#define MAC_HS_DL_UNDEF_TSN                        0xFFFF
#define MAC_HS_MAX_NUM_TB_SET_PER_10_MS_FRAME      32
#define MAC_HS_DL_MAX_TSN_PER_10_MS_FRAME          (32 - UE_MAX_TRCH)

#ifdef FEATURE_WCDMA_HS_FACH
#define MAC_HS_MAX_RESET_TIMER_VAL                 4
#define MAC_HS_MIN_RESET_TIMER_VAL                 1
#endif

#if ((defined (FEATURE_HSDPA_MAC_EHS)) && (defined (FEATURE_WCDMA_REL7)))
#define MAC_EHS_DL_MAX_TSN_PER_10_MS_FRAME         3
#endif

#define INVALID_HS_QUEUE_ID                        8

#define MAC_HS_MAX_SAVED_TSNS_DURING_RECONFIG      32

#ifdef FEATURE_MAC_HS_QXDM_LOGGING
#define MAC_HS_DL_STATUS_LOG_TIMER_VAL             1000
/*  Store the information for 50 TSNs */
#define MAC_EHS_NO_OF_TSN_LOG          50
/*  In one TSN network can send maximum of 26 puds*/
/* Arun- the logging is also done in MACHS, putting it to 26 causes outofbound access and causes crash*/
#define MAC_EHS_NO_OF_PDU_IN_TSN_LOG           70
/*  In one TSN network can send maximum of 2 partial pdus*/
#define MAC_EHS_NO_OF_PART_PDU_IN_TSN_LOG      2
#endif


/*===========================================================================

                     KEY MACROS USED IN WINDOW PROCESSING IN MAC-HS

===========================================================================*/

/*--------------------------------------------------------------------------
 * Routine that indicates if T1 is with in the window (T2<lower edge>,T3<upper edge>)
 *-------------------------------------------------------------------------*/
#define MAC_HS_IS_TSN_INSIDE_WIN(t1, t2, t3, window_size) \
         MAC_HS_IS_TSN_LATER_OR_EQ(t1, t2, window_size) && MAC_HS_IS_TSN_BEFORE_OR_EQ(t1, t3, window_size)

/*--------------------------------------------------------------------------
 * Routine that indicates if T1 is outside the window (T2<lower edge>,T3<upper edge>)
 *-------------------------------------------------------------------------*/
#define MAC_HS_IS_TSN_OUTSIDE_WIN(t1, t2, t3, window_size) \
         (!(MAC_HS_IS_TSN_INSIDE_WIN(t1, t2, t3, window_size))) 

/*--------------------------------------------------------------------------
 * Routine that indicates if T1 is later than (>) T2
 *-------------------------------------------------------------------------*/
#define MAC_HS_IS_TSN_LATER(t1, t2, window_size)    \
         ((t1!=t2)&&(SUB_MOD(t1, t2, MAC_HS_DL_MAX_TSN) < window_size))          

/*--------------------------------------------------------------------------
 * Routine that indicates if T1 is later than or equal to (>=) T2
 *-------------------------------------------------------------------------*/
#define MAC_HS_IS_TSN_LATER_OR_EQ(t1, t2, window_size) \
      (SUB_MOD(t1, t2, MAC_HS_DL_MAX_TSN) < window_size)

/*--------------------------------------------------------------------------
 * Routine that indicates if T1 is befire than (<) T2
 *-------------------------------------------------------------------------*/
#define MAC_HS_IS_TSN_BEFORE(t1, t2, window_size) \
      ((t1!=t2)&&(SUB_MOD(t2, t1, MAC_HS_DL_MAX_TSN) < window_size))    

/*--------------------------------------------------------------------------
 * Routine that indicates if T1 is before than or equal to (<=) T2
 *-------------------------------------------------------------------------*/
#define MAC_HS_IS_TSN_BEFORE_OR_EQ(t1, t2, window_size) \
      (SUB_MOD(t2, t1, MAC_HS_DL_MAX_TSN) < window_size)

/*--------------------------------------------------------------------------
 * Routines to advance a TSN by a given amount
 *-------------------------------------------------------------------------*/
#define ADD_MOD( a, b, m ) \
      ( ( (a) + (b) < (m) ) ? ( (a) + (b) ) : ( (a) + (b) - (m) ) )

/* Routine to Add TSNs */
#define ADD_TSN( a, b ) ADD_MOD( (a), (b), MAC_HS_DL_MAX_NUM_TSN )

/*--------------------------------------------------------------------------
 * Routines to decrement a TSN by a given amount
 *-------------------------------------------------------------------------*/
#define SUB_MOD( a, b, m ) \
      ( ( (a) >= (b) ) ? ( (a) - (b) ) : ( (m) + (a) - (b) ) )

/* Routine to subtract TSNs */
#define SUB_TSN( a, b ) SUB_MOD( (a), (b), MAC_HS_DL_MAX_NUM_TSN )

/*--------------------------------------------------------------------------
 * Routine to Extract Lower Byte
 *-------------------------------------------------------------------------*/
#define MAC_DL_GET_LOWER_BYTE(a) (a & 0x00FF)
/*--------------------------------------------------------------------------
 * Routine to Extract Upper Byte 
 *-------------------------------------------------------------------------*/
#define MAC_DL_GET_UPPER_BYTE(a) ((a & 0xFF00) >> 8)

/*===========================================================================

                     GENERAL PURPOSE MACROS

===========================================================================*/

#define MAC_HS_DL_QUEUE_ID_IS_INVALID(queue_id)\
      (queue_id > MAC_HS_DL_MAX_QUEUE_ID)

#define MAC_HS_DL_TSN_IS_INVALID(tsn)\
      (tsn > MAC_HS_DL_MAX_TSN)

#define MAC_HS_DL_NUM_MAC_D_FLOW_IS_INVALID(num_flows)\
      ((num_flows < MAC_HS_DL_MIN_MAC_D_FLOWS) || \
       (num_flows > MAC_HS_DL_MAX_MAC_D_FLOWS))

#define MAC_HS_DL_MAC_D_FLOW_ID_IS_INVALID(flow_id)\
       (flow_id > MAC_HS_DL_MAX_MAC_D_FLOW_ID)

#define MAC_HS_DL_NUM_QUEUE_IS_INVALID(num_queues)\
       (num_queues > MAC_HS_DL_MAX_QUEUES)

#define MAC_HS_DL_NUM_LC_PER_FLOW_IS_INVALID(num_dlc)\
       ((num_dlc < MAC_HS_DL_MIN_LC_PER_FLOW) || \
        (num_dlc > MAC_HS_DL_MAX_LC_PER_FLOW))

#define MAC_HS_DL_NUM_SID_IS_INVALID(num_sids)\
       ((num_sids < MAC_HS_DL_MIN_SIDS) || \
        (num_sids > MAC_HS_DL_MAX_SIDS))

#define MAC_HS_DL_PDU_SIZE_IS_INVALID(pdu_size)\
        ((pdu_size < MAC_HS_DL_MIN_PDU_SIZE) || \
         (pdu_size > MAC_HS_DL_MAX_PDU_SIZE))

#define MAC_HS_DL_PDU_SIZE_INDEX_IS_INVALID(pdu_size_index)\
         (pdu_size_index > MAC_HS_DL_MAX_PDU_SIZE_INDEX)

#define MAC_HS_DL_NUM_TB_PER_TSN_IS_INVALID(num_tb)\
         (num_tb > MAC_HS_DL_MAX_MAC_D_PDUS_PER_SUB_FRAME)

#define MAC_HS_DL_NUM_BLKS_PER_SID_IS_INVALID(num_blks)\
         (num_blks > MAC_HS_DL_MAX_MAC_D_PDUS_PER_SUB_FRAME)

#define MAC_HS_DL_REL_TIMER_IS_INVALID(timer_val)\
        ((timer_val < MAC_HS_DL_MIN_REL_TIMER_VAL) || \
         (timer_val > MAC_HS_DL_MAX_REL_TIMER_VAL))

#define MAC_HS_DL_WINDOW_SIZE_IS_INVALID(size)\
         (size > MAC_HS_DL_MAX_WINDOW_SIZE)

#ifdef FEATURE_WCDMA_HS_FACH
#define MAC_HS_RESET_TIMER_INVALID(reset_timer)\
          ((reset_timer < MAC_HS_MIN_RESET_TIMER_VAL)|| \
          (reset_timer > MAC_HS_MAX_RESET_TIMER_VAL))

#define MAC_HS_FACH_OR_PCH_STATE()  (mac_hs_reset_timer_valid_state)

#endif

#define MAC_HS_IS_DATA_RECEIVED(data_received, int_queue_ptr, tsn)  \
       data_received = (((int_queue_ptr->tsn_info[(tsn&(MAC_HS_DL_MAX_WINDOW_SIZE-1))] != NULL) &&  \
                         (int_queue_ptr->tsn_info[(tsn&(MAC_HS_DL_MAX_WINDOW_SIZE-1))]->received == TRUE)) ? TRUE : FALSE)

#define MAC_HS_GET_TSN_PTR(int_queue_ptr, tsn, tsn_info_ptr) \
        tsn_info_ptr = int_queue_ptr->tsn_info[(tsn&(MAC_HS_DL_MAX_WINDOW_SIZE-1))] 

#define MAC_HS_PUT_TSN_PTR(int_queue_ptr, tsn, tsn_info_ptr) \
        int_queue_ptr->tsn_info[(tsn&(MAC_HS_DL_MAX_WINDOW_SIZE-1))] = tsn_info_ptr 

#define MAC_HS_GET_SAVED_TSN_PTR(mac_hs_saved_tsn_data, i, tsn_info_ptr) \
        tsn_info_ptr = mac_hs_saved_tsn_data[i].tsn_info 

#define MAC_HS_PUT_SAVED_TSN_PTR(mac_hs_saved_tsn_data, i, tsn_info_ptr) \
        mac_hs_saved_tsn_data[i].tsn_info = tsn_info_ptr 

#if ((defined (FEATURE_HSDPA_MAC_EHS)) && (defined (FEATURE_WCDMA_REL7)))
#define MAC_EHS_DL_NUM_TB_PER_TSN_IS_INVALID(num_tb)\
         (num_tb > MAC_EHS_DL_MAX_PDUS_PER_SUB_FRAME)
#endif /* ((defined (FEATURE_HSDPA_MAC_EHS)) && (defined (FEATURE_WCDMA_REL7))) */

#define MAC_HS_GET_REORDERING_QUEUE_PTR(int_queue_ptr,queue_id) \
         int_queue_ptr = mac_hs_reordering_entity.reordering_queue[queue_id]

#if ((defined (FEATURE_HSDPA_MAC_EHS )) && (defined (FEATURE_WCDMA_REL7)))

#define HSPA_PLUS_MAC_HS_TYPE_VALID_CHECK(mac_hs_or_ehs) \
\
   if(mac_hs_or_ehs == MAC_HS_TYPE_INVALID) {  \
\
     MSG_ERROR("EHS_ERR: MAC_HS_TYPE is Invalid ",0,0,0);   \
\
   }  \

#endif

#define IS_TSN_NOT_WITHIN_BUF(tsn_info_ptr)  ((tsn_info_ptr < &mcalwcdma_dec_tsn_info_buf[0]) || \
				( tsn_info_ptr > &mcalwcdma_dec_tsn_info_buf[MCALWCDMA_DEC_MAX_TSN_INFO_BUF]))

#ifdef FEATURE_WCDMA_3C_HSDPA
/*--------------------------------------------------------------------------
 * Routine to Update TSN info for 0x421E log packet
 *-------------------------------------------------------------------------*/
#define MAC_EHS_LOG_FILL_TSN_INFO(cfg_pkt_ptr,rcvd_tsn,num_partial_pdus,num_comp_pdus,    \
                     next_expected_tsn,upper_edge,timer_tsn)                              \
do{ \
  *cfg_pkt_ptr++ = (num_partial_pdus + num_comp_pdus);                                \
  *cfg_pkt_ptr++ = (uint8)MAC_DL_GET_LOWER_BYTE(rcvd_tsn);                          \
  *cfg_pkt_ptr++ = (uint8)MAC_DL_GET_UPPER_BYTE(rcvd_tsn);                          \
  *cfg_pkt_ptr++ = (uint8)MAC_DL_GET_LOWER_BYTE(next_expected_tsn);                 \
  *cfg_pkt_ptr++ = (uint8)MAC_DL_GET_UPPER_BYTE(next_expected_tsn);                 \
  *cfg_pkt_ptr++ = (uint8)MAC_DL_GET_LOWER_BYTE(upper_edge);                        \
  *cfg_pkt_ptr++ = (uint8)MAC_DL_GET_UPPER_BYTE(upper_edge);                        \
  *cfg_pkt_ptr++ = (uint8)MAC_DL_GET_LOWER_BYTE(timer_tsn);                         \
  *cfg_pkt_ptr++ = (uint8)MAC_DL_GET_UPPER_BYTE(timer_tsn);                         \
}while(0)
#endif

/*===========================================================================
MACRO: mac_ehs_log_fill_win_info
DESCRIPTION:
  	mac_ehs_window_logging[] = {4,6,8,12,16,24,32,64,128,256}  	
	This Routine computes the index corresponding to window size. For example: 
	for win_size of 4, value of 0 is passed to Diag who will print 4 in 0x4220. 
===========================================================================*/
#define MAC_EHS_LOG_FILL_WIN_INFO(log_win_size,window_index,             \
			mac_ehs_window_logging,cfg_pkt_ptr)              \
do{\
    for(window_index = 0; window_index < 10; window_index++)	    	\
    {\
      if(log_win_size ==  mac_ehs_window_logging[window_index])		\
      {								    	\
        *cfg_pkt_ptr++ = window_index;					\
        break;         							\
      }									\
   }									\
}while(0)

/*===========================================================================

                     TYPE DEFINITIONS FOR MODULE

===========================================================================*/

/*--------------------------------------------------------------------------
 * Data type to store HS timer information
 *-------------------------------------------------------------------------*/
typedef struct
{
   /* Timer value */
   uint16                              release_timer_val;

   /* The TSN associated with this timer.*/
   uint16                              tsn;

   /* Timer block associated with this timer */
   rex_timer_type                      timer_blk;

} mac_hs_timer_info_type;


/*--------------------------------------------------------------------------
 * Data type to store HS window information
 *-------------------------------------------------------------------------*/
typedef struct
{
   /* The lower edge of the window.*/
   uint16                               lower_edge;

   /* The upper edge of the window.*/
   uint16                               upper_edge;

   /* window size as configured by RRC */
   uint8                               window_size;
}mac_hs_window_info_type;


/*--------------------------------------------------------------------------
 * Data type to store mac-d pdu that belongs to a TSN
 *-------------------------------------------------------------------------*/
typedef struct
{
   /* True indicates that this pdu is valid */
   boolean                             valid;

   /* DL RLC logical channel ID */
   rlc_lc_id_type                      rlc_id;

   /* Actual pdu data */
   dsm_item_type                       *pdu_ptr;
}mac_hs_mac_d_pdu_info_type;


/*--------------------------------------------------------------------------
 * Data Type to store the size indicator information for mac-d pdus.
 *-------------------------------------------------------------------------*/
typedef struct
{
   /* Actual value of the Size Indicator.*/
   uint8                               sid_val;

   /* Number of PDUs of the same size */
   uint8                               num_blks;

   /* Pdu size */
   uint16                              pdu_size;
}mac_hs_sid_info_type;

#ifdef FEATURE_WCDMA_HS_FACH 

/* mac hs base timer time in 10 ms*/
  #define MAC_HS_RESET_BASE_TIMER_TIME_IN_MS 10

/* mac hs 10 ms base timer - used for TRESET */
   extern rex_timer_type mac_hs_reset_base_timer;

   extern boolean mac_hs_reset_base_timer_running;

typedef struct
{ 
 /* Indicate timer value in 10 ms */
   uint16 time_in_10_ms;
 /* state variable to show if the treset timer is running for the current queue */
   boolean running;
 /* The count variable dentotes how many 10 ms interval is remaining before the elapse of the timer */
   uint16 count; 

} mac_hs_reset_timer_info_type;
 

#endif

/*--------------------------------------------------------------------------
 * Type to store TSN specific information
 *-------------------------------------------------------------------------*/
typedef struct
{
   /* Sequence Number. */
   uint8                               tsn;

   /* TSN is processed and shipped to RLC. */
   boolean                             processed;

   /* TSN is received and sitting in the re-ordering queue.*/
   boolean                             received;

   /* Number of size indicators received for this TSN */
   uint8                               num_sids;

   /* Data pertaining to the SIDs received for this TSN */
   mac_hs_sid_info_type                sid_data[MAC_HS_DL_MAX_SIDS];

   /* This is an array of size 70 as there can be 70 mac-d pdus per MAC-HS pdu.*/
   mac_hs_mac_d_pdu_info_type          pdu_data[MAC_HS_DL_MAX_MAC_D_PDUS_PER_SUB_FRAME];
}mac_hs_tsn_info_type;

typedef struct
{
  /* Queue ID. */
  uint8                               queue_id;
#ifdef FEATURE_CS_VOICE_OVER_HSPA
  boolean                             opt_timer_enabled;
#endif

  /* Information about the timer associated with this queue */
  mac_hs_timer_info_type              timer_info;

  /* Information about the window associated with this queue */
  mac_hs_window_info_type             window_info;

  /* Number of valid pdu sizes for this queue as configured by RRC. */
  uint8                               num_valid_pdu_sizes;

  /* mac-d pdu size information valid for this queue as configured by RRC. */
  mac_hs_pdu_info_struct_type         valid_pdu_sizes[MAC_HS_DL_MAX_SIDS];

  /* Mac-d flow information mapped to this queue as configured by RRC.*/
  dl_dch_config_info_type             mac_d_flow_info;
  
#if ((defined (FEATURE_HSDPA_MAC_EHS )) && (defined (FEATURE_WCDMA_REL7)))
  /* Mac-enhanced/HSPA+ flow information mapped to this queue as configured by RRC.*/
  dl_dch_config_info_type             mac_enh_flow_info;
#endif

#ifdef FEATURE_WCDMA_HS_FACH
   /* Information about the reset timer assoscciated with this queue*/
   mac_hs_reset_timer_info_type       reset_timer_info;
#endif

} mac_hs_queue_config_info_type;


/*--------------------------------------------------------------------------
 * Information pertaining to a re-ordering queue.
 *-------------------------------------------------------------------------*/
typedef struct
{
   /* Indicate whether this re-ordering entity is enabled or not.*/
   boolean                             enabled;
#ifdef FEATURE_CS_VOICE_OVER_HSPA
   boolean   opt_timer_enabled;
#endif

   /* Queue ID. */
   uint8                               queue_id;

   /* Next Expected TSN  */
   uint16                               next_expected_tsn;

   /* Saved next expected TSNs */
   uint16                               saved_next_expected_tsn;

   /* Number of TSN received in last 10 ms TTI */
   uint8                               curr_num_tsn_rcvd;

   /* List of TSNs received in last 10 ms TTI */
   l1_dec_hs_tsn_info_type            *curr_rcvd_tsn_info[5*MAC_HS_DL_MAX_TSN_PER_10_MS_FRAME];

   /* Number of TSN received in last 10 ms TTI */
   uint8                               new_num_tsn_rcvd;

   /* List of TSNs received in last 10 ms TTI */
   l1_dec_hs_tsn_info_type            *new_rcvd_tsn_info[5*MAC_HS_DL_MAX_TSN_PER_10_MS_FRAME];

   /* Number of TSN received in last 10 ms TTI */
   uint8                               new_num_tsn_rcvd_temp;

   /* List of TSNs received in last 10 ms TTI, temporary array to copy TSN info in one shot in ISR */
   l1_dec_hs_tsn_info_type            *new_rcvd_tsn_temp_info[5*MAC_HS_DL_MAX_TSN_PER_10_MS_FRAME];

   /* Information about the timer associated with this queue */
   mac_hs_timer_info_type              timer_info;

#ifdef FEATURE_WCDMA_HS_FACH
   /* Information about the reset timer assoscciated with this queue*/
   mac_hs_reset_timer_info_type       reset_timer_info;
#endif


   /* Information about the window associated with this queue */
   mac_hs_window_info_type             window_info;

#if ((defined (FEATURE_HSDPA_MAC_EHS )) && (defined (FEATURE_WCDMA_REL7)))
   /* Mac-enhanced/HSPA+ flow information mapped to this queue as configured by RRC.*/
   dl_dch_config_info_type             mac_enh_flow_info;
#endif

   /* Number of valid pdu sizes for this queue as configured by RRC. */
   uint8                               num_valid_pdu_sizes;

   /* mac-d pdu size information valid for this queue as configured by RRC. */
   mac_hs_pdu_info_struct_type         valid_pdu_sizes[MAC_HS_DL_MAX_SIDS];

   /* Mac-d flow information mapped to this queue as configured by RRC.*/
   dl_dch_config_info_type             mac_d_flow_info;

   /* TSN information stored for this queue.*/
   l1_dec_hs_tsn_info_type            *tsn_info[MAC_HS_DL_MAX_WINDOW_SIZE];
} mac_hs_queue_info_type;


/*--------------------------------------------------------------------------
 * Data type to hold all the re-odering queues.
 *-------------------------------------------------------------------------*/
typedef struct
{
   /* Number of queues as configured by RRC */
   uint8                              num_queues;

   /* cctrch_id for this configuration */
   cctrch_id_type                     cctrch_id;

/*---------------------------------------------------------------------------
 *In DL data ISR received TSNs are put into this queue; MAC-hs dequeue
 *TSN items from this queue in task context and put them into reordering buffer.
--------------------------------------------------------------------------*/
#ifdef FEATURE_CS_VOICE_OVER_HSPA
   q_type mac_tsn_q[MAC_HS_DL_MAX_QUEUES];
#endif

   /* queue information (both configuration and data). */
   mac_hs_queue_info_type             *reordering_queue[MAC_HS_DL_MAX_QUEUES];
}mac_hs_reordering_entity_type;

/*--------------------------------------------------------------------------
 * Data type to hold the list of tsn and queue_id received in last 10 ms TTI
 *-------------------------------------------------------------------------*/
typedef struct
{
   /* The queue Id to which this TSN belongs. */
   uint8                              queue_id;

   /* The TSN received in the header information. */
   uint16                              tsn;

   /* cctrch_id for this configuration */
   cctrch_id_type                     cctrch_id;

   /* index into the holding buffer */
   uint8                              saved_tsn_idx;

}mac_hs_tsn_to_queue_mapping_info_type;

/*--------------------------------------------------------------------------
 * Data type to store the operation to be performed during config/reconfig
 *-------------------------------------------------------------------------*/
typedef struct
{
   /* Save non-hs releated parameters if set */
   boolean save_non_hs_params;

   /* Save hs releated parameters if set */
   boolean save_hs_params;

   /* Perform reset */
   boolean perform_hs_reset;

   /* Restore the saved TSNs */
   boolean perform_restore_tsns;

   /* Perform Flush Only*/
   boolean perform_hs_flush_only;
}mac_hs_dl_operation_type;


/*--------------------------------------------------------------------------
 * Data type used to store the necessary information to continue configuration
 * in MAC-HS task context. The configuration from RRC is received in L2 DL
 * Task context.
 *-------------------------------------------------------------------------*/
typedef struct
{
   /* Num of mac_dflows received from RRC */
   uint8    num_dflow;

   /* Action to be performed */
   hsdpa_action_enum_type   action;

   /* Operation to be performed */
   mac_hs_dl_operation_type operation;

   /* List of mac_dflow entities configured */
   mac_dflow_struct_type *hsdpa_info[UE_MAX_MAC_D_FLOW];

#if ((defined (FEATURE_HSDPA_MAC_EHS )) && (defined (FEATURE_WCDMA_REL7)))

   mac_hs_e_type mac_hs_or_ehs;

   /*Informs transition between HS and EHS, if present */
   mac_hs_transition_e_type mac_hs_transition_type;

  /* Number of dedicated RLC logical channels mapped */
   uint8       ndlchan_macehs;

   /* Info of dedicated logical channels mapped */
   mac_dl_ehs_logch_config_type * dl_macehs_logchan_info[UE_MAX_DL_LOGICAL_CHANNEL];
   
   uint8 num_mac_ehs_queue;
   
   mac_ehs_queue_struct_type *hsdpa_ehs_queue_info[UE_MAX_HS_QUEUE];
#endif  /* FEATURE_HSDPA_MAC_EHS  */

#ifdef FEATURE_WCDMA_3C_HSDPA  
   boolean tsn_extn;
#endif

}mac_hs_dl_config_req_type;


typedef struct
{
   /* Queue ID. */
   uint8                               queue_id;

   /* TSN information stored for this queue.*/
   l1_dec_hs_tsn_info_type            *tsn_info;

} mac_hs_saved_tsns_during_reconfig_type;


/*--------------------------------------------------------------------------
 * Enum Type for indicating the various stated MAC-HS can be
 *-------------------------------------------------------------------------*/
typedef enum
{
   MAC_HS_INIT,
   MAC_HS_ACTIVE,
   MAC_HS_INACTIVE,
   MAC_HS_CHECK_RESET,
   MAC_HS_INVALID
}mac_hs_dl_state_type;

#if defined (FEATURE_HSDPA_MAC_EHS) && defined (FEATURE_WCDMA_REL7)
#ifdef FEATURE_MAC_HS_QXDM_LOGGING
/*--------------------------------------------------------------------------
 *  structure types used for logging 0x421E 
 *-------------------------------------------------------------------------*/

typedef struct
{
  uint8 queue_lc_id;
  uint16 pdu_size;
}mac_ehs_complete_pdu_log_info_type;

typedef struct
{
  mcalwcdma_dec_rlc_pdu_segmentation_type segmentation_info;
  uint8 queue_lc_id;
  uint16 pdu_size;
}mac_ehs_partial_pdu_log_info_type;

typedef struct
{
  uint16 tsn;
#ifdef FEATURE_WCDMA_HS_FACH
  uint8 h_rnti;
#endif
  uint8 num_complete_pdus;
  mac_ehs_complete_pdu_log_info_type complete_pdu_info[MAC_EHS_NO_OF_PDU_IN_TSN_LOG];
  uint8 num_partial_pdus;
  mac_ehs_partial_pdu_log_info_type partial_pdu_info[MAC_EHS_NO_OF_PART_PDU_IN_TSN_LOG];
}mac_ehs_log_tsn_buf_type;
#endif /* FEATURE_MAC_HS_QXDM_LOGGING */

/*--------------------------------------------------------------------------
 * Enum Type for indicating whether partial PDU combination is valid or Invalid
 *-------------------------------------------------------------------------*/
typedef enum
{
   PARTIAL_COMBO_INVALID,
   PARTIAL_COMBO_VALID
}partial_valid_type;

extern mac_hs_e_type mac_hs_or_ehs;

#ifdef FEATURE_WCDMA_HS_FACH
extern boolean mac_hs_reset_timer_valid_state;
#endif /* FEATURE_WCDMA_HS_FACH */

#endif

/*===========================================================================

                         GLOBAL FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION: process_dl_hs_dsch_tbs_hdr

DESCRIPTION:
  This function processes the MAC-HS pdu header received from L1.
  This function executes in the ISR context. In addition to validating
  the header information, it fills in the ciphering parameters and
  sets the transport block id for each transport block (mac-d pdu)
  received as part of the transport block set (MAC-HS pdu).

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void process_dl_hs_dsch_tbs_hdr(
   uint8 tbs_id,
   uint8 tsn_to_queue_map_idx,
   l1_dl_tb_set_hdr_struct_type  *tbs_hdr_ptr
   );

/*===========================================================================
FUNCTION: process_dl_hs_dsch_tbs_data

DESCRIPTION:
  This function handles the data received from L1, one MAC-HS pdu at a time.
  This function is executed in the ISR context. If the data is correctly
  received, mac-d pdus are stored locally depending upon the queue to which
  they belong. The window/timer processing then delivers the mac-d pdus to
  RLC appropriately.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void process_dl_hs_dsch_tbs_data(
   uint8 tbs_id,
   uint8 *mac_hs_num_tsn_rcvd,
   l1_dl_tb_set_data_struct_type   *tbsptr
   );

/*===========================================================================
FUNCTION: mac_hs_dl_init

DESCRIPTION:
  This function performs the intialization of some global data and creates
  timers for all the reordering queues. This function is invoked when the
  mac-hs task is created.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void mac_hs_dl_init( void );

/*===========================================================================
FUNCTION: mac_hs_dl_data_handler

DESCRIPTION:
  This function executes in the mac-hs task context once it's signalled
  about the arrival of one or more MAC-HS PDUs from the ISR.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void mac_hs_dl_data_handler( void );

/*===========================================================================
FUNCTION: mac_hs_dl_timer_expiry_handler

DESCRIPTION:
  This function handle the timer expiry event for a given reordering queue.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void mac_hs_dl_timer_expiry_handler(uint8 queue_id);

/*===========================================================================
FUNCTION: mac_hs_dl_process_dsch_config

DESCRIPTION:
  This function handles the HS configuration (re-config) received from the
  RRC in mac-hs task context.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void mac_hs_dl_process_dsch_config(
   uint8 num_mac_d_flow,
   hsdpa_action_enum_type   action,
   mac_dflow_struct_type *hsdpa_info[]
   );

/*===========================================================================
FUNCTION: mac_hs_queue_init

DESCRIPTION:
  This function performs the intialization of some global data and creates
  timers for all the configured reordering queues. This function is invoked after the
  mac-hs allocates the memory to the reordering queue.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/

void mac_hs_queue_init(uint8 queue_id);


/*===========================================================================
FUNCTION: mac_hs_queue_free

DESCRIPTION:
  This function frees the  allocated memory to the reordering queue.


DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void mac_hs_queue_free(uint8 queue_id);


/*===========================================================================
FUNCTION: mac_hs_dl_init_ZI_opt

DESCRIPTION:
  This function is used for dynamic memory allocation to MAC_HS data structures


DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void mac_hs_dl_init_ZI_opt( void );


/*===========================================================================
FUNCTION: mac_hs_dl_free_mem_ZI_opt

DESCRIPTION:
  This function frees the MAC-HS dynamic allocated data structures


DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void mac_hs_dl_free_mem_ZI_opt( void );


#if ((defined (FEATURE_HSDPA_MAC_EHS)) && (defined (FEATURE_WCDMA_REL7)))

/*===========================================================================
FUNCTION: mac_enhs_dl_process_dsch_config

DESCRIPTION:
  This function handles the En-HS configuration (re-config) received from the
  RRC in mac-hs task context.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void mac_enhs_dl_process_dsch_config(void);

#endif

/*===========================================================================
FUNCTION: process_dl_hs_dsch_config

DESCRIPTION:
  This function is called in the L2-DL task context upon receiving HS
  configuration from RRC. This function saves the necessary pointers given
  by RRC in the configuration and signals the MAC-HS task to pick up
  futher processing.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void process_dl_hs_dsch_config(mac_dl_config_type *cfgptr);

/*===========================================================================
FUNCTION: mac_hs_dl_start_dsch_reset_req

DESCRIPTION:
  This function starts the hs reset processing. It delivers all the
  MAC-HS pdus within the window to RLC, stops any active timers, resets the
  window and next_expected_tsn and finally performs a flush.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void mac_hs_dl_start_reset_req( void );

/*===========================================================================
FUNCTION: mac_hs_dl_finish_reset_req

DESCRIPTION:
  This function completed the hs reset processing by sending a reset req
  to RLC with a list of rlc ids.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void mac_hs_dl_finish_reset_req( void );

/*===========================================================================
FUNCTION: mac_hs_dl_validate_config_req

DESCRIPTION:
  This function performs initial validation of HS configuration received
  from RRC. Specifically, the action type and the reset flag indicators are
  examined to see if they result in a correct state transition in MAC-HS.
  This functions tells us, whether we have have to perform any or all of the
  following; 1) perform hs reset; 2) save non-hs data; 3) save hs data.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void mac_hs_dl_validate_config_req(mac_dl_config_type    *cfgptr);


/*===========================================================================
FUNCTION: mac_hs_dl_config_req_handler

DESCRIPTION:
  This function is the called when mac-hs task is signalled from the L2 DL
  task indicating the arrival of a new MAC HS configuration from RRC. The
  processing of MAC HS configuration is handled in the mac-hs task context.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void mac_hs_dl_config_req_handler( void );

#ifdef FEATURE_MAC_HS_QXDM_LOGGING
/*===========================================================================
FUNCTION: mac_hs_dl_send_hs_config_log_packet

DESCRIPTION:
  This function sends the the HS config log packet.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void mac_hs_dl_send_hs_config_log_packet(mac_dl_config_type *cfgptr);

/*===========================================================================
FUNCTION: mac_hs_dl_status_log_timer_expiry_handler

DESCRIPTION:
  This function sends the the HS config log packet.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void mac_hs_dl_status_log_timer_expiry_handler(void);

#endif /* FEATURE_MAC_HS_QXDM_LOGGING  */

extern void mac_hs_dl_reset_local_config_data(void);
#if ((defined (FEATURE_HSDPA_MAC_EHS )) && (defined (FEATURE_WCDMA_REL7)))
/*===========================================================================
FUNCTION: mac_enhs_dl_reset_config_info

DESCRIPTION:
  This is a local function to reset the global data structure that saves
  some of the details of the configuration received from RRC. This function
  is used during power up initialization and after the config/re-config has
  finished.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/

void mac_enhs_dl_reset_local_config_data(void);
#endif 


/*===========================================================================
FUNCTION: process_dl_hs_enh_dsch_tbs_data

DESCRIPTION:
  This function handles the data received from L1, one MAC-HS TSN at a time.
  This function is executed in the ISR context. If the data is correctly
  received, TSN will be stored locally depending upon the queue to which
  they belong. If the TSN is already present and validation failed, PDUs present
  in this TSN will be freed and TSN will be added into TSN Free queue.
  The window/timer processing then delivers the TSN to RLC appropriately.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/

void process_dl_hs_enh_dsch_tbs_data(uint8 *mac_hs_num_tsn_rcvd);


#if ((defined (FEATURE_HSDPA_MAC_EHS)) && (defined (FEATURE_WCDMA_REL7)))
/*===========================================================================
FUNCTION: process_dl_ehs_dsch_tbs_hdr

DESCRIPTION:
  This function processes the MAC-HS pdu header received from L1.
  This function executes in the ISR context. In addition to validating
  the header information, it fills in the ciphering parameters and
  sets the transport block id for each transport block (mac-d pdu)
  received as part of the transport block set (MAC-HS pdu).

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void process_dl_ehs_dsch_tbs_hdr(
   uint8 tbs_id,
   uint8 *tsn_to_queue_map_idx,
   l1_dl_tb_set_hdr_struct_type *tbs_hdr_ptr
   );

/*===========================================================================
FUNCTION: mac_ehs_provide_partial_to_complete_info

DESCRIPTION:
  Prepare the partial-to-complete PDUs possible from every queue. Max number of 
  partial-to-complete are limited by the allowed_length in total of the PDU
  sizes.
  MAC also provides the ciphering parameters for each Partial-to-Complete
  PDU provided.
  
DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void mac_ehs_provide_partial_to_complete_info(uint16 allowed_length,
            mcalwcdma_dec_rlc_pdu_concatenate_info_struct_type *rlc_p_to_c_ptr);

/*===========================================================================
FUNCTION: mac_ehs_dl_get_rlc_id

DESCRIPTION:
  This function retrieves the rlc_id(internal value) from mac_id (1...15 from OTA)

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/

uint8 mac_ehs_dl_get_rlc_id(uint8      mac_id);

/*===========================================================================
FUNCTION: mac_ehs_discard_partial_update

DESCRIPTION:

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void mac_ehs_discard_partial_update(l1_dec_hs_tsn_info_type *tsn_info_ptr, 
                                   boolean first_discard, boolean second_discard);
/*===========================================================================
FUNCTION: mac_ehs_discard_partial_upto_comp_tsn

DESCRIPTION:
  This function is called, if L1 missed the DM activity (partial to complete)
  and ignores the request further. Next batch of DM response came. 
  Work around is - to discard the partials in TSN upto the received batch first
  TSN and pass them to RLC.
  
DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  As we are discarding valid PARTIALS and passing the TSN to RLC, RLC might 
  experience some holes in the receive window. This might cause, RLC to wait
  minimum one RTT for retransmission. In Very high data rates, if this happens
  quite often, we might see RLC level retransmission requests often causing 
  RLC Window Stall conditions in DL (In effect, peer NW might not schedule
  as not seeing the RLC ACK moving quickly and window is reached)
===========================================================================*/

void mac_ehs_discard_partial_upto_comp_tsn(q_type *mac_partial_tsn_q_ptr, 
                                           l1_dec_hs_tsn_info_type *complete_tsn_ptr);
/*===========================================================================
FUNCTION: mac_ehs_update_partial_first_info

DESCRIPTION:
  Update the Header info and remember the TSN Info from Partial Start

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/

void mac_ehs_update_partial_first_info(
             mcalwcdma_dec_partial_rlc_pdu_hdr_struct_type *rlc_pdu_hdr_info_ptr,
             rlc_dl_tb_hdr_info_type *rlc_hdr_info_ptr, 
             partial_pdu_type *partial_pdu_ptr,
             l1_dec_hs_tsn_info_type *temp_tsn_ptr);

#ifdef FEATURE_WCDMA_HS_FACH
/*===========================================================================
FUNCTION: mac_ehs_update_local_tb_header_info

DESCRIPTION:
  Update the local tb header information upon parsing through each partials

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void mac_ehs_update_local_tb_header_info
(
  rlc_dl_tb_hdr_info_type *rlc_hdr_info_ptr, 
  partial_pdu_type *partial_pdu_ptr, 
  uint16 *bytes_copied_to_tb_header_ptr, 
  uint16 *rem_bytes_in_tb_header_ptr
);

#endif

/*===========================================================================
FUNCTION: mac_ehs_update_queue_due_to_invalid_combo

DESCRIPTION:
  Find the previous PARTIAL_FIRST before the clear_upto_tsn_ptr. Clear from that
  TSN onwards till and including the clear_upto_tsn_ptr

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void mac_ehs_update_queue_due_to_invalid_combo(uint8 queue_id, 
                                              l1_dec_hs_tsn_info_type *clear_upto_tsn_ptr);

/*===========================================================================
FUNCTION: mac_hs_dl_pass_tsn_pdus_to_partial_tsn_q

DESCRIPTION:
  Place the TSN in partial_tsn_q
  
DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
boolean mac_ehs_dl_pass_tsn_pdus_to_partial_tsn_q(l1_dec_hs_tsn_info_type *tsn_info_ptr);


#endif /* ((defined (FEATURE_HSDPA_MAC_EHS)) && (defined (FEATURE_WCDMA_REL7))) */

/*===========================================================================
FUNCTION: mac_hs_dl_discard_tsn

DESCRIPTION:
  This function discards a MAC-HS tsn. This includes freeing the dsm items
  for all the mac-d pdus forming the mac-hs pdu.

  Decrement the app_field part of the DSM item shown by iovec.
  Release the DSM Item, if the app_field is zero. Assign null to iovec.ptr
  Invalidate the TSN contents

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void mac_hs_dl_discard_tsn(l1_dec_hs_tsn_info_type *int_tsn_ptr);

/*===========================================================================
FUNCTION: mac_hs_discard_dsm_item

DESCRIPTION:
  This function discards a MAC-HS pdu. It means, the app_field of the dsm item
  will be decremented. If the app_field count reaches zero, dsm item will be freed

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/

void mac_hs_discard_dsm_item(dsm_item_type **dsm_ptr);


/*===========================================================================
FUNCTION: mac_hs_tsn_empty_recovery

DESCRIPTION:
This function will flush the all MAC-hs/MAC-ehs re-ordering queue

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/

void mac_hs_tsn_empty_recovery( void );


#if ((defined (FEATURE_HSDPA_MAC_EHS)) && (defined (FEATURE_WCDMA_REL7)))
void mac_ehs_dl_send_hs_config_log_packet(mac_dl_config_type *cfgptr);

/*===========================================================================
FUNCTION: mac_ehs_pass_comp_pdu_to_rlc

DESCRIPTION:
  For every complete PDU TSN present in queue, It will pass all the TSN from 
  Partial Start to Partial Last to RLC. All the TSN following Partial Last
  which doesnt have any Partials also will be delivered to RLC.
  
DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void mac_ehs_pass_comp_pdu_to_rlc(void);

#endif /* ((defined (FEATURE_HSDPA_MAC_EHS)) && (defined (FEATURE_WCDMA_REL7))) */

#ifdef FEATURE_WCDMA_HS_FACH
/*===========================================================================
FUNCTION: mac_hs_reset_base_timer_handler

DESCRIPTION:
  handler function for mac hs reset base timer
  
DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void mac_hs_reset_base_timer_handler(void);

/*===========================================================================
FUNCTION: mac_hs_reset_the_reordering_queue

DESCRIPTION:
  Flushes and resets a single HS Queue
  
DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/

void mac_hs_reset_the_reordering_queue (mac_hs_queue_info_type* int_queue_ptr);


/*===========================================================================
FUNCTION: mac_hs_reset_base_timer_cb

DESCRIPTION:
  call back function for the mac reset base timer.
  
DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void mac_hs_reset_base_timer_cb(unsigned long cb_param);

/*===========================================================================
FUNCTION: process_dl_hs_enh_dsch_tm_data

DESCRIPTION:
  Function to handle TM mode BCCH/PCCH EFACH pdus
  
DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void process_dl_hs_enh_dsch_tm_data(l1_dec_hs_tsn_info_type *tsn_info_ptr);

#endif

#ifdef TEST_FRAMEWORK
#error code not present
#endif

#if !(defined FEATURE_WCDMA_HS_FACH)

#define MAC_HS_FACH_OR_PCH_STATE()  (FALSE)

#endif

/*===========================================================================
 FUNCTION     WCDMA_MAC_HS_DL_INIT_TCB

 DESCRIPTION
   This function populates the cached WCDMA L2 DL TCB pointer through rcinit. 

 DEPENDENCIES
   None.

 PARAMETERS
   None.

 RETURN VALUE
   None.

 SIDE EFFECTS
   An assertion will fail if the lookup fails.
===========================================================================*/
extern void wcdma_mac_hs_dl_init_tcb(void);

/*===========================================================================
 FUNCTION     WCDMA_MAC_HS_DL_GET_TCB

 DESCRIPTION
   This function returns a pointer to the WCDMA L1 TCB. 

 DEPENDENCIES
   None.

 PARAMETERS
   None.

 RETURN VALUE
   A pointer to the WCDMA MAC HS DL TCB.

 SIDE EFFECTS
   An assertion will fail if the lookup fails.
===========================================================================*/
extern rex_tcb_type* wcdma_mac_hs_dl_get_tcb(void);

/*===========================================================================
 FUNCTION     MAC_DL_HS_CHECK_PENDING_TSNS

 DESCRIPTION
   This function checks all the configured 
   HS queued id if there are any 
   1) Pending TSN to be received based upon T1 timer. 
   2) Pending partials in partial queue.

 DEPENDENCIES
   None.

 PARAMETERS
   None.

 RETURN VALUE
   TRUE  - if any pending DL data to be received/processed.
   FALSE - otherwise.

 SIDE EFFECTS
   None
===========================================================================*/
boolean mac_dl_hs_check_pending_tsns(void);

#endif /* MAC_HS_DL_H */
