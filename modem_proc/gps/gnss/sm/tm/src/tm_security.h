#ifndef TM_SECURITY_H
#define TM_SECURITY_H

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                  TM_SECURITY Header File

DESCRIPTION
  This header file contains definitions of data structure necessary for
  TM security layer.

  Copyright (c) 2000 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/src/tm_security.h#1 $  

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/20/07    cl     Initial Release
===========================================================================*/

#include "customer.h"  /* Customer configuration file */
#include "comdef.h"    /* Definition for basic types and macros */

#include "aries_os_api.h"
#include "pdapi.h"
#include "tm_pdapi_iface.h"

/*===========================================================================

                        Data Definition
                 
===========================================================================*/
typedef uint32 tm_sec_data_validity_mask_type;
#define        TM_SEC_DATA_VALIDITY_SSD      0x0001
#define        TM_SEC_DATA_VALIDITY_RAND     0x0002

/* Source of random challenge trigger */
typedef enum
{
  TM_SEC_RND_UPDATE_SOURCE_GET_POS,
  TM_SEC_RND_UPDATE_SOURCE_TIMER
} tm_sec_rnd_update_source_e_type;


typedef struct
{
  tm_sec_data_validity_mask_type     data_validity_mask;
  pdsm_pd_ssd_s_type                 base_ssd;
  pdsm_pd_sec_update_rate_s_type     rand_update_rate;
} tm_sec_enable_data_s_type;

/*===========================================================================

FUNCTION tm_sec_enable

DESCRIPTION
  This function turns on the security layer
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_sec_enable
(
  tm_sec_enable_data_s_type  *enable_data_ptr,
  uint8                       flag  
);

/*===========================================================================

FUNCTION tm_sec_init

DESCRIPTION
  This function is to initialize TM security layer based on NV settings.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_sec_init(void);

/*===========================================================================

FUNCTION tm_sec_init

DESCRIPTION
  This function is to initialize TM security layer based on NV settings.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean tm_sec_get_pos_check(const tm_pdapi_get_pos_param_s_type *get_pos_param_ptr);

/*===========================================================================

FUNCTION tm_sec_handle_timer_expiry

DESCRIPTION
  This function is called by TM-core to inform security layer about timer
  expiry event.
  
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void tm_sec_handle_timer_expiry(const os_TimerExpiryType *timer_data);


/*===========================================================================

FUNCTION tm_sec_encrypt_pd_info

DESCRIPTION
  This function is called by TM-core to encrypt pd data.
  
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void tm_sec_encrypt_pd_info(pdsm_pd_info_s_type *pd_info_ptr);

/*===========================================================================

FUNCTION  tm_sec_handle_clnt_activated

DESCRIPTION 
  Handles security client activated
  
RETURN VALUE 
  None

DEPENDENCIES

===========================================================================*/
extern void tm_sec_handle_clnt_activated(void);

/*===========================================================================

FUNCTION tm_sec_update_random_challenge

DESCRIPTION
  This function 
    1. Generate a new random challenge
    2. Generate hash digest
    3. notify client the newly generated random challenge
  
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void tm_sec_update_random_challenge(tm_sec_rnd_update_source_e_type source);

#ifdef __cplusplus
}
#endif

#endif /* TM_SECURITY_H */

