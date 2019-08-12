#ifndef TM_1X_UP_H
#define TM_1X_UP_H

#include "gps_variation.h"
#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                  TM_1X_UP Header File

DESCRIPTION
  This header file contains definitions of data structure necessary for TM 1X UP module

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/1x_up/inc/tm_1x_up.h#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/17/14   skm    NV Async Read changes 
07/02/08    ank     Added LBS support for CDG OMH R-UIM Spec
12/22/06    ank     Added support for MSBased Throttling feature.
12/11/06    ank     Changes to read NV items.
11/15/06    ank     Added tm_1x_up_create_custom_header for Vx and Trusted call flows.
                    Added TIMER and TRANSACTION_ID releated defines.
10/25/06    ank     Initial Release
===========================================================================*/

/*=============================================================================

                                INCLUDE FILES

=============================================================================*/

/*####################################################

        Data Strutture Definition Section

####################################################*/
/* 
   TM_1X_UP_TIMER_ID_RANGE = 0x02000000
*/
#define TM_1X_UP_TIMER_ID_DISPATCH_MASK     0xFFFF0000

/* IS801 session driver should use timer ID range from 0x02000000 - 0x0200FFFF*/
#define TM_1X_UP_IS801_DRV_TIMER_ID_BASE    TM_1X_UP_TIMER_ID_RANGE
#define TM_1X_UP_IS801_DRV_TIMER_ID_RANGE   0x00010000

/* Vx module should use timer ID range from 0x02010000 - 0x0201FFFF*/
#define TM_1X_UP_VX_TIMER_ID_BASE           (TM_1X_UP_IS801_DRV_TIMER_ID_BASE + TM_1X_UP_IS801_DRV_TIMER_ID_RANGE)
#define TM_1X_UP_VX_TIMER_ID_RANGE          0x00010000

/* KDDI module should use timer ID range from 0x02020000 - 0x0202FFFF*/
#define TM_1X_UP_KDDI_TIMER_ID_BASE         (TM_1X_UP_VX_TIMER_ID_BASE + TM_1X_UP_VX_TIMER_ID_RANGE)
#define TM_1X_UP_KDDI_TIMER_ID_RANGE        0x00010000

/* 
   TM_1X_UP_TRANSACTION_ID_RANGE = 0x01000000
*/
#define TM_1X_UP_TRANSACTION_ID_DISPATCH_MASK     0xFFFF0000

/* IS801 session driver should use transaction ID range from 0x01000000 - 0x0100FFFF */
#define TM_1X_UP_IS801_DRV_TRANSACTION_ID_BASE    TM_1X_UP_TRANSACTION_ID_RANGE
#define TM_1X_UP_IS801_DRV_TRANSACTION_ID_RANGE   0x00010000

/* Vx module should use transaction ID range from 0x01010000 - 0x0101FFFF */
#define  TM_1X_UP_VX_TRANSACTION_ID_BASE          (TM_1X_UP_IS801_DRV_TRANSACTION_ID_BASE + TM_1X_UP_IS801_DRV_TRANSACTION_ID_RANGE)
#define  TM_1X_UP_VX_TRANSACTION_ID_RANGE         0x00010000

#define TM_PDE_MAX_URL_SIZE  128

/* MSBased aiding throttling feature */
typedef struct
{
    byte                         msb_throttling_enabled;  /* Throttling is enabled for msb */
    byte                         back_off_factor;         /* Exponential back-off factor */
    qword                        back_off_time;           /* Current back off time */
    qword                        back_off_min_duration;   /* Min back-off duration */
    qword                        back_off_max_duration;   /* Max back-off duration */
    qword                        back_off_reset_duration; /* Safe time to reset the throttling */
    qword                        throttle_end_time;
    qword                        msb_last_request_time;
	boolean						  mpc_throttle_check;      /* Set when MPC throttling checks have been performed */
#ifdef FEATURE_CALL_THROTTLE          		
    boolean                      safe_dload_req;
#endif /* FEATURE_CALL_THROTTLE */

} tm_1x_up_msb_throttling_info_s_type;

/* Data structure for static information */
typedef struct
{
    dword                      pde_adrs;
    dword                      port_id;
    char                       pde_addr_url[TM_PDE_MAX_URL_SIZE];
    boolean                    coarse_location_enabled;
    boolean                    support_is801_b;
    boolean                    support_gps;
    boolean                    support_glo;
    boolean                    assisted_session;
    tm_1x_up_msb_throttling_info_s_type msb_throttling_info;
} tm_1x_up_config_s_type;

/* MACRO to convert time from seconds to qword for throttling.
*/
#define convert_to_qword(a,b) \
{\
      qw_set(a,0,b);\
      qw_mul(a,a,1000);\
      qw_shift(a,16);\
      qw_mul(a,a,4);\
      (void)qw_div(a,a,5);\
}
    
/*===========================================================================

FUNCTION tm_1x_up_create_custom_header

DESCRIPTION
  This function creates the custom header required for TCP/IP implementation.

DEPENDENCIES
  None

RETURN VALUE
  Length of created header in bytes.

SIDE EFFECTS
  None

===========================================================================*/

extern word tm_1x_up_create_custom_header(byte* start);

/*===========================================================================

FUNCTION   tm_1x_up_read_msid_fields

DESCRIPTION
  This function is used to read NAM parameters from NV for initializing MSID

RETURN VALUE
  None

============================================================================*/
void tm_1x_up_read_msid_fields( void );

/*===========================================================================

FUNCTION   tm_1x_up_read_msid_fields_complete

DESCRIPTION
  This function is used to update NAM parameters from NV for initializing MSID.
  It is called once NV Async read operation is complete.

RETURN VALUE
  None

============================================================================*/
void tm_1x_up_read_msid_fields_complete( const prtl_event_u_type  *event_payload );

/*===========================================================================

FUNCTION tm_1xup_prtl_init

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern void tm_1xup_prtl_init(void);


#ifdef FEATURE_UIM_SUPPORT_LBS
/*===========================================================================

FUNCTION tm_1x_up_ruim_read

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern void tm_1x_up_ruim_read(void);

/*===========================================================================

FUNCTION tm_1x_up_ruim_read_complete

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern void tm_1x_up_ruim_read_complete(const prtl_event_u_type  *event_payload);

#endif /* FEATURE_UIM_SUPPORT_LBS */

#ifdef __cplusplus
}
#endif

#endif /* TM_1X_UP_H */

