#ifndef TDSRRCCOMMON_H
#define TDSRRCCOMMON_H
/*===========================================================================
                                    R R C   C O M M O N   H E A D E R

DESCRIPTION

  This module contains the internal header information for the RRC
  Common module

Copyright (c) 2010-2012 Qualcomm Technologies, Incorporated. All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/rrc/inc/tdsrrccommon.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/02/14  sarao   Removed the inclusion of tmc.h as its being discontinued in core.
10/20/11   dhf    Clean up redundant include files
08/25/11   dhf    Split from tdsrrcdata.h
===========================================================================*/


/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */  
#include "tdscdma_variation.h"
#include "tdsrrcasn1.h"

/*for tmc_heap on unit test*/
/*#if defined(TEST_FRAMEWORK)
#include "tmc.h"
#endif */

#include "log_codes_tdscdma.h"
   
#ifdef FEATURE_MVS
#include <mvsamr.h>
#endif /* FEATURE_MVS */
    
#include <mvs.h>
#include "modem_mem.h"
#include "rrcmmif.h"  
#include "stringl.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* This macros are meant for default configuration types that can received by UE 
through SIB16/Intersystem HO command to UTRAN */

#define TDSRRC_DEF_CONFIG_0  0
#define TDSRRC_DEF_CONFIG_1  1
#define TDSRRC_DEF_CONFIG_3  3
#define TDSRRC_DEF_CONFIG_4  4
#define TDSRRC_DEF_CONFIG_6  6
#define TDSRRC_DEF_CONFIG_7  7
#define TDSRRC_DEF_CONFIG_8  8
#define TDSRRC_DEF_CONFIG_9  9
#define TDSRRC_DEF_CONFIG_10 10

#ifdef FEATURE_TDSCDMA_SRB5_SUPPORT
#define TDSRRC_DEF_CONFIG_11 11
#define TDSRRC_DEF_CONFIG_12 12
#define TDSRRC_DEF_CONFIG_13 13

#define  TDSRRC_DEF_CONFIG_14 14

#endif /* FEATURE_TDSCDMA_SRB5_SUPPORT */


#define TDSRRC_DEF_CONFIG_15  15
#define  TDSRRC_DEF_CONFIG_16 16

#define TDSRRC_DEF_CONFIG_18  18
#define TDSRRC_DEF_CONFIG_19  19
#define TDSRRC_DEF_CONFIG_20  20
#define  TDSRRC_DEF_CONFIG_21 21
#define  TDSRRC_DEF_CONFIG_22 22
#define  TDSRRC_DEF_CONFIG_23 23


#define TDSRRC_DEF_CONFIG_INVALID 0xFF

#ifdef FEATURE_TDSCDMA_DSDS
#define TDSRRCTMR_WRM_LOCK_WAIT_TIMER_FOR_INTERRAT_IN_MS 10000
#endif

/* Followig two definitions map to the OSS error codes defined in 
asn1code.h file */
#define TDSASN1_CONSTRAINT_VIOLATED        17 /* constraint violation error occured */
#define TDSASN1_OUT_MEMORY                  8 /* memory-allocation error */

#define TDSRRC_TOT_SCN 128 /* Total number of state change notifications
                       supported my the module. This may be changed
                       as desired to support all RRC procedures. */


/*MAcro which indicates one time RACH attempt for a RRC message, 
 *This is needed for RACH transmission statistics to evaluate performance
 *in TCXO Shutdown */
#define TDSRRC_RACH_TX_STATS_ONE_TIME_RACH 1

/* The following define SIB bit masks */
#define TDSMIB_BIT           0x00000001
#define TDSSIB_1_BIT         0x00000002
#define TDSSIB_2_BIT         0x00000004
#define TDSSIB_3_BIT         0x00000008
#define TDSSIB_4_BIT         0x00000010
#define TDSSIB_5_BIT         0x00000020
#define TDSSIB_6_BIT         0x00000040
#define TDSSIB_7_BIT         0x00000080
#define TDSSIB_11_BIT        0x00000100
#define TDSSIB_12_BIT        0x00000200
#define TDSSIB_18_BIT        0x00000400
#define TDSSB_1_BIT          0x00000800
#define TDSSB_2_BIT          0x00001000
#define TDSSIB_5bis_BIT      0x00002000
#define TDSSIB_11BIS_BIT     0x00004000
#define TDSSIB_19_BIT        0x00008000

/* Invalid Cell Id for Inter-RAT Measurements */
#define TDSINVALID_CELL_ID   250

/*Invalid Cell Id in OTA MSG*/
#define TDSINVALID_CELL_ID_OTA 0xFFFFFFFF

/* Set num SDU to be purged to 0xFF, 
 *RLC will delete all the SDU which are in water mark 
 */
#define  TDSMAX_SRB2_SDUS_TOBE_PURGED 0xFF

/* Defines for purging WM */
#define  TDSMAX_RB0_SDUS_TOBE_PURGED 4
#define  TDSMAX_SRB1_SDUS_TOBE_PURGED 15


#define TDSRRC_DEF_CONFIG_17  17


/* Total number of SIBs (Including the MIB, Scheduling Blocks and 3 spares). 
This is as per the June '01 version of the RRC specification.
See the definition tdsrrc_SIB_type in tdsrrcsibproci.h for details. */
#define TDSRRC_TOT_SIBS 16

/*If the NV is 3 or above, then UE supports Rel7*/
#define TDSRRC_NV_REL_INDICATOR_MINIMUM_REL7 3

/* only for PFR use begin*/
#define TDSRRC_PFR_FREQUENCY_RANGE1_BASE 9400
#define TDSRRC_PFR_FREQUENCY_RANGE2_BASE 9500
#define TDSRRC_PFR_FREQUENCY_RANGE3_BASE 10050
/* only for PFR use end*/

/*===========================================================================
**                  ENUMERATED TYPES
**=========================================================================*/

typedef enum {
  TDSRRC_COMPLETE_CONFIG = 0,
  TDSRRC_PREDEF_CONFIG   = 1,
  TDSRRC_DEFAULT_CONFIG  = 2,
  TDSRRC_INVALID_CONFIG  = 3 /* Internal use only */
} tdsrrc_config_e_type;

/* Indicates whether we need to update UE Conn mode timers and consts */
typedef enum 
{
  TDSRRC_CONN_TIMER_AND_CONSTANTS_UPDATE_NOT_REQ,
  TDSRRC_CONN_TIMER_AND_CONSTANTS_UPDATE_REQ
} tdsrrc_conn_timer_constants_update_status_e_type; 

typedef enum
{
  TDSRRC_CSP_INTERRAT_NONE,
  TDSRRC_CSP_INTERRAT_GSM,
  TDSRRC_CSP_INTERRAT_LTE,
  TDSRRC_CSP_INTERRAT_LTE_CGI,
  TDSRRC_CSP_INTERRAT_MAX
} tdsrrc_csp_interrat_rat_e_type;

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* This is used between CSP,MEAS & CU functions for decoding SIB bit mask in 
     SIB_CHANGE_IND sent by sib*/
typedef  uint32  tdsrrc_sib_change_mask_type;

/* This is frequency representation in CSP */
typedef  uint16  tdsrrc_freq_type;

/* This is Scrambling code representation in CSP */
typedef  uint8  tdsrrc_cpid_type;



typedef struct{
  int32  decode_status;    /* Decoding failure cause       */
  int32  pdu_num;          /* pdu on which decoding failed */
  int32  pdu_size;         /* size of the encoded string   */
  void  *encoded_pdu_ptr;  /* ptr to encoded string        */
  int32  decode_length;    /* Decoded length incase pre-allocated
                              decode buffer is used */
} tdsrrc_decode_info_type;


/* Following two structures are used to get the ASN.1 decoding/encoding status when
there is any decoding or encoding failure */
typedef struct{
  int32  encode_status;      /* Encoding failure cause      */ 
  int32  pdu_num;            /*pdu on which encoding failed */
  void   *unencoded_pdu_ptr; /*un-encoded pdu ptr           */
} tdsrrc_encode_info_type;


/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

extern boolean tdssib_change_safe_guard;

extern boolean tdsrrc_protocol_error_reject;/* Indicates whether a severe 
                                            protocol error */
extern rrc_rel_cause_e_type tdsrrc_release_cause;

/* Indicates if valid TMSI is present */
extern boolean                     tdsrrc_tmsi_valid;
/* Indicates if valid PTMSI is present */
extern boolean                     tdsrrc_ptmsi_valid;
/* Indicates if valid IMSI is present */
extern boolean                     tdsrrc_imsi_valid;

extern rrc_tmsi_gsm_map_type       tdsrrc_tmsi;

extern rrc_ptmsi_gsm_map_type      tdsrrc_ptmsi;

extern rrc_imsi_gsm_map_type       tdsrrc_imsi;

/* stores the access classes for the UE */
extern rrc_lo_access_class_e_type  tdsrrc_lo_access_class;
extern rrc_hi_access_class_type    tdsrrc_hi_access_class;

/* stores the admin data received in SIM INSERTED REQ */
extern byte                        tdsrrc_rcvd_admin_data;

/*--------------------------------------------------------*/
/*                                                        */
/*      EXTERNALIZED RRC COMMAND QUEUES                   */
/*                                                        */
/*  All Command Queues for the RRC Task are defined here  */
/*--------------------------------------------------------*/


/* =======================================================================
**                          Macro Definitions
** ======================================================================= */
/* RRC Statistics */

#define TDSRRC_DEBUG_INCREMENT_STATS(field)    tdsrrc_debug_stats(field)
    
#define TDSRRC_DEBUG_CLEAR_ALL_STATS()    tdsrrc_debug_clear_stats()

/*===========================================================================

FUNCTION RRC_FREE

DESCRIPTION
  This function is called by the RRC Modules in order to free dynamic
  memory. This function simply calls mem_free by selecting proper heap.

DEPENDENCIES
  The memory manager must have been previously initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
/*lint -save -e683 */

#define tdsrrc_free(ptr)\
  {\
     modem_mem_free(ptr, MODEM_MEM_CLIENT_TDSCDMA_RRC_CMD_CRIT);\
     ptr = NULL;\
  }

/*lint -restore */

/*===========================================================================

FUNCTION RRC_MALLOC

DESCRIPTION
  This function is called by the RRC modules in order to allocate 
  dynamic memory. This function simply calls mem_malloc and selects suitable
  heap and returs pointer to the allocated memory.

DEPENDENCIES
  The memory manager must have been previously initialized.

RETURN VALUE
  A pointer to the allocated memory if the memory was successfully allocated,
  otherwise NULL.

SIDE EFFECTS
  None.

===========================================================================*/
#ifdef __GNUC__
static inline void * tdsrrc_malloc(size_t size)
#else
__inline void * tdsrrc_malloc(size_t size)
#endif
{
  void * return_ptr =  NULL;

  return_ptr = 

     modem_mem_calloc(1,size,
        MODEM_MEM_CLIENT_TDSCDMA_RRC_CMD_CRIT);


  if(return_ptr == NULL)
  {
    ERR_FATAL("Failed to allocate memory for size 0x%x",size,0,0);
  }

  return(return_ptr);
}

/*===========================================================================
MACRO  TDSVALID_RRC_STATE

DESCRIPTION
  Verify that passed value is a valid RRC state. "TDSRRC_STATE_WILDCARD"
  is not considered valid for this macro.

PARAMETERS
  state  - member of tdsrrc_state_e_type 

DEPENDENCIES
  None

SIDE EFFECTS
  None

===========================================================================*/
#define TDSVALID_RRC_STATE(state) \
        ( (state == TDSRRC_STATE_DISCONNECTED) || \
          (state == TDSRRC_STATE_CONNECTING)   || \
          (state == TDSRRC_STATE_CELL_FACH)    || \
          (state == TDSRRC_STATE_CELL_DCH)     || \
          (state == TDSRRC_STATE_CELL_PCH)     || \
          (state == TDSRRC_STATE_URA_PCH) )

/*===========================================================================

FUNCTION tdsrrc_free_for_external_cmd

DESCRIPTION
  This function is called by the RRC Modules in order to free dynamic
  memory for external cmds. 
  This function simply calls mem_free by selecting proper heap.

DEPENDENCIES
  The memory manager must have been previously initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
/*lint -save -e683 */

#define tdsrrc_free_for_external_cmd(ptr)\
     modem_mem_free(ptr, MODEM_MEM_CLIENT_TDSCDMA_RRC_CMD_CRIT)
/*lint -restore */

#define TDSRRC_PRINT_SIB_HEAP()       



/* =======================================================================
**                        Function Declarations
** ======================================================================= */
/*===========================================================================

FUNCTION        tdsrrc_validate_tds_frequency()

DESCRIPTION     This function validates the UARFCN values in the provided
                rrc_FrequencyFDD IE according to table 5.2 in 25.101.
  
RETURN VALUE    Returns FAILURE if either UARFCN is invalid.
 
SIDE EFFECTS    None

===========================================================================*/
uecomdef_status_e_type tdsrrc_validate_tds_frequency
(    
  tdsrrc_UARFCN             uarfcn_Nt
);



/*===========================================================================

FUNCTION        tdsrrc_validate_dl_arfcn()

DESCRIPTION     This function validates the DL UARFCN values.
  
RETURN VALUE    FAILURE if UARFCN is invalid,
                SUCCESS otherwise
 
SIDE EFFECTS    None

===========================================================================*/
uecomdef_status_e_type tdsrrc_validate_dl_arfcn
(    
  tdsrrc_freq_type            uarfcn_dl
);


/*===========================================================================

FUNCTION VALIDATE_FREQUENCY_INFO

DESCRIPTION

  This function validates the UARFCN values in the provided Frequency Info
  IE according to table 5.2 in 25.101.
  
DEPENDENCIES


RETURN VALUE
  Returns FAILURE if either UARFCN is invalid.
 

SIDE EFFECTS

  None

===========================================================================*/
uecomdef_status_e_type tdsrrc_validate_frequency_info
(    
  tdsrrc_FrequencyInfo *ie_ptr
);

/*===========================================================================

FUNCTION        IS_FREQ_IN_OVERLAPPING_850_AND_800_BANDS

DESCRIPTION     This function determines if the UARFCN passed to it is in 
                the overlapping region of 850 and 800 bands or not.


RETURN VALUE    TRUE:  UARFCN is in the overlapping region
                FALSE: Otherwise

SIDE EFFECTS    

===========================================================================*/
boolean tdsrrc_is_freq_in_overlapping_850_and_800_bands(uint32 freq);

/*===========================================================================

FUNCTION tdsrrc_print_plmn_id

DESCRIPTION           Prints PLMN I.D
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void   tdsrrc_print_plmn_id
(
  rrc_plmn_identity_type plmn_id
);
#endif
