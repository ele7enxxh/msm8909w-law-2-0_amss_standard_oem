/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S

                         D S 7 0 7 _ S 0 3 3 

GENERAL DESCRIPTION
  This file contains functions specific to service option 33.  It does NOT
  contain RLP 3 services.  

EXTERNALIZED FUNCTIONS
  DS707_SO33_PKT_INITIALIZER
    Called when an SO 33 call connects.  Initializes RLP 3 and initializes
    the RRM state (which includes SCRM & Control/Hold).
    
  DS707_SO33_REG_INITIALIZERS
    Called at startup.  Registers initializer with SNM for when SO 33 
    call comes up.  Also registers RLP 3 BLOB generator asw well as QOS
    functionality.
    
  DS707_SO33_ORIG_QOS
    Asks RLP to create a QOS blob for CM origination parameters.
    
  DS707_SO33_PROCESS_QOS
    Called when QOS info received from BS.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Must call ds707_so33_reg_initializers at startup.

   Copyright (c) 2002-2013 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_so33.c_v   1.5   18 Dec 2002 17:30:26   ajithp  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_so33.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/26/13   sc      Replace all the memcpy and memmove with memscpy and 
                   memsmove as its safer.
12/16/12   ssb     Added changes to ignore RLP initialization if primary phys 
                   link state is NULL. This will avoid queuing of data to WM 
                   when there is no data call.
02/09/11   ack     Added is_1xrlp_ready flag to chk for 1X RLP init state 
02/02/11   ack     Added DS_RLP_INIT_CMD
10/13/10   op      Migrated to MSG 2.0 macros
10/07/08   cs      Compiler warning fix.
01/01/07   TMR     Added dsrlp_register_snm_initializers() call
07/25/06   ksu     EMPA: 1x RLP code review comments
12/08/05   atp     Removed special RLP txq size for MuxPDU5 turbo.
04/27/05   gr      Featurized handling of SO_OMIT under feature IS2000_SDB
04/05/05   gr      Added interface to handle SO_OMIT specified in SCM/UHDM
04/22/04   ak      Lint fixes.
12/18/02   atp     Minor print message changes.
11/17/02   ak      Updated file header comments.
09/24/02   atp     Added support for QOS. (1x Release-A feature).

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"
#include <stringl/stringl.h>

#ifdef FEATURE_DATA_IS707
#include "cai.h"
#include "caii.h"
#include "cm.h"
#include "data_msg.h"
#include "ds707_rrm.h"
#ifdef FEATURE_DATA_IS2000_SDB
#include "ds707_sdb.h"
#endif /* FEATURE_DATA_IS2000_SDB */
#include "dsrlp_v.h"
#include "err.h"
#include "msg.h"
#include "rxc.h"
#include "snm.h"
#include "dstask_v.h"
#include "ds707_pkt_mgr.h"
#include "dsrlp_v.h"

/*===========================================================================
                         EXTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_SO33_PKT_INITIALIZER

DESCRIPTION   Called when a so 33 packet data call is connected.  Goes ahead
              and sets up the associated RLP service.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so33_pkt_initializer
(
  const caii_srv_cfg_type*  cfg_info,
  word                      so_index
)
{
  ds_cmd_type *cmd_ptr = NULL;
  ps_phys_link_type      *phys_link_ptr;
  phys_link_state_type    phys_link_state;
  dsrlp_blob_cmd_type    *dsrlp_blob_cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  phys_link_ptr  = ds707_pkt_get_pri_phys_link_ptr();

  /* Get the phys link state */
  phys_link_state = PS_PHYS_LINK_GET_STATE(phys_link_ptr);

  /*-------------------------------------------------------------------------
    Ignore RLP initialization request if primary phys link state is NULL. 
    Primary phys link state NULL indicates that there is no data call and so 
    RLP need not be initialized. For MT Pages, incoming call event will be 
    sent by CM to DS.
  -------------------------------------------------------------------------*/
  if( phys_link_state == PHYS_LINK_NULL )
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "Primary Phys Link state is NULL."
                     "Ignore RLP initialization request from 1xCP." );
    return;
  }

  if (cfg_info->con[so_index].so == CAI_SO_PPP_PKT_DATA_3G)
  {
    cmd_ptr = ds_allocate_cmd_buf(sizeof(dsrlp_blob_cmd_type));
    if((NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr))
    {
      ASSERT(0);
      return;
    }
    else
    {
       /*-----------------------------------------------------------------------
       Set is_1xrlp_ready flag to false. This flag will ensure that RX/TX
       tasks don't transfer data until it is set to	true by dsrlp_init that
       gets called as part of the below posted DS Cmd. 
       -----------------------------------------------------------------------*/
       dsrlp_set_1xrlp_ready_flag(FALSE);

       /* Post command to DS task to init DSRLP */
       cmd_ptr->hdr.cmd_id = DS_CMD_RLP_INIT;

       dsrlp_blob_cmd_ptr = (dsrlp_blob_cmd_type *)cmd_ptr->cmd_payload_ptr;
       dsrlp_blob_cmd_ptr->sr_id = cfg_info->con[so_index].sr_id;
       dsrlp_blob_cmd_ptr->rlp_blob_len = cfg_info->con[so_index].rlp_blob_len;
       dsrlp_blob_cmd_ptr->rlp_info_incl = cfg_info->con[so_index].rlp_info_incl;
       memscpy( dsrlp_blob_cmd_ptr->rlp_blob, dsrlp_blob_cmd_ptr->rlp_blob_len,
                cfg_info->con[so_index].rlp_blob, cfg_info->con[so_index].rlp_blob_len );
       ds_put_cmd( cmd_ptr );
    }
  }
  else
  {
    ERR_FATAL("Only SO 33 uses callback right now",0,0,0);
  }
}

/*===========================================================================
FUNCTION      DS707_SO33_REG_INITIALIZERS

DESCRIPTION   Register initializer functions used by snm when a Service
              Option 33 is connected.

DEPENDENCIES  This function should only be called after all SO's have been
              added to the SNM.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so33_reg_initializers(void)
{
  snm_status_type     status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  status = snm_reg_connection_initializer(CAI_SO_PPP_PKT_DATA_3G, 
                                          ds707_so33_pkt_initializer);
  if (status == SNM_FAILURE)
  {
    ERR_FATAL("Could not register initializer",0,0,0);
  }

  /* Call RLP to register the RLP related initializers */
  (void) dsrlp_register_snm_initializers();

#if defined(FEATURE_DATA_IS2000_SDB)

  status =  snm_reg_sdb_so_omit_status_handler( CAI_SO_PPP_PKT_DATA_3G,
                                            ds707_sdb_so_omit_handler);

  if (status == SNM_FAILURE)
  {
    ERR_FATAL("Could not register initializer",0,0,0);
  }
#endif /*  FEATURE_DATA_IS2000_SDB*/
}


#ifdef FEATURE_IS2000_REL_A
/*===========================================================================
FUNCTION      DS707_SO33_ORIG_QOS

DESCRIPTION   Asks RLP to create QOS blob and  puts the blob in CM
              origination parameter.

DEPENDENCIES  NONE

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so33_orig_qos
(
  cm_cdma_orig_params_s_type    *orig_params
)
{
  /*-------------------------------------------------------------------------
    Build QOS blob.  Note that we donot build blob always
  -------------------------------------------------------------------------*/
  orig_params->qos_parms_len = dsrlp_build_qos_blob (orig_params->qos_parms);

  /*-------------------------------------------------------------------------
    dsrlp_build_qos_blob returns length of QOS blob to be sent after
    building the blob. If length == 0, it means default values of QOS
    are enough, and no blob needs to be sent to BS.
  -------------------------------------------------------------------------*/
  if ( orig_params->qos_parms_len > 0 )
  {
    orig_params->qos_parms_incl = TRUE;
  }
} /* ds707_so33_orig_qos() */


/*===========================================================================
FUNCTION      DS707_SO33_PROCESS_QOS

DESCRIPTION   Asks RLP to process QOS blob received from BS.

DEPENDENCIES  NONE

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so33_process_qos
(
  cm_call_mode_info_s_type  *mode_info_ptr
)
{
  /*-------------------------------------------------------------------------
    Check if qos blob (from BS) present in connect event. If so, process
    blob, otherwise note qos values not received from BS.
    The check for qos_parms_len > 0 is just for safety. If 1st condition
    is true, the second should be true normally.
  -------------------------------------------------------------------------*/
  if (
          (  mode_info_ptr->info.cdma_call.qos_parms_incl     )
       && (( mode_info_ptr->info.cdma_call.qos_parms_len ) >0 )
     )
  {
    dsrlp_enable_qos();
    

    if (dsrlp_process_qos_blob (
                                 mode_info_ptr->info.cdma_call.qos_parms,
                                 mode_info_ptr->info.cdma_call.qos_parms_len
                               ) == FALSE)
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Could not parse QOS RLP BLOB!!!");
    }
  }
  else
  {
    /*-----------------------------------------------------------------------
      Didn't receive QOS blob from BS
    -----------------------------------------------------------------------*/
    dsrlp_disable_qos();
  }

  /*-------------------------------------------------------------------------
    Determine current QOS values (default or rx from BS)
  -------------------------------------------------------------------------*/
  dsrlp_calc_curr_qos_values();

} /* ds707_so33_process_qos() */
#endif /* FEATURE_IS2000_REL_A */

#endif /* FEATURE_DATA_IS707 */

