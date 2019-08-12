#ifndef RRCLCM_H
#define RRCLCM_H
/*===========================================================================
              R R C L C M  H E A D E R  F I L E

DESCRIPTION
  This file contains data types and declarations associated with the
  L2 ACK manager and RRC Logical Channel Id manager
  
Copyright (c) 2000-2004, 2006-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrclcm.h_v   1.15   12 Jun 2002 06:53:52   rjeenaga  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/inc/rrclcm.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/13/13   gv      Do not allow TA if l2 is waiting for ACK from network for its internal data
05/23/13   as      Made changes to consider the successful decode of DDT as implicit L2 ACK
                   for Security mode complete
10/18/12   gv      Replace FEATURE_DUAL_SIM_WCDMA_TUNEAWAY with FEATURE_DUAL_SIM
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
02/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.50
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
10/28/10   as      Added changes to check for DL ACK for Tuneaway
05/08/09   ss      Updated Copyright Information
01/05/09   ps      Made changes to call dsm_queue_init for watermark queues
                   only at RRC initialisation stage
03/06/08   da      Changed FEATURE_MBMS to FEATURE_MODEM_MBMS
12/27/07   rm      Added prototype for rrclcm_get_nchan_dl_tm
12/21/07   da      Support for FEATURE_MBMS
08/21/06   tkk     Added SRB#5 support.
04/14/06   vr      Changed prototype of rrclcm_post_event_for_dl_l2ack_ind
                   to support failure reason in L2 Ack Ind from RLC
12/29/03   vsk     Handle the change in RLC callback
01/15/03   sk      Added parenthesis around the expression like macros.
                   (Detected the error by linting)
06/11/02   rj      Updated interfaces for rrclcm_get_mui and rrclcm_identify_proc
                   to address continuity for MUIs.
05/07/02   rj      Added additional formal parameter for the
                   rrclcm_post_event_for_dl_sdu function to dequeue multiple 
                   SDUs if RLC enqueues before calling callback function.
12/04/01   rj      Added prototypes for new interface functions 
                   rrclcm_get_ul_rlc_mode and rrclcm_get_dl_rlc_mode to 
                   return the mode of allocated RLC Logical Channel Id.
08/30/01   rj      Added prototype for new interface functions 
                   rrclcm_get_ul_rlc_lc_id and rrclcm_get_dl_rlc_lc_id for
                   RRCRBE module to get RLC ID for a particular Radio Bearer Id.
06/14/01   vn      Added prototype for rrclcm_clear_rlc_lc_info.
06/11/01   rj      Fixed a typo.
06/08/01   rj      Modified interfaces to callback function 
                   rrclcm_post_event_for_dl_l2ack_ind to match the RLC 
                   declarations.
05/23/01   rj      Modified rrclcm_deallocate_rlc_lc_id function into 
                   rrclcm_deallocate_ul_rlc_lc_id and 
                   rrclcm_deallocate_dl_rlc_lc_id to de-allocate RLC Logical
                   channel IDs. This functions take RLC Logical Channel ID
                   as formal paramter instead of Radio Bearer ID.
05/11/01   rj      Added a new function rrclcm_check_dl_rlc_lc_id 
                   to check whether RLC Logical Channel Id is allocated for 
                   requested RLC Service entity or not.
02/01/01   rj/ram  The RLC Define constants are replaced with uecomdef define
                   constants.
01/30/01   rj      rrclcm_lc_id_type, rrclcm_channel_type_e_type  and 
                   rlclcm_mode_e_type are replaced with rlc_lc_id_type,
                   uecomdef_logchan_e_type and uecomdef_logch_mode_e_type                  
                   repectively from uecomdef.h. 
                   Deleted typedefs for  rrclcm_lc_id_type, 
                   rrclcm_channel_type_e_type and
                   rlclcm_mode_e_type from this file.
01/25/01   rj      Moved Radio Bearer define constants to uecomdef.h
12/22/00   rj      Modified interface for rrclcm_check_ul_rlc_lc_id. Removed
                   Watermark ptr from the formal parameters of this function.
12/13/00   rj      Added rrclcm_get_ul_rb_id, rrclcm_get_dl_rb_id,
                   rrclcm_get_ul_watermark_ptr,  and rrclcm_get_dl_watermark_ptr
                   and  deleted rrclcm_get_watermark_ptr and rrclcm_get_rb_id.
12/07/00   rj      Updated functions rrclcm_post_event_for_dl_l2ack_ind,
                   rrclcm_post_event_for_dl_sdu, and rrclcm_identify_proc to
                   work with unified RRC command type.
                   rrc_downlink_sdu_ind_type and rrc_downlink_l2ack_ind_type
                   are moved to rrcintif.h.
11/30/00   rj      Changed filename from rrclcp.h to rrclcm.h and implemented 
                   REX++ updates. Moved rrc_proc_e_type to rrcdata.h file.
11/20/00   rj      Editorial corrections. changed rrclcm_status_type to
                   rrclcm_status_e_type
11/19/00   rj      Updates for the code review comments.
                   Added a new function prototype rrclcm_get_rb_id.
                   Implementation for L2 ACK management is changed to take 
                   care of multiple instances of same RRC procedure.
11/13/00   rj      modified interfaces to functions 
                   rrclcm_allocate_ul_rlc_lc_id, rrclcm_allocate_dl_rlc_lc_id
                   rrclcm_check_ul_rlc_lc_id.
                   Added a new function rrclcm_get_watermark_ptr.
10/30/00   rj      corrected typos.
10/29/00   rj      editorial corrections for compilations errors.
10/25/00   rj      Created file.

===========================================================================*/

/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/

/*---------------------------------------------------------------------------
** Include Files
**-------------------------------------------------------------------------*/
#include "wcdma_variation.h"
#include "comdef.h"

#include "dsm.h"
#include "dsmutil.h"

#include "queue.h"
#include "rex.h"

#include "rrcasn1.h"
#include "rrcintif.h"
#include "rrcdata_v.h"
#include "rrccmd_v.h"
#include "uecomdef.h"


/*===========================================================================
**                  CONSTANT/DEFINE DECLARATIONS
**=========================================================================*/

#define RRCLCM_DL_DCCH_SRB5_SDU_TYPE 0xEF

#define RRCLCM_INVALID_SDU_TYPE    0xFF     /* Invalid SDU type. This same
                                               as PDU type in ASN1 */
#define RRCLCM_RLC_LC_ID_ERROR     0xFF     /* This identifies the RLC 
                                               Logical Channel allocation is
                                               failed */
                               
#define RRCLCM_RLC_LC_ID_NOT_FOUND 0xFF     /* This identifies the particular
                                               requested RLC Id is not 
                                               found. */
#define RRCLCM_RB_ID_NOT_ALLOCATED 0xFF     /* This identifies that particular 
                                               RB Id is not allocated */

#define RRCLCM_WATERMARK_SIZE      0x1FF    /* This size is used to set 
                                               hi_watermark, low_watermark and
                                               dont_exceed_cnt fields. This 
                                               needs adjusted during the
                                               testing. */
#define RRCLCM_UL_TM_START_IDX      0       /* Uplink Starting index for 
                                               Transparent mode Logical 
                                               Channel Id */
#define RRCLCM_UL_UM_START_IDX UE_MAX_UL_TM_CHANNEL
                                            /* Uplink Starting index for 
                                               Unacknowledged mode Logical 
                                               Channel Id */
#define RRCLCM_UL_AM_DATA_START_IDX (UE_MAX_UL_TM_CHANNEL + \
                                    UE_MAX_UL_UM_CHANNEL)
                                            /* Uplink Starting index for 
                                               Acknowledged mode Logical 
                                               Channel Id for Data. */
#define RRCLCM_UL_AM_CONTROL_START_IDX (UE_MAX_UL_TM_CHANNEL + \
                                       UE_MAX_UL_UM_CHANNEL + \
                                       UE_MAX_UL_AM_DATA_CHANNEL)
                                            /* Uplink Starting index for 
                                               Acknowledged mode Logical
                                               Channel Id for Control. */
#define RRCLCM_DL_TM_START_IDX      0       /* Downlink Starting index for 
                                               Transparent mode Logical 
                                               Channel Id */
#define RRCLCM_DL_UM_START_IDX UE_MAX_DL_TM_CHANNEL
                                            /* Downlink Starting index for 
                                               Unacknowledged mode Logical 
                                               Channel Id */
#define RRCLCM_DL_AM_DATA_START_IDX (UE_MAX_DL_TM_CHANNEL + \
                                    UE_MAX_DL_UM_CHANNEL)
                                            /* Downlink Starting index for
                                               Acknowledged mode Logical
                                               Channel Id. for Data */
#define RRCLCM_DL_AM_CONTROL_START_IDX (UE_MAX_DL_TM_CHANNEL + \
                                       UE_MAX_DL_UM_CHANNEL + \
                                       UE_MAX_DL_AM_DATA_CHANNEL)
                                            /* Downlink Starting index for
                                               Acknowledged mode Logical 
                                               Channel Id. for control */
/*===========================================================================
**                  ENUMERATED TYPES
**=========================================================================*/

/*--------------------------------------------------------------------------- 
  Type for defining the status of a logical channel entity.  
---------------------------------------------------------------------------*/
typedef enum 
{
  RRCLCM_LC_NOT_IN_USE = 0,                /* This identifies that the 
                                              particular RLC Logical Channel
                                              entity is not allocated and 
                                              not in use. */
  RRCLCM_LC_IN_USE                         /* This identifies that the 
                                              particular RLC Logical Channel
                                              entity is allocated and in 
                                              use. */ 
} rrclcm_lc_status_e_type; 


 
/*===========================================================================
**                      TYPE DECLARATIONS
**=========================================================================*/

/*---------------------------------------------------------------------------
** Type for RLC Logical Channel Identifer. 
---------------------------------------------------------------------------*/

typedef uint8 rrclcm_mui_type;             /* Message Unit Id type for SDU
                                              to transmit in Acknowledged 
                                              mode. */
typedef enum 
{
  RRCLCM_SUCCESS,                          /* Indicates successful status.*/

  RRCLCM_FAILURE                           /* Indicates Failure status. */

} rrclcm_status_e_type ;    

typedef struct
{
  rrc_proc_e_type rrc_proc;                /* Unique identifier for RRC
                                              procedure. */
  rrclcm_mui_type mui;                     /* Unique Message Unit Id for 
                                              L2 ACK.*/
} rrclcm_l2_ack_info_type;
                                            
/*---------------------------------------------------------------------------
  Type for storing the Uplink RLC Logical Channel IDs infomation.
---------------------------------------------------------------------------*/
typedef struct 
{
  rlc_lc_id_type lc_id;                    /* Unique identifier for the Uplink 
                                              RLC logical channel.*/
  rrclcm_lc_status_e_type status;          /* Status of RLC Logical Channel 
                                              Indentifier entity. */
  uecomdef_logch_mode_e_type  mode;        /* Identifies mode of transmission 
                                              of RLC Logical Channel entity.*/
  uecomdef_logchan_e_type lc_type;         /* Type of Logical Channel. */
  
  rrc_RB_Identity rb_id;                   /* Unique Identifier for Radio 
                                              bearer.  */
  dsm_watermark_type *tx_queue;            /* Transmit data queue. */
                                          
} rrclcm_ul_rlc_lc_info_type;


/*---------------------------------------------------------------------------
  Type for storing the Downlink RLC Logical Channel IDs infomation.
---------------------------------------------------------------------------*/
typedef struct 
{
  rlc_lc_id_type lc_id;                    /* Unique identifier for the 
                                              Downlink RLC logical 
                                              channel. */
  rrclcm_lc_status_e_type status;          /* Status of RLC Logical Channel 
                                              Indentifier entity. */
  uecomdef_logch_mode_e_type  mode;         /* Identifies mode of transmission 
                                              of RLC Logical Channel entity. */
  uecomdef_logchan_e_type lc_type;          /* Type of Logical Channel. */
  
  rrc_RB_Identity rb_id;                    /* Unique Identifier for Radio 
                                               bearer. */
  dsm_watermark_type *rx_queue;            /* Receive data queue. */
                                          
} rrclcm_dl_rlc_lc_info_type;



/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION rrclcm_init_ul_rlc_lc_info

DESCRIPTION
  This function will initialize Uplink RLC Logical Channel Info to the default
  values. All the Uplink RLC Logical Channel entities will be set to "Not in use"
  status.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void rrclcm_init_ul_rlc_lc_info
(    
  void  
);

/*===========================================================================

FUNCTION rrclcm_init_dl_rlc_lc_info

DESCRIPTION
  This function will initialize Downlink RLC Logical Channel Info to the default
  values. All the Downlink RLC Logical Channel entities will be set to "Not in 
  use" status.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void rrclcm_init_dl_rlc_lc_info
(    
  void  
);
/*===========================================================================

FUNCTION rrclcm_create_ul_dl_rlc_lc_q

DESCRIPTION
  This function will crete the Uplink/Downlinkwatermark queues

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrclcm_create_ul_dl_rlc_lc_q
(
  void
);
/*===========================================================================

FUNCTION rrclcm_allocate_ul_rlc_lc_id

DESCRIPTION
  This function will allocate Uplink RLC Logical Channel Id for the requested
  RLC service entity.

DEPENDENCIES
  None

RETURN VALUE
   The allocated RLC Logical Channel Id. If failed, it'll return 
   RRCLCM_RLC_LC_ID_ERROR.

SIDE EFFECTS
  None
===========================================================================*/

rlc_lc_id_type rrclcm_allocate_ul_rlc_lc_id
(      
  uecomdef_logchan_e_type ul_lc_type,      /* Uplink Logical Channel Type. */
  rrc_RB_Identity rb_id,                   /* Radio Bearer Id assoicated with
                                              the above Uplink Logical Channel
                                              Type. */
  uecomdef_logch_mode_e_type mode,         /* Mode of transmission for the RLC 
                                              service entity. */
  dsm_watermark_type **dsm_tx_queue        /* Pointer to the DSM Water Mark ptr
                                              associated with Uplink RLC Logical
                                              Channel Id. For Radio Bearers other
                                              than 0,1,2,3and 4 this will be 
                                              set to NULL pointer. */
);

/*===========================================================================

FUNCTION rrclcm_allocate_dl_rlc_lc_id

DESCRIPTION
  This function will allocate Downlink RLC Logical Channel Id for the 
  requested RLC service entity.

DEPENDENCIES
  None

RETURN VALUE
   The allocated RLC Logical Channel Id. If failed, it'll return 
   RRCLCM_RLC_LC_ID_ERROR.

SIDE EFFECTS
  None
===========================================================================*/

rlc_lc_id_type rrclcm_allocate_dl_rlc_lc_id
(    
  uecomdef_logchan_e_type dl_lc_type,      /* Downlink Logical Channel Type.*/
  rrc_RB_Identity rb_id,                   /* Radio Bearer Id assoicated with
                                              the above Downlink Logical 
                                              Channel type. */
  uecomdef_logch_mode_e_type mode,         /* Mode of transmission for the RLC 
                                              service entity. */
  dsm_watermark_type **dsm_rx_queue        /* Pointer to the DSM Water Mark 
                                              ptr associated with Downlink RLC 
                                              Logical Channel Id. For Radio
                                              Bearers other than 0,1,2,3,4,32
                                              33 and 34,this will be set to 
                                              NULL pointer. */
);

/*===========================================================================

FUNCTION rrclcm_check_ul_rlc_lc_id

DESCRIPTION
  This function will check whether Uplink RLC Logical Channel Id is alloated 
  or not for the requested RLC service entity.

DEPENDENCIES
  None

RETURN VALUE
   The allocated RLC Logical Channel Id. If it is not alreday allocated,
   it'll return RRCLCM_RLC_LC_ID_NOT_FOUND.

SIDE EFFECTS
  None
===========================================================================*/

rlc_lc_id_type rrclcm_check_ul_rlc_lc_id
(    
  uecomdef_logchan_e_type ul_lc_type,      /* Uplink Logical Channel Type. */
  rrc_RB_Identity rb_id,                   /* Radio Bearer Id assoicated with
                                              the above Downlink Logical 
                                              Channel type. */
  uecomdef_logch_mode_e_type mode          /* Mode of transmission for the RLC 
                                              service entity. */ 
);
/*===========================================================================

FUNCTION rrclcm_check_dl_rlc_lc_id

DESCRIPTION
  This function will check whether Downlink RLC Logical Channel Id is alloated 
  or not for the requested RLC service entity. RRC LLC can use this
  function to check whether a particular Logical channel entity is already 
  setup or not. If this function returns RRCLCM_RLC_LC_ID_NOT_FOUND, then the
  RRC LLC needs to setup Logical Channel entity  and the corresponding 
  physical channel for transmitting SDU on the uplink.

DEPENDENCIES
  None

RETURN VALUE
   The allocated RLC Logical Channel Id. If it is not alreday allocated,
   it'll return RRCLCM_RLC_LC_ID_NOT_FOUND.

SIDE EFFECTS
  None
===========================================================================*/

rlc_lc_id_type rrclcm_check_dl_rlc_lc_id
(    
  uecomdef_logchan_e_type dl_lc_type,       /* Downlink Logical Channel Type. */
  rrc_RB_Identity rb_id,                    /* Radio Bearer Id assoicated with
                                               the above Downlink Logical 
                                               Channel type. */
  uecomdef_logch_mode_e_type mode           /* Mode of transmission for the RLC 
                                               service entity. */
 
);
/*===========================================================================

FUNCTION rrclcm_deallocate_ul_rlc_lc_id

DESCRIPTION
  This function will deallocate Uplink RLC Logical Channel Id requested. It also 
  flushes out DSM items in the watermark queue before deallocating RLC Logical
  Channel Id.

DEPENDENCIES
  None

RETURN VALUE
   TRUE if deallocation is successful. Otherwise it returns FALSE.

SIDE EFFECTS
  None
===========================================================================*/

rrclcm_status_e_type rrclcm_deallocate_ul_rlc_lc_id
(    
  rlc_lc_id_type ul_lc_id               /* Uplink  RLC Logical Channel ID */
);

/*===========================================================================

FUNCTION rrclcm_deallocate_dl_rlc_lc_id

DESCRIPTION
  This function will deallocate Downlink RLC Logical Channel Id requested. It 
  also flushes out DSM items in the watermark queue before deallocating RLC 
  Logical Channel Id.

DEPENDENCIES
  None

RETURN VALUE
   TRUE if deallocation is successful. Otherwise it returns FALSE.

SIDE EFFECTS
  None
===========================================================================*/

rrclcm_status_e_type rrclcm_deallocate_dl_rlc_lc_id
(    
  rlc_lc_id_type dl_lc_id               /* Downlink  RLC Logical Channel ID */
);
/*===========================================================================

FUNCTION rrclcm_get_dl_lc_type

DESCRIPTION
  This function will identify the Logical Channel to which the OTA message
  belongs. The RRC procedure uses this infomation for processing the received 
  message.

DEPENDENCIES
  None

RETURN VALUE
   RRCLCM_SUCCESS if match was found. Otherwise returns RRCLCM_FAILURE.

SIDE EFFECTS
  This function should normally never return RRCLCM_FAILURE. 
  If it returns RRCLCM_FAILURE, then there is a race condition between Logical
  Channel Manager and RRC LLC. 
===========================================================================*/

rrclcm_status_e_type rrclcm_get_dl_lc_type
(    
  rlc_lc_id_type dl_lc_id,                    /* The Downlink RLC Logical
                                                 Channel Id. */
  uecomdef_logchan_e_type *dl_lc_type_ptr     /* Pointer to the Logical Channel
                                                 type assocated with the above 
                                                 Id. */
);
/*===========================================================================

FUNCTION rrclcm_get_ul_lc_type

DESCRIPTION
  This function will identify the Logical Channel to which the OTA message
  belongs. The RRC procedure uses this infomation for processing the received 
  message.

DEPENDENCIES
  None

RETURN VALUE
   RRCLCM_SUCCESS if match was found. Otherwise returns RRCLCM_FAILURE.

SIDE EFFECTS
  This function should normally never return RRCLCM_FAILURE. 
  If it returns RRCLCM_FAILURE, then there is a race condition between Logical
  Channel Manager and RRC LLC. 
===========================================================================*/

rrclcm_status_e_type rrclcm_get_ul_lc_type
(    
  rlc_lc_id_type ul_lc_id,                    /* The uplink RLC Logical
                                                 Channel Id. */
  uecomdef_logchan_e_type *ul_lc_type_ptr     /* Pointer to the Logical Channel
                                                 type assocated with the above 
                                                 Id. */
);

/*===========================================================================

FUNCTION rrclcm_identify_proc

DESCRIPTION
  This function will processes the event from the Dispatcher and identifies 
  the procedure to which the L2 ACK belongs.

DEPENDENCIES
  None

RETURN VALUE
  RRCLCM_SUCCESS if there is a match for MUI received. Other it returns
  RRCLCM_FAILURE

SIDE EFFECTS
  None
===========================================================================*/

rrclcm_status_e_type rrclcm_identify_proc
(    
  rrc_cmd_type *cmd_ptr,                    /* RRC Command Type */
  rrc_RB_Identity rb_id,                    /* RB Id for which MUI need to
                                               be allocated. */  
  rrc_proc_e_type *proc_name                /* RRC procedure    */
);

/*===========================================================================

FUNCTION rrclcm_get_mui

DESCRIPTION
  This function allocatess the MUI for the RRC procedure and RRC procedure can 
  include the same in the SDU if it is transmitting SDU in Acknowledged mode.

DEPENDENCIES
  None

RETURN VALUE
  RRCLCM_SUCCESS if allocation of MUI is successful. Other it returns
  RRCLCM_FAILURE

SIDE EFFECTS
  None
===========================================================================*/

rrclcm_status_e_type rrclcm_get_mui
(    
  rrc_proc_e_type rrc_proc,                /* The RRC Procedure type for 
                                              which MUI required. */
  rrc_RB_Identity rb_id,                   /* RB Id for which MUI need to
                                              be allocated. */  
  boolean l2ack_status,                    /* Indicates whether RRC needs
                                               L2 ACK or not */
  rrclcm_mui_type *mui                     /* The MUI allocated for the RRC
                                              procedure. */ 
);

/*===========================================================================

FUNCTION rrclcm_init_l2_ack_info

DESCRIPTION
  This function will initailize the L2 ACK info of all RRC procedures.

DEPENDENCIES
  None

RETURN VALUE
   None

SIDE EFFECTS
  None
===========================================================================*/

void rrclcm_init_l2_ack_info
(    
  void                  
);

/*===========================================================================

 FUNCTION rrclcm_post_event_for_dl_sdu

DESCRIPTION
  This function will post an internal event to RRC for the downlink SDU.

DEPENDENCIES
  None

RETURN VALUE
   None

SIDE EFFECTS
  None
===========================================================================*/

void rrclcm_post_event_for_dl_sdu
(    
  rlc_lc_id_type dl_lc_id,                    /* The Downlink RLC Logical
                                                 Channel Id. */  
  uint8 no_of_sdus,                            /* No of SDUs that are
                                                 queued */          
 void *context
);

/*===========================================================================

 FUNCTION rrclcm_post_event_for_dl_l2ack_ind

DESCRIPTION
  This function will post an internal event to RRC for the downlink L2 ACK 
  confirm indication.

DEPENDENCIES
  None

RETURN VALUE
   None

SIDE EFFECTS
  None
===========================================================================*/

void rrclcm_post_event_for_dl_l2ack_ind
(    
  rlc_lc_id_type dl_lc_id,                     /* The Downlink RLC Logical
                                                  Channel Id. */  
  uecomdef_status_e_type  status,              /* Indicates the status of 
                                                  success or failure of SDU
                                                  transmission */
  uint8 mui,                                   /* Message Unit Id */
  rlc_sdu_tx_fail_reason_e_type failure_reason /* Failure reason for failed l2ack */
);
/*===========================================================================

FUNCTION rrclcm_get_dl_watermark_ptr

DESCRIPTION
  This function will get watermark pointer corresponding to the RLC Logical
  channel Id.

DEPENDENCIES
  None

RETURN VALUE
   Pointer the DSM items' queue.

SIDE EFFECTS
  None
===========================================================================*/

dsm_watermark_type *rrclcm_get_dl_watermark_ptr
(    
  rlc_lc_id_type dl_lc_id                  /* The Downlink RLC Logical
                                              Channel Id. */  
  
);
/*===========================================================================

FUNCTION rrclcm_get_ul_watermark_ptr

DESCRIPTION
  This function will get watermark pointer corresponding to the RLC Logical
  channel Id.

DEPENDENCIES
  None

RETURN VALUE
   Pointer the DSM items' queue.

SIDE EFFECTS
  None
===========================================================================*/

dsm_watermark_type *rrclcm_get_ul_watermark_ptr
(    
  rlc_lc_id_type ul_lc_id                  /* The Downlink RLC Logical
                                              Channel Id. */  
  
);
/*===========================================================================

FUNCTION rrclcm_get_dl_rb_id

DESCRIPTION
  This function will get radio bearer Id  corresponding to the RLC Logical
  channel Id.

DEPENDENCIES
  None

RETURN VALUE
   rrc_RB_Identity.

SIDE EFFECTS
  None
===========================================================================*/

rrc_RB_Identity rrclcm_get_dl_rb_id
(    
  rlc_lc_id_type dl_lc_id                  /* The Downlink RLC Logical
                                              Channel Id. */  
  
);
/*===========================================================================

FUNCTION rrclcm_get_ul_rb_id

DESCRIPTION
  This function will get radio bearer Id  corresponding to the RLC Logical
  channel Id.

DEPENDENCIES
  None

RETURN VALUE
  rrc_RB_Identity.

SIDE EFFECTS
  None
===========================================================================*/

rrc_RB_Identity rrclcm_get_ul_rb_id
(    
  rlc_lc_id_type ul_lc_id                  /* The uplink RLC Logical
                                              Channel Id. */   
);

/*===========================================================================

FUNCTION rrclcm_clear_rlc_lc_info

DESCRIPTION
  This function will clear and initialize the RLC Logical Channel Id
  Info for both Uplink and Downlink.
  Data items in all watermark queues will be dequeued and released.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void rrclcm_clear_rlc_lc_info
( 
 void
);

/*===========================================================================

FUNCTION rrclcm_get_ul_rlc_lc_id

DESCRIPTION
  This function will check whether Uplink RLC Logical Channel Id is alloated 
  or not for the requested RLC service entity. Any RRC procedure can use this
  function to check whether a particular Logical channel entity is already 
  setup or not.

DEPENDENCIES
  None

RETURN VALUE
   The allocated RLC Logical Channel Id. If it is not alreday allocated,
   it'll return RRCLCM_RLC_LC_ID_NOT_FOUND.

SIDE EFFECTS
  None
===========================================================================*/

rlc_lc_id_type rrclcm_get_ul_rlc_lc_id
(    
  rrc_RB_Identity rb_id                     /* Radio Bearer Id assoicated with
                                               the above Downlink Logical 
                                               Channel type. */ 
);

/*===========================================================================

FUNCTION rrclcm_get_dl_rlc_lc_id

DESCRIPTION
  This function will check whether Downlink RLC Logical Channel Id is alloated 
  or not for the requested RLC service entity. RRC LLC can use this
  function to check whether a particular Logical channel entity is already 
  setup or not. 

DEPENDENCIES
  None

RETURN VALUE
   The allocated RLC Logical Channel Id. If it is not alreday allocated,
   it'll return RRCLCM_RLC_LC_ID_NOT_FOUND.

SIDE EFFECTS
  None
===========================================================================*/

rlc_lc_id_type rrclcm_get_dl_rlc_lc_id
(    
  rrc_RB_Identity rb_id                     /* Radio Bearer Id assoicated with
                                               the above Downlink Logical 
                                               Channel type. */
 
);

/*===========================================================================

FUNCTION rrclcm_get_ul_rlc_mode

DESCRIPTION
  This function will check whether Uplink RLC Logical Channel Id is alloated 
  or not for the requested RLC service entity. If allocated then it returns
  assoctaed mode of transmission.

DEPENDENCIES
  None

RETURN VALUE
   Returns RRCLCM_SUCCESS if RLC Logical Channel Id allocated. Otherwise returns
   RRCLCM_FAILURE.

SIDE EFFECTS
  None
===========================================================================*/

rrclcm_status_e_type rrclcm_get_ul_rlc_mode
(    
  rlc_lc_id_type ul_lc_id,                     /* RLC Logical Channel Id */
  uecomdef_logch_mode_e_type *tx_mode          /* Mode of transmission */
);

/*===========================================================================

FUNCTION rrclcm_get_dl_rlc_mode

DESCRIPTION
  This function will check whether Downlink RLC Logical Channel Id is alloated 
  or not for the requested RLC service entity. If allocated then it returns 
  associated mode reception.

DEPENDENCIES
  None

RETURN VALUE
   Returns RRCLCM_SUCCESS if RLC Logical Channel Id allocated. Otherwise returns
   RRCLCM_FAILURE.

SIDE EFFECTS
  None
===========================================================================*/

rrclcm_status_e_type rrclcm_get_dl_rlc_mode
(    
  rlc_lc_id_type dl_lc_id,                   /* RLC Logical Channel Id */
  uecomdef_logch_mode_e_type *rx_mode        /* Mode of reception */

 
);

/*===========================================================================

FUNCTION rrclcm_update_db_with_srb5_wmq_info

DESCRIPTION
   This function ideally is expected to be called after a logical
   channel ID gets allocated to SRB#5 RB. During LCM initialization,
   watermark pointer is set to NULL. 

DEPENDENCIES
  None

RETURN VALUE
   Returns TRUE SRB#5 watermark is successfully updated.
           FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
boolean rrclcm_update_db_with_srb5_wmq_info(rlc_lc_id_type srb5_lc_id);

/*===========================================================================

FUNCTION rrclcm_get_rb_id_if_dl_lc_type_exists

DESCRIPTION
   This function checks whether Downlink RLC Logical Channel type exists in LCM
   database or not & returns the RB Id if present. See side effect.

DEPENDENCIES
  None

RETURN VALUE
   Returns RRCLCM_SUCCESS if DL RLC Logical Channel type exists.
   Otherwise returns RRCLCM_FAILURE.

SIDE EFFECTS
  Please note that for the given logical channel type, only the first 
  occurence of RB ID is returned.
===========================================================================*/
rrclcm_status_e_type rrclcm_get_rb_id_if_dl_lc_type_exists
(    
  uecomdef_logchan_e_type  lc_type,        /* Mode of reception */
  rrc_RB_Identity         *rb_id           /* Radio bearer ID */
);

#if 1//def FEATURE_DUAL_SIM

/*===========================================================================

FUNCTION rrc_check_for_pending_ul_srb_data

DESCRIPTION
  This function checks whether UE is waiting for ul srb pending data
  for SRB

DEPENDENCIES
  None

RETURN VALUE
   TRUE : If any SRB is waiting for UL srb pending data
   else returns  FALSE

SIDE EFFECTS
  None
===========================================================================*/

boolean rrc_check_for_pending_ul_srb_data(boolean *l2_int_data_pending);
/*===========================================================================

FUNCTION rrc_check_for_pending_dl_srb_data

DESCRIPTION
  This function checks whether UE is waiting for dl srb pending data
  for SRB

DEPENDENCIES
  None

RETURN VALUE
   TRUE : If any SRB is waiting for dl srb pending data
   else returns  FALSE

SIDE EFFECTS
  None
===========================================================================*/

boolean rrc_check_for_pending_dl_srb_data(void);

#endif

/*===========================================================================

FUNCTION rrclcm_get_nchan_dl_tm

DESCRIPTION
  This function will return number of DL TM channel that are used

DEPENDENCIES
  None

RETURN VALUE
   Returns number of CL TM channels that are used

SIDE EFFECTS
  None
===========================================================================*/

uint8 rrclcm_get_nchan_dl_tm
(    
  void
);
/*===========================================================================

FUNCTION rrclcm_get_mui_from_proc_id

DESCRIPTION
  This function gets the mui from the procedure id

DEPENDENCIES
  None

RETURN VALUE
   None

SIDE EFFECTS
  None
===========================================================================*/

rrclcm_mui_type rrclcm_get_mui_from_proc_id
(    
  rrc_proc_e_type proc_id,
  rrc_RB_Identity rb_id
);

/*===========================================================================

FUNCTION rrclcm_internal_post_l2_ack_ind

DESCRIPTION
  This function posts l2 ack indication

DEPENDENCIES
  None

RETURN VALUE
   None

SIDE EFFECTS
  None
===========================================================================*/

void rrclcm_internal_post_l2_ack_ind_for_proc
(    
  rrc_proc_e_type proc_id,
  rrc_RB_Identity rb_id
);

#endif /* RRCLCM_H */
