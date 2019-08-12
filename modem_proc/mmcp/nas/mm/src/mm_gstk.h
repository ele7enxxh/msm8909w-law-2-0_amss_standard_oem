#ifndef mm_gstk_h
#define mm_gstk_h
/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2002, 2003 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mm_gstk.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/06/03   cd     Initial version.
01/28/03   cd     Added tracking of version control
05/26/05   abe    Added prototype declaration of
                  mm_gstk_is_location_status_evt_reqd &
                  mm_gstk_set_location_status_evt.
10/11/05   vdr    o Added prototype for mm_send_gstk_no_service_info() 
===========================================================================*/ 



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/
#ifdef FEATURE_GSTK
boolean mm_get_mcc_mnc_lac(uint8 *mcc_mnc, uint8 *lac);
void mm_send_gstk_mm_idle_info(void);
void mm_gstk_set_location_status_evt (boolean flag);
boolean mm_gstk_is_location_status_evt_reqd (void);
void mm_send_gstk_no_service_info(void);
void mm_send_gstk_gprs_attach_reject_evt(byte);
void mm_send_gstk_routing_area_update_reject_evt(byte);
void mm_send_gstk_location_updating_reject_evt(byte, byte);
#ifdef FEATURE_SGLTE
void mm_send_gstk_mm_event(gstk_io_send_mm_ind_enum_type , mm_as_id_e_type );
#endif
#endif /* FEATURE_GSTK */

#endif /* mm_gstk_h */
