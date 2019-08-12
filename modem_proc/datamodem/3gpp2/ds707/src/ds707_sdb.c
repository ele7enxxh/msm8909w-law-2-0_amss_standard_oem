
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               D S 7 0 7 _ S D B   F I L E

GENERAL DESCRIPTION
  This file contains functions used to implement Short Data Burst (SDB)
  available in IS2000.

EXTERNALIZED FUNCTIONS
  ds707_sdb_DBM_event_handler()

  ds707_sdb_tx_sdb_pkt()

  ds707_sdb_init()

  ds707_sdb_supports_SDB_now()

INITIALIZATION AND SEQUENCING REQUIREMENTS

 Copyright (c) 2002-2013 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_sdb.c_v   1.4   06 Feb 2003 17:49:18   ajithp  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_sdb.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ------------------------------------------------------- 
09/04/13    sc     Fix the memory leak when SDB queue cannot accept any
                   more data as it has reached max size 
06/26/13    sc     Replace all the memcpy and memmove with memscpy and 
                   memsmove as its safer.
04/25/13    fj     Cleanup Qchat feature.  
02/05/13    sc     Fixed flag check for MSG_EXPEDITE/MSG_FAST_EXPEDITE
01/29/13    ssb    Added changes to support freeing of chained buffers
07/11/12    sb     Fixed KW Errors
02/16/12    msh    Fixed compiler warning
01/04/12    ack    Fixed KW Warnings
12/23/11    sb     Fixed compiler warnings
11/29/11    vpk    HDR SU API cleanup
10/28/11    jz     Re-sending on TC when DoS sending fails, 3 main         
                   modifications:
                   - add two elements in ds707_sdb_data_block_s_type:
                     tx_wm_ptr & backup_item_ptr;
                   - backup dsm item before sending packet;
                   - when sending fails, try re-sending the backuped
                     packet on TC.
08/09/11    sha    Global variable clean up.
08/09/11    ms     Made changes to drop the DOS/SDB packet if its payload 
                   size is greater than max allowed length of 255 bytes.
06/15/11    ls     Send DOS only when RLP is not inited which means that
                   HDR connection is not open and registers PCP call back
04/19/11    dvk    Changed MSG_HIGH/MSG_ERROR to DATA_IS707_MSG* and 
                   replaced ASSERT() with gracefull return
04/09/11    ms     Fix compiler error.
04/08/11    dvk    Fix SDB Support Query functionality. 
03/15/11    mg     Global variable cleanup
10/13/10    op     Migrated to MSG 2.0 macros
11/10/09    as     Cleared Windows Compiler warnings for MOB builds
06/11/09    ss     Fixed off target lint medium error.
08/16/07    spn    Lint changes.
03/05/07    ac     Merge the change in the EMPA SandBox 5209 to the tip
03/05/07    spn    Fixed lint errors
12/21/06    spn    Removed parameters that are not required.
12/18/06    spn    Introduced DS_META_INFO in place of PS_META_INFO
10/24/06    vrk    assign dsm item apps field before putting the item
                   to the PPP queue
06/21/06    squ    removed hdlc_mode = HDLC_HW_PREF.
01/03/06    ksu    EMPA: hook up forward link with new phys link architecture
12/23/05    ksu    EMPA RSP: two fwd rx queues
12/23/05    ksu    EMPA RSP: two rev watermarks per ip flow
12/12/05    spn    Added support for DoS Attribute
12/15/05    TMR    Changed RLP RX queue to watermark and added parameter to 
                   dsrlp_reg_srvc() and RX callback function and added 
                   dsrlp_queue_sdb_rx_data call
09/07/05    gr/vrk enabled hdlc_mode to HDLC_HW_PREF
08/23/05    spn    modified so that FEATURE_HDR_DOS is valid only when 
                   FEATURE_HDR_QOS is turned on but not vice-versa. 
05/27/05    gr     modified to make sockets work temp fix 
                   before HDR DATA QOS merge
05/17/05    gr     Temporarily commented out QOS specific changes for merge
04/05/05    gr     Fixed the handling of SO_OMIT field in the SDB header
04/19/05    atp    Modified ds707_dos_rx_handler() to take in a dsm_item.
04/13/05    atp    Modified Featurization (Feature_hdr_dos)
04/07/05    atp    Added support for DOS.
04/05/05    gr     Integrated EPZID functionality onto the main line
09/08/04    vr     Notify the PZID hysterisis engine if an SDB is received
                   or transmitted
07/21/04    vr     Do not send SDB if the mobile dormant on HDR, bring up
                   the traffic channel instead.
04/22/04    ak     Made ds707_sdb_curr_pkt_needs_SDB global.
02/17/04    ak     SDB handler is now with phys link instead of ps iface.
02/03/04    ak     Meta info now inside dsm item data block, so extract & 
                   pull up appropriately.
10/14/03    atp    Removed LOCAL declaration of ds707_sdb_supports_SDB_now().
07/28/03    atp    Changes to map CM status info to DS for apps.
07/07/03    atp    SDB enhancements to pass status notification to app.
04/17/03    atp    Minor change - use q_cnt() function instead of directly
                   accessing queue count field.
03/17/03    atp    Added 2 fixes: 
                   - scenario flags ACH=1,REACH=1, but REACH not supported.
                   - Fixed incoming SDB problem (not going to Rx queue).
02/06/03    atp    Moved ds707_sdb_supports_SDB_now() to .h as external fn.
11/27/02    atp    Get tx wmk from pkt mgr using call instance instead of 
                   sr_id. Removed assert if wmk is NULL.
11/08/02    atp    PS reverted to passing meta info as * in dsm item.
11/06/02    atp    Synced to PS change that passes meta info as ** in dsm.
10/29/02    atp/sy Initial version.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "data_msg.h"
#include <stringl/stringl.h>

#ifdef FEATURE_DATA_IS707
#if defined(FEATURE_DATA_IS2000_SDB) || defined (FEATURE_HDR_DOS)
#include "dstask.h"
#include "ds3gmgrint.h"
#include "ds707_pkt_mgr.h"
#include "ds707_pkt_mgri.h"

#ifdef FEATURE_HDR_QOS
#include "ds707_sec_pkt_mgr.h"
#include "hdrmrlpcif.h"
#ifdef FEATURE_HDR_DOS 
#include "hdrcom_api.h"
#include "ds707_sdb_mdm.h"
#endif /* FEATURE_HDR_DOS */
#endif /* FEATURE_HDR_QOS */

#ifdef FEATURE_QCHAT 
#include "data1x_qchatupk_opts.h"
#include "data1x_qchatupk.h"
#endif /* FEATURE_QCHAT */ 

#include "ds707_sdb.h"
#include "ds707_pkt_mgri.h"
#include "ds707_wmk.h"
#include "dssocket_defs.h"
#include "mccdma_v.h"

#include "ps_phys_link.h"
#include "dsm.h"
#ifdef FEATURE_DS_EPZID_HYSTERESIS
#include "ds707_epzid_hyst.h"
#endif /* FEATURE_DS_EPZID_HYSTERESIS */

#include "ps_svc.h"

#ifdef FEATURE_DATA_OPTHO
  #include "ds707_s101.h"
#endif /* FEATURE_DATA_OPTHO */

/*===========================================================================
                         TYPEDEFS & VARIABLES
===========================================================================*/
word ds707_sdb_prev_data_call_so;
byte ds707_sdb_prev_data_sr_id;
LOCAL ds707_sdb_info_type ds707_sdb_info;

LOCAL boolean ds707_sdb_so_omit_indicator = FALSE;

/*---------------------------------------------------------------------------
  SDB data free queue.
---------------------------------------------------------------------------*/
LOCAL q_type ds707_sdb_data_free_q;    

ds707_sdb_data_block_s_type ds707_sdb_free_bufs[DS707_MAX_SDB_PKT]; 

/* Declare a task critical section for serialized code execution. Used
 * in place of TASKLOCK()/TASKFREE(). */
static rex_crit_sect_type ds707_sdb_process_txq_crit_sect;

/*===========================================================================
                            Forward Declarations
===========================================================================*/

LOCAL boolean ds707_sdb_is_reach_supported_or_is_hdr
(
  void
);


LOCAL void ds707_sdb_cm_tx_err_status
(
  const void   *data_block_ptr,         
  cm_dbm_cmd_e_type dbm_cmd, 
  cm_dbm_cmd_err_e_type dbm_cmd_err   
);

LOCAL byte ds707_sdb_sr_id_to_idx
(
  byte sr_id
);

LOCAL void ds707_sdb_tx_handler
(
  dsm_item_type *item_ptr,
  uint8              link_flow_number,
  byte               route,
  dsm_watermark_type        *tx_wm_ptr,
  const ds1x_meta_info_type  *meta_info_ptr
);

LOCAL void ds707_sdb_rx_handler
(
  uint8 *buf,
  uint32 buf_len
);

LOCAL void ds707_dos_rx_handler
(
  const cm_dbm_info_s_type* dbm_info_ptr
);

LOCAL int8  ds707_sdb_to_cm_copy
(
  void   *dsm_item_ref_ptr,
  uint8  *buf,
  uint16 *len_ptr
);

LOCAL ds707_radio_transport_in_dorm_type ds707_sdb_radio_transport_in_dorm
(
  const ds1x_meta_sdb_type *meta_info_ptr,
  boolean            * set_sig,
  ps_flow_type      *flow_ptr
);

LOCAL boolean ds707_sdb_clone_dsm_item(
                                  dsm_item_type *src_item_ptr,
                                  dsm_item_type       **copied_item_ptr
                                  );

/*===========================================================================
                   EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_SDB_DBM_EVENT_HANDLER

DESCRIPTION   This function will be registered by DS on power on. This
              function will be called by CM when ever it obtains an MT
              DBM message, or it gets status of MO SDB that was transmitted.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_sdb_DBM_event_handler
(
  cm_dbm_event_e_type event, 
  const cm_dbm_info_s_type* dbm_info_ptr
)
{
  int32             sock_id;  /* this was passed as user data to CM when
                                   SDB was sent                            */
  ps_phys_link_type            *phys_link_ptr; /* ptr to pkt iface         */

  /* status passed to upper layer
   */
  ps_phys_link_dos_ack_status_info_type  ack_status; 
  ds707_sdb_data_block_s_type           *data_block_ptr;
  

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*-----------------------------------------------------------------------
      Check the input values
      If they are unacceptable, just return from function without doing
      anything.
    -----------------------------------------------------------------------*/
    if( (event >= CM_DBM_EVENT_MAX) || (dbm_info_ptr == NULL) )
    {
       DATA_IS707_MSG2(MSG_LEGACY_ERROR, "invalid event:%d or ptr:%x",
                       event, dbm_info_ptr);
       return;
    }

    /*-----------------------------------------------------------------------
      Process the input.
      Find which event occurred. 
    -----------------------------------------------------------------------*/
    switch(event)
    {
    /*-----------------------------------------------------------------------
      Mobile-Terminated SDB.
    -----------------------------------------------------------------------*/
      case CM_DBM_EVENT_MT_MSG:
        /*-------------------------------------------------------------------
          Check if the incoming message is SDB or not.
          If it is SDB, then, handle it.
          If its not SDB,then ignore it.
        -------------------------------------------------------------------*/
        if(dbm_info_ptr->burst_type == CAI_SHORT_DATA_BURST)
        {
#ifdef FEATURE_DS_EPZID_HYSTERESIS
         /*------------------------------------------------------------------
           Inform PZID hysteresis processor of data ready state 
	 ------------------------------------------------------------------*/
         ds707_epzid_hyst_send_cmd(DS_CMD_707_PKT_EPZID_HYS_SDB_DATA);
#endif /* FEATURE_DS_EPZID_HYSTERESIS */
#ifdef FEATURE_HDR_QOS
#ifdef FEATURE_HDR_DOS
          if (dbm_info_ptr->burst_source == CM_DBM_FROM_HDR)
          {
             /*----------------------------------------------------------------
               Call the DOS receive handler
             ----------------------------------------------------------------*/
             ds707_dos_rx_handler(dbm_info_ptr);
             break;
          }
          else
#endif /* FEATURE_HDR_DOS */
#endif /* FEATURE_HDR_QOS */
          {
            if (dbm_info_ptr->burst_source == CM_DBM_FROM_1X)
            {
               /*-------------------------------------------------------------
                 Call the SDB receive handler
               -------------------------------------------------------------*/
               ds707_sdb_rx_handler(dbm_info_ptr->payload,
                                    dbm_info_ptr->num_bytes
                                   );
            }
            else
            {
              DATA_IS707_MSG0(MSG_LEGACY_LOW, 
                              "rxed DBM from unlikely system:Ignoring");
            }
          }
        }
        else
        {
          DATA_IS707_MSG0(MSG_LEGACY_LOW, "rxed Non-SDB DB event:Ignoring");
        }
        break;

    /*-----------------------------------------------------------------------
      Status of mobile-originated SDB, that was sent earlier.
    -----------------------------------------------------------------------*/
      case CM_DBM_EVENT_MO_STATUS:
        /*-------------------------------------------------------------------
          Check if SDB or not.
          If it is SDB, then, handle it.
          If its not SDB,then ignore it.
        -------------------------------------------------------------------*/
        if(dbm_info_ptr->burst_type == CAI_SHORT_DATA_BURST)
        {
          data_block_ptr = (ds707_sdb_data_block_s_type *) dbm_info_ptr->user_data;
          sock_id = data_block_ptr->sock_id;

          /*-----------------------------------------------------------------
            Re-claim the memory and put in free queue
          -----------------------------------------------------------------*/
          q_put(&ds707_sdb_data_free_q, &data_block_ptr->link);
        
          /*-----------------------------------------------------------------
            Map CM info to DS type.
          -----------------------------------------------------------------*/
          ack_status.overflow = (uint32) dbm_info_ptr->burst_overflow_size;
          DATA_IS707_MSG1(MSG_LEGACY_MED, "DoS call back for status indication : %d", 
                   (uint32)dbm_info_ptr->mo_status);
          switch ( dbm_info_ptr->mo_status )
          {
            case CM_DBM_MO_NONE:
              ack_status.status = PS_PHYS_LINK_DOS_ACK_NONE;
              break;

            case CM_DBM_MO_OK:
              ack_status.status = PS_PHYS_LINK_DOS_ACK_OK;
              break;

            case CM_DBM_MO_HOLD_ORIG_RETRY_TIMEOUT:
              ack_status.status = PS_PHYS_LINK_DOS_ACK_HOLD_ORIG_RETRY_TIMEOUT;
              break;

            case CM_DBM_MO_HOLD_ORIG:
              ack_status.status = PS_PHYS_LINK_DOS_ACK_HOLD_ORIG;
              break;

            case CM_DBM_MO_NO_SRV:
              ack_status.status = PS_PHYS_LINK_DOS_ACK_NO_SRV;
              break;

            case CM_DBM_MO_ABORT:
              ack_status.status = PS_PHYS_LINK_DOS_ACK_ABORT;
              break;

            case CM_DBM_MO_NOT_ALLOWED_IN_AMPS:
              ack_status.status = PS_PHYS_LINK_DOS_ACK_NOT_ALLOWED_IN_AMPS;
              break;

            case CM_DBM_MO_NOT_ALLOWED_IN_HDR:
              ack_status.status = PS_PHYS_LINK_DOS_ACK_NOT_ALLOWED_IN_HDR;
              break;

            case CM_DBM_MO_L2_ACK_FAILURE:
              ack_status.status = PS_PHYS_LINK_DOS_ACK_L2_ACK_FAILURE;
              break;

            case CM_DBM_MO_L3_ACK_FAILURE:
              ack_status.status = PS_PHYS_LINK_DOS_ACK_L3_ACK_FAILURE;
              break;            

            case CM_DBM_MO_OUT_OF_RESOURCES:
              ack_status.status = PS_PHYS_LINK_DOS_ACK_OUT_OF_RESOURCES;
              break;

            case CM_DBM_MO_ACCESS_TOO_LARGE:
              ack_status.status = PS_PHYS_LINK_DOS_ACK_ACCESS_TOO_LARGE;
              break;

            case CM_DBM_MO_DTC_TOO_LARGE:
              ack_status.status = PS_PHYS_LINK_DOS_ACK_DTC_TOO_LARGE;
              break;

            case CM_DBM_MO_OTHER:
              ack_status.status = PS_PHYS_LINK_DOS_ACK_OTHER;
              break;

            case CM_DBM_MO_ACCT_BLOCK:
              ack_status.status = PS_PHYS_LINK_DOS_ACK_ACCT_BLOCK;
              break;            
              
            default:
              ack_status.status = PS_PHYS_LINK_DOS_ACK_NONE;
              break;
          }
          if(dbm_info_ptr->mo_status != CM_DBM_MO_OK)
          {
            DATA_IS707_MSG1(MSG_LEGACY_MED, "DoS sending fails, error:%d",
                            dbm_info_ptr->mo_status);
            /*-----------------------------------------------------------------
              Try re-sending packet on TC: enqueue the backuped dsm item on
              corresponding flow watermark. Note that the MSG_EXPEDITE or
              MSG_FAST_EXPEDITE flag in this backuped dsm item's meta info 
              had been cleared when we backuped the packet before sending, hence,
              even when in dormant, it will eventually go to RLP 'cause ds707
              will bringup TC when ds707_sdb_process_txq return FALSE.
            -----------------------------------------------------------------*/
            if(data_block_ptr->tx_wm_ptr!=NULL &&
               data_block_ptr->backup_item_ptr!=NULL)
            {
              DATA_IS707_MSG2(MSG_LEGACY_MED, 
                  "DoS trying re-sending on TC,backup_item_ptr:%x,wm:%x",
                  data_block_ptr->backup_item_ptr, data_block_ptr->tx_wm_ptr);

              dsm_enqueue(data_block_ptr->tx_wm_ptr,
                          &(data_block_ptr->backup_item_ptr));
              /*-----------------------------------------------------------------
                Since we re-send the packet on TC, we indicate applications that
                packet has been successfully sent
              -----------------------------------------------------------------*/
              ack_status.status = PS_PHYS_LINK_DOS_ACK_OK;
              ack_status.overflow = 0;
            }
            else
            {
              DATA_IS707_MSG2(MSG_LEGACY_MED, "DoS trying re-sending fails, "
                  "tx_wm_ptr=%x, backup_item_ptr=%x",
                  data_block_ptr->tx_wm_ptr, data_block_ptr->backup_item_ptr);
              if(data_block_ptr->backup_item_ptr!=NULL)
              {
                dsm_free_packet(&(data_block_ptr->backup_item_ptr));
                data_block_ptr->backup_item_ptr = NULL;
              }
            }
          }
          else
          {
            /*-----------------------------------------------------------------
              DoS sending succeeds, release the backup dsm item.
            -----------------------------------------------------------------*/
            if(data_block_ptr->backup_item_ptr!=NULL)
            {
              DATA_IS707_MSG0(MSG_LEGACY_MED,
                              "DoS send successful. Freeing Packet");
              dsm_free_packet(&(data_block_ptr->backup_item_ptr));
              data_block_ptr->backup_item_ptr = NULL;
            }
          }
          /*-----------------------------------------------------------------
            Call the phys link handler to convey SDB status to upper layers.
          -----------------------------------------------------------------*/
          phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();
          ps_phys_link_dos_ack_ind (phys_link_ptr, sock_id, &ack_status);
        }
        else
        {
          DATA_IS707_MSG0(MSG_LEGACY_LOW, "rxed Non-SDB DB event:Ignoring");
        }
        break;

      default:
        DATA_IS707_MSG1(MSG_LEGACY_MED, "Unreg Function called for %d event",event);
        break;
    }
} /* ds707_sdb_DBM_event_handler() */


/*===========================================================================
FUNCTION      ds707_sdb_clone_dsm_item

DESCRIPTION   To clone a dsm item.

DEPENDENCIES  None.

RETURN VALUE  True: success         
              False: fail

SIDE EFFECTS  None.
===========================================================================*/
LOCAL boolean ds707_sdb_clone_dsm_item(
                                  dsm_item_type *src_item_ptr,
                                  dsm_item_type       **copied_item_ptr
                                  )
{
  uint16  len = 0;
  uint32  copied_len = 0;

  len = (uint16) dsm_length_packet(src_item_ptr);
  if ((NULL == src_item_ptr) || (NULL == copied_item_ptr) || (0 == len))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ds707_sdb_copy_dsm_item input error: "
                    "src_item_ptr or copied_item_ptr is NULL or len = 0");
    return FALSE;
  }

  *copied_item_ptr = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL);
  if(NULL == *copied_item_ptr)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "ds707_sdb_copy_dsm_item dsm_new_buffer fails!");
    return FALSE;
  }

  copied_len = dsm_pushdown(copied_item_ptr, src_item_ptr->data_ptr, len, 
               (int)DSM_DS_SMALL_ITEM_POOL);
  if(copied_len!=len)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "ds707_sdb_copy_dsm_item copied_len!=len");
    dsm_free_packet(copied_item_ptr);
    return FALSE;
  }
  return TRUE;
}
/*===========================================================================
FUNCTION      DS707_SDB_PROCESS_TXQ

DESCRIPTION   Return true if we can do send the SDB packet or if there is
              nothing to do otherwise returns false saying that the packet
              needs to be sent using RLP3.

DEPENDENCIES  None.

RETURN VALUE  True/False.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_sdb_process_txq(const ds707_flow_type *flow_ptr)
{
  ds707_radio_transport_in_dorm_type radio_transport;
  dsm_watermark_type         *tx_wm_ptr;/* Watermark pointer to take data  */
  dsm_item_type              *item_ptr; /* Packet to be sent               */
  ds1x_meta_info_type         meta_info;
  uint16                      meta_size;
  byte route                 = 0;
  boolean set_sig            = FALSE;   /* Set flag to reorig dormant call */
  boolean ret_val            = FALSE;   /* Need to send pkt using RLP3     */
  uint8 link_flow_number;
  
  /* check which route has packets to transmit */
  for (route = 0; route < DSRSP_NUM_ROUTES; ++route) 
  {
    tx_wm_ptr = &flow_ptr->tx_wm_list[route];  
    if (tx_wm_ptr->current_cnt > 0) 
    {
      break;
    }
  }
  if (route == DSRSP_NUM_ROUTES) 
  {
    /* Should not happen. May be better to return TRUE here? */
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "No packets to transmit on SDB/DoS");
    return ret_val;
  }

  DATA_IS707_MSG3(MSG_LEGACY_MED, "WM:0x%x has %d packets for route:%d",
                  tx_wm_ptr, tx_wm_ptr->q_ptr->cnt, route);

  #ifdef FEATURE_HDR_QOS
    /*
     * Before sending SDB, it needs to check if RLP is inited.
     * If so, can not send SDB. Instead, it will send over RLP.
     */
    if ( ds707_sec_pkt_is_qos_available_on_system ())
    {
      /* if it is QOS system, we need to do further check */
      if ( ds707_sec_pkt_is_rlp_inited( flow_ptr,
                                        DSRLP_VER_HDR_MULTIFLOW_REVERSE
                                        ) == TRUE )
      {
          /*
           * Since RLP is inited, we can not send
           * DOS message here. RLP PCP will pick the data and
           * send to RLP Mac layer
           */
        DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                        "RLP is inited. Can not send SDB");
        return FALSE;
      }
    }
  #endif /* FEATURE_HDR_QOS */

  /* No more than one packet can be queued up as DS has higher 
   * priority than PS
   */
  if (tx_wm_ptr->q_ptr->cnt > 1)
  {
    DATA_IS707_MSG3(MSG_LEGACY_ERROR, "WM:0x%x has %d packets for route:%d - "
                                      "Cannot send more than 1 pkt over SDB", 
               tx_wm_ptr, tx_wm_ptr->q_ptr->cnt, route);

    return FALSE;
  }

  /* Retrieve the packet without dequeuing it */
  if((item_ptr = (dsm_item_type *)q_check(tx_wm_ptr->q_ptr)) == NULL)
  {
    /* Usually should not happen. May be better to return TRUE here? */
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "No items in Transmit Queue");
    return ret_val;
  }

  /* If no meta_info send it over RLP */
  if (item_ptr->kind != DS1X_META_INFO_KIND) 
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                    "Kind filed does not indicate presence of meta_info");
    return ret_val;
  }

  /* Retrieve the meta_info */
  meta_size = dsm_extract(item_ptr, 0, &meta_info, sizeof(ds1x_meta_info_type));
  /* Should not fail, so asserting */
  ASSERT(meta_size == sizeof(ds1x_meta_info_type));

  if (((uint32)meta_info.ds1x_meta_kind & (uint32)DS1X_META_SDB_KIND) != 
                                (uint32)DS1X_META_SDB_KIND)
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, 
                    "Kind filed does not indicate SDB kind. Returning");
    return ret_val;
  }

  /* Make sure the packet is there*/
  ASSERT (item_ptr != NULL);

#ifdef FEATURE_HDR_DOS  
  /* 
   * Before sending SDB, it needs to check if RLP is inited. 
   * If so, can not send SDB. Instead, it will send over RLP.
   */
  if ( ds707_sec_pkt_is_qos_available_on_system ())
  {
    /* if it is QOS system, we need to do further check */
    if ( ds707_sec_pkt_is_rlp_inited( flow_ptr,
                                      DSRLP_VER_HDR_MULTIFLOW_REVERSE
                                      ) == TRUE )
    {
        /* 
         * Since RLP is inited, we can not send
         * DOS message here. RLP PCP will pick the data and 
         * send to RLP Mac layer
         */
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "RLP is inited. Can not send SDB");
      return FALSE;
    }
  }
#endif /* FEATURE_HDR_DOS */
  /*-------------------------------------------------------------------------
    Look at the Meta info contents and decide if the data should go as SDB 
    or Re-originate a call. If packet wants to go on SDB, then make next 
    check. Otherwise, return to originate a RLP call.

    Then look if the current DS scenario can support SDB call. If yes,
    then make SDB else make a RLP call.
  -------------------------------------------------------------------------*/
  rex_enter_crit_sect(&ds707_sdb_process_txq_crit_sect);
  if((ds707_pkt_is_dormant(ds707_pkt_get_pri_phys_link_ptr()) == TRUE) &&
        (tx_wm_ptr->current_cnt > 0))
  {
    /*---------------------------------------------------------------------
    Remove packet out of the water mark and give it to the SDB.
  ---------------------------------------------------------------------*/
    item_ptr = (dsm_item_type *)dsm_dequeue(tx_wm_ptr);
    if(item_ptr == NULL)
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, "item_ptr NULL: can not do SDB");
    }
  }
  rex_leave_crit_sect(&ds707_sdb_process_txq_crit_sect);

  if(item_ptr != NULL)
  {
    radio_transport = ds707_sdb_radio_transport_in_dorm (
                      &(meta_info.info.sdb), &set_sig, flow_ptr->ps_flow_ptr);
#ifdef FEATURE_QCHAT 
    /*---------------------------------------------------------------------
    If the packet is QCHAT packet, QCHAT module will check whether the msg 
    can be send in DOS on AC or on R-TCH, and set the radio transport type 
    accordingly. 
    ---------------------------------------------------------------------*/
    if (((meta_info.info.sdb.tx_flags & MSG_WAKEUP) == MSG_WAKEUP) &&
         qchatupk_data_mod_enabled()) 
    { 
      radio_transport = qchatupk_ds707_sdb_set_radio_transport(
                              radio_transport, dsm_length_packet(item_ptr));
    } 
#endif /* FEATURE_QCHAT */     
    DATA_IS707_MSG1(MSG_LEGACY_MED, "radio_transport : %d", radio_transport);

    switch(radio_transport)
    {
      case RADIO_TRANSPORT_SDB:
      
        meta_size = dsm_pullup(&item_ptr, &meta_info, sizeof(ds1x_meta_info_type));
        ASSERT(meta_size == sizeof(ds1x_meta_info_type));
        ASSERT(item_ptr != NULL); //We come here only if its not NULL.

        /*-------------------------------------------------------------------
            handle this packet. In case something fails inside 
            don't bother but it should not fail.
        -------------------------------------------------------------------*/
        link_flow_number = flow_ptr->rev_tc_ptr->rev_rlp.flow.flow;
        ds707_sdb_tx_handler(item_ptr, link_flow_number, route, tx_wm_ptr, &meta_info);
        if (TRUE == set_sig)
        {
             /* We do not want reorigination now */
        }
        ret_val = TRUE;

#ifdef FEATURE_DS_EPZID_HYSTERESIS
        /* Inform PZID hysteresis processor of data ready state */
        ds707_epzid_hyst_send_cmd(DS_CMD_707_PKT_EPZID_HYS_SDB_DATA);
#endif /* FEATURE_DS_EPZID_HYSTERESIS */
  
        break;

       case RADIO_TRANSPORT_RLP3:
       default:
         /* Put the DSM item back on the watermark */
       dsm_enqueue(tx_wm_ptr, &item_ptr);
         ret_val = FALSE;
         break;
    }
  }
  return (ret_val);
} /* ds707_sdb_process_txq() */

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
)
{
  uint8 i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  rex_init_crit_sect  (&ds707_sdb_process_txq_crit_sect);

  if( sr_id_index < DSRLPI_MAX_SRID_INDEX )
  {
    ds707_sdb_info.sdb_cfg[sr_id_index].tx_wm_ptr   = tx_watermark_ptr;
    ds707_sdb_info.sdb_cfg[sr_id_index].rx_func_ptr = post_rx_func_ptr;
    ds707_sdb_info.sdb_cfg[sr_id_index].rx_q_ptr    = post_rx_q_ptr;

    /* allocate HDLC instance for DOS */
    hdlc_init_config(&(ds707_sdb_info.hdlc_config));
    ds707_sdb_info.hdlc_inst_ptr = 
      hdlc_alloc_instance(&(ds707_sdb_info.hdlc_config));

    /* Initalize the SDB free queue */
    (void)q_init(&ds707_sdb_data_free_q);
    for(i = 0; i<DS707_MAX_SDB_PKT; i++)
    {
      (void) q_link((void *)&ds707_sdb_free_bufs[i], &ds707_sdb_free_bufs[i].link);
       q_put(&ds707_sdb_data_free_q, &ds707_sdb_free_bufs[i].link);
    }
  }
  else
  {
    DATA_IS707_MSG1(MSG_LEGACY_HIGH, "Cannot initialize SDB, sr_id_index %d",
                    sr_id_index);
  }
} /* ds707_sdb_init */


/*===========================================================================
                             OTHER FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_SDB_IS_REACH_SUPPORTED

DESCRIPTION   Returns TRUE if REACH supported else FALSE.

DEPENDENCIES  None.

RETURN VALUE  True/False.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL boolean ds707_sdb_is_reach_supported_or_is_hdr
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_HDR_QOS
#ifdef FEATURE_HDR_DOS  
    if (ds707_sec_pkt_is_qos_available_on_system() == TRUE)
	{
	  DATA_IS707_MSG0(MSG_LEGACY_HIGH, "QoS is available");
	  return TRUE;
	}
#endif /*FEATURE_HDR_DOS */
#endif /*FEATURE_HDR_QOS */

#ifdef FEATURE_IS2000_REL_A
    if (OVHD_CHAN_IS_BCCH)
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, "REACH Supported");
      return TRUE;
    }
    else
    {
      return FALSE;
    }
#else
    {
      return FALSE;
    }
#endif
} /* ds707_sdb_is_reach_supported_or_is_hdr() */


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
  const ds1x_meta_sdb_type *meta_info_ptr
)
{
  boolean           pkt_needs_SDB  = FALSE; /* flag:Indicates SDB/DoS needed   */  

  /*-------------------------------------------------------------------------
    See if it the meta_info is SDB releated. If yes, we can process further.
    If the tx_flags are 0, it means we have no SDB information. So, this
    packet does not need SDB transport.
    Look if ACH flag is set or Rev-Enhanced Access Channel flag is set.
    
    Note that the meta_info is not yet pulled out of the dsm item (it was
    copied in the extract).
  -------------------------------------------------------------------------*/
  if (meta_info_ptr->tx_flags == 0)
  {
    pkt_needs_SDB = FALSE;
    DATA_IS707_MSG0(MSG_LEGACY_MED,"Return False as tx_flags is 0");
    return pkt_needs_SDB;
  }

  if((meta_info_ptr->tx_flags & MSG_FAST_EXPEDITE) == MSG_FAST_EXPEDITE)
  {
    /*-----------------------------------------------------------------------
      Check if REACH is supported.
    -----------------------------------------------------------------------*/
    DATA_IS707_MSG0(MSG_LEGACY_MED,"Tx_flags has MSG_FAST_EXPEDITE set");

    if (ds707_sdb_is_reach_supported_or_is_hdr() == TRUE)
    {
      pkt_needs_SDB = TRUE;
      DATA_IS707_MSG0(MSG_LEGACY_MED,"Return True");
      return pkt_needs_SDB;
    }
    else
    {
      DATA_IS707_MSG0(MSG_LEGACY_MED,"Return False");
      pkt_needs_SDB = FALSE;
    }
  }
  if ((meta_info_ptr->tx_flags & MSG_EXPEDITE) == MSG_EXPEDITE)
  {
    pkt_needs_SDB = TRUE;
    DATA_IS707_MSG0(MSG_LEGACY_MED,"Return True as tx_flags has MSG_EXPEDITE set");   
    return pkt_needs_SDB;
  }

  DATA_IS707_MSG1(MSG_LEGACY_MED,"Return %d", pkt_needs_SDB);
  return pkt_needs_SDB;

} /* ds707_sdb_curr_pkt_needs_SDB() */

#ifdef FEATURE_HDR
#ifdef FEATURE_HDR_QOS
#ifdef FEATURE_HDR_DOS
/*===========================================================================
FUNCTION      DS707_IS_SDB_DOS_SUPPORTED_NOW

DESCRIPTION   Confirm if atleast one of the Sec flows /Default flow Supports SDB/DOS. 
            
DEPENDENCIES  None.

RETURN VALUE  TRUE:  If SDB is supported in current DS state.(Depends on SO)
              FALSE: If SDB is not supported in current DS state.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_is_SDB_DOS_supported_now
(
  uint32 flags
)
{
  ps_phys_link_type        *phys_link_ptr = NULL;
  ps_flow_type             *ps_flow_ptr;/* Pointer to the PS Flow */
  ds707_flow_type          *ds_flow_ptr = NULL;
  uint8                     rlpid;
  uint8                     i;
  boolean                   can_do_sdb = FALSE;
  boolean                   allowed = FALSE;
  boolean                   dos_allowed = FALSE;
  ps_iface_type             *iface_ptr = NULL;

  for(i=0; i<(DS707_NUM_FWD_PS_FLOWS + DS707_NUM_REV_PS_FLOWS);i++)
  {
    ds_flow_ptr = ds707_get_flow_pool_ptr(i); /* start from default flow ptr */
    /*-----------------------------------------------------------------------
        If a Flow is set to be Invalid, then it is a free entry
        -----------------------------------------------------------------------*/
    if( ds_flow_ptr->ds_flow_state == DS707_DS_FLOW_STATE_FREE)  
    {
      continue;
    }
    ps_flow_ptr = ds_flow_ptr->ps_flow_ptr;
    if(ps_flow_ptr == NULL)
    {
      continue;
    }
   
    /* Check if DoS is allowed for the given RLP */
    if (ds707_sec_pkt_rlp_id ( ps_flow_ptr, DSRLP_VER_HDR_MULTIFLOW_REVERSE, 
                               &rlpid) == FALSE )
    {
      continue;
    }
    if (hdrmrlpcif_get_dos_allowed_attrib (rlpid, &dos_allowed) != E_SUCCESS)
    {
      continue;
    }
  
    if (dos_allowed == FALSE)
    {
      DATA_IS707_MSG1( MSG_LEGACY_ERROR,
                       "DoS Allowed Attrib for rlpid : %d is 0", rlpid );
      continue;
    }

    /* If RLP is not active cannot do DoS */
    if (ds707_sec_pkt_is_rlp_active(ps_flow_ptr, DSRLP_VER_HDR_MULTIFLOW_REVERSE) != TRUE)
    {
      continue;
    }
    iface_ptr = PS_FLOW_GET_IFACE(ps_flow_ptr);
     /* For QoS flows, if Reservation is not active, cannot do DoS */
    if (PS_IFACE_GET_DEFAULT_FLOW(iface_ptr)!= ps_flow_ptr &&
        PS_FLOW_GET_STATE (ps_flow_ptr) != FLOW_ACTIVATED)
    {
      DATA_IS707_MSG0( MSG_LEGACY_ERROR,"PS flow reservation is not active" );
      continue;
    }
    /* Retrieve phys_link_ptr for checking dormancy next */  
    phys_link_ptr = ((ds707_flow_type*)
                     (ps_flow_ptr->client_data_ptr))->rev_tc_ptr->ps_phys_link_ptr;

    /* Make sure phys_link_ptr has been assigned before */
    if( phys_link_ptr == NULL )
    {
      DATA_IS707_MSG0( MSG_LEGACY_ERROR,"Null Physical Link Pointer" );
      return can_do_sdb;
    }
  
    /* If physlink is not dormant cannot do SDB/DoS. This is not a standard 
        * requirement, only our AMSS implementation.
        */
    if ( ds707_pkt_is_dormant(phys_link_ptr) != TRUE )
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH,
                       "Cannot do SDB/DoS as call state is not dormant" );
      continue;
    }
    allowed = TRUE;
    break;
  }/* end of for loop */ 

  if(allowed == FALSE)
  {
    return can_do_sdb;
  }
  /* once we reach here, it means one of the sec flows does support
   * SDB / DOS ,
   * its flow /reservation/ rlp is active and bound
   * it is dormant 
   */
   /*-------------------------------------------------------------------------
       Now check if the FLAGS are supported in current call.
    -------------------------------------------------------------------------*/
  if((flags & MSG_EXPEDITE) == MSG_EXPEDITE)
  {
    /*-------------------------------------------------------------------
         Nothing need to be done for ACH case.  Return TRUE.
        -------------------------------------------------------------------*/
    can_do_sdb = TRUE;
  }
  else if((flags & MSG_FAST_EXPEDITE) == MSG_FAST_EXPEDITE)
  {
   /*-------------------------------------------------------------------
       Check if Reverse Enhanced Access channel is supported.
      -------------------------------------------------------------------*/
    if (ds707_sdb_is_reach_supported_or_is_hdr() == TRUE)
    {
      can_do_sdb = TRUE;
    }
    else
    {
      can_do_sdb = FALSE;
    }
  }
  else
  {
    /*-------------------------------------------------------------------
     Should be caught in the input parameters check. If we got this far,
     got to check the code again.  Play safe and return FALSE.
    ------------------------------------------------------------------*/
    DATA_IS707_MSG0( MSG_LEGACY_HIGH,"Unknown FLAGS given for SDB" );
    can_do_sdb = FALSE;
  }

  return can_do_sdb;
}/* end of ds707_is_SDB_DOS_supported_now */
#endif /* FEATURE_HDR_DOS */
#endif /* FEATURE_HDR_QOS */
#endif /* FEATURE_HDR */

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
                 If(previous data call  == SO 33 && mobile is dormant on CDMA)
                   Return SDB
                 Else // mobile is dormant on HDR || previous SO is not 33
                   Return RLP

DEPENDENCIES  None.

RETURN VALUE  TRUE:  If SDB is supported in current DS state.(Depends on SO)
              FALSE: If SDB is not supported in current DS state.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_sdb_supports_SDB_now
(
  uint32            flags,
  ps_flow_type      *flow_ptr
)
{
  boolean           can_do_sdb = FALSE;
  ps_phys_link_type *phys_link_ptr = NULL;
  uint8             rlpid;
  boolean           dos_allowed = FALSE;
  ds707_flow_type   *ds_flow_ptr = NULL;
  ps_iface_type     *iface_ptr = NULL;

  /*-------------------------------------------------------------------------
    Input Sanity Check
    If we get unknown flags, then return FALSE
  -------------------------------------------------------------------------*/
  DATA_IS707_MSG1(MSG_LEGACY_MED, "ds707_sdb_supports_SDB_now, %x", flags); 

  if  (ds707_pkt_get_current_data_sess_network() == SYS_SYS_MODE_CDMA)      
  {
      /* Processing for SDB */

      /* If previous data call  is not SO 33, cannot do SDB */
    ds707_sdb_prev_data_call_so = ds707_pkt_get_prev_call_so();
    if (ds707_sdb_prev_data_call_so != CAI_SO_PPP_PKT_DATA_3G)
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                      "previous data call not S0 33, cannot do SDB");
      return FALSE;
    }  
      /* Retrieve the phys_link_ptr to check for dormancy later */
    phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();
  }
#ifdef FEATURE_HDR
#ifdef FEATURE_HDR_QOS
#ifdef FEATURE_HDR_DOS
  else if (ds707_pkt_get_current_data_sess_network() == SYS_SYS_MODE_HDR
#ifdef FEATURE_DATA_OPTHO
           && ds707_s101_query_sm_state() == DS707_S101_SM_NON_TUNNEL
#endif /* FEATURE_DATA_OPTHO */
          )  
  {
      /* Processing for DoS */
      
      /* No DoS for DPA */
      if (ds707_sec_pkt_is_qos_available_on_system () != TRUE)
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, "QoS unavailable");
        return FALSE;
      }      
      
      if (PS_FLOW_IS_VALID(flow_ptr) == FALSE)
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                        "Null Flow Ptr passed. Check all the Ps/DS_flows");
        can_do_sdb = ds707_is_SDB_DOS_supported_now(flags);
        return can_do_sdb;
      }
      else
      {
        /* Check if DoS is allowed for the given RLP */
        if (ds707_sec_pkt_rlp_id ( flow_ptr, DSRLP_VER_HDR_MULTIFLOW_REVERSE, 
                                 &rlpid) == FALSE )
        {
          DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Cannot get RLP ID from flow ptr");
          return FALSE;
        }
        if (hdrmrlpcif_get_dos_allowed_attrib(rlpid, &dos_allowed) != E_SUCCESS)
        {
          DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
                        "Cannot get DoS Allowed Attrib for rlpid : %d", rlpid);
          return FALSE;
        }
  
        if (dos_allowed == FALSE)
        {
          DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
              "DoS Allowed Attrib for rlpid : %d is 0. Cant send DoS", rlpid);
          return FALSE;
        }
  
        /* If RLP is not active cannot do DoS */
        if (ds707_sec_pkt_is_rlp_active(flow_ptr, 
                                      DSRLP_VER_HDR_MULTIFLOW_REVERSE) != TRUE)
        {
          DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                          "RLP is not activated. Cant send DoS");
          return FALSE;
        }

        ds_flow_ptr = ds707_pkt_mgr_get_ds_flow( flow_ptr );
        if(ds_flow_ptr == NULL)
        {
          DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Null ds_flow for ps_flow %x, ver", flow_ptr);
          return FALSE;
        }

        /* if RLP is inited, it cannot do DOS */
        if ( ds707_sec_pkt_is_rlp_inited( ds_flow_ptr,
                                        DSRLP_VER_HDR_MULTIFLOW_REVERSE
                                        ) == TRUE )
        {
          DATA_IS707_MSG0(MSG_LEGACY_ERROR, "RLP is inited. Cant send DoS");
          return FALSE;
        }
        iface_ptr = PS_FLOW_GET_IFACE(flow_ptr);
        
        /* For QoS flow, if Reservation is not active, cannot do DoS */
        if (PS_IFACE_GET_DEFAULT_FLOW(iface_ptr)!= flow_ptr &&
            PS_FLOW_GET_STATE (flow_ptr) != FLOW_ACTIVATED)
        {
          DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                          "PS flow reservation is not active, cannot do DoS");
          //return FALSE;
          /* If sec flow is not active, send the data on BE */
        }
            
        /* Retrieve phys_link_ptr for checking dormancy next */
        phys_link_ptr = ((ds707_flow_type*)(flow_ptr->client_data_ptr))->rev_tc_ptr->ps_phys_link_ptr;
      }
  }
#endif /* FEATURE_HDR_DOS */
#endif /* FEATURE_HDR_QOS */
#endif /* FEATURE_HDR     */

  /* Make sure phys_link_ptr has been assigned before */
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "Cannot do SDB/DoS as sys mode is not CDMA/HDR");
    return FALSE;
  }

  /* If physlink is not dormant cannot do SDB/DoS. This is not a standard 
   * requirement, only our AMSS implementation.
   */
  if ( ds707_pkt_is_dormant(phys_link_ptr) != TRUE )
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "Cannot do SDB/DoS as call state is not dormant");
    return FALSE;
  }

  if((flags & MSG_EXPEDITE) == MSG_EXPEDITE)
  {
    /*-------------------------------------------------------------------
      Nothing need to be done for ACH case.  Return TRUE.
    -------------------------------------------------------------------*/
    can_do_sdb = TRUE;
  }
  else if((flags & MSG_FAST_EXPEDITE) == MSG_FAST_EXPEDITE)
  {
    /*-------------------------------------------------------------------
      Check if Reverse Enhanced Access channel is supported.
    -------------------------------------------------------------------*/
     if (ds707_sdb_is_reach_supported_or_is_hdr() == TRUE)
     {
       can_do_sdb = TRUE;
     }
     else
     {
       can_do_sdb = FALSE;
     }  
  }
  else
  {
    /*-------------------------------------------------------------------
      Should be caught in the input parameters check. If we got this far,
      got to check the code again.  Play safe and return FALSE.
    -------------------------------------------------------------------*/
     DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Unknown FLAGS given for SDB");
     can_do_sdb = FALSE;  
  }

  DATA_IS707_MSG1(MSG_LEGACY_MED," return %d", can_do_sdb);
  return can_do_sdb;

} /* ds707_sdb_supports_SDB_now() */

/*===========================================================================
FUNCTION      DS707_SDB_CM_TX_ERR_STATUS

DESCRIPTION   This call back function will be called by CM if it has some
              thing to inform DS for bad SDB transmissions.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_sdb_cm_tx_err_status
(
  const void   *data_block_ptr,         /* Pointer to client provided data block */
  cm_dbm_cmd_e_type dbm_cmd, /*Indicate which pd cmd this err status is for*/
  cm_dbm_cmd_err_e_type dbm_cmd_err                /* Reason for rejection */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG0(MSG_LEGACY_ERROR, "CM could not transmit SDB data");
} /* ds707_sdb_cm_tx_err_status() */

/*===========================================================================
FUNCTION      DS707_SDB_SR_ID_TO_IDX

DESCRIPTION   This function will return the index to SDB configuration based
              on SR_ID.

DEPENDENCIES  None.

RETURN VALUE  Value of the index.
              NOTE: If index value is equal(or greater) to DS_NUM_SDB, then
              the caller should reject it.

SIDE EFFECTS  None.
===========================================================================*/
byte ds707_sdb_sr_id_to_idx
(
  byte sr_id
)
{
  byte sdbi_index = 0;       /* i is used to identify it as local variable */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Search if we have any sr_id saved. If yes, return the index.
    NOTE: This may not be same as RLP. So, may not be good to
    couple RLP sr_id_index with this.
  -------------------------------------------------------------------------*/
  for(sdbi_index = 0; sdbi_index < DS707_SDB_NUM; sdbi_index++)
  {
    if (ds707_sdb_info.sdb_cfg[sdbi_index].sr_id == sr_id)
    {
      break;
    }
  }
  return(sdbi_index);
} /* ds707_sdb_sr_id_to_idx() */

/*===========================================================================
FUNCTION  DS707_SDB_CM_DBM_CB

DESCRIPTION
  This is a call back function passed to CM when calling cm_dbm_cmd_mo_msg2

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
LOCAL void ds707_sdb_cm_dbm_cb 
(
  void                        *data_block_ptr,
        /**< Pointer to the client-provided data block. */
  cm_dbm_cmd_e_type           dbm_cmd,
        /**< Indicates which DBM command the dbm_cmd_err error status is for. */
  cm_dbm_cmd_err_e_type       dbm_cmd_err
        /**< Reason for the rejection. */
)
{
  DATA_IS707_MSG2( MSG_LEGACY_HIGH, 
                   "ds707_sdb_cm_dbm_cb() CMD %d, CMD Err %d ",
                   dbm_cmd, 
                   dbm_cmd_err );

  if (( dbm_cmd_err != CM_DBM_CMD_ERR_NOERR ) &&
      ( dbm_cmd_err != CM_DBM_CMD_ERR_NONE ))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "ds707_sdb_cm_dbm_cb() got error. Not sending DoS / SDB");
    if ( data_block_ptr )
    {
      if(((ds707_sdb_data_block_s_type *)data_block_ptr)->backup_item_ptr!=NULL)
      {
        dsm_free_packet(&(((ds707_sdb_data_block_s_type *)data_block_ptr)->backup_item_ptr));
        ((ds707_sdb_data_block_s_type *)data_block_ptr)->backup_item_ptr = NULL;
      }
      /* Reclaim the memory */
      q_put(&ds707_sdb_data_free_q, &(((ds707_sdb_data_block_s_type *)data_block_ptr)->link));
    }
    else
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                      "ds707_sdb_cm_dbm_cb() NULL data block ptr");
    }
  }
} /* ds707_sdb_cm_dbm_cb */


/*===========================================================================
FUNCTION      DS707_SDB_TX_HANDLER

DESCRIPTION   Function that builds the SDB functionality.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_sdb_tx_handler
(
  dsm_item_type     *item_ptr,
  uint8              link_flow_number,
  byte               route,
  dsm_watermark_type        *tx_wm_ptr,
  const ds1x_meta_info_type  *meta_info_ptr
)
{
  byte              msg_buf_index = 0;   
  byte              sdb_ctl_byte = 0x0; 
  byte              sdb_hdr_buf[3];         /* buffer to hold the SDB hdr  */
  uint16            sdb_size;             
  uint32            flags = (uint32)CM_DBM_TRANSMIT_IN_CURR_PROTOCOL_STATE; 
  uint8             l2_ack_wait_time,l3_ack_wait_time;            
  ds707_sdb_data_block_s_type *data_block;
  cm_dbm_cmd_info_s_type      dbm_cmd_info;
  cm_client_id_type           ds3g_cm_client_id;
  dsm_item_type               *backuped_item_ptr;
  ds1x_meta_info_type         backuped_meta_info;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - -*/
#ifdef FEATURE_QCHAT 
  uint32 sdb_tx_flags; 
#endif   
  DATA_IS707_MSG2(MSG_LEGACY_MED, 
                  "ds707_sdb_tx_handler called link_flow_number: %d route: %d",
                  link_flow_number, route);
  /*-------------------------------------------------------------------------
    Input check
  -------------------------------------------------------------------------*/
  if(NULL==item_ptr || NULL==meta_info_ptr)
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "DoS ds707_sdb_tx_handler illegal input, "
                    "item_ptr or meta_info_ptr is NULL");
    return;
  }

  /*-------------------------------------------------------------------------
    Tx Handler should fill up the message buffer with the data to be
    transmitted on the SDB.
    If the Service Option == 33, then SO Omit field = 1; Data starts
    from 2nd byte
    If the Service Option != 33, then SO omit field = 0, fill SO in
    2,3rd bytes; Data starts at byte 4 in the message buffer.
    NOTE: FOR NOW WE DONOT SUPPORT SO != 33.
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
    Initializations. This will be written into first byte of the msg_buf
  -------------------------------------------------------------------------*/
  sdb_ctl_byte = 0x0; /* clear the first byte */

  /*-------------------------------------------------------------------------
    Confirm that the service option before we went dormant is SO 33.

    If (previous data call is SO 33)
      Use the SO omit field with 1 and build the SDB
    Else
      We donot support Non-SO 33 calls to have SDB.

    Note: Update the variable ds_prev_so whenever a call is connected from
    CM. Hence, this variable is called only in dormancy.
   ------------------------------------------------------------------------*/
   if (((ds707_pkt_get_current_data_sess_network() == SYS_SYS_MODE_CDMA)
         && (ds707_pkt_get_prev_call_so() == CAI_SO_PPP_PKT_DATA_3G))
#ifdef FEATURE_HDR
#ifdef FEATURE_HDR_QOS
#ifdef FEATURE_HDR_DOS
	   || (ds707_sec_pkt_is_qos_available_on_system() == TRUE)
#endif /* FEATURE_HDR_DOS */
#endif /* FEATURE_HDR_QOS */
#endif /* FEATURE_HDR     */
     )
   {
     /*-------------------------------------------------------------------------
       Clone the dsm item for backup, this cloned item will be recorded in
      ds707_sdb_data_free_q. Note that, the item_ptr has no meta info, the meta
      info has already pulled up in function ds707_sdb_process_txq(...), so we
      need to push down meta info later.
     -------------------------------------------------------------------------*/
     if(!ds707_sdb_clone_dsm_item(item_ptr, &backuped_item_ptr))
     {
       DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ds707_sdb_clone_dsm_item fails!");
       return;
     }
     /*-------------------------------------------------------------------------
       to clear the MSG_EXPEDITE or MSG_FAST_EXPEDITE flag, since the backuped
       packet is used for re-sending on TC
     -------------------------------------------------------------------------*/
     memscpy(&backuped_meta_info, sizeof(ds1x_meta_info_type), 
             meta_info_ptr, sizeof(ds1x_meta_info_type));
     backuped_meta_info.info.sdb.tx_flags &= !(MSG_EXPEDITE|MSG_FAST_EXPEDITE);
     /*-------------------------------------------------------------------------
       push the modified meta info into backuped item
     -------------------------------------------------------------------------*/
      backuped_item_ptr->kind = DS1X_META_INFO_KIND;
     (void)dsm_pushdown_packed(&backuped_item_ptr,
                               &backuped_meta_info,
                               sizeof(ds1x_meta_info_type),
                               DSM_DS_SMALL_ITEM_POOL);
     DATA_IS707_MSG0(MSG_LEGACY_MED, "Attempting DoS / SDB");
     /*----------------------------------------------------------------------
       The SDB structure now looks as:
        ______________________________________________________________________
       |  1st Byte               | 2nd byte | 3rd byte| 2nd byte(if SO_OMIT=1) 
       |                         |          |         | 4th byte(if SO_OMIT=0)
       |_________________________|__________|_________|_______________________
       |3 bits  | 1 bit | 4 bits |  16 optional bits  |          
       |________|_______|________|__________ _________|_______________________
       |sr_id   |SO_Omit| Rsvd   |  Service Option    | PPP Data
       |________|_______|________|____________________|_______________________

       Copy the sr_id and SO omit information into the first byte
       Shift SR_ID by 5 bits to be appended at upper position.
       Since first byte is zeroed out initially, reserved bits are already 0.
     ----------------------------------------------------------------------*/

    msg_buf_index = 1;

    /*-----------------------------------------------------------------------
      By now sdb_ctl_byte should be zero, so OR'ing it is no problem.
    -----------------------------------------------------------------------*/
     ds707_sdb_prev_data_sr_id = ds707_pkt_get_prev_sr_id();
     sdb_ctl_byte |= (byte)(ds707_sdb_prev_data_sr_id << 5);

    /*-----------------------------------------------------------------------
      If the ds707_sdb_so_omit_indicator is set via the callback registered
      with MC (either in a Service Connect/UHDM) message, set the SO OMIT
      mask in the SDB control byte
    -----------------------------------------------------------------------*/
     if (ds707_sdb_so_omit_indicator == TRUE)
     {
       sdb_ctl_byte  |=  DS707_SDB_SO_OMIT_MASK;
       sdb_hdr_buf[0] = sdb_ctl_byte;
     }
     else
     {
       sdb_hdr_buf[0] = sdb_ctl_byte;
    /*-----------------------------------------------------------------------
      Since SO_OMIT is 0, include the Service option in the SDB header. 
      Populate the Service option(2 bytes) and bump the message index by 2
    -----------------------------------------------------------------------*/
       /*lint -save -e572*/
       sdb_hdr_buf[1] = (byte)(( (uint32)CAI_SO_PPP_PKT_DATA_3G & (uint32)0xff00 ) >> 8);
       /*lint -restore*/

       sdb_hdr_buf[2] = (byte)((uint32)CAI_SO_PPP_PKT_DATA_3G & (uint32)0x00ff);
       msg_buf_index += 2;
     }

     DATA_IS707_MSG3(MSG_LEGACY_MED, 
                     "sdb_hdr[0] %x sdb_hdr[1] %x sdb_hdr[2] %x", 
                     sdb_hdr_buf[0], sdb_hdr_buf[1],  sdb_hdr_buf[2]);  
                                                       
    /*-----------------------------------------------------------------------
      Copy the SDB header to the packet
      Use dsm_pushdown to push the header on to the top.
      Note: For one byte, we are writing the entire small item. Find out
      if we could do better.
    -----------------------------------------------------------------------*/
    (void)dsm_pushdown ( &item_ptr,
                   sdb_hdr_buf,
                   msg_buf_index,
                   DSM_DS_SMALL_ITEM_POOL);
   }
   else
   {
     /* We should not have come this far; Since we came, how to handle it? 
      */
     DATA_IS707_MSG0(MSG_LEGACY_ERROR, "No support for DoS or SDB");
     return;
   }

   /*-----------------------------------------------------------------------
     Translate the flags from Sockets ENUMs to CM ENUMs.
     Sockets Flags:
           MSG_EXPEDITE
           MSG_FAST_EXPEDITE

     CM Flags:
           CM_DBM_TRANSMIT_AUTO,
           CM_DBM_TRANSMIT_ENH_ACC_OR_TRAFFIC,
           CM_DBM_TRANSMIT_ANY_OR_TRAFFIC,

     The flags are translated as below:
       SOCKETS                      CM
       MSG_EXPEDITE                 CM_DBM_TRANSMIT_IN_CURR_PROTOCOL_STATE
       MSG_FAST_EXPEDITE            CM_DBM_TRANSMIT_ENH_ACCESS_OR_TRAFFIC
       EXPEDITE | FAST_EXPEDITE     CM_DBM_TRANSMIT_IN_CURR_PROTOCOL_STATE

   Implementation:  By default make the flags as
   CM_DBM_TRANSMIT_ANY_OR_TRAFFIC. This will take care of ACH. Note that
   sanity check of the FLAGS was done elsewhere.
   -----------------------------------------------------------------------*/
#ifdef FEATURE_QCHAT 
   /*-----------------------------------------------------------------------
    Set the UPK flag in QOS based on "sdb_tx_flags" to decide whether to
    piggyback Connection Req with DOS or not.
   -----------------------------------------------------------------------*/
  sdb_tx_flags=meta_info_ptr->info.sdb.tx_flags;
  if(((sdb_tx_flags & MSG_WAKEUP) == MSG_WAKEUP) &&
      qchatupk_data_mod_enabled()) 
  {
    qchatupk_dos_setup_cm_flags(&sdb_tx_flags);
  }
  else
#endif /* FEATURE_QCHAT */
  {
    if( (meta_info_ptr->info.sdb.tx_flags & MSG_EXPEDITE) == MSG_EXPEDITE)
    {
      flags = (uint32)CM_DBM_TRANSMIT_IN_CURR_PROTOCOL_STATE;
    }
    else if ((meta_info_ptr->info.sdb.tx_flags & MSG_FAST_EXPEDITE) == MSG_FAST_EXPEDITE)
    {
      flags = (uint32)CM_DBM_TRANSMIT_ENH_ACCESS_OR_TRAFFIC;
    }
    else
    {
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, "unknown SDB flags %D. Assume ACH", 
                      meta_info_ptr->info.sdb.tx_flags);
    }
  }

   /*-----------------------------------------------------------------------
      Currently we donot have support for wait time for l2_ack. We may need
      a socket  option later to decide the wait time.
      l3_ack.. is of no use for SDB. Set it so that CM is happy.
   -----------------------------------------------------------------------*/
   l2_ack_wait_time = 15; //This value is given by CM. Enquire them for now.
   l3_ack_wait_time = 0;

   /*-----------------------------------------------------------------------
     Obtain the length of the SDB packet
   -----------------------------------------------------------------------*/
   sdb_size = (uint16)dsm_length_packet( item_ptr);

   /*-----------------------------------------------------------------------
       Write the data to the CM api with the following parameters:
        Burst_type =    SDB. CAI_SHORT_DATA_BURST
        Cmd_cb_func =   ds_sdb_tx_err_status_fn; This function is called
                        whenever CM has some errors. If CM has errors, DS
                        does not care and expects that the upper application
                        will know it.
                        For our debugging ease, we write this function to
                        printout some messages on the DM screen.
        Data_block_ptr: This contains the SOCK_ID. This sock id is obtained
                        from the socket
        Client:         DS client.
        Payload:        Pointer to the array that contains the outgoing data.
                        We give the dsm_item_ptr and expect CM to interpret
                        it. We provide a function that will copy from DSM
                        to CM buffers.
        NumBytes:       Size of the data. This is same used field in the
                        item ptr.
        payload_copy_func: This function is provided by DS to copy the data
                        from DSM items to CM buffer. This is provided because
                        CM does not understand DSM structure.
                        Pointer to callback function to copy payload from
                        client's memory space to CM's memory space
        l2_ack_wait_time: This should be a FLAG that we provide again at
                          dss_sendto level?
        l3_ack_wait_time  Maximum number of seconds to wait for receiving L3 ack
                          before sending the pref sys change command
        trans_pref      Type of channel used to send the data out.
                        Transmission preference like REACH, ACH etc..
   -----------------------------------------------------------------------*/
   data_block = (ds707_sdb_data_block_s_type *) q_get(&ds707_sdb_data_free_q);

   if(data_block == NULL)
   {
     DATA_IS707_MSG1(MSG_LEGACY_ERROR, "MAX OUTSTANDING SDB PKTS %d "
                     "processing, can't process more",
                     DS707_MAX_SDB_PKT);
     (void)dsm_free_packet(&item_ptr);      
     if(backuped_item_ptr!=NULL)
     {
       dsm_free_packet(&backuped_item_ptr);
     }
     return ;
   }

   /*-----------------------------------------------------------------------
     Obtain the information from Meta Info
   -----------------------------------------------------------------------*/
   data_block->sock_id = meta_info_ptr->info.sdb.sockfd;

   /*-----------------------------------------------------------------------
     to record the backup dsm item and link flow number
   -----------------------------------------------------------------------*/
   data_block->backup_item_ptr = backuped_item_ptr;
   data_block->tx_wm_ptr = tx_wm_ptr;

#ifdef FEATURE_HDR_QOS
   dbm_cmd_info.rlp_flow = link_flow_number;                    
   dbm_cmd_info.route = route;
#endif /* FEATURE_HDR_QOS */

   dbm_cmd_info.num_bytes = sdb_size;

   if (ds707_sdb_to_cm_copy(item_ptr, dbm_cmd_info.payload, &sdb_size))
   {      
     DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                     "ds707_sdb_tx_handler: Error during copying");
     (void)dsm_free_packet(&item_ptr);      
      /* reclaim the memory */
     data_block->backup_item_ptr = NULL;
     q_put(&ds707_sdb_data_free_q, &(data_block->link));
     if(backuped_item_ptr!=NULL)
     {
       dsm_free_packet(&backuped_item_ptr);
     }
     return;
   }

   dbm_cmd_info.trans_pref = (cm_dbm_trans_pref_e_type) flags;

   DATA_IS707_MSG2(MSG_LEGACY_MED, 
                   "DoS: calling cm_dbm_cmd_mo_msg2 rlp_flow:%d route:%d", 
                    dbm_cmd_info.rlp_flow, dbm_cmd_info.route);

   ds3g_cm_client_id = ds3g_get_cm_client_id();

   if (cm_dbm_cmd_mo_msg2(CAI_SHORT_DATA_BURST,
                        ds707_sdb_cm_dbm_cb,
                        (void *)data_block,
                        ds3g_cm_client_id,
                        &dbm_cmd_info,
                        l2_ack_wait_time,
                        l3_ack_wait_time
                       ) == FALSE)
   {
     DATA_IS707_MSG0(MSG_LEGACY_ERROR, "cm_dbm_cmd_mo_msg2 returned FALSE. "
                     "Not sending DoS / SDB");
   }

   /*-----------------------------------------------------------------------
      By now we removed the packet from the watermark. Find out if any more
      data is queued up in the Watermark.
      If yes, we need to again call the empty watermark function again.
   -----------------------------------------------------------------------*/
   return;
} /* ds707_sdb_tx_handler() */


/*===========================================================================
FUNCTION      DS707_SDB_RX_HANDLER

DESCRIPTION   This function will be registered by the DS on power on. This
              function will be called by the CM when ever it obtains the 
              SDB data.  Note that only DS will register for Receiving the
              SDB data.
              On receiving the incoming data, DS should copy the incoming
              data into the queue that is registered in the SDB. Then it
              should inform PS about it by calling the function.

DEPENDENCIES  We assume that the DS state is Idle/Dormant. Just to be safe,
              we also accept incoming SDB packet.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_sdb_rx_handler
(
  uint8 *buf, 
  uint32 buf_len
)
{
  byte   sdb_ctl_byte;         /* Ctl Byte: The first byte of incoming pkt */
  word   sdb_so;               /*                           Service Option */
  byte   bs_sr_id;             /*                          SR-ID of the BS */
  byte   sr_id_idx;            /*                   Index of the RLP array */
  dsm_item_type *sdb_item_ptr; /*            Item to copy incoming data to */
  byte   sdb_index = 0;        /* Index in CM buffer: Points to data start */
  ps_phys_link_type       *phys_link_ptr;            /* ptr to phys link   */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Received SDB Message:DS CB called by CM");
  /*-------------------------------------------------------------------------
    Input parameters Sanity Check
    Buf pointer: Should not be NULL
    buf_len: should not be zero.
  -------------------------------------------------------------------------*/
  if((buf == NULL) || (buf_len == 0))
  {
    DATA_IS707_MSG2(MSG_LEGACY_ERROR, "invalid inputs: buf:%x, buf_len:%d",
                    buf, buf_len);
    return;
  }

  /*-------------------------------------------------------------------------
    Sanity Check time:
    Check for correctness in incoming data. It includes:
    a) Service option of dormant data call is SO 33. For future compatibility
       we are not returning, if its not SO 33. We just accept it. Hopefully
       CM and CP levels, they donot accept SDB if its not SO 33 call.
    b) MS is currently dormant? We test that MS is dormant to receive,
       because SDB is designed mainly for that. DS will accept the data even
       if its not dormant and expects higher apps to throw it off.
    c) Sr_Id of BS and MS are same? This will be a good check to see.
  -------------------------------------------------------------------------*/
  phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();
  if ( ds707_pkt_is_dormant (phys_link_ptr) != TRUE )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Rxed SDB in Non-Dormant state");
  }
  if (ds707_pkt_get_prev_call_so() != CAI_SO_PPP_PKT_DATA_3G)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Rxed SDB in Non-SO 33 call");
  }

  /*-------------------------------------------------------------------------
    The SDB structure now looks as:
     _________________________________________
    |  1st Byte                | 2nd byte on~E
    |_________________________|__________________
    |3 bits  | 1 bit | 4 bits |
    |________|_______|________|_______________
    |sr_id   |SO_Omit| Rsvd   | PPP Data
    |________|_______|________|_______________
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
    Check SR_ID between BS and MS match. Thrash if they donot match
    Obtain the first byte of incoming SDB. 
  -------------------------------------------------------------------------*/
  sdb_ctl_byte = buf[0];

  /*-------------------------------------------------------------------------
    Since SR_ID is the upper 3 bits, we should shift it by 5 bits to get the
    sr_id.
    Obtain the SR_ID_Index in the SDB_Array
  -------------------------------------------------------------------------*/
  bs_sr_id = (sdb_ctl_byte >> 5);
  ds707_sdb_prev_data_sr_id = ds707_pkt_get_prev_sr_id();
  if( ds707_sdb_prev_data_sr_id != bs_sr_id)
  {
    DATA_IS707_MSG2(MSG_LEGACY_ERROR, "BS:%d & MS:%d SR_ID mismatch",
                    ds707_sdb_prev_data_sr_id, bs_sr_id);
  }

  sr_id_idx = DSRLP_FIXED_SR_ID_INDEX;

  /*-------------------------------------------------------------------------
    Check if the SO omit field is set. Else check if the SO included in
    the SDB packet corresponds to SO33. Currently we support only for SO 33.
    If its not, flag an error stating SDB not supported for the SO
  -------------------------------------------------------------------------*/
  if ((sdb_ctl_byte & DS707_SDB_SO_OMIT_MASK) != DS707_SDB_SO_OMIT_MASK)
  {
    sdb_so = (word)buf[1]; /* INTERFACE bytes 2 & 3 */
    
    if (sdb_so != CAI_SO_PPP_PKT_DATA_3G)
    {
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, "rx'ed SDB for SO %x:Not Supported",
                      sdb_so);
    }
  }


  /*-------------------------------------------------------------------------
    Remove the SDB header from the incoming packet.
    if SO Omit field is set to ONE, then copy from 2nd byte. If its set to
    0, copy from 4th byte onwards.
  -------------------------------------------------------------------------*/
  if( (sdb_ctl_byte & DS707_SDB_SO_OMIT_MASK) == DS707_SDB_SO_OMIT_MASK)
  {
  /*-------------------------------------------------------------------------
    buf_len should be >1 to have atleast one PPP byte on the payload 
  -------------------------------------------------------------------------*/
      if ( buf_len > 1 )
      {
        buf_len--;
        sdb_index++;
      }
      else
      {
        DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Invalid size of buf_len:%d", 
                        buf_len);
        return;
      }
  }
  else
  {
  /*-------------------------------------------------------------------------
    buf_len should be >3 to have atleast one PPP byte on the payload 
  -------------------------------------------------------------------------*/
      if ( buf_len > 3 )
      {
        buf_len -= 3;
        sdb_index += 3;
      }
      else
      {
        DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Invalid size of buf_len:%d", 
                        buf_len);
        return;
      }
  }
  /*-------------------------------------------------------------------------
    Obtain a new dsm item and copy the data from the buffer to the DSM_ITEM.
    We know that SDB packet cannot be more than 256 bytes, so we may chain 
    3-4 small items. So take a SMALL item.
    This is done to pass the received data up the data stack.
  -------------------------------------------------------------------------*/
  sdb_item_ptr = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL);
  (void)dsm_pushdown(&sdb_item_ptr, &(buf[sdb_index]), (uint16)buf_len, 
               (int)DSM_DS_SMALL_ITEM_POOL);

  sdb_item_ptr->app_field = (uint32) ds707_sdb_info.hdlc_inst_ptr;
  /*-------------------------------------------------------------------------
    Give data to RLP, if call fails, data didn't go to RLP so delete the
    sdb packet
  -------------------------------------------------------------------------*/
  if (!dsrlp_queue_sdb_rx_data (sr_id_idx, sdb_item_ptr))
  {
    ERR ("Failure to queue SDB data with RLP",0,0,0);
    dsm_free_packet (&sdb_item_ptr);
  }

} /* ds707_sdb_rx_handler() */


#ifdef FEATURE_HDR_QOS
#ifdef FEATURE_HDR_DOS
/*===========================================================================
FUNCTION      DS707_DOS_RX_HANDLER

DESCRIPTION   This function will be registered by the DS on power on. This
              function will be called by the CM when ever it obtains the 
              DOS data.  Note that only DS will register for Receiving the
              DOS data.
              On receiving the incoming data, DS should copy the incoming
              data into the queue that is registered. Then it
              should inform PS about it by calling the function.

DEPENDENCIES  We assume that the DS state is Idle/Dormant. Just to be safe,
              we also accept incoming DOS packet.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_dos_rx_handler
(
  const cm_dbm_info_s_type* dbm_info_ptr
)
{
  ps_phys_link_type *phys_link_ptr = NULL; /* ptr to phys link   */

  /*-------------------------------------------------------------------------
    Input parameters Sanity Check
    buffer pointer: Should not be NULL
    buffer length: should not be zero.
  -------------------------------------------------------------------------*/
  if ((NULL == dbm_info_ptr) || (dbm_info_ptr->dos_payload == NULL) 
      || (dbm_info_ptr->dos_num_bytes == 0))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "invalid inputs");
    return;
  }

  DATA_IS707_MSG2(MSG_LEGACY_MED, 
                  "Received DOS Message in APPS - payload_ptr %0x size %d",
                  dbm_info_ptr->dos_payload, dbm_info_ptr->dos_num_bytes);

  /*-------------------------------------------------------------------------    
    MS is currently dormant? We test that MS is dormant to receive,
    because SDB is designed mainly for that. DS will accept the data even
    if its not dormant and expects higher apps to throw it off.
  -------------------------------------------------------------------------*/
  phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();
  if ( ds707_pkt_is_dormant (phys_link_ptr) != TRUE )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Rxed DOS in Non-Dormant state");
  }

  /* Route B is not supported -- Revisit when Route B is supported */
  /*-------------------------------------------------------------------------
    Route the MT-DoS packet back to MODEM to make it take the SHIM route to
    APPS
  -------------------------------------------------------------------------*/  
  ds707_sdb_mdm_rx_handler(dbm_info_ptr->dos_payload, dbm_info_ptr->rlp_flow,
                           DSRSP_ROUTE_A
                           /*dbm_info_ptr->route */);

} /* ds707_dos_rx_handler() */

#endif /* FEATURE_HDR_DOS */
#endif /* FEATURE_HDR_QOS */


/*===========================================================================
FUNCTION      DS707_SDB_TO_CM_COPY

DESCRIPTION   Copies to buffer.

DEPENDENCIES  This function assumes that any length data is valid. So, its
              upto calling functions to see if they are not happy with length
              passed here.

RETURN VALUE  -1: if there is an error (e.g. a parameter is invalid)
               0: if there were 'len' or fiewer bytes to copy from the packet
               chain number of bytes that were not copied

SIDE EFFECTS  None.
===========================================================================*/
LOCAL int8  ds707_sdb_to_cm_copy
(
  void   *dsm_item_ref_ptr,
  uint8  *buf,
  uint16 *len_ptr
)
{
  int8 ret;
  void **dsm_item_ref_ptr_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Check the input values. If they are unacceptable, just return from
    function without doing anything.
  -------------------------------------------------------------------------*/
  if(  (NULL == dsm_item_ref_ptr) || (NULL == buf) || (NULL == len_ptr))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "NULL dsm_item_ref_ptr/buf/len_ptr");
    return (-1);
  }

  if( *len_ptr > CM_DBM_MAX_PAYLOAD_SIZE )
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
                    "DoS packet size has exceeded max packet length %d",
                    *len_ptr);
    return -1;
  }

  dsm_item_ref_ptr_ptr = &dsm_item_ref_ptr;
  /*-------------------------------------------------------------------------
    Copy the data to the Buffer
  -------------------------------------------------------------------------*/
  *len_ptr -= dsm_pullup((dsm_item_type**)dsm_item_ref_ptr_ptr, (void*) buf, 
                          *len_ptr);
  ret = (int8)dsm_length_packet((dsm_item_type*)dsm_item_ref_ptr);
  dsm_free_packet((dsm_item_type**)dsm_item_ref_ptr_ptr);
  return ret;
} /* ds707_sdb_to_cm_copy() */


/*===========================================================================
FUNCTION      DS707_SDB_RADIO_TRANSPORT_IN_DORM

DESCRIPTION   To decide the type of Radio Transport Protocol for CDMA2000 
              look at the following:
              a) What transport does Packet request. If SDB it may request
                 explicitly. For RLP, packets may not specify any thing. If
                 the packet does not specify anything assume its RLP.
              b) What transport can Mobile support Now: If its dormant and
                 SO 33 call, it will be able to support SDB. Else, it will
                 only support RLP.

              Confirm that the current packet wants to go on SDB by
              looking at its Meta Info. If it does not desire SDB, return
              radio_transport as RLP and it will originate a call.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL ds707_radio_transport_in_dorm_type ds707_sdb_radio_transport_in_dorm
(
  const ds1x_meta_sdb_type *meta_info_ptr,  /* Meta info        */
  boolean           * set_sig,     /* Signal for re-orig. Currently unused */
  ps_flow_type   *flow_ptr 
)
{
  ds707_radio_transport_in_dorm_type radio_transport_ret_val 
                                               = RADIO_TRANSPORT_RLP3;
  /* sanity check */
  if (NULL == set_sig)
  {
    return (radio_transport_ret_val);
  }

  /* We don't want reoriginiation to take place after processing 
     DoS or SDB packet */
  *set_sig = FALSE;

  /*-------------------------------------------------------------------------
     Check what transport does Packet request. If SDB it may request
     explicitly in its META information which is always present here.

     SDB_flags is passed by reference. ds_curr_pkt_needs_SDB will write the
     value of the flags here. Use this to decide if the configuration can be
     supported by DS now.
  -------------------------------------------------------------------------*/
  if (ds707_sdb_curr_pkt_needs_SDB(meta_info_ptr) == TRUE )
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "Packet Wants SDB/DoS");
    radio_transport_ret_val = RADIO_TRANSPORT_SDB;
  }  

  if (radio_transport_ret_val != RADIO_TRANSPORT_RLP3)
  {
    /*------------------------------------------------------------------------
      If we come until here, we know, outgoing packet prefers SDB. So let's
      see what transport can Mobile support Now:
      If its dormant and SO 33 call: it will be able to support SDB.
      Else: it will only support RLP.

      But we also have to decide if we can send the outgoing packet on the
      desired channel (i.e. ACH or REACH)
      SDB_flags is passed by value. This flag will inform we we need ACH or
      REACH or anything.
    ------------------------------------------------------------------------*/
    if(ds707_sdb_supports_SDB_now(meta_info_ptr->tx_flags, flow_ptr) == TRUE )
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Sending SDB/DoS Packet");
      radio_transport_ret_val = RADIO_TRANSPORT_SDB;
    }
    else
    {
      radio_transport_ret_val = RADIO_TRANSPORT_RLP3;
    }
  }

  return (radio_transport_ret_val);
} /* ds707_sdb_radio_transport_in_dorm() */

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
)
{
  /*------------------------------------------------------------------------
    Save the SO OMIT indicator to use it while transmitting SDB
  ------------------------------------------------------------------------*/
  DATA_IS707_MSG2(MSG_LEGACY_MED, "Changing SO Omit from %d to %d",
                  ds707_sdb_so_omit_indicator, so_omit_indicator);
  ds707_sdb_so_omit_indicator = so_omit_indicator;
} 

#endif /* FEATURE_DATA_IS2000_SDB || FEATURE_HDR_DOS */
#endif /* FEATURE_DATA_IS707      */
