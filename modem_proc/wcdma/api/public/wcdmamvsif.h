#ifndef WCDMAMVSIF_H
#define WCDMAMVSIF_H

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/api/public/wcdmamvsif.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/06/14   pr       L1 support for MVS voice packet exchange.
10/17/12   pkg      Added support for WCDMA Tx Power Indication API for GPS
07/24/12   geg      Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
03/19/12   geg      Cleanup W API files and move internal modem APIs to internal files
10/18/10   ssg      Added changes for new WCDMA-L2 interface.
07/09/10   grk      1. Changes to indicate UL ISR to send RESET_ACK only after 
                       finishing UL RESET procedure.
                    2. Introduced new command to send RESET_ACK for duplicate RESETs.
02/23/10   grk      Modified prototype of DSM WM callback for CMI compliance.
02/22/10   prk      Changed structure defintions for timer optimization in CS over HS.
12/05/09   rmsd     Removed redefinition of rlc_lc_id_type. It is already defined in uecomdef.h
12/04/09   ssg      New L2 interface.
10/01/09   rmsd     CMI change:: Replaced dsmutil.h with dsm.h
09/09/09   rmsd     Removed inclusion of customer.h 
08/06/09   rm       Mainlined all features as part of CMI Phase 2-defeaturization effort.
07/01/09   rm       Updated rlc_cipher_algo_e_type with uecomdef_wcdma_cipher_algo_e_type (Snow 3G changes)
06/01/09   vk       Updated rlc_cipher_algo_e_type with UEA_2 for Snow3G.
03/12/09   rm       Moved MAC RRC "ehs" structures to public.
03/02/09   rm       Initial Release
===========================================================================*/
#include "comdef.h"
#include "dsm.h"        

#define MAX_DL_LOGICAL_CHANNEL  24
#define MAX_UL_LOGICAL_CHANNEL  24


/* -------------------------------------- */
/* PUBLIC API section of mcalwcdma_dec.h  */
/* -------------------------------------- */

/** Enum type definition for TTI enum type */
typedef enum {
  L1_TTI_10MS, /**< 10 ms TTI */
  L1_TTI_20MS, /**< 20 ms TTI */
  L1_TTI_40MS, /**< 40 ms TTI */
  L1_TTI_80MS, /**< 80 ms TTI */
  L1_NUM_TTI   /**< Total number oif TTI types */
} mcalwcdma_dec_tti_enum_type;

/* -------------------------------------- */
/* PUBLIC API section of l1sapcommon.h    */
/* -------------------------------------- */

/* Enum type definition for TTI enum type */
typedef mcalwcdma_dec_tti_enum_type  l1_tti_enum_type;

typedef enum
{
  E_TTI_2MS,
  E_TTI_10MS,
  NUM_E_TTI
} e_tti_enum_type;


/*Enums for supporting MVS clients for voice packet exchange*/
typedef void WL1_MODEM_MVS_CLIENT_DATA (void);

typedef void (*wl1_modem_timing_cb_type) (
    boolean steady_state,
                /**
     *  TRUE: DPDCH timeline is stable.
     *              Ready for vocoder packet exchange
     *  FALSE: DPDCH timeline is NOT stable. 
     *              STMR/Tx timing is being slammed to the new cell during inter-frequency HHO
     */
    uint8 cb_cfn,
   /**< Connection Frame Number - range from 0 to 255*/
    uint8 stmr_tick_count,
    /**<  STMR tick count -  range from 0 to 149 */
    void* client_data
    /**< client provided data during registration. Simply pass this back to client. */
);
typedef struct
{
  wl1_modem_timing_cb_type wl1_modem_mvs_timing_cb_ptr;
  WL1_MODEM_MVS_CLIENT_DATA *client_data_mvs;
} wl1_modem_mvs_client_type;

/*============================================================================================
                        DOWNLINK LAYER 2 COMMAND DEFINITIONS
============================================================================================*/

/* Downlink layer 2 command ID types */
typedef enum
{

  RLC_DL_REGISTER_SRVC_REQ = 0, /* Register Downlink Watermark */

  RLC_DL_DEREGISTER_SRVC_REQ, /* DeRegister Downlink Watermark */

  L2_DL_MAX_CMDS            /* Number of DL L2 Command types. must be last entry */

}l2_dl_cmd_enum_type;

/*-------------------------------------------------------------------
TYPE: rlc_lc_id_type

This defines the RLC Logical Channel ID that is used as buffer index
between RLC and RRC, MAC and RRC interfaces.
--------------------------------------------------------------------*/
typedef uint8   rlc_lc_id_type;

/*---------------------------------------------------------------------------
  Type for registration details for each downlink logical channel.
---------------------------------------------------------------------------*/
typedef struct {
  rlc_lc_id_type lc_id;                 /* Unique identifier for the       */
                                        /* downlink RLC logical channel.   */
                                        /* Can have a value from 0 to 18.  */
  dsm_watermark_type  *dl_wm_ptr;       /* Watermark for storing downlink  */
                                        /* data                            */
  boolean context;                      /* Task (FALSE) or Interrupt (TRUE)*/

  void *rlc_post_rx_func_ptr_para;      /* 3rd parameter for               */
                                        /* rlc_post_rx_proc_func_ptr()     */
  void  (*rlc_post_rx_proc_func_ptr)(uint8, uint8, void *);
                                        /* The callback function that is   */
                                        /* called by RLC when data is      */
                                        /* available in the downlink queue,*/
                                        /* for further processing by upper */
                                        /* layers.This function passes     */
                                        /* logical channel id as parameter.*/

} rlc_dl_channel_reg_type;


/*---------------------------------------------------------------------------
  Type to register the downlink queue for storing data
  bytes received over the air and a callback function to be called each
  time data is available in the queue, for one or more downlink
  logical channels.
---------------------------------------------------------------------------*/
typedef struct {
  uint8 nchan;                          /* Number of downlink logical      */
                                        /* channels to register            */
  rlc_dl_channel_reg_type  rlc_data[MAX_DL_LOGICAL_CHANNEL];
                                        /* Registration details for each   */
                                        /* logical channel                 */
} rlc_dl_register_srvc_type;


/*---------------------------------------------------------------------------
  Type to de-register the downlink queue for storing data
  bytes received over the air and a callback function to be called each
  time data is available in the queue, for one or more downlink
  logical channels.
---------------------------------------------------------------------------*/
typedef struct {
  uint8 nchan;                          /* Number of downlink logical      */
                                        /* channels to de register         */
  rlc_lc_id_type  rlc_id[MAX_DL_LOGICAL_CHANNEL];
                                        /* De-Registration logical channel */
} rlc_dl_deregister_srvc_type;


/* Downlink Layer 2 Command header */
typedef struct
{
  q_link_type link; /* Quaue link */

  l2_dl_cmd_enum_type cmd_id; /* Command ID */

}l2_dl_cmd_hdr_type;

/* Downlink Layer 2 command data type */
typedef union
{
  /* DL Register Srvc Request */
  rlc_dl_register_srvc_type dl_reg;

  /* DL DeRegister Srvc Request */
  rlc_dl_deregister_srvc_type dl_dereg;

}l2_dl_cmd_data_type;

/* DL Layer 2 command buffer type */
typedef struct
{
  l2_dl_cmd_hdr_type    cmd_hdr;

  l2_dl_cmd_data_type   cmd_data;

}l2_dl_cmd_type;


/*===========================================================================

                      Downlink L2-FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION  l2_dl_get_cmd_buf

DESCRIPTION
  This function should be called to get a command buffer from downlink Layer 2
  task command free queue

DEPENDENCIES
  None.

RETURN VALUE

  Pointer to downlink L2 command buffer if one is available
  NULL  otherwise

SIDE EFFECTS

===========================================================================*/
extern  l2_dl_cmd_type  *l2_dl_get_cmd_buf
(
  void
);

/*===========================================================================

FUNCTION        l2_dl_put_cmd

DESCRIPTION
  This function should be called to send a command to downlink Layer 2 task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Item will be enqueued for downlink Layer 2 task and signal will be set to
  indicate that the item has been enqueued.

===========================================================================*/
extern  void  l2_dl_put_cmd

(
  /* Pointer to the command buffer */
  l2_dl_cmd_type   *cmdptr

);

/* --------------------------------- */
/* PUBLIC APIs of l2ultask.h         */
/* --------------------------------- */


/*===========================================================================
                        UPLINK LAYER 2 COMMAND DEFINITIONS
===========================================================================*/


/* Uplink layer 2 command ID types */
typedef enum
{

  RLC_UL_REGISTER_SRVC_REQ = 0,   /* Request for service registration for RRC */

  RLC_UL_DEREGISTER_SRVC_REQ,   /* DeRequest for service registration for RRC */

  L2_UL_MAX_CMDS              /* Number of Ul L2 Command types. must be last entry */

} l2_ul_cmd_enum_type;



/*---------------------------------------------------------------------------
  Type for registration details for each uplink logical channel.
---------------------------------------------------------------------------*/
typedef struct {
  rlc_lc_id_type lc_id;                  /* Unique identifier for the      */
                                         /* uplink RLC logical channel.    */
                                         /* Can have a value from 0 to 18. */
  dsm_watermark_type  *ul_wm_ptr;        /* Watermark for storing uplink   */
                                         /* data                           */
} rlc_ul_channel_reg_type;


/*---------------------------------------------------------------------------
  Type to register the uplink queue for storing data bytes
  to be transmitted, for one or more uplink logical channels.
---------------------------------------------------------------------------*/
typedef struct {
  uint8 nchan;                          /* Number of uplink logical        */
                                        /* channels to register            */
  rlc_ul_channel_reg_type  rlc_data[MAX_UL_LOGICAL_CHANNEL];
                                        /* Registration details for each   */
                                        /* logical channel                 */
} rlc_ul_register_srvc_type;
  

/*---------------------------------------------------------------------------
  Type to deregister the uplink queue for storing data bytes
  to be transmitted, for one or more uplink logical channels.
---------------------------------------------------------------------------*/
typedef struct {
  uint8 nchan;                          /* Number of uplink logical        */
                                        /* channels to register            */
  rlc_lc_id_type  rlc_id[MAX_UL_LOGICAL_CHANNEL];
                                        /* DeRegistration WM of logical    */
                                        /* channel                         */
} rlc_ul_deregister_srvc_type;



/* Command header */
typedef struct
{
  q_link_type       link;       /* Quaue link */

  l2_ul_cmd_enum_type   cmd_id; /* Command ID */

}l2_ul_cmd_hdr_type;

/* Uplink Layer 2 command data type */
typedef union
{
  /* UL Register Srvc Request */
  rlc_ul_register_srvc_type   ul_reg;

  /* UL DeRegister Srvc Request */
  rlc_ul_deregister_srvc_type ul_dereg;

}l2_ul_cmd_data_type;

/* UL Layer 2 command buffer type */
typedef struct
{
  l2_ul_cmd_hdr_type    cmd_hdr;

  l2_ul_cmd_data_type   cmd_data;

}l2_ul_cmd_type;
/*===========================================================================

                      UpLink L2 FUNCTION DECLARATIONS

===========================================================================*/



/*===========================================================================

FUNCTION  l2_ul_get_cmd_buf

DESCRIPTION
  This function should be called to get a command buffer from uplink Layer 2
  task command free queue

DEPENDENCIES
  None.

RETURN VALUE

  Pointer to the uplink L2 command buffer if one is available
  NULL  otherwise

SIDE EFFECTS

===========================================================================*/
extern  l2_ul_cmd_type    *l2_ul_get_cmd_buf
(
  void
);

/*===========================================================================

FUNCTION        l2_ul_put_cmd

DESCRIPTION
  This function should be called to send a command to uplink Layer 2 task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Item will be enqueued for uplink Layer 2 task and signal will be set to
  indicate that the item has been enqueued.

===========================================================================*/
extern  void     l2_ul_put_cmd

(
  /* Pointer to the command buffer */
  l2_ul_cmd_type   *cmd_ptr

);

/*===========================================================================
FUNCTION wcdma_query_tx_power_dbm

DESCRIPTION
  This function returns the greater of the max TxAGC values computed for
  the immediately previous 670ms time period and current running time period.
  If not in DCH state it will return WCDMA_QUERY_TX_POWER_DBM_NO_INFO.

DEPENDENCIES
  None

RETURN VALUE
  int16. Returns max TxAGC value which is 1 sec old on an average.
 
SIDE EFFECTS
  None
===========================================================================*/
int16 wcdma_query_tx_power_dbm(void);

/* ========================================================================================== 
FUNCTION wl1_register_for_modem_timing

DESCRIPTION
  This function will be used by MVS modules for registering their cb ptr

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_register_for_modem_timing (wl1_modem_timing_cb_type wl1_modem_timing_cb_ptr,
    /**< This is the callback function WL1 calls every 10ms that is aligned with DPDCH frame boundary */
    void* client_data
    /**< The client provided data for client's use. */
);

#endif /* WCDMAMVSIF_H*/
