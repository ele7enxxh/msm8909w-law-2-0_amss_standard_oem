#ifndef MCALWCDMA_HSRACH_H
#define MCALWCDMA_HSRACH_H
/*===========================================================================
                MCAL for HS RACH (HEADER)

GENERAL DESCRIPTION
  This file contains the MCAL APIs which will be used in abstraction of information
  for HS RACH feature



EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2006 - 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/mcalwcdma_hsrach.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who    what, where, why
--------   ---   ----------------------------------------------------------
08/18/14   vr     Cleanup code for HSRACH 'conventional' seamless recfg.
04/08/14   ar     Cleaned up mcalwcdma_e_tti_enum_type
11/08/13   ash    Force set the HS DPCCH ovsf code to HW after UE wakes from E-DRX
07/06/12   vr     HSRACH fixes
06/04/12   gv     Feature HS RACH Initial code check in -- File Creation
*/

#include "wcdma_variation.h"
#ifdef FEATURE_WCDMA_HS_RACH

 /*==============================================
                                       Header Inclusions
 **==============================================*/
 #include "mdspsync.h"
 #include "wfw_mod_intf.h"



/*==============================================
                                        MACROS
**==============================================*/
#define MCALWCDMA_HSRACH_GET_NUM_EDCH_RSRC() \
   (mcalwcdma_fwsw_intf_addr_ptr->edchRsrcChanMap.numEdchRsrcIndices)

#define MCALWCDMA_HSRACH_NUM_JUDC_CFG_DB_PAGES 2
#define MCALWCDMA_INVALID_CFG_PAGE_INDEX 0xFFFF
/* ===============================================
**                                   ENUMERATED TYPES DEFINTIONS
** ===============================================*/
typedef enum{
 MCALWCDMA_HS_RACH_E_UL_CHAN_DISABLE,
 MCALWCDMA_HS_RACH_E_UL_CHAN_ENABLE,
 MCALWCDMA_HS_RACH_E_UL_CHAN_ACTION_INVALID
}mcalwcdma_hsrach_eul_action_enum_type;

/* ===============================================
**                                   STRUCTURE TYPES DEFINTIONS
** ===============================================*/
 typedef ul_dpcch_cfg_cmd_parms_struct_type mcalwcdma_hsrach_ul_dpcch_cfg_struct_type;
 typedef mdsp_e_ul_chan_cfg_cmd_parms_struct_type mcalwcdma_hsrach_e_ul_cfg_struct_type;
 typedef WfwEdchRsrcChanMappingStruct mcalwcdma_hsrach_edch_rsrc_info_struct_type;
 typedef WfwEdchRsrcIdxToChanParamStruct mcalwcdma_hsrach_rsrc_idx_channel_param_struct_type;
 typedef WfwDemodJUDCActionTypeEnum mcalwcdma_hsrach_judc_action_enum_type;

 /* MCAL HS RACH structure that holds the JUDC contents*/
 typedef struct{
   /* Boolean to indicate HS RACH, UL channels are enabled*/
  mcalwcdma_hsrach_judc_action_enum_type hs_rach_enable_UL;

  /* boolean to indicate HS RACH DL channels are enabled*/
  mcalwcdma_hsrach_judc_action_enum_type hs_rach_enable_DL;

  /* phychan bmsk that needs to be configured */
  uint32 phychan_bmsk;

  /* combiner bmsk that needs to be configured */
  uint16 comb_bmsk;

  /* Indication to the FW that UL DPCCH configuration is modified*/
  uint8 ul_dpcch_cfg_present;

   /* Indication to the FW that UL DPCCH configuration is modified*/
  uint8 e_ul_cfg_present;

  /* UL DPCCH config information*/
  mcalwcdma_hsrach_ul_dpcch_cfg_struct_type ul_dpcch_cfg_info;

  /* EUL UL config information*/
  mcalwcdma_hsrach_e_ul_cfg_struct_type e_ul_cfg_info;
 }mcalwcdma_hsrach_judc_cfg_cmd_struct_type;

/* ===============================================
**                                   EXTERN of GLOBAL VARIABLES
** ===============================================*/


/* ===============================================
**                                   FUNCTION PROTOTYPES
** ===============================================*/

/*===========================================================================
FUNCTION        MCALWCDMA_HSRACH_GET_EDCH_RSRC_INFO_BUFFER_PTR

DESCRIPTION     This function returns the interface buffer pointer for the EDCH
              Resource information in the FW structure.

DEPENDENCIES

RETURN VALUE    None

SIDE EFFECTS    Interface variable for storing the edch resource information
             is updated
===========================================================================*/
extern mcalwcdma_hsrach_edch_rsrc_info_struct_type*
         mcalwcdma_hsrach_get_edch_rsrc_info_buffer_ptr(void);

 /*===========================================================================

FUNCTION		MCALWCDMA_HSRACH_INIT_JUDC_VARS

DESCRIPTION 	This function initializes the JUDC global before beginning to
	 populate the same

DEPENDENCIES   None

RETURN VALUE	None

SIDE EFFECTS	None

===========================================================================*/
extern void mcalwcdma_hsrach_init_judc_vars(void);

 /*===========================================================================

FUNCTION        MCALWCDMA_HSRACH_FILL_FDPCH_CFG_INFO

DESCRIPTION     This function is called from the DL F-DPCH setup module
                during the channel setup procedure, this could be for enable/disable
                or suspend/resume operations.

DEPENDENCIES   None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void mcalwcdma_hsrach_fill_fdpch_cfg_info(uint32 phychan_bmsk, uint16 comb_bmsk);
/*===========================================================================

FUNCTION        MCALWCDMA_HSRACH_FILL_EUL_DL_CFG_INFO

DESCRIPTION     This function is called from the DL F-DPCH setup module
                during the channel setup procedure, this could be for enable/disable
                or suspend/resume operations.

DEPENDENCIES   None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void mcalwcdma_hsrach_fill_eul_dl_cfg_info(uint32 phychan_bmsk, boolean enable);

/*===========================================================================

FUNCTION        MCALWCDMA_HSRACH_FILL_UL_DPCCH_CFG_INFO

DESCRIPTION     This function is called from the UL DPCCH setup module
                during the channel setup procedure, this could be for enable/disable
                or suspend/resume operations.

DEPENDENCIES   None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void mcalwcdma_hsrach_fill_ul_dpcch_cfg_info(
 /* enable or disable DPCCH processing */
 uint16 enable,
  /* slot format*/
 uint16 slot_fmt,
  /* the uncoded TFCI bits to be used on the uplink DPCCH */
 uint16 uncoded_tfci);

/*===========================================================================

FUNCTION        MCALWCDMA_HSRACH_FILL_E_UL_CFG_INFO

DESCRIPTION     This function is called from the EUL UL setup module
                during the channel setup procedure, this could be for enable/disable
                or suspend/resume operations.

DEPENDENCIES   None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void mcalwcdma_hsrach_fill_e_ul_cfg_info(
 mcalwcdma_hsrach_eul_action_enum_type action,
 boolean  ir,
 e_tti_enum_type  tti,
 uint16                        start_sub_fn,
 uint16                        final_sub_fn);

/*===========================================================================

FUNCTION       MCALWCDMA_HSRACH_JUDC_CMD_PRE_CB

DESCRIPTION     This function is pre-call back for the JUDC config
                command.

DEPENDENCIES   None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void mcalwcdma_hsrach_judc_cmd_pre_cb(void);

/*===========================================================================
FUNCTION        MCALWCDMA_HSRACH_GET_EUL_FINAL_SFN_VAL

DESCRIPTION     This function gets the eul final SFN value from the JUDC structure

DEPENDENCIES

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern uint16 mcalwcdma_hsrach_get_eul_final_sfn_val(void);

/*===========================================================================
FUNCTION        MCALWCDMA_HSRACH_SET_EUL_FINAL_SFN_VAL

DESCRIPTION     This function sets the correct eul final SFN value from the JUDC structure

DEPENDENCIES

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void mcalwcdma_hsrach_set_eul_final_sfn_val(uint16 eul_final_sfn_val);

/*===========================================================================

 FUNCTION        mcalwcdma_hsrach_save_judc_cfg_cmd

 DESCRIPTION     This function allocates an ordered page for MCAL HS RACH for JUDC

 DEPENDENCIES   None

 RETURN VALUE    None

 SIDE EFFECTS    None

 ===========================================================================*/
 extern void mcalwcdma_hsrach_save_judc_cfg_cmd(void);

#endif /*FEATURE_WCDMA_HS_RACH*/
#endif

