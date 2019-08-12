/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

DESCRIPTION
  This module executes AT commands. It executes IS-707 specific HDR related
  AT commands.

EXTERNALIZED FUNCTIONS
dsat707_exec_qchdrc_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCHDRC command.
  For 1xEV select a fixed DCR value. The default value is F, for
  variable rate mode.

dsat707_exec_qchdrr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCHDRR command.
  For 1xEV, select a fixed reverse rate. Default value is F,
  for variable rate mode.

dsat707_exec_qchdrt_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCHDRT command.
  For 1xEV, toggle early termination. Default value is 1.
  
Copyright (c) 2002 - 2011,2014 by Qualcomm Technologies Incorporated.
 All Rights Reserved.
 Qualcomm Confidential and Proprietary.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsat707hdr.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
		      
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/11/14   tk      Optimized debug macros usage in ATCoP.
11/18/11   vpk     HDR SU API cleanup
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
12/12/08   ua      Fixed Off target lint errors.
10/30/06   spn     Changed Featurization from DS_AT_TEST_ONLY to 
                   HDR_AT_TEST_ONLY
10/04/05   kss     Added support for qchdret command for HDR Rev A RL.
09/07/05   spn     Changed the dsat707_exec_qchdrr_cmd() corresponding to 
                   changes in hdrl1_rmac_set_fixed_rri(), which now returns
                   a boolean indicating if the command was executed properly 
                   or not.
08/31/05   spn     Changed the call to hdrsrch_at_set_drc_value() to
                   hdrl1_fmac_set_drc_value()
04/03/02   rsl     Initial version of file.

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_HDR
#ifdef FEATURE_HDR_AT_TEST_ONLY
#include "dsati.h"
#include "dsatparm.h"
#include "msg.h"
#include "dsat707hdr.h"
#include "dsat707hdrctab.h"
#include "hdrcom_api.h"


/*===========================================================================
  FUNCTION DSAT707_EXEC_QCHDRC_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCHDRC command.
  For 1xEV select a fixed DCR value. The default value is F, for
  variable rate mode.
  0 - Null Rate
  1 - 38.4 kbps
  2 - 76.8 kbps
  3 - 153.6 kbps
  4 - 307.2 kbps (short)
  5 - 307.2 kbps (long)
  6 - 614.4 kbps (short)
  7 - 614.4 kbps (long)
  8 - 921.6 kbps
  9 - 1288.8 kbps (short)
  A - 1288.8 kbps (long)
  B - 1843.2 kbps
  C - 2457.6 kbps
  D - 1536.0 kbps
  E - 3072.0 kbps
  F - Variable Rate Mode
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qchdrc_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsat_num_item_type atoi_result;
  dsat_num_item_type temp_val;
/*-------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  if ( result != DSAT_OK )
  {
    return result;
  }

  /* Convert the received ASCII string to integer */
  temp_val =(dsat_num_item_type)dsatutil_get_val(
                                  DSAT707_HDR_QCHDRC_IDX,0,0,NUM_TYPE);
if ( dsatutil_atoi ( &atoi_result,
                       dsat707_hdrc_valstr[temp_val],
                       16 ) == 0)
  {
    if ( hdrl1_fmac_set_drc_value( (uint8)atoi_result ) == TRUE )
    {
      result = DSAT_OK;
    }
    else
    {
      result = DSAT_ERROR;
    }  
  }
  else
  {
    result = DSAT_ERROR;
  }
  rb_ptr->used = (uint16)strlen((char *)rb_ptr->data_ptr);
  return result;
} /* dsat707_exec_qchdrc_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCHDRR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCHDRR command.
  For 1xEV, select a fixed reverse rate. Default value is F,
  for variable rate mode.
  1 - 9.6 kbps
  2 - 19.2 kbps
  3 - 38.4 kbps
  4 - 76.8 kbps (short)
  5 - 153.6 kbps (long)
  F - Variable Rate Mode

  For Do-Rev-A, select a fixed Maximum reverse packet size.
  1 - 96
  2 - 224 
  3 - 480 
  4 - 736 
  5 - 992 
  6 - 1504 
  7 - 2016 
  8 - 3040 
  9 - 4064 
  A - 6112 
  B - 8162 
  C - 12256 
  F - Variable Rate Mode 
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qchdrr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsat_num_item_type atoi_result;
  dsat_num_item_type temp_val;
/*-------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);

  if ( result != DSAT_OK )
  {
    return result;
  }

  /* Convert the received ASCII string to integer */
  temp_val = (dsat_num_item_type)dsatutil_get_val(
                                  DSAT707_HDR_QCHDRR_IDX,0,0,NUM_TYPE);
  if ( dsatutil_atoi (&atoi_result,
                      dsat707_hdrr_valstr[temp_val],
                      16 ) == 0)
  {
    if ( hdrl1_rmac_set_fixed_rri( (uint8)atoi_result ) == TRUE )
    {
      result = DSAT_OK;
    }
    else
    {
      result = DSAT_ERROR;
    }  
  }
  else
  {
    result = DSAT_ERROR;
  }
  rb_ptr->used = (uint16)strlen((char *)rb_ptr->data_ptr);
  return result;
} /* dsat707_exec_qchdrr_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCHDRT_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCHDRT command.
  For 1xEV, toggle early termination. Default value is 1.
  0 - Early termination disabled.
  1 - Early termination enabled.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qchdrt_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsat_num_item_type temp_val;
/*-------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  temp_val = (dsat_num_item_type)dsatutil_get_val(
                                  DSAT707_HDR_QCHDRT_IDX,0,0,NUM_TYPE);
  if (temp_val == 0)
  {
    hdrl1_fmac_set_ack_mode(IS856_NO_ACK);
  }
  else /* can't be anything other than 1 */
  {
    hdrl1_fmac_set_ack_mode(IS856_COMPLIANT);
  }
  rb_ptr->used = (uint16)strlen((char *)rb_ptr->data_ptr);
  return result;
} /* dsat707_exec_qchdrt_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCHDRET_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCHDRET command, which configures
  the early termination test mode for the Rev A Reverse Link.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qchdret_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsat_num_item_type temp_val;
/*-------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  temp_val = (dsat_num_item_type)dsatutil_get_val(
                                 DSAT707_HDR_QCHDRET_IDX,0,0,NUM_TYPE);
  if (hdrl1_rmac_config_early_termination( temp_val) == TRUE)
  {
    result = DSAT_OK;
  }
  else
  {
    result = DSAT_ERROR;
  }

  rb_ptr->used = (uint16)strlen((char *)rb_ptr->data_ptr);
  return result;
} /* dsat707_exec_qchdret_cmd */

#endif /* FEATURE_HDR_AT_TEST_ONLY */

#endif /* FEATURE_HDR        */
#endif /* FEATURE_DATA_IS707 */
