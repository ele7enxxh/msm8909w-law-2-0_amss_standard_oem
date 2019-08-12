/*===========================================================================
               D O W N L I N K   M A C    L O G G I N G

DESCRIPTION
  This file provides the functions for MAC downlink logging.

EXTERNALIZED FUNCTIONS
  List any functions that are externalized in this file.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  List any initialization and sequencing that is required for this module.

Copyright (c) 2001 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

Copyright (c) 2002-2003, 2005-2006, 2008-2009 Qualcomm Technologies, Inc.  
All Rights Reserved.
===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mac/vcs/macdllog.c_v   1.2   29 Aug 2001 10:49:20   rchar  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/src/macdllog.c#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when      who     what, where, why
--------  ---     ----------------------------------------------------------
07/26/14   rkmk    Reduce image size by using right macro depending upon number of parameters
01/02/12  grk     Feature cleanup
06/16/11  grk     Fix to update DL config log ptr correctly.
05/07/09  ssg     Updated Copyright Information
12/20/06  sk/pr   Fix for the correct logging of the DL channels for the log packet 0x411D
02/05/06  sk      Cast with WCDMA_DL_MAC_CFG_LOG_PACKET_type for the pointer
                  returned.
03/09/05  sk      Fixed critical lint errors.
11/10/03  sk      Added additional checks to see that the number of times the
                  FOR loop is executed is bounded by the UE_MAX value.
07/09/03  sk      Populated the ciphering related parameters for CS domain only.
06/17/03  sk      Store the ciphering related parameters.
08/02/02  sk      mac_dl_send_cfg_log_packet(): Commented out code related to ciphering.
08/28/01  rc      Fixed bug with multiple logical channels mapped to a
                  Transport Channel.
08/24/01  rc      Moved FEATURE definition to after the include definitions.
08/21/01  rc      Created file.

===========================================================================*/

/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* --------------------------------------------------------------------------
** Include Files
** ------------------------------------------------------------------------*/
#include "macdllog.h"
#include "msg.h"
#include <string.h>

/*===========================================================================

FUNCTION MAC_DL_SEND_CFG_LOG_PACKET

DESCRIPTION
   This function prepares a log packet for configured Downlink MAC Transport
   Channels and sends the packet to log services.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mac_dl_send_cfg_log_packet
(
  mac_dl_config_type *cfgptr        /* Ptr to Downlink MAC config          */
)
{
  WCDMA_DL_MAC_CFG_LOG_PACKET_type *dl_cfg_ptr; /* Ptr to DL MAC config log*/
                                                /* Packet                  */
  mac_dl_bch_cctrch_config_type *bch_info;      /* Ptr to BCH info         */
  mac_dl_pch_cctrch_config_type *pch_info;      /* Ptr to PCH info         */
  mac_dl_fach_trch_config_type  *fach_info;     /* Ptr to FACH info        */
  mac_dl_dch_trch_config_type   *dch_info;      /* Ptr to DCH info         */
  mac_dl_dsch_trch_config_type  *dsch_info;     /* Ptr to DSCH info        */
  mac_dl_ded_logch_config_type  *dlc_ptr;       /* Ptr to Logical ch. info */
  uint8                         *dl_log_ptr = NULL;
                                                /* Temp Ptr to Log packet  */
  uint8                         num_trch = 0;   /* Num of Transport Channel*/
  uint8                         active_fach = 0;/* Num of Active FACHs     */
  uint8                         num_logchan = 0;/* Num of Logical Channels */
  uint16                                              i,k;
 uint8 no_Logical_fach=0;
  /*-------------------------------------------------------------------------
   Calculate the Number of Transport Channels.
  -------------------------------------------------------------------------*/
  num_trch = (cfgptr->num_bchs + cfgptr->num_pchs + cfgptr->num_fachs +
             cfgptr->num_dchs + cfgptr->num_dschs);

  /*-------------------------------------------------------------------------
    Calculate the Number of Logical Channels for each transport channel.
  -------------------------------------------------------------------------*/

  /* Number of Logical Channels mapped to BCH */
  for (i = 0; (i < cfgptr->num_bchs) && (i< MAC_DL_MAX_CCTRCH);i++ )
  {
    num_logchan++;
  }
  /* Number of Logical Channels mapped to PCH */
  for (i = 0; (i < cfgptr->num_pchs) && (i< MAC_DL_MAX_CCTRCH);i++ )
  {
    num_logchan++;
  }
  /* Number of Logical Channels mapped to FACH */
  for (i = 0; (i < cfgptr->num_fachs) && (i<UE_MAX_TRCH);i++ )
  {
        fach_info = cfgptr->fach_info[i];

    if (fach_info->bcch_enable == TRUE)
    {
      active_fach++;
    }
    if (fach_info->ccch_enable == TRUE)
    {
      active_fach++;
    }
    if (fach_info->ctch_enable == TRUE)
    {
      active_fach++;
    }
    active_fach += fach_info->ndlchan;
    num_logchan += active_fach;
  }
  /* Number of Logical Channels mapped to DCH */
  for (i = 0; (i < cfgptr->num_dchs) && (i < UE_MAX_TRCH); i++)
  {
        dch_info = cfgptr->dch_info[i];
    num_logchan += dch_info->ndlchan;
  }
  /* Number of Logical Channels mapped to DSCH */
  for (i = 0; (i < cfgptr->num_dschs) && (i<UE_MAX_TRCH); i++)
  {
        dsch_info = cfgptr->dsch_info[i];
    num_logchan += dsch_info->ndlchan;
  }

  /* Allocate memory here.*/
  dl_cfg_ptr = (WCDMA_DL_MAC_CFG_LOG_PACKET_type *)log_alloc(WCDMA_DL_MAC_CFG_LOG_PACKET,
                              MAC_DL_CFG_PACKET_LEN(num_trch, num_logchan));

  if (dl_cfg_ptr != NULL)
  {
        /*dl_log_ptr = (uint8 *)dl_cfg_ptr;
        dl_log_ptr += FPOS(WCDMA_DL_MAC_CFG_LOG_PACKET_type,num_trch);

        *(dl_log_ptr++) = num_trch;
        *(dl_log_ptr++) = cfgptr->cipher_info[0].cipher_enable;
        *(dl_log_ptr++) = cfgptr->cipher_info[0].key_idx;
        *((uint32 *)dl_log_ptr)++ = cfgptr->cipher_info[0].hfn; */

    dl_cfg_ptr->num_trch  = num_trch;
    // Store ciphering parameters for CS domain only
    dl_cfg_ptr->ciphering_flag = cfgptr->cipher_info[0].cipher_enable;
    dl_cfg_ptr->ciphering_key_id = cfgptr->cipher_info[0].key_idx;
    dl_cfg_ptr->hfn = cfgptr->cipher_info[0].hfn;

    dl_log_ptr = (uint8 *)dl_cfg_ptr;
        dl_log_ptr += FPOS(WCDMA_DL_MAC_CFG_LOG_PACKET_type,trch_info);

    /* Store BCH information.*/
    for (i = 0; (i < cfgptr->num_bchs) && (i< MAC_DL_MAX_CCTRCH);i++)
    {
      bch_info = &cfgptr->bch_info[i];

      /* Store CCtrch ID instead of the Transport Channel ID */
      *(dl_log_ptr++) = bch_info->cctrch_id;

      /* Store the Transport Channel Type - BCH */
      *(dl_log_ptr++) = (uint8)UE_DL_TRCH_BCH;

      /* Store the Number of Logical Channels mapped to BCH - always 1*/
      *(dl_log_ptr++) = 1;

      /* Store the Logical Channel ID */
      *(dl_log_ptr++)  = bch_info->bcch_id;

      /* Store the Logical Channel Type - BCCH */
      *(dl_log_ptr++)  = (uint8)UE_LOGCHAN_BCCH;

      /* Store the Logical Channel Mode */
      *(dl_log_ptr++) = (uint8)UE_MODE_TRANSPARENT;

      /* Radio Bearer ID not applicable - Set it to 0*/
      *(dl_log_ptr++) = 0;

      /* MAC ID not applicable - Set it to 0  */
      *(dl_log_ptr++) = 0;

      /* MAC Priority not applicable - Set it to 0  */
      *(dl_log_ptr++) = 0;

    }

    /* Store PCH information.*/
    for (i = 0; (i < cfgptr->num_pchs) && (i< MAC_DL_MAX_CCTRCH);i++)
    {
            pch_info = &cfgptr->pch_info[i];

      /* Store CCtrch ID instead of the Transport Channel ID */
      *(dl_log_ptr++) = pch_info->cctrch_id;

      /* Store the Transport Channel Type - PCH */
      *(dl_log_ptr++) = (uint8)UE_DL_TRCH_PCH;

      /* Store the Number of Logical Channels mapped to PCH - always 1*/
      *(dl_log_ptr++) = 1;

      /* Store the Logical Channel ID */
      *(dl_log_ptr++)  = (uint8)pch_info->pcch_id;

      /* Store the Logical Channel Type - PCCH */
      *(dl_log_ptr++)  = (uint8)UE_LOGCHAN_PCCH;

      /* Store the Logical Channel Mode */
      *(dl_log_ptr++) = (uint8)UE_MODE_TRANSPARENT;

      /* Radio Bearer ID not applicable - Set it to 0*/
      *(dl_log_ptr++) = 0;

      /* MAC ID not applicable - Set it to 0  */
      *(dl_log_ptr++) = 0;

      /* MAC Priority not applicable - Set it to 0  */
      *(dl_log_ptr++) = 0;

    }

    /* Store FACH information.*/
    for (i = 0; (i < cfgptr->num_fachs) && (i<UE_MAX_TRCH);i++)
    {
            fach_info = cfgptr->fach_info[i];
        no_Logical_fach = 0;
        if (fach_info->bcch_enable == TRUE)
        {
          no_Logical_fach++;
        }
        if (fach_info->ccch_enable == TRUE)
        {
          no_Logical_fach++;
        }
        if (fach_info->ctch_enable == TRUE)
        {
          no_Logical_fach++;
        }
        no_Logical_fach += fach_info->ndlchan;

      /* Store the Transport Channel ID */
      *(dl_log_ptr++) = (uint8)fach_info->trch_id;

      /* Store the Transport Channel Type */
      *(dl_log_ptr++) = (uint8)UE_DL_TRCH_FACH;

      /* Store the Number of Logical Channels mapped to FACH*/
      *(dl_log_ptr++) = no_Logical_fach;

      /* If BCCH is enabled, store BCCH information */
      if (fach_info->bcch_enable == TRUE)
      {
        /* Store the Logical Channel ID */
        *(dl_log_ptr++) = fach_info->bcch_id;

        /* Store the Logical Channel Type - BCCH */
        *(dl_log_ptr++)= (uint8)UE_LOGCHAN_BCCH;

        /* Store the Logical Channel Mode */
        *(dl_log_ptr++)= (uint8)UE_MODE_TRANSPARENT;

        /* Radio Bearer ID not applicable - Set it to 0*/
        *(dl_log_ptr++) = 0;

        /* MAC ID not applicable - Set it to 0  */
        *(dl_log_ptr++) = 0;

        /* MAC Priority not applicable - Set it to 0  */
        *(dl_log_ptr++) = 0;

      }

      /* If CCCH is enabled, store CCCH information */
      if (fach_info->ccch_enable == TRUE)
      {
        /* Store the Logical Channel ID */
        *(dl_log_ptr++) = fach_info->ccch_id;

        /* Store the Logical Channel Type - CCCH */
        *(dl_log_ptr++) = (uint8)UE_LOGCHAN_CCCH;

        /* Store the Logical Channel Mode */
        *(dl_log_ptr++)= (uint8)UE_MODE_UNACKNOWLEDGED;

        /* Radio Bearer ID not applicable - Set it to 0*/
        *(dl_log_ptr++) = 0;

        /* MAC ID not applicable - Set it to 0  */
        *(dl_log_ptr++) = 0;

        /* MAC Priority not applicable - Set it to 0  */
        *(dl_log_ptr++) = 0;

      }

      /* If CTCH is enabled, store CTCH information */
      if (fach_info->ctch_enable == TRUE)
      {
        /* Store the Logical Channel ID */
        *(dl_log_ptr++)  = fach_info->ctch_id;

        /* Store the Logical Channel Type - CTCH */
        *(dl_log_ptr++) = (uint8)UE_LOGCHAN_CTCH;

        /* Store the Logical Channel Mode */
        *(dl_log_ptr++)= (uint8)UE_MODE_UNACKNOWLEDGED;

        /* Radio Bearer ID not applicable - Set it to 0*/
        *(dl_log_ptr++) = 0;

        /* MAC ID not applicable - Set it to 0  */
        *(dl_log_ptr++) = 0;

        /* MAC Priority not applicable - Set it to 0  */
        *(dl_log_ptr++) = 0;

      }

      /* Dedicated logical channels mapped to DCH */
      for (k = 0; (k < fach_info->ndlchan) && (k < MAX_DED_LOGCHAN_PER_TRCH);k++)
      {
        /* Store the Logical Channel ID */
        *(dl_log_ptr++) = fach_info->dlchan_info[k].rlc_id;

        /* Store the Logical Channel Type */
        *(dl_log_ptr++) = (uint8)fach_info->dlchan_info[k].chan_type;

        /* Store the Logical Channel Mode */
        *(dl_log_ptr++) = (uint8)fach_info->dlchan_info[k].rlc_mode;

        /* Store the Radio Bearer ID */
        *(dl_log_ptr++) = (uint8)fach_info->dlchan_info[k].rb_id;

        /* Store the MAC ID */
        *(dl_log_ptr++) = fach_info->dlchan_info[k].mac_id;

        *(dl_log_ptr++) = 0;

      }
    } /* for FACH */

    /* Store DCH information.*/
    for (i = 0; (i < cfgptr->num_dchs) && (i<UE_MAX_TRCH);i++)
    {
            dch_info = cfgptr->dch_info[i];

      /* Store the Transport Channel ID */
      *(dl_log_ptr++) = (uint8)dch_info->trch_id;

      /* Store the Transport Channel Type */
      *(dl_log_ptr++) = (uint8)UE_TRCH_DCH;

      /* Store the Number of Logical channels mapped to DCH */
      *(dl_log_ptr++) = dch_info->ndlchan;

      for (k = 0; (k < dch_info->ndlchan) && (k < MAX_DED_LOGCHAN_PER_TRCH);k++)
      {
        dlc_ptr = &dch_info->dlchan_info[k];

        /* Store the Logical Channel ID */
        *(dl_log_ptr++) = dlc_ptr->rlc_id;

        /* Store the Logical Channel Type */
        *(dl_log_ptr++) = (uint8)dlc_ptr->chan_type;

        /* Store the Logical Channel Mode */
        *(dl_log_ptr++) = (uint8)dlc_ptr->rlc_mode;

        /* Store the Radio Bearer ID */
        *(dl_log_ptr++) = (uint8)dlc_ptr->rb_id;

        /* Store the MAC ID */
        *(dl_log_ptr++) = dlc_ptr->mac_id;

        /* MAC Priority not applicable - Set it to 0  */
        *(dl_log_ptr++) = 0;
      }
    } /* for DCH */

    /* Store DSCH information.*/
    for (i = 0; (i < cfgptr->num_dschs) && (i<UE_MAX_TRCH);i++)
    {
            dsch_info = cfgptr->dsch_info[i];

      /* Store the Transport Channel ID */
      *(dl_log_ptr++) = (uint8)dsch_info->trch_id;

      /* Store the Transport Channel Type */
      *(dl_log_ptr++) = (uint8)UE_DL_TRCH_DSCH;

      /* Store the Number of Logical Channels mapped to DSCH */
      *(dl_log_ptr++) = dsch_info->ndlchan;

      for (k = 0; (k < dsch_info->ndlchan) && (k < MAX_DED_LOGCHAN_PER_TRCH);k++)
      {
        dlc_ptr = &dsch_info->dlchan_info[k];

        /* Store the Logical Channel ID */
        *(dl_log_ptr++) = dlc_ptr->rlc_id;

        /* Store the Logical Channel Type */
        *(dl_log_ptr++) = (uint8)dlc_ptr->chan_type;

        /* Store the Logical Channel Mode */
        *(dl_log_ptr++) = (uint8)dlc_ptr->rlc_mode;

        /* Store the Radio Bearer ID */
        *(dl_log_ptr++) = (uint8)dlc_ptr->rb_id;

        /* Store the MAC ID */
        *(dl_log_ptr++) = dlc_ptr->mac_id;

        /* MAC Priority not applicable - Set it to 0  */
        *(dl_log_ptr++) = 0;
      }
    } /* for DSCH */

    /* Commit the buffer to log services.*/
    log_commit(dl_cfg_ptr);
  }
  else
  {
    MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW, "Dropped 0x%x, code enabled: %d\n",
                                              WCDMA_DL_MAC_CFG_LOG_PACKET,
                                              log_status(WCDMA_DL_MAC_CFG_LOG_PACKET));
  }
}/* mac_dl_send_cfg_log_packet() */


