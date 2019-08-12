#ifndef TDSL2EXTERNAL_H
#define TDSL2EXTERNAL_H
/*===========================================================================



                    TDS L2 EXTERNAL HEADER FILE



DESCRIPTION

  This file contains L2 data structures, variables, enums and function prototypes

  needed to be used outside TD-SCDMA


Copyright (c) 2000-2010 Qualcomm Technologies, Incorporated.  All Rights Reserved.

QUALCOMM Proprietary.  Export of this technology or software is regulated

by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/api/public/tdsl2external.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
6/9/14     sarao     Added wrapper function for APIs
11/23/10   cyx       Remove mvssup.h, mvs.h ,tdsuecomdef.h from included files
11/22/10   cyx       Initial Release
===========================================================================*/
#include "comdef.h"
#include "sys.h"
#include "dsm.h"        

/*************************************************************************/
/**************                    CMD_ID for APIs          **************/
/*************************************************************************/

#define TDSL2_CMD_UL_SERVICE_REGISTER  0 
#define TDSL2_CMD_DL_SERVICE_REGISTER  1 
#define TDSL2_CMD_VFR_NOTIFICATION_REGISTER 2


/*************************************************************************/
/**************                    L2 interface             **************/
/*************************************************************************/

#define   TDSL2_MAX_UL_LC   16
#define   TDSL2_MAX_DL_LC   16

typedef struct {

  boolean service;      /* TRUE- Register/FASLE-De-register */
  uint8 nchan;          /* Number of LC to register/de-register */
  uint8 rlc_id [TDSL2_MAX_UL_LC];    
                        /* Unique identifier for the UL/DL  LC */
  dsm_watermark_type  *ul_wm_ptr[TDSL2_MAX_UL_LC];    
                        /* WM for storing data. Used ony for Register */

}tdsl2_ul_service_register_type;

typedef struct {

  boolean service;      /* TRUE - Register/FASE-De-register */
  uint8 nchan;          /* Number of LC to register/de-register */
  uint8 rlc_id [TDSL2_MAX_DL_LC];    
                        /* Unique identifier for the UL/DL  LC */
  dsm_watermark_type  *dl_wm_ptr[TDSL2_MAX_DL_LC];    
                        /* WM for storing data. Used ony for Register */

  boolean context[TDSL2_MAX_DL_LC];                      
                        /* Task (FALSE) or Interrupt (TRUE)*/
  void *rlc_post_rx_func_ptr_para[TDSL2_MAX_DL_LC];      
                        /* 3rd parameter for rlc_post_rx_proc_func_ptr()   */
  void  (*rlc_post_rx_proc_func_ptr[TDSL2_MAX_DL_LC])(uint8, uint8, void *);
                                        /* The callback function that is   */
                                        /* called by RLC when data is      */
                                        /* available in the downlink queue,*/
                                        /* for further processing by upper */
                                        /* layers.This function passes     */
                                        /* logical channel id as parameter.*/
} tdsl2_dl_service_register_type;


typedef enum
{
	TDS_EOK = 0,
	TDS_EFAILED = 1,
	TDS_EBADPARAM = 2,
	TDS_EUNSUPPORTED = 3,
	TDS_ENOMEM = 4,

} tds_l2_api_ret_value_t;


typedef enum
{

	TDS_VFR_STATE_READY = 0, 
		/**< VFR strobe is stable and now aligned with the network timeline.
			*  This means VFR is reliable and usable, and voice can packet
			*  exchange with modem.
			*/
	TDS_VFR_STATE_NOT_READY =1,
		/**< VFR strobe is not stable. Voice shall not packet exchange 
			*  with modem in this state.
			*/

}tds_vfr_state_t;

typedef struct
{
tds_vfr_state_t vfr_state;  /* VFR_STATE: */
uint32 tds_sys_time;  /* TDS Stmr value which is lower 29 bits */
} tds_vfr_info_type;


typedef void (*tds_vfr_cb_type)(tds_vfr_info_type vfr_info, void* client_data);
extern tds_vfr_cb_type tds_vfr_cb_ptr; //This is a local copy of cb_ptr for saving MVS CB ptr during registration


/*===========================================================================

FUNCTION:       tdscdma_register_vfr_notification

DESCRIPTION:
  This function should be called by MVS to register for silent frame vfr notification.

DEPENDENCIES:
  None.

RETURN VALUE:
  Boolean.

SIDE EFFECTS:
  None

===========================================================================*/
void  tdscdma_register_vfr_notification

(
	tds_vfr_cb_type cb_ptr, /* CB pointer which will be saved */
	void* client_data
 
);





/*===========================================================================
FUNCTION:        tdsl2_ul_service_register

DESCRIPTION:
  This function should be called to send a L2 command to L2-UL task for 
  water mark registering or de-registering.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  Command will be enqueued in L2-UL or L2-DL task queue and a signal will be set 
  to indicate that the item has been enqueued.

===========================================================================*/
extern  boolean  tdsl2_ul_service_register

(
  tdsl2_ul_service_register_type *srvc_ptr
);

/*===========================================================================

FUNCTION:        tdsl2_dl_service_register

DESCRIPTION:
  This function should be called to send a L2 command to L2-DL task 
  for water mark registering or de-registering.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  Command will be enqueued in  L2-DL task queue and a signal will be set 
  to indicate that the item has been enqueued.

===========================================================================*/
extern  boolean  tdsl2_dl_service_register

(
  tdsl2_dl_service_register_type *srvc_ptr
);


/**===========================================================================
 
 FUNCTION:        tdscmda_l2_api_call
 
 DESCRIPTION  This function implements the wrapper for all public APIs. This wrapper function can be used by external modules to use 
 TDSCDMA APIs. Currently this function supports the following  L2 external APIs tdsl2_ul_service_register(), tdsl2_dl_service_register()
 and  tdscdma_register_vfr_notification().
 
 Instead of calling the APIs directly, MVS/Audio or other modules can call the generic tdscdma_l2_api_call() and provide the arguments and
 cmd_id suggesting which API they are referring. The wrapper function will internally call the APIs and provide the result back.
 
 The return value is defined in enum tds_l2_api_ret_value_t. Users can use this enum to find out the error cause.
 
 DEPENDENCIES:
  None.

RETURN VALUE:
  .tds_l2_api_ret_value_t

SIDE EFFECTS:
None.
 
 ****************************************************************************/

tds_l2_api_ret_value_t tdscdma_l2_api_call 
(
  uint32   cmd_id,   /*This will tell us which API needs to be called */
  void*    params,   /* This will provide the API arguments */
  uint32   size      
);

/**===========================================================================

Structures to call the service apis.

****************************************************************************/
typedef struct tdsl2_cmd_ul_service_register_t tdsl2_cmd_ul_service_register_t;
struct tdsl2_cmd_ul_service_register_t
{
  tdsl2_ul_service_register_type* service;
    /* The caller provides a pointer to the UL service registration data structure. */
};

typedef struct tdsl2_cmd_dl_service_register_t tdsl2_cmd_dl_service_register_t;
struct tdsl2_cmd_dl_service_register_t
{
  tdsl2_dl_service_register_type* service;
    /* The caller provides a pointer to the DL service registration data structure. */
};

typedef struct tdscdma_register_vfr_notification_t tdscdma_register_vfr_notification_t;
struct tdscdma_register_vfr_notification_t
{
	tds_vfr_cb_type service; /* CB pointer which will be saved */
	void* client_dt;
};


#endif /* TDSL2EXTERNAL_H */
