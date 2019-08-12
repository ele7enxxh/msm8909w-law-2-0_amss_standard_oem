/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001,2002,2003 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mmsysinfo.c_v   1.7   01 Jul 2002 12:00:54   jault  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mmsysinfo.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/03/01   sbk     Global variables are renamed to use mm_ prefix
10/19/01   sbk     New function to process T3212 value
06/05/02   ks      Added support for Dual mode operation
06/26/02   ks      Added storing of System information received in
                   RR_PLMN_SELECT_CNF in mmgsm_store_system_information.
07/01/02   jca     Rewrote function mm_handle_new_T3212() to fix CR.
07/25/02   jca     Fixed divide-by-zero error in function mm_handle_new_T3212().
08/15/02   ks      Writing the BCCH or CPBCCH based on the SIM mode type
08/30/02   mks     Now calling utility function mmsim_save_bcch_information()
                   to save the BCCH/CPBCCH allocation info in SIM
09/23/02   ks      Initializing service_status to default MM_AS_NO_SERVICE
                   in mmumts_process_T3212(); fixes compiler warning.
10/31/02   ks      Removed calling mmsim_save_bcch_information() as the BCCH
                   List is not supported in RR and no need to write in SIM.
11/08/02   cd      Added storing cell_access information received in a
                   RR_PLMN_SELECT_CNF or in a RR_SERVICE_IND in the system information
11/14/02   ks      T3212 is not started if the UE is operating in GPRS mode,
                   Added support for GPRS revision indicator (SGSNR)
11/15/02   mks     Wrapped the code processing gprs_sys_info parameter in
                   rr_service_ind and rr_plmn_select_cnf_T prmittives using
                   FEATURE_GSM_GPRS
02/10/03   cd      Fixed Lint errors
02/13/03   jca     Now updating mm_serving_lai in call to
                   mmgsm_store_system_information().
11/16/04   sv      Addition of new primitive RRC_SERVICE_IND_FROM_OTA_MSG 
12/14/05   abe     Supported changes for FEATURE_GSM_DTM
07/14/06   pk    Updated for DSAC
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "ms.h"
#include "timers_v.h"
#include "gs_v.h"
#include "ms_timer_v.h"
#include "mmrat.h"
#include "mm_v.h"
#include "mmtask.h"
#include "mmtask_int.h"
#include "mm_sim.h"
#include "err.h"
#include <stringl/stringl.h>

static void mm_handle_new_T3212(dword T3212_timeout);

/*===========================================================================

FUNCTION MM_STORE_SYSTEM_INFORMATION

DESCRIPTION
  This function stores the system information supplied by RRC which is
  required for MM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mmumts_store_system_information( byte *value, dword length, byte msg_id)
{
   dword  new_T3212_timer_value = 0; /* Default - Should always be reset*/

   if(length < 2)
   {
      MSG_ERROR_DS(MM_SUB, "=MM= The CS specific domain information must be atleast 2 bytes in length", 0,0,0);
      return;
   }

   /*
    * convert deci-hours into milliseconds
    */
   new_T3212_timer_value = (dword)value[0] * 360L * 1000L;

   if(msg_id != (byte)RRC_SERVICE_IND_FROM_OTA_MSG)
   {
     if (value[1] & 0x01)
     {
       mm_system_information.ATT = TRUE;
     }
     else
     {
       mm_system_information.ATT = FALSE;
     }
     MSG_HIGH_DS_1(MM_SUB ,"=MM= NON OTA MSG : mm_system_information.ATT = %d", mm_system_information.ATT);
   }


   /* REVISIT - according to Vikas RR takes care of reestablishment */
   mm_system_information.reestablishment_allowed = FALSE;

   if ( new_T3212_timer_value != mm_system_information.T3212_timer_value )
   {
      mm_handle_new_T3212(new_T3212_timer_value);
   }

}


/*===========================================================================

FUNCTION MMGSM_STORE_SYSTEM_INFORMATION

DESCRIPTION
  This function stores the system information supplied by RR which is
  required for MM. This is sent to MM using the RR_UNIT_DATA_IND primitive.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mmgsm_store_system_information( mm_cmd_type* msg_p )
{
   dword T3212_timeout = 0; /* Default - Should always be reset */
   byte  lai[LAI_SIZE] = { 0xff, 0xff, 0xff, 0, 0 };

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   if (msg_p->cmd.hdr.message_set == MS_MM_RR)
   {
      if (msg_p->cmd.hdr.message_id == (int) RR_PLMN_SELECT_CNF)
      {
         mm_system_information.ATT = msg_p->cmd.rr_plmn_select_cnf.ATT;
         MSG_HIGH_DS_1(MM_SUB ,"=MM= mm_system_information.ATT = %d", mm_system_information.ATT);
         mm_system_information.reestablishment_allowed =
            msg_p->cmd.rr_plmn_select_cnf.reestablishment_allowed;
         mm_system_information.mscr = msg_p->cmd.rr_plmn_select_cnf.MSCR;
#ifdef FEATURE_GSM_GPRS
         mm_system_information.sgsnr = msg_p->cmd.rr_plmn_select_cnf.gprs_sys_info.sgsnr;
#endif
         mm_system_information.cell_access = msg_p->cmd.rr_plmn_select_cnf.cell_access;

         if (mm_serving_plmn.ms_op_mode != SYS_SRV_DOMAIN_PS_ONLY)
         {
           /*
           * convert deci-hours into milliseconds
           */
           T3212_timeout = (dword)( msg_p->cmd.rr_plmn_select_cnf.T3212_timeout_value
                                 * 360L * 1000L);

           if (T3212_timeout != mm_system_information.T3212_timer_value)
           {
              mm_handle_new_T3212(T3212_timeout);
           }
         }
         else
         {
           MSG_HIGH_DS_0(MM_SUB, "=MM= Ignore 3212 timer for GPRS");
         }
      }
      else if (msg_p->cmd.hdr.message_id == (int) RR_SERVICE_IND)
      {
         if (msg_p->cmd.rr_service_ind.service_state == MM_AS_SERVICE_AVAILABLE)
         {
            mm_system_information.ATT = msg_p->cmd.rr_service_ind.ATT;
            MSG_HIGH_DS_1(MM_SUB ,"=MM= mm_system_information.ATT = %d", mm_system_information.ATT);
            mm_system_information.reestablishment_allowed =
              msg_p->cmd.rr_service_ind.reestablishment_allowed;
            mm_system_information.mscr = msg_p->cmd.rr_service_ind.MSCR;
#ifdef FEATURE_GSM_GPRS
            mm_system_information.sgsnr = msg_p->cmd.rr_service_ind.gprs_sys_info.sgsnr;
#endif
            mm_system_information.cell_access = msg_p->cmd.rr_service_ind.cell_access;
            if (mm_serving_plmn.ms_op_mode != SYS_SRV_DOMAIN_PS_ONLY)
            {
               /*
               * convert deci-hours into milliseconds
               */
               T3212_timeout =
                 (dword) (msg_p->cmd.rr_service_ind.T3212_timeout_value * 360L * 1000L);

               if (T3212_timeout != mm_system_information.T3212_timer_value)
               {
                  mm_handle_new_T3212(T3212_timeout);
               }
            }
            else
            {
               MSG_MED_DS(MM_SUB, "=MM= Ignore 3212 timer for GPRS",0,0,0);
            }

            lai[0] = mm_serving_plmn.info.plmn.identity[0];
            lai[1] = mm_serving_plmn.info.plmn.identity[1];
            lai[2] = mm_serving_plmn.info.plmn.identity[2];
            lai[3] = mm_serving_plmn.lac.lac[0];
            lai[4] = mm_serving_plmn.lac.lac[1];

            if (memcmp( lai, mm_serving_lai, LAI_SIZE) != 0) /* New LAI */
            {
               memscpy(mm_serving_lai, LAI_SIZE, lai, LAI_SIZE);

               MSG_HIGH_DS_3( MM_SUB, "=MM= Serving PLMN %X %X %X",
                         mm_serving_lai[0], mm_serving_lai[1], mm_serving_lai[2]);

               MSG_HIGH_DS_2(MM_SUB, "=MM= Serving LAC %X %X", mm_serving_lai[3], mm_serving_lai[4]);
            }
         }
         else if (msg_p->cmd.rr_service_ind.service_state == MM_AS_NO_SERVICE)
         {
            (void) memscpy( mm_serving_lai, LAI_SIZE,lai, LAI_SIZE );
            mm_serving_plmn.mm_service_state = SERVICE_NOT_AVAILABLE;
         }
      }
#ifdef FEATURE_GSM_DTM
      else if (msg_p->cmd.hdr.message_id == (int) RR_SERVICE_IND_FROM_OTA_MSG)
      {
         if (msg_p->cmd.rr_service_ind_from_ota_msg.service_state == MM_AS_SERVICE_AVAILABLE)
         {
            MSG_HIGH_DS_1(MM_SUB ,"=MM= OTA_MSG ATT flag not used, curr val mm_system_information.ATT = %d", mm_system_information.ATT);
            mm_system_information.reestablishment_allowed =
              msg_p->cmd.rr_service_ind_from_ota_msg.reestablishment_allowed;
            /*
            ** During HO from MSC-A (connection initiated in MSC-A, R99) to MSC-B (R98 or erlier), 
            ** MSCR should remain same as received in MSC-A. MSCR is not given in dedicated mode.
            ** So MSCR received in OTA MSG is ignored in NAS. MSC-B worked as relay MSC (29.002).
            ** mm_system_information.mscr = msg_p->cmd.rr_service_ind_from_ota_msg.MSCR;
            */
            mm_system_information.cell_access = msg_p->cmd.rr_service_ind_from_ota_msg.cell_access;

            if (mm_serving_plmn.ms_op_mode != SYS_SRV_DOMAIN_PS_ONLY)
            {
               /*
               * convert deci-hours into milliseconds
               */
               T3212_timeout =
                 (dword) (msg_p->cmd.rr_service_ind.T3212_timeout_value * 360L * 1000L);

               if (T3212_timeout != mm_system_information.T3212_timer_value)
               {
                  mm_handle_new_T3212(T3212_timeout);
               }
            }
            else
            {
               MSG_MED_DS(MM_SUB, "=MM= Ignore 3212 timer for GPRS",0,0,0);
            }

            lai[0] = mm_serving_plmn.info.plmn.identity[0];
            lai[1] = mm_serving_plmn.info.plmn.identity[1];
            lai[2] = mm_serving_plmn.info.plmn.identity[2];
            lai[3] = mm_serving_plmn.lac.lac[0];
            lai[4] = mm_serving_plmn.lac.lac[1];

            if (memcmp( lai, mm_serving_lai, LAI_SIZE) != 0) /* New LAI */
            {
               memscpy(mm_serving_lai, LAI_SIZE, lai, LAI_SIZE);

               MSG_HIGH_DS_3( MM_SUB, "=MM= Serving PLMN %X %X %X",
                         mm_serving_lai[0], mm_serving_lai[1], mm_serving_lai[2]);

               MSG_HIGH_DS_2(MM_SUB, "=MM= Serving LAC %X %X", mm_serving_lai[3], mm_serving_lai[4]);
            }
         }
         else if (msg_p->cmd.rr_service_ind_from_ota_msg.service_state == MM_AS_NO_SERVICE)
         {
            (void) memscpy( mm_serving_lai, LAI_SIZE, lai, LAI_SIZE );
            mm_serving_plmn.mm_service_state = SERVICE_NOT_AVAILABLE;
         }
      }
#endif /* FEATURE_GSM_DTM */
      else
      {
         MSG_ERROR_DS( MM_SUB, "=MM= Unexpected Message Id received",0,0,0);
      }
   }
   else
   {
      MSG_ERROR_DS(MM_SUB, "=MM= unexpected message set/id",
               msg_p->cmd.hdr.message_set*0x100 + msg_p->cmd.hdr.message_id, 0,0);
   }
}


/*===========================================================================

FUNCTION MM_HANDLE_NEW_T3212

DESCRIPTION
  This function handles change in T3212 timer due to the cell information
  change

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void mm_handle_new_T3212(dword T3212_timeout)
{
   dword timeout_value;

   /* TS 24.008 4.4.2 */
   if (((mm_idle_substate != MM_NO_CELL_AVAILABLE) &&
       (mm_idle_substate != MM_LIMITED_SERVICE) &&
       (mm_idle_substate != MM_PLMN_SEARCH) &&
       (mm_idle_substate != MM_PLMN_SEARCH_NORMAL_SERVICE))
#if defined(FEATURE_LTE)
      || (mm_interrat_state == MM_LTE_TO_UMTS_INTERRAT_CHANGE_IN_PROGRESS 
           || mm_interrat_state == MM_LTE_TO_GSM_INTERRAT_CHANGE_IN_PROGRESS) 
#endif
       )
   {
#ifdef FEATURE_NAS_REL10
       if (mm_is_per_ms_t3212_present)
       {
           /*
           * Store message.T3212_timeout_value in mm_system_information
           */
           mm_system_information.T3212_timer_value = T3212_timeout;
           MSG_HIGH_DS_0(MM_SUB, "=MM= Broadcasted T3212 will not be used due to presence of Per MS 3212");
       }
       else
#endif
       if (T3212_timeout != mm_system_information.T3212_timer_value)
       {
         /* If no longer have periodic updates */
         if (T3212_timeout == 0)
         {
            mm_stop_timer( TIMER_T3212 );
         }
         /* If we are not beginning T3212 timeouts */
         else if (mm_system_information.T3212_timer_value != 0 &&
                  mm_timer_status [ TIMER_T3212] != TIMER_STOPPED)
         {
                     
            /* Find out how much timeout is left */
            timeout_value = (dword) gs_enquire_timer_ext(
                                                           get_mm_timer_queue(),
                                                           TIMER_T3212,
                                                           TRUE 
                                                        );

            mm_start_timer( TIMER_T3212,
                            timeout_value % T3212_timeout );
         }
         else if(mm_state != MM_WAIT_FOR_RR_CONNECTION_LU)   /* value has changed */ 
         {
            /*
             * Get random value in range 0..T3212_timer_value
             */
            timeout_value = mm_get_random_value( T3212_timeout );

            /*
             * Restart timer with this value
             */
            mm_start_timer( TIMER_T3212, timeout_value );
         }

         /*
          * Store message.T3212_timeout_value in mm_system_information
          */
         mm_system_information.T3212_timer_value = T3212_timeout;

#ifdef FEATURE_DUAL_SIM
         if(MM_MODEM_STATISTICS_SUB1)
#endif
         {
           mm_set_signal_for_cm(SYS_NAS_PERIODIC_TIMER_MASK);
         }
      }
   }
}


/*===========================================================================

FUNCTION MMUMTS_PROCESS_T3212

DESCRIPTION
  This function processed the new T3212 timer value

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mmumts_process_T3212( mm_cmd_type *cmd )
{
   mm_as_service_state_T service_status;
   rrc_service_info_type service_info ;
   memset((void *)&service_info, 0, sizeof(rrc_service_info_type));

   /* Set the default Service_status to NO_SERVICE */
   service_status = MM_AS_NO_SERVICE;

   if (cmd->cmd.hdr.message_id == (int) RRC_SERVICE_CNF)
   {
      service_status = cmd->cmd.rrc_service_cnf.service_status;
      service_info   = cmd->cmd.rrc_service_cnf.service_info;
   }
   else if (cmd->cmd.hdr.message_id == (int) RRC_SERVICE_IND)
   {
      service_status = cmd->cmd.rrc_service_ind.service_status;
      service_info   = cmd->cmd.rrc_service_ind.service_info;
   }
   else if (cmd->cmd.hdr.message_id == (int) RRC_SERVICE_IND_FROM_OTA_MSG)
   {
      service_status = cmd->cmd.rrc_service_ind_from_ota_msg.service_status;
      service_info   = cmd->cmd.rrc_service_ind_from_ota_msg.service_info;
   }
   else
   {
      MSG_FATAL_DS( MM_SUB, "=MM= Unexpected Message Id %d", cmd->cmd.hdr.message_id, 0,0 );
   }

   if (service_status == MM_AS_SERVICE_AVAILABLE)
   {
      byte i;

      for (i=0; i < service_info.num_cn_domains; i++)
      {
         if (service_info.cn_domain_info[i].cn_domain_id == RRC_CS_DOMAIN_CN_ID)
         {
            mmumts_store_system_information(
               service_info.cn_domain_info[i].cn_domain_specific_info.gsm_map_info.value,
               service_info.cn_domain_info[i].cn_domain_specific_info.gsm_map_info.length,
               cmd->cmd.hdr.message_id
               );
         }
      }
   }
}
