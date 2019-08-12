#ifndef DS707_SDB_H
#define DS707_SDB_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               D S 7 0 7 _ S D B   F I L E

GENERAL DESCRIPTION
  This file contains functions used to implement Short Data Burst (SDB)
  available in IS2000.

EXTERNALIZED FUNCTIONS
  ds707_sdb_DBM_event_handler()

  ds707_sdb_process_txq()

  ds707_sdb_init()

  ds707_sdb_supports_SDB_now()

INITIALIZATION AND SEQUENCING REQUIREMENTS

 Copyright (c) 2002 - 2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_sdb.h_v   1.1   06 Feb 2003 17:49:02   ajithp  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_sdb.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    -------------------------------------------------------
11/28/11    jz      support ppp control packet on dos 
04/08/11    dvk    Fix SDB Support Query functionality.
12/21/06    spn    Removed parameters that are not required
12/18/06    spn    Introduced DS_META_INFO in place of PS_META_INFO
04/05/05    gr     Fixed the handling of SO_OMIT field in the SDB header
04/07/05    atp    Added support for DOS.
04/22/04    ak     Made ds707_sdb_curr_pkt_needs_SDB global.
07/07/03    atp    SDB enhancements to pass status notification to app.
02/06/03    atp    Added ds707_sdb_supports_SDB_now as external function.
10/29/02    atp/sy Initial version.

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA_IS707
#if defined(FEATURE_DATA_IS2000_SDB) || defined (FEATURE_HDR_DOS)
#include "cm.h"
#include "dsm.h"
#include "dsrlpi.h"
#include "queue.h"
#include "ds707_pkt_mgri.h"
#include "ps_hdlc_lib.h"
#include "ds1x_meta_info.h"
#include "ps_iface_defs.h"

/*===========================================================================
                         TYPEDEFS & VARIABLES
===========================================================================*/
#define DS707_SDB_SO_OMIT_MASK  0x10  /* Bit 1 in location shown: xxx1xxxx */

/*---------------------------------------------------------------------------
  Number of SDB session that can be using SDB. This number should be same as
  Number of simultaneous RLP3 sessions that can be supported.
---------------------------------------------------------------------------*/
#define DS707_SDB_NUM DSRLP_MAX_RSCB_ELEMENTS

/*---------------------------------------------------------------------------
  Maximum size of the SDB message. This value is obtained from the
  standard, because it allocates 8 bits. And hence the value is choosen
  as 2^8 = 256
---------------------------------------------------------------------------*/
#define DS707_SDB_MSG_MAX_SIZE  256

/*---------------------------------------------------------------------------
   Maximum size of the data payload that can fit in the outgoing message.
   For SO 33 calls it is 255. For non-SO 33 calls it is 253. Since we want
   to see maximum data its value is chosen as 255.
---------------------------------------------------------------------------*/
#define DS707_SDB_DATA_MAX_SIZE 255

/*---------------------------------------------------------------------------
   Maximum Number of outstanding SDB packets that standard allows
---------------------------------------------------------------------------*/
#define DS707_MAX_SDB_PKT 3

/*---------------------------------------------------------------------------
  Radio transports that can be used during dormancy.
    For SDB we build a message and send it immediately.
    For RLP , we need to reoriginate a call before sending SDB.
---------------------------------------------------------------------------*/
typedef enum
{
  RADIO_TRANSPORT_SDB         = 0,
  RADIO_TRANSPORT_RLP3        = 1
} ds707_radio_transport_in_dorm_type;

/*---------------------------------------------------------------------------
  SDB configuration type. This will contain the watermark from where SDB
  will read the outgoing data;
  The watermark into which SDB will write the incoming data.
  The Receive function will be called, to inform PS/SIO if there is any data
  transferred to them.
  Note: The contents will be same as RLP configuration. So, initialize these
  watermarks to same values as RLP.
---------------------------------------------------------------------------*/
typedef struct
{
  dsm_watermark_type *tx_wm_ptr;              /* */
  void (*rx_func_ptr)(void);                  /* */
  q_type *rx_q_ptr;                           /* */
  byte   sr_id;                               /* */
} ds707_sdb_cfg_type;

/*---------------------------------------------------------------------------
  Contains the data related information for the buffer. It stores the Size
  of the element and Data array. MAY NOT BE NEEDED
---------------------------------------------------------------------------*/
typedef struct
{
  byte size;
  byte msg_buf[DS707_SDB_MSG_MAX_SIZE];
} ds707_sdb_data_type;

/*---------------------------------------------------------------------------
  For configuration parameters we need it for every Service i.e. multiple
  SR_Ids
  For data, we copy the data temporarily and give it to CM. So, this data
  will not be used by multiple services at same instance and hence is not
  needed for every service.
---------------------------------------------------------------------------*/
typedef struct
{
 ds707_sdb_cfg_type  sdb_cfg[DS707_SDB_NUM];
 ds707_sdb_data_type sdb_arr;

 /* hdlc configuration data */
 void                     *hdlc_inst_ptr;
 hdlc_config_type          hdlc_config;

} ds707_sdb_info_type;

/* Structure to hold DoS or SDB call back data */
typedef struct ds707_sdb_data_block_s {

  q_link_type link; /*  link to queue (This has to be the first element of the 
                     structure. Refer the queue.h for more info.)*/
  int32    sock_id; /*  sock_id for the dbm msg */
  dsm_item_type       *backup_item_ptr; /*backuped dsm item for re-sending on TC when DoS sending fails*/
  dsm_watermark_type  *tx_wm_ptr; /*water mark that this packet belongs to*/
} ds707_sdb_data_block_s_type;

/*===========================================================================
                   EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_SDB_DBM_EVENT_HANDLER

DESCRIPTION   This function will be registered by DS on power on. This
              function will be called by CM when ever it obtains an MT
              DBM message. So in this function, DS should act only if it
              has MT SDB message.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_sdb_DBM_event_handler
(
  cm_dbm_event_e_type event, 
  const cm_dbm_info_s_type*  dbm_info_ptr
);

/*===========================================================================
FUNCTION      DS707_SDB_PROCESS_TXQ

DESCRIPTION   Return true if we can do send the SDB packet or if there is
              nothing to do otherwise returns false saying that the packet
              needs to be sent using RLP3.

DEPENDENCIES  None.

RETURN VALUE  True/False.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_sdb_process_txq(const ds707_flow_type *flow_ptr);

/*===========================================================================
FUNCTION      DS707_SDB_INIT

DESCRIPTION   This function initializes all the SDB related variables. This
              will be called when a data call is established.  This function
              should be called whenever RLP is being set up. So, whenever
              dsrlp_reg_srvc() is called, its good to call this function also
              and set up the appropriate values.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_sdb_init
(
  byte                sr_id_index,           /* index for a session id     */
  dsm_watermark_type  *tx_watermark_ptr,     /* ptr to Tx wm for txmit data*/
  void               (*post_rx_func_ptr)(void),/* rx callback for rx frames  */
  q_type              *post_rx_q_ptr         /* queue for rx'ed frames     */
);

/*===========================================================================
FUNCTION      DS707_SDB_CURR_PKT_NEEDS_SDB

DESCRIPTION   This function is used to confirm that the current packet wants
              to go on the SDB by looking at its Meta Info.
              If this packet does not desire SDB, originate a data call.
              If it is SDB, then, we need to check for two more things:
              a) Are  flags requesting for ACH or Enhanced-Access channel.
                 If Access channel, return TRUE
                 If Enhanced-Access Channel,
              b) Confirm the PREV before returning TRUE.
                 If PREV is >= 7, return TRUE.
                 Else return FALSE.

DEPENDENCIES  None.

RETURN VALUE  True/False.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_sdb_curr_pkt_needs_SDB
(
  const ds1x_meta_sdb_type  *meta_info_ptr
);

/*===========================================================================
FUNCTION      DS707_SDB_SUPPORTS_SDB_NOW

DESCRIPTION   Confirm if MS supports SDB. We need to check for two things:
              1. Are the flags requesting for ACH or Enhanced-Access channel.
                 If (Access channel)
                   Continue to test 2.
                 If (Enhanced-Access Channel)
                   Confirm the PREV supports R-EACH before returning SDB.
                 If (PREV is >= 7)
                   continue to test 2
                 else
                   return RLP

              2. Is the Mobile state currently dormant.
                 If (MS is currently dormant)
                 If(previous data call  == SO 33)
                   Return SDB
                 Else
                   Return RLP

DEPENDENCIES  None.

RETURN VALUE  TRUE:  If SDB is supported in current DS state.(Depends on SO)
              FALSE: If SDB is not supported in current DS state.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_sdb_supports_SDB_now
(
  uint32 flags,
  ps_flow_type *flow_ptr
);

#ifdef FEATURE_HDR
#ifdef FEATURE_HDR_QOS
#ifdef FEATURE_HDR_DOS
/*===========================================================================
FUNCTION      DS707_IS_SDB_DOS_SUPPORTED_NOW

DESCRIPTION   Confirm if atleast one of the Sec flows /Default flow 
              Supports SDB/DOS. 
          
DEPENDENCIES  None.

RETURN VALUE  TRUE:  If SDB is supported in current DS state.(Depends on SO)
              FALSE: If SDB is not supported in current DS state.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_is_SDB_DOS_supported_now
(
  uint32 flags
);
#endif /* FEATURE_HDR_DOS */
#endif /* FEATURE_HDR_QOS */
#endif /* FEATURE_HDR */

/*===========================================================================
FUNCTION      DS707_SDB_SO_OMIT_HANDLER

DESCRIPTION   This is the callback function registered with MC (hence it 
              runs in the MC context). It just updates the SO_OMIT_INDICATOR
              as informed by the base station

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds707_sdb_so_omit_handler
(
  boolean so_omit_indicator
);

#endif /* FEATURE_DATA_IS2000_SDB || FEATURE_HDR_DOS */
#endif /* FEATURE_DATA_IS707      */
#endif /* DS707_SDB_H             */
