#ifndef CMNV_H
#define CMNV_H
/*===========================================================================

          C A L L   M A N A G E R   N V   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  with CMNV.C


Copyright (c) 1998 - 2011 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmnv.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/15/11   jh      Integrated RPM feature
02/26/11   sn      Support for graceful pdp deactivation before detach.
12/23/10   sv      Integrate DSDS feature
09/23/10   xs      Superlint critical/high error fixed
07/22/10   aj      NV EFS support for LTE band pref
02/01/10   aj      Add support for DCTM 4.0
01/19/10   aj      Added VOIP support for emergency call
04/04/09   ks      Added support for disable_call_type to cmnv_data_s_type
02/27/09   rm      Adding header files of other MOBs
09/15/08   np      Added HPCD Rev B. 
01/03/08   ka      Adding handset based plus code dialing feature.
06/05/07   pk      Added support to read mob_cai_rev from NV
03/09/07   sk      RAM usage reduction.
03/24/06   ka      Added VideoTelephony fallback to voice feature for UMTS.
10/03/05   pk      Added Nnv_ddtm_settings_s_type to cmnv_data_s_type
                   DDTM settings NV item
06/06/05   pk      Added nv_accolc_type to cmnv_data_s_type
08/11/04   ka      Added support for accessing AMR from NV
02/06/04   ws      Initial jaguar/mainline merge.
10/28/03   ws      Removed uim pref slot support.
10/03/03   ws      Added ps orig data pref handling
09/16/03   vt      Added preferred slot handling.
01/21/03   ws      Updated copyright information for 2003
11/05/02   vt      Added changing RTRE configuration.
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the changes
                   between those two were merged with CCS CM
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32 (CM VU in N*2165)
                   that we needed to merge with and changes between those two VUs
                   were merged onto CCS CM tip.
10/23/01   VT/IC   Baseline for CCS
08/14/01   WLI     Added RUIM related functions.
02/07/01   RC      Added support for System Determination 2.0.
01/07/99   RC      Changes per ARM compiler.
11/24/98   RC      Initial release.

===========================================================================*/

#include "mmcp_variation.h"
#include "cm.h"
#include "cm_v.h"
#include "nv.h"        /* Interface to NV services */
#include "comdef.h"    /* Definition for basic types and macros */

#ifdef FEATURE_UIM_RUIM
#include "uim.h"       /* Interface to R-UIM services */
#include "uim_v.h"
#endif


/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/

/* Invalid user zone id
*/
#define CM_INVALID_DDTM_NUM_SRV_OPT         ((uint16)(-1))

/** EFS id enumeration
 *  NOTE: Add new enum values to the end of this table. It is
 *  very important to keep the order of this table consistent
 *  with the order in cmnv_efs_id_to_path_mapping_tbl[] to
 *  maintain correct operation
 *   */
typedef enum
{
  CMNV_EFS_ID_NONE = -1,          
    /**< internal use only */

  CMNV_EFS_ID_RPM_INFO,  
    /**< RPM Parameters stored in NV-EFS */

  CMNV_EFS_ID_RPM_MCC_MNC_LIST, 
    /**< RPM Supported MCC/MNC list */

  CMNV_EFS_ID_TDS_RPM_INFO,
    /**< TDS RPM Info parameters stored in NV-EFS*/

  CMNV_EFS_ID_MODE_DISABLED_IN_TSTS,
   /**< diable LTE when moving from triple_standby to dual_stanby 
      ** support in NV-EFS*/

  CMNV_EFS_ID_RPM_SYS_TIME,
  /**< System time till last power down */

  CMNV_EFS_ID_MTCSFB_CALL_REDIAL_OVER_1X,
  /**< MTCSFB call to be redialled over 1x*/

  CMNV_EFS_ID_MAX

} cmnv_efs_id_e_type;



/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/* Type for CM NV data buffer. The only purpose of this type is to create
** a big enough buffer to hold data that is associated with CM NV items.
**
** Note that we could simply use nv_item_type, but this would be a waste
** of ~300 bytes of RAM.
**
** Note that Lint info 754 (local structure member not referenced) is
** is expected here.
*/
typedef struct {  /*lint -e754 */

    union {

        nv_pref_mode_type               pref_mode;
        nv_pref_serv_type               pref_serv;
        nv_sys_pref_type                sys_pref;
        nv_sec_code_type                sec_code;
        nv_ps_data_orig_pref_enum_type  ps_data_orig_pref;
        nv_accolc_type                  accolc;
        #if (defined(FEATURE_UIM_RUIM) && defined(FEATURE_UIM_RUN_TIME_ENABLE))
        nv_rtre_configuration_type      rtre_config;
        #endif
        boolean                         gsm_amr_call_config;
        #ifdef FEATURE_DDTM_CNTL
        nv_ddtm_settings_s_type         ddtm_settings;
        #endif
        boolean                         umts_call_vt_codec_list;    
        byte                            mob_cai_rev;

        /* ifdef to prevent mem bloat */
        uint16                          home_mcc;
        nv_usr_sid_to_mcc_assoc_type    usr_sid_to_mcc_assoc_tbl;
        uint8                           hs_based_plus_dial_setting;
        uint64                          disable_call_type;
        uint8                           cs_to_voip_fallback_timer;
        uint8                           voip_cancel_retry_timer;
        uint32                          hard_fail_thr_time;
        #ifdef FEATURE_GRACEFUL_PDP_DEACTIVATION_B4_DETACH
        boolean                        pdp_deactivate_before_detach_supported;
        #endif

    } buffer;

    byte    padding[8];

} cmnv_data_s_type;


typedef struct {
    uint64  bits_65_128;
    uint64  bits_129_192;
    uint64  bits_193_256;
} bit_mask_192_ext_s_type;

/*===========================================================================

FUNCTION cmnv_init

DESCRIPTION
  Initialize the NV interface block.

  This function must be called before the NV interface block is used in
  any way, place or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmnv_init( void );


/*===========================================================================

FUNCTION cmnv_efs_read

DESCRIPTION
Read EFS item denoted by efs_id and copy to data_ptr

  MODE
DEPENDENCIES
  None

RETURN VALUE
  *size_ptr is set to the actual number of bytes that were copied into the  
  buffer pointed by data_ptr. If the read failed and there is no default 
  value defined, *size_ptr = 0


SIDE EFFECTS
  None.

==========================================================================*/
void                  cmnv_efs_read(

        cmnv_efs_id_e_type          efs_id,
            /* passing the efs Id to read. 
            */ 
        byte                     *data_ptr,
            /* Pointer to a data buffer where to store the data that is
            ** associated with the EFS item.
            */
        int32                    *size_ptr
            /* On input this parameter is the  maximum number
            ** of bytes to be copied into the buffer pointed by data_ptr.
            ** This parameter must not be nil.
            ** On output it is set *size_ptr to the actual number of bytes
            ** that were copied into the buffer pointed by data_ptr.
            */
);


/*===========================================================================

FUNCTION cmnv_efs_write

DESCRIPTION
  Write contents of data_ptr to EFS item denoted by efs_id

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void                  cmnv_efs_write(

        cmnv_efs_id_e_type          efs_id,
            /* passing the efs Id to write. 
            */ 
        const byte                  *data_ptr,
            /* Pointer to a data buffer to write to EFS.
            */
        int32                       size
            /* Size of the data_ptr buffer.
            */
);
    

/*===========================================================================

FUNCTION cmnv_write_extn

DESCRIPTION
  Write an item to NV memory.

  Note that NV writes are done in asynchronous fashion; that is, upon
  return of this function the write request is only being queued in the
  CM NV queue. The CM NV queue then services the request in the background.

DEPENDENCIES
  CM NV queue must have already been initialized with
  cmnv_q_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmnv_write_extn(

    nv_items_enum_type    nv_item,
        /* NV item to write */

    const nv_item_type    *data_ptr,
        /* pointer to data associated with the NV item */

    uint16                context

);

/*===========================================================================

FUNCTION cmnv_write

DESCRIPTION
  Write an item to NV memory.

  Note that NV writes are done in asynchronous fashion; that is, upon
  return of this function the write request is only being queued in the
  CM NV queue. The CM NV queue then services the request in the background.

DEPENDENCIES
  CM NV queue must have already been initialized with
  cmnv_q_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmnv_write(

    nv_items_enum_type    nv_item,
        /* NV item to write */

    const nv_item_type    *data_ptr
        /* pointer to data associated with the NV item */
);


/*===========================================================================

FUNCTION cmnv_write

DESCRIPTION
  Write an item to NV memory.

  Note that NV writes are done in asynchronous fashion; that is, upon
  return of this function the write request is only being queued in the
  CM NV queue. The CM NV queue then services the request in the background.

DEPENDENCIES
  CM NV queue must have already been initialized with
  cmnv_q_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmnv_write_extn(

    nv_items_enum_type    nv_item,
        /* NV item to write */

    const nv_item_type    *data_ptr,
        /* pointer to data associated with the NV item */

    uint16                context
        /* Context of the subscription */
);


/*===========================================================================

FUNCTION cmnv_write_wait_extn

DESCRIPTION
  Write an item from the NV memory in a synchronous fashion; that is, this
  function only returns  after the NV write is done being serviced by the
  NV task.

  This function is only expected to be used during power-up initialization.
  During operation cmnv_write() must be used to write items to NV.

DEPENDENCIES
  mc_task_start() must have already been called.

  This function is only expected to be used during power-up initialization.
  During operation cmnv_write() must be used to items to NV.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmnv_write_wait_extn(

    nv_items_enum_type    nv_item,
        /* NV item to write */

    nv_item_type          *data_ptr,
        /* pointer to data associated with the NV item */

    uint16                context
        /* Context of the subscription */

);


/*===========================================================================

FUNCTION cmnv_write_wait

DESCRIPTION
  Write an item to the NV memory in a synchronous fashion; that is, this
  function only returns  after the NV write is done being serviced by the
  NV task.

  This function is only expected to be used during power-up initialization.
  During operation cmnv_write() must be used to write items to NV.

DEPENDENCIES
  mc_task_start() must have already been called.

  This function is only expected to be used during power-up initialization.
  During operation cmnv_write() must be used to items to NV.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmnv_write_wait(

    nv_items_enum_type    nv_item,
        /* NV item to write */

    nv_item_type          *data_ptr
        /* pointer to data associated with the NV item */
);




/*===========================================================================

FUNCTION cmnv_q_write_done_cb

DESCRIPTION
  Callback function for completion of NV write operations

DEPENDENCIES
  CM NV queue must have already been initialized with
  cmnv_q_init().

RETURN VALUE
  none

SIDE EFFECTS
  Dequeue item from the CM NV queue.

===========================================================================*/
extern void cmnv_q_write_done_cb( void );



/*===========================================================================

FUNCTION cmnv_read_wait_extn

DESCRIPTION
  Read an item from the NV memory.

  Note that NV reads are done in a synchronous fashion; that is, this
  function only returns  after the NV read is done being serviced by the
  NV task.

DEPENDENCIES
  mc_task_start() must have already been called.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
nv_stat_enum_type cmnv_read_wait_extn(

    nv_items_enum_type  nv_item,
        /* NV item to read */

    nv_item_type        *data_ptr,
        /* pointer to buffer to place data associated with the NV item */

    uint16              context
        /* Context of the subscription */

);



/*===========================================================================

FUNCTION cmnv_read_wait

DESCRIPTION
  Read an item to NV memory.

  Note that NV reads are done in synchronous fashion; that is, this function
  returns only after the NV read is done being serviced by the NV task.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
nv_stat_enum_type cmnv_read_wait(

    nv_items_enum_type  nv_item,
        /* NV item to read */

    nv_item_type        *data_ptr
        /* pointer to buffer to place data associated with the NV item */
);


const char * cmnv_map_efs_id_to_filename(

        cmnv_efs_id_e_type          efs_id
            /* passing the efs Id to map 
            */ 

);
#ifdef FEATURE_UIM_RUIM

/*===========================================================================

FUNCTION cmnv_get_uim_status

DESCRIPTION
  Get the status from the UIM by actually pinging the card.

DEPENDENCIES
  Non-reentrant.

RETURN VALUE
  Returns UIM_PASS or UIM_FAIL.

SIDE EFFECTS
  None.

===========================================================================*/
extern uim_rpt_status cmnv_get_uim_status( void );




/*===========================================================================

FUNCTION cmnv_get_uim_spc_status

DESCRIPTION
  Read the spc status from RUIM.

DEPENDENCIES

RETURN VALUE
  TRUE    if the spc status is other than 0.
  FLASE   if the spc status is 0.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean cmnv_get_uim_spc_status( void );


#ifdef FEATURE_UIM_RUN_TIME_ENABLE

/*===========================================================================

FUNCTION cmnv_send_rtre_command

DESCRIPTION
  Sends RTRE configuration to NV.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
extern void cmnv_send_rtre_command
(
  nv_item_type *data_ptr                     /* pointer to space for item */
);
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */


#endif /* FEATURE_UIM_RUIM */

#endif /* CMNV_H */
