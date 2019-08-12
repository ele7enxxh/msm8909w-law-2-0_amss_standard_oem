
/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/emm_sim_handler.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/21/11   gps     Removed refrence for header file "gsdi_exp.h"
08/04/10   KRR     Removed code where we are setting TIN to NONE in function
                   emm_unload_sim_data().
05/11/10   RI      Mainlined spec changes upto Dec 09.
05/06/10   RI      Fixed the saving of EPS LOCI.
04/13/10   MNK     ISR implementation
04/07/10   zren    Updated to only save valid GUTI and last registered TAI to 
                   EPSLOCI
03/16/10   zren    Updated MO detach procedure upon receiving MMR_STOP_MODE
03/11/10   MNK     Fix a bug in unload_sim_data
03/05/10   MNK     TIN related changes
02/16/10   MNK     Set tin type 
02/05/10   zren    Updated processing of connection abort in waiting for RRC 
                   confirmation state
02/02/10   RI      Fixed the EFS storage of EPS LOCI
01/22/10   RI      Added support for handling multiple EPS contexts.
12/18/09   RI      Updated with unified EFS and USIM reads.
06/23/09   hnam    Added support for NAS log packet with log code: 0XB0EF
06/23/09   zren    Added support of processing EPLMN list, forbidden PLMN list,
                   forbidden TAI list for roaming and forbidden TAI list for
                   regional provision of service 
05/29/09   zren    Added support for TAU feature
05/15/09   hnam    Deleted hplmn value assignment in emm_sim_process_assignment()
04/15/09   RI      Added support for LTE NAS authentication.
04/15/09   zren    Completed MO detach feature
04/03/09   vdr     Update plmn_service_state upon reception of the MMR_SIM_AVAILABLE_REQ
                   and MMR_SIM_NOT_AVAILABLE_REQ
02/03/09   vdr     Starting using EPSLOCI table
01/31/09   mnk     Changed USIM_USIM_GUTI to USIM_USIM_EPSLOCI
01/30/09   mnk     Removed return when last_visisted_tai is ivalid to be able to send SIM_UPDATE to RRC
01/29/09   mnk     Added the test stub for reg_sim_hplmn_get
01/20/09   vdr     Now sending emm_send_sim_update() upon SIM available and unavailable REG indications
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "comdef.h"
#include "emm.h"
#include "rex.h"
#include "emm_database.h"
#include "emm_rrc_if.h"
#include "msgr.h"
#include "msgr_lte.h"
#include "emm_utility.h"
#include "emm_sim_handler.h"
#include "emm_reg_handler.h"
#include "reg_sim_v.h"
#include "msg_lib_decode_common_emm_ie.h"
#include "msg_lib_encode_common_emm_ie.h"
#include "emm_update_lib.h"
#include "emm_security.h"
#include "lte_nas_log.h"
#include "mm_sim.h"

#include "naslog_v.h"
#include "err.h"
#include "fs_lib.h"
#include "stringl.h"
#include "mcfg_fs.h"
#ifdef TEST_FRAMEWORK
#error code not present
#endif
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*===========================================================================

                        INTERNAL TYPES DEFINITION

===========================================================================*/

#define EMM_PLMN_ID_LENGTH 3

#ifdef TEST_FRAMEWORK
 #error code not present
#endif // TEST_FRAMEWORK

lte_rrc_imsi_s lte_convert_imsi_to_digits(imsi_data_T imsi)
{
   lte_rrc_imsi_s rrc_imsi_local;
   byte           odd_digits;
   byte           rrc_index = 0;
   byte           index = 0;

   memset((void *)&rrc_imsi_local, 0x0, sizeof(lte_rrc_imsi_s));
   if(imsi.length == 0)
   {
      MSG_ERROR_DS_0(MM_SUB, "=EMM= No valid IMSI present to convert");
      rrc_imsi_local.num_digits = 0;
      return rrc_imsi_local;
   }

   odd_digits = imsi.data[0] & 0x08;

   if( odd_digits)
      rrc_imsi_local.num_digits = imsi.length *2 - 1;
   else
      rrc_imsi_local.num_digits = imsi.length *2 - 2;


   rrc_imsi_local.imsi_digits[rrc_index++] = UPPER_BCD(imsi.data[0]);

   ASSERT(imsi.length <= 8);

   /* start from the 2nd byte of IMSI value */
   for(index = 1; index < imsi.length; index++)
   {
     ASSERT((rrc_index + 2) < LTE_RRC_MAX_IMSI_DIGITS );

      if( index == imsi.length -1) /* last byte */
      {
         rrc_imsi_local.imsi_digits[rrc_index++] = LOWER_BCD( imsi.data[index] );
         if(odd_digits) /* count the last one if odd digits */
            rrc_imsi_local.imsi_digits[rrc_index++] = UPPER_BCD( imsi.data[index] );
      }
      else
      {
         rrc_imsi_local.imsi_digits[rrc_index++] = LOWER_BCD( imsi.data[index] );
         rrc_imsi_local.imsi_digits[rrc_index++] = UPPER_BCD( imsi.data[index] );
      }
   }

   return rrc_imsi_local;
}

#ifdef TEST_FRAMEWORK
#error code not present
#endif

boolean is_tai_invalid
( 
  lte_nas_tai_lst1_type  tai
)
{
  if (tai.tac == 0xFFFE)
  {
    return TRUE ;
  }
  else if (is_mcc_invalid(tai.plmn))
  {
    return TRUE ;
  }
  return FALSE ;
} /* end of is_tai_invalid() */

void send_usim_card_mode_log(
    mmgsdi_app_enum_type sim_card_mode,
    boolean lte_supported, byte *imsi,byte *epsloci)
{
  lte_nas_emm_usim_card_mode_type esm_usim_card_mode_info; 

  memset(&esm_usim_card_mode_info,0,sizeof(lte_nas_emm_usim_card_mode_type));
  /*-----------------------------------------------------------
         Log the NAS Message packet (Log code: 0XB0EF)
  ------------------------------------------------------------*/
  esm_usim_card_mode_info.usim_card_mode = sim_card_mode;
  esm_usim_card_mode_info.lte_service_supported = lte_supported; /*hnam shall revisit here later, to get the value from SIM*/
  memscpy((void*)&esm_usim_card_mode_info.imsi[0],
          sizeof(esm_usim_card_mode_info.imsi),
          (void*)imsi,
          IMSI_LEN);
  memscpy((void*)&esm_usim_card_mode_info.epsloci[0],
          sizeof(esm_usim_card_mode_info.epsloci),
          (void*)epsloci,
          EPSLOCI_LENGTH);
  send_emm_usimcard_mode_log_info(&esm_usim_card_mode_info);
}

boolean emm_load_epsloci_data
( 
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
  boolean mmgsdi_status = TRUE;

  byte                    epsloci[EPSLOCI_LENGTH];
  word                    usim_guti_offset = EMM_USIM_GUTI_OFFSET ;
  word                    guti_length = EMM_GUTI_LENGTH ;
  boolean                 eps_loci_status = FALSE;
  mcfg_fs_status_e_type   efs_status = MCFG_FS_STATUS_OK;
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 
  if(emm_ctrl_data_ptr->emm_tai_list_ptr == NULL)
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Wrong sub this should not happen, return from function emm_load_epsloci_data");
    return FALSE;
  }

  memset( (void *)epsloci, 0xFF, EPSLOCI_LENGTH );

  if( emm_ctrl_data_ptr->eps_storage_supported ) 
  {
   /* read the EPS LOCI only if USIM */
    if(mm_sim_card_mode != MMGSDI_APP_USIM)
    {
      memset((void *)&emm_ctrl_data_ptr->last_registered_tai.plmn,0xFF,sizeof(sys_plmn_id_s_type)) ;
      return FALSE;
    }
    mmgsdi_status = mm_sim_read(MMGSDI_USIM_EPSLOCI, (byte *)epsloci, EPSLOCI_LENGTH);
    
  } else 
  {
    /* read from EFS */
    efs_status = mcfg_fs_read("/nvm/alpha/modem/nas/lte_nas_eps_loci", 
                              epsloci, 
                              sizeof(epsloci),
                              MCFG_FS_TYPE_EFS,
                              (mcfg_fs_sub_id_e_type)mm_sub_id);
  }

  if( ( !mmgsdi_status )  || ( efs_status != MCFG_FS_STATUS_OK  ) )
  {
     /* Note: If the EPS LOCI is not available, the multimode code 
              can continue with IMSI instead of GUTI
     */
     MSG_ERROR_DS_2(MM_SUB, "=EMM= EPS LOCI can not be read, status - %d-%d, invalidate params now [DELETE GUTI, last visited registered TAI, TAI list, and KSI]",
                    mmgsdi_status, efs_status);

     /* Delete GUTI */
     emm_ctrl_data_ptr->emm_guti_valid = FALSE ;
     memset((void *)&emm_ctrl_data_ptr->emm_guti, 0xFF, sizeof(lte_nas_emm_guti_type)) ;
     emm_ctrl_data_ptr->plmn_service_state->rac_or_mme_code = 0xFF;

     /* Delete last visited registered TAI */ 

     emm_ctrl_data_ptr->last_registered_tai_valid = FALSE ;
     emm_ctrl_data_ptr->last_registered_tai.tac =  0xFFFE;
     memset((void *)&emm_ctrl_data_ptr->last_registered_tai.plmn,0xFF,sizeof(sys_plmn_id_s_type)) ;
     emm_ctrl_data_ptr->emm_update_status  = EMM_STATUS_NOT_UPDATED ;

     /* Delete TAI list */ 
     emm_ctrl_data_ptr->emm_tai_list_ptr->tai_lst_length = 0 ;

     /* Invalidate NAS Security parameters */ 
     emm_invalidate_nas_security_params(emm_ctrl_data_ptr);

     send_usim_card_mode_log(mm_sim_card_mode,emm_ctrl_data_ptr->eps_storage_supported,(byte*)mm_stored_imsi.data,&epsloci[0]);
  }
  else
  {
    /* update the global copy */
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Saving the last known ESPLOCI info read from the SIM");

    memscpy((void*)&emm_ctrl_data_ptr->last_known_epsloci[EMM_USIM_GUTI_OFFSET],
            sizeof(emm_ctrl_data_ptr->last_known_epsloci) - EMM_USIM_GUTI_OFFSET,
            (void*)&epsloci[EMM_USIM_GUTI_OFFSET],
            EPSLOCI_LENGTH);

    memscpy((void*)&emm_ctrl_data_ptr->last_registered_tai.plmn,
            sizeof(emm_ctrl_data_ptr->last_registered_tai.plmn),
            (void*)&epsloci[EMM_GUTI_LENGTH],
            sizeof(sys_plmn_id_s_type));

    //memcpy((void*)&emm_ctrl_data_ptr->last_registered_tai.tac,(void*)&epsloci[EMM_GUTI_LENGTH + sizeof(sys_plmn_id_s_type)],sizeof(word));

    emm_ctrl_data_ptr->last_registered_tai.tac = ((epsloci[15]) << 8) + (epsloci[16]);            

    if (is_tai_invalid( emm_ctrl_data_ptr->last_registered_tai ))
    {
      /* ------------------------------------------------------------
      ** If the RAI value indicates it has been "deleted", invalidate
      ** the remaining PSLOCI data (see 24.008 section 4.1.3.2)
      ** ------------------------------------------------------------ */        
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Invalid RAI, deleting PSLOCI");
      emm_ctrl_data_ptr->last_registered_tai_valid = FALSE ;
      //emm_invalidate_mobility_info(EMM_STATUS_NOT_UPDATED, 
      //                             SYS_SIM_STATE_AVAILABLE, 
      //                             emm_ctrl_data_ptr, TRUE) ;
    }
    else
    {
      emm_ctrl_data_ptr->last_registered_tai_valid = TRUE;

      switch (epsloci[EMM_UPDATE_STATUS])
      {
        case 0:
          emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_UPDATED ;
          break ;
        case 1:
          emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_NOT_UPDATED ;
          break ;
        case 2:
          emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_ROAMING_NOT_ALLOWED ;
          break ;
        default:
          emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_NOT_UPDATED ;
          break ;
      }

      guti_length *= 8;
      if (decode_guti_ie(&epsloci[EMM_USIM_GUTI_OFFSET],&usim_guti_offset, &guti_length, &emm_ctrl_data_ptr->emm_guti) == FALSE)
      {
        emm_ctrl_data_ptr->emm_guti_valid = FALSE ;

//        if(emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI)
//        {
//          emm_set_and_write_tin_type(NAS_MM_TIN_NONE);
//        }
        //emm_invalidate_mobility_info(EMM_STATUS_NOT_UPDATED, 
        //                             SYS_SIM_STATE_AVAILABLE, 
        //                             emm_ctrl_data_ptr, TRUE) ;
        MSG_ERROR_DS_0(MM_SUB, "=EMM= GUTI can not be read");
        // return ;
      }
      else
      {
        emm_ctrl_data_ptr->emm_guti_valid = TRUE ;

        eps_loci_status = TRUE;
      }
      
    }
    send_usim_card_mode_log(mm_sim_card_mode,emm_ctrl_data_ptr->eps_storage_supported,(byte*)mm_stored_imsi.data,&epsloci[0]);
  }  
  return eps_loci_status;

} /* end of emm_process_sim_available() */

/*===========================================================================

FUNCTION  EMM_UNLOAD_SIM_DATA

DESCRIPTION
  This function unloads SIM data

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void emm_unload_sim_data
( 
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{

  int index;
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 

  if(emm_ctrl_data_ptr->plmn_service_state->sim_state != SYS_SIM_STATE_NOT_AVAILABLE)
  {
    emm_setup_reg_container((emm_reg_container_type)NULL) ;  

    emm_ctrl_data_ptr->plmn_service_state->sim_state = SYS_SIM_STATE_NOT_AVAILABLE ;
    
    emm_ctrl_data_ptr->plmn_service_state->rac_or_mme_code = 0xFF;
    
    emm_ctrl_data_ptr->last_registered_tai_valid = FALSE ;
    
    emm_ctrl_data_ptr->emm_equivalent_PLMN_list.length_of_plmn_lst_contents = 0;
    
    emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_UPDATED ;

    if( emm_ctrl_data_ptr->forbidden_for_service_list_ptr != NULL)
    {
      emm_ctrl_data_ptr->forbidden_for_service_list_ptr->tai_lst_length = 0 ;
    }

    if(emm_ctrl_data_ptr->rrc_forbidden_list_ptr != NULL)
    {
      emm_ctrl_data_ptr->rrc_forbidden_list_ptr->tai_lst_length = 0 ;
      emm_clear_rpm_ftai_list(FALSE,emm_ctrl_data_ptr);
    }
    emm_clear_manual_ftai_list();
    
    mm_stop_timer(EMM_FORBIDDEN_TAI_CLEAR_TIMER);

    emm_ctrl_data_ptr->reg_req_pended_indication = FALSE ;
    
    /* delete the context information */
    emm_ctrl_data_ptr->reset_nas_context = TRUE;
    emm_send_sim_update(emm_ctrl_data_ptr) ;
  
    for ( index = 0; index < EMM_MAX_CONTEXTS; index++ )
    {
      emm_reset_security_info(&emm_ctrl_data_ptr->context_info[index]);
    }
    memset(&emm_ctrl_data_ptr->last_rrc_deactivate_cnf,0,sizeof(lte_rrc_deactivate_cnf_s));

    /* Reset FPLMN list for combined registration */
    emm_ctrl_data_ptr->combined_reg_fplmn_list.length_of_plmn_lst_contents = 0;
    memset((void *)emm_ctrl_data_ptr->combined_reg_fplmn_list.plmn, 
           0xFF, sizeof(sys_plmn_id_s_type) * MAX_NO_OF_PLMNs);

    /* Invalidate GUTI */
    emm_ctrl_data_ptr->emm_guti_valid = FALSE;
    memset((void *)&emm_ctrl_data_ptr->emm_guti, 
           0xFF, 
           sizeof(lte_nas_emm_guti_type));

    /* Invalidate IMSI */
    memset((void *)&emm_ctrl_data_ptr->imsi, 0xFF, sizeof(lte_rrc_imsi_s));
  }
} /* end of emm_process_sim_unavailable() */

void emm_save_mobility_info
(
  emm_ctrl_data_type         *emm_ctrl_data_ptr
)
{
  boolean mmgsdi_status = TRUE;
  byte                    epsloci[EPSLOCI_LENGTH];
  word                    pos = 0 ;
  mcfg_fs_status_e_type   efs_status = MCFG_FS_STATUS_OK;
  int16                   epsloci_status =0;

  ASSERT(emm_ctrl_data_ptr != NULL); 

#ifdef FEATURE_LTE_REL9  
  if(emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_ONLY)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= EMC attached. Do not save mobility info to USIM/EFS");
    return;
  }
#endif

  if(mm_sim_card_mode != MMGSDI_APP_USIM)  
  {
     MSG_ERROR_DS_0(MM_SUB, "=EMM= SIM either not initialized or invalid");
     //emm_ctrl_data_ptr->plmn_service_state->sim_state = SYS_SIM_STATE_NOT_AVAILABLE;
     return;
  }

  memset(&epsloci[0],0xFF,EPSLOCI_LENGTH);
  if(emm_ctrl_data_ptr->emm_guti_valid == TRUE)
  {
    /* Encode GUTI into EPS LOCI table */
    encode_guti_ie(&emm_ctrl_data_ptr->emm_guti,&pos,&epsloci[EMM_USIM_GUTI_OFFSET]);
  }
  else
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= No valid GUTI to save to EPSLOCI");
  }

  if(emm_ctrl_data_ptr->last_registered_tai_valid == TRUE)
  {
    /* Encode last registered TAI into EPS LOCI table */
    memscpy((void*)&epsloci[EMM_GUTI_LENGTH],
            sizeof(epsloci) - EMM_GUTI_LENGTH,
            (void*)&emm_ctrl_data_ptr->last_registered_tai.plmn, 
            sizeof(sys_plmn_id_s_type));

    epsloci[EMM_GUTI_LENGTH+EMM_PLMN_ID_LENGTH] = 
            (byte) ((emm_ctrl_data_ptr->last_registered_tai.tac) >> 8);
    epsloci[EMM_GUTI_LENGTH+EMM_PLMN_ID_LENGTH+1] = 
            (byte) ((emm_ctrl_data_ptr->last_registered_tai.tac) & 0x00FF);

  }
  else
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= No valid last registered TAI to save to EPSLOCI");
  }

  MSG_HIGH_DS_1(MM_SUB,"=EMM= Set EPS update status to %d (Save mobility)",
              (int)emm_ctrl_data_ptr->emm_update_status);
  epsloci[EMM_UPDATE_STATUS] = (byte)emm_ctrl_data_ptr->emm_update_status ;

  epsloci_status = memcmp(&emm_ctrl_data_ptr->last_known_epsloci[EMM_USIM_GUTI_OFFSET],
                             &epsloci[EMM_USIM_GUTI_OFFSET], EPSLOCI_LENGTH);

  if( emm_ctrl_data_ptr->eps_storage_supported )
  {
    if( epsloci_status != 0 )
    {  
      mmgsdi_status = mm_sim_write(MMGSDI_USIM_EPSLOCI, &epsloci[EMM_USIM_GUTI_OFFSET], EPSLOCI_LENGTH);

      /* update the local copy */
      memscpy((void*)&emm_ctrl_data_ptr->last_known_epsloci[EMM_USIM_GUTI_OFFSET],
              sizeof(emm_ctrl_data_ptr->last_known_epsloci) - EMM_USIM_GUTI_OFFSET,
              (void*)&epsloci[EMM_USIM_GUTI_OFFSET],
              EPSLOCI_LENGTH);
    }
    else
    {
      MSG_HIGH_DS_0(MM_SUB, "=EMM= EPSLOCI is same as previous");
    }
  } else 
  {
    if( epsloci_status != 0 )
    {  
      efs_status = mcfg_fs_write("/nvm/alpha/modem/nas/lte_nas_eps_loci" , 
                                 epsloci, 
                                 sizeof(epsloci),
                                 MCFG_FS_O_RDWR|MCFG_FS_O_CREAT|MCFG_FS_O_TRUNC|MCFG_FS_O_AUTODIR,
                                 MCFG_FS_ALLPERMS,
                                 MCFG_FS_TYPE_EFS,
                                 (mcfg_fs_sub_id_e_type)mm_sub_id);

      /* update the local copy */
      memscpy((void*)&emm_ctrl_data_ptr->last_known_epsloci[EMM_USIM_GUTI_OFFSET],
              sizeof(emm_ctrl_data_ptr->last_known_epsloci) - EMM_USIM_GUTI_OFFSET,
              (void*)&epsloci[EMM_USIM_GUTI_OFFSET],
              EPSLOCI_LENGTH);
    }
    else
    {
      MSG_HIGH_DS_0(MM_SUB, "=EMM= EPSLOCI is same as previous");
    }
  }

  if( ( !mmgsdi_status ) || ( efs_status != MCFG_FS_STATUS_OK ) )
  {
     MSG_ERROR_DS_2(MM_SUB, "=EMM= GUTI can not be saved, status - %d-%d)", mmgsdi_status, efs_status);
     //emm_ctrl_data_ptr->plmn_service_state->sim_state = SYS_SIM_STATE_NOT_AVAILABLE;
     //emm_invalidate_mobility_info(EMM_STATUS_NOT_UPDATED, 
     //                             SYS_SIM_STATE_NOT_AVAILABLE, 
     //                             emm_ctrl_data_ptr, TRUE) ;
  }
} /* end of emm_save_mobility_info() */

/*===========================================================================

FUNCTION  EMM_INVALIDATE_MOBILITY_INFO

DESCRIPTION 
  This function invalidates mobility information 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_invalidate_mobility_info
(
  emm_eps_update_status_type  emm_update_status,
  sys_sim_state_e_type        sim_state,
  emm_ctrl_data_type         *emm_ctrl_data_ptr,
  boolean                     delete_EPLMN_list
)
{
  boolean mmgsdi_status = TRUE;
  byte                    epsloci[EPSLOCI_LENGTH];
  word                    pos = 0 ;
  byte                    plmn_size = 3; 
  mcfg_fs_status_e_type   efs_status = MCFG_FS_STATUS_OK;

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 

  if(mm_sim_card_mode != MMGSDI_APP_USIM)  
  {
     MSG_ERROR_DS_0(MM_SUB, "=EMM= SIM either not initialized or invalid");
     return;
  }

  MSG_HIGH_DS_1(MM_SUB, "=EMM= LAST_REGISTERED_TAI valid = %d",
                emm_ctrl_data_ptr->last_registered_tai_valid);
  if(emm_ctrl_data_ptr->last_registered_tai_valid == FALSE) /*case where SIM data was not copied into 'emm_ctrl_data'*/
  {
    if(FALSE == emm_load_epsloci_data(emm_ctrl_data_ptr))
    {      
      MSG_ERROR_DS_0(MM_SUB, "=EMM= SIM read failed, this must not happen in this stage because we end up writing incorrect values in LAST_REG_PLMN");
    }
  }

  /* Delete GUTI */
  MSG_HIGH_DS_0(MM_SUB, "=EMM= Delete GUTI, and Delete last visited registered TAI");
  emm_ctrl_data_ptr->emm_guti_valid = FALSE ;

  memset((void *)&emm_ctrl_data_ptr->emm_guti, 0xFF, sizeof(lte_nas_emm_guti_type)) ;
  emm_ctrl_data_ptr->plmn_service_state->rac_or_mme_code = 0xFF;
  encode_guti_ie(&emm_ctrl_data_ptr->emm_guti,&pos,&epsloci[EMM_USIM_GUTI_OFFSET]);

  /* Delete last visited registered TAI */ 

  emm_ctrl_data_ptr->last_registered_tai_valid = FALSE ;
  emm_ctrl_data_ptr->last_registered_tai.tac =  0xFFFE;
  memscpy((void*)&epsloci[EMM_GUTI_LENGTH],
          sizeof(epsloci) - EMM_GUTI_LENGTH,
          (void*)&emm_ctrl_data_ptr->last_registered_tai.plmn,
          plmn_size);
  epsloci[EMM_GUTI_LENGTH + plmn_size] = 0xFF;
  epsloci[EMM_GUTI_LENGTH + plmn_size + 1] = 0xFE;

  /* Set EPS update status */
  MSG_HIGH_DS_1(MM_SUB,"=EMM= Set EPS update status to %d (Invalidate mobility)",
               (int)emm_update_status);
  emm_ctrl_data_ptr->emm_update_status  = emm_update_status ;
  epsloci[EMM_UPDATE_STATUS] = (byte)emm_update_status ;

  if ( emm_ctrl_data_ptr->eps_storage_supported )
  {
    mmgsdi_status = mm_sim_write(MMGSDI_USIM_EPSLOCI, &epsloci[EMM_USIM_GUTI_OFFSET], EPSLOCI_LENGTH);
  }
  else 
  {
    efs_status = mcfg_fs_write("/nvm/alpha/modem/nas/lte_nas_eps_loci" , 
                               epsloci, 
                               sizeof(epsloci), 
                               MCFG_FS_O_RDWR|MCFG_FS_O_CREAT|MCFG_FS_O_TRUNC|MCFG_FS_O_AUTODIR,
                               MCFG_FS_ALLPERMS,
                               MCFG_FS_TYPE_EFS,
                               (mcfg_fs_sub_id_e_type)mm_sub_id);
  }

  if(( !mmgsdi_status ) || ( efs_status != MCFG_FS_STATUS_OK ) )
  {
    MSG_ERROR_DS_2(MM_SUB, "=EMM= GUTI can not be saved, status - %d-%d", mmgsdi_status, efs_status);
     //emm_ctrl_data_ptr->plmn_service_state->sim_state = SYS_SIM_STATE_NOT_AVAILABLE;
     //return;
  }

  /* update the global copy */
  MSG_HIGH_DS_0(MM_SUB, "=EMM= Saving the (invalidated) last known ESPLOCI info");
  memscpy((void*)&emm_ctrl_data_ptr->last_known_epsloci[EMM_USIM_GUTI_OFFSET],
          sizeof(emm_ctrl_data_ptr->last_known_epsloci) - EMM_USIM_GUTI_OFFSET,
          (void*)&epsloci[EMM_USIM_GUTI_OFFSET],
          EPSLOCI_LENGTH);
    
  /* Delete TAI list */ 
  if(emm_ctrl_data_ptr->emm_tai_list_ptr != NULL)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Delete TAI list");
    emm_ctrl_data_ptr->emm_tai_list_ptr->tai_lst_length = 0 ;
  }

  /* Invalidate NAS Security parameters */ 
  MSG_HIGH_DS_0(MM_SUB, "=EMM= Delete KSI");
  emm_invalidate_nas_security_params(emm_ctrl_data_ptr);

  /* EPLMN list */ 
  if(delete_EPLMN_list == TRUE)
  {  
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Delete list of equivalent PLMNs");
    emm_ctrl_data_ptr->emm_equivalent_PLMN_list.length_of_plmn_lst_contents = 0;
    if(is_lte_mode())
    {
      emm_ctrl_data_ptr->update_eplmn_list=TRUE;
    }
  }
  if(emm_ctrl_data_ptr->update_sim_params == TRUE)
  {
    /* Only process USIM if not GW reject halding */
    if(emm_ctrl_data_ptr->gw_reject_update == FALSE)
    {
      /* Invalidate USIM */
      switch (sim_state)
      {
        case SYS_SIM_STATE_CS_PS_INVALID:
          (void) mmgsdi_session_manage_illegal_subscription(mm_sim_mmgsdi_info.session_id,
                                                              MMGSDI_SESSION_APP_IS_ILLEGAL);
          emm_ctrl_data_ptr->plmn_service_state->sim_state = sim_state ;
          break ;
        case SYS_SIM_STATE_AVAILABLE:
          /* Nothing to do */
          break ;
        case SYS_SIM_STATE_CS_INVALID:
          if (emm_ctrl_data_ptr->plmn_service_state->sim_state == SYS_SIM_STATE_PS_INVALID)
          {
            emm_ctrl_data_ptr->plmn_service_state->sim_state = SYS_SIM_STATE_CS_PS_INVALID ;
           (void) mmgsdi_session_manage_illegal_subscription(mm_sim_mmgsdi_info.session_id,
                                                           MMGSDI_SESSION_APP_IS_ILLEGAL);
          }
          else
          {
            emm_ctrl_data_ptr->plmn_service_state->sim_state = sim_state ;
          }
          break;
        case SYS_SIM_STATE_PS_INVALID:      
          if (emm_ctrl_data_ptr->plmn_service_state->sim_state == SYS_SIM_STATE_CS_INVALID)
          {
            emm_ctrl_data_ptr->plmn_service_state->sim_state = SYS_SIM_STATE_CS_PS_INVALID ;
            (void) mmgsdi_session_manage_illegal_subscription(mm_sim_mmgsdi_info.session_id,
                                                           MMGSDI_SESSION_APP_IS_ILLEGAL);
          }
          else
          {
            emm_ctrl_data_ptr->plmn_service_state->sim_state = sim_state ;
          }
          break ;
        default:
          MSG_ERROR_DS_0(MM_SUB, "=EMM= Illigal SIM state");
          break ;
      }

      emm_send_sim_update(emm_ctrl_data_ptr);
      emm_send_rrc_system_update(emm_ctrl_data_ptr) ;
    }
}

} /* end of emm_invalidate_mobility_info() */

#endif /*FEATURE_LTE*/
