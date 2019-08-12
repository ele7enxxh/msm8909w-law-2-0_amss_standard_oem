#ifndef GHDI_EXP_V_H
#define GHDI_EXP_V_H
/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/ghdi_exp.h_v   1.9   17 Jul 2002 11:52:16   kishores  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/ghdi_exp_v.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/26/04   ab      Added GSM GHDI_TURN_SPEECH_TCH_AMR_ON and
                     GHDI_TURN_SPEECH_AMR_OFF.
05/11/01   jca     Fixed Lint errors.
06/11/01   TS      Added GSM build support.
08/15/01   ab      Renamed all MMI references to CM
09/18/01   sbk     NV related functionality added and prototypes declared
01-Apr-2002  VT    GSM EFR: Added GHDI_TURN_SPEECH_TCH_EF_ON and 
                     GHDI_TURN_SPEECH_TCH_EF_OFF to the enum related to 
                     ghdi_tch_control_T.
                   Actually merged the above changes from rev 1.7.1.0 (branch) 
                     of this file.
05/01/02   rmc     Added ghdi_read_ue_revision_level function
===========================================================================*/
/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "sys_stru.h"
#include "sys_type.h"
#include "nv_items.h" /* NV item definitions */

#include "sys_cnst.h"

#include "mvs.h"

typedef byte                           ghdi_tch_control_T;
enum
{
   GHDI_TURN_DATA_TCH_F_ON = 0,
   GHDI_TURN_DATA_TCH_H_0_ON,
   GHDI_TURN_DATA_TCH_H_1_ON,
   GHDI_TURN_SPEECH_TCH_F_ON,

   GHDI_TURN_SPEECH_TCH_EF_ON,

   GHDI_TURN_SPEECH_TCH_H_0_ON,
   GHDI_TURN_SPEECH_TCH_H_1_ON,
   GHDI_TURN_SPEECH_TCH_AMR_ON,    /* GSM AMR */
   GHDI_TURN_SPEECH_TCH_AMR_WB_ON,    /* GSM AMR WB */
   GHDI_TURN_DATA_TCH_F_OFF,
   GHDI_TURN_DATA_TCH_H_0_OFF,
   GHDI_TURN_DATA_TCH_H_1_OFF,
   GHDI_TURN_SPEECH_TCH_F_OFF,

   GHDI_TURN_SPEECH_TCH_EF_OFF,

   GHDI_TURN_SPEECH_TCH_H_0_OFF,
   GHDI_TURN_SPEECH_TCH_H_1_OFF,
   GHDI_TURN_SPEECH_TCH_AMR_OFF,
   GHDI_TURN_SPEECH_TCH_AMR_WB_OFF,   /* GSM AMR WB */
   GHDI_SUSPEND_SPEECH,
   GHDI_SUSPEND_DATA,
   GHDI_RESUME_SPEECH,
   GHDI_RESUME_DATA
};




typedef enum
{
   CLASS_1,   /* GSM900 or DCS1800 */
   CLASS_2,   /* GSM900 or DCS1800 */
   CLASS_3,   /* GSM900 or DCS1800 */
   CLASS_4,   /* GSM900 only       */
   CLASS_5    /* GSM900 only       */
}
rf_power_capability_T;

typedef struct
{
   boolean A5_1_supported;
   boolean A5_2_supported;
   boolean A5_3_supported;
   boolean A5_4_supported;
   boolean A5_5_supported;
   boolean A5_6_supported;
   boolean A5_7_supported;
   boolean freq_capability;
   byte default_rf_power_capability;
   byte current_rf_power_capability;
} hw_classmark_T;






typedef enum
{
   DAI_NORMAL_OPERATION,
   DAI_SPEECH_DECODER_TEST,
   DAI_SPEECH_ENCODER_TEST,
   DAI_ACOUSTIC_TEST
}
dai_mode_T;

typedef enum
{
   NO_LOOPBACK,
   LOOPBACK_TYPE_A,
   LOOPBACK_TYPE_B,
   LOOPBACK_TYPE_C,
   LOOPBACK_TYPE_D,
   LOOPBACK_TYPE_E,
   LOOPBACK_TYPE_F,
   LOOPBACK_TYPE_I
}
loopback_type_T;


typedef enum
{
   NO_TCH,
   HALF_0_OR_FULL_TCH,
   HALF_1_TCH
}
tch_T;



/* Sampling rates for eAMR */
#define GHDI_EAMR_SR_8000     8000
#define GHDI_EAMR_SR_16000    16000





#define GHDI_CLASSMARK_CHANGED_IND     0x20


/* <EJECT> */
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

ghdi_status_T ghdi_control_tch_gating
(
   ghdi_tch_control_T                  ghdi_tch_control,
   channel_mode_T                      channel_mode
   ,uint32                             vsid
);

ghdi_status_T ghdi_control_subs_tch_gating
(
                                      ghdi_tch_control_T        tch_control,
                                      channel_mode_T                      channel_mode
                                    , uint32                              vsid
                                    , sys_modem_as_id_e_type              as_id
);


ghdi_status_T ghdi_control_tch_gating_subs
(
   ghdi_tch_control_T                  ghdi_tch_control,
   channel_mode_T                      channel_mode,
   uint32                              vsid
);


ghdi_status_T ghdi_control_tch_gating_vs_0
(
   ghdi_tch_control_T   tch_control,
   channel_mode_T       channel_mode
);

ghdi_status_T ghdi_control_tch_gating_vs_1
(
   ghdi_tch_control_T   tch_control,
   channel_mode_T       channel_mode
);


ghdi_status_T ghdi_control_tch_gating_subs_vs_1
(
   ghdi_tch_control_T      tch_control,
   channel_mode_T          channel_mode,
   sys_modem_as_id_e_type  as_id
);

ghdi_status_T ghdi_control_tch_loopback
(
   loopback_type_T  loopback_type,
   tch_T            tch
);



/*----------------------------------
** typedefs for generic nvmem types
**--------------------------------*/
typedef nv_items_enum_type ghdi_nvmem_parameter_id_T;
typedef nv_item_type ghdi_nvmem_data_T;



/* see definition */
extern ghdi_status_T ghdi_nvmem_read
(
   ghdi_nvmem_parameter_id_T           parameter_id,
   ghdi_nvmem_data_T                   *data_ptr
);

/* see definition */
extern ghdi_status_T ghdi_nvmem_write
(
   ghdi_nvmem_parameter_id_T           parameter_id,
   ghdi_nvmem_data_T                   *data_ptr
);




ghdi_status_T ghdi_read_hw_classmark (hw_classmark_T *hw_classmark);

void ghdi_mvs_release(mvs_client_type);
void ghdi_mvs_acquire(mvs_client_type);

void ghdi_mvs_amr_set_amr_mode
(
  mvs_amr_mode_type amr_ul_mode
);

void ghdi_mvs_amr_set_scr_mode
(
  mvs_scr_mode_type scr_mode
);

mvs_amr_mode_type ghdi_mvs_get_ul_amr_mode(void);

void ghdi_mvs_tdscdma_set_ul_channel
(
  rlc_lc_id_type lc_class_a,
  rlc_lc_id_type lc_class_b,
  rlc_lc_id_type lc_class_c,
  mvssup_umts_chan_class_type chan_class
);

void ghdi_mvs_tdscdma_set_dl_channel
(
  rlc_lc_id_type lc_class_a,
  rlc_lc_id_type lc_class_b,
  rlc_lc_id_type lc_class_c,
  mvssup_umts_chan_class_type chan_class
);


/*******************************************************************************
 *
 *  Function name:   ghdi_get_sampling_rate()
 *  --------------
 *
 *  Description:
 *  ------------
 *    RRC/TDSRRC/RR can use this API to get the sampling rate given by MVS for current codec.
 *    MVS can give sampling rate of 16kHz when NB codec is used.
 *    
 *  Parameters:
 *  -----------
 *
 *  Returns: sampling rate stored in GHDI cache.
 *  --------
 *
 ******************************************************************************/

extern uint32  ghdi_get_sampling_rate(void);


/*******************************************************************************
 *
 *  Function name:   ghdi_reset_sampling_rate()
 *  --------------
 *
 *  Description:
 *  ------------
 *    RRC/TDSRRC/RR/NAS can use this API to reset the sampling rate in GHDI cache.
 *    NAS does it at call initiation and call clearing (when no other call is in progress).
 *    AS does it when codec changes to a non narrow band type.
 *    
 *  Parameters:
 *  -----------
 *
 *  Returns: None
 *  --------
 *
 ******************************************************************************/
extern void    ghdi_reset_sampling_rate(void);

#endif /* GHDI_EXP_V_H */
