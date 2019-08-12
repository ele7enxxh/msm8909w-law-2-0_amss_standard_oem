#ifndef GMM_H
#define GMM_H
/*===========================================================================

              GPRS MOBILITY MANAGEMENT (GMM) SUB-SYSTEM HEADER FILE

DESCRIPTION
  This file data and function declarations necessary for the GMM sub-system
  of the UMTS/GSM MM task.

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/gmm.h_v   1.9   06 Jun 2002 10:13:56   kishores  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/gmm.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/25/01   jca     Created file.
04/12/01   jca     Merged with MM baseline code.
05/11/01   jca     Moved L3 message definitions to gmm_msg.h
05/23/01   jca     Added gmm_service_state_type.
08/03/01   jca     Modified gmm_handle_plmn_found() function prototype.
08/15/01   ab      Rename mmi_reg.h to reg_cm.h.
02/20/02   sbk     CSN encoder function prototypes added
06/05/02   ks      Added support for Dual mode, consolidated the
                   PLMN informatin and GMM state information.
08/01/03   jca     Added new substate GMM_NULL_SUBSTATE.
08/27/04   ab      CR39294. Updated the NPDU list primitives.
02/28/04   ks      Externalized function set_access_tech_info() so that
                   MAC uses this function directly for supporting EGPRS.
12/18/09   RI      Renamed gmm_load_sim_data() to gmm_load_psloci_data()
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "mm_mm.h"
#include "mm.h"
#include "mmtask.h"
#include "mmtask_v.h"
#include "ghdi_exp.h"
#include "ghdi_exp_v.h"



/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*=======================
 CSN encoder structure
 ========================*/
typedef struct csn_encoder
{
   byte *buf;
   uint16 buf_size;
   byte byte_pos;
   byte bit_pos;
} csn_encoder_T;




/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/


/*================================================
   CSN encoder functions
 ================================================*/
extern void    csn_encoder_init(csn_encoder_T *csn, byte *buf, uint16 size);
extern uint16  csn_bit_pos(csn_encoder_T *csn);
extern void    csn_encode_bit(csn_encoder_T *csn, byte val);
extern void    csn_encode_bits(csn_encoder_T *csn, byte num_bits, byte val);
extern byte    csn_decode_bit(csn_encoder_T *csn);
extern byte    csn_decode_bits(csn_encoder_T *csn, byte num_bits);
extern void    csn_fill_spare_bits(csn_encoder_T *csn);
extern void    csn_encode_bits_at_pos(csn_encoder_T *csn, byte num_bits, byte val, uint16 pos);


extern boolean set_access_tech_info_per_subs
(
  csn_encoder_T  *encoder ,
  byte           access_technology ,
  byte           revision_level_ind,
  byte           rf_power_class,
  hw_classmark_T *hw_classmark,
  boolean        *A5_already_included,
  boolean        *gprs_msclass_already_included


  , word           max_rac_length /*0 means no length specified*/

 ,sys_modem_as_id_e_type  as_id
);

extern boolean set_access_tech_info
(
  csn_encoder_T  *encoder ,
  byte           access_technology ,
  byte           revision_level_ind,
  byte           rf_power_class,
  hw_classmark_T *hw_classmark,
  boolean        *A5_already_included,
  boolean        *gprs_msclass_already_included

 
  , word           max_rac_length /*0 means no length specified*/

);

extern byte gmm_read_gprs_multislot_class(void);

extern byte gmm_read_edge_multislot_class(void);

#endif /* GMM_H */
