/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   T C X O M G R _ D I A G - TCXOMGR Diag interface

GENERAL DESCRIPTION
  This module contains functions pertaining to the interface between tcxomgr
  and diag

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document
  are confidential and proprietary information of Qualcomm Technologies
  Incorporated and all rights therein are expressly reserved.  By accepting
  this material the recipient agrees that this material and the information
  contained therein are held in confidence and in trust and will not be used,
  copied, reproduced in whole or in part, nor its contents revealed in any
  manner to others without the express written permission of Qualcomm
  Technologies Incorporated.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/tcxomgr/src/tcxomgr_diag.c#1 $

when         who      what, where, why
----------   ---      ----------------------------------------------------------
06/04/15     tl       Record RGS log packet time in milliseconds
01/30/15     tl       Remove obsolete featurization
01/07/15     tl       Use Tcxomgr NV critical section during NV write operations
11/18/13     mn       Support for client and subscription RGS retrieval log packet.
11/04/13     sm       Remove messages flooding the logs 
10/04/13     mn       Support to save and retrieve subscription and system
                      RGS to/from EFS.
01/29/13     sk       Support for frequency estimate separation
07/23/13     mn       New API to maintain tech specific RGS.
06/12/13     mn       Adding CNo to the GPS XO cal interface.
02/04/13     mn       New log packet for Retrieve RGS.
11/29/12     sk       tcxomgr.nv.xxx changed to tcxomgr.nv->xxx
11/28/12     mn       Fixing KW errors.
10/10/12     sk       Internal diag commands under featurization flag
07/13/12     gs       Support discarded RGS logging
07/10/12     sg       Support for crystal aging feature
05/22/12     mn       Support to reset RGS through the SET_UNCERTAINTY diag command.
04/23/12     sg       Support for factory fine cal removal
02/17/11     mn       Extrapolate RGS XO temp based on old and new cached temp
                      values.
02/10/12     sr       TCXOMGR Feature Cleanup
01/27/12     ns       Support for IFC
07/08/11     ns       Adding RGS updates as log packet and also enchanced
                      Field cal log packets
07/06/11     ag       Correct typo in macro name.
05/25/10     ns       Adding RGS updates as log packet and also enchanced
                      Field cal log packets
11/10/2010   sm       Removed references to QSR_
11/04/2010   sm       Added Support For Fast Field Cal
07/15/2010   sg       Compiler warnings and Lint fixes for genesis targets
02/26/10     bb       Code changes to lower down priority of log message in
                      tcxomgr_log_entry_updated()
02/03/10     ns       IQ logging Support
06/19/09     ag       Macros used for critical section which defines to rex
                      critical section on Q6 targets and INTLOCK on non Q6.
04/22/09     sst      Update invalid temp define
10/20/08     adw      Replaced FEATURE_XO_DIAG w/ FEATURE_TCXOMGR_DIAG.
09/09/2008   sg       Fix lint/klocwork warnings
08/28/2008   adw      Added FEATURE_XO_DIAG to featurize for ULC.
07/11/2008   scm      Check for unknown temperature in rgs_override().
06/30/2008   mca      Suppressed lint errors
06/20/2008   mca      Merge from //depot
04/07/2008   ljl      Supported multi-carrier AFC log.
03/03/2008   va       Use temperature set instead of temp in RGS
12/20/2007   scm      Check for NULL ptr from diagpkt_subsys_alloc().
11/16/2007   va       Remove TCXO Diag cal functions.
                      XO Freq Est logging Support
09/25/2007   cab      Moved tcxomgr global object reference to header file
09/04/2007   cab      Added support for diag control of slope error
                      Field cal logging support
07/26/2007   sst      Change get_time_and_temp to use temp_set_type
07/20/2007   sst      Update FacCal to increase offset size and enable NV write
06/26/2007   sst      More FacCal initializations
06/13/2007   sst      Update some FacCal initializations
06/08/2007   sst      Set cal_test_dc__params.update_nv in FacCal diag cmd
06/06/2007   sst      Added Factory Calibration diag command
03/20/2007   cab      Removed type cast to log_type
03/15/2007   cab      Removed inclusion of mc.h
10/10/2006   cab      Initial version for tcxomgr v3
10/12/2006   cab      Removed TCXOMGR_RGS_NONE
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mcs_variation.h"
#include "customer.h"

#include "tcxomgr.h"
#include "tcxomgr_diag.h"



#include "rex.h"
#include "tcxomgr_i.h"
#include "tcxomgr_rot_client_handling_i.h"
#include "comdef.h"
#include "diagpkt.h"
#include "diagcmd.h"
#include "msg.h"
#include "err.h"
#include "stringl.h"
#include "err.h"

/*===========================================================================
              LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

#define TCXOMGR_TEMP_INVALID(temp) ((temp).xo == TCXOMGR_INVALID_TEMP)

/*-------------------------------------------------------------------------
      Constants
-------------------------------------------------------------------------*/
extern volatile uint16 tcxomgr_uncertainty_value;
extern volatile boolean tcxomgr_disable_uncertainty_flag;
extern volatile int8 tcxomgr_error_value;
extern volatile boolean tcxomgr_disable_rgs_flag;

/* Response identifier for the delay response packet for WPLT DIAG cmd */
static diagpkt_subsys_delayed_rsp_id_type tcxomgr_diag_cal_cmd_delay_rsp_id = 0;

/*-------------------------------------------------------------------------
      Typedefs
-------------------------------------------------------------------------*/
typedef struct
{
  override_cmd_type          temp_table_override_enable;
  tcxomgr_vco_type           temp_table[VCO_TEMP_TABLE_SIZE];

  override_cmd_type          vco_dflt_override_enable;
  tcxomgr_vco_type           vco_dflt;

  override_cmd_type          vco_slope_override_enable;
  uint16                     vco_slope;

  override_cmd_type          slope_range_override_enable;
  uint8                      slope_range;

  override_cmd_type          rgs_override_enable;
  uint32                     rgs_delta_time;
  int16                      rgs_delta_temp;

  boolean                    rfe_slope_override_enable;
} tcxomgr_diag_type;


/*-------------------------------------------------------------------------
     Static Function Prototypes
-------------------------------------------------------------------------*/


static PACK(void *) tcxomgr_diag_temp_table_override  (PACK(void*) req_pkt, uint16 pkt_len);
static PACK(void *) tcxomgr_diag_temp_table_read      (PACK(void*) req_pkt, uint16 pkt_len);
static PACK(void *) tcxomgr_diag_vco_default_override (PACK(void*) req_pkt, uint16 pkt_len);
static PACK(void *) tcxomgr_diag_vco_slope_override   (PACK(void*) req_pkt, uint16 pkt_len);
static PACK(void *) tcxomgr_diag_slope_range_override (PACK(void*) req_pkt, uint16 pkt_len);
static PACK(void *) tcxomgr_diag_rgs_override         (PACK(void*) req_pkt, uint16 pkt_len);
static PACK(void *) tcxomgr_diag_rfe_slope_override   (PACK(void*) req_pkt, uint16 pkt_len);
static PACK(void *) tcxomgr_diag_set_uncertainty      (PACK(void*) req_pkt, uint16 pkt_len);
static PACK(void *) tcxomgr_diag_set_age              (PACK(void*) req_pkt, uint16 pkt_len);
static PACK(void *) tcxomgr_diag_coarse_dc_cal_cmd    (PACK(void*) req_pkt, uint16 pkt_len);


/*-------------------------------------------------------------------------
      Variables
-------------------------------------------------------------------------*/

/* Diag internal variables */
static tcxomgr_diag_type tcxomgr_diag;

static const diagpkt_user_table_entry_type tcxomgr_diag_tbl[] =
{
  /* start id, end id, callback function */
  {(word)TEMP_TABLE_OVERRIDE,
   (word)TEMP_TABLE_OVERRIDE,
   tcxomgr_diag_temp_table_override},
  {(word)TEMP_TABLE_READ,
   (word)TEMP_TABLE_READ,
   tcxomgr_diag_temp_table_read},
  {(word)VCO_DFLT_OVERRIDE,
   (word)VCO_DFLT_OVERRIDE,
   tcxomgr_diag_vco_default_override},
  {(word)VCO_SLOPE_OVERRIDE,
   (word)VCO_SLOPE_OVERRIDE,
   tcxomgr_diag_vco_slope_override},
  {(word)SLOPE_RANGE_OVERRIDE,
   (word)SLOPE_RANGE_OVERRIDE,
   tcxomgr_diag_slope_range_override},
  {(word)RGS_OVERRIDE,
   (word)RGS_OVERRIDE,
   tcxomgr_diag_rgs_override},
  {(word)RFE_SLOPE_OVERRIDE,
   (word)RFE_SLOPE_OVERRIDE,
   tcxomgr_diag_rfe_slope_override},
  {(word)SET_UNCERTAINTY,
   (word)SET_UNCERTAINTY,
   tcxomgr_diag_set_uncertainty},
  {(word)SET_AGE,
   (word)SET_AGE,
   tcxomgr_diag_set_age},
};

static const diagpkt_user_table_entry_type tcxomgr_diag_tbl_v2_delay[] =
{
  /* start id, end id, callback function */
  {(word)COARSE_DC_CAL,
   (word)COARSE_DC_CAL,
   tcxomgr_diag_coarse_dc_cal_cmd},
};
/*===========================================================================
              FUNCTION DEFINITIONS FOR MODULE
===========================================================================*/


/*-------------------------------------------------------------------------
      Static  Functions
-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------
      Diag Table Call Back Functions
-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION TCXOMGR_DIAG_SET_UNCERTAINTY

DESCRIPTION    This function is called when Diag requests the uncertainity
  value to be returned to RATs.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   The TCXOMGR uncertainity will be changed.

===========================================================================*/

static PACK(void *) tcxomgr_diag_set_uncertainty
(
  PACK(void*) req_pkt,  /* request packet */
  uint16 pkt_len        /* request packet length */
)
{

  /* response pkt */
  DIAG_SUBSYS_TCXOMGR_SET_UNCERTAINTY_rsp_type *rsp_ptr;
  uint16                                        uncertainty;
  int8                                         error;
  uint8                                        rgs_status;

  /*-----------------------------------------------------------------------*/

  /* Check if the request if valid */
  if(pkt_len != sizeof(DIAG_SUBSYS_TCXOMGR_SET_UNCERTAINTY_req_type))
  {
    MSG_1(MSG_SSID_TCXOMGR, MSG_LEGACY_ERROR, "Invalid diag request to set uncertainty (%d)",pkt_len);
    return(NULL);
  }

  /* Copy the request from Diag */
  uncertainty =
    ((DIAG_SUBSYS_TCXOMGR_SET_UNCERTAINTY_req_type *)req_pkt)->uncertainty;
  error =
    ((DIAG_SUBSYS_TCXOMGR_SET_UNCERTAINTY_req_type *)req_pkt)->error;
  rgs_status =
    ((DIAG_SUBSYS_TCXOMGR_SET_UNCERTAINTY_req_type *)req_pkt)->rgs_status;

  /* Process the diag message */
  if(uncertainty == 0)
  {
    /* If the value is zero then disable the override */
    tcxomgr_disable_uncertainty_flag = FALSE;
    tcxomgr_uncertainty_value = uncertainty;
  }
  else if (uncertainty <= PPM_FULL_TO_PPM(20))
  {
    /* If the value is between 1 and 20 then set the uncertainity
       and error override */
    tcxomgr_disable_uncertainty_flag = TRUE;
    tcxomgr_uncertainty_value        = uncertainty;
    tcxomgr_error_value              = error;
  }
  else
  {
    MSG_1(MSG_SSID_TCXOMGR, MSG_LEGACY_ERROR,
          "Invalid diag request to set uncertainty. greater than 20ppm (%d)ppb",
          tcxomgr_uncertainty_value);
    return(NULL);
  }

  if(rgs_status == 0)
  {
    tcxomgr_disable_rgs_flag = FALSE;
  }
  else if(rgs_status == 1)
  {
    tcxomgr_disable_rgs_flag = FALSE;
    tcxomgr_reset_all_rgs_values(FALSE);
  }
  else
  {
    tcxomgr_disable_rgs_flag = TRUE;
    tcxomgr_reset_all_rgs_values(FALSE);
  }

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_TCXOMGR_SET_UNCERTAINTY_rsp_type *)
            diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_TCXOMGR,
                (diagpkt_subsys_cmd_code_type)SET_UNCERTAINTY,
              sizeof(DIAG_SUBSYS_TCXOMGR_SET_UNCERTAINTY_rsp_type));

  return(rsp_ptr);

} /* tcxomgr_diag_temp_table_override */


/*===========================================================================

FUNCTION TCXOMGR_DIAG_SET_AGE

DESCRIPTION    This function is called when Diag requests the age
of coeffecients to be changed.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   The TCXOMGR coeffecients age and C0 will be changed.

===========================================================================*/

static PACK(void *) tcxomgr_diag_set_age
(
  PACK(void*) req_pkt,  /* request packet */
  uint16 pkt_len        /* request packet length */
)
{

  /* response pkt */
  DIAG_SUBSYS_TCXOMGR_SET_AGE_rsp_type         *rsp_ptr;
  tcxomgr_cal_field_data_type                  *gps_field_data;
  tcxomgr_cal_field_data_type                  *wwan_field_data;
  uint8                                        age;

  /*-----------------------------------------------------------------------*/

  /* Check if the request if valid */
  if(pkt_len != sizeof(DIAG_SUBSYS_TCXOMGR_SET_AGE_req_type))
  {
    MSG_1(MSG_SSID_TCXOMGR, MSG_LEGACY_ERROR, "Invalid diag request to set age (%d)",pkt_len);
    return(NULL);
  }

  /* Copy the request from Diag */
  age = ((DIAG_SUBSYS_TCXOMGR_SET_AGE_req_type *)req_pkt)->age;

  /* Process the diag message */

  /* get the pointer to WWAN and GPS tables */
  gps_field_data  = &(tcxomgr.fieldcal_data[TCXOMGR_TABLE_GPS].cal_data);
  wwan_field_data  = &(tcxomgr.fieldcal_data[TCXOMGR_TABLE_WWAN].cal_data);


  DBG_2(HIGH, "Diag Age Override: WWAN Current C0: %d, Current Timestamp: %d",
              wwan_field_data->c0, wwan_field_data->timestamp);

  DBG_2(HIGH, "Diag Age Override: GPS Current C0: %d, Current Timestamp: %d",
                gps_field_data->c0, gps_field_data->timestamp);

  /* Offset C0 to account for aging */
  gps_field_data->c0 += age*86*4;
  wwan_field_data->c0 += age*86*4;

  /* Offset the calibration time stamp */
  if(gps_field_data->timestamp > age*2592000)
  {
    gps_field_data->timestamp -= age*2592000;
  }
  else
  {
    gps_field_data->timestamp = 0;
  }

  /* Offset the calibration time stamp */
  if(wwan_field_data->timestamp > age*2592000)
  {
    wwan_field_data->timestamp -= age*2592000;
  }
  else
  {
    wwan_field_data->timestamp = 0;
  }


  DBG_3(HIGH, "Diag Age Override: Age: %d, WWAN new C0: %d, Current Timestamp: %d",
                age, wwan_field_data->c0, wwan_field_data->timestamp);

  DBG_3(HIGH, "Diag Age Override: Age: %d, GPS new C0: %d, Current Timestamp: %d",
                age, gps_field_data->c0, gps_field_data->timestamp);

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_TCXOMGR_SET_AGE_rsp_type *)
            diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_TCXOMGR,
            (diagpkt_subsys_cmd_code_type)SET_AGE,
            sizeof(DIAG_SUBSYS_TCXOMGR_SET_AGE_rsp_type));

  return(rsp_ptr);

} /* tcxomgr_diag_temp_table_override */

/*===========================================================================

FUNCTION TCXOMGR_DIAG_TEMP_TABLE_OVERRIDE

DESCRIPTION    This function is called when Diag requests to enable the
  Temp Table Override.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   New VCO Temp Table Values may be written to NV.

NOTE: Writes to NV may be undone by disabling the override and shutting
  down the phone.  This will restore the old data in RAM, and on shutdown
  the RAM values will be written to NV.


===========================================================================*/

static PACK(void *) tcxomgr_diag_temp_table_override
(
  PACK(void*) req_pkt,  /* request packet */
  uint16 pkt_len         /* request packet length */
)
{
  /*if TCXOMGR_ENABLE_TEST_DIAG not defined, return NUlL */
#ifndef TCXOMGR_ENABLE_TEST_DIAG
  return(NULL);
#else
  int i;
  override_cmd_type old_override;

  /* response pkt */
  DIAG_SUBSYS_TCXOMGR_TEMP_TABLE_OVERRIDE_rsp_type *rsp_ptr;

  /*-----------------------------------------------------------------------*/

  if(pkt_len != sizeof(DIAG_SUBSYS_TCXOMGR_TEMP_TABLE_OVERRIDE_req_type))
  {
    MSG_1(MSG_SSID_TCXOMGR, MSG_LEGACY_ERROR, "Invalid Temp Table Override Request (%d)",pkt_len);
    return(NULL);
  }

  /* Keep the current override state */
  old_override = tcxomgr_diag.temp_table_override_enable;

  /* Store the value from the message */
  tcxomgr_diag.temp_table_override_enable = (override_cmd_type)
     ((DIAG_SUBSYS_TCXOMGR_TEMP_TABLE_OVERRIDE_req_type *)req_pkt)->enable;

  /* Did we just turn the override off? */
  if ( (old_override != NO_OVERRIDE) &&
       (tcxomgr_diag.temp_table_override_enable == NO_OVERRIDE) )
  {
    /* Put the old data back */
    for (i = 0; i < VCO_TEMP_TABLE_SIZE; i++)
    {
      /* Write the old table */
      tcxomgr.vco.temp[i] = tcxomgr_diag.temp_table[i];
    }
  }
  else if (tcxomgr_diag.temp_table_override_enable != NO_OVERRIDE)
  {
    for ( i = 0; i < VCO_TEMP_TABLE_SIZE; i++ )
    {
      /* Store the old table */
      tcxomgr_diag.temp_table[i] = tcxomgr.vco.temp[i];

      /* Overwrite the old value */
      tcxomgr.vco.temp[i] = (tcxomgr_vco_type)
                           ((DIAG_SUBSYS_TCXOMGR_TEMP_TABLE_OVERRIDE_req_type *)req_pkt)->temp[i];
    }
  }

  /* Check if we need to write to NV */
  if ( tcxomgr_diag.temp_table_override_enable == OVERRIDE_AND_WRITE )
  {
    rex_enter_crit_sect(&tcxomgr.nv->crit_sect);

    /* Copy the data into the nv data struct */
    for ( i=0; i<VCO_TEMP_TABLE_SIZE; i++ )
    {
      tcxomgr.nv->item.vco_temp[i] = tcxomgr.vco.temp[i];
    }

    /* Write the data into NV */
    (void) tcxomgr_nv_cmd( NV_WRITE_F, NV_VCO_TEMP_I );

    rex_leave_crit_sect(&tcxomgr.nv->crit_sect);
  }

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_TCXOMGR_TEMP_TABLE_OVERRIDE_rsp_type *)
            diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_TCXOMGR,
                (diagpkt_subsys_cmd_code_type)TEMP_TABLE_OVERRIDE,
              sizeof(DIAG_SUBSYS_TCXOMGR_TEMP_TABLE_OVERRIDE_rsp_type));

  return(rsp_ptr);
#endif /* TCXOMGR_ENABLE_TEST_DIAG */
} /* tcxomgr_diag_temp_table_override */


/*===========================================================================

FUNCTION TCXOMGR_DIAG_TEMP_TABLE_READ

DESCRIPTION    This function is called when Diag requests to read the
  temp table.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
static PACK(void *) tcxomgr_diag_temp_table_read
(
  PACK(void*) req_pkt,  /* request packet */
  uint16 pkt_len         /* request packet length */
)
{
  /*if TCXOMGR_ENABLE_TEST_DIAG not defined, return NUlL */
#ifndef TCXOMGR_ENABLE_TEST_DIAG
  return(NULL);
#else
  DIAG_SUBSYS_TCXOMGR_TEMP_TABLE_READ_rsp_type  *rsp_ptr;

  const int rsp_len = sizeof(DIAG_SUBSYS_TCXOMGR_TEMP_TABLE_READ_rsp_type);

  int i;

  /*-----------------------------------------------------------------------*/

  TCXOMGR_NOTUSED(req_pkt);
  TCXOMGR_NOTUSED(pkt_len);

  /* Allocate memory for this packet */
  rsp_ptr = (DIAG_SUBSYS_TCXOMGR_TEMP_TABLE_READ_rsp_type *)
              diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_TCXOMGR,
                                   (diagpkt_subsys_cmd_code_type)TEMP_TABLE_READ,
                                   rsp_len);

  if (rsp_ptr)
  {
    for (i=0; i<VCO_TEMP_TABLE_SIZE; i++)
    {
      rsp_ptr->temp[i] = tcxomgr.vco.temp[i];
    }
  }
  else
  {
    MSG_3(MSG_SSID_TCXOMGR, MSG_LEGACY_ERROR,
          "diagpkt_subsys_alloc(%d, %d, %d) returned NULL",
          DIAG_SUBSYS_TCXOMGR, TEMP_TABLE_READ, rsp_len);
  }

  return rsp_ptr;
#endif /* TCXOMGR_ENABLE_TEST_DIAG */
}




/*===========================================================================

FUNCTION TCXOMGR_DIAG_VCO_DEFAULT_OVERRIDE

DESCRIPTION    This function is called when Diag requests to enable the
  VCO Default Override.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   New VCO Default Value may be written to NV.

NOTE: Writes to NV may be undone by disabling the override and shutting
  down the phone.  This will restore the old data in RAM, and on shutdown
  the RAM values will be written to NV.


===========================================================================*/
static PACK(void *) tcxomgr_diag_vco_default_override
(
  PACK(void*) req_pkt,  /* request packet */
  uint16 pkt_len         /* request packet length */
)
{
  /*if TCXOMGR_ENABLE_TEST_DIAG not defined, return NUlL */
#ifndef TCXOMGR_ENABLE_TEST_DIAG
  return(NULL);
#else
  override_cmd_type old_override;

  /* response pkt */
  DIAG_SUBSYS_TCXOMGR_VCO_DFLT_OVERRIDE_rsp_type *rsp_ptr;

  /*-----------------------------------------------------------------------*/

  if(pkt_len != sizeof(DIAG_SUBSYS_TCXOMGR_VCO_DFLT_OVERRIDE_req_type))
  {
    MSG_1(MSG_SSID_TCXOMGR, MSG_LEGACY_ERROR, "Invalid VCO Default Override Request (%d)",pkt_len);
    return(NULL);
  }

  /* Keep the old override state */
  old_override = tcxomgr_diag.vco_dflt_override_enable;

  /* Store the value from the message */
  tcxomgr_diag.vco_dflt_override_enable = (override_cmd_type)
     ((DIAG_SUBSYS_TCXOMGR_VCO_DFLT_OVERRIDE_req_type *)req_pkt)->enable;

  /* Check to see if we just dissabled the override */
  if ( (old_override != NO_OVERRIDE) &&
       (tcxomgr_diag.vco_dflt_override_enable == NO_OVERRIDE) )
  {
    /* Disabling the override so put the old value back */
    tcxomgr.vco.dflt = tcxomgr_diag.vco_dflt;
  }
  else if (tcxomgr_diag.vco_dflt_override_enable != NO_OVERRIDE)
  {
    /* Save the current default value */
    tcxomgr_diag.vco_dflt = tcxomgr.vco.dflt;

    /* Write the new value */
    tcxomgr.vco.dflt = (tcxomgr_vco_type)
                      ((DIAG_SUBSYS_TCXOMGR_VCO_DFLT_OVERRIDE_req_type *)req_pkt)->dflt;
  }

  /* Check if we need to write to NV */
  if ( tcxomgr_diag.vco_dflt_override_enable == OVERRIDE_AND_WRITE )
  {
    rex_enter_crit_sect(&tcxomgr.nv->crit_sect);

    /* Copy the data into the nv data struct */
    tcxomgr.nv->item.vco_default = tcxomgr.vco.dflt;

    /* Write the data into NV */
    (void) tcxomgr_nv_cmd( NV_WRITE_F, NV_VCO_DEFAULT_I );

    rex_leave_crit_sect(&tcxomgr.nv->crit_sect);
  }

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_TCXOMGR_VCO_DFLT_OVERRIDE_rsp_type *)
            diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_TCXOMGR,
                  (diagpkt_subsys_cmd_code_type)VCO_DFLT_OVERRIDE,
              sizeof(DIAG_SUBSYS_TCXOMGR_VCO_DFLT_OVERRIDE_rsp_type));

  return(rsp_ptr);
#endif /* TCXOMGR_ENABLE_TEST_DIAG */
}



/*===========================================================================

FUNCTION TCXOMGR_DIAG_VCO_SLOPE_OVERRIDE

DESCRIPTION    This function is called when Diag requests to enable the
  Temp Slope Override.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   New VCO Slope Value may be written to NV.

NOTE: Writes to NV may be undone by disabling the override and shutting
  down the phone.  This will restore the old data in RAM, and on shutdown
  the RAM values will be written to NV.

===========================================================================*/
static PACK(void *) tcxomgr_diag_vco_slope_override
(
  PACK(void*) req_pkt,  /* request packet */
  uint16 pkt_len         /* request packet length */
)
{
  /*if TCXOMGR_ENABLE_TEST_DIAG not defined, return NUlL */
#ifndef TCXOMGR_ENABLE_TEST_DIAG
  return(NULL);
#else
  override_cmd_type old_override;

  /* response pkt */
  DIAG_SUBSYS_TCXOMGR_VCO_SLOPE_OVERRIDE_rsp_type *rsp_ptr;

  /*-----------------------------------------------------------------------*/

  if(pkt_len != sizeof(DIAG_SUBSYS_TCXOMGR_VCO_SLOPE_OVERRIDE_req_type))
  {
    MSG_1(MSG_SSID_TCXOMGR, MSG_LEGACY_ERROR, "Invalid VCO Default Override Request (%d)",pkt_len);
    return(NULL);
  }

  /* Keep the old override state */
  old_override = tcxomgr_diag.vco_slope_override_enable;

  /* Store the value from the message */
  tcxomgr_diag.vco_slope_override_enable = (override_cmd_type)
     ((DIAG_SUBSYS_TCXOMGR_VCO_SLOPE_OVERRIDE_req_type *)req_pkt)->enable;

  /* Check to see if we just dissabled the override */
  if ( (old_override != NO_OVERRIDE) &&
       (tcxomgr_diag.vco_slope_override_enable == NO_OVERRIDE) )
  {
    /* We just dissabled the override so put the original value back */
    tcxomgr.vco.slope = tcxomgr_diag.vco_slope;
  }
  else if (tcxomgr_diag.vco_slope_override_enable != NO_OVERRIDE)
  {
    /* Save the old value */
    tcxomgr_diag.vco_slope = tcxomgr.vco.slope;

    /* Write the new value */
    tcxomgr.vco.slope = (uint16)
                       ((DIAG_SUBSYS_TCXOMGR_VCO_SLOPE_OVERRIDE_req_type *)req_pkt)->slope;
  }

  /* Check if we need to write to NV */
  if ( tcxomgr_diag.vco_slope_override_enable == OVERRIDE_AND_WRITE )
  {
    rex_enter_crit_sect(&tcxomgr.nv->crit_sect);

    /* Copy the data into the nv data struct */
    tcxomgr.nv->item.vco_slope = tcxomgr.vco.slope;

    /* Write the data into NV */
    (void) tcxomgr_nv_cmd( NV_WRITE_F, NV_VCO_SLOPE_I );

    rex_leave_crit_sect(&tcxomgr.nv->crit_sect);
  }

  /* Since we probably changed the slope value lets update the thresholds */
  /* Set the update threshold to .5ppm or:
   * 2^20 / NV_VCO_SLOPE / 2 */
  tcxomgr.vco_temp_update_thresh  =
    TCXOMGR_GEN_UPDATE_THRESH( tcxomgr.vco.slope );

  /* Set the initial threshold to 5ppm or:
   * 5 * 2^20 / NV_VCO_SLOPE */
  tcxomgr.vco_temp_initial_thresh =
    TCXOMGR_GEN_INITIAL_THRESH( tcxomgr.vco.slope );

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_TCXOMGR_VCO_SLOPE_OVERRIDE_rsp_type *)
            diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_TCXOMGR,
                  (diagpkt_subsys_cmd_code_type)VCO_SLOPE_OVERRIDE,
              sizeof(DIAG_SUBSYS_TCXOMGR_VCO_SLOPE_OVERRIDE_rsp_type));

  return(rsp_ptr);
#endif /* TCXOMGR_ENABLE_TEST_DIAG */
}



/*===========================================================================

FUNCTION TCXOMGR_DIAG_SLOPE_RANGE_OVERRIDE

DESCRIPTION    This function is called when Diag requests to enable the
  Temp Slope Override.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   New Slope Range Value may be written to NV.

NOTE: Writes to NV may be undone by disabling the override and shutting
  down the phone.  This will restore the old data in RAM, and on shutdown
  the RAM values will be written to NV.


===========================================================================*/
static PACK(void *) tcxomgr_diag_slope_range_override
(
  PACK(void*) req_pkt,  /* request packet */
  uint16 pkt_len         /* request packet length */
)
{
  /*if TCXOMGR_ENABLE_TEST_DIAG not defined, return NUlL */
#ifndef TCXOMGR_ENABLE_TEST_DIAG
  return(NULL);
#else
  override_cmd_type old_override;

  /* response pkt */
  DIAG_SUBSYS_TCXOMGR_SLOPE_RANGE_OVERRIDE_rsp_type *rsp_ptr;

  /*-----------------------------------------------------------------------*/

  if(pkt_len != sizeof(DIAG_SUBSYS_TCXOMGR_SLOPE_RANGE_OVERRIDE_req_type))
  {
    MSG_1(MSG_SSID_TCXOMGR, MSG_LEGACY_ERROR, "Invalid Slope Range Override Request (%d)",pkt_len);
    return(NULL);
  }

  /* Keep the old override state */
  old_override = tcxomgr_diag.slope_range_override_enable;

  /* Store the value from the message */
  tcxomgr_diag.slope_range_override_enable = (override_cmd_type)
     ((DIAG_SUBSYS_TCXOMGR_SLOPE_RANGE_OVERRIDE_req_type *)req_pkt)->enable;

  /* Check to see if we just dissabled the override */
  if ( (old_override != NO_OVERRIDE) &&
       (tcxomgr_diag.slope_range_override_enable == NO_OVERRIDE) )
  {
    /* Put the old value back */
    tcxomgr.vco.slope_range = tcxomgr_diag.slope_range;
  }
  else if (tcxomgr_diag.slope_range_override_enable != NO_OVERRIDE)
  {
    /* Save the old value */
    tcxomgr_diag.slope_range = tcxomgr.vco.slope_range;

    /* Write the new value */
    tcxomgr.vco.slope_range = (uint8)
                             ((DIAG_SUBSYS_TCXOMGR_SLOPE_RANGE_OVERRIDE_req_type *)req_pkt)->slope_range;
  }

  /* Check if we need to write to NV */
  if ( tcxomgr_diag.slope_range_override_enable == OVERRIDE_AND_WRITE )
  {
    rex_enter_crit_sect(&tcxomgr.nv->crit_sect);

    /* Copy the data into the nv data struct */
    tcxomgr.nv->item.vco_slope_range = tcxomgr.vco.slope_range;

    /* Write the data into NV */
    (void) tcxomgr_nv_cmd( NV_WRITE_F, NV_VCO_SLOPE_RANGE_I );

    rex_leave_crit_sect(&tcxomgr.nv->crit_sect);
  }

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_TCXOMGR_SLOPE_RANGE_OVERRIDE_rsp_type *)
            diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_TCXOMGR,
                   (diagpkt_subsys_cmd_code_type)SLOPE_RANGE_OVERRIDE,
              sizeof(DIAG_SUBSYS_TCXOMGR_SLOPE_RANGE_OVERRIDE_rsp_type));

  return(rsp_ptr);
#endif /* TCXOMGR_ENABLE_TEST_DIAG */
}



/*===========================================================================

FUNCTION TCXOMGR_DIAG_RGS_OVERRIDE

DESCRIPTION    This function is called when Diag requests to enable the
  Temp Slope Override.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   New RGS Values may be written to NV.

NOTE: Writes to NV may be undone by disabling the override and shutting
  down the phone.  This will restore the old data in RAM, and on shutdown
  the RAM values will be written to NV.

===========================================================================*/
static PACK(void *) tcxomgr_diag_rgs_override
(
  PACK(void*) req_pkt,  /* request packet */
  uint16 pkt_len         /* request packet length */
)
{
  /*if TCXOMGR_ENABLE_TEST_DIAG not defined, return NUlL */
#ifndef TCXOMGR_ENABLE_TEST_DIAG
  return NULL;
#else
  override_cmd_type old_override;

  /* Time (in seconds) since 6 JAN 1980 00:00:00 */
  uint32                          time;

  /* Current Temperature set */
  tcxomgr_temp_set_type           temp;

  /* response pkt */
  DIAG_SUBSYS_TCXOMGR_RGS_OVERRIDE_rsp_type *rsp_ptr;

  /*-----------------------------------------------------------------------*/

  /* Get temp */
  tcxomgr_get_time_and_temp(&time, &temp);

  if(pkt_len != sizeof(DIAG_SUBSYS_TCXOMGR_RGS_OVERRIDE_req_type))
  {
    MSG_1(MSG_SSID_TCXOMGR, MSG_LEGACY_ERROR, "Invalid RGSult Override Request (%d)",pkt_len);
    return(NULL);
  }

  /* Make sure we know temperature to process RGS override. */

  if ( TCXOMGR_TEMP_INVALID(temp))
  {
    MSG(MSG_SSID_TCXOMGR, MSG_LEGACY_ERROR, "Can't override RGS. Unknown UE temperature");
    return(NULL);
  }

  /* Keep the old override state */
  old_override = tcxomgr_diag.rgs_override_enable;

  /* Store the value from the message */
  tcxomgr_diag.rgs_override_enable = (override_cmd_type)
     ((DIAG_SUBSYS_TCXOMGR_RGS_OVERRIDE_req_type *)req_pkt)->enable;


  /* Did we just disable the override? */
  if ( (old_override != NO_OVERRIDE) &&
       (tcxomgr_diag.rgs_override_enable == NO_OVERRIDE) )
  {
    /* Reset the RGS data so that it will not get used until someone
     * updates it */
    tcxomgr.rgs.system       = TCXOMGR_CLIENT_NONE;
    tcxomgr.rgs.vco          = tcxomgr.vco.dflt;
    tcxomgr.rgs.temp.pa      = TCXOMGR_INVALID_TEMP;
    tcxomgr.rgs.temp.pmic    = TCXOMGR_INVALID_TEMP;
    tcxomgr.rgs.temp.xo      = TCXOMGR_INVALID_TEMP;
    tcxomgr.rgs.time         = 0;
    tcxomgr.rgs.time_in_msec = 0;
    tcxomgr.rgs.rot          = 0;
  }
  else if (tcxomgr_diag.rgs_override_enable != NO_OVERRIDE)
  {
    /* Write the new values */
    tcxomgr.rgs.system = (tcxomgr_client_id_type)
                         ((DIAG_SUBSYS_TCXOMGR_RGS_OVERRIDE_req_type *)req_pkt)->rgs.system;

    /* Store the temp delta locally */
    tcxomgr_diag.rgs_delta_temp = (int16)
     ((DIAG_SUBSYS_TCXOMGR_RGS_OVERRIDE_req_type *)req_pkt)->rgs.delta_temp;

    /* Store the current temp plus the delta temp to keep the data valid */
    tcxomgr.rgs.temp.xo = temp.xo + (tcxomgr_diag.rgs_delta_temp << 10 );

    /* However, we want the rgs packet to always return the delta time
    * passed in here. */
    tcxomgr_diag.rgs_delta_time = (uint32)
                                  ((DIAG_SUBSYS_TCXOMGR_RGS_OVERRIDE_req_type *)req_pkt)->rgs.time;

    /* Store the current time in the RGS field so that it is valid */
    tcxomgr.rgs.time = time - tcxomgr_diag.rgs_delta_time;

    tcxomgr.rgs.vco = (tcxomgr_vco_type)
                      ((DIAG_SUBSYS_TCXOMGR_RGS_OVERRIDE_req_type *)req_pkt)->rgs.vco;

    tcxomgr.rgs.rot = (tcxomgr_rotator_type)
                      ((DIAG_SUBSYS_TCXOMGR_RGS_OVERRIDE_req_type *)req_pkt)->rgs.rot;
  }

  /* Check if we need to write to NV */
  if ( tcxomgr_diag.rgs_override_enable == OVERRIDE_AND_WRITE )
  {
    rex_enter_crit_sect(&tcxomgr.nv->crit_sect);

    /* Write the data into NV */
    tcxomgr.nv->item.rgs_type = (uint8)tcxomgr.rgs.system;
    (void) tcxomgr_nv_cmd( NV_WRITE_F, NV_RGS_TYPE_I );

    /* Write the data into NV */
    tcxomgr.nv->item.rgs_temp = (uint8)(((temp.xo)>>10) + tcxomgr_diag.rgs_delta_temp);
    (void) tcxomgr_nv_cmd( NV_WRITE_F, NV_RGS_TEMP_I );

    /* Store the current time minus the delta in the RGS field */
    /* Write the data into NV */
    tcxomgr.nv->item.rgs_time = time - tcxomgr_diag.rgs_delta_time;
    (void) tcxomgr_nv_cmd( NV_WRITE_F, NV_RGS_TIME_I );

    /* Write the data into NV */
    tcxomgr.nv->item.rgs_vco = tcxomgr.rgs.vco;
    (void) tcxomgr_nv_cmd( NV_WRITE_F, NV_RGS_VCO_I );

    /* Write the data into NV */
    tcxomgr.nv->item.rgs_rot = tcxomgr.rgs.rot;
    (void) tcxomgr_nv_cmd( NV_WRITE_F, NV_RGS_ROT_I );

    rex_leave_crit_sect(&tcxomgr.nv->crit_sect);
  }

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_TCXOMGR_RGS_OVERRIDE_rsp_type *)
            diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_TCXOMGR,
                     (diagpkt_subsys_cmd_code_type)RGS_OVERRIDE,
              sizeof(DIAG_SUBSYS_TCXOMGR_RGS_OVERRIDE_rsp_type));

  return(rsp_ptr);
#endif /* TCXOMGR_ENABLE_TEST_DIAG */
}



/*===========================================================================

FUNCTION TCXOMGR_DIAG_RFE_SLOPE_OVERRIDE

DESCRIPTION    This function is called when Diag requests to enable the
  RFE Slope Override.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   New RFE Slope Value may be written to NV.

NOTE: Writes to NV may be undone by disabling the override and shutting
  down the phone.  This will restore the old data in RAM, and on shutdown
  the RAM values will be written to NV.

===========================================================================*/
static PACK(void *) tcxomgr_diag_rfe_slope_override
(
  PACK(void*) req_pkt,  /* request packet */
  uint16 pkt_len         /* request packet length */
)
{
  /*if TCXOMGR_ENABLE_TEST_DIAG defined, return NUlL */
#ifndef TCXOMGR_ENABLE_TEST_DIAG
  return(NULL);
#else
  /* response pkt */
  DIAG_SUBSYS_TCXOMGR_RFE_SLOPE_OVERRIDE_rsp_type *rsp_ptr;

  /*-----------------------------------------------------------------------*/

  if(pkt_len != sizeof(DIAG_SUBSYS_TCXOMGR_RFE_SLOPE_OVERRIDE_req_type))
  {
    MSG_1(MSG_SSID_TCXOMGR, MSG_LEGACY_ERROR, "Invalid RFE Slope Override Request (%d)",pkt_len);
    return(NULL);
  }

  /* Store the value from the message */
  tcxomgr_diag.rfe_slope_override_enable = (boolean)
     ((DIAG_SUBSYS_TCXOMGR_RFE_SLOPE_OVERRIDE_req_type *)req_pkt)->enable;

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_TCXOMGR_RFE_SLOPE_OVERRIDE_rsp_type *)
            diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_TCXOMGR,
                      (diagpkt_subsys_cmd_code_type)RFE_SLOPE_OVERRIDE,
              sizeof(DIAG_SUBSYS_TCXOMGR_RFE_SLOPE_OVERRIDE_rsp_type));

  return(rsp_ptr);
#endif /* TCXOMGR_ENABLE_TEST_DIAG */
}

/*-------------------------------------------------------------------------
      External  Functions
-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION TCXOMGR_DIAG_INIT

DESCRIPTION    This function is called at startup to load Diag with the
      TCXOMGR logging dispatch table.

DEPENDENCIES   tcxomgr_diag_tbl.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
void tcxomgr_diag_init (void)
{

  /*-----------------------------------------------------------------------*/

  memset(&tcxomgr_diag,0,sizeof(tcxomgr_diag));

  DIAGPKT_DISPATCH_TABLE_REGISTER ((diagpkt_subsys_id_type)DIAG_SUBSYS_TCXOMGR,
                                   tcxomgr_diag_tbl);

  DIAGPKT_DISPATCH_TABLE_REGISTER_V2_DELAY (DIAG_SUBSYS_CMD_VER_2_F,
                                   (diagpkt_subsys_id_type)DIAG_SUBSYS_TCXOMGR,
                                   tcxomgr_diag_tbl_v2_delay);

  tcxomgr_diag_cal_cmd_delay_rsp_id = 0;
} /* tcxomgr_diag_init */



/*===========================================================================

FUNCTION TCXOMGR_DIAG_GET_TEMP_TABLE_OVERRIDE_ENABLE

DESCRIPTION    This function returns the state of the temp table override.

DEPENDENCIES   None.

RETURN VALUE   boolean.

SIDE EFFECTS   None.

===========================================================================*/
boolean tcxomgr_diag_get_temp_table_override_enable( void )
{
  return( (tcxomgr_diag.temp_table_override_enable == NO_OVERRIDE) ?
            FALSE : TRUE);
}

/*===========================================================================

FUNCTION TCXOMGR_DIAG_GET_VCO_DFLT_OVERRIDE_ENABLE

DESCRIPTION    This function returns the state of the vco_dflt override.

DEPENDENCIES   None.

RETURN VALUE   boolean.

SIDE EFFECTS   None.

===========================================================================*/
boolean tcxomgr_diag_get_vco_dflt_override_enable( void )
{
  return( (tcxomgr_diag.vco_dflt_override_enable == NO_OVERRIDE) ?
            FALSE : TRUE);
}

/*===========================================================================

FUNCTION TCXOMGR_DIAG_GET_VCO_SLOPE_OVERRIDE_ENABLE

DESCRIPTION    This function returns the state of the vco_slope override.

DEPENDENCIES   None.

RETURN VALUE   boolean.

SIDE EFFECTS   None.

===========================================================================*/
boolean tcxomgr_diag_get_vco_slope_override_enable( void )
{
  return( (tcxomgr_diag.vco_slope_override_enable == NO_OVERRIDE) ?
            FALSE : TRUE);
}

/*===========================================================================

FUNCTION TCXOMGR_DIAG_GET_SLOPE_RANGE_OVERRIDE_ENABLE

DESCRIPTION    This function returns the state of the SLOPE_RANGE override.

DEPENDENCIES   None.

RETURN VALUE   boolean.

SIDE EFFECTS   None.

===========================================================================*/
boolean tcxomgr_diag_get_slope_range_override_enable( void )
{
  return( (tcxomgr_diag.slope_range_override_enable == NO_OVERRIDE) ?
            FALSE : TRUE);
}

/*===========================================================================

FUNCTION TCXOMGR_DIAG_GET_RFE_SLOPE_OVERRIDE_ENABLE

DESCRIPTION    This function returns the state of the rfe_slope override.

DEPENDENCIES   None.

RETURN VALUE   boolean.

SIDE EFFECTS   None.

===========================================================================*/
boolean tcxomgr_diag_get_rfe_slope_override_enable( void )
{
  return( tcxomgr_diag.rfe_slope_override_enable);
}

/*===========================================================================

FUNCTION TCXOMGR_DIAG_GET_RGS_OVERRIDE_ENABLE

DESCRIPTION    This function returns the state of the RGS override.

DEPENDENCIES   None.

RETURN VALUE   boolean.

SIDE EFFECTS   None.

===========================================================================*/
boolean tcxomgr_diag_get_rgs_override_enable( void )
{
  return( (tcxomgr_diag.rgs_override_enable == NO_OVERRIDE) ?
            FALSE : TRUE);
}



/*===========================================================================

FUNCTION TCXOMGR_DIAG_GET_RGS_TIME

DESCRIPTION    This function returns the delta time for the overriden RGS

DEPENDENCIES   None.

RETURN VALUE   uint32 delta time.

SIDE EFFECTS   None.

===========================================================================*/
uint32 tcxomgr_diag_get_rgs_time( void )
{
  return( tcxomgr_diag.rgs_delta_time);
}


/*===========================================================================

FUNCTION TCXOMGR_DIAG_GET_RGS_TEMP_DIFF

DESCRIPTION    This function returns the temperature difference for the overriden RGS

DEPENDENCIES   None.

RETURN VALUE   uint32 delta time.

SIDE EFFECTS   None.

===========================================================================*/
int16 tcxomgr_diag_get_rgs_temp_diff( void )
{
  return( tcxomgr_diag.rgs_delta_temp);
}

/*===========================================================================

FUNCTION TCXOMGR_DIAG_SEND_IQ_DATA_LOG

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean tcxomgr_diag_send_iq_data_log
(
  tcxomgr_log_iq_data_struct_type *raw_data
)
{
  LOG_XO_IQ_DATA_C_type *log_ptr = NULL;


  log_ptr = (LOG_XO_IQ_DATA_C_type*)
            log_alloc_ex( (log_code_type) LOG_XO_IQ_DATA_C,
            sizeof(LOG_XO_IQ_DATA_C_type) );

  if ( log_ptr  != NULL )
  {
    /* Update the version number of the log packet */
    log_ptr->version = TCXOMGR_IQ_DATA_LOG_VERSION;

    /* Copy the information */
    log_ptr->iq_data   = *raw_data;

    /* submit the log to DIAG */
    log_commit( log_ptr );

    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /*  tcxomgr_diag_send_iq_data_log */

/*===========================================================================

FUNCTION TCXOMGR_DIAG_SEND_FIELD_CAL_LOG

DESCRIPTION: Logs Field cal log packet to diag

DEPENDENCIES: None

RETURN VALUE: None

SIDE EFFECTS: None

===========================================================================*/
void tcxomgr_diag_send_field_cal_log
(
  tcxomgr_log_field_cal_struct_type *field_cal_data,
  uint16 id
)
{
  tcxomgr_log_field_cal_calibration_data_type *log_ptr;

  log_ptr = (tcxomgr_log_field_cal_calibration_data_type *)
            log_alloc_ex( (log_code_type) LOG_TCXOMGR_FIELD_CAL_C,
            sizeof(tcxomgr_log_field_cal_calibration_data_type) );

  if ( log_ptr  != NULL )
  {
    TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

    /* Update the version number of the log packet */
    log_ptr->version = TCXOMGR_FIELD_CAL_LOG_VERSION;
    log_ptr->subcode = TCXOMGR_FIELD_CAL_LOG_CAL_SUBCODE;
    log_ptr->id      = id;

    /* Copy the header information */
    log_ptr->field_cal_data   = *field_cal_data;

    TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);

    /* submit the log to DIAG */
    log_commit( log_ptr );
  }
}


/*===========================================================================

FUNCTION TCXOMGR_DIAG_SEND_LOG_IFC

DESCRIPTION: Logs IFC log packets to diag

DEPENDENCIES: None

RETURN VALUE: None

SIDE EFFECTS: None

===========================================================================*/
void tcxomgr_diag_send_log_ifc
(
  int32 log_id,
  int32 log_version,
  int32 log_sub_code,
  size_t payload_size,
  void *payload
)
{
  PACK(void *) log_ptr;
  size_t header_size = sizeof(tcxomgr_log_ifc_template);

  /* get memory from log subsystem */
  log_ptr = log_alloc_ex((log_code_type) log_id, header_size + payload_size);

  if ( log_ptr != NULL )
  {
    /* Update the version number of the log packet */
    ((tcxomgr_log_ifc_template *)log_ptr)->version = log_version;
    ((tcxomgr_log_ifc_template *)log_ptr)->subcode = log_sub_code;

    /* Copy the payload */
    memscpy((uint8 *)log_ptr + header_size, payload_size, payload, payload_size);

    /* submit the log to DIAG */
    log_commit( log_ptr );
  }
}

/*===========================================================================

FUNCTION TCXOMGR_DIAG_SEND_RGS_LOG

DESCRIPTION: Logs new RGS log packet to diag

DEPENDENCIES: None

RETURN VALUE: None

SIDE EFFECTS: None

===========================================================================*/
void tcxomgr_diag_send_rgs_log
(
  tcxomgr_log_rgs_struct_type *rgs_data,
  uint16 id
)
{
  tcxomgr_log_rgs_data_type *log_ptr;

  log_ptr = (tcxomgr_log_rgs_data_type *)
            log_alloc_ex( (log_code_type) LOG_TCXOMGR_RGS_C,
            sizeof(tcxomgr_log_rgs_data_type) );

  if ( log_ptr  != NULL )
  {
    TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

    /* Update the version number of the log packet */
    log_ptr->version = TCXOMGR_RGS_LOG_VERSION;
    log_ptr->subcode = TCXOMGR_RGS_LOG_SUBCODE;
    log_ptr->id      = id;

    /* Copy the header information */
    log_ptr->rgs_data   = *rgs_data;
    log_ptr->rgs_data.log_packet_timestamp_ms = timetick_get_ms();

    TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);

    /* submit the log to DIAG */
    log_commit( log_ptr );
  }
}

/*===========================================================================

FUNCTION TCXOMGR_DIAG_SEND_TEMP_SUBS_LOG

DESCRIPTION: Logs new TEMP SUBSCRIPTION log packet to diag

DEPENDENCIES: None

RETURN VALUE: None

SIDE EFFECTS: None

===========================================================================*/
void tcxomgr_diag_send_temp_subs_log
(
  tcxomgr_log_temp_subscription_struct_type *temp_subscribe,
  uint16 id
)
{
  tcxomgr_temp_subscription_data_type *log_ptr;

  log_ptr = (tcxomgr_temp_subscription_data_type *)
            log_alloc_ex( (log_code_type) LOG_TCXOMGR_TEMP_SUBSCRIPTION_C,
                          sizeof(tcxomgr_temp_subscription_data_type) );

  if ( log_ptr  != NULL )
  {
    TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

    log_ptr->version = TCXOMGR_TEMP_SUBSCRIPTION_LOG_VERSION;
    log_ptr->subcode = TCXOMGR_TEMP_SUBSCRIPTION_LOG_SUBCODE;
    log_ptr->id      = id;

    memscpy( &(log_ptr->subs_data),
            sizeof(tcxomgr_log_temp_subscription_struct_type),
            temp_subscribe,
            sizeof(tcxomgr_log_temp_subscription_struct_type) );

    log_ptr->subs_data.log_packet_timestamp = timetick_get_ms();

    TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);

    /* submit the log to DIAG */
    log_commit( log_ptr );
  }
}

/*===========================================================================

FUNCTION TCXOMGR_DIAG_SEND_RGS_RETRIEVAL_LOG

DESCRIPTION: Logs new RGS log packet to diag

DEPENDENCIES: None

RETURN VALUE: None

SIDE EFFECTS: None

===========================================================================*/
void tcxomgr_diag_send_rgs_retrieval_log
(
  tcxomgr_log_rgs_retrieval_struct_type *rgs_data,
  uint16 id
)
{
#ifdef LOG_TCXOMGR_RGS_RETRIEVAL_C
  tcxomgr_log_rgs_retrieval_data_type *log_ptr;

  log_ptr = (tcxomgr_log_rgs_retrieval_data_type *)
            log_alloc_ex( (log_code_type) LOG_TCXOMGR_RGS_RETRIEVAL_C,
            sizeof(tcxomgr_log_rgs_retrieval_data_type) );

  if ( log_ptr  != NULL )
  {
    TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

    /* Update the version number of the log packet */
    log_ptr->version = TCXOMGR_RGS_RETRIEVAL_LOG_VERSION;
    log_ptr->subcode = TCXOMGR_RGS_RETRIEVAL_LOG_SUBCODE;
    log_ptr->id      = id;

    /* Copy the header information */
    log_ptr->rgs_data = *rgs_data;
    log_ptr->rgs_data.log_packet_timestamp_ms = timetick_get_ms();

    TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);

    /* submit the log to DIAG */
    log_commit( log_ptr );
  }
#endif /* LOG_TCXOMGR_RGS_RETRIEVAL_C */
}

/*===========================================================================

FUNCTION TCXOMGR_DIAG_SEND_AGED_RGS_RETRIEVAL_LOG

DESCRIPTION: Logs new RGS retrieval log packet to diag

DEPENDENCIES: None

RETURN VALUE: None

SIDE EFFECTS: None

===========================================================================*/
void tcxomgr_diag_send_aged_rgs_retrieval_log
(
  tcxomgr_log_aged_rgs_retrieval_struct_type *rgs_data,
  uint16 id
)
{
#ifdef LOG_TCXOMGR_RGS_RETRIEVAL_C
  tcxomgr_log_aged_rgs_retrieval_data_type *log_ptr;

  log_ptr = (tcxomgr_log_aged_rgs_retrieval_data_type *)
            log_alloc_ex( (log_code_type) LOG_TCXOMGR_RGS_RETRIEVAL_C,
            sizeof(tcxomgr_log_aged_rgs_retrieval_data_type) );

  if ( log_ptr  != NULL )
  {
    TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

    /* Update the version number of the log packet */
    log_ptr->version = TCXOMGR_RGS_RETRIEVAL_LOG_VERSION;
    log_ptr->subcode = TCXOMGR_RGS_AGED_RETRIEVAL_LOG_SUBCODE;
    log_ptr->id      = id;

    /* Copy the header information */
    log_ptr->rgs_data = *rgs_data;
    log_ptr->rgs_data.log_packet_timestamp_ms = timetick_get_ms();

    TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);

    /* submit the log to DIAG */
    log_commit( log_ptr );
  }
#endif /* LOG_TCXOMGR_RGS_RETRIEVAL_C */
}

/*============================================================================

FUNCTION TCXOMGR_MC_FLUSH_CLIENT_LOG

DESCRIPTION
  This function sends the AFC log to diag if the log buffer is full or if
  logging client wanted it to be flushed because of AFC state change etc.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

============================================================================*/

void tcxomgr_mc_flush_client_log
(
  tcxomgr_mc_log_struct_type     *afc_log_ptr
)
{
  LOG_TCXOMGR_AFC_DATA_C_type *log_ptr;       /* Log buffer pointer */
  uint8                        i;             /* loop variable */
  uint32                       num_freq_entry;/* Number of frequency entries*/
  uint8                        c;             /* carrier index */
  uint8                        carrier_num = 0;  /* Number of carriers */
  uint16                       mc_log_header_size;
    /* log header size in byte */
  uint16                       mc_log_carriers_size;
    /* log carrier size in byte */
  uint16                       mc_log_entry_size;
    /* log size per entry in byte */
  uint32                       mc_log_entries_size;

/*------------------------------------------------------------------------*/

  /* Get the current number of frequency entries */
  num_freq_entry = afc_log_ptr->afc_header_info.num_freq_entry;
  if ( num_freq_entry > TCXOMGR_MAX_FREQ_ENTRIES )
  {
    ERR_FATAL("num_freq_entries is too large", num_freq_entry, 0, 0);
  }

  /* Log the packet if this log code is on and if there is an entry */
  if ( log_status(LOG_TCXOMGR_AFC_DATA_C) && ( num_freq_entry != 0 ))
  {
    for ( c = 0; c < TCXOMGR_MAX_NUM_CARRIERS; c++ )
    {
      if ( afc_log_ptr->afc_header_info.carrier_mask & ( 1 << c ))
      {
        carrier_num++;
         /* The carrier is enabled */
      }
    }

    mc_log_header_size = sizeof(tcxomgr_mc_log_header_struct_type);
      /* size of log header */

    mc_log_carriers_size =
                        sizeof(tcxomgr_mc_log_carrier_struct_type) * carrier_num;
      /* Total size of carrier info */

    mc_log_entry_size = sizeof(tcxomgr_mc_log_entries_struct_type) -
               (sizeof(uint16)*(TCXOMGR_MAX_NUM_CARRIERS - carrier_num ));
      /* Size of each entry */

    mc_log_entries_size = mc_log_entry_size * num_freq_entry;
      /* Total size of entries */

    log_ptr = (LOG_TCXOMGR_AFC_DATA_C_type*)
              log_alloc_ex( (log_code_type) LOG_TCXOMGR_AFC_DATA_C,
              TCXOMGR_MC_VAR_SIZE( mc_log_header_size,
                                   mc_log_carriers_size,
                                   mc_log_entries_size ) );

    if ( log_ptr  != NULL )
    {
      TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

      /* Update the version number of the log packet */
      log_ptr->version = TCXOMGR_MC_AFC_LOG_VERSION;

      /* Update the header information */
      log_ptr->afc_header_info.logging_client_id = afc_log_ptr->afc_header_info.logging_client_id;
      log_ptr->afc_header_info.ctrl_client_id    = afc_log_ptr->afc_header_info.ctrl_client_id;
      memscpy( log_ptr->afc_header_info.timestamp,
               sizeof(qword),
               afc_log_ptr->afc_header_info.timestamp,
               sizeof(qword) );
      log_ptr->afc_header_info.afc_mode          = afc_log_ptr->afc_header_info.afc_mode;
      log_ptr->afc_header_info.afc_gain          = afc_log_ptr->afc_header_info.afc_gain;
      log_ptr->afc_header_info.srl_indicator     = afc_log_ptr->afc_header_info.srl_indicator;
      log_ptr->afc_header_info.cross_mode        = afc_log_ptr->afc_header_info.cross_mode;
      log_ptr->afc_header_info.vco_slope         = afc_log_ptr->afc_header_info.vco_slope;
      log_ptr->afc_header_info.therm             = afc_log_ptr->afc_header_info.therm.xo;
      log_ptr->afc_header_info.num_freq_entry    = afc_log_ptr->afc_header_info.num_freq_entry;

      /* Copy frequency entries info into the log record */
      for (i = 0; i < num_freq_entry; i++)
      {
        log_ptr->entries[i].vco_acc         = afc_log_ptr->entries[i].vco_acc;
        log_ptr->entries[i].delta_time      = afc_log_ptr->entries[i].delta_time;
        log_ptr->entries[i].signal_strength = afc_log_ptr->entries[i].signal_strength;
        log_ptr->entries[i].rot_acc         = afc_log_ptr->entries[i].rot_acc[0];
      }

      TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);

      /* submit the log to DIAG */
      log_commit( log_ptr );
    } /* if log_ptr */

  } /* if log_status */

  /* Reset the number of frequency entries */
  afc_log_ptr->afc_header_info.num_freq_entry = 0;

} /* tcxomgr_mc_flush_client_log */


/*============================================================================

FUNCTION TCXOMGR_FLUSH_CLIENT_LOG

DESCRIPTION
  This function sends the AFC log to diag if the log buffer is full or if
  logging client wanted it to be flushed because of AFC state change etc.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

============================================================================*/

void tcxomgr_flush_client_log
(
  tcxomgr_log_struct_type     *afc_log_ptr
)
{
  LOG_TCXOMGR_AFC_DATA_C_type *log_ptr;       /* Log buffer pointer */
  uint8                        i;             /* loop variable */
  uint32                       num_freq_entry;/* Number of frequency entries*/

  /*------------------------------------------------------------------------*/

  /* Get the current number of frequency entries */
  num_freq_entry = afc_log_ptr->afc_header_info.num_freq_entry;

  /* Log the packet if this log code is on and if there is an entry */
  if ( log_status(LOG_TCXOMGR_AFC_DATA_C) && ( num_freq_entry != 0 ))
  {
    log_ptr = (LOG_TCXOMGR_AFC_DATA_C_type*)
              log_alloc_ex( (log_code_type) LOG_TCXOMGR_AFC_DATA_C,
                TCXOMGR_VAR_SIZEOF(LOG_TCXOMGR_AFC_DATA_C_type,
                tcxomgr_log_entries_struct_type,
                afc_log_ptr->afc_header_info.num_freq_entry)
                );

    if ( log_ptr  != NULL )
    {
      TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

      /* Update the version number of the log packet */
      log_ptr->version = TCXOMGR_AFC_LOG_VERSION;

      /* Copy the header information */
      log_ptr->afc_header_info   = afc_log_ptr->afc_header_info;

      /* Copy frequency entries info into the log record */
      for (i = 0; i < num_freq_entry; i++)
      {
        memscpy( (void *)( &log_ptr->entries[i] ),
                sizeof (tcxomgr_log_entries_struct_type),
                (void *)( &afc_log_ptr->entries[i] ),
                sizeof (tcxomgr_log_entries_struct_type) );
      }

      TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);

      /* submit the log to DIAG */
      log_commit( log_ptr );
    } /* if log_ptr */

  } /* if log_status */

  /* Reset the number of frequency entries */
  afc_log_ptr->afc_header_info.num_freq_entry = 0;

} /* tcxomgr_flush_client_log */

/*============================================================================

FUNCTION TCXOMGR_LOG_ENTRY_UPDATED

DESCRIPTION
  This function is used by clients to indicate that they have updated
  one frequency entry to their AFC log buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If the AFC buffer is full, the packet is flushed.

============================================================================*/

void tcxomgr_log_entry_updated
(
  tcxomgr_log_struct_type     *afc_log_ptr
)
{

/*------------------------------------------------------------------------*/

  if( afc_log_ptr->afc_header_info.num_freq_entry  == 1 )
  {
    /* First entry in this log pkt, so update controlling client id */
    afc_log_ptr->afc_header_info.ctrl_client_id =
                                   (uint8) tcxomgr.client_in_control;
  }
  else if( afc_log_ptr->afc_header_info.num_freq_entry >=
           TCXOMGR_MAX_FREQ_ENTRIES )
  {
    /* Client log buffer full - send AFC log packet to diag, start afresh. */
    tcxomgr_flush_client_log( afc_log_ptr );
  }
  else if( afc_log_ptr->afc_header_info.ctrl_client_id !=
           (uint8) tcxomgr.client_in_control )
  {
    DBG_2( MED, "Flushing AFC log buffer - logging client changed: old %d, new %d",
            afc_log_ptr->afc_header_info.ctrl_client_id,
            tcxomgr.client_in_control );

    /* Controlling tech changed - send AFC log packet to diag, start afresh. */
    tcxomgr_flush_client_log( afc_log_ptr );
  }

} /* tcxomgr_log_entry_updated */


/*============================================================================

FUNCTION TCXOMGR_MC_LOG_ENTRY_UPDATED

DESCRIPTION
  This function is used by clients to indicate that they have updated
  one frequency entry to their AFC log buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If the AFC buffer is full, the packet is flushed.

============================================================================*/

void tcxomgr_mc_log_entry_updated
(
  tcxomgr_mc_log_struct_type     *afc_log_ptr
)
{

/*------------------------------------------------------------------------*/

  if( afc_log_ptr->afc_header_info.num_freq_entry  == 1 )
  {
    /* First entry in this log pkt, so update controlling client id */
    afc_log_ptr->afc_header_info.ctrl_client_id =
                                   (uint8) tcxomgr.client_in_control;
  }
  else if( afc_log_ptr->afc_header_info.num_freq_entry >=
           TCXOMGR_MAX_FREQ_ENTRIES )
  {
    /* Client log buffer full - send AFC log packet to diag, start afresh. */
    tcxomgr_mc_flush_client_log( afc_log_ptr );
  }
  else if( afc_log_ptr->afc_header_info.ctrl_client_id !=
           (uint8) tcxomgr.client_in_control )
  {
    DBG_2( MED, "Logging client changed:  old %d, new %d",
            afc_log_ptr->afc_header_info.ctrl_client_id,
            tcxomgr.client_in_control );

    /* Controlling tech changed - send AFC log packet to diag, start afresh. */
    tcxomgr_mc_flush_client_log( afc_log_ptr );
  }

} /* tcxomgr_log_entry_updated */

/*===========================================================================

FUNCTION TCXOMGR_DIAG_COARSE_DC_CAL_CMD

DESCRIPTION    This function is called to initiate XO Coarse DC calibration.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None

===========================================================================*/

static PACK(void *) tcxomgr_diag_coarse_dc_cal_cmd
(
  PACK(void*) req_pkt,  /* request packet */
  uint16 pkt_len        /* request packet length */
)
{
  /* request pkt */
  trxomgr_diag_dc_coarse_cal_req_struct_type*          my_req_ptr;
  trxomgr_diag_dc_coarse_cal_imm_rsp_struct_type*      imm_rsp_ptr = NULL;
  tcxomgr_coarse_dc_cal_req_data_struct_type_v1*       cmd_v1_ptr;
  tcxomgr_cmd_type*                                    cmd_ptr;
  diagpkt_subsys_status_type                           status  = 1;

  /*-----------------------------------------------------------------------*/

  /* Check if the request if valid */
  if(pkt_len != sizeof(trxomgr_diag_dc_coarse_cal_req_struct_type))
  {
    MSG_2(MSG_SSID_TCXOMGR, MSG_LEGACY_ERROR,
          "tcxomgr_diag_coarse_dc_cal_cmd: Invalid packet length: %d, Expected: %d",
          pkt_len, sizeof(trxomgr_diag_dc_coarse_cal_req_struct_type));
    return NULL;
  }

  do
  {
    /* Get the pointer to the command */
    my_req_ptr = (trxomgr_diag_dc_coarse_cal_req_struct_type *)req_pkt;

    if (tcxomgr_diag_cal_cmd_delay_rsp_id != 0)
    {
      DBG_0(ERROR, "Already processing a message");
      break;
    }

    if (my_req_ptr->data.version == TCXOMGR_DIAG_COARSE_DC_CAL_CMD_V1)
    {
      DBG_2(HIGH, "Coarse DC Cal: Version: %d, Identifier: %d",
              my_req_ptr->data.version,
              my_req_ptr->id);

      /* get an command buffer */
      cmd_ptr = tcxomgr_get_cmd_buf();
      if ( cmd_ptr == NULL )
      {
        DBG_0(ERROR,"IFC:Tcxomgr Command queue buffer allocation failed!!");
        break;
      }

      cmd_v1_ptr = &my_req_ptr->data.u.v1;

      DBG_2(HIGH, "Coarse DC Cal: Freq: %d (kHz), Offset: %d (Hz)",
              cmd_v1_ptr->rx_freq_khz,
              cmd_v1_ptr->offset);

      /* Initialize the message body */
      cmd_ptr->cmd_id                               = TCXOMGR_CMD_COARSE_CAL;
      cmd_ptr->cmd.fac_cal_data.id                  = my_req_ptr->id;
      cmd_ptr->cmd.fac_cal_data.data.version        = my_req_ptr->data.version;
      memscpy(&cmd_ptr->cmd.fac_cal_data.data.u.v1,
              sizeof(cmd_ptr->cmd.fac_cal_data.data.u.v1),
              cmd_v1_ptr,
              sizeof(*cmd_v1_ptr));

      /* Send the packet on its way */
      tcxomgr_send_cmd(cmd_ptr);

      status = 0;
    }
    else
    {
      DBG_2(ERROR, "Coarse DC Cal: Unsupported version: %d, identifier: %d",
              my_req_ptr->data.version,
              my_req_ptr->id);
      tcxomgr_diag_cal_cmd_delay_rsp_id = 0;
    }

    imm_rsp_ptr = (trxomgr_diag_dc_coarse_cal_imm_rsp_struct_type *)
                   diagpkt_subsys_alloc_v2 (
                     (diagpkt_subsys_id_type) DIAG_SUBSYS_TCXOMGR,
                     (diagpkt_subsys_cmd_code_type) COARSE_DC_CAL,
                     sizeof (trxomgr_diag_dc_coarse_cal_imm_rsp_struct_type) );

    if ( imm_rsp_ptr == NULL )
    {
      DBG_0(ERROR, "Coarse DC Cal: Could not immeditate allocate response");
      break;
    }

    diagpkt_subsys_set_status( imm_rsp_ptr, status );

    if ( status == 0 )
    {
      /* Save the packet id for later use in the delay respose */
      tcxomgr_diag_cal_cmd_delay_rsp_id = diagpkt_subsys_get_delayed_rsp_id( imm_rsp_ptr );

      /* set the response count */
      diagpkt_subsys_set_rsp_cnt( imm_rsp_ptr, 0 );
    }
    else
    {
      /* set the response count */
      diagpkt_subsys_set_rsp_cnt( imm_rsp_ptr, 1 );

      /* Nothing more to follow */
      diagpkt_subsys_reset_delayed_rsp_id(imm_rsp_ptr);

      tcxomgr_diag_coarse_dc_cal_log
      (
        my_req_ptr->id,
        TCXOMGR_ERR_EST_FAIL_FATAL_ERROR,
        &my_req_ptr->data,
        NULL
      );
    }

    diagpkt_commit(imm_rsp_ptr);

  } while(0);

  return imm_rsp_ptr;

} /* tcxomgr_diag_coarse_dc_cal_cmd */

/*===========================================================================

FUNCTION TCXOMGR_DIAG_COARSE_DC_CAL_RSP

DESCRIPTION    This function is called to respond to the
               XO Coarse DC calibration command.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None

===========================================================================*/

void tcxomgr_diag_coarse_dc_cal_rsp
(
  uint32                               id,            /* Identifier from req       */
  tcxomgr_coarse_dc_cal_req_data_type* req_data,      /* Request data              */
  diagpkt_subsys_status_type           status,        /* Status indicating success
                                                          or failure               */
  tcxomgr_cal_fac_param_type      *fac_cal_param,     /* factory cal parameters    */
  tcxomgr_cal_info_type           *info_pkt,          /* cal info pkt              */
  tcxomgr_cal_results_info_struct *cal_results        /* Cal results               */
)
{
  /* request pkt */

  trxomgr_diag_dc_coarse_cal_delayed_rsp_struct_type*  rsp_ptr = NULL;
  tcxomgr_coarse_dc_cal_rsp_data_struct_type_v1*       rsp_v1_ptr = NULL;
  /*-----------------------------------------------------------------------*/

  do
  {
    if ( tcxomgr_diag_cal_cmd_delay_rsp_id == 0 )
    {
      DBG_0(ERROR, "Coarse DC Cal: No outstanding response identifier found");
      break;
    }

    if ( fac_cal_param == NULL || info_pkt == NULL || req_data == 0 )
    {
      ERR_FATAL("Coarse DC Cal: Invalid pointer encountered", 0, 0, 0);
      break;
    }

    /* Allocate the memory for the response */
    rsp_ptr = (trxomgr_diag_dc_coarse_cal_delayed_rsp_struct_type *)
              diagpkt_subsys_alloc_v2_delay (
                 (diagpkt_subsys_id_type) DIAG_SUBSYS_TCXOMGR,
                 (diagpkt_subsys_cmd_code_type) COARSE_DC_CAL,
                 tcxomgr_diag_cal_cmd_delay_rsp_id,
                 sizeof (trxomgr_diag_dc_coarse_cal_delayed_rsp_struct_type) );

    if ( rsp_ptr == NULL )
    {
      DBG_0(ERROR, "Coarse DC Cal: Could not allocate delayed response");
      break;
    }

    rsp_v1_ptr = &rsp_ptr->data.u.v1;

    if ( info_pkt != NULL )
    {
      rsp_ptr->id                       =  id;
      rsp_ptr->data.version             =  TCXOMGR_DIAG_COARSE_DC_CAL_RSP_V1;

      DBG_1(HIGH, "Coarse DC Cal: Cal done. Trim set to %d",
              info_pkt->dc_xo_trim);

      rsp_v1_ptr->init_xo_freqerr       =  info_pkt->init_xo_freqerr;
      rsp_v1_ptr->dc_xo_freqerr         =  info_pkt->dc_xo_freqerr;
      rsp_v1_ptr->fail_reason           =  info_pkt->fail_reason;
      rsp_v1_ptr->init_xo_trim          =  info_pkt->init_xo_trim;
      rsp_v1_ptr->dc_xo_trim            =  info_pkt->dc_xo_trim;
      rsp_v1_ptr->init_temp             =  info_pkt->init_temp.xo;
      rsp_v1_ptr->final_temp            =  info_pkt->final_temp.xo;
    }
    else
    {
      rsp_v1_ptr->fail_reason           =  TCXOMGR_ERR_EST_FAIL_FATAL_ERROR;
    }

    if ( fac_cal_param != NULL )
    {
      rsp_v1_ptr->c3                    = fac_cal_param->cal_coef.c3;
      rsp_v1_ptr->c2                    = fac_cal_param->cal_coef.c2;
      rsp_v1_ptr->c1                    = fac_cal_param->cal_coef.c1;
      rsp_v1_ptr->c0                    = fac_cal_param->cal_coef.c0;
      rsp_v1_ptr->t0                    = fac_cal_param->cal_coef.t0;
      rsp_v1_ptr->t0p                   = fac_cal_param->cal_coef.t0p;
      rsp_v1_ptr->p                     = fac_cal_param->cal_coef.p;
      rsp_v1_ptr->ft_qual_ind           = fac_cal_param->cal_coef.ft_qual_ind;
    }
    else
    {
      rsp_v1_ptr->fail_reason           = TCXOMGR_ERR_EST_FAIL_FATAL_ERROR;
    }

    if ( cal_results != NULL )
    {
      rsp_v1_ptr->gps_cno               = cal_results->gps_cno;
    }

    diagpkt_subsys_set_rsp_cnt(rsp_ptr, 1);
    diagpkt_subsys_set_status(rsp_ptr, status);
    diagpkt_delay_commit(rsp_ptr);

    /* Send the log packet as well */
    tcxomgr_diag_coarse_dc_cal_log
    (
      id,
      rsp_v1_ptr->fail_reason,
      req_data,
      &rsp_ptr->data
    );

    DBG_1(HIGH, "Coarse DC Cal: Sent response. identifier: %d", id);

    /* We have responded to an outstanding command. So reset the
       outstanding response identifier */
    tcxomgr_diag_cal_cmd_delay_rsp_id = 0;
  } while(0);
} /* tcxomgr_diag_coarse_dc_cal_rsp */

/*===========================================================================

FUNCTION TCXOMGR_DIAG_SEND_COARSE_DC_CAL_LOG

DESCRIPTION: Logs new coarse cal log packet to DIAG.

DEPENDENCIES: None

RETURN VALUE: None

SIDE EFFECTS: None

===========================================================================*/
void tcxomgr_diag_coarse_dc_cal_log
(
  uint32                               id,
  tcxomgr_cal_failure_enum_type        fail_reason,
  tcxomgr_coarse_dc_cal_req_data_type* req_data,
  tcxomgr_coarse_dc_cal_rsp_data_type* rsp_data
)
{
#ifdef LOG_TCXOMGR_COARSE_DC_CAL_C
  tcxomgr_log_coarse_dc_cal_data_type *my_log_ptr;

  my_log_ptr = (tcxomgr_log_coarse_dc_cal_data_type *)
            log_alloc_ex( (log_code_type) LOG_TCXOMGR_COARSE_DC_CAL_C,
            sizeof(tcxomgr_log_coarse_dc_cal_data_type) );

  if ( my_log_ptr != NULL && req_data != NULL )
  {
    TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

    my_log_ptr->id              = id;

    /* Update the data in the log packet of the log packet */
    memscpy(&my_log_ptr->req_data,
            sizeof(my_log_ptr->req_data),
            req_data,
            sizeof(*req_data));

    if ( rsp_data != NULL )
    {
      memscpy(&my_log_ptr->rsp_data,
              sizeof(my_log_ptr->rsp_data),
              rsp_data,
              sizeof(*rsp_data));
    }
    else
    {
      memset(&my_log_ptr->rsp_data, 0,
              sizeof(my_log_ptr->rsp_data));
    }

    my_log_ptr->rsp_data.u.v1.fail_reason = fail_reason;

    TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);

    /* submit the log to DIAG */
    log_commit( my_log_ptr );
  }
#endif /* LOG_TCXOMGR_COARSE_DC_CAL_C */
}
