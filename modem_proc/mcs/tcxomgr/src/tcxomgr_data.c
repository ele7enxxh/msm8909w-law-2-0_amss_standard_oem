/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

                               T M   D A T A
                           TCXO Manager NV and RGS data

GENERAL DESCRIPTION

  This module maintains the Recent Good System values, and VCO-Temperature
  tables, for proper TCXO operation.

EXTERNALIZED FUNCTIONS


REGIONAL FUNCTIONS

  None


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

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=



==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/tcxomgr/src/tcxomgr_data.c#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      ---------------------------------------------------------
08/04/15   tl       Adjust XO trim in field if not factory calibrated
06/04/15   tl       Record RGS log event time in milliseconds
04/29/15   tl       Remove deprecated task.h header
03/18/15   tl       Check RGS returned against absolute limits
01/30/15   tl       Remove obsolete featurization
01/23/15   tl       Write field cal params EFS asynchronously during field cal
01/22/15   tl       Add profiling to selected Tcxomgr APIs
01/07/15   tl       Use Tcxomgr NV critical section during NV write operations
12/17/14   tl       Fix handling of tcxomgr.uncertainty during sub & client RGS
11/18/14   tl       Clean up unused macro
11/10/14   tl       Increase C1 uncertainty in FT2
10/19/14   tl       Only log RGS reset when set
09/25/14   sg       Remove call to CGPS API to get time
09/12/14   tl       Use EFS async put to defer writing FT table during field cal
08/07/14   tl       Defer writing the FT table to EFS during field calibration
05/01/14   tl       Change FT bins to 0.5 C across the entire temperature range
                    Add coefficient uncertainty levels for FT2 field states
                    Account for uncertainty in C2
04/17/14   tl       Expose the FT curve rotator and uncertainty in returned vco
03/12/14   tl       Calculate ppm uncertainty by rounding up ppb uncertainty
02/21/14   tl       Store calibration data as saved in flash.
                    Write aging at shutdown if it has changed.
02/13/14   tl       Expose function to calculate FT uncertainty
12/18/13   tl       Replace hard-coded temperature range with T0 when
                    calculating uncertainity
10/28/13   mn       Support for client and subscription RGS retrieval log packet.
10/20/13   mn       Calculate RGS uncertainty based on difference from FT curve.
10/04/13   mn       Support to save and retrieve subscription and system
                    RGS to/from EFS.
01/29/13   sk       Support for frequency estimate separation
09/30/13   mn       TCXOMGR should return FT estimates when no subscription RGS
                    is available.
09/12/13   mn       New Rpush API to support multi-SIM targets.
08/16/13   sm       Disable error check for client RGS
08/12/13   mn       New API to maintain tech specific RGS.
08/01/13   mn       Flush RGS to NV on offline indication and shutdown.
07/29/13   mn       Remove the featurization around the cgps_GetGpsTime
                    API usage.
07/16/13   mn       Sync EFS data after the first C1 cal.
07/16/13   mn       Dynamic C0 uncertainty in coarse cal state.
05/06/13   sg       Changed logging levels
04/18/13   mn       Handling of uncertainties for TCXO crystal.
04/09/13   mn       Extrapolate RGS uncertainty according to temperature.
04/02/13   sg       Added relaxed temp span check
03/14/13   mn       Remove featurization flag from api header files.
02/04/13   mn       New log packet for Retrieve RGS.
12/10/12   mn       Logging optimization on tcxomgr messages.
12/12/12   sk       only aging data are reset when efs read fails for aging data
12/10/12   mn       TCXOMGR should flush XO NV contents during powerdown.
11/29/12   sk       tcxomgr.nv.xxx changed to tcxomgr.nv->xxx
11/26/12   mn       Fixing KW errors.
11/19/12   sk       Added uncertainities to include
                      field0 and field1 states
11/13/12   sk       efs support for xtal type added
11/02/12   mn       Capture return values of efs_get and efs_put in integers.
10/18/12   mn       Optimize tcxomgr.conf to avoid multiple writes at every boot
10/18/12   sk       check for negative time difference in aging added
10/12/12   sm       Using C2 for uncertanity instead of t0
09/14/12   sm       Added tcxomgr_get_time to ensure consistency
09/13/12   sk       support scheduling rgs update function to xo task queue
09/06/12   sk       EFS path for aging data added
09/04/12   sk       T0 deviation added for slope uncertainity estimation
08/13/12   sk       check for factory calibration version added
08/06/12   sk       initialization of t0p and p added
07/13/12   gs       Support for RGS log packet
07/10/12   sg       Support for crystal aging feature
05/22/12   mn       Support to reset RGS through the SET_UNCERTAINTY diag command.
05/16/12   mn       Optimize efs_write's in tcxomgr_nv_init to a single efs_write
04/25/12   mn       Stub out pm_dev_mega_xo_get_xo_trim and pm_dev_mega_xo_set_xo_trim
                      during bring up to avoid crashes in RPM interaction internal to
                      the APIs.
03/30/12   ag       Replaced deprecated pmic trim apis.
04/23/12   sg       Support for factory fine cal removal
03/26/12   sg       Added API to send OSC offset to CGPS
02/17/11   mn       Extrapolate RGS XO temp based on old and new cached temp
                      values.
02/10/12   sr       TCXOMGR Feature Cleanup
01/27/12   ns       Support for IFC
01/15/12   sg       support for fast field cal v3.1
11/30/11   mn       Added check to check discard RGS if they are off from
                      the estimate only in FT1 and FT2 states.
11/28/11   mn       Moved macros temp range check macros to tcxomgr_i.h file.
11/17/11   mn       Added mutex protection to tcxomgr_nv_init and got rid of
                      the tcb_ptr argument.
11/11/11   mn       Added support to write cal debug data to nv conf file.
11/01/11   ns       Add support for Tcxo mode
10/20/11   sm       Add support for GPS RGS
09/09/11   sg       Allow RGS update based on temparature
08/17/11   ns       Fix KW errors
07/08/11   ns       Adding RGS updates as log packet and also enchanced
                      Field cal log packets
07/07/11   ag       Update temp cache only when task receives temp read cmd.
                    and also get rid of the fine temp time check.
06/29/11   sm       Disable bringup feature for temperature reads
05/19/11   ag       Return default temp in coarse temp read
04/04/11   ns       Print temperature when we discard RGS
03/28/11   ag       Added apis to read and write to version num efs file.
03/11/11   sm       Added new qualifier for extreme temps
02/03/10   ns       Fast field cal support for 2.5 degree c1 calibration
01/12/11   sg       Fixed KW error and compile warnings
11/10/10   sm       Saving RGS temp in C for XO Targets
12/01/10   ag       Protect NV read/writes with critical section
11/24/10   ag       Added check to discard bad rotator and invalidate RGS.
11/20/10   ag       Disable coarse PMIC reads.
11/12/10   ag       APIs for L1 to query temp table and default vco in VCTCXO
11/10/10   sm       Removed references to QSR_
11/04/10   sm       Added Support For Fast Field Cal
11/02/10   ns       Increase temp table range
11/02/10   ns       Fixed issues in coarse and fine temp reads.
10/14/10   hh       Fix for Coarse and fine temperature reads.
10/05/10   ag       Fix for tcxomgr_get_uncertainty returning incorrect value.
07/14/10   sg       Added code to perform recalibration for high temp change
06/12/10   sm       get_temp will return default temp during target bringups
05/12/10   ns       Feature wrap the Test framework under XO
05/11/10   ns       Add range check for temperature
05/11/10   ns       Add Offtarget stubs
01/27/10   sm       removed patch to return default values in tcxomgr_get_temp()
09/01/10   vks      Return default values in tcxomgr_get_temp() for MDM9K
                    bringup.
01/05/10   ns       Remove max correction applied to RGS based on temp
11/25/09   ag       Wrapping ADC DAL with FEATURE_TCXOMGR_ADCDAL
11/09/09   ag       Featurized the tcxomgr_send_osc_offset_to_cgps() for 9K.
10/09/09   va       Add API to send XO information to CGPS.
09/18/09   ns       Making c2 and c3 constant for factory cal. Also
                     changing temp ranges for field cal
07/29/09   ag       Replacing adc_read with the DAL api
08/31/09   ag       Replaced clk_read_secs() with time_get_secs()
08/10/09   sb       Implement additional trk_lo access functions.
07/22/09   sb       Do not read PA_THERM in XO as the channel is not supported
05/07/09   ns       Merge xo changes from Main latest
05/01/09   ns       Changing VCO defaults to avoid search issues
04/16/09   ag       Enabled the TCXOMGR_ADC_READ in tcxomgr_get_temp by
                    uncommenting it.
04/13/09   ag       Merged from //depot
12/10/08   sb       Modify tcxomgr_get_uncertainty() to check in RGS is more
                    than 30 seconds old.
12/05/08   sst      Update for QSC11x0 to us XO therm
10/20/08   adw      Replaced FEATURE_XO_DIAG w/ FEATURE_TCXOMGR_DIAG.
09/15/08   sg       Fix lint/klocwork warnings
09/15/08   sst      Correctly featurize for XO_TASK
08/28/08   adw      Added FEATURE_XO_DIAG to featurize for ULC.
07/11/08   scm      Support new RF temperature API: rfm_adc_read().
                      Implement tcxomgr_reset_rgs_and_temp_table().q
07/08/08   bb       Remove hard coded trim current value for QSC1100 target
06/20/08   mca      Merge from //depot
04/10/08   sg       Guard against getting temp before adc is running
03/14/08   sst      Temporarily disable sleep and QPCH for QSC1100
03/03/08   va       For XO - Adjust rot in RGS by freq-temp table for temp
11/16/07   mca      Fixed compiler warnings
11/14/07   sst      Properly set nv_fac_data->ft_qual_ind in cal_data_write
11/06/07   cab      Remove "double translation" of temp to index in update tbl
10/30/07   cab      If a trk_lo vs. temperature entry is empty, search +/-3
                    indexes for a trk_lo value, instead of +/-1.
10/18/07   sst      Updated calculations for nominal C2/C3
10/16/07   sst      Move XO Trim NV functions to tcxomgr_data.c
10/10/07   va       Do not allow sleep when reading from XO ADC
09/25/07   cab      Move field cal to tcxomgr task
                    Allow client to call update rgs in quick succession
09/06/07   sst      Sync enum of ft_qual btw NV and TCXOMGR
                    Lint fixes
09/05/07   cab      Cache therm read due to timeline impacts
08/13/07   sst      Add missing return in tcxomgr_get_temp_table
08/03/07   sst      Correct XO Fac Cal NV read fail check
08/02/07   va       PMIC temp should not be divided by 10.
07/26/07   sst      Change get_time_and_temp to use temp_set_type
                    Call tcxomgr_field_switch_xo_trim on power-up
07/10/07   va       Updates to tcxomgr_nv_field_cal_data_write.
07/09/07   sst      Use current task for NV read/writes
                    Remove NV fac cal featurizations
06/29/07   cab      Added tcxomgr_field_cal
06/18/07   aps      Added factory cal NV read/write functions
06/18/07    va      Macros for reading temperature
06/14/07   cab      Adding back in factory cal
06/13/07   cab      Reverted factory cal, removed check for controlling
                    client id in write_rgs_values
06/08/07   sst      Moved tcxomgr_get_temp from _cal.c
06/06/07   sst      Added xo_trim NV read/write functions
05/17/07   cab      Added uncertainty for passive clients
05/11/07   cab      Get rgs value only when it is valid and time is valid
03/02/07   nnv      Added code not to look rotator upper limit while updating
                    temp table upon storing rgs data and to use rotator data
                    for frequency adjustment in XO targets.
01/19/07   cab      Removed featurization of xo cal nv items (always present)
                    Fixed compiler warning for unsigned comparison (>=) 0
11/14/06   cab      Fix to return RGS value if updated in last second
10/12/06   cab      Removed TCXOMGR_RGS_NONE
09/29/06   cab      Initial version

============================================================================*/



/*============================================================================

                           INCLUDE FILES FOR MODULE

============================================================================*/
#include "mcs_variation.h"
#include "customer.h"
#include "time_svc.h"
#include "err.h"
#include "tcxomgr_adc_client.h"
#include "timetick.h"
#include "fs_async_put.h"
#include "mcsprofile.h"

#include "tcxomgr_i.h"
#include "tcxomgr_log_i.h"
#include "tcxomgr_rot_client_handling_i.h"

#include "modem_mcs_defs.h"

#include "tcxomgr_diag.h"

#include "tcxomgr_cal.h"
#include "tcxomgr_xo_trim.h"
#ifndef FEATURE_TCXOMGR_PMIC_BRINGUP
#include "pm.h"
#endif

#include "custglobal.h"

#include "assert.h"

/*---------------------------------------------------------------------------
  TCXO Manager local variables
---------------------------------------------------------------------------*/

extern tcxomgr_client_type clients[];
extern volatile boolean tcxomgr_disable_gps_client;
volatile boolean tcxomgr_rgs_disable_flag = TRUE;
volatile uint16 tcxomgr_uncertainty_value = 0;
volatile int8 tcxomgr_error_value = 0;
volatile boolean tcxomgr_disable_uncertainty_flag = FALSE;

/** Data structure used to hold the FT table prior to writing to EFS */
typedef struct 
{
  tcxomgr_efs_vco_struct_type   vco;
} tcxomgr_efs_dpc_data;

/** FT table formatted for writing to EFS. */
static tcxomgr_efs_dpc_data     ft_table_efs[TCXOMGR_TABLE_MAX];

/** Estimated values for the XO frequency error, in ppb, at each trim value. */
static int32 tcxomgr_trim_estimate[] = {
     61440,
     58163,
     54886,
     51610,
     48333,
     45056,
     41779,
     38502,
     35226,
     31949,
     28672,
     26010,
     23347,
     20685,
     18022,
     15360,
     12698,
     10035,
      7373,
      4710,
      2048,
       307,
     -1434,
     -3174,
     -4915,
     -6656,
     -8397,
    -10138,
    -11878,
    -13619,
    -15360,
    -16282,
    -17203,
    -18125,
    -19046,
    -19968,
    -20890,
    -21811,
    -22733,
    -23654,
    -24576,
    -25498,
    -26419,
    -27341,
    -28262,
    -29184,
    -30106,
    -31027,
    -31949,
    -32870,
    -33792,
    -34304,
    -34816,
    -35328,
    -35840,
    -36352,
    -36864,
    -37376,
    -37888,
    -38400,
    -38912,
    -39424,
    -39936,
    -40448,
};

/*============================================================================

                   Defines

============================================================================*/

/*Worst case C2 error is 4.5e-4. Scaled up by 2^28*/
#define TCXOMGR_MAX_C2_ERORR              120796

/* Min XO temp delta for valid samples (2^-10 deg C / unit) */
#define CAL_TEMP_SPAN_RELAXED             256

/* Convert uncertainty in PPB to PPM by adding 0.25 PPM (256 ppb), rounding up
 * (by adding 1023 ppb), and dividing by 1024 */
#define TCXOMGR_ROUND_UP_PPB_TO_PPM(x) (((x) + 256 + 1023) >> 10)

static const tcxomgr_ppm_type
tcxomgr_xo_coef_uncertainity[TCXOMGR_INT_QUAL_LAST][TCXOMGR_XO_COEF_LAST] =
{
  /* Coarse Cal */
  {
    16384,
    2516582,
    120796,
    64425
  },

  /* IFC Cal */
  {
    8192,
    1342177,
    120796,
    64425
  },

  /* Fine Cal */
  {
    8192,
    1006632,
    120796,
    64425
  },

  /* Field C1 */
  {
    8192,
    503316,
    120796,
    64425
  },

  /* Field C0 */
  {
    4096,
    503316,
    120796,
    64425
  },

  /* FT2, substate 3 */
  {
    1024,       /* .25 */
    335544,     /* .02 */
    80531,      /* 3e-4 */
    64425       /* 1.5e-5 */
  },

  /* FT2, substate 4 */
  {
    1024,       /* .25 */
    335544,     /* .02 */
    67109,      /* 2.5e-4 */
    30065       /* .7e-5 */
  },

  /* FT2, substate 5 */
  {
    1024,       /* .25 */
    335544,     /* .02 */
    33554,      /* 1.25e-4 */
    8590        /* .2e-5 */
  },

  /* RGS */
  {
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF

  },

  /* Default */
  {
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF
  }
};

/*============================================================================

                            FUNCTION DECLARATIONS

============================================================================*/

/*============================================================================

FUNCTION TCXOMGR_SET_DEFAULT_RGS_INFO

DESCRIPTION
  This API resets the RGS values from the TCXOMGR data structure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_set_default_rgs_info (tcxomgr_vco_info_type* ft)
{
  if ( ft != NULL )
  {
    ft->vco_type                  = TCXOMGR_VCO_DEFAULT;
    ft->vco_value                 = tcxomgr.vco.dflt;
    ft->rot_value                 = 0;
    ft->freq_uncertainty          = 56;
    ft->freq_uncertainty_ppb      = PPM_FULL_TO_PPM(56);
    ft->gps_vco_type              = TCXOMGR_VCO_DEFAULT;
    ft->gps_rot_value             = 0;
    ft->gps_freq_uncertainty      = 56;
    ft->gps_freq_uncertainty_ppb  = PPM_FULL_TO_PPM(56);
    ft->id                        = TCXOMGR_CLIENT_NONE;
    ft->as_id                     = TCXOMGR_AS_ID_NONE;
    ft->temp_diff                 = 0;
    ft->time_diff                 = 0;
    ft->rgs_offset                = 0;
    ft->ft_vco_type               = TCXOMGR_VCO_DEFAULT;
    ft->ft_rot_value              = 0;
    ft->ft_freq_uncertainty_ppb   = PPM_FULL_TO_PPM(56);
  }
}

/*============================================================================

FUNCTION TCXOMGR_NV_INIT

DESCRIPTION
  This function initializes the NV member of the TCXO manager
  Also write the config file which specifies the EFS items to back up for QCN

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void tcxomgr_nv_init
(
  /* Signal to wait for, for NV reading */
  rex_sigs_type sig,

  /* Function to call to wait for NV reading to complete, eg) rex_wait */
  void (*wait)(rex_sigs_type)
)
{

/*--------------------------------------------------------------------------*/
  int32    fd;
  struct fs_stat conf_file_det;
  size_t         data_size;
  int            file_stat_result;

  /* buffer for the list of file paths to be be written to the conf file */
  char     *str = TCXOMGR_FAC_CAL_VER_PATH "\n"     \
                  FIELD_CAL_EFS_PATH "\n"           \
                  TCXOMGR_OPERATION_MODE "\n"       \
                  FAC_CAL_DEBUG_EFS_PATH "\n"       \
                  TCXOMGR_EFS_FIELD_CAL_PARAMS "\n" \
                  TCXOMGR_EFS_FT_TABLE_WWAN "\n"    \
                  TCXOMGR_EFS_FT_TABLE_GPS "\n"     \
                  TCXOMGR_IFC_HIST_FILE "\n"        \
                  TCXOMGR_IFC_VERSION_FILE "\n"     \
                  TCXOMGR_IFC_C1_FILTER_FILE "\n"   \
                  TCXOMGR_EFS_FIELD_AGING_DATA "\n" \
                  TCXOMGR_XO_CRYSTAL_TYPE_FILE "\n" \
                  TCXOMGR_RGS_DATA_FILE "\n";

  /* Ensure mutex is free */
  rex_enter_crit_sect(&tcxomgr.nv->crit_sect);

  /* Initialize NV command buffer */
  tcxomgr.nv->cmd.data_ptr   = &tcxomgr.nv->item;
  tcxomgr.nv->cmd.sigs       = sig;
  tcxomgr.nv->cmd.done_q_ptr = NULL;

  /* Remember the wait function */
  tcxomgr.nv->wait = wait;

  file_stat_result = efs_stat(TCXOMGR_EFS_CONF_FILE, &conf_file_det);
  data_size = (size_t)strlen(str);

  /* If the conf file does not exist or if the size of the existing conf file is
     not what we expect, create a new conf file */
  if ( file_stat_result != 0 || data_size != conf_file_det.st_size )
  {
    /* Create conf file that is needed for backing the EFS files, erase it if it already exists */
    fd = efs_open(TCXOMGR_EFS_CONF_FILE, O_CREAT|O_WRONLY|O_TRUNC,
                  ALLPERMS );

    if (fd < 0)
    {
      DBG_1(ERROR, "Error opening EFS config file %d", fd);
      return;
    }

    /* Write the conf file */
    (void)efs_write(fd, str, data_size);

    efs_close(fd);
  }

  /* Release the mutex */
  rex_leave_crit_sect(&tcxomgr.nv->crit_sect);

} /* nv_init */


/*============================================================================

FUNCTION TCXOMGR_NV_CMD

DESCRIPTION
  This function reads/write an item from/to the NV into tcxomgr.nv->item

DEPENDENCIES
  The caller must have tcxomgr.nv->crit_sect locked

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

nv_stat_enum_type tcxomgr_nv_cmd
(
  /* READ, WRITE, PEEK, POKE, etc. */
  nv_func_enum_type cmd,

  /* Item ID to read/write from/to NV */
  nv_items_enum_type id
)
{
  /* Status of NV operation to return */
  nv_stat_enum_type  status;
  /*------------------------------------------------------------------------*/

  /* Setup NV command buffer to perform the required operation on the item */
  tcxomgr.nv->cmd.cmd     = cmd;

  tcxomgr.nv->cmd.item    = id;
  tcxomgr.nv->cmd.tcb_ptr = rex_self();

  /* Ensure the signal we are about to wait on is not already set */
  (void) rex_clr_sigs( tcxomgr.nv->cmd.tcb_ptr, tcxomgr.nv->cmd.sigs );

  /* Command the NV task to process the command */
  nv_cmd( &tcxomgr.nv->cmd );

  /* Wait for NV task to set the signal indicating operation is complete */
  tcxomgr.nv->wait( tcxomgr.nv->cmd.sigs );

  /* Clear the signal we waited on */
  (void) rex_clr_sigs( tcxomgr.nv->cmd.tcb_ptr, tcxomgr.nv->cmd.sigs );

  if ( tcxomgr.nv->cmd.status != NV_DONE_S )
  {
    DBG_3( HIGH, "NV cmd %d, item %d, status %d", cmd, id,
          tcxomgr.nv->cmd.status );
  }

  /* Copy the status to return */
  status = tcxomgr.nv->cmd.status;

  return status;

} /* tcxomgr_nv_cmd */


/*============================================================================

FUNCTION TCXOMGR_XO_TRIM_READ

DESCRIPTION
  This function reads the XO Trim/Capacitor/Coarse Rotator values from NV

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  XO Trim will be initialized

============================================================================*/

void tcxomgr_xo_trim_read( void )
{
#ifndef FEATURE_TCXOMGR_PMIC_BRINGUP
  uint8 pm_xo_trim_val = XO_TRIM_DEFAULT_VALUE;
  pm_err_flag_type pm_err_ret = PM_ERR_FLAG__SUCCESS;
#endif
/*--------------------------------------------------------------------------*/
  if (tcxomgr_get_crystal_type()== TCXO_CRYSTAL )
  {
    return;
  }

  /* Initialize values, so "else { ... }" clauses are not needed */
  tcxomgr.xo_trim.curr_val        = XO_TRIM_DEFAULT_VALUE;
  tcxomgr.xo_trim.table_val       = XO_TRIM_DEFAULT_VALUE;
  tcxomgr.xo_trim.switched        = FALSE;

  /* Attempt to read the curr and table xo_trim values from NV */
  rex_enter_crit_sect(&tcxomgr.nv->crit_sect);
  if ( tcxomgr_nv_cmd( NV_READ_F, NV_XO_TRIM_VALUES_I ) == NV_DONE_S )
  {
    tcxomgr.xo_trim.curr_val  = (uint8)tcxomgr.nv->item.xo_trim_values.current;
    tcxomgr.xo_trim.table_val = (uint8)tcxomgr.nv->item.xo_trim_values.table;
  }
  rex_leave_crit_sect(&tcxomgr.nv->crit_sect);

#ifndef FEATURE_TCXOMGR_PMIC_BRINGUP
  pm_err_ret = pm_dev_mega_xo_get_xo_trim(0,0,&pm_xo_trim_val);

  if ( pm_err_ret != PM_ERR_FLAG__SUCCESS )
  {
    /* either the XO_trim switching didn't happen or
       switching is not allowed*/
    DBG_2( ERROR, " pm_dev_mega_xo_get_xo_trim failed (%d) trim=%d",
                    pm_err_ret , pm_xo_trim_val );
    return ;
  }

  /* ensure the cap value from the tcxomgr is the same as that which is in
     the PMIC */
  if ( pm_xo_trim_val != tcxomgr.xo_trim.curr_val )
  {
    (void) pm_dev_mega_xo_set_xo_trim( 0,0, tcxomgr.xo_trim.curr_val );
  }
#endif

} /* tcxomgr_xo_trim_read( ) */


/*============================================================================

FUNCTION TCXOMGR_XO_TRIM_WRITE

DESCRIPTION
  This function writes the XO Trim/Capacitor/Coarse Rotator values to NV

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void tcxomgr_xo_trim_write( void )
{
#ifndef FEATURE_TCXOMGR_PMIC_BRINGUP
  pm_err_flag_type pm_err_ret = PM_ERR_FLAG__SUCCESS;

/*--------------------------------------------------------------------------*/

  if ( tcxomgr_get_crystal_type()== TCXO_CRYSTAL )
  {
    return;
  }

  /* initialize the trim value */
  tcxomgr.xo_trim.curr_val = XO_TRIM_DEFAULT_VALUE;

  /* get PMIC hardware value */
  pm_err_ret = pm_dev_mega_xo_get_xo_trim(0,0, &(tcxomgr.xo_trim.curr_val));

  if ( pm_err_ret != PM_ERR_FLAG__SUCCESS )
  {
    /* either the XO_trim switching didn't happen or
       switching is not allowed*/
    DBG_2( ERROR, " pm_dev_mega_xo_get_xo_trim failed (%d) trim=%d",
                    pm_err_ret , tcxomgr.xo_trim.curr_val );
   return;
  }

  /* Attempt to write the current and table xo_trim value */
  rex_enter_crit_sect(&tcxomgr.nv->crit_sect);
  tcxomgr.nv->item.xo_trim_values.current = (uint8)tcxomgr.xo_trim.curr_val;
  tcxomgr.nv->item.xo_trim_values.table   = (uint8)tcxomgr.xo_trim.table_val;

  (void) tcxomgr_nv_cmd( NV_WRITE_F, NV_XO_TRIM_VALUES_I );
  rex_leave_crit_sect(&tcxomgr.nv->crit_sect);

#endif
} /* tcxomgr_xo_trim_write( ) */


/*============================================================================

FUNCTION TCXOMGR_NV_CAL_DATA_READ

DESCRIPTION
  This function reads the calibration data from NV.

  If NV read of factory cal data fails or if ft_qual of factory cal data is
  FAIL, use defaults. If field cal data read fails, load the factory cal
  data into field cal structure.

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_nv_cal_data_read( void )
{
  nv_xo_cal_fac_data_type *nv_fac_data     =
    &(tcxomgr.nv->item.xo_factory_calibration);
  nv_xo_cal_field_data_type *nv_field_data =
    &(tcxomgr.nv->item.xo_field_calibration);
  tcxomgr_cal_fac_data_type   *fac_data    = &(tcxomgr.cal_data.fac_data);
  tcxomgr_cal_field_data_type *field_data  = &(tcxomgr.cal_data.field_data);

/*--------------------------------------------------------------------------*/

  rex_enter_crit_sect(&tcxomgr.nv->crit_sect);

  /* read in factory calibration coefficients */
  if (( tcxomgr_nv_cmd (NV_READ_F, NV_XO_FACTORY_CALIBRATION_I) == NV_DONE_S )
        && ( nv_fac_data->ft_qual_ind !=
                      ( nv_xo_ft_qual_enum_type )TCXOMGR_CAL_FT_QUAL_FAILED ))
  {
    fac_data->c3          = nv_fac_data->c3;
    fac_data->c2          = nv_fac_data->c2;
    fac_data->c1          = nv_fac_data->c1;
    fac_data->c0          = nv_fac_data->c0;
    fac_data->t0          = nv_fac_data->t0;
    fac_data->t0p         = nv_fac_data->t0p;
    fac_data->p           = nv_fac_data->p;
    fac_data->xo_trim     = nv_fac_data->xo_trim;
    CAL_FT_QUAL_FROM_NV( nv_fac_data->ft_qual_ind, fac_data->ft_qual_ind );
  }
  else /* Use defaults if read fails */
  {
    DBG_0(ERROR,
          "XO Factory calibration data read from NV failed, loading defaults");

    /* Load default values */
    fac_data->t0          = CAL_NV_DEFAULT_VAL_T0;
    fac_data->t0p         = CAL_NV_DEFAULT_VAL_T0P;
    fac_data->p           = CAL_NV_DEFAULT_VAL_P;
    fac_data->c0          = CAL_NV_DEFAULT_VAL_C0;
    fac_data->c1          = CAL_NV_DEFAULT_VAL_C1;
    tcxomgr_cal_calc_c2( fac_data);
    tcxomgr_cal_calc_c3( fac_data );
    (void) tcxomgr_xo_trim_get_val( &(fac_data->xo_trim) );
    fac_data->ft_qual_ind = TCXOMGR_CAL_FT_QUAL_NOT_CAL;
  }

  /* read in field calibration coefficients */
  if ( tcxomgr_nv_cmd ( NV_READ_F, NV_XO_FIELD_CALIBRATION_I ) == NV_DONE_S )
  {
    field_data->c3            = nv_field_data->c3;
    field_data->c2            = nv_field_data->c2;
    field_data->c1            = nv_field_data->c1;
    field_data->c0            = nv_field_data->c0;
    field_data->t0            = nv_field_data->t0;
    field_data->t0p           = nv_field_data->t0p;
    field_data->p             = nv_field_data->p;
    field_data->timestamp     = (uint64)(nv_field_data->timestamp);
    field_data->xo_trim       = nv_field_data->xo_trim;
    field_data->ft_sample_cnt = nv_field_data->ft_sample_cnt;
    field_data->state         = (tcxomgr_field_state_type)nv_field_data->state;
    CAL_FT_QUAL_FROM_NV( nv_field_data->ft_qual_ind, field_data->ft_qual_ind );
  }
  else
  {
    DBG_0(ERROR,
          "XO Field calibration data read from NV failed, loading defaults");

    /* copy factory values on failed read and initialize rest of fields to
       their default values */
    tcxomgr_cal_copy_fac_to_field();
  }

  rex_leave_crit_sect(&tcxomgr.nv->crit_sect);

  if (tcxomgr_get_crystal_type()== TCXO_CRYSTAL )
  {
    /* zero out the nv items if they exist, also used as a
       indication to clear FT table, as we just switched modes */
    if (field_data->c0 != 0 || field_data->c1 != 0 || field_data->c2 != 0 ||
        field_data->c3 != 0 || field_data->xo_trim != 0 )
    {
      /* set the co-efficients to zero */
      memset(fac_data,0,sizeof(tcxomgr_cal_fac_data_type));
      memset(field_data,0,sizeof(tcxomgr_cal_field_data_type));

      /* write back the data to NV and also flush the FT table since
         we just swapped */
      tcxomgr_fast_field_cal_set_default();
    }
  }
  else if(tcxomgr.xo_trim.curr_val != field_data->xo_trim )
  {
    DBG_2(ERROR, "Mismatch in trim values #4212 has %d and 4953 has %d",
                            tcxomgr.xo_trim.curr_val,
                            field_data->xo_trim);
  }

} /* tcxomgr_nv_cal_data_read */


/*============================================================================

FUNCTION TCXOMGR_NV_CAL_DATA_WRITE

DESCRIPTION
  This function writes xo calibration data values to NV.

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_nv_cal_data_write( void )
{

  nv_xo_cal_fac_data_type *nv_fac_data     =
    &(tcxomgr.nv->item.xo_factory_calibration);
  nv_xo_cal_field_data_type *nv_field_data =
    &(tcxomgr.nv->item.xo_field_calibration);
  tcxomgr_cal_fac_data_type   *fac_data    = &(tcxomgr.cal_data.fac_data);
  tcxomgr_cal_field_data_type *field_data  = &(tcxomgr.cal_data.field_data);

/*--------------------------------------------------------------------------*/

  rex_enter_crit_sect(&tcxomgr.nv->crit_sect);

  /* Fill in field calibration coefficients */
  nv_field_data->c3                    = field_data->c3;
  nv_field_data->c2                    = field_data->c2;
  nv_field_data->c1                    = field_data->c1;
  nv_field_data->c0                    = field_data->c0;
  nv_field_data->t0                    = field_data->t0;
  nv_field_data->t0p                   = field_data->t0p;
  nv_field_data->p                     = field_data->p;
  nv_field_data->timestamp             = field_data->timestamp;
  nv_field_data->xo_trim               = field_data->xo_trim;
  nv_field_data->ft_sample_cnt         = field_data->ft_sample_cnt;
  nv_field_data->state           = (nv_xo_field_state_type) field_data->state;
  CAL_FT_QUAL_TO_NV( field_data->ft_qual_ind, nv_field_data->ft_qual_ind );

  /* Attempt to write field calibration values */
  (void) tcxomgr_nv_cmd( NV_WRITE_F, NV_XO_FIELD_CALIBRATION_I );

  /* Fill in factory calibration coefficients */
  nv_fac_data->c3                      = fac_data->c3;
  nv_fac_data->c2                      = fac_data->c2;
  nv_fac_data->c1                      = fac_data->c1;
  nv_fac_data->c0                      = fac_data->c0;
  nv_fac_data->t0                      = fac_data->t0;
  nv_fac_data->t0p                     = fac_data->t0p;
  nv_fac_data->p                       = fac_data->p;
  nv_fac_data->xo_trim                 = fac_data->xo_trim;
  CAL_FT_QUAL_TO_NV( fac_data->ft_qual_ind, nv_fac_data->ft_qual_ind );

  /* Attempt to write factory calibration values */
  (void) tcxomgr_nv_cmd( NV_WRITE_F, NV_XO_FACTORY_CALIBRATION_I );

  rex_leave_crit_sect(&tcxomgr.nv->crit_sect);

} /* tcxomgr_nv_cal_data_write */


/*============================================================================

FUNCTION TCXOMGR_EFS_FIELD_CAL_PARAM_READ

DESCRIPTION
  This function reads the field calibration parameters from the EFS.
  The parameters are read for GPS and WWAN sturctures.

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  If the EFS is not present or the read fails then default values are
  returned.

============================================================================*/
void tcxomgr_efs_field_cal_params_read( void )
{

  tcxomgr_efs_field_data_type   cal_data[TCXOMGR_TABLE_MAX];
  uint16                        size;
  uint8                         loop;
  int                           val;
  tcxomgr_field_cal_info        *field_data;

  /* --------------------------------------------------------------------- */

  /* Get the size of the data to be read */
  size = sizeof(tcxomgr_efs_field_data_type) * TCXOMGR_TABLE_MAX;

  /* Read WWAN from the file */
  val = efs_get(TCXOMGR_EFS_FIELD_CAL_PARAMS, (void *)(&cal_data), size);

  /* Check if the data was read correctly */
  if (val < 0)
  {
    /* EFS returned error */
    DBG_1(ERROR, "Field Cal Param: Error reading EFS file %d", val);
    tcxomgr_cal_copy_fac_to_field();
  }
  else
  {
    /* Get the field cal pointer */
    field_data = tcxomgr.fieldcal_data;

    /* We are here because the read is successful so copy the results */
    for(loop = 0; loop < TCXOMGR_TABLE_MAX; loop++)
    {
      field_data[loop].cal_data.c0 = cal_data[loop].c0;
      field_data[loop].cal_data.c1 = cal_data[loop].c1;
      field_data[loop].cal_data.c2 = cal_data[loop].c2;
      field_data[loop].cal_data.c3 = cal_data[loop].c3;
      field_data[loop].cal_data.t0 = cal_data[loop].t0;
      field_data[loop].cal_data.t0p = tcxomgr.cal_data.fac_data.t0p;
      field_data[loop].cal_data.p   = tcxomgr.cal_data.fac_data.p;
      field_data[loop].cal_data.state = cal_data[loop].state;
      field_data[loop].cal_data.xo_trim = cal_data[loop].xo_trim;
      field_data[loop].cal_data.timestamp = cal_data[loop].timestamp;
      field_data[loop].cal_data.ft_qual_ind = cal_data[loop].ft_qual_ind;
      field_data[loop].cal_data.nv_timestamp = 0;

      field_data[loop].cal_data_nv = field_data[loop].cal_data;
    }
  }

  return;
}

/*============================================================================

FUNCTION TCXOMGR_EFS_FIELD_AGING_DATA_READ

DESCRIPTION
  This function reads the field aging data from the EFS.
  The parameters are read for GPS and WWAN sturctures.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If the EFS is not present or the read fails then default values are
  returned.

============================================================================*/
void tcxomgr_efs_field_aging_data_read( void )
{
  tcxomgr_efs_field_aging_type  aging_data[TCXOMGR_TABLE_MAX];
  uint16                        size;
  uint8                         loop;
  int                           val;
  tcxomgr_field_cal_info        *field_data;

 /* --------------------------------------------------------------------- */

 /* Get the size of the data to be read */
 size = sizeof(tcxomgr_efs_field_aging_type) * TCXOMGR_TABLE_MAX;
 /* Get the field cal pointer */
 field_data = tcxomgr.fieldcal_data;

 /* Read WWAN from the file */
 val = efs_get(TCXOMGR_EFS_FIELD_AGING_DATA, (void *)(&aging_data), size);

 /* Check if the data was read correctly */
 if (val < 0)
 {
   /* EFS returned error */
   DBG_1(ERROR, "Field Aging Data: Error reading EFS file %d", val);

   /* error readin efs file so reset aging data and write to efs */
   for(loop = 0; loop < TCXOMGR_TABLE_MAX; loop++)
   {
     field_data[loop].aging_data.c0_fresh      = 0;
     field_data[loop].aging_data.c0_fresh_nv   = 0;
     field_data[loop].aging_data.count         = 0;
     field_data[loop].aging_data.timestamp     = 0;
     field_data[loop].aging_data.timestamp_nv  = 0;
     field_data[loop].aging_data.valid         = FALSE;
     field_data[loop].aging_data.c1_num_points = 0;
     field_data[loop].aging_data.c1_std_dev    = 0;
     field_data[loop].aging_data.c3_num_points = 0;
     field_data[loop].aging_data.c3_std_dev    = 0;
   }
   tcxomgr_efs_field_aging_data_write();
 }
 else
 {

   /* We are here because the read is successful so copy the results */
   for(loop = 0; loop < TCXOMGR_TABLE_MAX; loop++)
   {
     field_data[loop].aging_data.c0_fresh      = aging_data[loop].c0_fresh;
     field_data[loop].aging_data.c0_fresh_nv   = aging_data[loop].c0_fresh;
     field_data[loop].aging_data.count         = aging_data[loop].count;
     field_data[loop].aging_data.timestamp     = aging_data[loop].timestamp;
     field_data[loop].aging_data.timestamp_nv  = aging_data[loop].timestamp;
     field_data[loop].aging_data.valid         = aging_data[loop].valid;
     field_data[loop].aging_data.c1_num_points = aging_data[loop].c1_num_points;
     field_data[loop].aging_data.c1_std_dev    = aging_data[loop].c1_std_dev;
     field_data[loop].aging_data.c3_num_points = aging_data[loop].c3_num_points;
     field_data[loop].aging_data.c3_std_dev    = aging_data[loop].c3_std_dev;
   }
 }

 return;

}


/*============================================================================

FUNCTION TCXOMGR_EFS_FIELD_CAL_PARAM_WRITE

DESCRIPTION
  This function writes the field calibration parameters from the EFS.
  The parameters are written for GPS and WWAN sturctures.


DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_efs_field_cal_params_write( boolean async )
{

  tcxomgr_efs_field_data_type   cal_data[TCXOMGR_TABLE_MAX];
  uint16                        size;
  int                           val;
  uint8                         loop;
  tcxomgr_field_cal_info        *field_data;

  /* --------------------------------------------------------------------- */

  /* Get the size of the data to be read */
  size = sizeof(tcxomgr_efs_field_data_type) * TCXOMGR_TABLE_MAX;

  /* Get the field cal pointer */
  field_data = tcxomgr.fieldcal_data;

  /* copy the field cal data to a local variable */
  for(loop = 0; loop < TCXOMGR_TABLE_MAX; loop++)
  {
    cal_data[loop].c0 = field_data[loop].cal_data.c0;
    cal_data[loop].c1 = field_data[loop].cal_data.c1;
    cal_data[loop].c2 = field_data[loop].cal_data.c2;
    cal_data[loop].c3 = field_data[loop].cal_data.c3;
    cal_data[loop].t0 = field_data[loop].cal_data.t0;
    cal_data[loop].state = field_data[loop].cal_data.state;
    cal_data[loop].xo_trim = field_data[loop].cal_data.xo_trim;
    cal_data[loop].timestamp = field_data[loop].cal_data.timestamp;
    cal_data[loop].ft_qual_ind = field_data[loop].cal_data.ft_qual_ind;
    cal_data[loop].nv_timestamp = field_data[loop].cal_data.nv_timestamp;

    field_data[loop].cal_data_nv = field_data[loop].cal_data;
  }

  if(async)
  {
    /* Write the field cal params to the EFS file asynchronously */
    val = efs_async_put(TCXOMGR_EFS_FIELD_CAL_PARAMS, (void *)(cal_data), size,
                  O_CREAT|O_AUTODIR, ALLPERMS);

    /* efs_async_put() could fail if the async queue is full. If it does, fall
     * back to efs_put(), which will block this task until the write is
     * complete.
     */
    if (val < 0)
    {
      DBG_0(HIGH, "Unable to write field cal params asynchronously. "
          "Falling back to direct call.");

      val = efs_put(TCXOMGR_EFS_FIELD_CAL_PARAMS, (void *)(cal_data), size,
                    O_CREAT|O_AUTODIR, ALLPERMS);
    }
  }
  else
  {
    /* Write the field cal params to the EFS file */
    val = efs_put(TCXOMGR_EFS_FIELD_CAL_PARAMS, (void *)(cal_data), size,
                  O_CREAT|O_AUTODIR, ALLPERMS);
  }

  /* Check if the data was written correctly */
  if (val < 0)
  {
    /* EFS returned error */
    DBG_1(ERROR, "Field Cal Params: Error writing EFS file %d", val);
  }

  return;
}


/*============================================================================

FUNCTION TCXOMGR_EFS_FIELD_AGING_DATA_WRITE

DESCRIPTION
  This function writes the field aging data from the EFS.
  The parameters are written for GPS and WWAN sturctures.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_efs_field_aging_data_write( void )
{

  tcxomgr_efs_field_aging_type  aging_data[TCXOMGR_TABLE_MAX];
  uint16                        size;
  int                           val;
  uint8                         loop;
  tcxomgr_field_cal_info        *field_data;

  /* --------------------------------------------------------------------- */

  /* Get the size of the data to be read */
  size = sizeof(tcxomgr_efs_field_aging_type) * TCXOMGR_TABLE_MAX;

  /* Get the field cal pointer */
  field_data = tcxomgr.fieldcal_data;

  /* copy the field cal data to a local variable */
  for(loop = 0; loop < TCXOMGR_TABLE_MAX; loop++)
  {
    aging_data[loop].c0_fresh      = field_data[loop].aging_data.c0_fresh;
    aging_data[loop].count         = field_data[loop].aging_data.count;
    aging_data[loop].timestamp     = field_data[loop].aging_data.timestamp;
    aging_data[loop].valid         = field_data[loop].aging_data.valid;
    aging_data[loop].c1_num_points = field_data[loop].aging_data.c1_num_points;
    aging_data[loop].c1_std_dev    = field_data[loop].aging_data.c1_std_dev;
    aging_data[loop].c3_num_points = field_data[loop].aging_data.c3_num_points;
    aging_data[loop].c3_std_dev    = field_data[loop].aging_data.c3_std_dev;

    field_data[loop].aging_data.c0_fresh_nv =
      field_data[loop].aging_data.c0_fresh;
    field_data[loop].aging_data.timestamp_nv =
      field_data[loop].aging_data.timestamp;
  }

  /* Read WWAN from the file */
  val = efs_put(TCXOMGR_EFS_FIELD_AGING_DATA, (void *)(aging_data), size,
                O_CREAT|O_AUTODIR, ALLPERMS);

  /* Check if the data was read correctly */
  if (val < 0)
  {
    /* EFS returned error */
    DBG_1(ERROR, "Field Aging Data: Error writing EFS file %d", val);
  }

  return;
}


/*============================================================================

FUNCTION TCXOMGR_EFS_FT_READ

DESCRIPTION
  This function reads the FT table from the EFS.
  There are two EFS files, one each for GPS and WWAN FT table.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
static boolean tcxomgr_efs_ft_table_read
(
  tcxomgr_ft_table_type          table,
  tcxomgr_efs_vco_struct_type    *vco,
  uint16                         size
)
{
  const char            *filename = NULL;
  int                    val;
  boolean                result = TRUE;

  /* --------------------------------------------------------------------- */

  switch(table)
  {
    case TCXOMGR_TABLE_WWAN:
      filename = TCXOMGR_EFS_FT_TABLE_WWAN;
      break;

    case TCXOMGR_TABLE_GPS:
      filename = TCXOMGR_EFS_FT_TABLE_GPS;
      break;

    default:
      DBG_1(ERROR, "EFS FT table: Invalid table %d", table);
      return FALSE;
  }

  /* Read WWAN from the file */
  val = efs_get(filename, (void *)vco, size);

  /* Check if the data was read correctly */
  if (val < 0)
  {
    /* EFS returned error */
    DBG_2(ERROR,
        "FT Cal Param: Error reading EFS file for table %d: %d",
        table, val);
    result = FALSE;
  }

  /* return whether or not the EFS read was successful */
  return result;

}


/*============================================================================

FUNCTION TCXOMGR_EFS_FT_WRITE

DESCRIPTION
  This function writes the FT table to the EFS.
  There are two EFS files, one each for GPS and WWAN FT table.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function may block while the FT table is being written to EFS.

============================================================================*/
static boolean tcxomgr_efs_ft_table_write
(
  tcxomgr_ft_table_type          table,
  tcxomgr_efs_vco_struct_type    *vco,
  boolean                        async
)
{
  const char  *filename = NULL;
  int          val;
  boolean      result = TRUE;

  /* --------------------------------------------------------------------- */

  switch(table)
  {
    case TCXOMGR_TABLE_WWAN:
      filename = TCXOMGR_EFS_FT_TABLE_WWAN;
      break;

    case TCXOMGR_TABLE_GPS:
      filename = TCXOMGR_EFS_FT_TABLE_GPS;
      break;

    default:
      DBG_1(ERROR, "EFS FT table: Invalid table %d", table);
      return FALSE;
  }

  if(async)
  {
    /* Write to the file asynchronously. Create it if it doesnt exist */
    val = efs_async_put(filename, (void *)vco,
                  sizeof(tcxomgr_efs_vco_struct_type),
                  O_CREAT|O_AUTODIR, ALLPERMS);

    /* efs_async_put() could fail if the async queue is full. If it does, fall
     * back to efs_put(), which will block this task until the write is
     * complete.
     */
    if (val < 0)
    {
      DBG_1(HIGH, "EFS FT table: Unable to write table %d asynchronously. "
          "Falling back to direct call.", table);

      val = efs_put(filename, (void *)vco, sizeof(tcxomgr_efs_vco_struct_type),
                    O_CREAT|O_AUTODIR, ALLPERMS);
    }
  }
  else
  {
    /* Write to the file. Create it if it doesnt exist */
    val = efs_put(filename, (void *)vco, sizeof(tcxomgr_efs_vco_struct_type),
                  O_CREAT|O_AUTODIR, ALLPERMS);
  }

  /* Check if the data was written correctly */
  if (val < 0)
  {
    /* EFS returned error */
    DBG_2(ERROR, "EFS FT table: Error writing to EFS file for table %d: %d",
        table, val);
    result = FALSE;
  }

  return result;

}


/*============================================================================

FUNCTION TCXOMGR_EFS_VCO_READ

DESCRIPTION
  This function reads the FT table and populatest the VCO structure.
  The VCO strucre is read by Field Cal during init and Field FT table
  is populated based on the data.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_efs_vco_read ( void )
{
  /* Loop index */
  int                             i;
  tcxomgr_efs_vco_struct_type     vco;

  /*--------------------------------------------------------------------------*/

  /* Initialize values */
  for ( i = 0; i < VCO_TEMP_TABLE_SIZE; i++ )
  {
    /* Initialize the WWAN table */
    tcxomgr.vco.temp[ i ]        = VCO_UNDEFINED;
    tcxomgr.vco.offset[ i ]      = VCO_UNDEFINED;
    tcxomgr.vco.rotCount[ i ]    = 0;
    tcxomgr.vco.offsetCount[ i ] = 0;

    /* Initialize the GPS table */
    tcxomgr.gps_vco.temp[ i ]        = VCO_UNDEFINED;
    tcxomgr.gps_vco.offset[ i ]      = VCO_UNDEFINED;
    tcxomgr.gps_vco.rotCount[ i ]    = 0;
    tcxomgr.gps_vco.offsetCount[ i ] = 0;
  }

  /* Even though these should not be used in XO targets they are still used
     in formulae resulting in div by zero errors, so set to default anyway */
  tcxomgr.vco.dflt        = VCO_DEFAULT;
  tcxomgr.vco.slope       = VCO_SLOPE_DEFAULT;
  tcxomgr.vco.slope_range = VCO_SLOPE_RANGE_DEFAULT;

  tcxomgr.gps_vco.dflt        = VCO_DEFAULT;
  tcxomgr.gps_vco.slope       = VCO_SLOPE_DEFAULT;
  tcxomgr.gps_vco.slope_range = VCO_SLOPE_RANGE_DEFAULT;

  if ( !tcxomgr_diag_get_temp_table_override_enable() )
  {
    /* Attempt to read WWAN temperature-frequency table */
    if (tcxomgr_efs_ft_table_read(TCXOMGR_TABLE_WWAN, &vco, sizeof(vco)))
    {
      for ( i = 0; i < VCO_TEMP_TABLE_SIZE; i++ )
      {
        tcxomgr.vco.temp[ i ]        = vco.temp[ i ];
        tcxomgr.vco.rotCount[ i ]    = vco.rotCount[ i ];
      }
    }

    /* Attempt to read GPS temperature-frequency table */
    if (tcxomgr_efs_ft_table_read(TCXOMGR_TABLE_GPS, &vco, sizeof(vco)))
    {
      for ( i = 0; i < VCO_TEMP_TABLE_SIZE; i++ )
      {
        tcxomgr.gps_vco.temp[ i ]        = vco.temp[ i ];
        tcxomgr.gps_vco.rotCount[ i ]    = vco.rotCount[ i ];
      }
    }
  }

}


/*============================================================================

FUNCTION TCXOMGR_EFS_VCO_WRITE

DESCRIPTION
  This function copies the data from VCO structure to an EFS structure and
  uses efs ft write function to write the structure to the EFS.

  If table is TCXOMGR_TABLE_MAX, this function will synchronously write both
  the WWAN and GPS FT tables to EFS. This is intended for use during factory
  calibration.

  If table is TCXOMGR_TABLE_WWAN or TCXOMGR_TABLE_GPS, this function will
  asynchronously write the specified FT table to EFS. This is intended for use
  during field calibration, where the Tcxomgr task should not be blocked while
  the EFS write takes place.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_efs_vco_write ( tcxomgr_ft_table_type table )
{
  /* Loop index */
  int                             i;

  /*--------------------------------------------------------------------------*/

  if((table != TCXOMGR_TABLE_WWAN) && (table != TCXOMGR_TABLE_GPS) &&
      (table != TCXOMGR_TABLE_MAX))
  {
    DBG_1(ERROR, "EFS FT table: Invalid table %d", table);
    return;
  }

  if ( !tcxomgr_diag_get_temp_table_override_enable() )
  {
    for ( i = 0; i < VCO_TEMP_TABLE_SIZE; i++ )
    {
      ft_table_efs[TCXOMGR_TABLE_WWAN].vco.temp[i]     =
        (int16)tcxomgr.vco.temp[i];
      ft_table_efs[TCXOMGR_TABLE_WWAN].vco.rotCount[i] =
        tcxomgr.vco.rotCount[i];
      ft_table_efs[TCXOMGR_TABLE_GPS].vco.temp[i]     =
        (int16)tcxomgr.gps_vco.temp[i];
      ft_table_efs[TCXOMGR_TABLE_GPS].vco.rotCount[i] =
        tcxomgr.gps_vco.rotCount[i];
    }

    if(table == TCXOMGR_TABLE_MAX)
    {
      /* Write all the data in EFS immediately. This is used during factory
       * calibration. */
      tcxomgr_efs_ft_table_write(TCXOMGR_TABLE_WWAN,
          &ft_table_efs[TCXOMGR_TABLE_WWAN].vco, FALSE);
      tcxomgr_efs_ft_table_write(TCXOMGR_TABLE_GPS,
          &ft_table_efs[TCXOMGR_TABLE_GPS].vco, FALSE);
    }
    else
    {
      /* Write only the specified table to EFS. This is used during field
       * calibration. Try to write the EFS file asynchronously, so the Tcxomgr
       * task is not blocked while the EFS file is saved.
       */
      tcxomgr_efs_ft_table_write(table, &ft_table_efs[table].vco, TRUE);
    }
  }

}


/*============================================================================

FUNCTION TCXOMGR_GET_VCO_SLOPE

DESCRIPTION


DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void tcxomgr_get_vco_slope
(
  /* vco slope */
  uint16 *slope,

  /* vco range */
  uint8 *range
)
{

  if (slope != NULL)
  {
    *slope = tcxomgr.vco.slope;
  }

  if (range != NULL)
  {
    *range = tcxomgr.vco.slope_range;
  }

} /* tcxomgr_get_vco_slope */

/*============================================================================

FUNCTION TCXOMGR_GET_TEMP_TABLE

DESCRIPTION
  This function retrieves the given temperature's index value.
  Index returned will be in the VCO_TEMP table domain [0, 63]
  The temperature should be passed in degree centigrade
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

static tcxomgr_vco_info_type tcxomgr_get_temp_table
(
  /* temperature at which to find a vco value */
  tcxomgr_temp_type temp32
)
{
  /* Interpolation point at cooler temperatures. */
  tcxomgr_vco_type                vco_low;

  /* Interpolation point at hotter temperatures. */
  tcxomgr_vco_type                vco_high;

  /* Value read from the FT table. It could be vco or rot */
  int16                          ft_table_value;

  /* Interpolation point, distance from current index. */
  uint16                          index_count;

  /* Used as an index to the table */
  uint8               temp;

  /* Return value */
  tcxomgr_vco_info_type vco;

/*--------------------------------------------------------------------------*/
  /* Initialize to defaults */
  tcxomgr_set_default_rgs_info(&vco);
  vco.freq_uncertainty         = 5;

  /* Don't retrive the table if the temperature is out of range or invalid */
  if ( !VALID_TEMP(temp32))
  {
    DBG_1( ERROR, "Temp Table Req, Sat or Inv Temp (%d)", temp32 );
    return vco;
  }

  /* convert thermistor reading to index, Valid index is from 0-63 degrees,
     so safe to cast to 8 bits */
  temp = (uint8)TEMP_TO_INX(temp32);

  /* check for an exact value in the temp table */
  if ( tcxomgr.vco.temp[ temp ] != VCO_UNDEFINED )
  {
    vco.vco_type = TCXOMGR_VCO_EXACT;
    vco.rot_value = tcxomgr.vco.temp[ temp ];
    return vco;
  }

  /* We have no table entry for the current temperature.  Look and look 3
  ** entries in either direction (cooler and hotter) to find a valid entry
  ** in the table.  If we find one, use it.  Otherwise, use default. */
  for (index_count = 1; index_count <= 3; index_count++)
  {
    /* Look lower in the temperature table for an entry. */
    vco_low  = (((temp - index_count) >= 0) ?
                 tcxomgr.vco.temp[ ( temp - index_count ) ] : VCO_UNDEFINED );

    /* Look higher in the temperature table for an entry. */
    vco_high = (((temp + index_count) < VCO_TEMP_TABLE_SIZE) ?
                 tcxomgr.vco.temp[ ( temp + index_count ) ] : VCO_UNDEFINED );

    if ((vco_low != VCO_UNDEFINED) || (vco_high != VCO_UNDEFINED))
    {
      /* We found a temperature table entry close by.  Quit looking. */
      break;
    }
  }

  /* if neither lower or higher VCO_TEMP values are defined use the default */
  if ( vco_low == VCO_UNDEFINED && vco_high == VCO_UNDEFINED )
  {
    vco.vco_type = TCXOMGR_VCO_DEFAULT;
    vco.vco_value = tcxomgr.vco.dflt;
  }
  else
  {
    /* One or both of the lower & higher values are defined so interpolate */
    vco.vco_type = TCXOMGR_VCO_INTERPOLATED;
    if ( vco_low == VCO_UNDEFINED )
    {
      ft_table_value = vco_high;
    }
    else if ( vco_high == VCO_UNDEFINED )
    {
      ft_table_value = vco_low;
    }
    else
    {
      ft_table_value = (vco_low + vco_high) / 2;
    }
    vco.rot_value = ft_table_value;
  }

  return vco;

} /* tcxomgr_get_temp_table */


/*============================================================================

FUNCTION TCXOMGR_GET_INT_VCO_TYPE

DESCRIPTION
   This function returns the vco type maintained internally for XO
   target alone.

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_int_xo_vco_type

SIDE EFFECTS
  None

============================================================================*/
static tcxomgr_int_xo_vco_type tcxomgr_get_int_vco_type
(
  tcxomgr_vco_indicator_type  vco_type,
  tcxomgr_field_state_type    state
)
{
  tcxomgr_int_xo_vco_type  int_vco_type;

/*--------------------------------------------------------------------------*/

  switch(vco_type)
  {
    case TCXOMGR_FT_QUAL_COARSE:
      int_vco_type = TCXOMGR_INT_FT_QUAL_COARSE;
      break;

    case TCXOMGR_FT_QUAL_IFC:
      int_vco_type = TCXOMGR_INT_FT_QUAL_IFC;
      break;

    case TCXOMGR_FT_QUAL_FINE:
      int_vco_type = TCXOMGR_INT_FT_QUAL_FINE;
      break;

    case TCXOMGR_FT_QUAL_FT1:
       if ( state == TCXOMGR_FIELD0 )
       {
         int_vco_type = TCXOMGR_INT_FT_QUAL_C1;
       }
       else
       {
        int_vco_type = TCXOMGR_INT_FT_QUAL_C0;
       }
      break;

    case TCXOMGR_FT_QUAL_FT2:
      switch(state)
      {
        case TCXOMGR_FIELD3:
        default:
          int_vco_type = TCXOMGR_INT_FT_QUAL_FT2_FIELD3;
          break;
        case TCXOMGR_FIELD4:
          int_vco_type = TCXOMGR_INT_FT_QUAL_FT2_FIELD4;
          break;
        case TCXOMGR_FIELD5:
          int_vco_type = TCXOMGR_INT_FT_QUAL_FT2_FIELD5;
          break;
      }
      break;

    case TCXOMGR_FT_QUAL_RGS:
      int_vco_type = TCXOMGR_INT_QUAL_RGS;
      break;

    case TCXOMGR_FT_QUAL_DEFAULT:
      int_vco_type = TCXOMGR_INT_QUAL_DEFAULT;
      break;

    default:
      int_vco_type = TCXOMGR_INT_QUAL_LAST;
      break;
  }

  return int_vco_type;

}

/*============================================================================

FUNCTION TCXOMGR_GET_TCXO_UNCERTAINTY

DESCRIPTION
   This function returns the uncertainity with for TCXO crystal based on
   the VCO Type.

DEPENDENCIES
  None

RETURN VALUE
  Uncertainty in PPB

SIDE EFFECTS
  None

============================================================================*/
static tcxomgr_ppm_type tcxomgr_get_tcxo_uncertainty
(
  tcxomgr_vco_indicator_type   vco_type
)
{
  tcxomgr_ppm_type uncertainty = 5120;

  switch ( vco_type )
  {
    case TCXOMGR_VCO_RGS:
        uncertainty = 512;
        break;

    case TCXOMGR_VCO_OLD_RGS:
        uncertainty = 1024;
        break;

    case TCXOMGR_FT_QUAL_FT1:
        uncertainty = 1024;
        break;

    case TCXOMGR_VCO_EXACT:
        uncertainty = 1024;
        break;

    case TCXOMGR_VCO_DEFAULT:
        uncertainty = 5120;
        break;

    default:
        DBG_1( ERROR, "Invalid VCO type: %d. Using 5ppm uncertainty.",
             vco_type );
        uncertainty = 5120;
        break;
  }

  return uncertainty;
}

/*============================================================================

FUNCTION TCXOMGR_GET_COEFF_UNCERTAINITY

DESCRIPTION
  This function provides the uncertainity of the coeffecient based on the
  Field Cal state

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_ppm_type

SIDE EFFECTS
  None

============================================================================*/
static tcxomgr_ppm_type tcxomgr_get_coef_uncertainity
(
  tcxomgr_vco_indicator_type  qual_ind,
  tcxomgr_field_state_type    state,
  tcxomgr_xo_coef_type        coeff
)
{
  tcxomgr_ppm_type                 coef_uncertainty = 0;
  uint8                            int_vco_type;

/*--------------------------------------------------------------------------*/

  /* Get the internal VCO type enum */
  int_vco_type = (uint8)tcxomgr_get_int_vco_type(qual_ind, state);

  do
  {
    /* check if the array indexes are valid */
    if( ( int_vco_type >= TCXOMGR_INT_QUAL_RGS ) ||
        ( coeff >= TCXOMGR_XO_COEF_LAST ) )
    {
      coef_uncertainty = 0;
      break;
    }

    if ( ( int_vco_type == TCXOMGR_INT_FT_QUAL_COARSE ) &&
         ( coeff == TCXOMGR_XO_COEF_C0 ) )
    {
      /* For c0 in coarse cal state, use the uncertainty value stored in
         the p parameter during coarse calibration                    */
      coef_uncertainty = tcxomgr.cal_data.fac_data.p;
      break;
    }

    /* Return the uncertainty */
    coef_uncertainty = tcxomgr_xo_coef_uncertainity[int_vco_type][coeff];
  } while(0);

  return coef_uncertainty;
}

/*============================================================================

FUNCTION TCXOMGR_GET_INT_PPB_UNCERTAINITY

DESCRIPTION
   This function returns the uncertainity with the FT curve based on
   the state and temparature.

DEPENDENCIES
  None

RETURN VALUE
  uint8

SIDE EFFECTS
  None

============================================================================*/
tcxomgr_ppm_type tcxomgr_get_int_ppb_uncertainty
(
  /* Temperature */
  tcxomgr_temp_type      temp,

  /* VCO type */
  tcxomgr_vco_type       vco_type,

    /*state*/
  tcxomgr_field_state_type  state
)
{
  uint8                 int_vco_type;
  tcxomgr_ppm_type      freq;
  int64                 term3, term2, term1, term_t;
  int32                 c0,c1,c2,c3;

/*--------------------------------------------------------------------------*/

  if ( tcxomgr_get_crystal_type() != TCXO_CRYSTAL )
  {
    /* Get the internal VCO type enum */
    int_vco_type = (uint8)tcxomgr_get_int_vco_type(vco_type, state);

    /* Check if the input is valid */
    if((temp == TCXOMGR_INVALID_TEMP) ||
       (int_vco_type == TCXOMGR_INT_QUAL_LAST))
    {
      /* If the input is not valid then return 56ppm of error */
      return PPM_FULL_TO_PPM(56);
    }
    else if(int_vco_type == TCXOMGR_INT_QUAL_DEFAULT)
    {
      /* If the VCO type is default then return 20 ppm of error */
      return PPM_FULL_TO_PPM(20);
    }
    else if(int_vco_type == TCXOMGR_INT_QUAL_RGS)
    {
      /* RGS is not an expected type. Return 56PPM */
      return PPM_FULL_TO_PPM(56);
    }

    /* Get the uncertainties in each of the coeffecient */
    c0 = tcxomgr_get_coef_uncertainity(vco_type, state, TCXOMGR_XO_COEF_C0);
    c1 = tcxomgr_get_coef_uncertainity(vco_type, state, TCXOMGR_XO_COEF_C1);
    c2 = tcxomgr_get_coef_uncertainity(vco_type, state, TCXOMGR_XO_COEF_C2);
    c3 = tcxomgr_get_coef_uncertainity(vco_type, state, TCXOMGR_XO_COEF_C3);

    /* Absolute Temp change */
    term_t = (temp << 10) - tcxomgr.cal_data.fac_data.t0;
    if(term_t < 0)
    {
      term_t *= -1;
    }

    /* Calculate the C1 error */
    term1 = c1 * term_t;

    /* Calculate the C2 error */
    term2 = c2 * ( ( term_t * term_t ) >> 10);

    /* Calculate the C3 error */
    term3 = c3 * ( ( term_t * term_t * term_t ) >> 20 );

    /* Calculate the total uncertainty and scale to HRPPB units */
    freq = (int32)( term3 >> 30 ) +
           (int32)( term2 >> 26 ) +
           (int32)( term1 >> 22 ) +
           c0;

    /* convert from HRPPB to PPB units */
    freq = (freq>>2);
  }
  else
  {
    freq = tcxomgr_get_tcxo_uncertainty(vco_type);
  }

  /* Return the frequency uncertainty in PPB units */
  return( freq );

}

/*============================================================================

FUNCTION TCXOMGR_GET_STORED_VCO

DESCRIPTION
  Wrapper function for tcxomgr_get_stored_vco_processing.

  This function returns the VCO/ROT value to the Clients.
  If the RGS information is present and is current then that is returned
  if not a decision is made on whether to use old RGS information or FT
  table information and that is returned.

  On XO targets, this function retuns the rotator information for both WWAN
  and GPS. This information is used by GPS to make a decision on which value
  is more reliable and that value gets used.

  GPS values are used by GPS only in XO mode and when the type is FT.

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_vco_info_type

SIDE EFFECTS
  None

============================================================================*/
tcxomgr_vco_info_type tcxomgr_get_stored_vco (void)
{
  tcxomgr_log_rgs_retrieval_data_struct_type rgs_log;

  tcxomgr_vco_info_type vco_info;

  MCSMARKER(TCXOMGR_GET_STORED_VCO_I);

  vco_info = tcxomgr_get_stored_vco_processing(&rgs_log);

  /* Set the API type */
  rgs_log.api_type  =   TCXOMGR_RGS_RETRIEVAL_SYSTEM_RGS;

  tcxomgr_log_rgs_data(TCXOMGR_RGS_RETRIEVAL_LOG_CMD, (void *)&rgs_log);

  MCSMARKER(TCXOMGR_GET_STORED_VCO_O);

  return vco_info;
}

/*============================================================================

FUNCTION TCXOMGR_GET_STORED_VCO_TCXO

DESCRIPTION
  VCO processing function for TCXO crystals.

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_vco_info_type

SIDE EFFECTS
  None

============================================================================*/
static tcxomgr_vco_info_type tcxomgr_get_stored_vco_tcxo
(
  tcxomgr_log_rgs_retrieval_data_struct_type* log_ptr
)
{
  /* Time (in seconds) since 6 JAN 1980 00:00:00 */
  uint32 time;

  /* Current temperature set */
  tcxomgr_temp_set_type temp_struct;

  /* Return Freq/Temp information */
  tcxomgr_vco_info_type ft;

  /* Temperature variables */
  tcxomgr_temp_type temp, prev_temp;

  /*field state type*/
  tcxomgr_field_state_type  state_wwan = 0;
  tcxomgr_field_state_type  state_gps  = 0;

  /* System RGS */
  tcxomgr_rgs_struct_type   system_rgs;

  /* Initialize the VCO type */
  ft.vco_type = TCXOMGR_VCO_DEFAULT;

/*--------------------------------------------------------------------------*/

  /* Initialize all the date in ft */
  tcxomgr_set_default_rgs_info(&ft);

  /* Get current time and temperature */
  tcxomgr_get_time_and_temp(&time, &temp_struct);

  /* Get the system RGS */
  tcxomgr_get_system_rgs(&system_rgs);

  /* Get the therm reading that will be used as RGS Therm in
     degree centigrade. Invalid reading should not be converted into degrees*/
  temp      = TCXOMGR_TEMP_TO_RGS_TEMP(temp_struct);
  prev_temp = TCXOMGR_TEMP_TO_RGS_TEMP(system_rgs.temp);

  if (temp == TCXOMGR_INVALID_TEMP)
  {
    if ( ( system_rgs.id != TCXOMGR_CLIENT_NONE ) &&
         ( time >= system_rgs.time )                  &&
         ( ( time - system_rgs.time ) <= TCXOMGR_AGING_YEAR_IN_SECONDS )  )
    {
      /* For TCXO crystal, if temperature is invalid but
         RGS is less than a year old, use the RGS but
         use higher uncertainty */
      ft.vco_type = TCXOMGR_VCO_OLD_RGS;
      ft.rot_value  = system_rgs.rot;
      ft.vco_value  = system_rgs.vco;

      DBG_3( ERROR, "Invalid temp ret old RGS sys/time:%d/%d Cur time:%d",
         system_rgs.id, system_rgs.time, time );
    }
    else
    {
      /* We do not know the current temperature, and RGS is not reliable, so
      ** return default values to the caller. */
      ft.vco_type   = TCXOMGR_VCO_DEFAULT;
      ft.rot_value  = 0;
      ft.vco_value  = tcxomgr.vco.dflt;

      DBG_3( ERROR, "Invalid temp ret defaults RGS old sys/time:%d/%d Cur time:%d",
             system_rgs.id, system_rgs.time, time );
    }
  }

  /* We have a temp reading */
  else if (    ( system_rgs.id != TCXOMGR_CLIENT_NONE )
            && ( time >= system_rgs.time )
            && ( ( time - system_rgs.time ) <= TCXOMGR_RGS_DELTA_TIME_THRESH )
            && ( ABS( temp - prev_temp ) <= TCXOMGR_RGS_DELTA_TEMP_THRESH )
          )
  {
    /* Temp and time have changed little, return RGS */

    /* If there is no thermistor and rgs is not old, still return old_rgs */
    if ( tcxomgr.vctcxo_therm == TCXO_MAX_THERM )
    {
      ft.vco_type = TCXOMGR_VCO_OLD_RGS;
    }
    else
    {
      ft.vco_type   = TCXOMGR_VCO_RGS;
    }

    ft.rot_value  = system_rgs.rot;
    ft.vco_value  = system_rgs.vco;
    ft.id         = system_rgs.id;
    ft.time_diff  = time - system_rgs.time;
    ft.temp_diff  = ABS( temp - prev_temp );
  }
  else
  /* Return the FT cal estimate if we have calibration performed  */
  {
    /* get FT table */
    ft = tcxomgr_get_temp_table( temp );

    if (( ft.vco_type == TCXOMGR_VCO_DEFAULT ) &&
        ( system_rgs.id != TCXOMGR_CLIENT_NONE ))
    {
      /* Table entry is not present, but we have RGS, return old RGS  */
      ft.vco_type   = TCXOMGR_VCO_OLD_RGS;
      ft.rot_value  = system_rgs.rot;
      ft.vco_value  = system_rgs.vco;
    }
    else if ( ft.vco_type == TCXOMGR_VCO_DEFAULT )
    {
      /* RGS is not present and no entry in the temp table return FT1. */
      ft.vco_type   = TCXOMGR_FT_QUAL_FT1;
      state_wwan    = TCXOMGR_FIELD0;
      state_gps     = TCXOMGR_FIELD0;
    }
    else if ( ft.vco_type == TCXOMGR_VCO_EXACT &&
              tcxomgr.vctcxo_therm == TCXO_MAX_THERM )
    {
       /* we have a entry and temp readings are not present */
       ft.vco_type   = TCXOMGR_VCO_OLD_RGS;
    }
  }

  /* keep record of worst case stored value used */
  if ( ft.vco_type < tcxomgr.uncertainty )
  {
    tcxomgr.uncertainty = ft.vco_type;
  }

  /* Debug Feature: Used for internal testing
     This flag is used to set the uncertainty and error to user defined
     value for testing
  */
  if((tcxomgr_disable_uncertainty_flag == TRUE) &&
     (ft.vco_type != TCXOMGR_VCO_RGS))
  {
    DBG_3(MED, "Diag override: Curr Rot: %d, Uncertainty: %d and Error: %d",
                ft.rot_value, tcxomgr_uncertainty_value,tcxomgr_error_value);
    
    ft.rot_value = ft.rot_value + PPM_FULL_TO_PPM(tcxomgr_error_value);
    ft.freq_uncertainty = tcxomgr_uncertainty_value>>10;  //ppb to ppm
    ft.freq_uncertainty_ppb = tcxomgr_uncertainty_value;
  }
  else
  {
    /* Get the uncertainity in PPB based on the VCO type
       and current operating temparature */
    ft.freq_uncertainty_ppb =
      tcxomgr_get_int_ppb_uncertainty(temp, ft.vco_type, state_wwan);

    /* Round up uncertainty in PPB to PPM */
    ft.freq_uncertainty = TCXOMGR_ROUND_UP_PPB_TO_PPM(ft.freq_uncertainty_ppb);
  }

  /* Update the GPS coeffecients from WWAN */
  ft.gps_vco_type             = ft.vco_type;
  ft.gps_rot_value            = ft.rot_value;
  ft.gps_freq_uncertainty     = ft.freq_uncertainty;
  ft.gps_freq_uncertainty_ppb = ft.freq_uncertainty_ppb;

  if (log_ptr != NULL)
  {
    /* Populate log data */
    log_ptr->rgs_tech             = \
            (ft.vco_type == TCXOMGR_VCO_RGS) ?
            system_rgs.id : TCXOMGR_CLIENT_NONE;
    log_ptr->as_id                = TCXOMGR_AS_ID_NONE;
    log_ptr->temp_xo              = temp;
    log_ptr->temp_pmic            = temp_struct.pmic;
    log_ptr->timestamp_ms         = timetick_get_ms();
    log_ptr->correction           = 0;
    log_ptr->vco_value            = ft.vco_value;
    log_ptr->rot_value            = ft.rot_value;
    log_ptr->wwan_rot_val_est     = 0;
    log_ptr->gps_rot_val_est      = 0;
    log_ptr->vco_type             = ft.vco_type;
    log_ptr->freq_uncertainty     = ft.freq_uncertainty;
    log_ptr->freq_uncertainty_ppb = ft.freq_uncertainty_ppb;
    log_ptr->wwan_data_available  = 0;
    log_ptr->state_wwan           = state_wwan;
    log_ptr->state_gps            = state_gps;
  }

  return ft;
}

/*============================================================================

FUNCTION TCXOMGR_GET_STORED_VCO_XO

DESCRIPTION
  VCO processing function for XO crystals.

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_vco_info_type

SIDE EFFECTS
  None

============================================================================*/
tcxomgr_vco_info_type tcxomgr_get_stored_vco_xo
(
  /* Pointer to the RGS values */
  tcxomgr_rgs_struct_type*                    rgs_ptr,

  /* Pointer to logging data                          */
  tcxomgr_log_rgs_retrieval_data_struct_type* log_ptr
)
{
  /* Time (in seconds) since 6 JAN 1980 00:00:00 */
  uint32 time;

  /* Current temperature set */
  tcxomgr_temp_set_type temp_struct;

  /* Return Freq/Temp information */
  tcxomgr_vco_info_type ft;

  /* Temperature variables */
  tcxomgr_temp_type temp;

  tcxomgr_ppm_type wwan_rot_val_est = 0;
  tcxomgr_ppm_type gps_rot_val_est = 0;

  /* Flag to indicate whether WWAN FT data was used
     for RGS estimation */
  uint8 using_wwan_ft_data = FALSE;

  /*field state type*/
  tcxomgr_field_state_type  state_wwan = 0;
  tcxomgr_field_state_type  state_gps  = 0;

/*--------------------------------------------------------------------------*/

  /* Initialize all the date in ft */
  tcxomgr_set_default_rgs_info(&ft);

  /* Get current time and temperature */
  tcxomgr_get_time_and_temp(&time, &temp_struct);

  /* Get the therm reading that will be used as RGS Therm in
     degree centigrade. Invalid reading should not be converted into degrees*/
  temp      = TCXOMGR_TEMP_TO_RGS_TEMP(temp_struct);

  /* If the RGS pointer is valid, try to use the RGS for estimate. */
  if ( rgs_ptr != NULL )
  {
    /* Try to see if we can get estimates from RGS */
    ft = tcxomgr_get_stored_vco_from_rgs(time, temp_struct, log_ptr, rgs_ptr);
  }

  /* If we did not get a valid estimate based on RGS, try to get the estimate
     based on the FT curve */
  if ( ft.vco_type != TCXOMGR_VCO_RGS )
  {
    if ( temp == TCXOMGR_INVALID_TEMP )
    {
      /* We do not know the current temperature, and RGS is not reliable, so
      ** return default values to the caller. */
      ft.vco_type   = TCXOMGR_VCO_DEFAULT;
      ft.rot_value  = 0;
      ft.vco_value  = tcxomgr.vco.dflt;

      if ( rgs_ptr != NULL )
      {
        DBG_3( ERROR, "Invalid temp ret defaults RGS old sys/time:%d/%d Cur time:%d",
           rgs_ptr->id, rgs_ptr->time, time );
      }

      /* Update the GPS coeffecients from WWAN as we are relying on RGS */
      ft.gps_vco_type         = ft.vco_type;
      ft.gps_rot_value        = ft.rot_value;
    }
    else
    {
      /* read the field cal quality indicator to determine the vco_type */
      ft.vco_type  =
          (tcxomgr_vco_indicator_type)
          tcxomgr.fieldcal_data[ TCXOMGR_TABLE_WWAN].cal_data.ft_qual_ind;
      state_wwan   =  tcxomgr.fieldcal_data[TCXOMGR_TABLE_WWAN].cal_data.state;

      ft.gps_vco_type  =
          (tcxomgr_vco_indicator_type)
           tcxomgr.fieldcal_data[ TCXOMGR_TABLE_GPS].cal_data.ft_qual_ind;
       state_gps  =  tcxomgr.fieldcal_data[TCXOMGR_TABLE_GPS].cal_data.state;

      /* Calculate the Freq vs. Temp PPM error value using the calibrated
         FT curve */
      ft.rot_value = tcxomgr_ft_eval( temp_struct );
      ft.gps_rot_value = tcxomgr_ft_eval_gps( temp_struct );

      /* Set the values used for logging */
      wwan_rot_val_est = ft.rot_value;
      gps_rot_val_est  = ft.gps_rot_value;

      if ( tcxomgr_ft_valid() == TRUE )
      {
        /* This is for XO targets only, so vco value is 0 */
        ft.vco_value = 0;
      }
      else
      {
        /* VCO value is used as an error check in XO targets, set to default */
        ft.vco_value  = tcxomgr.vco.dflt;
      }
    }

    /* Get the uncertainity in PPB based on the VCO type
       and current operating temparature */
    ft.freq_uncertainty_ppb =
      tcxomgr_get_int_ppb_uncertainty(temp, ft.vco_type, state_wwan);
    ft.gps_freq_uncertainty_ppb =
      tcxomgr_get_int_ppb_uncertainty(temp, ft.gps_vco_type, state_gps);

    /* Round up uncertainty in PPB to PPM */
    ft.freq_uncertainty = TCXOMGR_ROUND_UP_PPB_TO_PPM(ft.freq_uncertainty_ppb);
    ft.gps_freq_uncertainty =
      TCXOMGR_ROUND_UP_PPB_TO_PPM(ft.gps_freq_uncertainty_ppb);

    /* Debug Feature: Used for internal testing
       This flag is used to set the uncertainty and error to user defined
       value for testing
    */
    if((tcxomgr_disable_uncertainty_flag == TRUE) &&
       (ft.vco_type != TCXOMGR_VCO_RGS))
    {
      DBG_3(MED, "Diag override: Curr Rot: %d, Uncertainty: %d and Error: %d",
                  ft.rot_value, tcxomgr_uncertainty_value,tcxomgr_error_value);
      
      ft.rot_value = ft.rot_value + PPM_FULL_TO_PPM(tcxomgr_error_value);
      ft.freq_uncertainty = tcxomgr_uncertainty_value;
      ft.freq_uncertainty_ppb = PPM_FULL_TO_PPM(tcxomgr_uncertainty_value);
    }

    /* If GPS active client is disabled or if WWAN table is availabe
       use WWAN data to provide to GPS
       GPS data is used only WWAN is not available
    */
    if((tcxomgr_ft_data_is_valid(TCXOMGR_TABLE_WWAN))||
       (tcxomgr_disable_gps_client == TRUE))
    {
      ft.gps_freq_uncertainty = ft.freq_uncertainty;
      ft.gps_freq_uncertainty_ppb = ft.freq_uncertainty_ppb;
      ft.gps_vco_type  = ft.vco_type;
      ft.gps_rot_value = ft.rot_value;
      using_wwan_ft_data = TRUE;
    }

    if (log_ptr != NULL)
    {
      /* Populate log data */
      log_ptr->rgs_tech             = TCXOMGR_CLIENT_NONE;
      log_ptr->as_id                = TCXOMGR_AS_ID_NONE;
      log_ptr->temp_xo              = temp_struct.xo;
      log_ptr->temp_pmic            = temp_struct.pmic;
      log_ptr->timestamp_ms         = timetick_get_ms();
      log_ptr->correction           = 0;
      log_ptr->vco_value            = ft.vco_value;
      log_ptr->rot_value            = ft.rot_value;
      log_ptr->wwan_rot_val_est     = wwan_rot_val_est;
      log_ptr->gps_rot_val_est      = gps_rot_val_est;
      log_ptr->vco_type             = ft.vco_type;
      log_ptr->freq_uncertainty     = ft.freq_uncertainty;
      log_ptr->freq_uncertainty_ppb = ft.freq_uncertainty_ppb;
      log_ptr->wwan_data_available  = using_wwan_ft_data;
      log_ptr->state_wwan           = state_wwan;
      log_ptr->state_gps            = state_gps;
    }
  }

  if(temp != TCXOMGR_INVALID_TEMP)
  {
    if(tcxomgr_ft_data_is_valid(TCXOMGR_TABLE_WWAN))
    {
      /* Populate the current FT curve value and uncertainty using the WWAN
       * table */
      ft.ft_vco_type = (tcxomgr_vco_indicator_type)
        tcxomgr.fieldcal_data[TCXOMGR_TABLE_WWAN].cal_data.ft_qual_ind,
      ft.ft_rot_value = tcxomgr_ft_eval(temp_struct);
      ft.ft_freq_uncertainty_ppb = tcxomgr_get_int_ppb_uncertainty(temp,
          tcxomgr.fieldcal_data[TCXOMGR_TABLE_WWAN].cal_data.ft_qual_ind,
          tcxomgr.fieldcal_data[TCXOMGR_TABLE_WWAN].cal_data.state);
    }
    else
    {
      /* Populate the current FT curve value and uncertainty using the GPS
       * table */
      ft.ft_vco_type = (tcxomgr_vco_indicator_type)
        tcxomgr.fieldcal_data[TCXOMGR_TABLE_GPS].cal_data.ft_qual_ind,
      ft.ft_rot_value = tcxomgr_ft_eval_gps(temp_struct);
      ft.ft_freq_uncertainty_ppb = tcxomgr_get_int_ppb_uncertainty(temp,
          tcxomgr.fieldcal_data[TCXOMGR_TABLE_GPS].cal_data.ft_qual_ind,
          tcxomgr.fieldcal_data[TCXOMGR_TABLE_GPS].cal_data.state);
    }
  }

  /* Verify returned rotator is within limits */
  if((ft.rot_value > MAX_ROT_OUT_VALUE) || (ft.rot_value < MIN_ROT_OUT_VALUE))
  {
    ERR_FATAL("Tcxomgr rotator out %d is out of bounds (temp %d, ft qual %d)",
        ft.rot_value, temp_struct.xo, ft.vco_type);
  }

  return ft;
}

/*============================================================================

FUNCTION TCXOMGR_GET_STORED_VCO_VCTCXO

DESCRIPTION
  VCO processing function for VCTCXO crystals.

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_vco_info_type

SIDE EFFECTS
  None

============================================================================*/
static tcxomgr_vco_info_type tcxomgr_get_stored_vco_vctcxo
(
  tcxomgr_log_rgs_retrieval_data_struct_type* log_ptr
)
{
    /* Time (in seconds) since 6 JAN 1980 00:00:00 */
  uint32 time;

  /* Current temperature set */
  tcxomgr_temp_set_type temp_struct;

  /* Return Freq/Temp information */
  tcxomgr_vco_info_type ft;

  /* Temperature variables */
  tcxomgr_temp_type temp, prev_temp;

  /* System RGS */
  tcxomgr_rgs_struct_type   system_rgs;

/*--------------------------------------------------------------------------*/

  /* Initialize all the date in ft */
  tcxomgr_set_default_rgs_info(&ft);

  /* Get current time and temperature */
  tcxomgr_get_time_and_temp(&time, &temp_struct);

  /* Get the system RGS */
  tcxomgr_get_system_rgs(&system_rgs);

  /* Get the therm reading that will be used as RGS Therm in
     degree centigrade. Invalid reading should not be converted into degrees*/
  temp      = TCXOMGR_TEMP_TO_RGS_TEMP(temp_struct);
  prev_temp = TCXOMGR_TEMP_TO_RGS_TEMP(system_rgs.temp);

  if (temp == TCXOMGR_INVALID_TEMP)
  {
    /* Invalid temp read, if last RGS update was less than 30 seconds return
       RGS, else return default values */
    if ( ( system_rgs.id != TCXOMGR_CLIENT_NONE ) &&
         ( time >= system_rgs.time )                  &&
         ( ( time - system_rgs.time ) <=
           TCXOMGR_RGS_DELTA_TIME_NO_TEMP_THRESH ) )
    {
      /* Even though we don't know the temperature of the phone, the RGS value
         is not very old.  Temperature should not have changed much, so let's
         still go ahead and use the RGS values. */
      ft.vco_type   = TCXOMGR_VCO_RGS;
      ft.rot_value  = system_rgs.rot;
      ft.vco_value  = system_rgs.vco;

      ft.id         = system_rgs.id;
      ft.time_diff  = time - system_rgs.time;
      ft.temp_diff  = TCXOMGR_INVALID_TEMP;

      DBG_3( ERROR, "Invalid temp ret RGS sys/time:%d/%d Cur time:%d",
             system_rgs.id, system_rgs.time, time );
    }
    else
    {
      /* We do not know the current temperature, and RGS is not reliable, so
      ** return default values to the caller. */
      ft.vco_type   = TCXOMGR_VCO_DEFAULT;
      ft.rot_value  = 0;
      ft.vco_value  = tcxomgr.vco.dflt;

      DBG_3( ERROR, "Invalid temp ret defaults RGS old sys/time:%d/%d Cur time:%d",
             system_rgs.id, system_rgs.time, time );
    }

    /* Update the GPS coeffecients from WWAN as we are relying on RGS */
    ft.gps_vco_type         = ft.vco_type;
    ft.gps_rot_value        = ft.rot_value;
  }

  /* We have a temp reading */
  else if (    ( system_rgs.id != TCXOMGR_CLIENT_NONE )
            && ( time >= system_rgs.time )
            && ( ( time - system_rgs.time ) <= TCXOMGR_RGS_DELTA_TIME_THRESH )
            && ( ABS( temp - prev_temp ) <= TCXOMGR_RGS_DELTA_TEMP_THRESH )
          )
  {
    /* Temp and time have changed little, return RGS */

    ft.vco_type   = TCXOMGR_VCO_RGS;
    ft.rot_value  = system_rgs.rot;
    ft.vco_value  = system_rgs.vco;

    ft.id         = system_rgs.id;
    ft.time_diff  = time - system_rgs.time;
    ft.temp_diff  = ABS( temp - prev_temp );

    /* Update the GPS coeffecients from WWAN as we are using RGS */
    ft.gps_vco_type     = ft.vco_type;
    ft.gps_rot_value    = ft.rot_value;
  }

  else
    /* Lot of time has elapsed or temp has changed quite a bit */
    /* Return the temp table entry */
  {
    /* returns temp tbl entry, or default if no entry exists */
    ft = tcxomgr_get_temp_table( temp );

    if ( ( ft.vco_type == TCXOMGR_VCO_DEFAULT ) &&
         ( system_rgs.id != TCXOMGR_CLIENT_NONE ) &&
         ( ABS( temp - prev_temp ) <= TCXOMGR_RGS_DELTA_TEMP_THRESH ) )
    {
      ft.vco_type   = TCXOMGR_VCO_OLD_RGS;
      ft.rot_value  = system_rgs.rot;
      ft.vco_value  = system_rgs.vco;
    }
  }

  /* keep record of worst case stored value used */
  if ( ft.vco_type < tcxomgr.uncertainty )
  {
    tcxomgr.uncertainty = ft.vco_type;
  }

  if (log_ptr != NULL)
  {
    /* Populate log data */
    log_ptr->rgs_tech             = \
            (ft.vco_type == TCXOMGR_VCO_RGS) ?
            system_rgs.id : TCXOMGR_CLIENT_NONE;
    log_ptr->as_id                = TCXOMGR_AS_ID_NONE;
    log_ptr->temp_xo              = temp;
    log_ptr->temp_pmic            = temp_struct.pmic;
    log_ptr->timestamp_ms         = timetick_get_ms();
    log_ptr->correction           = 0;
    log_ptr->vco_value            = ft.vco_value;
    log_ptr->rot_value            = ft.rot_value;
    log_ptr->wwan_rot_val_est     = 0;
    log_ptr->gps_rot_val_est      = 0;
    log_ptr->vco_type             = ft.vco_type;
    log_ptr->freq_uncertainty     = ft.freq_uncertainty;
    log_ptr->freq_uncertainty_ppb = ft.freq_uncertainty_ppb;
    log_ptr->wwan_data_available  = 0;
    log_ptr->state_wwan           = 0;
    log_ptr->state_gps            = 0;
  }

  return ft;
}

/*============================================================================

FUNCTION TCXOMGR_GET_STORED_VCO_PROCESSING

DESCRIPTION
  This function returns the VCO/ROT value to the Clients.
  If the RGS information is present and is current then that is returned
  if not a decision is made on whether to use old RGS information or FT
  table information and that is returned.

  On XO targets, this function retuns the rotator information for both WWAN
  and GPS. This information is used by GPS to make a decision on which value
  is more reliable and that value gets used.

  GPS values are used by GPS only in XO mode and when the type is FT.

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_vco_info_type

SIDE EFFECTS
  None

============================================================================*/
tcxomgr_vco_info_type tcxomgr_get_stored_vco_processing
(
  tcxomgr_log_rgs_retrieval_data_struct_type* log_ptr
)
{
  /* Return Freq/Temp information */
  tcxomgr_vco_info_type   ft;
  tcxomgr_rgs_struct_type system_rgs;

  tcxomgr_crystal_type xtal_type = tcxomgr_get_crystal_type();

  /* Get the system RGS */
  tcxomgr_get_system_rgs(&system_rgs);

  /* Start of critical section */
  TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

  switch ( xtal_type )
  {
    case XO_CRYSTAL:
      ft = tcxomgr_get_stored_vco_xo(&system_rgs, log_ptr);

      /* keep record of worst case stored value used.
         if uncertainty is default, it could have been due to no temp read
         attempts, so reset it now if the ft.vco_type has changed */
      if ((ft.vco_type < tcxomgr.uncertainty) ||
           (tcxomgr.uncertainty == TCXOMGR_VCO_DEFAULT))
      {
        tcxomgr.uncertainty = ft.vco_type;
      }
      break;

    case TCXO_CRYSTAL:
      ft = tcxomgr_get_stored_vco_tcxo(log_ptr);
      break;

    case VCTCXO_CRYSTAL:
      ft = tcxomgr_get_stored_vco_vctcxo(log_ptr);
      break;

    default:
      DBG_1(ERROR,"tcxomgr_get_stored_vco: Unsupported crystal type: %d", 
                  xtal_type);
      break;
  }

  /* List out what we are returning */
  DBG_5(HIGH, "Retrieve RGS Temp: %d Rot: %d VCO Type: %d "
              "Uncertainty: %d system: %d",
                 log_ptr->temp_xo, ft.rot_value,
                 ft.vco_type, ft.freq_uncertainty,
                 system_rgs.id);

  /* End of critical section */
  TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);

  return ft;
}

/*============================================================================

FUNCTION TCXOMGR_GET_STORED_AGED_VCO

DESCRIPTION
  This function returns the VCO/ROT value after taking into account
  the aging of the XO crystal to the Clients.

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_vco_info_type

SIDE EFFECTS
  None

============================================================================*/
tcxomgr_vco_info_type tcxomgr_get_stored_vco_aged()
{

  uint32                        time;
  uint32                        time_diff = 0;
  int32                         sign;
  tcxomgr_vco_info_type         ft;
  tcxomgr_cal_aging_data_type   *aging_wwan_ptr;
  tcxomgr_cal_aging_data_type   *aging_gps_ptr;
  tcxomgr_cal_field_data_type   *aging_ft_ptr;
  tcxomgr_rotator_type          aging_rot_val = 0;
  uint32                        uncertainty  =0;
  tcxomgr_log_rgs_retrieval_data_struct_type rgs_log;
  tcxomgr_log_aged_rgs_retrieval_data_struct_type aged_rgs_log;
/*--------------------------------------------------------------------------*/

  MCSMARKER(TCXOMGR_GET_VCO_AGED_I);

  /* Cases handled in this API are:
     - IF Time is Invalid then default values are returned
     - IF FT is of RGS type then Aging is not added
     - IF Field cal has not happened then Aging is not added
     - IF Field C0 is new then aging is not added
     - IF C0 Fresh is available and Field C0 is old aging is added
     - IF Field C0 is old and C0 fresh is unavailable then Unc is increased
  */

  /* Get the ft values. this value does not account for aging of crystal */
  ft = tcxomgr_get_stored_vco_processing(&rgs_log);

  /* Get the pointer to calibration data that is used */
  if(tcxomgr_ft_data_is_valid(TCXOMGR_TABLE_WWAN))
  {
    aging_ft_ptr  = &(tcxomgr.fieldcal_data[TCXOMGR_TABLE_WWAN].cal_data);
  }
  else
  {
    aging_ft_ptr  = &(tcxomgr.fieldcal_data[TCXOMGR_TABLE_GPS].cal_data);
  }

  /* Aging data is not used if the FT is of RGS type or
     If the Field Calibration has not kicked in         */
  if((ft.gps_vco_type == TCXOMGR_VCO_RGS) ||
     (aging_ft_ptr->timestamp == 0))
  {
    DBG_2(MED, "Retrieve aging RGS:  Aging Impact not added."
                " Vco Type: %d, time stamp: %d",
                ft.gps_vco_type, aging_ft_ptr->timestamp);
    MCSMARKER(TCXOMGR_GET_VCO_AGED_O);
    return ft;
  }

  /* Get current timestamp */
  time = time_get_secs();

  /* If there is no valid time then return default values */
  if(time < TCXOMGR_EPOC_TIME)
  {
    DBG_0(ERROR,"Retrieve aging RGS: Returning Default aging values");
    ft.gps_freq_uncertainty += TCXOMGR_AGING_C0_SATURATION_PPM;
    ft.gps_freq_uncertainty_ppb += PPM_FULL_TO_PPM(TCXOMGR_AGING_C0_SATURATION_PPM);
    MCSMARKER(TCXOMGR_GET_VCO_AGED_O);
    return ft;
  }

  /* Get the pointer to aging data of WWAN and GPS */
  aging_wwan_ptr = &(tcxomgr.fieldcal_data[TCXOMGR_TABLE_WWAN].aging_data);
  aging_gps_ptr  = &(tcxomgr.fieldcal_data[TCXOMGR_TABLE_GPS].aging_data);

  /* If the field cal data is less than 2 months old then do nothing */
  if( ((time - aging_ft_ptr->timestamp) < TCXOMGR_AGING_C0_TIME)||
       (time < aging_ft_ptr->timestamp) ||
       (time < aging_wwan_ptr->timestamp) ||
       (time < aging_gps_ptr->timestamp) )
  {
    aging_rot_val = 0;
    time_diff = time - aging_ft_ptr->timestamp;
  }
  /* If field cal is more than 2 months old and C0 fresh WWAN is new
     then add C0 fresh WWAN offset to the estimates */
  else if((aging_wwan_ptr->valid == TRUE) &&
          ((time - aging_wwan_ptr->timestamp) < TCXOMGR_AGING_C0_TIME))
  {
    aging_rot_val = PPM_HR_PPM_TO_PPM(aging_wwan_ptr->c0_fresh);
    time_diff = time - aging_wwan_ptr->timestamp;
  }
  /* If field cal is more than 2 months old and C0 fresh GPS is new
     then add C0 fresh GPS offset to the estimates */
  else if((aging_gps_ptr->valid == TRUE) &&
          ((time - aging_gps_ptr->timestamp) < TCXOMGR_AGING_C0_TIME))
  {
    aging_rot_val = PPM_HR_PPM_TO_PPM(aging_gps_ptr->c0_fresh);
    time_diff = time - aging_gps_ptr->timestamp;
  }
  /* field cal data is more than 2 months old and there is no C0 fresh
     then increase uncertainty */
  else
  {
    aging_rot_val = 0;
    uncertainty =  (uint32)((time - aging_ft_ptr->timestamp)/
                  TCXOMGR_AGING_YEAR_IN_SECONDS);
    uncertainty = (uncertainty > TCXOMGR_AGING_C0_SATURATION_PPM) ? TCXOMGR_AGING_C0_SATURATION_PPM:uncertainty;

    ft.gps_freq_uncertainty += uncertainty;

    uncertainty =  (uint32)(((time - aging_ft_ptr->timestamp)* 1024)/
                   TCXOMGR_AGING_YEAR_IN_SECONDS);
    uncertainty = (uncertainty > TCXOMGR_AGING_C0_SATURATION_PPM*1024)? TCXOMGR_AGING_C0_SATURATION_PPM*1024:uncertainty;

    ft.gps_freq_uncertainty_ppb += uncertainty;

    time_diff = time - aging_ft_ptr->timestamp;
  }

  /* Saturate the C0 change due to Aging to 7PPM max */
  if(ABS(aging_rot_val) >
     PPM_FULL_TO_PPM(TCXOMGR_AGING_C0_SATURATION_PPM))
  {
    sign = aging_rot_val/ABS(aging_rot_val);
    aging_rot_val = sign * PPM_FULL_TO_PPM(TCXOMGR_AGING_C0_SATURATION_PPM);
  }

  /* Add the aging impact to the rotator value calculated by get_stored_vco */
  ft.gps_rot_value += aging_rot_val;

  /* Print the return values */
  DBG_5( HIGH, "Retrieve aging RGS: rot: %d, unc: %d, type: %d, aging_rot_val: %d, time_diff: %d",
            ft.gps_rot_value, ft.gps_freq_uncertainty_ppb,
            ft.gps_vco_type, aging_rot_val, time_diff);

  /* Log the data */
  aged_rgs_log.rgs_tech             = rgs_log.rgs_tech;
  aged_rgs_log.temp_xo              = rgs_log.temp_xo;
  aged_rgs_log.temp_pmic            = rgs_log.temp_pmic;
  aged_rgs_log.timestamp_ms         = rgs_log.timestamp_ms;
  aged_rgs_log.correction           = rgs_log.correction;
  aged_rgs_log.vco_value            = rgs_log.vco_value;
  aged_rgs_log.rot_value            = rgs_log.rot_value;
  aged_rgs_log.wwan_rot_val_est     = rgs_log.wwan_rot_val_est;
  aged_rgs_log.gps_rot_val_est      = rgs_log.gps_rot_val_est;
  aged_rgs_log.vco_type             = rgs_log.vco_type;
  aged_rgs_log.freq_uncertainty     = rgs_log.freq_uncertainty;
  aged_rgs_log.freq_uncertainty_ppb = rgs_log.freq_uncertainty_ppb;
  aged_rgs_log.wwan_data_available  = rgs_log.wwan_data_available;
  aged_rgs_log.aging_rot_val        = aging_rot_val;
  aged_rgs_log.uncertainty          = uncertainty;
  tcxomgr_log_rgs_data(TCXOMGR_AGED_RGS_RETRIEVAL_LOG_CMD, (void *)&aged_rgs_log);

  MCSMARKER(TCXOMGR_GET_VCO_AGED_O);

  /* return the age accounted FT value */
  return ft;
}

/*============================================================================

FUNCTION TCXOMGR_GET_SLOPE_UNCERTAINITY

DESCRIPTION
  This function provides the slope uncertainity based on the FT curve
  and the temparature

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_slope_uncertainity_type

SIDE EFFECTS
  None

============================================================================*/
tcxomgr_ext_slope_uncertainty_type  tcxomgr_get_slope_uncertainity (void)
{
  int32                                c3, c1;
  tcxomgr_temp_type                    t, t0;
  tcxomgr_temp_set_type                temp;
  tcxomgr_ext_slope_uncertainty_type   slope_uncertainty;
  int64                                term_t, term1, term3, term2;
  tcxomgr_cal_field_data_type          *cal_data;
  uint32                               time;
  uint32                               loop;
  tcxomgr_slope_uncertainity_type      uncertainty[TCXOMGR_TABLE_MAX];

/*--------------------------------------------------------------------------*/

  /* Get the current temp and time */
  tcxomgr_get_time_and_temp(&time, &temp);

  /* Get the XO temp and the crystal T0 */
  t= temp.xo;
  t0 = tcxomgr.cal_data.fac_data.t0;

  /* Get the temp term,
     take absolute value to get positive uncertainity terms*/
  term_t = abs(t - t0);

  for(loop=0; loop< TCXOMGR_TABLE_MAX; loop++)
  {
    /* Get WWAN data */
    cal_data = &(tcxomgr.fieldcal_data[loop].cal_data);

    /* Get the uncertainty in each of the coeffecients */
    c3 = tcxomgr_get_coef_uncertainity
           ((tcxomgr_vco_indicator_type)cal_data->ft_qual_ind, cal_data->state,
           TCXOMGR_XO_COEF_C3);
    c1 = tcxomgr_get_coef_uncertainity
           ((tcxomgr_vco_indicator_type)cal_data->ft_qual_ind, cal_data->state,
           TCXOMGR_XO_COEF_C1);

     /* Calculate the C1 error */
    term1 = c1;

    /* Calculate the C3 error */
    term3 = 3 * c3 * ( ( term_t * term_t ) >> 10 );

    /*Calculate the error due to difference in t0*/
    term2 = 2 * TCXOMGR_MAX_C2_ERORR * term_t;

    /* Calculate the total uncertainty and scale to HRPPB units */
    uncertainty[loop] = (int32)( term3 >> 30 ) + (int32)( term1 >> 12 ) + (int32)( term2 >> 26);

    /* convert the HRPPB units to PPB units */
    uncertainty[loop] = (uncertainty[loop]>>2);
  }

  /* Fill the slope uncertainty structure */
  slope_uncertainty.gps_slope_uncertainty =
    uncertainty[TCXOMGR_TABLE_GPS];
  slope_uncertainty.wwan_slope_uncertainty =
    uncertainty[TCXOMGR_TABLE_WWAN];

  if((tcxomgr_ft_data_is_valid(TCXOMGR_TABLE_WWAN))||
     (tcxomgr_disable_gps_client == TRUE))
  {
    slope_uncertainty.gps_slope_uncertainty =
      slope_uncertainty.wwan_slope_uncertainty;
  }
  else
  {
    DBG_2(MED,"Slope Uncertainty. WWAN: %d, GPS: %d",
      slope_uncertainty.wwan_slope_uncertainty,
      slope_uncertainty.gps_slope_uncertainty);
  }

 /* Return the result */
  return slope_uncertainty;

}

/*============================================================================

FUNCTION TCXOMGR_GET_FREQ_ERR_TEMP

DESCRIPTION
  This function reads the trk_lo vs. temperature table and returns the trk_lo
  value corresponding to the current temperature of the phone.

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_vco_indicator_type - Describes trk_lo type, DEFAULT, EXACT, etc.
  tcxomgr_vco_type *vco      - Return parameter with actual VCO value.

SIDE EFFECTS
  None

============================================================================*/

tcxomgr_vco_indicator_type tcxomgr_get_freq_err_temp
(
  /* VCTCXO Accumulator value for the current temperature */
  tcxomgr_vco_type *vco
)
{
  tcxomgr_vco_info_type  vco_info;
  tcxomgr_temp_set_type  temp_set;

  /* Time (in seconds) since 6 JAN 1980 00:00:00 */
  uint32                 current_time;

/*--------------------------------------------------------------------------*/

  tcxomgr_get_time_and_temp(&current_time, &temp_set);

  /* returns temp tbl entry, or default if no entry exists */
  vco_info = tcxomgr_get_temp_table( TCXOMGR_TEMP_TO_RGS_TEMP(temp_set) );

  *vco = vco_info.vco_value;

  return vco_info.vco_type;

} /* tcxomgr_get_freq_err_temp( ) */


/*============================================================================

FUNCTION TCXOMGR_GET_DEFAULT_FREQ_ERR

DESCRIPTION
  This function returns the default value of the trk_lo PDM.

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_vco_type - Default trk_lo value.

SIDE EFFECTS
  None

============================================================================*/

tcxomgr_vco_type tcxomgr_get_default_freq_err(void)
{
  return tcxomgr.vco.dflt;
}


/*============================================================================

FUNCTION TCXOMGR_GET_UNCERTAINTY

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_vco_indicator_type

SIDE EFFECTS
  None

============================================================================*/
tcxomgr_vco_indicator_type tcxomgr_get_uncertainty (void)
{
  uint32 current_time = 0;
  tcxomgr_rgs_struct_type system_rgs;

  /* Get the system RGS */
  tcxomgr_get_system_rgs(&system_rgs);

  /* Get current timestamp */
  current_time = time_get_secs();

  if ( (tcxomgr_get_crystal_type()== TCXO_CRYSTAL )  &&
       (tcxomgr.vctcxo_therm == TCXO_MAX_THERM) )
  {
    /* For TCXO crystal, return old rgs if there is no thermistor */
    return TCXOMGR_VCO_OLD_RGS;
  }

  if (tcxomgr.uncertainty == TCXOMGR_VCO_RGS)
  {
    /* Is RGS newer than 30 secs */
    if ((current_time - system_rgs.time) < TCXOMGR_RGS_RECENT_THRESHOLD )
    {
      return TCXOMGR_VCO_RGS_RECENT;
    }
  }

  return tcxomgr.uncertainty;
}


/*============================================================================

FUNCTION TCXOMGR_UPDATE_TEMP_TABLE

DESCRIPTION
 	This function updates the VCO temperature calibrated table
 	The temperature should be in degree centigrade

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void tcxomgr_update_temp_table
(
  /* VCTCXO accumulator value (TRK_LO_ADJ PDM value) */
  tcxomgr_vco_type vco_acc,

  /* Time (in seconds) since 6 JAN 1980 00:00:00 */
  uint32 time,

  /* Temperature */
  tcxomgr_temp_type temp32
)
{
  /* Previous VCTCXO value for current temperature */
  tcxomgr_vco_info_type old_vco_temp_info;

  /* Previous VCTCXO value for current temperature */
  tcxomgr_vco_type old_vco_temp;

  /* New VCTCXO value for current temperature */
  tcxomgr_vco_type new_vco_temp;

  /* Typecasting to tcxomgr_temp_type to remove lint errors */
  tcxomgr_temp_type temp;

  /* Delta between old and new vco_temp */
  tcxomgr_vco_type delta_vco_temp = 0;

/*--------------------------------------------------------------------------*/

  /* Don't update the table if the temperature is out of range or invalid */
  if ( !VALID_TEMP( temp32 ) )
  {
    DBG_1( MED, "Can't update Temperature Table. "
          "Out of range temperature: %d", temp32 );
    return;
  }

  /* Can't update VCO_TEMP if we're too close to last update */
  if ( time < ( tcxomgr.vco.last_update_secs + TCXOMGR_UPDATE_TEMP_TBL_SEC ) )
  {
    return;
  }

  /* Get old VCTCXO accumulator value */
  old_vco_temp_info = tcxomgr_get_temp_table( temp32 );

  /* convert thermistor reading to index */
  temp = (uint8)TEMP_TO_INX(temp32);

  old_vco_temp = old_vco_temp_info.rot_value;

  if ( old_vco_temp_info.vco_type != TCXOMGR_VCO_DEFAULT )
  {
    /* Filter the rot and store it in the temp table */
    new_vco_temp = ROT_TEMP_FILT(old_vco_temp, vco_acc) + old_vco_temp;

    DBG_3( HIGH, "VCO_TEMP[%d]: %d --> %d", temp, old_vco_temp, new_vco_temp );
  }
  else /* old vco temp type is vco default */
  {
    /* New entry into the FT table, store it as is */
    new_vco_temp = vco_acc;

    DBG_2( HIGH, "VCO_TEMP[%d]: --> %d", temp, new_vco_temp );
  }
  TCXOMGR_NOTUSED(delta_vco_temp);

  /* Update RAM version of the temp table */
  tcxomgr.vco.temp[ temp ] = new_vco_temp;

  /* Update time we last updated the VCO_TEMP table */
  tcxomgr.vco.last_update_secs = time;

} /* tcxomgr_update_temp_table */


/*============================================================================

FUNCTION TCXOMGR_FT_DATA_IS_VALID

DESCRIPTION
    This function returns of if the passed table has valid contents or not.

DEPENDENCIES
    None

RETURN VALUE
    Boolean

SIDE EFFECTS
    None

============================================================================*/
boolean tcxomgr_ft_data_is_valid
(
  tcxomgr_ft_table_type table
)
{
  tcxomgr_cal_field_data_type *cal_data;
  tcxomgr_cal_field_data_type *new_cal_data;
  boolean                     result = TRUE;

/*--------------------------------------------------------------------------*/

  cal_data = &(tcxomgr.fieldcal_data[table].cal_data);

  /* Get the other table pointer to compare with the passed table */
  if(table == TCXOMGR_TABLE_WWAN)
  {
    new_cal_data = &(tcxomgr.fieldcal_data[TCXOMGR_TABLE_GPS].cal_data);
  }
  else
  {
    new_cal_data = &(tcxomgr.fieldcal_data[TCXOMGR_TABLE_WWAN].cal_data);
  }

  /* There are three possible conditions:
     1. passed table is field calibrated then return TRUE
     2. passed table is not field calibrated but the other table is
        then return FALSE
     3. both the tables are not field calibrated
        then return TRUE
  */
  if((cal_data->ft_qual_ind > TCXOMGR_CAL_FT_QUAL_FT1) ||
     (cal_data->state == TCXOMGR_FIELD0) ||
     (cal_data->state == TCXOMGR_FIELD3))
  {
    result = TRUE;
  }
  else if((new_cal_data->ft_qual_ind > TCXOMGR_CAL_FT_QUAL_FT1) ||
     (new_cal_data->state == TCXOMGR_FIELD0) ||
     (new_cal_data->state == TCXOMGR_FIELD3))
  {
    result = FALSE;
  }

  return result;
}

/*============================================================================

FUNCTION TCXOMGR_RESET_RGS_AND_TEMP_TABLE

DESCRIPTION
  This function blows away all Recent Good System data and the entire trk_lo
  vs. temperature table.  It should be called very rarely and very carefully.
  If some RAT (GSM, WCDMA, ...) feels TCXO Manager data is bad, corrupt,
  invalid, etc., and should no longer be used, it calls this function to dump
  all the data.

  Effects of this function take place immediately in RAM cached copies.  New
  data doesn't go to NV until power down.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_reset_rgs_and_temp_table
(
  /* Client resetting RGS and temperature table. */
  tcxomgr_client_id_type  client_id
)
{
  /* Loop index */
  uint16                  index;

  /* Current time (in seconds) since 6 JAN 1980 00:00:00 */
  uint32                  current_time;

  /* Current temperature set */
  tcxomgr_temp_set_type   current_temp;

  /* System RGS */
  tcxomgr_rgs_struct_type system_rgs;

/*--------------------------------------------------------------------------*/

  /* Get current time and temperature. */
  tcxomgr_get_time_and_temp(&current_time, &current_temp);

  /* Get the system RGS */
  tcxomgr_get_system_rgs(&system_rgs);

  /* Pring debug about client doing reset and current time and temperature. */
  DBG_3( ERROR, "reset_rgs() by client:%d curr_time:%d curr_temp: %d",
            client_id, current_time, current_temp.xo );

  /* Print debug about RGS value, before blowing it away. */
  DBG_3( ERROR, "reset_rgs() old RGS system:%d vco:%d rot:%d",
        system_rgs.id, system_rgs.vco, system_rgs.rot );

  /* Dumping the entire trk_lo vs. temperature table. */
  for ( index = 0; index < VCO_TEMP_TABLE_SIZE; index++ )
  {
    /* Only log the RGS we're resetting if it isn't empty */
    if(tcxomgr.vco.temp[ index ] != VCO_UNDEFINED)
    {
      DBG_2( ERROR, "reset_rgs() Index: [%d] Value: [%d] ",
                     index, tcxomgr.vco.temp[ index ]);
    }
  }

  if ( !VALID_TEMP( TCXOMGR_TEMP_TO_RGS_TEMP(current_temp) ) )
  {
    /* Write an entry to error log in NV. */
    ERR( "reset_rgs() by client:%d old RGS system:%d current_temp vco:%d",
         client_id, system_rgs.vco,
         tcxomgr.vco.temp[ TEMP_TO_INX( TCXOMGR_TEMP_TO_RGS_TEMP(current_temp))] );
  }

  /* Reset all of our RGS data. */
  tcxomgr_reset_all_rgs_values(FALSE);

  /* Next reset the entire trk_lo vs. temperature table. */
  for ( index = 0; index < VCO_TEMP_TABLE_SIZE; index++ )
  {
    tcxomgr.vco.temp[ index ] = VCO_UNDEFINED;
    tcxomgr.vco.rotCount[ index ]    = 0;
    tcxomgr.vco.offset[ index ]      = VCO_UNDEFINED;
    tcxomgr.vco.offsetCount[ index ] = 0;

    tcxomgr.gps_vco.temp[ index ]        = VCO_UNDEFINED;
    tcxomgr.gps_vco.rotCount[ index ]    = 0;
    tcxomgr.gps_vco.offset[ index ]      = VCO_UNDEFINED;
    tcxomgr.gps_vco.offsetCount[ index ] = 0;
  }


  tcxomgr.vco.last_update_secs= time_get_secs();

  tcxomgr.gps_vco.last_update_secs = time_get_secs();

} /* tcxomgr_reset_rgs_and_temp_table */


/*============================================================================

FUNCTION TCXOMGR_GET_VCO_TEMP

DESCRIPTION
  This function reads the trk_lo vs. temperature table and returns the trk_lo
  value corresponding to the current temperature of the phone.

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_vco_indicator_type - Describes trk_lo type, DEFAULT, EXACT, etc.
  tcxomgr_vco_type *vco      - Return parameter with actual VCO value.

SIDE EFFECTS
  None

============================================================================*/

tcxomgr_vco_indicator_type tcxomgr_get_vco_temp
(
  /* VCTCXO Accumulator value for the current temperature */
  tcxomgr_vco_type *vco
)
{
  tcxomgr_vco_info_type  vco_info;
  tcxomgr_temp_set_type  temp_set;

  /* Time (in seconds) since 6 JAN 1980 00:00:00 */
  uint32                 current_time;


/*--------------------------------------------------------------------------*/

  /* Get the current temperature */
  /* Todo: this needs to change to a blocking read based on WCDMA requirements */
  /* worst case for cached read is 110ms old stale value. */
  /* note that time is redundant here. Will have to use new API once */
  /* ADC DAL is ready */
  tcxomgr_get_time_and_temp(&current_time, &temp_set);

  /* returns temp tbl entry, or default if no entry exists */
  vco_info = tcxomgr_get_temp_table( TCXOMGR_TEMP_TO_RGS_TEMP(temp_set) );

  *vco = vco_info.vco_value;

  return vco_info.vco_type;

} /* tcxomgr_get_vco_temp( ) */

/*============================================================================

FUNCTION TCXOMGR_GET_DEFAULT_VCO

DESCRIPTION
  This function returns the default value of the trk_lo PDM.

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_vco_type - Default trk_lo value.

SIDE EFFECTS
  None

============================================================================*/

tcxomgr_vco_type tcxomgr_get_default_vco(void)
{
  return tcxomgr.vco.dflt;
}

/*===========================================================================
               EFS read / write functions
===========================================================================*/
/*===========================================================================

FUNCTION TCXOMGR_EFS_VERSION_READ

DESCRIPTION    This function reads version number from the EFS file

DEPENDENCIES   None.

RETURN VALUE   Version number if efs file exists, otherwise 0.

SIDE EFFECTS   None.

===========================================================================*/
uint32 tcxomgr_efs_version_read
(
  tcxomgr_efs_type *efs,
  uint32 num_bytes
)
{
  uint32 val=0;

  if ( efs == NULL )
  {
    DBG_0(ERROR, " Null efs pointer passed.");
    return 0;
  }

  /* Read from the file */
  if (efs_get(efs->fname, efs->buffer, num_bytes) < 0)
  {
    DBG_1(ERROR, "Error reading EFS file %d", val);
  }
  else
  {
    val = *((uint32*)(efs->buffer));
  }

  return val;
}

/*===========================================================================

FUNCTION TCXOMGR_EFS_VERSION_WRITE

DESCRIPTION    This function writes version number to the EFS file. If the
 file does not exist, then it is created first and then the version number
 is updated in the file.

DEPENDENCIES   None.

RETURN VALUE   0 if the write is successful, negative otherwise.

SIDE EFFECTS   None.

===========================================================================*/
int32 tcxomgr_efs_version_write
(
  tcxomgr_efs_type *efs,
  uint32 version
)
{
  int32 val=0;

  if ( efs == NULL )
  {
    DBG_0(ERROR, " Null efs pointer passed.");
    return -1;
  }

  /* Update the version number in the efs file */
  *((uint32*)(efs->buffer)) = version;

  /* Write to the file. Create it if it doesnt exist */
  val = efs_put(efs->fname, (void *)efs->buffer, sizeof(version),
                O_CREAT|O_AUTODIR, ALLPERMS);
  if (val < 0)
  {
    DBG_1(ERROR, "Error writing to EFS file %d", val);
  }

  return val;
}

/*============================================================================

FUNCTION TCXOMGR_EFS_READ_XTAL_TYPE

DESCRIPTION
  This function reads the NV to determine the XO crystal type

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
tcxomgr_cal_xtal_enum_type tcxomgr_efs_read_xtal_type ( void )
{
  tcxomgr_efs_type efs           = { TCXOMGR_XO_CRYSTAL_TYPE_FILE };
  uint32        xtal_type  = (uint32) TCXOMGR_XTAL_DEFAULT;

  if ( efs_get(efs.fname, (void *)efs.buffer, sizeof(xtal_type)) < 0 )
  {
    /* EFS file is not present, use default crystal type*/
    DBG_0(ERROR, "XO crystal type not set in NV..");
  }
  else
  {
    /* Read succeeded, read 32 bits and check for valid value */
    xtal_type = *((uint32*)(efs.buffer));

    if ( xtal_type >= TCXOMGR_XTAL_MAX )
    {
      DBG_0( ERROR,"XO crystal type not configured, using crystal type" );
      xtal_type  = TCXOMGR_XTAL_DEFAULT;
    }
  }

  return (tcxomgr_cal_xtal_enum_type)xtal_type;
}

/*============================================================================

FUNCTION TCXOMGR_EFS_WRITE_XTAL_TYPE

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  Result: True if successfull, False otherwise

SIDE EFFECTS
  None

============================================================================*/

int32 tcxomgr_efs_write_xtal_type
(
  uint32 xtal_type
)
{
  int32 val=0;
  tcxomgr_efs_type efs           = { TCXOMGR_XO_CRYSTAL_TYPE_FILE };

 /* Update the xtal type in the efs file */
  *((uint32*)(efs.buffer)) = xtal_type;

  /* Write to the file. Create it if it doesnt exist */
  val = efs_put(efs.fname, (void *)efs.buffer, sizeof(xtal_type),
                O_CREAT|O_AUTODIR, ALLPERMS);
  if (val < 0)
  {
    DBG_1(ERROR, "Error writing to EFS file %d", val);
  }

  return val;
}


/*============================================================================

FUNCTION TCXOMGR_FIELD_CAL_AGING_CHANGE

DESCRIPTION
  Calculates the difference in the aging correction from the aging correction
  saved in flash, for all tables (WWAN and GPS).

DEPENDENCIES
  None

RETURN VALUE
  The maximum difference aging correction, in units of ppm*1024

SIDE EFFECTS
  None

============================================================================*/
static tcxomgr_ppm_type tcxomgr_aging_change(void)
{
  int                   loop;
  tcxomgr_ppm_type      aging_change = 0;
  tcxomgr_ppm_type      difference;

  for(loop = 0; loop < TCXOMGR_TABLE_MAX; loop++)
  {
    difference = ABS(tcxomgr.fieldcal_data[loop].aging_data.c0_fresh_nv -
        tcxomgr.fieldcal_data[loop].aging_data.c0_fresh);
    if(difference > aging_change)
    {
      aging_change = difference;
    }
  }

  return aging_change;
}


/*============================================================================

FUNCTION TCXOMGR_FLUSH_DATA_TO_NV

DESCRIPTION
  This function flushes TCXOMGR data to NV

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_flush_data_to_nv ( void )
{
  tcxomgr_ppm_type      aging_change;

  /* Note: For now we will only flush RGS and aging to NV to avoid overwriting
   * any data that has been written to NV manually by the user. */

  /* Write the RGS values */
  tcxomgr_efs_rgs_data_write();

  /* Check whether the aging data has changed by the minimum threshold before
   * writing it to flash, to minimize the number of writes to flash. */
  aging_change = tcxomgr_aging_change();

  if(aging_change > TCXOMGR_MIN_FT_CALIBRATION_CHANGE)
  {
    tcxomgr_efs_field_aging_data_write();
  }
}

/*============================================================================

FUNCTION TCXOMGR_SYNC_EFS_DATA

DESCRIPTION
  This function can be called after EFS writes to make sure that all data has
  been synced to the RFS.

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_sync_efs_data (void)
{
  fs_sync_token_t sync_token;

  if ( efs_sync_no_wait("/", &sync_token) < 0 )
  {
    DBG_0( ERROR, "tcxomgr_sync_efs_data: Failed to sync EFS data");
  }
}

/*============================================================================

FUNCTION TCXOMGR_CAL_CHECK_TEMP_SPAN

DESCRIPTION
  This function checks if the test tree is using the min required temp span

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

============================================================================*/
boolean tcxomgr_cal_check_temp_span(tcxomgr_temp_type temp)
{

/*--------------------------------------------------------------------------*/

  /* If the test tree is using a temp span less than 0.2C then return FALSE
     It is recommended that the test trees use a temp span of 1C or
     Make use of the factory fine calibration feature */

  if(temp < CAL_TEMP_SPAN_RELAXED)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

/*============================================================================

FUNCTION TCXOMGR_SCHEDULE_FIELD_TRIM

DESCRIPTION
  This function sends a command to the Tcxomgr task to adjust the XO trim
  value prior to field calibration, if the device does not have a valid XO
  factory calibration.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_schedule_field_trim(int32 delta_c0)
{
  tcxomgr_cmd_type *cmd_buffer;

  cmd_buffer = tcxomgr_get_cmd_buf();

  if(cmd_buffer != NULL)
  {
    cmd_buffer->cmd_id = TCXOMGR_CMD_FIELD_TRIM;
    cmd_buffer->cmd.field_trim_data.delta_c0 = delta_c0;
    tcxomgr_send_cmd(cmd_buffer);
  }
}

/*============================================================================

FUNCTION TCXOMGR_ADJUST_FIELD_TRIM

DESCRIPTION
  This function adjusts the XO trim value prior to field calibration, if the
  device does not have a valid XO factory calibration.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  May adjust the XO trim value in NV

============================================================================*/
void tcxomgr_adjust_field_trim(int32 delta_c0)
{
  int                   i;
  uint8                 new_trim;
  int32                 current_offset, min_offset;
  boolean               field_cal_state_changed = FALSE;
  tcxomgr_efs_type      field_cal_version_efs = { FIELD_CAL_EFS_PATH };

  if(ABS(delta_c0) <= 2048)
  {
    DBG_1(LOW, "tcxomgr_field_trim: New/delta C0 is %d, within expected range",
        delta_c0);
  }
  else
  {
    /* Estimate a new trim value based on the delta C0 value and the current
     * trim value, taking into account the non-linearity of the impact of trim
     * on XO frequency. */

    ASSERT(tcxomgr.xo_trim.curr_val < 64);

    new_trim = tcxomgr.xo_trim.curr_val;
    current_offset = tcxomgr_trim_estimate[tcxomgr.xo_trim.curr_val];
    min_offset = 0x7fffffff;

    for(i = 0; i < 64; i++)
    {
      int32 offset = tcxomgr_trim_estimate[i] - current_offset + delta_c0;
      offset = ABS(offset);
      if(offset < min_offset)
      {
        min_offset = offset;
        new_trim = i;
      }
    }

    ASSERT(new_trim < 64);

    DBG_3(HIGH,
        "tcxomgr_field_trim: New/delta C0 is %d, trim is %d, est new trim %d",
        delta_c0, tcxomgr.xo_trim.curr_val, new_trim);

    /* Check whether the new trim value is different from the trim value last
     * written to NV */

    if(tcxomgr.cal_data.field_data.xo_trim == new_trim)
    {
      /* New trim is already written to NV. No need to do anything here. At the
       * next reboot Tcxomgr will pick up the new trim. */
    }
    else
    {
      DBG_1(MED, "tcxomgr_field_trim: Writing new trim %d to NV", new_trim);

      /* Write the new trim to NV */
      rex_enter_crit_sect(&tcxomgr.nv->crit_sect);
      tcxomgr.nv->item.xo_trim_values.current = new_trim;
      tcxomgr.nv->item.xo_trim_values.table   = new_trim;

      tcxomgr_nv_cmd(NV_WRITE_F, NV_XO_TRIM_VALUES_I);
      rex_leave_crit_sect(&tcxomgr.nv->crit_sect);

      /* Write the new factory calibration to NV */
      tcxomgr.cal_data.fac_data.xo_trim = new_trim;
      tcxomgr.cal_data.fac_data.t0p = tcxomgr.cached_xo_temp.xo;

      tcxomgr.cal_data.field_data.xo_trim = new_trim;

      tcxomgr_nv_cal_data_write();

      /* Reset field calibration so Tcxomgr will discard field calibration,
       * RGS, and any FT table data at the next reboot, when the new XO trim
       * takes effect */
      tcxomgr_efs_version_write(&field_cal_version_efs, 0);
    }
  }

  /* Update field calibration states to coarse calibration so field
   * calibration will behave normally */

  for(i = TCXOMGR_TABLE_WWAN; i < TCXOMGR_TABLE_MAX; i++)
  {
    if(tcxomgr.fieldcal_data[i].cal_data.ft_qual_ind <
        TCXOMGR_CAL_FT_QUAL_COARSE)
    {
      tcxomgr.fieldcal_data[i].cal_data.ft_qual_ind =
        TCXOMGR_CAL_FT_QUAL_COARSE;
      tcxomgr.fieldcal_data[i].cal_data.c0 = delta_c0 * 4;
      field_cal_state_changed = TRUE;
    }
  }

  if(field_cal_state_changed)
  {
    /* Try to start up IFC, if we now have a good-enough field trim */
    tcxomgr_ifc_init();
  }

  /* If field C0 is within coarse-calibration range, and field cal state is
   * changed, write field cal data to EFS */
  if((ABS(delta_c0) <= 2048) && field_cal_state_changed)
  {
    DBG_1(LOW, "tcxomgr_field_trim: Writing field calibration "
        "(state=coarse c0=%d)",
        tcxomgr.fieldcal_data[0].cal_data.c0);
    tcxomgr_efs_field_cal_params_write(TRUE);
  }
}
