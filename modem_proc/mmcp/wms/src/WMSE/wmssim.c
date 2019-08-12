/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           wmssim.c -- SIM Interface Module

  The WMS module which implements the User API for SMS. This source file
  defines the SIM Interface functions.

  -------------

    Copyright (c) 1998-2014 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/WMSE/wmssim.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "mmcp_variation.h"
#include "comdef.h"
#include "wmssim.h"
#include "wmscfg.h"
#include "wmsbc.h"
#include "msg.h"
#include "stringl.h"

#ifdef FEATURE_GWSMS
#include "string.h"
#include "wmsi.h"
#include "wmsmsgi.h"
#endif /* FEATURE_GWSMS */

#include "wmsutils.h"
#include "mmgsdisessionlib.h"
#include "mmgsdisessionlib_v.h" // Modem private header file for cache reads
#include "wmsts.h"
#include "wmsnv.h"
#include "wmstscdma.h"

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/*===========================================================================
======================== STATIC DATA ========================================
===========================================================================*/

// Define a variable to hold the WMS task TCB.  This is done for
// a several reasons -- see definition of wms_myself in wms.c
static rex_tcb_type *wms_myself = NULL;  // To be initialized to rex_self()
                                         // when task is running

static mmgsdi_data_type         wms_mmgsdi_read_cnf;
static mmgsdi_return_enum_type  wms_mmgsdi_write_cnf;

/*===========================================================================
=================== FORWARD DECLARATIONS ====================================
===========================================================================*/
extern void nvruim_init_wms_svc_items(boolean has_bcsms, boolean has_smscap);
void wms_sim_cache_clean_up(wms_prov_app_info_s_type * appInfo_ptr);

#if defined(FEATURE_QSC1215_SGLTE_FUSION) || defined(FEATURE_8960_SGLTE_FUSION)
void wms_sim_mmgsdi_file_update_event_proc(
  mmgsdi_event_data_type   *event,
  wms_prov_app_info_s_type *appInfo_ptr
);
#endif /* defined(FEATURE_QSC1215_SGLTE_FUSION) || defined(FEATURE_8960_SGLTE_FUSION) */

/*===========================================================================
========================  FUNCTIONS  ========================================
===========================================================================*/
void wms_check_status_mmgsdi_session_read_rec(mmgsdi_return_enum_type ret)
{
   if (MMGSDI_SUCCESS != ret)
   {
     MSG_ERROR_1("mmgsdi_session_read_record failed: %d", ret);
   }
}

#ifdef FEATURE_GWSMS

#ifdef FEATURE_GWSMS_BROADCAST

/*
*/
boolean wms_sim_write_cbmi(
  wms_prov_app_info_s_type *appInfo_ptr,
  uint8                    *data
)
{
  mmgsdi_return_enum_type  ret;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    return FALSE;
  }

  ret = wms_sim_mmgsdi_write_data( appInfo_ptr,
                                   appInfo_ptr->cbmi_file_enum,
                                   0, //not used
                                   data,
                                   appInfo_ptr->bc.gw.sim_cbmi_rec_len );
  if (MMGSDI_SUCCESS != ret)
  {
     MSG_ERROR_1("wms_sim_write_cbmi failed: %d", ret);
     return FALSE;
  }

  return TRUE;
} /* wms_sim_write_cbmi() */


/*
*/
boolean wms_sim_write_cbmir(
  wms_prov_app_info_s_type *appInfo_ptr,
  uint8                    *data
)
{
  mmgsdi_return_enum_type  ret;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    return FALSE;
  }

  ret = wms_sim_mmgsdi_write_data( appInfo_ptr,
                                   appInfo_ptr->cbmir_file_enum,
                                   0, //not used
                                   data,
                                   appInfo_ptr->bc.gw.sim_cbmir_rec_len );
  if (MMGSDI_SUCCESS != ret)
  {
     MSG_ERROR_1("wms_sim_write_cbmir failed: %d", ret);
     return FALSE;
  }

  return TRUE;
} /* wms_sim_write_cbmir() */

STATIC byte wms_sim_tolower(
  byte c
)
{
  if ( ('A' <= c) && ('Z' >= c) )
  {
    c -= (byte)('A' - 'a');
  }

  return c;
}

/*=========================================================================
FUNCTION wms_sim_xlate_iso639_to_lang_filter()

DESCRIPTION
  Translate an ISO639 language code to a 16-bit language code.

DEPENDENCIES
  None

RETURN VALUE
  16-bit language code -- if unknown, then WMS_GW_CB_LANGUAGE_UNSPECIFIED.

SIDE EFFECTS
  None
=========================================================================*/
uint16 wms_sim_xlate_iso639_to_lang_filter(
  byte iso_lang_msb,
  byte iso_lang_lsb
)
{
  uint16 lang_filter = (uint16)WMS_GW_CB_LANGUAGE_DUMMY;

  if ( (0xff == iso_lang_msb) && (0xff == iso_lang_lsb) )
  {
    MSG_HIGH_0("Empty lang");
  }
  else
  {
    iso_lang_msb = wms_sim_tolower(iso_lang_msb);
    iso_lang_lsb = wms_sim_tolower(iso_lang_lsb);

    lang_filter = ((((uint16)iso_lang_msb) << 8) | ((uint16)iso_lang_lsb));
  }
  return lang_filter;
} /* wms_sim_xlate_iso639_to_lang_filter */


/*=========================================================================
FUNCTION wms_sim_xlate_dcs_to_lang_filter()

DESCRIPTION
  Translate dcs to 16-bit language code.

DEPENDENCIES
  None

RETURN VALUE
  16-bit language code

SIDE EFFECTS
  None
=========================================================================*/
uint16 wms_sim_xlate_dcs_to_lang_filter(
  wms_gw_cb_language_e_type dcs
)
{
  uint16 lang_filter;

  switch (dcs)
  {
  case WMS_GW_CB_LANGUAGE_ARABIC:
    lang_filter =  wms_sim_xlate_iso639_to_lang_filter( 'a', 'r' );
    break;
  case WMS_GW_CB_LANGUAGE_CZECH:
    lang_filter =  wms_sim_xlate_iso639_to_lang_filter( 'c', 's' );
    break;
  case WMS_GW_CB_LANGUAGE_DANISH:
    lang_filter =  wms_sim_xlate_iso639_to_lang_filter( 'd', 'a' );
    break;
  case WMS_GW_CB_LANGUAGE_GERMAN:
    lang_filter =  wms_sim_xlate_iso639_to_lang_filter( 'd', 'e' );
    break;
  case WMS_GW_CB_LANGUAGE_GREEK:
    lang_filter =  wms_sim_xlate_iso639_to_lang_filter( 'e', 'l' );
    break;
  case WMS_GW_CB_LANGUAGE_ENGLISH:
    lang_filter =  wms_sim_xlate_iso639_to_lang_filter( 'e', 'n' );
    break;
  case WMS_GW_CB_LANGUAGE_SPANISH:
    lang_filter =  wms_sim_xlate_iso639_to_lang_filter( 'e', 's' );
    break;
  case WMS_GW_CB_LANGUAGE_FRENCH:
    lang_filter =  wms_sim_xlate_iso639_to_lang_filter( 'f', 'r' );
    break;
  case WMS_GW_CB_LANGUAGE_FINNISH:
    lang_filter =  wms_sim_xlate_iso639_to_lang_filter( 'f', 'i' );
    break;
  case WMS_GW_CB_LANGUAGE_HEBREW:
    lang_filter =  wms_sim_xlate_iso639_to_lang_filter( 'h', 'e' );
    break;
  case WMS_GW_CB_LANGUAGE_HUNGARIAN:
    lang_filter =  wms_sim_xlate_iso639_to_lang_filter( 'h', 'u' );
    break;
  case WMS_GW_CB_LANGUAGE_ITALIAN:
    lang_filter =  wms_sim_xlate_iso639_to_lang_filter( 'i', 't' );
    break;
  case WMS_GW_CB_LANGUAGE_ICELANDIC:
    lang_filter =  wms_sim_xlate_iso639_to_lang_filter( 'i', 's' );
    break;
  case WMS_GW_CB_LANGUAGE_DUTCH:
    lang_filter =  wms_sim_xlate_iso639_to_lang_filter( 'n', 'l' );
    break;
  case WMS_GW_CB_LANGUAGE_NORWEGIAN:
    lang_filter =  wms_sim_xlate_iso639_to_lang_filter( 'n', 'o' );
    break;
  case WMS_GW_CB_LANGUAGE_POLISH:
    lang_filter =  wms_sim_xlate_iso639_to_lang_filter( 'p', 'l' );
    break;
  case WMS_GW_CB_LANGUAGE_PORTUGUESE:
    lang_filter =  wms_sim_xlate_iso639_to_lang_filter( 'p', 't' );
    break;
  case WMS_GW_CB_LANGUAGE_RUSSIAN:
    lang_filter =  wms_sim_xlate_iso639_to_lang_filter( 'r', 'u' );
    break;
  case WMS_GW_CB_LANGUAGE_SWEDISH:
    lang_filter =  wms_sim_xlate_iso639_to_lang_filter( 's', 'v' );
    break;
  case WMS_GW_CB_LANGUAGE_TURKISH:
    lang_filter =  wms_sim_xlate_iso639_to_lang_filter( 't', 'r' );
    break;
  default:
    lang_filter = (uint16)WMS_GW_CB_LANGUAGE_DUMMY;
    break;
  }

  return lang_filter;
} /* wms_sim_xlate_dcs_to_lang_filter */

/*=========================================================================
FUNCTION wms_sim_convert_card_data_to_lang_filter_array()

DESCRIPTION
  Decode raw card data from language files into a caller-provided
  16-bit language code array.

PARAMETERS
  file_ef            [IN]     The file id associated with the data
  read_data          [IN]     The raw card data (buf_ptr, buf_len)
  lang_list_ptr      [IN/OUT] Pointer to caller-allocated memory to hold list
  lang_list_size     [IN]     Length of list (number of language elements)
  lang_list_count    [IN/OUT] Optional pointer to memory to store language count

DEPENDENCIES
  None

RETURN VALUE
  WMS_CMD_ERR_BAD_PARM    If file_ef is not one of the three language files
  WMS_CMD_ERR_NULL_PTR    If any required pointers are NULL or sizes are 0
  WMS_CMD_ERR_NONE        Successful completion

SIDE EFFECTS
  None
=========================================================================*/
wms_cmd_err_e_type wms_sim_convert_card_data_to_lang_filter_array(
  mmgsdi_file_enum_type      file_ef,
  mmgsdi_data_type           read_data,
  uint16                    *lang_list_ptr,
  int                        lang_list_size,
  int                       *lang_list_count
)
{
  wms_cmd_err_e_type   ret_value = WMS_CMD_ERR_NONE;
  int                  lang_list_idx = 0;
  uint8               *data_ptr = NULL;
  int                  lang_data_count = 0;  // number of language entries read from card
  int                  lang_count = 0;       // number of languages inserted into the list

  if ( (NULL == lang_list_ptr) ||
       (0 == lang_list_size) ||
       (NULL == read_data.data_ptr) ||
       (0 == read_data.data_len) )
  {
    return WMS_CMD_ERR_NULL_PTR;
  }

  if ( (MMGSDI_ELP != file_ef) &&
       (MMGSDI_GSM_LP != file_ef) &&
       (MMGSDI_USIM_LI != file_ef) )
  {
    return WMS_CMD_ERR_BAD_PARM;
  }

  // Init/empty lang array
  for (lang_list_idx = 0; lang_list_idx < lang_list_size; lang_list_idx++)
  {
    lang_list_ptr[lang_list_idx] = (uint16)WMS_GW_CB_LANGUAGE_DUMMY;
  }

  data_ptr = read_data.data_ptr;        // points to data read from card
  lang_data_count = read_data.data_len; // length of card data

  // Calculate the number of language entries in the card data based
  // on the length of the data and EF type. MMGSDI_GSM_LP is already
  // in DCS format (one byte per lang), while MMGSDI_ELP and MMGSDI_USIM_LI
  // are in ISO 639 format (two bytes per lang).
  if (MMGSDI_GSM_LP != file_ef)
  {
    lang_data_count /= 2;  // Divide bytes by 2 to get lang count
  }

  // The language list will be filled with either the number of entries
  // read from the card (lang_data_count) or the number of elements given
  // to fill in the list (lang_list_size), whichever is lower
  lang_data_count = MIN(lang_data_count, lang_list_size);

  // Fill the list
  for (lang_list_idx = 0; lang_list_idx < lang_data_count; lang_list_idx++)
  {
    if (MMGSDI_GSM_LP == file_ef) // File is EFlp (SIM app)
    {
      if (0xFF != *data_ptr)
      {
        // Convert from DCS to 16-bit language code
        lang_list_ptr[lang_count++] = wms_sim_xlate_dcs_to_lang_filter((wms_gw_cb_language_e_type)*data_ptr);
      }
      data_ptr++;
    }
    else // File is EFli (USIM app) or EFelp (global file, SIM/USIM)
    {
      if ( (0xFF != *data_ptr) || (0xFF != *(data_ptr + 1)) )
      {
        // Convert from ISO 639 to 16-bit language code
        lang_list_ptr[lang_count++] = wms_sim_xlate_iso639_to_lang_filter( *(data_ptr),
                                                                           *(data_ptr + 1) );
      }
      data_ptr += 2;
    }
  }

  // Log languages read
  for (lang_list_idx = 0; lang_list_idx < lang_count; lang_list_idx++)
  {
    MSG_HIGH_2( "lang_list[%d]=%d",
                lang_list_idx,
                lang_list_ptr[lang_list_idx] );
  }

  // Return language count if requested
  if (NULL != lang_list_count)
  {
    *lang_list_count = lang_count;
  }

  return ret_value;
}

/*===========================================================================
FUNCTION wms_sim_mmgsdi_read_lang_files()

DESCRIPTION
  Read the language file from the MMGSDI cache and return the normalized
  list of languages supported in broadcast messages.

  The file to be read are initialized at wms task startup.
  See wms_sim_init_lang_files().

  Files read:
   MMGSDI_GSM_LP  (App-specific, SIM)
   MMGSDI_USIM_LI (App-specific, USIM)
   MMGSDI_ELP     (Global, SIM/USIM)

PARAMETERS
  lang_list_ptr       [IN/OUT] Caller-allocated memory to hold list
  lang_list_size      [IN]     Length of list (number of elements)
  num_languages_read  [IN/OUT] Optional, pointer to memory to store the number
                               of language entries returned.

RETURN VALUE
  WMS_CMD_ERR_NULL_PTR     Could not get active subscription information,
                           lang_list_ptr==NULL, or lang_list_size==0
  WMS_CMD_ERR_UNSUPPORTED  No language file (app-specific or global) could be read
  WMS_CMD_ERR_BC_SIM_READ  The read of the cached data from MMGSDI failed

DEPENDENCIES
  MMGSDI initialization is complete and session is open
===========================================================================*/
wms_cmd_err_e_type wms_sim_mmgsdi_read_lang_files(
  wms_prov_app_info_s_type  *appInfo_ptr,
  uint16                    *lang_list_ptr,
  int                        lang_list_size,
  int                       *num_languages_read
)
{
  wms_cmd_err_e_type        ret_val = WMS_CMD_ERR_NONE;
  int                       list_count = 0;

  if ( (NULL == appInfo_ptr) ||
       (NULL == lang_list_ptr) ||
       (0 == lang_list_size) )
  {
    MSG_ERROR_3( "Invalid appInfo_ptr (0x%x), lang_list_ptr (0x%x), or lang_list_size (%d)",
                 appInfo_ptr,
                 lang_list_ptr,
                 lang_list_size );

    return WMS_CMD_ERR_NULL_PTR;
  }

  if (MMGSDI_NO_FILE_ENUM == appInfo_ptr->lang_file_enum)
  {
    MSG_ERROR_0("No file to read: MMGSDI_NO_FILE_ENUM");
    return WMS_CMD_ERR_UNSUPPORTED;
  }

  {
    mmgsdi_return_enum_type   mmgsdi_ret;
    mmgsdi_data_type          read_data = {0, NULL};
    uint8                     data_buffer[WMS_GW_CB_MAX_LANGUAGES];

    // At this point:
    //    appInfo_ptr->lang_file_enum  contains the fileEF to read
    //    appInfo_ptr->lang_file_len   contains the length of the file
    // These were initialized when the mmgsdi session was opened.

    // Set up the buffer for the returned EF data
    read_data.data_len = appInfo_ptr->lang_file_len;
    read_data.data_ptr = data_buffer;

    // Fetch the EF data from the mmgsdi cache
    mmgsdi_ret = mmgsdi_session_read_cache( appInfo_ptr->mmgsdi_appInfo.session_id,
                                            appInfo_ptr->lang_file_enum,
                                            read_data );

    if (MMGSDI_SUCCESS != mmgsdi_ret)
    {
      MSG_HIGH_2( "MMGSDI failure reading fileEF 0x%04X, return=%d\n",
                  appInfo_ptr->lang_file_enum,
                  mmgsdi_ret );
      ret_val = WMS_CMD_ERR_BC_SIM_READ;
    }

    if (WMS_CMD_ERR_NONE == ret_val)
    {
      // Data was read successfully -- convert to the 16-bit language code
      ret_val = wms_sim_convert_card_data_to_lang_filter_array( appInfo_ptr->lang_file_enum,
                                                                read_data,
                                                                lang_list_ptr,
                                                                lang_list_size,
                                                                &list_count );
    }
  }

  if (WMS_CMD_ERR_NONE == ret_val)
  {
    // Return optional language count if requested
    if (NULL != num_languages_read)
    {
      *num_languages_read = list_count;
    }
  }

  return ret_val;
} /* wms_sim_mmgsdi_read_lang_files() */

/*===========================================================================
FUNCTION wms_sim_init_lang_files()

DESCRIPTION
  Determine which language file to use and obtain the file size
  from the MMGSDI cache.

  The initial files to be read are initialized according to application type.
  The non-global (app-specific) files are tried first. If the non-global file
  cannot be read or is zero-length, then the global language file will be
  used.  Once the global file is used, no further attempts to read the
  non-global file will be done until a refresh event.

  Files read:
   MMGSDI_GSM_LP  (App-specific, SIM)
   MMGSDI_USIM_LI (App-specific, USIM)
   MMGSDI_ELP     (Global, SIM/USIM)

RETURN VALUE
  WMS_CMD_ERR_NULL_PTR     Could not get active subscription information
  WMS_CMD_ERR_UNSUPPORTED  Neither language file (app-specific or global) could be read
  WMS_CMD_ERR_BAD_PARM     Application type is not SIM or USIM

DEPENDENCIES
  None

SIDE EFFECTS
  appInfo_ptr->lang_file_enum is modified based on results
===========================================================================*/
wms_cmd_err_e_type wms_sim_init_lang_files(
  wms_prov_app_info_s_type *appInfo_ptr
)
{
  wms_cmd_err_e_type        ret_val = WMS_CMD_ERR_NONE;
  mmgsdi_return_enum_type   mmgsdi_ret;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    return WMS_CMD_ERR_NULL_PTR;
  }

  if (MMGSDI_APP_SIM == appInfo_ptr->mmgsdi_appInfo.app_type)
  {
    appInfo_ptr->lang_file_enum = MMGSDI_GSM_LP;
  }
  else if (MMGSDI_APP_USIM == appInfo_ptr->mmgsdi_appInfo.app_type)
  {
    appInfo_ptr->lang_file_enum = MMGSDI_USIM_LI;
  }
  else
  {
    appInfo_ptr->lang_file_enum = MMGSDI_NO_FILE_ENUM;
    ret_val = WMS_CMD_ERR_BAD_PARM;
  }

  // Try reading non-global language file
  if ( (MMGSDI_GSM_LP == appInfo_ptr->lang_file_enum) ||
       (MMGSDI_USIM_LI == appInfo_ptr->lang_file_enum) )
  {
    mmgsdi_ret = mmgsdi_session_read_cache_file_size( appInfo_ptr->mmgsdi_appInfo.session_id,
                                                      appInfo_ptr->lang_file_enum,
                                                      &appInfo_ptr->lang_file_len );

    // Failure or empty file, try global file MMGSDI_ELP
    if ( (MMGSDI_SUCCESS != mmgsdi_ret) || (0 == appInfo_ptr->lang_file_len) )
    {
      // If the non-global file couldn't be read or is empty, try to use
      // the global language file.  Reading from the non-global file will
      // not be attempted again.
      appInfo_ptr->lang_file_enum = MMGSDI_ELP;
      MSG_HIGH_1( "Couldn't read (mmgsdi_result=%d) or 0-length app lang pref file, trying global file", mmgsdi_ret);
    }
  }

  // Try reading the global language file if necessary
  if (MMGSDI_ELP == appInfo_ptr->lang_file_enum)
  {
    mmgsdi_ret = mmgsdi_session_read_cache_file_size( appInfo_ptr->mmgsdi_appInfo.session_id,
                                                      appInfo_ptr->lang_file_enum,
                                                      &appInfo_ptr->lang_file_len );

    // Failure to read or empty file.  No more files to try
    if ( (MMGSDI_SUCCESS != mmgsdi_ret) || (0 == appInfo_ptr->lang_file_len) )
    {
      appInfo_ptr->lang_file_enum = MMGSDI_NO_FILE_ENUM;
      ret_val = WMS_CMD_ERR_UNSUPPORTED;
      MSG_HIGH_1( "Couldn't read (mmgsdi_result=%d) or 0-length global lang pref file, CB langs not supported", mmgsdi_ret);
    }
  }

  if (WMS_CMD_ERR_NONE == ret_val)
  {
    if (WMS_GW_CB_MAX_LANGUAGES < appInfo_ptr->lang_file_len)
    {
      // See data_buffer definition in wms_sim_mmgsdi_read_lang_files()
      MSG_ERROR_0("WARNING: language file bigger than static buffer");
      appInfo_ptr->lang_file_len = WMS_GW_CB_MAX_LANGUAGES;
    }
  }

  return ret_val;
} /* wms_sim_init_lang_files() */

#endif /* FEATURE_GWSMS_BROADCAST */
#endif /* FEATURE_GWSMS */


void wms_sim_mmgsdi_init(
  void
)
{
  wms_cfg_s_type          *lcfg_s_ptr    = wms_cfg_s_ptr();
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_SUCCESS;

  wms_myself = rex_self(); // Will never change after this

  /* Initialize mmgsdi to default values */
  lcfg_s_ptr->wms_sim_mmgsdi_init_state = WMS_SIM_MMGSDI_NOT_INIT;

  mmgsdi_status = mmgsdi_client_id_and_evt_reg( wms_sim_mmgsdi_event_cb,
                                                wms_sim_mmgsdi_client_id_reg_cb,
                                                (mmgsdi_client_data_type)(int)SYS_MODEM_AS_ID_1);

  if (MMGSDI_SUCCESS != mmgsdi_status)
  {
    MSG_ERROR_1("mmgsdi_client_id_reg failed %d", mmgsdi_status);
  }

  /* Set MMGSDI Client Init State to In Progress */
  lcfg_s_ptr->wms_sim_mmgsdi_init_state = WMS_SIM_MMGSDI_INIT_IN_PROGRESS;

  return;
} /* wms_sim_mmgsdi_init() */

void wms_sim_mmgsdi_client_id_reg_cb(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr
)
{
  wms_cfg_s_type          *lcfg_s_ptr    = wms_cfg_s_ptr();

  MSG_HIGH_0("wms_sim_mmgsdi_client_id_reg_cb()");

  if ( (MMGSDI_SUCCESS != status) ||
       (MMGSDI_CLIENT_ID_AND_EVT_REG_CNF != cnf) ||
       (NULL == cnf_ptr) )
  {
     MSG_ERROR_3( "bad cb: cnf %d, status = %d, cnf_ptr = 0x%x",
                  cnf,
                  status,
                  cnf_ptr );
     return;
  }

  lcfg_s_ptr->wms_sim_mmgsdi_init_state = WMS_SIM_MMGSDI_INIT_COMPLETE;
  lcfg_s_ptr->wms_sim_mmgsdi_client     = cnf_ptr->client_id_and_evt_reg_cnf.response_header.client_id;

  {
    mmgsdi_return_enum_type  mmgsdi_res;
    uint32                   session_mask = MMGSDI_GW_PROV_PRI_SESSION_MASK |
                                            MMGSDI_GW_PROV_SEC_SESSION_MASK |
                                            MMGSDI_1X_PROV_PRI_SESSION_MASK |
                                            MMGSDI_GW_PROV_TER_SESSION_MASK;

    // Open the subscription sessions (all of them, always)
    mmgsdi_res = mmgsdi_session_open_ext( lcfg_s_ptr->wms_sim_mmgsdi_client,
                                          session_mask,
                                          wms_sim_mmgsdi_event_cb,
                                          0, /* set notify mask */
                                          wms_sim_mmgsdi_session_open_cb,
                                          0  /* client ref */ );

    if (MMGSDI_SUCCESS != mmgsdi_res)
    {
      MSG_ERROR_1("Failed to open mmgsdi sessions: %d", mmgsdi_res);
    }
  }

  return;
} /* wms_sim_mmgsdi_client_id_reg_cb() */

/*=========================================================================
FUNCTION wms_sim_do_subs_ready()                                 (INTERNAL)

DESCRIPTION
  Sends the CDMA or GW ready events if both 'sim_init_complete' and
  'is_available' are TRUE for a subscription.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void wms_sim_do_subs_ready(
  wms_prov_app_info_s_type    *appInfo_ptr
)
{
  if (appInfo_ptr->sim_init_complete && appInfo_ptr->is_available)
  {
    wms_cfg_do_ready_event(appInfo_ptr);
  }
  else
  {
    // Log the attempt and indicate what is going on
    MSG_HIGH_3( "wms_sim_do_subs_ready: appInfo_ptr 0x%x, sim_init_complete %d, is_available %d",
                appInfo_ptr,
                appInfo_ptr->sim_init_complete,
                appInfo_ptr->is_available );
  }

  return;
} /* wms_sim_do_subs_ready() */

/*===========================================================================
FUNCTION wms_sim_sim_busy_proc()

DESCRIPTION
  Function that processes the SIM_BUSY event coming from MMGSDI.

RETURN VALUE
  None .

DEPENDENCIES
  None.
===========================================================================*/
void wms_sim_sim_busy_proc(
  wms_prov_app_info_s_type *appInfo_ptr,
  boolean                   sim_is_busy
)
{
  if (NULL != appInfo_ptr)
  {
    if (sim_is_busy)
    {
      if (!appInfo_ptr->mmgsdi_is_busy)
      {
        appInfo_ptr->mmgsdi_is_busy = TRUE;

        if (TRUE == appInfo_ptr->sim_command_is_queued)
        {
          (void)rex_set_sigs( wms_myself, WMS_SIM_BUSY_SIG );
        }
        wms_cfg_report_not_ready_event(appInfo_ptr);
      }
    }
    else
    {
      if (!appInfo_ptr->sim_command_is_queued && appInfo_ptr->mmgsdi_is_busy)
      {
        appInfo_ptr->mmgsdi_is_busy = FALSE;
        (void)rex_clr_sigs( wms_myself, WMS_SIM_BUSY_SIG );

        wms_sim_do_subs_ready(appInfo_ptr);
      }
    }
  }
  else
  {
    MSG_ERROR_0("appInfo_ptr is NULL");
  }
} /* wms_sim_sim_busy_proc() */


wms_prov_app_info_s_type * wms_sim_alloc_appInfo_ptr(
  wms_prov_mmgsdi_app_info_s_type *wms_mmsgdi_appInfo
)
{
  wms_prov_app_info_s_type *appInfo_ptr = NULL;
  if (NULL != wms_mmsgdi_appInfo)
  {
    appInfo_ptr = (wms_prov_app_info_s_type *)wms_mem_malloc( sizeof( wms_prov_app_info_s_type ) );
    if (NULL != appInfo_ptr)
    {
      appInfo_ptr->mmgsdi_appInfo     = *wms_mmsgdi_appInfo;
      appInfo_ptr->primary_client     = (uint8)WMS_CLIENT_TYPE_MAX;
      appInfo_ptr->smss_mem_full_flag = (uint8)WMS_SIM_MEM_FULL_CLEAR;
      appInfo_ptr->asid               = SYS_MODEM_AS_ID_NONE;
      appInfo_ptr->voice_mail_index   = (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX;
      appInfo_ptr->voice_mail_active  = FALSE;
    }

     MSG_HIGH_3( "Allocated appInfo_ptr for session_id: 0x%x, session_type: %d - 0x%x", 
                 wms_mmsgdi_appInfo->session_id,
                 wms_mmsgdi_appInfo->session_type,
                 appInfo_ptr );
  }
  return appInfo_ptr;
}

void wms_sim_mmgsdi_event_cb(
  const mmgsdi_event_data_type *event
)
{
  wms_cfg_s_type           *lcfg_s_ptr       = wms_cfg_s_ptr();
  boolean                   proc_in_wms_task = FALSE;
  wms_prov_app_info_s_type *appInfo_ptr      = NULL;

  if (NULL == event) {
    return;
  }

  MSG_HIGH_1("In wms_sim_mmgsdi_event_cb(), event = %d", event->evt);

  // Only session events have valid session id
  if (event->evt >= MMGSDI_SUBSCRIPTION_READY_EVT)
  {
    // Don't use the get_appinfo_by_session_id() here so that any sessions
    // that have not yet been marked as active can be handled.
    if (event->session_id == lcfg_s_ptr->wms_mmgsdi_app0_info.session_id)
    {
      if (NULL == lcfg_s_ptr->wms_app0_info)
      {
        lcfg_s_ptr->wms_app0_info = wms_sim_alloc_appInfo_ptr(&lcfg_s_ptr->wms_mmgsdi_app0_info);
      }
      appInfo_ptr = lcfg_s_ptr->wms_app0_info;
    }
    else if (event->session_id == lcfg_s_ptr->wms_mmgsdi_app1_info.session_id)
    {
      if (NULL == lcfg_s_ptr->wms_app1_info)
      {
        lcfg_s_ptr->wms_app1_info = wms_sim_alloc_appInfo_ptr(&lcfg_s_ptr->wms_mmgsdi_app1_info);
      }
      appInfo_ptr = lcfg_s_ptr->wms_app1_info;
    }
    else if (event->session_id == lcfg_s_ptr->wms_mmgsdi_app2_info.session_id)
    {
      if (NULL == lcfg_s_ptr->wms_app2_info)
      {
        lcfg_s_ptr->wms_app2_info = wms_sim_alloc_appInfo_ptr(&lcfg_s_ptr->wms_mmgsdi_app2_info);
      }
      appInfo_ptr = lcfg_s_ptr->wms_app2_info;
    }
    else if (event->session_id == lcfg_s_ptr->wms_mmgsdi_app3_info.session_id)
    {
      if (NULL == lcfg_s_ptr->wms_app3_info)
      {
        lcfg_s_ptr->wms_app3_info = wms_sim_alloc_appInfo_ptr(&lcfg_s_ptr->wms_mmgsdi_app3_info);
      }
      appInfo_ptr = lcfg_s_ptr->wms_app3_info;
    }

    if (NULL == appInfo_ptr)
    {
      MSG_ERROR_1("Received MMGSDI event for non-existent session id 0x%x", event->session_id);
      return;
    }
  }

  switch (event->evt)
  {
  case MMGSDI_CARD_ERROR_EVT:
  case MMGSDI_SUBSCRIPTION_READY_EVT:
  case MMGSDI_SESSION_CHANGED_EVT:
  case MMGSDI_REFRESH_EVT:
  case MMGSDI_FDN_EVT:
  case MMGSDI_CARD_INSERTED_EVT:
#if defined(FEATURE_QSC1215_SGLTE_FUSION) || defined(FEATURE_8960_SGLTE_FUSION)
  case MMGSDI_FILE_UPDATE_EVT:
#endif /* defined(FEATURE_QSC1215_SGLTE_FUSION) || defined(FEATURE_8960_SGLTE_FUSION) */
    proc_in_wms_task = TRUE;
    break;

  case MMGSDI_SIM_BUSY_EVT:
    {
      mmgsdi_slot_id_enum_type  slot            = MMGSDI_MAX_SLOT_ID_ENUM;

      MSG_HIGH_2( "Received MMGSDI_BUSY_EVT with is_busy %d on slot %d",
                  event->data.sim_busy.sim_busy,
                  event->data.sim_busy.slot );

      slot = event->data.sim_busy.slot;

      switch (slot)
      {
      case MMGSDI_SLOT_1:
        wms_sim_sim_busy_proc(lcfg_s_ptr->wms_app0_info, event->data.sim_busy.sim_busy);
        wms_sim_sim_busy_proc(lcfg_s_ptr->wms_app1_info, event->data.sim_busy.sim_busy);
        break;

      case MMGSDI_SLOT_2:
        wms_sim_sim_busy_proc(lcfg_s_ptr->wms_app2_info, event->data.sim_busy.sim_busy);
        break;

      case MMGSDI_SLOT_3:
        wms_sim_sim_busy_proc(lcfg_s_ptr->wms_app3_info, event->data.sim_busy.sim_busy);
        break;

      default:
        MSG_ERROR_0("Invalid slot, couldn't process SIM_BUSY event");
        break;
      }
    }
    break;

  default:
    break;
  }

  if (proc_in_wms_task)
  {
    wms_cmd_type  *cmd_ptr;

    cmd_ptr = wms_get_cmd_buf();

    if (NULL != cmd_ptr)
    {
      cmd_ptr->hdr.cmd_id      = WMS_CMD_MMGSDI_EVENT_CB;
      cmd_ptr->hdr.client_id   = (wms_client_id_type)WMS_CLIENT_TYPE_INTERNAL;
      cmd_ptr->hdr.cmd_cb      = NULL;
      cmd_ptr->hdr.user_data   = NULL;
      cmd_ptr->hdr.appInfo_ptr = appInfo_ptr;

      (void)memscpy( &(cmd_ptr->cmd.mmgsdi_event_cb.event),
                     sizeof(mmgsdi_event_data_type),
                     event,
                     sizeof(mmgsdi_event_data_type));

      wms_put_cmd(cmd_ptr);
    }
    else
    {
      MSG_ERROR_0("Out of cmd buf for WMS_CMD_MMGSDI_EVENT_CB");
    }
  }

  return;
}

boolean asid_is_valid(
  sys_modem_as_id_e_type asid
)
{
  if ( (SYS_MODEM_AS_ID_NONE < asid) &&
       ((int)asid < MAX_AS_IDS) )
  {
    return TRUE;
  }

  MSG_ERROR_1("Invalid AS_ID = %d",asid);
  return FALSE;
} /* asid_is_valid() */

wms_prov_app_info_s_type *wms_get_lte_appinfo(
  void
)
{
  wms_cfg_s_type  *lcfg_s_ptr = wms_cfg_s_ptr();

  if (NULL != lcfg_s_ptr->wms_app0_info &&
      lcfg_s_ptr->wms_app0_info->lte_is_available)
  {
    return lcfg_s_ptr->wms_app0_info;
  }
  else if (NULL != lcfg_s_ptr->wms_app1_info &&
           lcfg_s_ptr->wms_app1_info->lte_is_available )
  {
    return lcfg_s_ptr->wms_app1_info;
  }
  else if (NULL != lcfg_s_ptr->wms_app2_info &&
           lcfg_s_ptr->wms_app2_info->lte_is_available )
  {
    return lcfg_s_ptr->wms_app2_info;
  }
  else if (NULL != lcfg_s_ptr->wms_app3_info &&
           lcfg_s_ptr->wms_app3_info->lte_is_available )
  {
    return lcfg_s_ptr->wms_app3_info;
  }

  return NULL;
} /* wms_get_lte_appinfo */


boolean wms_check_srv_avail_by_asid_and_message_mode( 
   sys_modem_as_id_e_type   asid,
   wms_message_mode_e_type  message_mode,
   sys_sys_mode_e_type      sys_mode 
)
{
  wms_prov_app_info_s_type  *appInfo_ptr;
  int                        indx;

  appInfo_ptr = wms_get_appinfo_by_asid_and_message_mode( asid, message_mode );

  if ( NULL == appInfo_ptr ) 
  {
    return FALSE;
  }

  appInfo_ptr->number_of_stacks = MIN(appInfo_ptr->number_of_stacks, CM_NO_STACKS);

  for ( indx = 0; indx < appInfo_ptr->number_of_stacks; indx++ ) 
  {
    MSG_HIGH_5("appInfo_ptr->stack_info[%d], as_id = %d, operational = %d, srv_status = %d, sys_mode = %d",
                indx,
                appInfo_ptr->asid,
                appInfo_ptr->stack_info[indx].is_stack_operational,
                appInfo_ptr->stack_info[indx].srv_status,
                appInfo_ptr->stack_info[indx].sys_mode );

    if ( (appInfo_ptr->stack_info[indx].is_stack_operational) &&
         (SYS_SRV_STATUS_SRV == appInfo_ptr->stack_info[indx].srv_status) && 
         (sys_mode == appInfo_ptr->stack_info[indx].sys_mode) )
    {
      MSG_HIGH_1( "Found sys_mode = %d srv available", sys_mode );
      return TRUE;
    }
  }

  return FALSE;
}


wms_prov_mmgsdi_app_info_s_type * wms_get_mmgsdi_appInfo_by_session_type(
  mmgsdi_session_type_enum_type session_type
)
{
  wms_cfg_s_type                  *lcfg_s_ptr = wms_cfg_s_ptr();
  wms_prov_mmgsdi_app_info_s_type *info_ptr   = NULL;

  // Previous versions of this function had an extra check before
  // returning the pointer:
  //          lcfg_s_ptr->wms_app2_info.session_activated
  // This has been eliminated for now.

  switch (session_type)
  {
  case MMGSDI_1X_PROV_PRI_SESSION:
    info_ptr = &lcfg_s_ptr->wms_mmgsdi_app0_info;
    break;

  case MMGSDI_GW_PROV_PRI_SESSION:
    info_ptr = &lcfg_s_ptr->wms_mmgsdi_app1_info;
    break;

  case MMGSDI_GW_PROV_SEC_SESSION:
    info_ptr = &lcfg_s_ptr->wms_mmgsdi_app2_info;
    break;

  /* For TSTS: TER=Tertiary */
  case MMGSDI_GW_PROV_TER_SESSION:
    info_ptr = &lcfg_s_ptr->wms_mmgsdi_app3_info;
    break;

  default:
    MSG_ERROR_1("Unmapped mmgsdi session type: %d", session_type);
  } // switch

  return info_ptr;
} /* wms_get_appinfo_by_session_type() */


wms_prov_app_info_s_type * wms_get_appinfo_by_session_type(
  mmgsdi_session_type_enum_type session_type
)
{
  wms_cfg_s_type           *lcfg_s_ptr = wms_cfg_s_ptr();
  wms_prov_app_info_s_type *info_ptr   = NULL;

  // Previous versions of this function had an extra check before
  // returning the pointer:
  //          lcfg_s_ptr->wms_app2_info.session_activated
  // This has been eliminated for now.

  switch (session_type)
  {
  case MMGSDI_1X_PROV_PRI_SESSION:
    info_ptr = lcfg_s_ptr->wms_app0_info;
    break;

  case MMGSDI_GW_PROV_PRI_SESSION:
    info_ptr = lcfg_s_ptr->wms_app1_info;
    break;

  case MMGSDI_GW_PROV_SEC_SESSION:
    info_ptr = lcfg_s_ptr->wms_app2_info;
    break;

  /* For TSTS: TER=Tertiary */
  case MMGSDI_GW_PROV_TER_SESSION:
    info_ptr = lcfg_s_ptr->wms_app3_info;
    break;

  default:
    MSG_ERROR_1("Unmapped mmgsdi session type: %d", session_type);
  } // switch

  return info_ptr;
} /* wms_get_appinfo_by_session_type() */

wms_prov_app_info_s_type * wms_get_appinfo_by_session_id(
  mmgsdi_session_id_type session_id
)
{
  wms_cfg_s_type           *lcfg_s_ptr = wms_cfg_s_ptr();
  wms_prov_app_info_s_type *info_ptr   = NULL;

  if (NULL != lcfg_s_ptr->wms_app0_info &&
      session_id == lcfg_s_ptr->wms_app0_info->mmgsdi_appInfo.session_id)
  {
    info_ptr = lcfg_s_ptr->wms_app0_info;
  }

  if (NULL != lcfg_s_ptr->wms_app1_info &&
      session_id == lcfg_s_ptr->wms_app1_info->mmgsdi_appInfo.session_id)
  {
    info_ptr = lcfg_s_ptr->wms_app1_info;
  }

  if (NULL != lcfg_s_ptr->wms_app2_info &&
      session_id == lcfg_s_ptr->wms_app2_info->mmgsdi_appInfo.session_id)
  {
    info_ptr = lcfg_s_ptr->wms_app2_info;
  }

  if (NULL != lcfg_s_ptr->wms_app3_info &&
      session_id == lcfg_s_ptr->wms_app3_info->mmgsdi_appInfo.session_id)
  {
    info_ptr = lcfg_s_ptr->wms_app3_info;
  }

  if (NULL == info_ptr)
  {
    MSG_ERROR_1("Unmapped mmgsdi session id: %d", session_id);
  }

  return info_ptr;
} /* wms_get_appinfo_by_session_id() */

wms_prov_app_info_s_type *wms_get_appinfo_by_asid_and_message_mode(
  sys_modem_as_id_e_type   asid,
  wms_message_mode_e_type  message_mode
)
{
  wms_cfg_s_type           *lcfg_s_ptr = wms_cfg_s_ptr();
  wms_prov_app_info_s_type *info_ptr   = NULL;

  do
  {
    if (WMS_MESSAGE_MODE_CDMA == message_mode)
    {
      // For CDMA, only AS_ID_1 is used
      if (SYS_MODEM_AS_ID_1 == asid)
      {
        info_ptr = lcfg_s_ptr->wms_app0_info;  // CDMA
      }

      break;
    } // message_mode CDMA

    { // Message mode: GW

      if (SYS_MODEM_AS_ID_NONE == asid)
      {
        break;
      }

      // Check if GW_PRI is mapped to this asid
      if (NULL != lcfg_s_ptr->wms_app1_info &&
          asid == lcfg_s_ptr->wms_app1_info->asid)
      {
        info_ptr = lcfg_s_ptr->wms_app1_info;
        break;
      }

      // Check if GW_SEC is mapped to this asid
      if (NULL != lcfg_s_ptr->wms_app2_info &&
          asid == lcfg_s_ptr->wms_app2_info->asid)
      {
        info_ptr = lcfg_s_ptr->wms_app2_info;
        break;
      }

      // Check if GW_TER is mapped to this asid
      if (NULL != lcfg_s_ptr->wms_app3_info &&
          asid == lcfg_s_ptr->wms_app3_info->asid)
      {
        info_ptr = lcfg_s_ptr->wms_app3_info;
      }
    } // message_mode GW
  } while(0);

  if (NULL == info_ptr)
  {
    MSG_ERROR_2( "Could not map asid %d and message_mode %d to an appinfo",
                 asid,
                 message_mode );
  }

  return info_ptr;
} /* wms_get_appinfo_by_asid_and_message_mode() */

wms_prov_app_info_s_type * wms_get_appinfo_by_asid_and_mem_store(
  sys_modem_as_id_e_type    asid,
  wms_memory_store_e_type   mem_store
)
{
  wms_cfg_s_type           *lcfg_s_ptr = wms_cfg_s_ptr();
  wms_prov_app_info_s_type *info_ptr   = NULL;

  switch (mem_store)
  {
  case WMS_MEMORY_STORE_RAM_GW:
  case WMS_MEMORY_STORE_SIM:
  case WMS_MEMORY_STORE_NV_GW:
  case WMS_MEMORY_STORE_USIM:
    {
      do
      { // Mem stores for GW

        if (SYS_MODEM_AS_ID_NONE == asid)
        {
          break;
        }

        // Check if GW_PRI is mapped to this asid
        if (NULL != lcfg_s_ptr->wms_app1_info &&
            asid == lcfg_s_ptr->wms_app1_info->asid)
        {
          info_ptr = lcfg_s_ptr->wms_app1_info;
          break;
        }

        // Check if GW_SEC is mapped to this asid
        if (NULL != lcfg_s_ptr->wms_app2_info &&
            asid == lcfg_s_ptr->wms_app2_info->asid)
        {
          info_ptr = lcfg_s_ptr->wms_app2_info;
          break;
        }

        // Check if GW_TER is mapped to this asid
        if (NULL != lcfg_s_ptr->wms_app3_info &&
            asid == lcfg_s_ptr->wms_app3_info->asid)
        {
          info_ptr = lcfg_s_ptr->wms_app3_info;
        }
      } while(0);
    }
    break;

  case WMS_MEMORY_STORE_RAM_CDMA:
  case WMS_MEMORY_STORE_RUIM:
  case WMS_MEMORY_STORE_NV_CDMA:
  case WMS_MEMORY_STORE_CSIM:
    { // Mem stores for CDMA

      // For CDMA, only AS_ID_1 is used
      if (SYS_MODEM_AS_ID_1 == asid)
      {
        info_ptr = lcfg_s_ptr->wms_app0_info;
      }
    }
    break;

  default:
    break;
  } // switch

  if (NULL == info_ptr)
  {
    MSG_ERROR_2( "Could not map asid %d and mem_store %d to an appinfo",
                 asid,
                 mem_store );
  }

  return info_ptr;
} /* wms_get_appinfo_from_asid_and_mem_store() */

boolean wms_check_appinfo_is_valid(
  wms_prov_app_info_s_type  *appInfo_ptr
)
{
  if (NULL != appInfo_ptr)
  {
      return TRUE;
  }

  MSG_ERROR_0("NULL appInfo_ptr");
  return FALSE;
} /* wms_check_appinfo_is_valid() */

wms_prov_app_info_s_type * wms_get_appinfo_1x(
  void
)
{
  wms_cfg_s_type            *lcfg_s_ptr = wms_cfg_s_ptr();
  wms_prov_app_info_s_type  *appInfo_ptr;

  // 1X session types are:
  //   MMGSDI_1X_PROV_PRI_SESSION
  appInfo_ptr = wms_get_appinfo_by_session_type(MMGSDI_1X_PROV_PRI_SESSION);

  if (NULL != appInfo_ptr)
  {
    return appInfo_ptr;
  }

  MSG_ERROR_0("Could not get 1X appInfo, using app0_info");

  return lcfg_s_ptr->wms_app0_info;
} /* wms_get_appinfo_1x() */

sys_modem_as_id_e_type wms_get_appinfo_1x_asid(
  void
)
{
  wms_prov_app_info_s_type  *appInfo_ptr;

  appInfo_ptr = wms_get_appinfo_1x();

  if (NULL != appInfo_ptr)
  {
    return appInfo_ptr->asid;
  }

  MSG_ERROR_0("Could not get 1X appInfo, using AS_ID_1");

  return SYS_MODEM_AS_ID_1;
} /* wms_get_appinfo_1x_asid() */

wms_prov_app_info_s_type * wms_get_gw_appinfo_by_asid(
  sys_modem_as_id_e_type    asid
)
{
  wms_cfg_s_type            *lcfg_s_ptr = wms_cfg_s_ptr();
  wms_prov_app_info_s_type  *appInfo_ptr = NULL;

  switch (asid)
  {
    case SYS_MODEM_AS_ID_1:
      if (NULL == lcfg_s_ptr->wms_app1_info)
      {
        lcfg_s_ptr->wms_app1_info = 
          wms_sim_alloc_appInfo_ptr(&lcfg_s_ptr->wms_mmgsdi_app1_info);
      }
      appInfo_ptr = lcfg_s_ptr->wms_app1_info;
      break;

    case SYS_MODEM_AS_ID_2:
      if (NULL == lcfg_s_ptr->wms_app2_info)
      {
        lcfg_s_ptr->wms_app2_info = 
          wms_sim_alloc_appInfo_ptr(&lcfg_s_ptr->wms_mmgsdi_app2_info);
      }
      appInfo_ptr = lcfg_s_ptr->wms_app2_info;
      break;

    case SYS_MODEM_AS_ID_3:
      if (NULL == lcfg_s_ptr->wms_app3_info)
      {
        lcfg_s_ptr->wms_app3_info = 
          wms_sim_alloc_appInfo_ptr(&lcfg_s_ptr->wms_mmgsdi_app3_info);
      }
      appInfo_ptr = lcfg_s_ptr->wms_app3_info;
      break;

    default:
      break;
  }

  if (NULL == appInfo_ptr)
  {
    MSG_ERROR_1("Could not get appInfo, as id = %d", asid);
  }

  return appInfo_ptr;
} /* wms_get_gw_appinfo_by_asid */

/*===========================================================================
FUNCTION wms_mmgsdi_wait_for_write_complete_cb()

DESCRIPTION
  Function is called by MMGSDI when finishing read to SIM.

RETURN VALUE
  NONE

DEPENDENCIES
  None
===========================================================================*/
void wms_mmgsdi_wait_for_write_complete_cb(
  mmgsdi_return_enum_type    status,
  mmgsdi_cnf_enum_type       cnf,
  const mmgsdi_cnf_type      *cnf_ptr
)
{
  wms_prov_app_info_s_type *appInfo_ptr = NULL;

  MSG_HIGH_2( "wms_mmgsdi_wait_for_write_complete_cb: cnf_ptr - %x status - %d ",
              cnf_ptr,
              status );

  if (NULL == cnf_ptr)
  {
    wms_mmgsdi_write_cnf = MMGSDI_ERROR;
    return;
  }

  appInfo_ptr = (wms_prov_app_info_s_type*)cnf_ptr->response_header.client_data;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    return;
  }

  appInfo_ptr->sim_command_is_queued = FALSE;

  if (appInfo_ptr->mmgsdi_is_busy)
  {
    appInfo_ptr->mmgsdi_is_busy = FALSE;

    wms_sim_do_subs_ready(appInfo_ptr);
    return;
  }

  wms_mmgsdi_write_cnf = (cnf != MMGSDI_WRITE_CNF) ? MMGSDI_ERROR : status;
 
  (void) rex_set_sigs( wms_myself, WMS_MMGSDI_RESPONSE_SIG );

}  /* wms_mmgsdi_wait_for_write_complete_cb() */

/*===========================================================================
FUNCTION wms_sim_mmgsdi_write_data()

DESCRIPTION
  Function that reads a specified Elementary File (EF) from the SIM.

RETURN VALUE
  boolean  Flag indicating whether the specified EF was read from the SIM
           successfully (TRUE) or unsuccessfully (FALSE).

DEPENDENCIES
  None
===========================================================================*/

mmgsdi_return_enum_type wms_sim_mmgsdi_write_data(
   wms_prov_app_info_s_type *appInfo_ptr,
   mmgsdi_file_enum_type     file_enum,
   mmgsdi_rec_num_type       record_number,
   uint8*                    write_data_ptr,
   uint32                    write_data_len
)
{
   mmgsdi_return_enum_type ret;
   mmgsdi_access_type      file_access;
   mmgsdi_write_data_type  write_data;
   wms_status_e_type       status;
   mmgsdi_session_id_type  session_id;

   if (!wms_check_appinfo_is_valid(appInfo_ptr))
   {
     return MMGSDI_ERROR;
   }

   ret                        = MMGSDI_INCORRECT_PARAMS;
   status                     = WMS_OK_S;
   session_id                 = appInfo_ptr->mmgsdi_appInfo.session_id;
   file_access.access_method  = MMGSDI_EF_ENUM_ACCESS;
   file_access.file.file_enum = file_enum;
   write_data.data_len        = (mmgsdi_len_type)write_data_len;
   write_data.data_ptr        = write_data_ptr;

   do
   {
      if (NULL == write_data_ptr || 0 == write_data_len || MMGSDI_NO_FILE_ENUM == file_enum)
      {
         break;
      }

      (void) rex_clr_sigs(wms_myself, (WMS_MMGSDI_RESPONSE_SIG | WMS_SIM_BUSY_SIG));

      switch (file_enum)
      {
      case MMGSDI_TELECOM_SMSS:
      case MMGSDI_USIM_SMSS:
      case MMGSDI_GSM_CBMI:
      case MMGSDI_USIM_CBMI:
      case MMGSDI_GSM_CBMIR:
      case MMGSDI_USIM_CBMIR:
      case MMGSDI_GSM_CBMID:
      case MMGSDI_USIM_CBMID:
      case MMGSDI_CDMA_SMS_STATUS:
      case MMGSDI_CSIM_SMS_STATUS:
         ret = mmgsdi_session_write_transparent( session_id,
                                                 file_access,
                                                 0, //offset
                                                 write_data,
                                                 wms_mmgsdi_wait_for_write_complete_cb,
                                                 (mmgsdi_client_data_type)appInfo_ptr );
         break;

      default:
         ret =  mmgsdi_session_write_record( session_id,
                                             file_access,
                                             MMGSDI_LINEAR_FIXED_FILE,
                                             record_number + 1,
                                             write_data,
                                             wms_mmgsdi_wait_for_write_complete_cb,
                                             (mmgsdi_client_data_type)appInfo_ptr );
         break;

      }

      if (MMGSDI_SUCCESS != ret)
      {
         break;
      }

      appInfo_ptr->sim_command_is_queued = TRUE;

      status = wms_signal_wait(WMS_MMGSDI_RESPONSE_SIG | WMS_SIM_BUSY_SIG);

      /* Wait for MMGSDI response */
      if (WMS_OK_S == status)
      {
        ret = wms_mmgsdi_write_cnf;
      }
      else
      {
        ret = MMGSDI_ERROR;
      }
   } while (0);

   return ret;
} /* wms_sim_mmgsdi_write_data() */

mmgsdi_return_enum_type wms_sim_mmgsdi_delete_data(
   wms_prov_app_info_s_type *appInfo_ptr,
   mmgsdi_file_enum_type     file_enum,
   mmgsdi_rec_num_type       record_number,
   uint32                    record_len
)
{
    uint8  record[255];

    MSG_HIGH_3( "wms_sim_mmgsdi_delete_data: file_enum 0x%x record_number %d appInfo_ptr 0x%x",
                file_enum,
                record_number,
                appInfo_ptr );

    if (NULL == appInfo_ptr)
    {
      return MMGSDI_ERROR;
    }

    if (0 == record_len || record_len > sizeof(record) || MMGSDI_NO_FILE_ENUM == file_enum)
    {
      return MMGSDI_INCORRECT_PARAMS;
    }

    memset(record, 0xFF, record_len);

    if (!(MMGSDI_TELECOM_SMSP == file_enum || MMGSDI_USIM_SMSP == file_enum ||
          MMGSDI_CDMA_SMS_PARAMS == file_enum || MMGSDI_CSIM_SMS_PARAMS == file_enum))

    {
      record[0] = 0;
    }

    return wms_sim_mmgsdi_write_data( appInfo_ptr,
                                      file_enum,
                                      record_number,
                                      record,
                                      record_len );
} /* wms_sim_mmgsdi_delete_data() */

/*===========================================================================
FUNCTION wms_mmgsdi_wait_for_read_complete_cb()

DESCRIPTION
  Function is called by MMGSDI when finishing read to SIM.

RETURN VALUE
  NONE

DEPENDENCIES
  None
===========================================================================*/
STATIC void wms_mmgsdi_wait_for_read_complete_cb(
  mmgsdi_return_enum_type    status,
  mmgsdi_cnf_enum_type       cnf,
  const mmgsdi_cnf_type      *cnf_ptr)
{
  wms_prov_app_info_s_type *appInfo_ptr = NULL;

  MSG_HIGH_2( "wms_mmgsdi_wait_for_read_complete_cb: cnf_ptr - %x status - %d ",
              cnf_ptr,
              status );

  wms_mmgsdi_read_cnf.data_len = 0;
  wms_mmgsdi_read_cnf.data_ptr = NULL;

  if (NULL == cnf_ptr)
  {
    return;
  }

  appInfo_ptr = (wms_prov_app_info_s_type*)cnf_ptr->response_header.client_data;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    return;
  }

  appInfo_ptr->sim_command_is_queued = FALSE;

  if (appInfo_ptr->mmgsdi_is_busy)
  {
    appInfo_ptr->mmgsdi_is_busy = FALSE;

    wms_sim_do_subs_ready(appInfo_ptr);
    return;
  }

  if (MMGSDI_SUCCESS == status)
  {

    MSG_HIGH_2( "wms_mmgsdi_wait_for_read_complete_cb: cnf - %d, data_len - %d ",
                cnf,
                cnf_ptr->read_cnf.read_data.data_len );

    if ((MMGSDI_READ_CNF == cnf) && (cnf_ptr->read_cnf.read_data.data_len != 0))
    {

      /* malloc the pointer for mmgsdi cnf type and copy the info over. */
      wms_mmgsdi_read_cnf.data_len = cnf_ptr->read_cnf.read_data.data_len;
      wms_mmgsdi_read_cnf.data_ptr = (uint8 *)wms_mem_malloc((uint32)wms_mmgsdi_read_cnf.data_len);

      if (NULL != wms_mmgsdi_read_cnf.data_ptr)
      {
         (void)memscpy( wms_mmgsdi_read_cnf.data_ptr,
                        (uint32)wms_mmgsdi_read_cnf.data_len,
                        cnf_ptr->read_cnf.read_data.data_ptr,
                        (uint32)wms_mmgsdi_read_cnf.data_len);
      }
      else
      {
        MSG_ERROR_0("Out of memory in wms_mmgsdi_wait_for_read_complete_cb() ");
      }
    }
  }
  (void) rex_set_sigs( wms_myself, WMS_MMGSDI_RESPONSE_SIG );

} /* wms_mmgsdi_wait_for_read_complete_cb() */

/*===========================================================================
FUNCTION wms_sim_mmgsdi_read_data()

DESCRIPTION
  Function that reads a specified Elementary File (EF) from the SIM.

RETURN VALUE
  boolean  Flag indicating whether the specified EF was read from the SIM
           successfully (TRUE) or unsuccessfully (FALSE).

DEPENDENCIES
  None
===========================================================================*/
mmgsdi_return_enum_type wms_sim_mmgsdi_read_data(
  wms_prov_app_info_s_type *appInfo_ptr,
  mmgsdi_file_enum_type     file_enum,
  mmgsdi_rec_num_type       record_number,
  mmgsdi_len_type           request_length,
  uint8*                    data_ptr,
  uint32                    data_len
)
{
  uint32                  len;
  mmgsdi_return_enum_type ret;
  mmgsdi_access_type      file_access;
  wms_status_e_type       status;
  mmgsdi_session_id_type  session_id;

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    return MMGSDI_ERROR;
  }

  len                        = 0;
  ret                        = MMGSDI_INCORRECT_PARAMS;
  session_id                 = appInfo_ptr->mmgsdi_appInfo.session_id;
  status                     = WMS_OK_S;
  file_access.access_method  = MMGSDI_EF_ENUM_ACCESS;
  file_access.file.file_enum = file_enum;

  MSG_HIGH_5( "wms_sim_mmgsdi_read_data(): appInfo_ptr=0x%x, session_id=0x%x, as_id=%d, file_enum=%d, record=%d",
              appInfo_ptr,
              appInfo_ptr->mmgsdi_appInfo.session_id,
              appInfo_ptr->asid,
              file_enum,
              record_number );

  do
  {
    if (NULL == data_ptr || 0 == data_len || MMGSDI_NO_FILE_ENUM == file_enum)
    {
      break;
    }

    (void) rex_clr_sigs(wms_myself, (WMS_MMGSDI_RESPONSE_SIG | WMS_SIM_BUSY_SIG));

    appInfo_ptr->accessed_rec_num = record_number + 1;

    ret = mmgsdi_session_read_record( session_id,
                                      file_access,
                                      // sim record start with index 1 while wms index start with index 0
                                      record_number + 1,
                                      request_length,
                                      wms_mmgsdi_wait_for_read_complete_cb,
                                      (mmgsdi_client_data_type)appInfo_ptr );

    if (MMGSDI_SUCCESS != ret) {
      break;
    }

    appInfo_ptr->sim_command_is_queued = TRUE;

    /* Wait for MMGSDI response */
    status = wms_signal_wait(WMS_MMGSDI_RESPONSE_SIG | WMS_SIM_BUSY_SIG);

    if (WMS_OK_S != status)
    {
      ret = MMGSDI_ERROR;
      break;
    }

    if ( (0 == wms_mmgsdi_read_cnf.data_len) ||
         (NULL == wms_mmgsdi_read_cnf.data_ptr) )
    {
      MSG_HIGH_0("data_len or data_ptr are null, returning FALSE");
      ret = MMGSDI_ERROR;
      break;
    }

    len = MIN((uint32)wms_mmgsdi_read_cnf.data_len, data_len);

    (void)memscpy( data_ptr,
                   len,
                   wms_mmgsdi_read_cnf.data_ptr,
                   len );

    wms_mem_free(wms_mmgsdi_read_cnf.data_ptr);
    wms_mmgsdi_read_cnf.data_ptr = NULL;
    wms_mmgsdi_read_cnf.data_len = 0;

  } while (0);

  return ret;
} /* wms_sim_mmgsdi_read_data() */

/*=========================================================================
FUNCTION wms_sim_mmgsdi_read_transparent()                       (INTERNAL)

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
mmgsdi_return_enum_type wms_sim_mmgsdi_read_transparent(
  wms_prov_app_info_s_type *appInfo_ptr,
  mmgsdi_file_enum_type     file_enum,
  mmgsdi_len_type           req_len
)
{
  mmgsdi_access_type        file_access;
  mmgsdi_return_enum_type   ret          = MMGSDI_INCORRECT_PARAMS;

  file_access.access_method  = MMGSDI_EF_ENUM_ACCESS;
  file_access.file.file_enum = file_enum;

  MSG_HIGH_4( "wms_sim_mmgsdi_read_transparent(): appInfo_ptr=0x%x, session_id=0x%x, as_id=%d, file_enum=%d",
              appInfo_ptr,
              appInfo_ptr->mmgsdi_appInfo.session_id,
              appInfo_ptr->asid,
              file_enum );

  if (MMGSDI_NO_FILE_ENUM != file_enum)
  {
    ret = mmgsdi_session_read_transparent( appInfo_ptr->mmgsdi_appInfo.session_id,
                                           file_access,
                                           0,
                                           req_len,
                                           wms_sim_mmgsdi_response_cb,
                                           (mmgsdi_client_data_type)appInfo_ptr );
  }

  return ret;
} /* wms_sim_mmgsdi_read_transparent() */

/*=========================================================================
FUNCTION wms_sim_mmgsdi_get_app_capabilities_cb_proc()           (INTERNAL)

DESCRIPTION
  Register for file update notification events

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
STATIC void wms_sim_mmgsdi_get_app_capabilities_cb_proc(
  const mmgsdi_cnf_type  *cnf_ptr
)
{
  wms_prov_app_info_s_type  *appInfo_ptr = NULL;
  mmgsdi_service_enum_type   srv_type;

  MSG_HIGH_0("In wms_sim_mmgsdi_get_app_capabilities_cb_proc()");

  if (NULL == cnf_ptr)
  {
    MSG_ERROR_0("bad cb: NULL cnf_ptr");
    return;
  }

  appInfo_ptr = wms_get_appinfo_by_session_id(cnf_ptr->session_app_capabilities_cnf.response_header.session_id);

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    return;
  }

  appInfo_ptr->fdn_enabled = cnf_ptr->session_app_capabilities_cnf.fdn_enabled;

  /* read next service - mo control */
  switch (appInfo_ptr->mmgsdi_appInfo.app_type)
  {
  case MMGSDI_APP_SIM:
    srv_type  = MMGSDI_GSM_SRV_MOSMS_CONTROL;
    break;
  case MMGSDI_APP_USIM:
    srv_type  = MMGSDI_USIM_SRV_MOSMS_CONTROL;
    break;
  default:
    MSG_ERROR_1("bad app type %d", appInfo_ptr->mmgsdi_appInfo.app_type);
    return;
  }

  (void)mmgsdi_session_is_service_available( appInfo_ptr->mmgsdi_appInfo.session_id,
                                             srv_type,
                                             wms_sim_mmgsdi_response_cb,
                                             (mmgsdi_client_data_type)srv_type );
  return;
} /* wms_sim_mmgsdi_get_app_capabilities_cb_proc() */

/*=========================================================================
FUNCTION wms_sim_mmgsdi_read_sms_services()                      (INTERNAL)

DESCRIPTION
  Starts the process of reading sms service entries.

  SIM/USIM services are read in the following order:
    1. FDN
    2. MO control
    3. Data download PP
    4. Data download CB

  CSIM/RUIM services are read in the following order:
    1. SMS
    2. Data download PP
    3. Data download CB
    4. Broadcast configuration
    5. 3GPD

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
STATIC void wms_sim_mmgsdi_read_sms_services(
  wms_prov_app_info_s_type *appInfo_ptr
)
{
  MSG_HIGH_0("In wms_sim_mmgsdi_read_sms_services()");

  if (NULL != appInfo_ptr)
  {

    switch (appInfo_ptr->mmgsdi_appInfo.app_type)
    {
    case MMGSDI_APP_SIM:
    case MMGSDI_APP_USIM:
      {
        (void)mmgsdi_session_get_app_capabilities( appInfo_ptr->mmgsdi_appInfo.session_id,
                                                   wms_sim_mmgsdi_response_cb,
                                                   (mmgsdi_client_data_type)appInfo_ptr );
      }
      break;

    case MMGSDI_APP_CSIM:
    case MMGSDI_APP_RUIM:
      {
        mmgsdi_service_enum_type   srv_type = MMGSDI_CDMA_SRV_SMS;

        if (MMGSDI_APP_CSIM == appInfo_ptr->mmgsdi_appInfo.app_type)
        {
          srv_type = MMGSDI_CSIM_SRV_SMS;
        }

        (void)mmgsdi_session_is_service_available( appInfo_ptr->mmgsdi_appInfo.session_id,
                                                   srv_type,
                                                   wms_sim_mmgsdi_response_cb,
                                                   (mmgsdi_client_data_type)srv_type );

      }
      break;

    default:
      break;
    }
  }

  return;
} /* wms_sim_mmgsdi_read_sms_services() */


#if defined(FEATURE_QSC1215_SGLTE_FUSION) || defined(FEATURE_8960_SGLTE_FUSION)
/*=========================================================================
FUNCTION wms_sim_mmgsdi_update_response_cb()                     (INTERNAL)

DESCRIPTION
  Receives the confirmation for file update registration.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void wms_sim_mmgsdi_update_response_cb(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr
)
{
   MSG_HIGH_3( "file_update response cb: client_data 0x%x, cnf %d, status %d",
               cnf_ptr->session_reg_file_update_cnf.response_header.client_data,
               cnf,
               status );
} /* wms_sim_mmgsdi_update_response_cb() */

/*=========================================================================
FUNCTION wms_sim_mmgsdi_register_for_file_update()               (INTERNAL)

DESCRIPTION
  Register for file update notification events

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void wms_sim_mmgsdi_register_for_file_update(
  wms_prov_app_mmgsdi_info_s_type *app_mmgsdi_Info_ptr
)
{
  if (NULL != app_mmgsdi_Info_ptr)
  {
    int                          i;
    mmgsdi_return_enum_type      ret;
    mmgsdi_file_update_list_type update_files;
    mmgsdi_file_enum_type        file_list[] =
    {
      MMGSDI_TELECOM_SMS,
      MMGSDI_TELECOM_SMSP,
      MMGSDI_TELECOM_SMSS,
      MMGSDI_TELECOM_SMSR,
      MMGSDI_GSM_MWIS,
      MMGSDI_USIM_SMS,
      MMGSDI_USIM_SMSP,
      MMGSDI_USIM_SMSS,
      MMGSDI_USIM_SMSR,
      MMGSDI_USIM_MWIS,
      MMGSDI_CDMA_SMS,
      MMGSDI_CDMA_SMS_PARAMS,
      MMGSDI_CDMA_SMS_STATUS,
      MMGSDI_CSIM_SMS,
      MMGSDI_CSIM_SMS_PARAMS,
      MMGSDI_CSIM_SMS_STATUS,
      MMGSDI_GSM_CBMI,
      MMGSDI_GSM_CBMID,
      MMGSDI_GSM_CBMIR,
      MMGSDI_USIM_CBMI,
      MMGSDI_USIM_CBMID,
      MMGSDI_USIM_CBMIR
    };
    mmgsdi_access_type           file_watch_list[sizeof(file_list)/sizeof(file_list[0])];

    MSG_HIGH_0("In mmgsdi_session_register_for_file_update()");

    memset(file_watch_list, 0, sizeof(file_watch_list));

    for (i = 0; i < (int)(sizeof(file_list)/sizeof(file_list[0])); i++)
    {
      file_watch_list[i].access_method = MMGSDI_EF_ENUM_ACCESS;
      file_watch_list[i].file.file_enum = file_list[i];
    }

    update_files.num_files = sizeof(file_watch_list)/sizeof(file_watch_list[0]);
    update_files.file_list_ptr = file_watch_list;

    ret = mmgsdi_session_register_for_file_update( app_mmgsdi_Info_ptr->session_id,
                                                   &update_files,
                                                   wms_sim_mmgsdi_update_response_cb,
                                                   (mmgsdi_client_data_type)app_mmgsdi_Info_ptr);
    if (MMGSDI_SUCCESS != ret)
    {
      MSG_ERROR_1("mmgsdi_session_register_for_file_update failed: %d", ret);
    }
  }
} /* wms_sim_mmgsdi_register_for_file_update() */
#endif /* defined(FEATURE_QSC1215_SGLTE_FUSION) || defined(FEATURE_8960_SGLTE_FUSION) */

/*=========================================================================
FUNCTION wms_sim_mmgsdi_refresh_response_cb()                    (INTERNAL)

DESCRIPTION
  Provides logged confirmation of a refresh operation.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void wms_sim_mmgsdi_refresh_response_cb(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr
)
{
   MSG_HIGH_3( "refresh response cb: client_data 0x%x, cnf %d, status %d",
               cnf_ptr->refresh_cnf.response_header.client_data,
               cnf,
               status );
} /* wms_sim_mmgsdi_refresh_response_cb() */

/*=========================================================================
FUNCTION wms_sim_mmgsdi_register_for_refresh()                   (INTERNAL)

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void wms_sim_mmgsdi_register_for_refresh(
  wms_prov_app_info_s_type *appInfo_ptr
)
{
   if (NULL != appInfo_ptr)
   {
      mmgsdi_refresh_file_list_type refresh_files;
      mmgsdi_return_enum_type       status;
      mmgsdi_file_enum_type         file_list[] =
      {
         MMGSDI_TELECOM_SMS,
         MMGSDI_TELECOM_SMSP,
         MMGSDI_TELECOM_SMSS,
         MMGSDI_TELECOM_SMSR,
         MMGSDI_GSM_MWIS,
         MMGSDI_USIM_SMS,
         MMGSDI_USIM_SMSP,
         MMGSDI_USIM_SMSS,
         MMGSDI_USIM_SMSR,
         MMGSDI_USIM_MWIS,
         MMGSDI_CDMA_SMS,
         MMGSDI_CDMA_SMS_PARAMS,
         MMGSDI_CDMA_SMS_STATUS,
         MMGSDI_CSIM_SMS,
         MMGSDI_CSIM_SMS_PARAMS,
         MMGSDI_CSIM_SMS_STATUS,
         MMGSDI_GSM_CBMI,
         MMGSDI_GSM_CBMID,
         MMGSDI_GSM_CBMIR,
         MMGSDI_USIM_CBMI,
         MMGSDI_USIM_CBMID,
         MMGSDI_USIM_CBMIR,
         MMGSDI_GSM_HPLMNACT,              // Home PLMN Access Technology
         MMGSDI_USIM_HPLMNWACT,            // Home PLMN Selector with Access Technology
         MMGSDI_USIM_RPLMNACT,             // Registered PLMN last used Access Technology
         MMGSDI_USIM_OCSGL,                // Operator CSG Lists and corresponding indications
         MMGSDI_SIM_7F66_PROP1_ACT_HPLMN,  // Acting Home PLMN
         MMGSDI_USIM_7F66_PROP1_ACT_HPLMN, // Acting Home PLMN
         MMGSDI_USIM_7F66_PROP1_RAT        // Radio Access Technology
      };

      refresh_files.num_files = sizeof(file_list)/sizeof(file_list[0]);
      refresh_files.file_list_ptr = file_list;
      refresh_files.file_path_ptr = NULL;
      refresh_files.file_path_len = 0;

      /* No refresh when RTRE mode is 1 */
      if ( ( (MMGSDI_APP_RUIM == appInfo_ptr->mmgsdi_appInfo.app_type) ||
             (MMGSDI_APP_CSIM == appInfo_ptr->mmgsdi_appInfo.app_type) ||
             (MMGSDI_APP_NONE == appInfo_ptr->mmgsdi_appInfo.app_type) ) &&
           wms_nv_rtre_control_is_nv() )
      {
        MSG_HIGH_0("mmgsdi_session_deregister_for_refresh()");
        status = mmgsdi_session_deregister_for_refresh( appInfo_ptr->mmgsdi_appInfo.session_id,
                                                        refresh_files,
                                                        FALSE,
                                                        wms_sim_mmgsdi_refresh_response_cb,
                                                        (mmgsdi_client_data_type)appInfo_ptr );
      }
      else
      {
        MSG_HIGH_0("mmgsdi_session_register_for_refresh()");
        status = mmgsdi_session_register_for_refresh( appInfo_ptr->mmgsdi_appInfo.session_id,
                                                      refresh_files,
                                                      TRUE,
                                                      wms_sim_mmgsdi_refresh_response_cb,
                                                      (mmgsdi_client_data_type)appInfo_ptr );
      }

      if (MMGSDI_SUCCESS != status)
      {
         MSG_ERROR_2( "mmgsdi_session_register_for_refresh session 0x%x failed: %d",
                      appInfo_ptr->mmgsdi_appInfo.session_id,
                      status );
      }
   }
} /* wms_sim_mmgsdi_register_for_refresh() */

/*=========================================================================
FUNCTION wms_sim_finish_mmgsdi_refresh()                         (INTERNAL)

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void wms_sim_finish_mmgsdi_refresh(
  wms_prov_app_info_s_type *appInfo_ptr
)
{
   MSG_HIGH_0("wms_sim_finish_mmgsdi_refresh");

   if (!wms_check_appinfo_is_valid(appInfo_ptr))
   {
      return;
   }
#ifdef FEATURE_GWSMS
   wms_cfg_do_message_list(appInfo_ptr, WMS_MEMORY_STORE_SIM, WMS_TAG_NONE, NULL);

   wms_cfg_do_template_list(appInfo_ptr, WMS_MEMORY_STORE_SIM, NULL);

   wms_cfg_do_memory_status(appInfo_ptr, WMS_MEMORY_STORE_SIM, WMS_TAG_MO_TEMPLATE, NULL);

   wms_cfg_do_memory_status(appInfo_ptr, WMS_MEMORY_STORE_SIM, WMS_TAG_NONE, NULL);

   wms_cfg_do_sts_report_list(appInfo_ptr, WMS_MEMORY_STORE_SIM, NULL);

   wms_cfg_do_memory_status(appInfo_ptr, WMS_MEMORY_STORE_SIM, WMS_TAG_STATUS_RPT, NULL);

 #endif /* FEATURE_GWSMS */

 #ifdef FEATURE_CDSMS
   wms_cfg_do_message_list(appInfo_ptr, WMS_MEMORY_STORE_RUIM, WMS_TAG_NONE, NULL);

   wms_cfg_do_template_list(appInfo_ptr, WMS_MEMORY_STORE_RUIM, NULL);

   wms_cfg_do_memory_status(appInfo_ptr, WMS_MEMORY_STORE_RUIM, WMS_TAG_MO_TEMPLATE, NULL);

   wms_cfg_do_memory_status(appInfo_ptr, WMS_MEMORY_STORE_RUIM, WMS_TAG_NONE, NULL);

#endif /* FEATURE_CDSMS */

   // Send refresh_complete for FCN refresh only
   if (MMGSDI_REFRESH_NAA_FCN == appInfo_ptr->refresh_mode)
   {
     mmgsdi_return_enum_type res;

     /* Send Refresh Complete */
     res = mmgsdi_session_refresh_complete( appInfo_ptr->mmgsdi_appInfo.session_id,
                                            TRUE,
                                            wms_sim_mmgsdi_refresh_response_cb,
                                            (mmgsdi_client_data_type)appInfo_ptr );
     if (MMGSDI_SUCCESS != res)
     {
       MSG_ERROR_1("mmgsdi_session_refresh_complete failed: error=%d", res);
     }
   }

   if (appInfo_ptr->refresh_sc_addr_changed)
   {
     // TODO: Fix pending MO records -- update with new SC addr
     appInfo_ptr->refresh_sc_addr_changed = FALSE;
   }

   appInfo_ptr->refresh_in_progress = FALSE;
   appInfo_ptr->refresh_started = FALSE;
   appInfo_ptr->refresh_mode = MMGSDI_REFRESH_MODE_ENUM_MAX;
   appInfo_ptr->refresh_voted_for_fcn = FALSE;

   return;
} /* wms_sim_finish_mmgsdi_refresh() */

/*=========================================================================
FUNCTION wms_sim_finish_subs_init()                              (INTERNAL)

DESCRIPTION
  Processes the final steps for the sim subscription initialization.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void wms_sim_finish_subs_init(
  wms_prov_app_info_s_type *appInfo_ptr
)
{
  if (wms_sim_mmgsdi_init_done(appInfo_ptr))
  {
    // If a refresh is in progress, finish it
    if (appInfo_ptr->refresh_in_progress)
    {
      wms_sim_finish_mmgsdi_refresh(appInfo_ptr);
    }

    appInfo_ptr->sim_init_complete = TRUE;
  }

  wms_sim_do_subs_ready(appInfo_ptr);

  return;
} /* wms_sim_finish_subs_init() */

/*=========================================================================
FUNCTION wms_sim_refresh_will_cause_network_detach()             (INTERNAL)

DESCRIPTION
  Parses the refresh file list to check for any files changed that
  may cause the network to detach.
 
  Certain OTA transactions need to finish before allowing refresh to
  continue.  This function determines if any files have changed that
  need to delay start of a refresh.

DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
STATIC boolean wms_sim_refresh_will_cause_network_detach(
  mmgsdi_refresh_evt_info_type *refresh_evt_info
)
{
  boolean result = FALSE;
  uint32  idx;

  MSG_HIGH_0("Checking for refreshed files causing detach");

  switch (refresh_evt_info->mode)
  {
    case MMGSDI_REFRESH_NAA_INIT_FCN:
    case MMGSDI_REFRESH_NAA_FCN:
      for (idx = 0; idx < refresh_evt_info->refresh_files.num_files; idx++)
      {
        MSG_HIGH_2( "Refresh file [%d]: 0x%04X",
                    idx,
                    refresh_evt_info->refresh_files.file_list_ptr[idx] );
    
        switch (refresh_evt_info->refresh_files.file_list_ptr[idx])
        {
        case MMGSDI_GSM_HPLMNACT:
        case MMGSDI_USIM_HPLMNWACT:
        case MMGSDI_USIM_RPLMNACT:
        case MMGSDI_USIM_OCSGL:
        case MMGSDI_SIM_7F66_PROP1_ACT_HPLMN:
        case MMGSDI_USIM_7F66_PROP1_ACT_HPLMN:
        case MMGSDI_USIM_7F66_PROP1_RAT:
          MSG_HIGH_1("Will cause detach: file 0x%04X", refresh_evt_info->refresh_files.file_list_ptr[idx]);
          result = TRUE;
          break;

        default:
          break;
        }

        if (result)
        {
          // No need to check more
          break;
        }
      }
      break;
      
    default: 
      // For all other refresh modes, file list will be empty
      // So by default treat if NW detach will happen
      result = TRUE;
      break;
  }

  return result;
} /* wms_sim_refresh_will_cause_network_detach() */

/*=========================================================================
FUNCTION wms_sim_refresh_will_cause_sc_addr_change()             (INTERNAL)

DESCRIPTION
  Parses the refresh file list to check for any files changed that
  may cause the SC address on the card to change.
 
  If the SC address changes when there are pending messages to be sent,
  the queued messages will not be using the latest SC address.
 
  While this condition is not expected to cause any issue, this function
  exists to make note of the condition in the case where such an issue
  may arise.  At this point, it is not expected that the SC address
  used by the UE must immediately and absolutely change.  Any remaining
  queued messages will be short-lived and the new SC address will be used
  within a short period of time.

DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
STATIC boolean wms_sim_refresh_will_cause_sc_addr_change(
  mmgsdi_refresh_evt_info_type *refresh_evt_info
)
{
  boolean result = FALSE;
  uint32  idx;

  MSG_HIGH_0("Checking for refreshed files causing sc_addr change");

  for (idx = 0; idx < refresh_evt_info->refresh_files.num_files; idx++)
  {
    MSG_HIGH_2( "Refresh file [%d]: 0x%04X",
                idx,
                refresh_evt_info->refresh_files.file_list_ptr[idx] );

    if ( (MMGSDI_TELECOM_SMSP    == refresh_evt_info->refresh_files.file_list_ptr[idx])  ||
         (MMGSDI_USIM_SMSP       == refresh_evt_info->refresh_files.file_list_ptr[idx])  ||
         (MMGSDI_CDMA_SMS_PARAMS == refresh_evt_info->refresh_files.file_list_ptr[idx])  || 
         (MMGSDI_CSIM_SMS_PARAMS == refresh_evt_info->refresh_files.file_list_ptr[idx]) )
    {
      MSG_HIGH_1("Will cause sc_addr_change: file 0x%04X", refresh_evt_info->refresh_files.file_list_ptr[idx]);
      result = TRUE;
      break;
    }
  }

  return result;
} /* wms_sim_refresh_will_cause_sc_addr_change() */

/*=========================================================================
FUNCTION wms_sim_set_sim_files_init_states()         (INTERNAL)

DESCRIPTION
  Set the INIT states of SIM files in WMS cache to the state supplied
  as an argument.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void wms_sim_set_sim_files_init_states(
  wms_prov_app_info_s_type *appInfo_ptr, 
  boolean                   inited
)
{
  uint8 i;
  MSG_HIGH_2("wms_sim_set_sim_files_init_states : appInfo_ptr = 0x%x, inited = %d", appInfo_ptr, inited);

  if (NULL != appInfo_ptr)
  {    
    for(i = 0; i < MAX_FILE_INDEX; i++)
    {
      appInfo_ptr->sim_file_init_is_done[i] = inited;      
    }	
  }
} /* wms_sim_set_sim_files_init_states() */

/*=========================================================================
FUNCTION wms_sim_map_file_enum_to_cache_index()         (INTERNAL)

DESCRIPTION
  Maps the SIM file enum to an index in WMS SIM file cache.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void wms_sim_map_file_enum_to_cache_index(
  mmgsdi_file_enum_type file_enum,
  uint8                *index
)
{
  switch (file_enum)
  {
    case MMGSDI_GSM_CBMI:
    case MMGSDI_USIM_CBMI:
      *index = CBMI_FILE_INDEX;
      break;
	
    case MMGSDI_GSM_CBMIR:
    case MMGSDI_USIM_CBMIR:
      *index = CBMIR_FILE_INDEX;
      break;

    case MMGSDI_GSM_CBMID:
    case MMGSDI_USIM_CBMID:
      *index = CBMID_FILE_INDEX;
      break;

    case MMGSDI_TELECOM_SMSS:
    case MMGSDI_USIM_SMSS:
    case MMGSDI_CDMA_SMS_STATUS:
    case MMGSDI_CSIM_SMS_STATUS:
      *index = SMSS_FILE_INDEX;
      break;

    case MMGSDI_TELECOM_SMSP:
    case MMGSDI_USIM_SMSP:
    case MMGSDI_CDMA_SMS_PARAMS:
    case MMGSDI_CSIM_SMS_PARAMS:
      *index = SMSP_FILE_INDEX;
      break;

    case MMGSDI_TELECOM_SMS:
    case MMGSDI_USIM_SMS:
    case MMGSDI_CDMA_SMS:
    case MMGSDI_CSIM_SMS:
      *index = SMS_FILE_INDEX;
      break;
	  
    case MMGSDI_TELECOM_SMSR:
    case MMGSDI_USIM_SMSR:
      *index = SMSR_FILE_INDEX;
      break;
		  
    case MMGSDI_GSM_MWIS:
    case MMGSDI_USIM_MWIS:
      *index = MWIS_FILE_INDEX;
      break;

    default :
      MSG_ERROR_1("Unrecognized file enum type :- %d", file_enum);
      *index = MAX_FILE_INDEX;
      break;
  }
} /* wms_sim_map_file_enum_to_cache_index() */

/*=========================================================================
FUNCTION wms_sim_mmgsdi_set_file_enums()                         (INTERNAL)

DESCRIPTION
  Initializes the list of files which need to be read for an app
  subscription to be ready for use.  The file list is based on the
  app type.  Some apps require fewer files to be processed.

DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void wms_sim_mmgsdi_set_file_enums(
  wms_prov_app_info_s_type *appInfo_ptr
)
{
  if (NULL == appInfo_ptr)
  {
    return;
  }

  switch (appInfo_ptr->mmgsdi_appInfo.app_type)
  {
    case MMGSDI_APP_SIM:
      appInfo_ptr->sms_file_enum   = MMGSDI_TELECOM_SMS;
      appInfo_ptr->smsp_file_enum  = MMGSDI_TELECOM_SMSP;
      appInfo_ptr->smsr_file_enum  = MMGSDI_TELECOM_SMSR;
      appInfo_ptr->smss_file_enum  = MMGSDI_TELECOM_SMSS;
      appInfo_ptr->mwis_file_enum  = MMGSDI_GSM_MWIS;
      appInfo_ptr->cbmi_file_enum  = MMGSDI_GSM_CBMI;
      appInfo_ptr->cbmir_file_enum = MMGSDI_GSM_CBMIR;
      appInfo_ptr->cbmid_file_enum = MMGSDI_GSM_CBMID;
      appInfo_ptr->lang_file_enum  = MMGSDI_GSM_LP;
      break;

    case MMGSDI_APP_USIM:
      appInfo_ptr->sms_file_enum   = MMGSDI_USIM_SMS;
      appInfo_ptr->smsp_file_enum  = MMGSDI_USIM_SMSP;
      appInfo_ptr->smsr_file_enum  = MMGSDI_USIM_SMSR;
      appInfo_ptr->smss_file_enum  = MMGSDI_USIM_SMSS;
      appInfo_ptr->mwis_file_enum  = MMGSDI_USIM_MWIS;
      appInfo_ptr->cbmi_file_enum  = MMGSDI_USIM_CBMI;
      appInfo_ptr->cbmir_file_enum = MMGSDI_USIM_CBMIR;
      appInfo_ptr->cbmid_file_enum = MMGSDI_USIM_CBMID;
      appInfo_ptr->lang_file_enum  = MMGSDI_USIM_LI;
      break;

    case MMGSDI_APP_RUIM:
      appInfo_ptr->sms_file_enum   = MMGSDI_CDMA_SMS;
      appInfo_ptr->smsp_file_enum  = MMGSDI_CDMA_SMS_PARAMS;
      appInfo_ptr->smsr_file_enum  = MMGSDI_NO_FILE_ENUM;
      appInfo_ptr->smss_file_enum  = MMGSDI_CDMA_SMS_STATUS;
      break;

    case MMGSDI_APP_CSIM:
      appInfo_ptr->sms_file_enum   = MMGSDI_CSIM_SMS;
      appInfo_ptr->smsp_file_enum  = MMGSDI_CSIM_SMS_PARAMS;
      appInfo_ptr->smsr_file_enum  = MMGSDI_NO_FILE_ENUM;
      appInfo_ptr->smss_file_enum  = MMGSDI_CSIM_SMS_STATUS;
      break;

    default:
      break;
  }
  return;
} /* wms_sim_mmgsdi_set_file_enums() */

/*===========================================================================
FUNCTION wms_sim_get_next_used_record()

DESCRIPTION
Returns the next used record number starting after prev_index. If the
memory allocation for SEEK data failed or the SEEK was unsuccessful,
returns the next record in the sequence starting from prev_index.

DEPENDENCIES
None

RETURNS
Record no. of the next filled record.

SIDE EFFECTS
None
===========================================================================*/
int wms_sim_get_next_used_record(
  wms_prov_app_info_s_type   *appInfo_ptr,
  int                        next_index
)
{
  int num_records = appInfo_ptr->wms_seek_data.num_of_records;

  /* Search for the next filled record starting from the next
     record to be read in sequence*/
  if ((NULL != appInfo_ptr->wms_seek_data.in_use_records) &&
      (WMS_OK_S == appInfo_ptr->wms_seek_data.status))
  {
    for (; next_index <= num_records; next_index++)
    {
      if (TRUE == appInfo_ptr->wms_seek_data.in_use_records[next_index])
      {
        break;
      }
    }
  }
  return next_index;
} /* wms_sim_get_next_used_record() */

/*===========================================================================
FUNCTION wms_sim_get_next_file_enum_index_to_be_read

DESCRIPTION  Returns the file enum index corresponding to the next file to be read.

DEPENDENCIES
None

RETURNS
File enum index corresponding to the SIM file to be read next.

SIDE EFFECTS
None
===========================================================================*/
uint8 wms_sim_get_next_file_enum_index_to_be_read(
  wms_prov_app_info_s_type   *appInfo_ptr,
  uint8                      next_file_index
)
{
  for ( ; next_file_index < MAX_FILE_INDEX; next_file_index ++)
  {
    if (!appInfo_ptr->sim_file_init_is_done[next_file_index])
    {
      break;
    }      
  }

  return next_file_index;
} /* wms_sim_get_next_file_enum_index_to_be_read() */

/*=========================================================================
FUNCTION wms_sim_map_cache_index_to_file_enum()       (INTERNAL)

DESCRIPTION
  Maps the index in WMS SIM file cache to corresponding SIM file enum.

DEPENDENCIES
  None.

RETURN VALUE
  mmgsdi_file_enum_type.

SIDE EFFECTS
  None
=========================================================================*/
mmgsdi_file_enum_type wms_sim_map_cache_index_to_file_enum(
uint8                      file_enum_cache_index,
wms_prov_app_info_s_type  *appInfo_ptr

)
{
  mmgsdi_file_enum_type file_enum = MMGSDI_MAX_FILE_ENUM;
  MSG_HIGH_2("wms_sim_map_cache_index_to_file_enum : appInfo_ptr= 0x%x, file_enum_cache_index = %d",
             appInfo_ptr,
             file_enum_cache_index );

  if (NULL != appInfo_ptr)
  {
    switch(file_enum_cache_index)
    {
      case CBMI_FILE_INDEX:
        file_enum = appInfo_ptr->cbmi_file_enum;
        break;

      case CBMIR_FILE_INDEX:
        file_enum = appInfo_ptr->cbmir_file_enum;
        break;

      case CBMID_FILE_INDEX:
        file_enum = appInfo_ptr->cbmid_file_enum;
        break;	

      case SMSS_FILE_INDEX:
        file_enum = appInfo_ptr->smss_file_enum;
        break;		

      case SMSP_FILE_INDEX:
        file_enum = appInfo_ptr->smsp_file_enum;
        break;  

      case SMS_FILE_INDEX:
        file_enum = appInfo_ptr->sms_file_enum;
        break;  

      case SMSR_FILE_INDEX:
        file_enum = appInfo_ptr->smsr_file_enum;
        break;	

      case MWIS_FILE_INDEX:
        file_enum = appInfo_ptr->mwis_file_enum;
        break;	
  	
      default :
        break;
    }
  }

  return file_enum;
} /* wms_sim_map_cache_index_to_file_enum() */


/* Files are read in the following order:
   1. EFcbmi
   2. EFcbmir
   3. EFcbmid
   4. EFsmss
   5. EFsmsp
   6. EFsms
   7. EFsmsr
   8. EFmwis
 */

/*=========================================================================
FUNCTION wms_sim_mmgsdi_read_next_sim_file()                         (INTERNAL)

DESCRIPTION
  Reads the SIM EF files in a fixed order . The files for which WMS SIM Init 
  cache states the initialization is completed will be skipped and will not 
  be read again.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void wms_sim_mmgsdi_read_next_sim_file(
  wms_prov_app_info_s_type *appInfo_ptr,
  uint8                     start_index
)
{
  uint8                  next_file_enum_index;
  mmgsdi_file_enum_type  next_file_enum;

  MSG_HIGH_2("wms_sim_mmgsdi_read_next_sim_file: appInfo_ptr 0x%x start_index %d", appInfo_ptr, start_index);

  /* When the session is deactivated, stop reading sim files */
  if (!appInfo_ptr->session_activated)
  {
    MSG_HIGH_1("Session is deactivated :%d",appInfo_ptr->session_activated);
    wms_sim_cache_clean_up(appInfo_ptr);
    return;
  }

  next_file_enum_index = wms_sim_get_next_file_enum_index_to_be_read(appInfo_ptr, start_index);	

  if (MAX_FILE_INDEX == next_file_enum_index)
  {
    MSG_HIGH_0("SIM file reading is complete");

    appInfo_ptr->sim_init_complete = TRUE;
    wms_cfg_report_sim_init_complete_event(appInfo_ptr, TRUE);
    wms_sim_finish_subs_init(appInfo_ptr);
    wms_cfg_send_sim_file_read_complete_event_to_pbm(appInfo_ptr);
    return;
  }
  
  next_file_enum = wms_sim_map_cache_index_to_file_enum(next_file_enum_index, appInfo_ptr);

  if (CBMI_FILE_INDEX <= next_file_enum_index && SMSS_FILE_INDEX >= next_file_enum_index)
  {
    mmgsdi_len_type  bytes_to_read = 2;

    switch(next_file_enum)
    {
      case MMGSDI_GSM_CBMI:
      case MMGSDI_GSM_CBMIR:
      case MMGSDI_GSM_CBMID:
      case MMGSDI_USIM_CBMI:
      case MMGSDI_USIM_CBMIR:
      case MMGSDI_USIM_CBMID:
        bytes_to_read = 0; // whole file 
        break;

      case MMGSDI_CDMA_SMS_STATUS:
      case MMGSDI_CSIM_SMS_STATUS:
        bytes_to_read = 5; // For EF_smss in CSIM and RUIM
        break;
      
      default:
        break;
    }
    (void)wms_sim_mmgsdi_read_transparent( appInfo_ptr,
                                           next_file_enum,
                                           bytes_to_read );
  }
  else
  {
    /* read next file */
    (void)wms_sim_mmgsdi_get_file_attr( appInfo_ptr,
                                        next_file_enum );
  }
} /* wms_sim_mmgsdi_read_next_sim_file() */

/*=========================================================================
FUNCTION wms_sim_mmgsdi_srv_available_cb_proc()                  (INTERNAL)

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
STATIC void wms_sim_mmgsdi_srv_available_cb_proc(
  const mmgsdi_cnf_type  *cnf_ptr
)
{
   wms_prov_app_info_s_type  *appInfo_ptr = NULL;
   mmgsdi_service_enum_type   srv_type;
   mmgsdi_service_enum_type   next_srv_type;

   MSG_HIGH_0("In wms_sim_mmgsdi_srv_available_cb_proc()");

   if (NULL == cnf_ptr)
   {
     MSG_ERROR_0("bad cb: NULL cnf_ptr");
     return;
   }

   // Get the app info using the session_id contained in the response
   appInfo_ptr = wms_get_appinfo_by_session_id(cnf_ptr->srv_available_cnf.response_header.session_id);

   if (!wms_check_appinfo_is_valid(appInfo_ptr))
   {
     return;
   }

   // Get the service type from the response 'client_data'
   srv_type = (mmgsdi_service_enum_type) cnf_ptr->srv_available_cnf.response_header.client_data;

   switch(srv_type)
   {
   case MMGSDI_GSM_SRV_MOSMS_CONTROL:
   case MMGSDI_USIM_SRV_MOSMS_CONTROL:
      appInfo_ptr->mo_control_enabled = cnf_ptr->srv_available_cnf.srv_available;

      /* read next service - data download PP */
      switch (appInfo_ptr->mmgsdi_appInfo.app_type)
      {
      case MMGSDI_APP_SIM:
         next_srv_type  = MMGSDI_GSM_SRV_DATA_DL_SMSPP;
         break;
      case MMGSDI_APP_USIM:
         next_srv_type  = MMGSDI_USIM_SRV_DATA_DL_SMSPP;
         break;
      default:
         MSG_ERROR_1("bad app type %d", appInfo_ptr->mmgsdi_appInfo.app_type);
         return;
      }

      (void)mmgsdi_session_is_service_available( appInfo_ptr->mmgsdi_appInfo.session_id,
                                                 next_srv_type,
                                                 wms_sim_mmgsdi_response_cb,
                                                 (mmgsdi_client_data_type)next_srv_type );
      break;

   case MMGSDI_GSM_SRV_DATA_DL_SMSPP:
   case MMGSDI_USIM_SRV_DATA_DL_SMSPP:
      appInfo_ptr->pp_data_download_enabled = cnf_ptr->srv_available_cnf.srv_available;

      /* read next service - data download CB */
      switch (appInfo_ptr->mmgsdi_appInfo.app_type)
      {
      case MMGSDI_APP_SIM:
         next_srv_type  = MMGSDI_GSM_SRV_DATA_DL_SMSCB;;
         break;
      case MMGSDI_APP_USIM:
         next_srv_type  = MMGSDI_USIM_SRV_DATA_DL_SMSCB;;
         break;
      default:
         MSG_ERROR_1("bad app type %d", appInfo_ptr->mmgsdi_appInfo.app_type);
         return;
      }

      (void)mmgsdi_session_is_service_available( appInfo_ptr->mmgsdi_appInfo.session_id,
                                                 next_srv_type,
                                                 wms_sim_mmgsdi_response_cb,
                                                 (mmgsdi_client_data_type)next_srv_type );
      break;

   case MMGSDI_GSM_SRV_DATA_DL_SMSCB:
   case MMGSDI_USIM_SRV_DATA_DL_SMSCB:
  {  
      uint8 start_index;

      appInfo_ptr->cb_data_download_enabled = cnf_ptr->srv_available_cnf.srv_available;
      MSG_HIGH_4( "FDN Enabled: %d, MO Control Enabled: %d, PP Download Enabled: %d, CB Download Enabled: %d", 
                   appInfo_ptr->fdn_enabled, 
                   appInfo_ptr->mo_control_enabled, 
                   appInfo_ptr->pp_data_download_enabled,
                   appInfo_ptr->cb_data_download_enabled );
      /* done with reading services - now read broadcast or sms files */
#ifdef FEATURE_GWSMS_BROADCAST
      start_index = CBMI_FILE_INDEX;
#else /* +FEATURE_GWSMS_BROADCAST- */
      start_index = SMSS_FILE_INDEX;
      appInfo_ptr->sim_file_init_is_done[CBMI_FILE_INDEX]  = TRUE;
      appInfo_ptr->sim_file_init_is_done[CBMIR_FILE_INDEX] = TRUE;
      appInfo_ptr->sim_file_init_is_done[CBMID_FILE_INDEX] = TRUE;
#endif /* -FEATURE_GWSMS_BROADCAST. */

      wms_sim_mmgsdi_read_next_sim_file(appInfo_ptr, start_index);
  }
   break;

   case MMGSDI_CDMA_SRV_SMS:
   case MMGSDI_CSIM_SRV_SMS:
      appInfo_ptr->smscap_enabled = cnf_ptr->srv_available_cnf.srv_available;

      if (MMGSDI_CDMA_SRV_SMS == srv_type)
      {
         next_srv_type = MMGSDI_CDMA_SRV_DATA_DL_SMSPP;
      }
      else
      {
         next_srv_type = MMGSDI_CSIM_SRV_DATA_DL_SMSPP;
      }

      (void)mmgsdi_session_is_service_available( appInfo_ptr->mmgsdi_appInfo.session_id,
                                                 next_srv_type,
                                                 wms_sim_mmgsdi_response_cb,
                                                 (mmgsdi_client_data_type)next_srv_type );
      break;

   case MMGSDI_CDMA_SRV_DATA_DL_SMSPP:
   case MMGSDI_CSIM_SRV_DATA_DL_SMSPP:
      appInfo_ptr->pp_data_download_enabled = cnf_ptr->srv_available_cnf.srv_available;

      if (MMGSDI_CDMA_SRV_DATA_DL_SMSPP == srv_type)
      {
         next_srv_type = MMGSDI_CDMA_SRV_DATA_DL_SMSCB;
      }
      else
      {
         next_srv_type = MMGSDI_CSIM_SRV_DATA_DL_SMSCB;
      }

      (void)mmgsdi_session_is_service_available( appInfo_ptr->mmgsdi_appInfo.session_id,
                                                 next_srv_type,
                                                 wms_sim_mmgsdi_response_cb,
                                                 (mmgsdi_client_data_type)next_srv_type );
      break;

   case MMGSDI_CDMA_SRV_DATA_DL_SMSCB:
   case MMGSDI_CSIM_SRV_DATA_DL_SMSCB:
      appInfo_ptr->cb_data_download_enabled = cnf_ptr->srv_available_cnf.srv_available;

      if (MMGSDI_CDMA_SRV_DATA_DL_SMSCB == srv_type)
      {
         next_srv_type = MMGSDI_CDMA_SRV_SVR_CAT_PROG_BCSMS;
      }
      else
      {
         next_srv_type = MMGSDI_CSIM_SRV_BSVR_CAT_PROG_BCSMS;
      }

      (void)mmgsdi_session_is_service_available( appInfo_ptr->mmgsdi_appInfo.session_id,
                                                 next_srv_type,
                                                 wms_sim_mmgsdi_response_cb,
                                                 (mmgsdi_client_data_type)next_srv_type );
      break;

   case MMGSDI_CDMA_SRV_SVR_CAT_PROG_BCSMS:
   case MMGSDI_CSIM_SRV_BSVR_CAT_PROG_BCSMS:
      appInfo_ptr->bcsms_enabled = cnf_ptr->srv_available_cnf.srv_available;

      if (MMGSDI_CDMA_SRV_SVR_CAT_PROG_BCSMS == srv_type)
      {
         next_srv_type = MMGSDI_CDMA_SRV_3GPD_EXT;
      }
      else
      {
         next_srv_type = MMGSDI_CSIM_SRV_3GPD_EXT;
      }

      (void)mmgsdi_session_is_service_available( appInfo_ptr->mmgsdi_appInfo.session_id,
                                                 next_srv_type,
                                                 wms_sim_mmgsdi_response_cb,
                                                 (mmgsdi_client_data_type)next_srv_type );
      break;

   case MMGSDI_CDMA_SRV_3GPD_EXT:
   case MMGSDI_CSIM_SRV_3GPD_EXT:
    MSG_HIGH_4( "BCSMS Enabled: %d, SMSCap Enabled: %d, PP Download Enabled: %d, CB Download Enabled: %d", 
                appInfo_ptr->bcsms_enabled, 
                appInfo_ptr->smscap_enabled, 
                appInfo_ptr->pp_data_download_enabled,
                appInfo_ptr->cb_data_download_enabled );
    
     appInfo_ptr->smscap_enabled = appInfo_ptr->smscap_enabled &&
                                    cnf_ptr->srv_available_cnf.srv_available;
     nvruim_init_wms_svc_items( appInfo_ptr->bcsms_enabled,
                                appInfo_ptr->smscap_enabled );

     if (appInfo_ptr->smscap_enabled)
     {
        MSG_HIGH_0("SMSCAP is enabled");
        wms_nv_init_nvruim();
     }

#ifdef FEATURE_CDSMS
     {
        wms_cfg_s_type     *lcfg_s_ptr    = wms_cfg_s_ptr();
        nv_item_type nvi;

        if( NV_DONE_S == wms_nv_read_wait( NV_ESN_I, & nvi ) )
        {
          lcfg_s_ptr->phone_esn = nvi.esn.esn;
          MSG_HIGH_1("ESN = 0x%x", lcfg_s_ptr->phone_esn);
        }
     }
#endif

     appInfo_ptr->sim_file_init_is_done[CBMI_FILE_INDEX]  = TRUE;
     appInfo_ptr->sim_file_init_is_done[CBMIR_FILE_INDEX] = TRUE;
     appInfo_ptr->sim_file_init_is_done[CBMID_FILE_INDEX] = TRUE;

     wms_sim_mmgsdi_read_next_sim_file(appInfo_ptr, SMSS_FILE_INDEX);
     break;

   default:
      MSG_ERROR("Unexpected srv type %d", srv_type, 0, 0);
      break;
   }

  return;
} /* wms_sim_mmgsdi_srv_available_cb_proc() */
/*===========================================================================
FUNCTION wms_sim_mmgsdi_seek_cb_proc()

DESCRIPTION
Processing of Call back for the mmgsdi_seek().

DEPENDENCIES
None

RETURNS
None

SIDE EFFECTS
None
===========================================================================*/
void wms_sim_mmgsdi_seek_cb_proc(
  wms_cmd_type *cmd_ptr
)
{
  uint8                       *seek_data_ptr   = NULL;
  int                          next_record_num = 0;
  int                          i;
  mmgsdi_return_enum_type      ret;
  mmgsdi_cnf_enum_type         cnf;
  mmgsdi_cnf_type             *cnf_ptr;
  mmgsdi_return_enum_type      status;
  wms_prov_app_info_s_type    *appInfo_ptr;

  if (NULL == cmd_ptr)
  {
    MSG_HIGH_0("Null cmd_ptr in seek_cb_proc");
    return;
  }

  cnf             = cmd_ptr->cmd.mmgsdi_seek_cb.cnf;
  cnf_ptr         = &cmd_ptr->cmd.mmgsdi_seek_cb.cnf_type;
  status          = (mmgsdi_return_enum_type)(int)cmd_ptr->hdr.user_data;
  appInfo_ptr     = (wms_prov_app_info_s_type *)cnf_ptr->search_cnf.response_header.client_data;
  seek_data_ptr   = cnf_ptr->search_cnf.searched_record_nums.data_ptr;   

  MSG_HIGH_1("In wms_sim_mmgsdi_seek_cb_proc(): handle cnf = %d", cnf);

  if (!wms_check_appinfo_is_valid(appInfo_ptr))
  {
    return;
  }

  if (MMGSDI_SUCCESS != status)
  {
    mmgsdi_return_enum_type ret;

    MSG_ERROR_1("Error Status from MMGSDI %d in seek_cb, falling back to normal procedure", status);
    appInfo_ptr->wms_seek_data.status = WMS_GENERAL_ERROR_S;
    MSG_HIGH_0("Reading EF record num 1");
    appInfo_ptr->accessed_rec_num = 1;
    ret = mmgsdi_session_read_record( cnf_ptr->response_header.session_id,
                                      cnf_ptr->search_cnf.access,
                                      1, // starts from 1
                                      appInfo_ptr->wms_seek_data.record_length,
                                      wms_sim_mmgsdi_response_cb,
                                      (mmgsdi_client_data_type)appInfo_ptr);

    wms_check_status_mmgsdi_session_read_rec(ret);
    WMS_MEM_FREEIF(seek_data_ptr);

    return;
  }

  MSG_HIGH_1("Number of Records %d", appInfo_ptr->wms_seek_data.num_of_records);

  // Response for the SEEK request from MMGSDI is a list of unused/empty record numbers
  for (i = 0; i < cnf_ptr->search_cnf.searched_record_nums.data_len; i++)
  {
    // Mark each record returned by SEEK as unused.
    if ((seek_data_ptr[i] > 0) && (seek_data_ptr[i] <= appInfo_ptr->wms_seek_data.num_of_records))
    {
      appInfo_ptr->wms_seek_data.in_use_records[seek_data_ptr[i]] = FALSE;
    }
  }

  next_record_num = wms_sim_get_next_used_record(appInfo_ptr, 1);

  if (next_record_num <= appInfo_ptr->wms_seek_data.num_of_records)
  {
    MSG_HIGH_1("Reading EF record num %d", next_record_num);
    appInfo_ptr->accessed_rec_num = next_record_num;
    ret = mmgsdi_session_read_record( cnf_ptr->response_header.session_id,
                                      cnf_ptr->search_cnf.access,
                                      next_record_num,
                                      appInfo_ptr->wms_seek_data.record_length,
                                      wms_sim_mmgsdi_response_cb,
                                      (mmgsdi_client_data_type)appInfo_ptr);

    wms_check_status_mmgsdi_session_read_rec(ret);
  }
  else
  {
    /* Finished reading this EF records from the card */
    uint8  mapped_cache_index;

    MSG_HIGH_1("Empty EF file(file_enum = %d)", cnf_ptr->search_cnf.access.file.file_enum);

    wms_sim_map_file_enum_to_cache_index(cnf_ptr->search_cnf.access.file.file_enum,
                                         &mapped_cache_index);

    if (MAX_FILE_INDEX != mapped_cache_index)
    {
      appInfo_ptr->sim_file_init_is_done[mapped_cache_index] = TRUE;
    }

    // Read next file.
    wms_sim_mmgsdi_read_next_sim_file(appInfo_ptr, 
                                      mapped_cache_index + 1);
  }
  WMS_MEM_FREEIF(seek_data_ptr);
  
  return;
} /* wms_sim_mmgsdi_seek_cb_proc() */

/*===========================================================================
FUNCTION wms_sim_mmgsdi_seek_cb()

DESCRIPTION
Call back for the mmgsdi_seek().

DEPENDENCIES
None

RETURNS
None

SIDE EFFECTS
None
===========================================================================*/
STATIC void wms_sim_mmgsdi_seek_cb(
  mmgsdi_return_enum_type    status,
  mmgsdi_cnf_enum_type       cnf,
  const mmgsdi_cnf_type      *cnf_ptr
)
{
  wms_cmd_type   *cmd_ptr;
  uint8          *data_ptr = NULL;

  MSG_HIGH_2( "In wms_sim_mmgsdi_seek_cb status = %d cnf = %d",
              status,
              cnf );

  if (NULL == cnf_ptr)
  {
    MSG_ERROR_0("bad cb: NULL cnf_ptr");
    return;
  }

  if ((MMGSDI_SUCCESS  == status) &&
      (MMGSDI_SEARCH_CNF == cnf) &&
      (cnf_ptr->search_cnf.searched_record_nums.data_len > 0))
  {
    data_ptr = (uint8*)wms_mem_malloc((uint32)cnf_ptr->search_cnf.searched_record_nums.data_len);

    if (NULL == data_ptr)
    {
      MSG_ERROR_0("Out of memory in wms_sim_mmgsdi_response_cb()");
      return;
    }
  }

  cmd_ptr = wms_get_cmd_buf();
  if ( NULL != cmd_ptr)
  {
    MSG_HIGH_0("In wms_sim_mmgsdi_seek_cb()");

    cmd_ptr->hdr.cmd_id             = WMS_CMD_MMGSDI_SEEK_CB;
    cmd_ptr->hdr.client_id          = (wms_client_id_type)WMS_CLIENT_TYPE_INTERNAL;
    cmd_ptr->hdr.cmd_cb             = NULL;
    cmd_ptr->hdr.user_data          = (void *)status;
    cmd_ptr->hdr.appInfo_ptr        = (wms_prov_app_info_s_type *)cnf_ptr->response_header.client_data;
    cmd_ptr->cmd.mmgsdi_seek_cb.cnf = cnf;

    memscpy(&(cmd_ptr->cmd.mmgsdi_seek_cb.cnf_type), sizeof(mmgsdi_cnf_type), cnf_ptr, sizeof(mmgsdi_cnf_type));

    if ((MMGSDI_SUCCESS  == status) &&
        (cnf_ptr->search_cnf.searched_record_nums.data_len > 0) &&
        (NULL != data_ptr))
    {
      cmd_ptr->cmd.mmgsdi_seek_cb.cnf_type.search_cnf.searched_record_nums.data_ptr = data_ptr;
      memscpy(data_ptr,
              (uint32)cnf_ptr->search_cnf.searched_record_nums.data_len,
              cnf_ptr->search_cnf.searched_record_nums.data_ptr,
              (uint32)cnf_ptr->search_cnf.searched_record_nums.data_len);
    }
    wms_put_cmd( cmd_ptr );
  }
  else
  {
    MSG_ERROR_0("Out of cmd buf for WMS_CMD_MMGSDI_SEEK_CB");
  }
} /* wms_sim_mmgsdi_seek_cb() */

/*===========================================================================
FUNCTION wms_sim_free_seek_data()

DESCRIPTION
Free memory for in_use_records in wms_session_seek_data.

DEPENDENCIES
None

RETURNS
None

SIDE EFFECTS
None
===========================================================================*/
void wms_sim_free_seek_data(
  wms_prov_app_info_s_type * appInfo_ptr
)
{
  WMS_MEM_FREEIF(appInfo_ptr->wms_seek_data.in_use_records);
} /* wms_sim_free_seek_data() */

/*===========================================================================
FUNCTION wms_sim_init_seek_data()

DESCRIPTION
Initialize the members of wms_seek_data for the session_type

DEPENDENCIES
None

RETURNS
None

SIDE EFFECTS
None
===========================================================================*/
void wms_sim_init_seek_data(
  wms_prov_app_info_s_type   *appInfo_ptr,
  int                        num_of_rec,
  int                        rec_len
)
{
  appInfo_ptr->wms_seek_data.status         = WMS_OK_S;
  appInfo_ptr->wms_seek_data.in_use_records = NULL;
  appInfo_ptr->wms_seek_data.num_of_records = num_of_rec;
  appInfo_ptr->wms_seek_data.record_length  = rec_len ;
} /* wms_sim_init_seek_data() */

/*===========================================================================
FUNCTION wms_sim_fill_seek_pattern()

DESCRIPTION
This function fills the SEEK pattern to be used for different files

DEPENDENCIES
None

RETURNS
Status specifying whether the seek pattern was filled successfully
or not.

SIDE EFFECTS
None
===========================================================================*/
wms_status_e_type wms_sim_fill_seek_pattern(
  mmgsdi_file_enum_type      file_enum,
  mmgsdi_seek_data_type      *seek_ptr,
  mmgsdi_len_type            rec_len
)
{
  seek_ptr->data_len = rec_len;
  seek_ptr->data_ptr = (uint8*)wms_mem_malloc(seek_ptr->data_len);

  if (NULL == seek_ptr->data_ptr)
  {
    MSG_ERROR_0("Couldn't allocate seek_data_ptr");
    return WMS_GENERAL_ERROR_S;
  }

  memset(seek_ptr->data_ptr, 0xFF, seek_ptr->data_len);

  /* Pattern for EF_SMS and EF_SMSR will have first byte as 0x00 */
  if (!(MMGSDI_USIM_SMSP == file_enum || MMGSDI_CSIM_SMS_PARAMS == file_enum))
  {
    seek_ptr->data_ptr[0] = 0x00;
  }

  return WMS_OK_S;
} /* wms_sim_fill_seek_pattern */

/*===========================================================================
FUNCTION wms_sim_send_seek_request()

DESCRIPTION
This function sends the SEEK request to MMGSDI

DEPENDENCIES
None

SIDE EFFECTS
===========================================================================*/
wms_status_e_type wms_sim_send_seek_request(
  wms_prov_app_info_s_type   *appInfo_ptr,
  mmgsdi_session_id_type     session_id,
  mmgsdi_access_type         access
)
{
  wms_status_e_type         ret_val = WMS_GENERAL_ERROR_S;
  mmgsdi_option_type        dummy_option;
  mmgsdi_file_enum_type     file_enum = access.file.file_enum;
  mmgsdi_seek_data_type     seek_pattern;
  mmgsdi_return_enum_type   seek_ret;

  memset(&seek_pattern, 0, sizeof(seek_pattern));
  memset(&dummy_option, 0, sizeof(dummy_option));

  ret_val = wms_sim_fill_seek_pattern(file_enum, &seek_pattern, appInfo_ptr->wms_seek_data.record_length);

  if (WMS_OK_S == ret_val)
  {
    mmgsdi_search_offset_data_type dummy_offset;
    memset(&dummy_offset, 0, sizeof(dummy_offset));
    seek_ret = mmgsdi_session_search(session_id,
                                     access,
                                     MMGSDI_UICC_SIMPLE_SEARCH,
                                     1,
                                     MMGSDI_SEARCH_FORWARD_FROM_REC_NUM,
                                     dummy_offset,
                                     seek_pattern,
                                     wms_sim_mmgsdi_seek_cb,
                                     (mmgsdi_client_data_type)appInfo_ptr);
    if (MMGSDI_SUCCESS == seek_ret)
    {
      ret_val = WMS_OK_S;
    }
    else
    {
      MSG_ERROR_1("mmgsdi_search returned %d", seek_ret);
      ret_val = WMS_GENERAL_ERROR_S;
    }
  }

  /* Free the memory allocated for the SEEK pattern */
  WMS_MEM_FREEIF(seek_pattern.data_ptr);

  return ret_val;
} /* wms_sim_send_seek_request() */

/*===========================================================================
FUNCTION wms_sim_alloc_seek_data()

DESCRIPTION
Allocate memory for in_use_records and set all the records to USED for
the corresponding session_type

DEPENDENCIES
None

RETURNS
None

SIDE EFFECTS
None
===========================================================================*/
void wms_sim_alloc_seek_data(
  wms_prov_app_info_s_type   *appInfo_ptr,
  int                        num_of_records
)
{
  if (num_of_records > 0)
  {
    appInfo_ptr->wms_seek_data.in_use_records =(uint8*) wms_mem_malloc((num_of_records+1) * sizeof(uint8));

    /* Mark all records as used */
    if (NULL != appInfo_ptr->wms_seek_data.in_use_records)
    {
      memset(appInfo_ptr->wms_seek_data.in_use_records, TRUE, (uint32) (num_of_records+1) * sizeof(uint8));
    }
    else
    {
      MSG_ERROR_0("Could not allocate memory for SEEK data");
    }
  }
} /* wms_sim_alloc_seek_data() */

/*===========================================================================
FUNCTION wms_sim_seek_unused_records()

DESCRIPTION
Get the list of unused records via mmgsdi_seek(). These are returned via
the wms_sim_mmgsdi_seek_cb().

DEPENDENCIES
None

RETURNS
None

SIDE EFFECTS
None
===========================================================================*/
STATIC wms_status_e_type wms_sim_seek_unused_records(
  wms_prov_app_info_s_type   *appInfo_ptr,
  mmgsdi_session_id_type     session_id,
  mmgsdi_access_type         access,
  int                        num_of_rec,
  int                        rec_len
)
{
  boolean ret_val =  WMS_OK_S;

  /* Free data allocated for the previous file_id */
  wms_sim_free_seek_data(appInfo_ptr);
  wms_sim_init_seek_data(appInfo_ptr, num_of_rec, rec_len);
  wms_sim_alloc_seek_data(appInfo_ptr, num_of_rec);

  if ((MMGSDI_APP_USIM == appInfo_ptr->mmgsdi_appInfo.app_type) || 
      (MMGSDI_APP_CSIM == appInfo_ptr->mmgsdi_appInfo.app_type))
  {
    ret_val = wms_sim_send_seek_request(appInfo_ptr, session_id, access);
  }
  else
  {
    MSG_HIGH_1("Blocking Seek Request for the 2G card, app_type is %d", appInfo_ptr->mmgsdi_appInfo.app_type);
    ret_val = WMS_GENERAL_ERROR_S;
  }

  return ret_val;
} /* wms_sim_seek_unused_records() */

/*=========================================================================
FUNCTION wms_sim_read_app_files()                                (INTERNAL)

DESCRIPTION
  Reads the app files associated with a given subscription.

DEPENDENCIES
  The CM subscription session must be ready (SUBS_PREF_INFO received)
    and
  the MMGSDI session must be activated (SUBSCRIPTION_READY received)

RETURN VALUE
  None

SIDE EFFECTS
  Current app data and/or settings previously read may be changed.
=========================================================================*/
void wms_sim_read_app_files(
  wms_prov_app_info_s_type *appInfo_ptr
)
{
  if (NULL == appInfo_ptr)
  {
    return;
  }

  wms_sim_mmgsdi_set_file_enums(appInfo_ptr);
	
  if ( (MMGSDI_CDMA_SMS_STATUS == appInfo_ptr->smss_file_enum) ||
       (MMGSDI_CSIM_SMS_STATUS == appInfo_ptr->smss_file_enum) )
  {
     // There is no EF_smsr in CSIM and RUIM
    appInfo_ptr->sim_file_init_is_done[SMSR_FILE_INDEX]  = TRUE; 
    // There is no EF_mwis in CSIM and RUIM
    appInfo_ptr->sim_file_init_is_done[MWIS_FILE_INDEX]  = TRUE; 
  }

  (void)wms_sim_init_lang_files(appInfo_ptr);
  wms_sim_mmgsdi_read_sms_services(appInfo_ptr); // Completes asynchronously
  wms_cfg_report_sim_init_start_event(appInfo_ptr);
} /* wms_sim_read_app_files() */

/*=========================================================================
FUNCTION wms_sim_mmgsdi_refresh_event_proc()                     (INTERNAL)

DESCRIPTION
  Processes refresh events received from MMGSDI

DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void wms_sim_mmgsdi_refresh_event_proc(
  mmgsdi_event_data_type   *event,
  wms_prov_app_info_s_type *appInfo_ptr
)
{
  if ( (NULL == event) || (NULL == appInfo_ptr) )
  {
    return;
  }

  MSG_HIGH_3( "In wms_sim_mmgsdi_refresh_event_proc: stage %d, mode %d, session_id 0x%x",
              event->data.refresh.stage,
              event->data.refresh.mode,
              appInfo_ptr->mmgsdi_appInfo.session_id );

  switch (event->data.refresh.stage)
  {
  case MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT:
  case MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN:
    {
      // MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_[INIT|FCN] is received when files
      // registered for change notification are required to be refreshed. At this
      // stage, WMS must vote based on whether it is busy or not.

      mmgsdi_return_enum_type res;
      boolean                 ok_to_refresh       = TRUE;
      boolean                 sc_addr_changed     = FALSE;
      boolean                 pending_mo_messages = FALSE;
      boolean                 item_is_active      = FALSE;
      boolean                 nv_refresh_vote_ok  = FALSE;

      // Indicate that no vote for FCN during INIT_FCN has occurred
      appInfo_ptr->refresh_voted_for_fcn = FALSE;

      // Fetch the MMGSDI NV item 'refresh_vote_ok'
      item_is_active = wms_nv_efs_read_refresh_vote_ok(&nv_refresh_vote_ok, appInfo_ptr->asid);

      // Always vote TRUE for refresh in cases :- 
      // 1. NV refresh_vote_ok is active and set as TRUE.
      // 2. Refresh mode is RESET mode and source is ECALL.
      if ((item_is_active && nv_refresh_vote_ok) ||                    
          (MMGSDI_REFRESH_RESET == event->data.refresh.mode && MMGSDI_REFRESH_SOURCE_ECALL == event->data.refresh.source))
      {
        // NV item is active and set TRUE; vote positive for refresh
        // regardless of call state, MO/MT status, or other WMS busy.
        ok_to_refresh = TRUE;
      }
      else
      {
        //  Otherwise : vote based on WMS busy
        boolean                 wms_is_idle         = TRUE;
        boolean                 will_detach         = FALSE;

        will_detach = wms_sim_refresh_will_cause_network_detach(&(event->data.refresh));

        if (will_detach)
        {
#ifdef FEATURE_GWSMS
          // Check for messages pending ACKs
          if (wms_msg_wait_for_ack_report_ind(appInfo_ptr->asid))
          {
            MSG_HIGH_0("Vote NO while waiting for ack report ind for delivery report");
            ok_to_refresh = FALSE;
          }
#endif /* FEATURE_GWSMS */
        }

        sc_addr_changed = wms_sim_refresh_will_cause_sc_addr_change(&(event->data.refresh));

        if (sc_addr_changed)
        {
          MSG_HIGH_0("SC_ADDR may change with refresh");
        }

        wms_is_idle = wms_check_idle(appInfo_ptr->asid, TRUE); // Checks if any MO, MT, or commands present

        MSG_HIGH_2( "refresh: WMS idle %d, as_id=%d",
                    wms_is_idle,
                    appInfo_ptr->asid );

        // Check for pending MO messages
        pending_mo_messages = wms_msg_check_pending_mo_messages(appInfo_ptr->asid);

        MSG_HIGH_1("refresh: Pending MO message count: %d", pending_mo_messages);
      }

      if (ok_to_refresh)
      {
        // If voting positive for refresh, lock out new requests.  If requests aren't
        // locked out, WMS may be busy by the time the refresh actually starts.
        //
        // If, for some reason, WMS votes postitive for refresh and another task does
        // not (effectively cancelling the refresh), then a refresh event with stage
        // MMGSDI_REFRESH_STAGE_END_FAILED will arrive so the lockout can be reversed.

        appInfo_ptr->refresh_in_progress = TRUE; // Lock out requests
        appInfo_ptr->refresh_mode = event->data.refresh.mode;

        wms_cfg_report_not_ready_event(appInfo_ptr);

        if (pending_mo_messages && sc_addr_changed)
        {
          // There are pending MO/MT messages and SC addr may change
          appInfo_ptr->refresh_sc_addr_changed = TRUE;
        }

        if ( (MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN == event->data.refresh.stage) &&
              ( (MMGSDI_REFRESH_NAA_INIT_FCN             == event->data.refresh.mode) ||
                (MMGSDI_REFRESH_3G_SESSION_RESET         == event->data.refresh.mode) ) )
        {
          // If asked to vote for the FCN refresh during an INIT_FCN, WMS can
          // expect the FCN refresh to follow after the INIT part has completed.
          // This state must be kept because the FCN refresh is not sent to
          // clients not registered for files in the changed file list.
          appInfo_ptr->refresh_voted_for_fcn = TRUE;
        }
      }

      MSG_HIGH_3( "WMS votes %d to refresh, session_id %d, voted_for_fcn %d",
                  ok_to_refresh,
                  event->session_id,
                  appInfo_ptr->refresh_voted_for_fcn );

      // Vote based on whether WMS is idle
      res = mmgsdi_session_ok_to_refresh( event->session_id,
                                          ok_to_refresh,
                                          wms_sim_mmgsdi_refresh_response_cb,
                                          (mmgsdi_client_data_type)appInfo_ptr );
      if (MMGSDI_SUCCESS != res)
      {
        MSG_ERROR_1("mmgsdi_session_ok_to_refresh WAIT_FOR_OK failed: error=%d", res);
      }

      appInfo_ptr->refresh_started = FALSE;

    } // STAGE_WAIT_FOR_OK
    break;

  case MMGSDI_REFRESH_STAGE_START:
    {

      if (!appInfo_ptr->refresh_started)
      {
        // Invalidate cached values
        appInfo_ptr->refresh_in_progress = TRUE;
        appInfo_ptr->refresh_mode        = event->data.refresh.mode;
        appInfo_ptr->refresh_started     = TRUE;
    
        MSG_HIGH_0("Refresh has started");

      if ( (MMGSDI_REFRESH_NAA_FCN == event->data.refresh.mode) ||
           ( ( (MMGSDI_REFRESH_NAA_INIT_FCN     == event->data.refresh.mode) ||
               (MMGSDI_REFRESH_3G_SESSION_RESET == event->data.refresh.mode) ) &&
               appInfo_ptr->refresh_voted_for_fcn ) )
      {
        // Resetting of SIM states will be done based on the files that got changed.
      }
      else
      {
        // FCN event will not come, Reset all the SIM file init states so that all files are read.
        MSG_HIGH_0("Resetting the INIT state of all SIM files");
        wms_sim_set_sim_files_init_states(appInfo_ptr , FALSE);
      }

      appInfo_ptr->sim_file_read_is_complete = FALSE;
      appInfo_ptr->sim_init_complete = FALSE;
      wms_cfg_report_start_refresh_event(appInfo_ptr);


        // For INIT refreshes, no files are read when MMGSDI_REFRESH_STAGE_START
        // arrives. When the INIT is complete, a WMS_SUBSCRIPTION_READY event
        // or MMGSDI_REFRESH_STAGE_END_[SUCCESS|FAILED] event will arrive and
        // processing will resume then.

        if ( (MMGSDI_REFRESH_NAA_FCN == event->data.refresh.mode))
        {
          
          // For FCN refresh, it is expected to read files when MMGSDI_REFRESH_STAGE_START
          // arrives. Once all files are read, a REFRESH_COMPLETE response must be sent.
          // REFRESH_COMPLETE is sent at the end of wms_sim_mmgsdi_read_sms_services() as part
          // of the wms_sim_read_app_files() sequence.
          uint8 i;
          uint8 mapped_cache_index;

          //  There are some WMS specific SIM files that have been changed.
          //  Only these files need to be read again. Mark the init state of only these files as NOT_INITED (FALSE)  
          MSG_HIGH_0("Processing refresh stage start with NAA_FCN mode");
          for (i = 0; i < event->data.refresh.refresh_files.num_files; i++)
          {
            wms_sim_map_file_enum_to_cache_index(event->data.refresh.refresh_files.file_list_ptr[i],
                                                 &mapped_cache_index);
			
            if (MAX_FILE_INDEX != mapped_cache_index)
            {
              appInfo_ptr->sim_file_init_is_done[mapped_cache_index] = FALSE;
              MSG_HIGH_1("init_done set as FALSE for file at file_index %d", mapped_cache_index);

              if (SMSP_FILE_INDEX == mapped_cache_index)
              {
                MSG_HIGH_0("SMSP file got changed , reset gw_template variable");
                appInfo_ptr->gw_template.sc_addr.number_of_digits = 0;  // SMSP file has changed, reset the gw_template variable.
              }
            }
          }

         // INIT states of the SIM files have already been updated.  
          wms_sim_read_app_files(appInfo_ptr);
        }
      }
    } // Stage: MMGSDI_REFRESH_STAGE_START
    break;

  case MMGSDI_REFRESH_STAGE_END_FAILED:
    {
      // The MMGSDI_REFRESH_STAGE_END_FAILED event arrives after WMS has read
      // its files and has responded with REFRESH_COMPLETE or after an INIT
      // refresh has completed.

      MSG_HIGH_0("REFRESH_STAGE_END_FAILED");

      // The refresh for this mode is complete, reset the refresh mode state
      appInfo_ptr->refresh_started       = FALSE;
      appInfo_ptr->refresh_mode          = MMGSDI_REFRESH_MODE_ENUM_MAX;
      appInfo_ptr->refresh_voted_for_fcn = FALSE;

      // Undo the command lockout
      appInfo_ptr->refresh_in_progress   = FALSE;

      wms_sim_do_subs_ready(appInfo_ptr);
      wms_cfg_report_end_refresh_event(appInfo_ptr, FALSE);
    } // Stage: MMGSDI_REFRESH_STAGE_END_FAILED
    break;

  case MMGSDI_REFRESH_STAGE_END_SUCCESS:
    {
      // The MMGSDI_REFRESH_STAGE_END_SUCCESS event arrives after WMS has read
      // its files and has responded with REFRESH_COMPLETE or after an INIT
      // refresh has completed.

      MSG_HIGH_0("REFRESH_STAGE_END_SUCCESS");

      if ( (MMGSDI_REFRESH_NAA_FCN == event->data.refresh.mode) ||
           ( ( (MMGSDI_REFRESH_NAA_INIT_FCN     == event->data.refresh.mode) ||
               (MMGSDI_REFRESH_3G_SESSION_RESET == event->data.refresh.mode) ) &&
               appInfo_ptr->refresh_voted_for_fcn ) )
      {
        // Handle successful refresh complete for the following modes:
        //   MMGSDI_REFRESH_NAA_FCN,
        //   MMGSDI_REFRESH_NAA_INIT_FCN (FCN expected to follow)
        //   MMGSDI_REFRESH_3G_SESSION_RESET (FCN expected to follow)

        // The refresh for this mode is complete, reset the refresh mode state
        appInfo_ptr->refresh_started       = FALSE;
        appInfo_ptr->refresh_mode          = MMGSDI_REFRESH_MODE_ENUM_MAX;
        appInfo_ptr->refresh_voted_for_fcn = FALSE;

        // If FCN refresh has completed, undo the command lockout, refresh is no longer in progress.
        // For INIT_FCN, refresh remains in progress because an FCN refresh is expected.
        if (MMGSDI_REFRESH_NAA_FCN == event->data.refresh.mode)
        {
          // Undo the command lockout
          appInfo_ptr->refresh_in_progress = FALSE;
          wms_cfg_report_end_refresh_event(appInfo_ptr, TRUE);
        }

      }
      // else
      // {
      //   Handle successful refresh complete for non-FCN refresh modes after receiving MMGSDI subscription ready :
      //     MGSDI_REFRESH_RESET,
      //     MMGSDI_REFRESH_NAA_INIT,
      //     MMGSDI_REFRESH_NAA_INIT_FCN (no FCN vote),
      //     MMGSDI_REFRESH_NAA_INIT_FULL_FCN,
      //     MMGSDI_REFRESH_NAA_APP_RESET,
      //     MMGSDI_REFRESH_3G_SESSION_RESET (no FCN vote),
      //     MMGSDI_REFRESH_STEERING_OF_ROAMING,
      //     MMGSDI_REFRESH_RESET_AUTO,
      //     MMGSDI_REFRESH_APP_RESET_AUTO
      //
      //   For non-FCN refresh modes, MMGSDI_SUBSCRIPTION_READY event will always arrive after refresh.
      //   Perform the reading of SIM files as part of processing of MMGSDI_SUBSCRIPTION_READY event.
      // }
    } // Stage: MMGSDI_REFRESH_STAGE_END_SUCCESS
    break;

  default:
    break;
  } // switch refresh stage
} /* wms_sim_mmgsdi_refresh_event_proc() */

/*===========================================================================
FUNCTION wms_sim_cache_clean_up

DESCRIPTION
Clears up the cache corresponding to EF_SMS, EF_SMSP and EF_SMSR.Because of 
seek functionality implementation, it may happen that some of the records of 
these EF files may not be read. So, there is a need to clean up all these 
cache when SIM gets removed so that cached data corresponding to last SIM
read doesn't remain stored for subsequent SIM read cycles also.
For instance, consider a fully loaded SIM replaced by an empty SIM.

DEPENDENCIES
None

RETURNS
None

SIDE EFFECTS
None
===========================================================================*/
void wms_sim_cache_clean_up(
  wms_prov_app_info_s_type * appInfo_ptr
)
{
  uint8 index;

  MSG_HIGH_1("In wms_sim_cache_clean_up, appInfo_ptr = 0x%x", appInfo_ptr);

  if (NULL != appInfo_ptr)
  {
    appInfo_ptr->sim_file_init_is_done[CBMI_FILE_INDEX]  = FALSE;
    appInfo_ptr->sim_file_init_is_done[CBMIR_FILE_INDEX] = FALSE;
    appInfo_ptr->sim_file_init_is_done[CBMID_FILE_INDEX] = FALSE;

    appInfo_ptr->sim_file_init_is_done[SMSS_FILE_INDEX] = FALSE;
  
    // Clear the cache corresponding to EF_SMSP.
    for (index = 0; index < appInfo_ptr->smsp_rec_count; index++)
    {
      WMS_MEM_FREEIF(appInfo_ptr->smsp_raw_rec[index].data_ptr);
      appInfo_ptr->smsp_raw_rec[index].data_len = 0; 
      appInfo_ptr->smsp_rec_templates[index] = FALSE;
    }

#ifdef FEATURE_GWSMS
    WMS_MEM_FREEIF(appInfo_ptr->gw_template.alpha_id.data);
    memset(&appInfo_ptr->gw_template, 0, sizeof(appInfo_ptr->gw_template));
#endif /* FEATURE_GWSMS */

    appInfo_ptr->smsp_rec_count                          = 0;  
    appInfo_ptr->smsp_rec_len                            = 0;
    appInfo_ptr->sim_file_init_is_done[SMSP_FILE_INDEX] = FALSE;

    // Clear the cache corresponding to EF_SMS.
    for (index = 0; index < appInfo_ptr->sms_rec_count; index++)
    {
      appInfo_ptr->sms_rec_tags[index] = WMS_TAG_NONE;
    }

    appInfo_ptr->sms_rec_count                          = 0;  
    appInfo_ptr->sms_rec_len                            = 0;  
    appInfo_ptr->sim_file_init_is_done[SMS_FILE_INDEX]  = FALSE;    
    appInfo_ptr->voice_mail_active                      = FALSE;
    appInfo_ptr->voice_mail_index                       = (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX;

#ifdef FEATURE_GWSMS
    // Clear the cache corresponding to EF_SMSR.
    for (index = 0; index < appInfo_ptr->smsr_rec_count; index++)
    {
      appInfo_ptr->smsr_status_rpts[index] = FALSE;
      appInfo_ptr->smsr_sms_index[index] = 0;
    }

    appInfo_ptr->smsr_rec_count = 0;  
    appInfo_ptr->smsr_rec_len = 0;
#endif /* FEATURE_GWSMS */
    
    appInfo_ptr->sim_file_init_is_done[SMSR_FILE_INDEX]            =  FALSE;
    appInfo_ptr->sim_file_init_is_done[MWIS_FILE_INDEX]            = FALSE;
    appInfo_ptr->sim_file_read_is_complete                          = FALSE;
  }
} /* wms_sim_cache_clean_up() */

void wms_sim_mmgsdi_event_cb_proc(
  wms_cmd_type *cmd_ptr
)
{
  mmgsdi_event_data_type   *event;
  wms_prov_app_info_s_type *appInfo_ptr = NULL;

  if (NULL == cmd_ptr)
  {
    return;
  }

  event       = &cmd_ptr->cmd.mmgsdi_event_cb.event;
  appInfo_ptr =  cmd_ptr->hdr.appInfo_ptr;

  MSG_HIGH_2("In wms_sim_mmgsdi_event_cb_proc(): process event = %d, appInfo_ptr", event->evt, appInfo_ptr);

  if (NULL == appInfo_ptr)
  {
    switch (event->evt)
    {
    case MMGSDI_CARD_INSERTED_EVT:
      MSG_HIGH_1("INFO: MMGSDI_CARD_INSERTED_EVT slot %d", event->data.card_inserted.slot);
      break;

    case MMGSDI_CARD_REMOVED_EVT:
      MSG_HIGH_1("INFO: MMGSDI_CARD_REMOVED_EVT slot %d", event->data.card_removed.slot);
      break;

    case MMGSDI_CARD_ERROR_EVT:
      MSG_ERROR_2( "INFO: MMGSDI_CARD_ERROR_EVT slot %d, status %d",
                   event->data.card_error.slot,
                   event->data.card_error.info );
      break;

    default:
      MSG_ERROR_0("appInfo_ptr is NULL");
      break;    
    }

    return;
  }

  switch (event->evt)
  {
  case MMGSDI_SUBSCRIPTION_READY_EVT:
    {
      boolean   read_app_files = TRUE;

      MSG_HIGH_5( "MMGSDI_SUBSCRIPTION_READY_EVT: app type %d->%d, slot 0x%x->0x%x, session_id 0x%x",
                  appInfo_ptr->mmgsdi_appInfo.app_type,
                  event->data.subscription_ready.app_info.app_data.app_type,
                  appInfo_ptr->mmgsdi_appInfo.slot_id,
                  event->data.subscription_ready.app_info.slot,
                  event->session_id );

      appInfo_ptr->mmgsdi_appInfo.app_type          = event->data.subscription_ready.app_info.app_data.app_type;
      appInfo_ptr->mmgsdi_appInfo.slot_id           = event->data.subscription_ready.app_info.slot;
      appInfo_ptr->session_activated = TRUE; // MMGSDI_SESSION_CHANGED_EVT may be missed at startup, READY implies activated

      if ( (MMGSDI_APP_RUIM == appInfo_ptr->mmgsdi_appInfo.app_type) ||
           (MMGSDI_APP_CSIM == appInfo_ptr->mmgsdi_appInfo.app_type) )
      {
        // A CDMA app exists on the card and is ready to use.
        // If RTRE settings indicate NV mode, then the app should not be used.

        if (wms_nv_rtre_control_is_nv())
        {
          // 1X is in NV mode
          appInfo_ptr->mmgsdi_appInfo.app_type          = MMGSDI_APP_NONE; // No SIM files to read
          read_app_files                 = FALSE;           // MMGSDI session isn't used

          appInfo_ptr->sim_init_complete = TRUE;
          wms_sim_do_subs_ready(appInfo_ptr);

          wms_cfg_report_sim_init_complete_event(appInfo_ptr, TRUE);
          wms_cfg_send_sim_file_read_complete_event_to_pbm(appInfo_ptr);

          MSG_HIGH_0( "In nv_mode. Skip reading App files");
        }
      }

      if (appInfo_ptr->refresh_in_progress)
      {
        if (appInfo_ptr->refresh_voted_for_fcn)
        {
          // Refresh is in progress and WMS has voted for FCN. 
          // Let the FCN part of refresh handle the file reads.
          //     MMGSDI_REFRESH_NAA_INIT_FCN     (FCN expected to follow).
          //     MMGSDI_REFRESH_3G_SESSION_RESET (FCN expected to follow).
          MSG_HIGH_0("Ignoring SUBS_READY received while refresh is in progress and WMS has voted for FCN ");
          read_app_files = FALSE;
        }
      }
      else
      {
        // Register for refresh only when the refresh is not in progress.
        wms_sim_mmgsdi_register_for_refresh( appInfo_ptr );
      }

      // Start reading SIM files
      if (read_app_files)
      {        
        MSG_HIGH_0("Resetting the INIT state of all SIM files");
        wms_sim_set_sim_files_init_states(appInfo_ptr , FALSE);
        appInfo_ptr->gw_template.sc_addr.number_of_digits = 0;
        wms_sim_read_app_files(appInfo_ptr);
      }

    }
    break;

  case MMGSDI_SESSION_CHANGED_EVT:
    {
      MSG_HIGH_5( "SESSION_CHANGED_EVT: appInfo_ptr 0x%x, as_id %d, session_id 0x%x: session_activated %d->%d",
                  appInfo_ptr,
                  appInfo_ptr->asid,
                  event->session_id,
                  appInfo_ptr->session_activated,
                  event->data.session_changed.activated );

      /* update the session_activated flag only when the session is deactivated. So that whenever session deactivates, WMS will stop reading sim files */
      if(!event->data.session_changed.activated)
      {
        appInfo_ptr->session_activated = event->data.session_changed.activated;
      }

      if (!appInfo_ptr->session_activated)
      {
         // Notify QMI_WMS to update QMI_WMS MWI State Variable
         wms_cfg_notify_mwi_data(NULL, appInfo_ptr, WMS_MWI_STATE_ERROR);

         appInfo_ptr->sim_init_complete = FALSE;
         wms_cfg_report_sim_init_complete_event(appInfo_ptr, FALSE);

         // Reset the app_type and slot_id
         appInfo_ptr->mmgsdi_appInfo.app_type          = MMGSDI_APP_NONE;
         appInfo_ptr->mmgsdi_appInfo.slot_id           = 0; // MMGSDI_SLOT_ID_NONE;

         // Cleanup of the subscription should be done here
         MSG_HIGH_0("Cleaning up WMS SIM cache");
         wms_sim_cache_clean_up(appInfo_ptr);
      }


      // Do not process session_activated == TRUE here.  That case
      // is handled later when the SUBSCRIPTION_READY arrives.
    }
    break;

  case MMGSDI_REFRESH_EVT:
    MSG_HIGH_0("Received MMGSDI_REFRESH_EVT");
    wms_sim_mmgsdi_refresh_event_proc(event, appInfo_ptr);
    break;

  case MMGSDI_FDN_EVT:
    MSG_HIGH_2( "Received MMGSDI_FDN_EVT with FDN enabled %d->%d",
                appInfo_ptr->fdn_enabled,
                event->data.fdn.enabled );

    appInfo_ptr->fdn_enabled = event->data.fdn.enabled;
    break;

  case MMGSDI_PERSO_EVT:
    break;

#if defined(FEATURE_QSC1215_SGLTE_FUSION) || defined(FEATURE_8960_SGLTE_FUSION)
  case MMGSDI_FILE_UPDATE_EVT:
    MSG_HIGH_0("Received MMGSDI_FILE_UPDATE_EVT");
    wms_sim_mmgsdi_file_update_event_proc(event, appInfo_ptr);
    break;
#endif /* defined(FEATURE_QSC1215_SGLTE_FUSION) || defined(FEATURE_8960_SGLTE_FUSION) */

  default:
    break;
  } // switch(event->evt)

  return;
} /* wms_sim_mmgsdi_event_cb_proc() */

mmgsdi_return_enum_type wms_sim_mmgsdi_get_file_attr(
  wms_prov_app_info_s_type *appInfo_ptr,
  mmgsdi_file_enum_type     file_enum
)
{
  mmgsdi_access_type        file_access;
  mmgsdi_return_enum_type   ret          = MMGSDI_INCORRECT_PARAMS;

  file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
  file_access.file.file_enum = file_enum;

  MSG_HIGH_4( "wms_sim_mmgsdi_get_file_attr: appInfo_ptr=0x%x, session_id=0x%x, as_id=%d, file_enum=%d",
              appInfo_ptr,
              appInfo_ptr->mmgsdi_appInfo.session_id,
              appInfo_ptr->asid,
              file_enum );

  if (MMGSDI_NO_FILE_ENUM != file_enum)
  {
    ret = mmgsdi_session_get_file_attr( appInfo_ptr->mmgsdi_appInfo.session_id,
                                        file_access,
                                        wms_sim_mmgsdi_response_cb,
                                        (mmgsdi_client_data_type)appInfo_ptr );
  }

  return ret;
} /* wms_sim_mmgsdi_get_file_attr() */

#ifdef FEATURE_GWSMS_BROADCAST
/*===========================================================================
FUNCTION: wms_sim_handle_cbmi_data()                               (INTERNAL)

DESCRIPTION:
  This function processes the data returned from the EFcbmi file when it is
  read. It is a common function used when the card is being initialized and
  when the notification event arrives that the EFcbmi file has changed.

RETURN VALUE
  NONE

DEPENDENCIES
  None

CALLED BY:
  wms_sim_mmgsdi_read_cb_cbmi_proc()
  wms_sim_mmgsdi_file_update_event_proc()
===========================================================================*/
STATIC void wms_sim_handle_cbmi_data(
  wms_prov_app_info_s_type *appInfo_ptr,
  mmgsdi_len_type           data_len,         // cnf_ptr->read_cnf.read_data.data_len
  uint8                    *data_ptr          // cnf_ptr->read_cnf.read_data.data_ptr
)
{
  if (NULL != appInfo_ptr)
  {

    if ( (data_len < 2) ||
         (NULL == data_ptr) )
    {
      appInfo_ptr->bc.gw.sim_cbmi_rec_len   = 0;
      appInfo_ptr->bc.gw.sim_cbmi_list_size = 0;

      MSG_HIGH_3( "EFcbmi asid %d: is empty, data_len %d, data_ptr 0x%x",
                  appInfo_ptr->asid,
                  data_len,
                  data_ptr );
    }
    else
    {
      uint32 len = 0;
      uint32 i = 0;
      uint8  valid_entry_count = 0;
      wms_gw_cb_srv_id_type srv_id;

      /* Ignore the last odd byte */
      len = (uint32)data_len / 2;
      len *= 2;
      len = MIN(len, WMS_SIM_CBMI_LIST_MAX * 2);

      valid_entry_count = 0;
      for (i = 0; i < len; i += 2)
      {
        srv_id = (data_ptr[i] << 8) | (data_ptr[i + 1]);
        if (srv_id != 0xFFFF)
        {
          appInfo_ptr->bc.gw.sim_cbmi_list[valid_entry_count] = srv_id;

          valid_entry_count++; /* a valid entry */
        }
      }

      appInfo_ptr->bc.gw.sim_cbmi_rec_len   = (uint32)data_len;
      appInfo_ptr->bc.gw.sim_cbmi_list_size = valid_entry_count;

      MSG_HIGH_3( "EFcbmi asid %d: length %d, valid entries %d",
                  appInfo_ptr->asid,
                  data_len,
                  valid_entry_count );
    }
  }
} /* wms_sim_handle_cbmi_data() */

/*===========================================================================
FUNCTION: wms_sim_handle_cbmir_data()                              (INTERNAL)

DESCRIPTION:
  This function processes the data returned from the EFcbmir file when it is
  read. It is a common function used when the card is being initialized and
  when the notification event arrives that the EFcbmir file has changed.

RETURN VALUE
  NONE

DEPENDENCIES
  None

CALLED BY:
  wms_sim_mmgsdi_read_cb_cbmir_proc()
  wms_sim_mmgsdi_file_update_event_proc()
===========================================================================*/
STATIC void wms_sim_handle_cbmir_data(
  wms_prov_app_info_s_type *appInfo_ptr,
  mmgsdi_len_type           data_len,         // cnf_ptr->read_cnf.read_data.data_len
  uint8                    *data_ptr          // cnf_ptr->read_cnf.read_data.data_ptr
)
{
  if (NULL != appInfo_ptr)
  {

    if ( (data_len < 4) ||
         (NULL == data_ptr) )
    {
      appInfo_ptr->bc.gw.sim_cbmir_rec_len   = 0;
      appInfo_ptr->bc.gw.sim_cbmir_list_size = 0;

      MSG_HIGH_3( "EFcbmir asid %d: is empty, data_len %d, data_ptr 0x%x",
                  appInfo_ptr->asid,
                  data_len,
                  data_ptr );
    }
    else
    {
      uint32 len = 0;
      uint32 i = 0;
      uint8  valid_entry_count = 0;
      wms_gw_cb_srv_id_type from;
      wms_gw_cb_srv_id_type to;

      /* Ignore the last 3 odd bytes */
      len = (uint32)data_len / 4;
      len *= 4;
      len = MIN(len, WMS_SIM_CBMIR_LIST_MAX * 4);

      valid_entry_count = 0;

      for (i = 0; i < len; i += 4)
      {
        from = (data_ptr[i] << 8) | (data_ptr[i + 1]);
        to   = (data_ptr[i + 2] << 8) | (data_ptr[i + 3]);
        if ((from != 0xFFFF) || (to != 0xFFFF))
        {
          appInfo_ptr->bc.gw.sim_cbmir_list[valid_entry_count].from = from;
          appInfo_ptr->bc.gw.sim_cbmir_list[valid_entry_count].to = to;
          valid_entry_count++; /* a valid entry */
        }
      }

      appInfo_ptr->bc.gw.sim_cbmir_rec_len   = (uint32)data_len;
      appInfo_ptr->bc.gw.sim_cbmir_list_size = valid_entry_count;

      MSG_HIGH_3( "EFcbmir asid %d: length %d, valid entries %d",
                  appInfo_ptr->asid,
                  data_len,
                  valid_entry_count );
    }
  }
} /* wms_sim_handle_cbmir_data() */

/*===========================================================================
FUNCTION: wms_sim_handle_cbmid_data()                              (INTERNAL)

DESCRIPTION:
  This function processes the data returned from the EFcbmid file when it is
  read. It is a common function used when the card is being initialized and
  when the notification event arrives that the EFcbmid file has changed.

RETURN VALUE
  NONE

DEPENDENCIES
  None

CALLED BY:
  wms_sim_mmgsdi_read_cb_cbmi_proc()
  wms_sim_mmgsdi_file_update_event_proc()
===========================================================================*/
STATIC void wms_sim_handle_cbmid_data(
  wms_prov_app_info_s_type *appInfo_ptr,
  mmgsdi_len_type           data_len,         // cnf_ptr->read_cnf.read_data.data_len
  uint8                    *data_ptr          // cnf_ptr->read_cnf.read_data.data_ptr
)
{
  if (NULL != appInfo_ptr)
  {

    if ( (data_len < 2) ||
         (NULL == data_ptr) )
    {
      appInfo_ptr->bc.gw.sim_cbmid_rec_len   = 0;
      appInfo_ptr->bc.gw.sim_cbmid_list_size = 0;

      MSG_HIGH_3( "EFcbmid asid %d: is empty, data_len %d, data_ptr 0x%x",
                  appInfo_ptr->asid,
                  data_len,
                  data_ptr );
    }
    else
    {
      uint32 len = 0;
      uint32 i = 0;
      uint8  valid_entry_count = 0;
      wms_gw_cb_srv_id_type srv_id;

      /* Ignore the last odd byte */
      len = (uint32)data_len / 2;
      len *= 2;
      len = MIN(len, WMS_SIM_CBMID_LIST_MAX * 2);

      valid_entry_count = 0;

      for (i = 0; i < len; i += 2 )
      {
        srv_id = (data_ptr[i] << 8) | (data_ptr[i + 1]);
        if (srv_id != 0xFFFF)
        {

          appInfo_ptr->bc.gw.sim_cbmid_list[valid_entry_count] = srv_id;
          valid_entry_count++; /* a valid entry */
        }
      }

      appInfo_ptr->bc.gw.sim_cbmid_rec_len   = (uint32)data_len;
      appInfo_ptr->bc.gw.sim_cbmid_list_size = valid_entry_count;

      MSG_HIGH_3( "EFcbmid asid %d: length %d, valid entries %d",
                  appInfo_ptr->asid,
                  data_len,
                  valid_entry_count );
    }
  }
} /* wms_sim_handle_cbmid_data() */
#endif  /* FEATURE_GWSMS_BROADCAST */

/* For certain error cases, the initialization of that file is considered */
void wms_sim_mmgsdi_handle_access_err(
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
)
{
  wms_prov_app_info_s_type *appInfo_ptr = NULL;
  mmgsdi_file_enum_type     file_enum;

  switch (cnf)
  {
  case MMGSDI_GET_FILE_ATTR_CNF:
    appInfo_ptr = (wms_prov_app_info_s_type *)cnf_ptr->get_file_attr_cnf.response_header.client_data;
    file_enum   = cnf_ptr->get_file_attr_cnf.access.file.file_enum;
    break;

  case MMGSDI_READ_CNF:
    appInfo_ptr = (wms_prov_app_info_s_type *)cnf_ptr->read_cnf.response_header.client_data;
    file_enum   = cnf_ptr->read_cnf.access.file.file_enum;
    break;

  default:
    MSG_ERROR_1("wms_sim_mmgsdi_handle_access_error: returning (ignore unexpected cnf %d)", cnf);
    return;
  }

  if (NULL == appInfo_ptr)
  {
    MSG_ERROR_1("wms_sim_mmgsdi_handle_access_error: returning (appInfo_ptr is NULL for cnf %d)", cnf);
    return;
  }

  MSG_HIGH_2( "wms_sim_mmgsdi_handle_access_err: session_type=%d, file enum=%d",
              appInfo_ptr->mmgsdi_appInfo.session_type,
              file_enum );

  switch (file_enum)
  {
  case MMGSDI_GSM_CBMI:
  case MMGSDI_USIM_CBMI:
    {
#ifdef FEATURE_GWSMS_BROADCAST
      wms_sim_handle_cbmi_data(appInfo_ptr, 0, NULL);
#endif  /* FEATURE_GWSMS_BROADCAST */

      appInfo_ptr->sim_file_init_is_done[CBMI_FILE_INDEX] = TRUE;
      wms_sim_mmgsdi_read_next_sim_file(appInfo_ptr,
                                        CBMIR_FILE_INDEX);
    }
    break;

  case MMGSDI_GSM_CBMIR:
  case MMGSDI_USIM_CBMIR:
    {
#ifdef FEATURE_GWSMS_BROADCAST
      wms_sim_handle_cbmir_data(appInfo_ptr, 0, NULL);
#endif  /* FEATURE_GWSMS_BROADCAST */

      appInfo_ptr->sim_file_init_is_done[CBMIR_FILE_INDEX] = TRUE;
      wms_sim_mmgsdi_read_next_sim_file(appInfo_ptr,
                                        CBMID_FILE_INDEX);

    }
    break;

  case MMGSDI_GSM_CBMID:
  case MMGSDI_USIM_CBMID:
    {
#ifdef FEATURE_GWSMS_BROADCAST
      wms_sim_handle_cbmid_data(appInfo_ptr, 0, NULL);

      /* done with reading broadcast files */
      wms_bc_gw_cb_init(appInfo_ptr, TRUE);
#endif  /* FEATURE_GWSMS_BROADCAST */
      appInfo_ptr->sim_file_init_is_done[CBMID_FILE_INDEX] = TRUE;
      wms_sim_mmgsdi_read_next_sim_file(appInfo_ptr,
                                         SMSS_FILE_INDEX);
    }
    break;

  case MMGSDI_TELECOM_SMSS:
  case MMGSDI_USIM_SMSS:
  case MMGSDI_CDMA_SMS_STATUS:
  case MMGSDI_CSIM_SMS_STATUS:
    {
      // If error getting file attributes or reading, set attributes
      appInfo_ptr->smss_last_message_number     = 0;
      appInfo_ptr->smss_last_wap_message_number = 0;
      appInfo_ptr->smss_mem_full_flag           = WMS_SIM_MEM_FULL_CLEAR;

      appInfo_ptr->sim_file_init_is_done[SMSS_FILE_INDEX] = TRUE;
      wms_sim_mmgsdi_read_next_sim_file(appInfo_ptr,
                                        SMSP_FILE_INDEX);

    }
    break;

  case MMGSDI_TELECOM_SMSP:
  case MMGSDI_USIM_SMSP:
  case MMGSDI_CDMA_SMS_PARAMS:
  case MMGSDI_CSIM_SMS_PARAMS:
    {
      if (MMGSDI_GET_FILE_ATTR_CNF == cnf)
      {
        // If error getting file attributes, consider the file empty
        appInfo_ptr->smsp_rec_count = 0;
        appInfo_ptr->smsp_rec_len   = 0;
      }
      else
      {
        // If error reading, record count is based on last valid accessed record
        appInfo_ptr->smsp_rec_count = appInfo_ptr->accessed_rec_num - 1;
      }

      appInfo_ptr->sim_file_init_is_done[SMSP_FILE_INDEX] = TRUE;
	  
      /* read next file */
      wms_sim_mmgsdi_read_next_sim_file(appInfo_ptr,
                                        SMS_FILE_INDEX);

    }
    break;

  case MMGSDI_TELECOM_SMS:
  case MMGSDI_USIM_SMS:
  case MMGSDI_CDMA_SMS:
  case MMGSDI_CSIM_SMS:
    {
      if (MMGSDI_GET_FILE_ATTR_CNF == cnf)
      {
        // If error getting file attributes, consider the file empty
        appInfo_ptr->sms_rec_count = 0;
        appInfo_ptr->sms_rec_len   = 0;
      }
      else
      {
        // If error reading, record count is based on last valid accessed record
        appInfo_ptr->sms_rec_count = appInfo_ptr->accessed_rec_num - 1;
      }

      appInfo_ptr->sim_file_init_is_done[SMS_FILE_INDEX] = TRUE;

      /* read next file */
      wms_sim_mmgsdi_read_next_sim_file(appInfo_ptr,
                                        SMSR_FILE_INDEX);

    }
    break;

  case MMGSDI_TELECOM_SMSR:
  case MMGSDI_USIM_SMSR:
    {
      if (MMGSDI_GET_FILE_ATTR_CNF == cnf)
      {
        // If error getting file attributes, consider the file empty
        appInfo_ptr->smsr_rec_count = 0;
        appInfo_ptr->smsr_rec_len   = 0;
      }
      else
      {
        // If error reading, record count is based on last valid accessed record
        appInfo_ptr->smsr_rec_count = appInfo_ptr->accessed_rec_num - 1;
      }

      appInfo_ptr->sim_file_init_is_done[SMSR_FILE_INDEX] = TRUE;
      wms_sim_mmgsdi_read_next_sim_file(appInfo_ptr,
                                        MWIS_FILE_INDEX);

    }
    break;

  case MMGSDI_GSM_MWIS:
  case MMGSDI_USIM_MWIS:
    {
      if (MMGSDI_GET_FILE_ATTR_CNF == cnf)
      {
        // If error getting file attributes, consider the file empty
        appInfo_ptr->mwis_rec_count = 0;
        appInfo_ptr->mwis_rec_len   = 0;
      }
      else
      {
        // If error reading, record count is based on last valid accessed record
        appInfo_ptr->mwis_rec_count = appInfo_ptr->accessed_rec_num - 1;
      }

      appInfo_ptr->sim_file_init_is_done[MWIS_FILE_INDEX] = TRUE;
      wms_sim_mmgsdi_read_next_sim_file(appInfo_ptr,
                                        MAX_FILE_INDEX);

    }
    break;

  default:
    break;
  }

  return;
} /* wms_sim_mmgsdi_handle_access_err() */

void wms_sim_mmgsdi_response_cb(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr
)
{
   wms_cmd_type   *cmd_ptr;
   uint8          *data_ptr = NULL;

   MSG_HIGH_2( "In wms_sim_mmgsdi_response_cb status = %d cnf = %d",
               status,
               cnf );

   if (NULL == cnf_ptr) {
     MSG_ERROR_0("bad cb: NULL cnf_ptr");
     return;
   }

   // Client 'user_data' is returned in: cnf_ptr->response_header.client_data

   // Allocate a buffer to hold returned data for read requests
   if ((MMGSDI_SUCCESS  == status) &&
       (MMGSDI_READ_CNF == cnf) &&
       (cnf_ptr->read_cnf.read_data.data_len > 0))
   {
     data_ptr = (uint8*)wms_mem_malloc((uint32)cnf_ptr->read_cnf.read_data.data_len);

     if (NULL == data_ptr)
     {
       MSG_ERROR_0("Out of memory in wms_sim_mmgsdi_response_cb()");
       return;
     }
   }

   cmd_ptr = wms_get_cmd_buf();

   if (NULL != cmd_ptr)
   {
      cmd_ptr->hdr.cmd_id      = WMS_CMD_MMGSDI_RESPONSE_CB;
      cmd_ptr->hdr.client_id   = (wms_client_id_type)WMS_CLIENT_TYPE_INTERNAL;
      cmd_ptr->hdr.cmd_cb      = NULL;
      cmd_ptr->hdr.user_data   = (void *)status;
      cmd_ptr->hdr.appInfo_ptr = NULL;

      // Put response type
      cmd_ptr->cmd.mmgsdi_response_cb.cnf = cnf;

      // Copy the response body (whole union so it is response-type agnostic)
      (void)memscpy( &(cmd_ptr->cmd.mmgsdi_response_cb.cnf_type),
                     sizeof(mmgsdi_cnf_type),
                     cnf_ptr,
                     sizeof(mmgsdi_cnf_type));

      // Copy the buffer for read requests
      if (NULL != data_ptr)
      {
        cmd_ptr->cmd.mmgsdi_response_cb.cnf_type.read_cnf.read_data.data_ptr = data_ptr;

        (void)memscpy( data_ptr,
                       (uint32)cnf_ptr->read_cnf.read_data.data_len,
                       cnf_ptr->read_cnf.read_data.data_ptr,
                       (uint32)cnf_ptr->read_cnf.read_data.data_len );
      }

      wms_put_cmd( cmd_ptr );
   }
   else
   {
      MSG_ERROR_0("Out of cmd buf for WMS_CMD_MMGSDI_RESPONSE_CB");
   }

   return;
} /* wms_sim_mmgsdi_response_cb() */

void wms_sim_mmgsdi_response_cb_proc(
  wms_cmd_type *cmd_ptr
)
{
   mmgsdi_cnf_enum_type      cnf;
   mmgsdi_cnf_type          *cnf_ptr;
   mmgsdi_return_enum_type   status;

   if (NULL == cmd_ptr) {
      return;
   }

   cnf     = cmd_ptr->cmd.mmgsdi_response_cb.cnf;
   cnf_ptr = &cmd_ptr->cmd.mmgsdi_response_cb.cnf_type;
   status  = (mmgsdi_return_enum_type)(int)cmd_ptr->hdr.user_data;

   MSG_HIGH_3( "In wms_sim_mmgsdi_response_cb_proc(): handle cnf = %d slot 0x%x, session_id 0x%x",
                cnf,
                cnf_ptr->response_header.slot_id,
                cnf_ptr->response_header.session_id );

   /* Continue to read the next EF file when error occurs */
   if (MMGSDI_SUCCESS != status)
   {
      wms_sim_mmgsdi_handle_access_err(cnf, cnf_ptr);
      return;
   }

   switch (cnf)
   {
   case MMGSDI_SESSION_GET_APP_CAPABILITIES_CNF:
      wms_sim_mmgsdi_get_app_capabilities_cb_proc(cnf_ptr);
      break;

   case MMGSDI_SRV_AVAILABLE_CNF:
      wms_sim_mmgsdi_srv_available_cb_proc(cnf_ptr);
      break;

   case MMGSDI_GET_FILE_ATTR_CNF:
      MSG_HIGH_1("get_file_attr response: file_enum %d", cnf_ptr->get_file_attr_cnf.access.file.file_enum);
      switch (cnf_ptr->get_file_attr_cnf.access.file.file_enum)
      {
      case MMGSDI_TELECOM_SMSP:
      case MMGSDI_USIM_SMSP:
      case MMGSDI_CDMA_SMS_PARAMS:
      case MMGSDI_CSIM_SMS_PARAMS:
         wms_sim_mmgsdi_file_attr_cb_smsp_proc(cnf_ptr);
         break;
      case MMGSDI_TELECOM_SMS:
      case MMGSDI_USIM_SMS:
      case MMGSDI_CDMA_SMS:
      case MMGSDI_CSIM_SMS:
         wms_sim_mmgsdi_file_attr_cb_sms_proc(cnf_ptr);
         break;
      case MMGSDI_TELECOM_SMSR:
      case MMGSDI_USIM_SMSR:
         wms_sim_mmgsdi_file_attr_cb_smsr_proc(cnf_ptr);
         break;
      case MMGSDI_GSM_MWIS:
      case MMGSDI_USIM_MWIS:
         wms_sim_mmgsdi_file_attr_cb_mwis_proc(cnf_ptr);
         break;
      default:
         break;
      }
      break;

   case MMGSDI_READ_CNF:
      MSG_HIGH_1("read response: file_enum %d", cnf_ptr->read_cnf.access.file.file_enum);
      switch (cnf_ptr->read_cnf.access.file.file_enum)
      {
#ifdef FEATURE_GWSMS
      case MMGSDI_GSM_CBMI:
      case MMGSDI_USIM_CBMI:
         wms_sim_mmgsdi_read_cb_cbmi_proc(cnf_ptr);
         break;
      case MMGSDI_GSM_CBMIR:
      case MMGSDI_USIM_CBMIR:
         wms_sim_mmgsdi_read_cb_cbmir_proc(cnf_ptr);
         break;
      case MMGSDI_GSM_CBMID:
      case MMGSDI_USIM_CBMID:
         wms_sim_mmgsdi_read_cb_cbmid_proc(cnf_ptr);
         break;
      case MMGSDI_TELECOM_SMSP:
      case MMGSDI_USIM_SMSP:
         wms_sim_mmgsdi_read_cb_smsp_gw_proc(cnf_ptr);
         break;
      case MMGSDI_TELECOM_SMSS:
      case MMGSDI_USIM_SMSS:
         wms_sim_mmgsdi_read_cb_smss_gw_proc(cnf_ptr);
         break;
      case MMGSDI_TELECOM_SMS:
      case MMGSDI_USIM_SMS:
         wms_sim_mmgsdi_read_cb_sms_gw_proc(cnf_ptr);
         break;
      case MMGSDI_TELECOM_SMSR:
      case MMGSDI_USIM_SMSR:
         wms_sim_mmgsdi_read_cb_smsr_proc(cnf_ptr);
         break;

      case MMGSDI_GSM_MWIS:
      case MMGSDI_USIM_MWIS:
         wms_sim_mmgsdi_read_cb_mwis_proc(cnf_ptr);
         break;

#endif
#ifdef FEATURE_CDSMS
      case MMGSDI_CDMA_SMS_PARAMS:
      case MMGSDI_CSIM_SMS_PARAMS:
         wms_sim_mmgsdi_read_cb_smsp_cdma_proc(cnf_ptr);
         break;
      case MMGSDI_CDMA_SMS_STATUS:
      case MMGSDI_CSIM_SMS_STATUS:
         wms_sim_mmgsdi_read_cb_smss_cdma_proc(cnf_ptr);
         break;
      case MMGSDI_CDMA_SMS:
      case MMGSDI_CSIM_SMS:
         wms_sim_mmgsdi_read_cb_sms_cdma_proc(cnf_ptr);
         break;
#endif
      default:
         break;
      }

      WMS_MEM_FREEIF(cnf_ptr->read_cnf.read_data.data_ptr);
      break;

   default:
      break;
   }
} /* wms_sim_mmgsdi_response_cb_proc() */


void wms_sim_mmgsdi_session_open_cb(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr
)
{
  if ( (MMGSDI_SUCCESS != status) ||
       (MMGSDI_SESSION_OPEN_EXT_CNF != cnf) ||
       (NULL == cnf_ptr) )
  {
    MSG_ERROR_3( "bad cb: cnf %d, status = %d, cnf_ptr = 0x%x",
                 cnf,
                 status,
                 cnf_ptr );
    return;
  }

  MSG_HIGH_0("In wms_sim_mmgsdi_session_open_cb()");

  {
    // Handle the multi-session open response
    int idx;

    for (idx = 0; idx < cnf_ptr->session_open_ext_cnf.num_sessions; idx++)
    {
      wms_prov_mmgsdi_app_info_s_type *wms_mmgsdi_appInfo_ptr;

      wms_mmgsdi_appInfo_ptr = wms_get_mmgsdi_appInfo_by_session_type(cnf_ptr->session_open_ext_cnf.session_info_ptr[idx].session_type);

      if (NULL != wms_mmgsdi_appInfo_ptr)
      {
        wms_mmgsdi_appInfo_ptr->session_id              = cnf_ptr->session_open_ext_cnf.session_info_ptr[idx].session_id;
        wms_mmgsdi_appInfo_ptr->slot_id                 = cnf_ptr->session_open_ext_cnf.session_info_ptr[idx].slot_id;
        wms_mmgsdi_appInfo_ptr->session_type            = cnf_ptr->session_open_ext_cnf.session_info_ptr[idx].session_type;
        wms_mmgsdi_appInfo_ptr->app_type                = cnf_ptr->session_open_ext_cnf.session_info_ptr[idx].app_info.app_type;
        wms_mmgsdi_appInfo_ptr->session_state           = WMS_SIM_MMGSDI_SESSION_OPENED;

        MSG_HIGH_5( "SessionX Opened: session_type=%d, session_id=0x%x, slot_id=0x%x, app_type=%d, appInfo_ptr=0x%x",
                    wms_mmgsdi_appInfo_ptr->session_type,
                    wms_mmgsdi_appInfo_ptr->session_id,
                    wms_mmgsdi_appInfo_ptr->slot_id,
                    wms_mmgsdi_appInfo_ptr->app_type,
                    wms_mmgsdi_appInfo_ptr );

        // Fetching nv_rtre_control here is too early.
        // RUIM hasn't registered with NV yet so return will always be NV_RTRE_CONTROL_NO_RUIM.
        // RTRE control can be fetched after the card has been registered (if there is one).

        // Register for refresh AFTER subs_ready.
        // If done now, then refresh events may be received for subscriptions which
        // aren't being used (CSIM when in nv mode).

#if defined(FEATURE_QSC1215_SGLTE_FUSION) || defined(FEATURE_8960_SGLTE_FUSION)
        wms_sim_mmgsdi_register_for_file_update(wms_mmgsdi_appInfo_ptr);
#endif /* defined(FEATURE_QSC1215_SGLTE_FUSION) || defined(FEATURE_8960_SGLTE_FUSION) */

      } // NULL != appInfo_ptr
    } // for
  }

  return;
} /* wms_sim_mmgsdi_session_open_cb() */


/*===========================================================================
FUNCTION: wms_sim_mmgsdi_init_done()                               (INTERNAL)

DESCRIPTION:

RETURN VALUE
  None

DEPENDENCIES
  None

CALLED BY:
  wms_sim_mmgsdi_response_cb_proc()
===========================================================================*/
boolean wms_sim_mmgsdi_init_done(
  wms_prov_app_info_s_type *appInfo_ptr
)
{
  uint8   file_index;
  boolean init_done = FALSE;
  
  if (NULL != appInfo_ptr)
  {
    init_done = TRUE;
    for(file_index = CBMI_FILE_INDEX; file_index < MAX_FILE_INDEX; file_index++)
    {
      if (!appInfo_ptr->sim_file_init_is_done[file_index])
      {
        init_done = FALSE;
        break;
      }
    }
  }
  return init_done;
} /* wms_sim_mmgsdi_init_done() */

/*===========================================================================
FUNCTION: wms_sim_mmgsdi_read_cb_cbmi_proc()                       (INTERNAL)

DESCRIPTION:
  This function processes the EFcbmi data when it is read.
  After handling the returned data, processing continues with reading
  the next file.

RETURN VALUE
  NONE

DEPENDENCIES
  None

CALLED BY:
  wms_sim_mmgsdi_response_cb_proc()
===========================================================================*/
void wms_sim_mmgsdi_read_cb_cbmi_proc(
  const mmgsdi_cnf_type *cnf_ptr
)
{
  /* cnf_ptr is checked by wms_sim_mmgsdi_response_cb */
  wms_prov_app_info_s_type *appInfo_ptr;

  appInfo_ptr = (wms_prov_app_info_s_type *)cnf_ptr->read_cnf.response_header.client_data;

  if (NULL == appInfo_ptr)
  {
    return;
  }

#ifdef FEATURE_GWSMS_BROADCAST
  wms_sim_handle_cbmi_data( appInfo_ptr,
                            cnf_ptr->read_cnf.read_data.data_len,
                            cnf_ptr->read_cnf.read_data.data_ptr);
#endif  /* FEATURE_GWSMS_BROADCAST */

  appInfo_ptr->sim_file_init_is_done[CBMI_FILE_INDEX] = TRUE;

  /* read next file */
  wms_sim_mmgsdi_read_next_sim_file(appInfo_ptr,
                                     CBMIR_FILE_INDEX);

} /* wms_sim_mmgsdi_read_cb_cbmi_proc() */

/*===========================================================================
FUNCTION: wms_sim_mmgsdi_read_cb_cbmir_proc()                      (INTERNAL)

DESCRIPTION:
  This function processes the EFcbmir data when it is read.
  After handling the returned data, processing continues with reading
  the next file.

RETURN VALUE
  NONE

DEPENDENCIES
  None

CALLED BY:
  wms_sim_mmgsdi_response_cb_proc()
===========================================================================*/
void wms_sim_mmgsdi_read_cb_cbmir_proc(
  const mmgsdi_cnf_type *cnf_ptr
)
{
  /* cnf_ptr is checked by wms_sim_mmgsdi_response_cb */
  wms_prov_app_info_s_type *appInfo_ptr;

  appInfo_ptr = (wms_prov_app_info_s_type *)cnf_ptr->read_cnf.response_header.client_data;

  if (NULL == appInfo_ptr)
  {
    return;
  }

#ifdef FEATURE_GWSMS_BROADCAST
  wms_sim_handle_cbmir_data( appInfo_ptr,
                             cnf_ptr->read_cnf.read_data.data_len,
                             cnf_ptr->read_cnf.read_data.data_ptr);
#endif  /* FEATURE_GWSMS_BROADCAST */

  appInfo_ptr->sim_file_init_is_done[CBMIR_FILE_INDEX] = TRUE;

  /* read next file */
  wms_sim_mmgsdi_read_next_sim_file(appInfo_ptr,
                                     CBMID_FILE_INDEX);


} /* wms_sim_mmgsdi_read_cb_cbmir_proc() */

/*===========================================================================
FUNCTION: wms_sim_mmgsdi_read_cb_cbmid_proc()                      (INTERNAL)

DESCRIPTION:
  This function processes the EFcbmid data when it is read.
  After handling the returned data, processing continues with reading
  the next file.

RETURN VALUE
  NONE

DEPENDENCIES
  None

CALLED BY:
  wms_sim_mmgsdi_response_cb_proc()
===========================================================================*/
void wms_sim_mmgsdi_read_cb_cbmid_proc(
  const mmgsdi_cnf_type *cnf_ptr
)
{
  /* cnf_ptr is checked by wms_sim_mmgsdi_response_cb */
  wms_prov_app_info_s_type *appInfo_ptr;

  appInfo_ptr = (wms_prov_app_info_s_type *)cnf_ptr->read_cnf.response_header.client_data;

  if (NULL == appInfo_ptr)
  {
    return;
  }

#ifdef FEATURE_GWSMS_BROADCAST
  wms_sim_handle_cbmid_data( appInfo_ptr,
                             cnf_ptr->read_cnf.read_data.data_len,
                             cnf_ptr->read_cnf.read_data.data_ptr);

  /* done with reading broadcast files */
  wms_bc_gw_cb_init(appInfo_ptr, TRUE);
#endif /* FEATURE_GWSMS_BROADCAST */

  appInfo_ptr->sim_file_init_is_done[CBMID_FILE_INDEX] = TRUE;

  /* read next file */
  wms_sim_mmgsdi_read_next_sim_file(appInfo_ptr,
                                     SMSS_FILE_INDEX);


} /* wms_sim_mmgsdi_read_cb_cbmid_proc() */

void wms_sim_mmgsdi_file_attr_cb_smsp_proc(
  const mmgsdi_cnf_type *cnf_ptr
)
{
  /* cnf_ptr is checked by wms_sim_mmgsdi_response_cb */
  wms_prov_app_info_s_type *appInfo_ptr;

  appInfo_ptr = (wms_prov_app_info_s_type *)cnf_ptr->get_file_attr_cnf.response_header.client_data;

  if (NULL == appInfo_ptr)
  {
    return;
  }

  MSG_HIGH_3( "wms_sim_mmgsdi_file_attr_cb_smsp_proc(): file_enum = %d, file_size = %d, num_of_rec = %d",
              cnf_ptr->get_file_attr_cnf.access.file.file_enum,
              cnf_ptr->get_file_attr_cnf.file_attrib.file_size,
              cnf_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.num_of_rec );

  if ( (cnf_ptr->get_file_attr_cnf.file_attrib.file_size <= 0) ||
       (cnf_ptr->get_file_attr_cnf.file_attrib.file_type != MMGSDI_LINEAR_FIXED_FILE) ||
       (cnf_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.num_of_rec <=0) )
  {
    uint8                 next_file_enum_index;
    mmgsdi_file_enum_type next_file_enum;

    appInfo_ptr->sim_file_init_is_done[SMSP_FILE_INDEX] = TRUE;
    next_file_enum_index                                = wms_sim_get_next_file_enum_index_to_be_read(appInfo_ptr,
                                                                                                      SMS_FILE_INDEX);
    next_file_enum		                        = wms_sim_map_cache_index_to_file_enum(next_file_enum_index,
                                                                                               appInfo_ptr);

#ifdef FEATURE_GWSMS
    if ( (MMGSDI_USIM_SMSP == cnf_ptr->get_file_attr_cnf.access.file.file_enum ) &&
         wms_nv_fallback_to_telecom_smsp_is_allowed() )
    {
      MSG_HIGH_0("USIM EFsmsp is empty. Fallback to TELECOM EFsmsp");
      next_file_enum = MMGSDI_TELECOM_SMSP;
      appInfo_ptr->smsp_file_enum = next_file_enum;
      appInfo_ptr->sim_file_init_is_done[SMSP_FILE_INDEX] = FALSE;
    }
#endif

    /* read next file */
    (void)wms_sim_mmgsdi_get_file_attr( appInfo_ptr,
                                        next_file_enum );
  }
  else
  {
    /* save info for next record read */
    appInfo_ptr->smsp_rec_count = MIN( WMS_MAX_TEMPLATES,
                                       cnf_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.num_of_rec);

    if (appInfo_ptr->smsp_rec_count > 0)
    {

      appInfo_ptr->smsp_rec_len = cnf_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.rec_len;
      appInfo_ptr->wms_seek_data.status = wms_sim_seek_unused_records(appInfo_ptr,
                                                                      cnf_ptr->response_header.session_id,
                                                                      cnf_ptr->get_file_attr_cnf.access,
                                                                      appInfo_ptr->smsp_rec_count,
                                                                      appInfo_ptr->smsp_rec_len);

      if (WMS_OK_S != appInfo_ptr->wms_seek_data.status)
      {
        /* Seek Failed, fall back to normal procedure */
        mmgsdi_return_enum_type ret;
        MSG_HIGH_0("Couldn't perform Seek, start reading EF sequentially from record num 1");
        appInfo_ptr->accessed_rec_num = 1;
        ret = mmgsdi_session_read_record( cnf_ptr->response_header.session_id,
                                          cnf_ptr->get_file_attr_cnf.access,
                                          1, // starts from 1
                                          appInfo_ptr->smsp_rec_len,
                                          wms_sim_mmgsdi_response_cb,
                                          (mmgsdi_client_data_type)appInfo_ptr );

        wms_check_status_mmgsdi_session_read_rec(ret);
      }
    }
  }
} /* wms_sim_mmgsdi_file_attr_cb_smsp_proc() */

void wms_sim_mmgsdi_file_attr_cb_mwis_proc(
  const mmgsdi_cnf_type *cnf_ptr
)
{
  wms_prov_app_info_s_type *appInfo_ptr;
  mmgsdi_access_type        file_access;

  file_access.access_method = MMGSDI_EF_ENUM_ACCESS;

  appInfo_ptr = (wms_prov_app_info_s_type *)cnf_ptr->get_file_attr_cnf.response_header.client_data;
  if (NULL == appInfo_ptr)
  {
    MSG_HIGH_0("appInfo_ptr is NULL");
    return;
  }

  MSG_HIGH_0("In wms_sim_mmgsdi_file_attr_cb_mwis_proc()");


  if ((cnf_ptr->get_file_attr_cnf.file_attrib.file_size <= 0) ||
      (cnf_ptr->get_file_attr_cnf.file_attrib.file_type != MMGSDI_LINEAR_FIXED_FILE) ||
      (cnf_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.num_of_rec <=0))
    {
      MSG_HIGH_3( "EFmwis is empty: file_size = %d, file_type = %d, num_of_rec = %d",
                  cnf_ptr->get_file_attr_cnf.file_attrib.file_size,
                  cnf_ptr->get_file_attr_cnf.file_attrib.file_type,
                  cnf_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.num_of_rec);


      appInfo_ptr->sim_file_init_is_done[MWIS_FILE_INDEX] = TRUE;

      /* read next file */
      wms_sim_mmgsdi_read_next_sim_file(appInfo_ptr,
                                        MAX_FILE_INDEX);
      return ;
    }

  /* save info for next record read */
  appInfo_ptr->mwis_rec_len   = MIN( WMS_SIM_MWI_REC_LEN_MAX,
                                     cnf_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.rec_len);
  appInfo_ptr->mwis_rec_count = cnf_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.num_of_rec;

  if (appInfo_ptr->mwis_rec_len < WMS_SIM_MWI_REC_LEN_MIN)
  {
    appInfo_ptr->mwis_rec_len = 0;
  }

  if (MMGSDI_APP_SIM == appInfo_ptr->mmgsdi_appInfo.app_type)
  {
    file_access.file.file_enum = MMGSDI_GSM_MWIS;
  }
  else if (MMGSDI_APP_USIM == appInfo_ptr->mmgsdi_appInfo.app_type)
  {
    file_access.file.file_enum = MMGSDI_USIM_MWIS;
  }
  else
  {
    MSG_ERROR_0("wms_sim_mmgsdi_file_attr_cb_mwis_proc(): Unknown app_type passed in");
    return;
  }

  appInfo_ptr->accessed_rec_num = 1;
  if (MMGSDI_SUCCESS != mmgsdi_session_read_record(cnf_ptr->response_header.session_id,
                                                    file_access,
                                                    1,  /* record number is 1 */
                                                    0,  /* record length of 0 means entire file will be read */
                                                    wms_sim_mmgsdi_response_cb,
                                                    (mmgsdi_client_data_type)appInfo_ptr))
  {
    MSG_ERROR_0("Error in mmgsdi_session_read_record()");
    return;
  }
} /* wms_sim_mmgsdi_file_attr_cb_mwis_proc() */

/*===========================================================================
FUNCTION: wms_sim_handle_smss_gw_data()                            (INTERNAL)

DESCRIPTION:
  This function processes the data returned from the EFsmss file when it is
  read. It is a common function used when the card is being initialized and
  when the notification event arrives that the EFsmss file has changed.

RETURN VALUE
  NONE

DEPENDENCIES
  None

CALLED BY:
  wms_sim_mmgsdi_read_cb_smss_gw_proc()
  wms_sim_mmgsdi_file_update_event_proc()
===========================================================================*/
STATIC void wms_sim_handle_smss_gw_data(
  wms_prov_app_info_s_type *appInfo_ptr,
  mmgsdi_len_type           data_len,        // cnf_ptr->read_cnf.read_data.data_len
  uint8                    *data_ptr         // cnf_ptr->read_cnf.read_data.data_ptr
)
{
  if ( (NULL != appInfo_ptr) &&
       (NULL != data_ptr) )
  {
    if (data_len < 2)
    {
      MSG_ERROR_1("bad smss data: data_len = %d", data_len);
    }
    else
    {
      appInfo_ptr->smss_last_message_number = data_ptr[0];
      appInfo_ptr->smss_mem_full_flag       = data_ptr[1];
    }
  }
}

/*===========================================================================
FUNCTION: wms_sim_mmgsdi_read_cb_smss_gw_proc()                    (INTERNAL)

DESCRIPTION:
  This function processes the EFsmss data when it is read.
  After handling the returned data, processing continues with reading
  the next file.

RETURN VALUE
  NONE

DEPENDENCIES
  None

CALLED BY:
  wms_sim_mmgsdi_response_cb_proc()
===========================================================================*/
void wms_sim_mmgsdi_read_cb_smss_gw_proc(
  const mmgsdi_cnf_type *cnf_ptr
)
{
   /* cnf_ptr is checked by wms_sim_mmgsdi_response_cb */
  wms_prov_app_info_s_type *appInfo_ptr;

  appInfo_ptr = (wms_prov_app_info_s_type *)cnf_ptr->read_cnf.response_header.client_data;

  MSG_HIGH_0("In wms_sim_mmgsdi_read_cb_smss_gw_proc()");

  if (NULL == appInfo_ptr)
  {
    return;
  }

  if (!appInfo_ptr->sim_file_init_is_done[SMSS_FILE_INDEX])
  {
    wms_sim_handle_smss_gw_data( appInfo_ptr,
                                 cnf_ptr->read_cnf.read_data.data_len,
                                 cnf_ptr->read_cnf.read_data.data_ptr);

  }

 appInfo_ptr->sim_file_init_is_done[SMSS_FILE_INDEX] = TRUE;
 
 /* read next file */
 wms_sim_mmgsdi_read_next_sim_file(appInfo_ptr,
                                   SMSP_FILE_INDEX);


} /* wms_sim_mmgsdi_read_cb_smss_gw_proc() */


void wms_sim_mmgsdi_file_attr_cb_sms_proc(
  const mmgsdi_cnf_type *cnf_ptr
)
{
  wms_prov_app_info_s_type *appInfo_ptr;

  MSG_HIGH_0("In wms_sim_mmgsdi_file_attr_cb_sms_proc()");

  appInfo_ptr = (wms_prov_app_info_s_type *)cnf_ptr->get_file_attr_cnf.response_header.client_data;


  if (NULL == appInfo_ptr)
  {
    return;
  }

  if ( (cnf_ptr->get_file_attr_cnf.file_attrib.file_size <= 0) ||
       (cnf_ptr->get_file_attr_cnf.file_attrib.file_type != MMGSDI_LINEAR_FIXED_FILE) ||
       (cnf_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.num_of_rec <= 0))
  {
    MSG_HIGH_3( "EFsms is empty: file_size = %d, file_type = %d, num_of_rec = %d",
                cnf_ptr->get_file_attr_cnf.file_attrib.file_size,
                cnf_ptr->get_file_attr_cnf.file_attrib.file_type,
                cnf_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.num_of_rec);

    appInfo_ptr->sim_file_init_is_done[SMS_FILE_INDEX] = TRUE;

    /* read next file */
    wms_sim_mmgsdi_read_next_sim_file(appInfo_ptr,
                                      SMSR_FILE_INDEX);

  }
  else
  {
    /* save info for next record read */
    appInfo_ptr->sms_rec_count = MIN( WMS_MESSAGE_LIST_MAX,
                                      cnf_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.num_of_rec);

    if (appInfo_ptr->sms_rec_count > 0)
    {

      appInfo_ptr->sms_rec_len = cnf_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.rec_len;
      appInfo_ptr->wms_seek_data.status = wms_sim_seek_unused_records( appInfo_ptr,
                                                                       cnf_ptr->response_header.session_id,
                                                                       cnf_ptr->get_file_attr_cnf.access,
                                                                       appInfo_ptr->sms_rec_count,
                                                                       appInfo_ptr->sms_rec_len );

      if(WMS_OK_S != appInfo_ptr->wms_seek_data.status)
      {
        /* Seek Failed, fall back to normal procedure */
        mmgsdi_return_enum_type ret;
        MSG_HIGH_0("Couldn't perform Seek, start reading EF sequentially from record num 1");
        appInfo_ptr->accessed_rec_num = 1;
        ret = mmgsdi_session_read_record( cnf_ptr->response_header.session_id,
                                          cnf_ptr->get_file_attr_cnf.access,
                                          1, // starts from 1
                                          appInfo_ptr->sms_rec_len,
                                          wms_sim_mmgsdi_response_cb,
                                          (mmgsdi_client_data_type)appInfo_ptr );

        wms_check_status_mmgsdi_session_read_rec(ret);

      }
    }
  }
} /* wms_sim_mmgsdi_file_attr_cb_sms_proc() */

#ifdef FEATURE_GWSMS
/*===========================================================================
FUNCTION: wms_sim_handle_sms_gw_data()                             (INTERNAL)

DESCRIPTION:
  This function processes the data returned from the EFsms file when it is
  read. It is a common function used when the card is being initialized and
  when the notification event arrives that the EFsms file has changed.

RETURN VALUE
  NONE

DEPENDENCIES
  None

CALLED BY:
  wms_sim_mmgsdi_read_cb_sms_gw_proc()
  wms_sim_mmgsdi_file_update_event_proc()
===========================================================================*/
STATIC void wms_sim_handle_sms_gw_data(
  wms_prov_app_info_s_type *appInfo_ptr,
  mmgsdi_len_type           data_len,         // cnf_ptr->read_cnf.read_data.data_len
  uint8                    *data_ptr,         // cnf_ptr->read_cnf.read_data.data_ptr
  mmgsdi_rec_num_type       accessed_rec_num  // cnf_ptr->read_cnf.accessed_rec_num
)
{
  if ( (NULL != appInfo_ptr) &&
       (NULL != data_ptr) )
  {
    if (data_len > 0)
    {
      if ( WMS_IS_EFSMS_REC_EMPTY(data_ptr[0]) )
      {
        appInfo_ptr->sms_rec_tags[accessed_rec_num - 1] = WMS_TAG_NONE;
      }
      else
      {
        appInfo_ptr->sms_rec_tags[accessed_rec_num - 1] = (wms_message_tag_e_type)data_ptr[0];
      }

      /* update voice mail index */
      wms_cfg_update_voice_mail_index( appInfo_ptr,
                                       data_ptr,
                                       (wms_message_index_type)accessed_rec_num - 1);
    }
  }
} /* wms_sim_handle_sms_gw_data() */

/*===========================================================================
FUNCTION: wms_sim_mmgsdi_read_cb_sms_gw_proc()                     (INTERNAL)

DESCRIPTION:
  This function processes an EFsms record when it is read.
  After handling the returned data:
   - the next record is read
     or
   - processing continues with reading the next file
     or
   - the file reading sequence is complete and ready processing starts

RETURN VALUE
  NONE

DEPENDENCIES
  None

CALLED BY:
  wms_sim_mmgsdi_response_cb_proc()
===========================================================================*/
void wms_sim_mmgsdi_read_cb_sms_gw_proc(
  const mmgsdi_cnf_type *cnf_ptr
)
{
  wms_prov_app_info_s_type *appInfo_ptr;
  int                      next_record_num = 0;
  appInfo_ptr = (wms_prov_app_info_s_type *)cnf_ptr->read_cnf.response_header.client_data;

  MSG_HIGH_1("In wms_sim_mmgsdi_read_cb_sms_gw_proc(): accessed_rec_num = %d", cnf_ptr->read_cnf.accessed_rec_num);

  if (NULL == appInfo_ptr)
  {
    return;
  }

  wms_sim_handle_sms_gw_data( appInfo_ptr,
                              cnf_ptr->read_cnf.read_data.data_len,
                              cnf_ptr->read_cnf.read_data.data_ptr,
                              cnf_ptr->read_cnf.accessed_rec_num );

  next_record_num = wms_sim_get_next_used_record(appInfo_ptr, cnf_ptr->read_cnf.accessed_rec_num+1);

  /* read next record if exists */
  if (next_record_num <= appInfo_ptr->sms_rec_count)
  {
    mmgsdi_return_enum_type ret;

    MSG_HIGH_1("Reading EF record num %d", next_record_num);
    appInfo_ptr->accessed_rec_num = next_record_num;
    ret = mmgsdi_session_read_record( cnf_ptr->response_header.session_id,
                                      cnf_ptr->read_cnf.access,
                                      next_record_num,
                                      appInfo_ptr->sms_rec_len,
                                      wms_sim_mmgsdi_response_cb,
                                      (mmgsdi_client_data_type)appInfo_ptr );

    wms_check_status_mmgsdi_session_read_rec(ret);

  }
  else
  {
    /* Finished reading sms records from the card */

    appInfo_ptr->sim_file_init_is_done[SMS_FILE_INDEX] = TRUE;

    /* read next file */
    wms_sim_mmgsdi_read_next_sim_file(appInfo_ptr,
                                      SMSR_FILE_INDEX);

  }
} /* wms_sim_mmgsdi_read_cb_sms_gw_proc() */
#endif /* FEATURE_GWSMS */

void wms_sim_mmgsdi_file_attr_cb_smsr_proc(
  const mmgsdi_cnf_type *cnf_ptr
)
{
  /* cnf_ptr is checked by wms_sim_mmgsdi_response_cb */
  wms_prov_app_info_s_type *appInfo_ptr;

  appInfo_ptr = (wms_prov_app_info_s_type *)cnf_ptr->get_file_attr_cnf.response_header.client_data;

  if (NULL == appInfo_ptr)
  {
    return;
  }

  MSG_HIGH_0("wms_sim_mmgsdi_file_attr_cb_smsr_proc()");

  if ( (cnf_ptr->get_file_attr_cnf.file_attrib.file_size <= 0) ||
       (cnf_ptr->get_file_attr_cnf.file_attrib.file_type != MMGSDI_LINEAR_FIXED_FILE) ||
       (cnf_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.num_of_rec <= 0) )
  {
    MSG_HIGH_3( "EFsmsr is empty: file_size = %d, file_type = %d, num_of_rec = %d",
                cnf_ptr->get_file_attr_cnf.file_attrib.file_size,
                cnf_ptr->get_file_attr_cnf.file_attrib.file_type,
                cnf_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.num_of_rec );


    appInfo_ptr->sim_file_init_is_done[SMSR_FILE_INDEX] = TRUE;
	
    /* read next file */
    wms_sim_mmgsdi_read_next_sim_file(appInfo_ptr,
                                      MWIS_FILE_INDEX);

  }
  else
  {
    /* save info for next record read */
    appInfo_ptr->smsr_rec_count = MIN( WMS_MESSAGE_LIST_MAX,
                                       cnf_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.num_of_rec );

    if (appInfo_ptr->smsr_rec_count > 0)
    {
      appInfo_ptr->smsr_rec_len = cnf_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.rec_len;
      appInfo_ptr->wms_seek_data.status = wms_sim_seek_unused_records( appInfo_ptr,
                                                                       cnf_ptr->response_header.session_id,
                                                                       cnf_ptr->get_file_attr_cnf.access,
                                                                       appInfo_ptr->smsr_rec_count,
                                                                       appInfo_ptr->smsr_rec_len);
      if(WMS_OK_S != appInfo_ptr->wms_seek_data.status)
      {
        /* Seek Failed, fall back to normal procedure */
        mmgsdi_return_enum_type ret;
        MSG_HIGH_0("Couldn't perform Seek, start reading EF sequentially from record num 1");
        appInfo_ptr->accessed_rec_num = 1;
        ret = mmgsdi_session_read_record( cnf_ptr->response_header.session_id,
                                          cnf_ptr->get_file_attr_cnf.access,
                                          1, // starts from 1
                                          appInfo_ptr->smsr_rec_len,
                                          wms_sim_mmgsdi_response_cb,
                                          (mmgsdi_client_data_type)appInfo_ptr );

        wms_check_status_mmgsdi_session_read_rec(ret);

      }
    }
  }
} /* wms_sim_mmgsdi_file_attr_cb_smsr_proc() */

#ifdef FEATURE_GWSMS
/*===========================================================================
FUNCTION: wms_sim_handle_smsp_gw_data()                            (INTERNAL)

DESCRIPTION:
  This function processes the data returned from the EFsmsp file when it is
  read. It is a common function used when the card is being initialized and
  when the notification event arrives that the EFsmsp file has changed.

RETURN VALUE
  NONE

DEPENDENCIES
  None

CALLED BY:
  wms_sim_mmgsdi_read_cb_smsp_gw_proc()
  wms_sim_mmgsdi_file_update_event_proc()
===========================================================================*/
STATIC void wms_sim_handle_smsp_gw_data(
  wms_prov_app_info_s_type *appInfo_ptr,
  mmgsdi_len_type           data_len,         // cnf_ptr->read_cnf.read_data.data_len
  uint8                    *data_ptr,         // cnf_ptr->read_cnf.read_data.data_ptr
  mmgsdi_rec_num_type       accessed_rec_num, // cnf_ptr->read_cnf.accessed_rec_num
  mmgsdi_file_enum_type     file_enum         // cnf_ptr->read_cnf.access.file.file_enum
)
{
  MSG_HIGH_0("In wms_sim_handle_smsp_gw_data()");

  if ( (NULL != appInfo_ptr) &&
       (NULL != data_ptr) )
  {
    if (data_len < WMS_GW_TEMPLATE_MIN)
    {
      MSG_ERROR_1("bad smsp data: data_len = %d", data_len);
    }
    else
    {
      mmgsdi_len_type alpha_id_len;
      mmgsdi_rec_num_type index = accessed_rec_num - 1;

      if ( NULL != appInfo_ptr->smsp_raw_rec[index].data_ptr )
      {
        MSG_HIGH_2( "wms_sim_handle_smsp_gw_data(): deallocate smsp_raw_rec[%d] = 0x%08x",
                    index,
                    appInfo_ptr->smsp_raw_rec[index].data_ptr );

        wms_mem_free((void *)appInfo_ptr->smsp_raw_rec[index].data_ptr);
        appInfo_ptr->smsp_raw_rec[index].data_ptr = NULL;
        appInfo_ptr->smsp_raw_rec[index].data_len = 0;
      }

      appInfo_ptr->smsp_raw_rec[index].data_ptr = (uint8 *)wms_mem_malloc((size_t)data_len);

      if ( NULL != appInfo_ptr->smsp_raw_rec[index].data_ptr )
      {
        MSG_HIGH_3( "wms_sim_handle_smsp_gw_data(): smsp_raw_rec[%d] = 0x%08x, len = %d",
                    index,
                    appInfo_ptr->smsp_raw_rec[index].data_ptr,
                    data_len );

        (void)memscpy( appInfo_ptr->smsp_raw_rec[index].data_ptr,
                       (uint32)data_len,
                       data_ptr,
                       (uint32)data_len );
        appInfo_ptr->smsp_raw_rec[index].data_len = data_len;
      }
      else
      {
        MSG_ERROR_1("malloc failed for smsp_raw_rec[%d]", index);
      }

      alpha_id_len = data_len - WMS_GW_TEMPLATE_MIN;

      /* if parameter indicator = 0xFF, the record is empty */
      if (data_ptr[alpha_id_len] == 0xFF)
      {
        /* rec_num starts from 1 */
        appInfo_ptr->smsp_rec_templates[index] = FALSE;
        MSG_HIGH_1("smsp record #%d is empty", accessed_rec_num);
      }
      else
      {
        alpha_id_len = MIN(alpha_id_len, WMS_ALPHA_ID_MAX);
        appInfo_ptr->smsp_rec_templates[index] = TRUE;

        /* only need to find the 1st sc address */
        if (0 == appInfo_ptr->gw_template.sc_addr.number_of_digits)
        {
          if ( NULL != appInfo_ptr->gw_template.alpha_id.data )
          {
             MSG_HIGH_1( "wms_sim_handle_smsp_gw_data(): deallocate sc address template buff - 0x%08lx",
                         appInfo_ptr->gw_template.alpha_id.data );

             wms_mem_free((void *)appInfo_ptr->gw_template.alpha_id.data);
             appInfo_ptr->gw_template.alpha_id.data = NULL;

          }

          if (alpha_id_len > 0)
          {
            appInfo_ptr->gw_template.alpha_id.data = (uint8 *)wms_mem_malloc((uint32)alpha_id_len);
          }

          if ((alpha_id_len > 0) && (NULL == appInfo_ptr->gw_template.alpha_id.data))
          {
            MSG_ERROR_0("malloc failed");
          }
          else if (FALSE == wms_ts_decode_smsp( data_ptr,
                                                (uint8)data_len,
                                                &appInfo_ptr->gw_template))
          {
            MSG_ERROR_0("wms_ts_decode_smsp failed");
          }
          else
          {
             MSG_HIGH_1("Found template in card. SC address length = %d", appInfo_ptr->gw_template.sc_addr.number_of_digits);
             appInfo_ptr->gw_template_index     = index;
             appInfo_ptr->gw_template_mem_store = (MMGSDI_USIM_SMSP == file_enum ? WMS_MEMORY_STORE_USIM : WMS_MEMORY_STORE_SIM);
          }
        } // 0 == sc_addr.number_of_digits
      } // else non-empty record
    } // else data_len OK
  } // null ptr check
} /* wms_sim_handle_smsp_gw_data() */

/*===========================================================================
FUNCTION: wms_sim_mmgsdi_read_cb_smsp_gw_proc()                    (INTERNAL)

DESCRIPTION:
  This function processes an EFsmsp record when it is read.
  After handling the returned data:
   - the next record is read
     or
   - if no SC addr template was read from the card, the template is read from
     NV, then processing continues with reading the next file (sms)

RETURN VALUE
  NONE

DEPENDENCIES
  None

CALLED BY:
  wms_sim_mmgsdi_response_cb_proc()
===========================================================================*/
void wms_sim_mmgsdi_read_cb_smsp_gw_proc(
  const mmgsdi_cnf_type *cnf_ptr
)
{
  /* cnf_ptr is checked by wms_sim_mmgsdi_response_cb */
  wms_prov_app_info_s_type *appInfo_ptr;
  int                      next_record_num =0;

  appInfo_ptr = (wms_prov_app_info_s_type *)cnf_ptr->read_cnf.response_header.client_data;

  MSG_HIGH_1("In wms_sim_mmgsdi_read_cb_smsp_gw_proc(): accessed_rec_num = %d", cnf_ptr->read_cnf.accessed_rec_num);

  if (NULL == appInfo_ptr)
  {
    return;
  }

  /* Process data for this record */
  wms_sim_handle_smsp_gw_data( appInfo_ptr,
                               cnf_ptr->read_cnf.read_data.data_len,
                               cnf_ptr->read_cnf.read_data.data_ptr,
                               cnf_ptr->read_cnf.accessed_rec_num,
                               cnf_ptr->read_cnf.access.file.file_enum );

   next_record_num = wms_sim_get_next_used_record(appInfo_ptr, cnf_ptr->read_cnf.accessed_rec_num+1);

  /* Read next record if exists */
  if (next_record_num <= appInfo_ptr->smsp_rec_count)
  {
    mmgsdi_return_enum_type ret;
    MSG_HIGH_1("Reading EF record num %d", next_record_num);
    appInfo_ptr->accessed_rec_num = next_record_num;
    ret = mmgsdi_session_read_record( cnf_ptr->response_header.session_id,
                                      cnf_ptr->read_cnf.access,
                                      next_record_num,
                                      appInfo_ptr->smsp_rec_len,
                                      wms_sim_mmgsdi_response_cb,
                                      (mmgsdi_client_data_type)appInfo_ptr );

    wms_check_status_mmgsdi_session_read_rec(ret);
  }
  else
  {
    /* Finished reading EFsmsp records from the card */
    appInfo_ptr->sim_file_init_is_done[SMSP_FILE_INDEX] = TRUE;
	
    /* read next file */
    wms_sim_mmgsdi_read_next_sim_file(appInfo_ptr,
                                      SMS_FILE_INDEX);

  }
} /* wms_sim_mmgsdi_read_cb_smsp_gw_proc() */

#endif /* FEATURE_GWSMS */

/*===========================================================================
FUNCTION: wms_sim_handle_smsr_data()                               (INTERNAL)

DESCRIPTION:
  This function processes the data returned from the EFsmsr file when it is
  read. It is a common function used when the card is being initialized and
  when the notification event arrives that the EFsmsr file has changed.

RETURN VALUE
  NONE

DEPENDENCIES
  None

CALLED BY:
  wms_sim_mmgsdi_read_cb_smsr_proc()
  wms_sim_mmgsdi_file_update_event_proc()
===========================================================================*/
STATIC void wms_sim_handle_smsr_data(
  wms_prov_app_info_s_type *appInfo_ptr,
  mmgsdi_len_type           data_len,        // cnf_ptr->read_cnf.read_data.data_len
  uint8                    *data_ptr,        // cnf_ptr->read_cnf.read_data.data_ptr
  mmgsdi_rec_num_type       accessed_rec_num // cnf_ptr->read_cnf.accessed_rec_num
)
{
  if ( (NULL != appInfo_ptr) &&
       (NULL != data_ptr) )
  {
    if (data_len > 0)
    {
      /* if record indentifier = 0, the record is empty */
      if (data_ptr[0] == 0)
      {
        /* rec_num starts from 1 */
        appInfo_ptr->smsr_status_rpts[accessed_rec_num - 1] = FALSE;
      }
      else
      {
        appInfo_ptr->smsr_status_rpts[accessed_rec_num - 1] = TRUE;
        appInfo_ptr->smsr_sms_index[accessed_rec_num - 1]   = (wms_message_index_type)data_ptr[0];
      }
    }
  }
} /* wms_sim_handle_smsr_data() */

void wms_sim_mmgsdi_read_cb_smsr_proc(
  const mmgsdi_cnf_type *cnf_ptr
)
{
  wms_prov_app_info_s_type *appInfo_ptr;
  int                      next_record_num =0;

  MSG_HIGH_1("In wms_sim_mmgsdi_read_cb_smsr_proc(): accessed_rec_num = %d", cnf_ptr->read_cnf.accessed_rec_num);

  appInfo_ptr = (wms_prov_app_info_s_type *)cnf_ptr->read_cnf.response_header.client_data;

  if (NULL == appInfo_ptr)
  {
    return;
  }

  wms_sim_handle_smsr_data( appInfo_ptr,
                            cnf_ptr->read_cnf.read_data.data_len,
                            cnf_ptr->read_cnf.read_data.data_ptr,
                            cnf_ptr->read_cnf.accessed_rec_num );

  next_record_num = wms_sim_get_next_used_record(appInfo_ptr, cnf_ptr->read_cnf.accessed_rec_num+1);

  /* read next record if exists */
  if (next_record_num <= appInfo_ptr->smsr_rec_count)
  {
    mmgsdi_return_enum_type ret;
    MSG_HIGH_1("Reading EF record num %d", next_record_num);
    appInfo_ptr->accessed_rec_num = next_record_num;
    ret = mmgsdi_session_read_record( cnf_ptr->response_header.session_id,
                                      cnf_ptr->read_cnf.access,
                                      next_record_num,
                                      appInfo_ptr->smsr_rec_len,
                                      wms_sim_mmgsdi_response_cb,
                                      (mmgsdi_client_data_type)appInfo_ptr );

    wms_check_status_mmgsdi_session_read_rec(ret);

  }
  else
  {
    /* read next file */    
    appInfo_ptr->sim_file_init_is_done[SMSR_FILE_INDEX] = TRUE;
	
    /* read next file */
    wms_sim_mmgsdi_read_next_sim_file(appInfo_ptr,
                                      MWIS_FILE_INDEX);

  }
} /* wms_sim_mmgsdi_read_cb_smsr_proc() */

/*===========================================================================
FUNCTION: wms_sim_handle_mwis_data()                               (INTERNAL)

DESCRIPTION:
  This function processes the data returned from the EFmwis file when it is
  read. It is a common function used when the card is being initialized and
  when the refresh occurs.

RETURN VALUE
  NONE

DEPENDENCIES
  None

CALLED BY:
  wms_sim_mmgsdi_read_cb_mwis_proc()
===========================================================================*/
STATIC void wms_sim_handle_mwi_data(
  wms_prov_app_info_s_type         *appInfo_ptr,
  const mmgsdi_read_cnf_type       *cnf_data
)
{
  uint8  state = WMS_MWI_STATE_INIT;

  if (MMGSDI_SUCCESS != cnf_data->response_header.mmgsdi_status)
  {
    /* Read of EF-MWIS failed.  Set the MWI cache to error state. */
    state = WMS_MWI_STATE_ERROR;   //UPDATE: to wms global
  }

  wms_cfg_notify_mwi_data(cnf_data->read_data.data_ptr, appInfo_ptr, state);

} /* wms_sim_handle_mwis_data() */

/*===========================================================================
FUNCTION: wms_sim_mmgsdi_read_cb_mwis_proc()                       (INTERNAL)

DESCRIPTION:
  This function gets the MWI data from EF_MWIS and notifies the client.

  It also notifies the client with Ready Event(GW_READY and MS_READY)
  if all the SMS files have been read.

RETURN VALUE
  NONE

DEPENDENCIES
  None

CALLED WHEN:
  After getting read confirmation event from MMGSDI for EF_MWIS
===========================================================================*/
void wms_sim_mmgsdi_read_cb_mwis_proc(
  const mmgsdi_cnf_type *cnf_ptr
)
{
  wms_prov_app_info_s_type *appInfo_ptr = (wms_prov_app_info_s_type *)cnf_ptr->read_cnf.response_header.client_data;

  MSG_HIGH_0("In wms_sim_mmgsdi_read_cb_mwis_proc()");

  if (NULL == appInfo_ptr)
  {
    return;
  }

  wms_sim_handle_mwi_data( appInfo_ptr, &cnf_ptr->read_cnf );
  appInfo_ptr->sim_file_init_is_done[MWIS_FILE_INDEX] = TRUE;

  /* read next file */
  wms_sim_mmgsdi_read_next_sim_file(appInfo_ptr,
                                    MAX_FILE_INDEX);      	   

} /* wms_sim_mmgsdi_read_cb_mwis_proc */

wms_memory_store_e_type wms_sim_mmgsdi_get_supproted_mem_store(
  mmgsdi_app_enum_type app_type
)
{
  wms_memory_store_e_type mem_store = WMS_MEMORY_STORE_NONE;

  switch(app_type)
  {
  case MMGSDI_APP_SIM:
  case MMGSDI_APP_USIM:
    mem_store = WMS_MEMORY_STORE_SIM;
    break;

  case MMGSDI_APP_CSIM:
  case MMGSDI_APP_RUIM:

    mem_store = WMS_MEMORY_STORE_RUIM;
    break;

  default:
    break;
  }

  return mem_store;
}

boolean wms_sim_mmgsdi_mem_store_supported(
   mmgsdi_app_enum_type app_type,
   wms_memory_store_e_type mem_store
)
{
   boolean supported = FALSE;

   if ((MMGSDI_APP_SIM  == app_type) ||
       (MMGSDI_APP_USIM == app_type))
   {
      if ((WMS_MEMORY_STORE_SIM  == mem_store) ||
          (WMS_MEMORY_STORE_USIM == mem_store))
      {
         supported = TRUE;
      }
   }
   else if ((MMGSDI_APP_CSIM == app_type) ||
            (MMGSDI_APP_RUIM == app_type))
   {
      if ((WMS_MEMORY_STORE_CSIM  == mem_store) ||
          (WMS_MEMORY_STORE_RUIM  == mem_store))
      {
         supported = TRUE;
      }
   }
   else
   {
      MSG_ERROR_2( "Unsupported app_type %d mem_store %d",
                   app_type,
                   mem_store );
   }

   return supported;
}

#ifdef FEATURE_GWSMS
void wms_read_gw_template_from_nv
(
   wms_gw_template_s_type *template_ptr,
   sys_modem_as_id_e_type as_id
)
{
   if ( NULL != template_ptr->alpha_id.data )
   {
      MSG_HIGH_1( "In wms_read_gw_template_from_nv(): deallocate sc address template buff - 0x%08lx",
                  template_ptr->alpha_id.data);

      wms_mem_free((void *)template_ptr->alpha_id.data);
      template_ptr->alpha_id.data = NULL;
   }

   template_ptr->alpha_id.data = wms_mem_malloc(WMS_ALPHA_ID_MAX);
   if (NULL == template_ptr->alpha_id.data)
   {
      MSG_ERROR_0("malloc failed");
   }
   else if( wms_nv_read_gw_params(template_ptr, as_id) == FALSE )
   {
     MSG_ERROR_0("wms_nv_read_gw_params failed");
   }
}
#endif

/*===========================================================================
FUNCTION: wms_sim_handle_smsp_cdma_data()                          (INTERNAL)

DESCRIPTION:
  This function processes the data returned from the EFsmsp file when it is
  read. It is a common function used when the card is being initialized and
  when the notification event arrives that the EFsmsp file has changed.

RETURN VALUE
  NONE

DEPENDENCIES
  None

CALLED BY:
  wms_sim_mmgsdi_read_cb_smsp_cdma_proc()
  wms_sim_mmgsdi_file_update_event_proc()
===========================================================================*/
STATIC void wms_sim_handle_smsp_cdma_data(
  wms_prov_app_info_s_type *appInfo_ptr,
  mmgsdi_len_type           data_len,        // cnf_ptr->read_cnf.read_data.data_len
  uint8                    *data_ptr,        // cnf_ptr->read_cnf.read_data.data_ptr
  mmgsdi_rec_num_type       accessed_rec_num // cnf_ptr->read_cnf.accessed_rec_num
)
{
  if ( (NULL != appInfo_ptr) &&
       (NULL != data_ptr) )
  {
    mmgsdi_rec_num_type index = accessed_rec_num - 1;

    if ( NULL != appInfo_ptr->smsp_raw_rec[index].data_ptr )
    {
      MSG_HIGH_2( "wms_sim_handle_smsp_cdma_data(): deallocate smsp_raw_rec[%d] = 0x%08x",
                  index,
                  appInfo_ptr->smsp_raw_rec[index].data_ptr );

      wms_mem_free((void *)appInfo_ptr->smsp_raw_rec[index].data_ptr);
      appInfo_ptr->smsp_raw_rec[index].data_ptr = NULL;
      appInfo_ptr->smsp_raw_rec[index].data_len = 0;
    }

    appInfo_ptr->smsp_raw_rec[index].data_ptr = (uint8 *)wms_mem_malloc((size_t)data_len);

    if ( NULL != appInfo_ptr->smsp_raw_rec[index].data_ptr )
    {
      MSG_HIGH_3( "wms_sim_handle_smsp_cdma_data(): smsp_raw_rec[%d] = 0x%08x, len = %d",
                  index,
                  appInfo_ptr->smsp_raw_rec[index].data_ptr,
                  data_len );

      (void)memscpy( appInfo_ptr->smsp_raw_rec[index].data_ptr,
                     (uint32)data_len,
                     data_ptr,
                     (uint32)data_len );
      appInfo_ptr->smsp_raw_rec[index].data_len = data_len;
    }
    else
    {
      MSG_ERROR_1("malloc failed for wms_sim_handle_smsp_cdma_data[%d]", index);
    }

    if (data_len <= 0)
    {
      MSG_ERROR_1("bad smsp data: data_len = %d", data_len);
    }
    else
    {
      if (WMS_RUIM_SMSP_EMPTY_CHAR != data_ptr[0])
      {
        appInfo_ptr->smsp_rec_templates[index] = TRUE;
      }
    }
  }
} /* wms_sim_handle_smsp_cdma_data() */

/*===========================================================================
FUNCTION: wms_sim_mmgsdi_read_cb_smsp_cdma_proc()                  (INTERNAL)

DESCRIPTION:
  This function processes an EFsmsp record when it is read.
  After handling the returned data:
   - the next record is read
     or
   - processing continues with reading the next file (sms)

RETURN VALUE
  NONE

DEPENDENCIES
  None

CALLED BY:
  wms_sim_mmgsdi_response_cb_proc()
===========================================================================*/
void wms_sim_mmgsdi_read_cb_smsp_cdma_proc(
  const mmgsdi_cnf_type *cnf_ptr
)
{
  /* cnf_ptr is checked by wms_sim_mmgsdi_response_cb */
  wms_prov_app_info_s_type *appInfo_ptr;
  int                      next_record_num =0;

  MSG_HIGH_1("In wms_sim_mmgsdi_read_cb_smsp_cdma_proc(): accessed_rec_num = %d", cnf_ptr->read_cnf.accessed_rec_num);

  appInfo_ptr = (wms_prov_app_info_s_type *)cnf_ptr->read_cnf.response_header.client_data;

  if (NULL == appInfo_ptr)
  {
    return;
  }

  wms_sim_handle_smsp_cdma_data( appInfo_ptr,
                                 cnf_ptr->read_cnf.read_data.data_len,
                                 cnf_ptr->read_cnf.read_data.data_ptr,
                                 cnf_ptr->read_cnf.accessed_rec_num);

  next_record_num = wms_sim_get_next_used_record(appInfo_ptr, cnf_ptr->read_cnf.accessed_rec_num+1);

  /* Read next record if exists */
  if (next_record_num <= appInfo_ptr->smsp_rec_count)
  {
    mmgsdi_return_enum_type ret;
    MSG_HIGH_1("Reading EF record num %d", next_record_num);
    appInfo_ptr->accessed_rec_num = next_record_num;
    ret = mmgsdi_session_read_record( cnf_ptr->response_header.session_id,
                                      cnf_ptr->read_cnf.access,
                                      next_record_num,
                                      appInfo_ptr->smsp_rec_len,
                                      wms_sim_mmgsdi_response_cb,
                                      (mmgsdi_client_data_type)appInfo_ptr );

    wms_check_status_mmgsdi_session_read_rec(ret);
  }
  else
  {
    /* Finished reading smsp records from the card */
    appInfo_ptr->sim_file_init_is_done[SMSP_FILE_INDEX] = TRUE;

    /* read next file */
    wms_sim_mmgsdi_read_next_sim_file(appInfo_ptr,
                                      SMS_FILE_INDEX);

  }
} /* wms_sim_mmgsdi_read_cb_smsp_cdma_proc() */

/*===========================================================================
FUNCTION: wms_sim_handle_smss_cdma_data()                          (INTERNAL)

DESCRIPTION:
  This function processes the data returned from the EFsmss file when it is
  read. It is a common function used when the card is being initialized and
  when the notification event arrives that the EFsmss file has changed.

RETURN VALUE
  NONE

DEPENDENCIES
  None

CALLED BY:
  wms_sim_mmgsdi_read_cb_smss_cdma_proc()
  wms_sim_mmgsdi_file_update_event_proc()
===========================================================================*/
STATIC void wms_sim_handle_smss_cdma_data(
  wms_prov_app_info_s_type *appInfo_ptr,
  mmgsdi_len_type           data_len,        // cnf_ptr->read_cnf.read_data.data_len
  uint8                    *data_ptr         // cnf_ptr->read_cnf.read_data.data_ptr
)
{
  if ( (NULL != appInfo_ptr) &&
       (NULL != data_ptr) )
  {
    if (data_len < 5)
    {
      MSG_ERROR_1("bad smss data: data_len = %d", data_len);
    }
    else
    {
      appInfo_ptr->smss_last_message_number     = (data_ptr[0] << 8) | (data_ptr[1]);
      appInfo_ptr->smss_last_wap_message_number = (data_ptr[2] << 8) | (data_ptr[3]);
      appInfo_ptr->smss_mem_full_flag           = data_ptr[4];
    }
  }
}

/*===========================================================================
FUNCTION: wms_sim_mmgsdi_read_cb_smss_cdma_proc()                  (INTERNAL)

DESCRIPTION:
  This function processes EFsmss data when it is read.
  After handling the returned data:
   - processing continues with reading the next file (smsp)

RETURN VALUE
  NONE

DEPENDENCIES
  None

CALLED BY:
  wms_sim_mmgsdi_response_cb_proc()
===========================================================================*/
void wms_sim_mmgsdi_read_cb_smss_cdma_proc(
  const mmgsdi_cnf_type *cnf_ptr
)
{
   /* cnf_ptr is checked by wms_sim_mmgsdi_response_cb */
  wms_prov_app_info_s_type *appInfo_ptr;

  appInfo_ptr = (wms_prov_app_info_s_type *)cnf_ptr->read_cnf.response_header.client_data;

  MSG_HIGH_0("In wms_sim_mmgsdi_read_cb_smss_cdma_proc()");

  if (NULL == appInfo_ptr)
  {
    return;
  }

  if (!appInfo_ptr->sim_file_init_is_done[SMSS_FILE_INDEX])
  {
    wms_sim_handle_smss_cdma_data( appInfo_ptr,
                                   cnf_ptr->read_cnf.read_data.data_len,
                                   cnf_ptr->read_cnf.read_data.data_ptr);

  }

  /* read next file */  
  appInfo_ptr->sim_file_init_is_done[SMSS_FILE_INDEX] = TRUE;
  
  /* read next file */
  wms_sim_mmgsdi_read_next_sim_file(appInfo_ptr,
                                    SMSP_FILE_INDEX);

 
} /* wms_sim_mmgsdi_read_cb_smss_cdma_proc() */

/*===========================================================================
FUNCTION: wms_sim_handle_sms_cdma_data()                           (INTERNAL)

DESCRIPTION:
  This function processes the data returned from the EFsms file when it is
  read. It is a common function used when the card is being initialized and
  when the notification event arrives that the EFsms file has changed.

RETURN VALUE
  NONE

DEPENDENCIES
  None

CALLED BY:
  wms_sim_mmgsdi_read_cb_sms_cdma_proc()
  wms_sim_mmgsdi_file_update_event_proc()
===========================================================================*/
STATIC void wms_sim_handle_sms_cdma_data(
  wms_prov_app_info_s_type *appInfo_ptr,
  mmgsdi_len_type           data_len,        // cnf_ptr->read_cnf.read_data.data_len
  uint8                    *data_ptr,        // cnf_ptr->read_cnf.read_data.data_ptr
  mmgsdi_rec_num_type       accessed_rec_num // cnf_ptr->read_cnf.accessed_rec_num
)
{
  if ( (NULL != appInfo_ptr) &&
       (NULL != data_ptr) )
  {
    if (data_len > 0)
    {
      if ( WMS_IS_EFSMS_REC_EMPTY(data_ptr[0]) )
      {
        appInfo_ptr->sms_rec_tags[accessed_rec_num - 1] = WMS_TAG_NONE;
      }
      else
      {
        appInfo_ptr->sms_rec_tags[accessed_rec_num - 1] = (wms_message_tag_e_type)data_ptr[0];
      }
      /* update the duplicate detection cache if message is MT */
    }
  }
} /* wms_sim_handle_sms_cdma_data() */

/*===========================================================================
FUNCTION: wms_sim_mmgsdi_read_cb_sms_cdma_proc()                   (INTERNAL)

DESCRIPTION:
  This function processes an EFsms record when it is read.
  After handling the returned data:
   - the next record is read
     or
   - the file reading sequence is complete and ready processing starts

RETURN VALUE
  NONE

DEPENDENCIES
  None

CALLED BY:
  wms_sim_mmgsdi_response_cb_proc()
===========================================================================*/
void wms_sim_mmgsdi_read_cb_sms_cdma_proc(
  const mmgsdi_cnf_type *cnf_ptr
)
{
  wms_prov_app_info_s_type *appInfo_ptr;
  int                       next_record_num = 0;

  MSG_HIGH_1("In wms_sim_mmgsdi_read_cb_sms_cdma_proc(): accessed_rec_num = %d", cnf_ptr->read_cnf.accessed_rec_num);

  appInfo_ptr = (wms_prov_app_info_s_type *)cnf_ptr->read_cnf.response_header.client_data;


  if (NULL == appInfo_ptr)
  {
    return;
  }

  wms_sim_handle_sms_cdma_data( appInfo_ptr,
                                cnf_ptr->read_cnf.read_data.data_len,
                                cnf_ptr->read_cnf.read_data.data_ptr,
                                cnf_ptr->read_cnf.accessed_rec_num);

  next_record_num = wms_sim_get_next_used_record(appInfo_ptr, cnf_ptr->read_cnf.accessed_rec_num + 1);

  /* Read next record if exists */
  if (next_record_num <= appInfo_ptr->sms_rec_count)
  {
    mmgsdi_return_enum_type ret;

    MSG_HIGH_1("Reading EF record num %d", next_record_num);
    appInfo_ptr->accessed_rec_num = next_record_num;
    ret = mmgsdi_session_read_record( cnf_ptr->response_header.session_id,
                                      cnf_ptr->read_cnf.access,
                                      next_record_num,
                                      appInfo_ptr->sms_rec_len,
                                      wms_sim_mmgsdi_response_cb,
                                      (mmgsdi_client_data_type)appInfo_ptr );

    wms_check_status_mmgsdi_session_read_rec(ret);
  }
  else
  {
    /* Finished reading sms records from the card */
    appInfo_ptr->sim_file_init_is_done[SMS_FILE_INDEX] = TRUE;

    /* read next file */
    wms_sim_mmgsdi_read_next_sim_file(appInfo_ptr,
                                      SMSR_FILE_INDEX);

  }
} /* wms_sim_mmgsdi_read_cb_sms_cdma_proc() */

#if defined(FEATURE_QSC1215_SGLTE_FUSION) || defined(FEATURE_8960_SGLTE_FUSION)
/*===========================================================================
FUNCTION: wms_sim_mmgsdi_file_update_event_proc()                  (INTERNAL)

DESCRIPTION:
  This function processes the file update events sent from MMGSDI when a
  file is modified.

RETURN VALUE
  NONE

DEPENDENCIES
  None

CALLED BY:
  wms_sim_mmgsdi_event_cb_proc()
===========================================================================*/
void wms_sim_mmgsdi_file_update_event_proc(
  mmgsdi_event_data_type   *event,
  wms_prov_app_info_s_type *appInfo_ptr
)
{
  mmgsdi_file_update_evt_info_type *update_info;
  int                               file_data_len;
  uint8                            *file_data_ptr;
  mmgsdi_file_structure_enum_type   file_type;

  update_info = &event->data.file_update;

  file_data_len = update_info->file_data.data_len;
  file_data_ptr = update_info->file_data.data_ptr;
  file_type     = update_info->file_type;

  MSG_HIGH_3( "Processing file update for file: 0x%x, type=%d, data_len=%d",
              update_info->file.file.file_enum,
              file_type,
              file_data_len );

  if (MMGSDI_TRANSPARENT_FILE == file_type)
  {
    mmgsdi_offset_type file_offset = (mmgsdi_offset_type)update_info->file_index.offset;

    MSG_HIGH_1("TRANSPARENT update at offset: %d", file_offset);

    switch (update_info->file.file.file_enum)
    {
#ifdef FEATURE_GWSMS
    case MMGSDI_TELECOM_SMSS:
    case MMGSDI_USIM_SMSS:
      wms_sim_handle_smss_gw_data(appInfo_ptr, file_data_len, file_data_ptr);
      break;
#endif /* FEATURE_GWSMS */
#ifdef FEATURE_CDSMS
    case MMGSDI_CDMA_SMS_STATUS:
    case MMGSDI_CSIM_SMS_STATUS:
      wms_sim_handle_smss_cdma_data(appInfo_ptr, file_data_len, file_data_ptr);
      break;
#endif /* FEATURE_CDSMS */
    default:
      MSG_ERROR_1( "Unhandled file update for TRANSPARENT file: 0x%x", update_info->file.file.file_enum);
      break;
    }
  }
  else if ( (MMGSDI_LINEAR_FIXED_FILE == file_type) ||
            (MMGSDI_CYCLIC_FILE == file_type) )
  {
    mmgsdi_rec_num_type rec_num = (mmgsdi_rec_num_type)update_info->file_index.rec_num;

    MSG_HIGH_1("LIN/CYC update at record: %d", rec_num);

    switch (update_info->file.file.file_enum)
    {
#ifdef FEATURE_GWSMS
    case MMGSDI_TELECOM_SMS:
    case MMGSDI_USIM_SMS:
      wms_sim_handle_sms_gw_data(appInfo_ptr, file_data_len, file_data_ptr, rec_num);
      break;
    case MMGSDI_TELECOM_SMSP:
    case MMGSDI_USIM_SMSP:
      wms_sim_handle_smsp_gw_data(appInfo_ptr, file_data_len, file_data_ptr, rec_num, update_info->file.file.file_enum);
      break;
    case MMGSDI_TELECOM_SMSR:
    case MMGSDI_USIM_SMSR:
      wms_sim_handle_smsr_data(appInfo_ptr, file_data_len, file_data_ptr, rec_num);
      break;
    case MMGSDI_GSM_MWIS:
    case MMGSDI_USIM_MWIS:
      // No data updated
      break;
    case MMGSDI_GSM_CBMI:
    case MMGSDI_USIM_CBMI:
      wms_sim_handle_cbmi_data(appInfo_ptr, file_data_len, file_data_ptr);
      break;
    case MMGSDI_GSM_CBMID:
    case MMGSDI_USIM_CBMID:
      wms_sim_handle_cbmid_data(appInfo_ptr, file_data_len, file_data_ptr);
      break;
    case MMGSDI_GSM_CBMIR:
    case MMGSDI_USIM_CBMIR:
      wms_sim_handle_cbmir_data(appInfo_ptr, file_data_len, file_data_ptr);
      break;
#endif /* FEATURE_GWSMS */
#ifdef FEATURE_CDSMS
    case MMGSDI_CDMA_SMS:
    case MMGSDI_CSIM_SMS:
      wms_sim_handle_sms_cdma_data(appInfo_ptr, file_data_len, file_data_ptr, rec_num);
      break;
    case MMGSDI_CDMA_SMS_PARAMS:
    case MMGSDI_CSIM_SMS_PARAMS:
      wms_sim_handle_smsp_cdma_data(appInfo_ptr, file_data_len, file_data_ptr, rec_num);
      break;
#endif /* FEATURE_CDSMS */
    default:
      MSG_ERROR_1( "Unhandled file update for LINEAR/CYCLIC file: 0x%x", update_info->file.file.file_enum);
      break;
    } // switch file_enum
  } // linear/cyclic file
  else
  {
    MSG_ERROR_1( "Unhandled file type: %d", update_info->file_type);
  }
} /* wms_sim_mmgsdi_file_update_event_proc() */
#endif /* defined(FEATURE_QSC1215_SGLTE_FUSION) || defined(FEATURE_8960_SGLTE_FUSION) */

#ifdef FEATURE_CDSMS
void wms_read_cdma_template_from_nv(wms_cdma_template_s_type *template_ptr)
{
   uint8   nv_data[NV_SMS_CD_PARMS_SIZE];

   if (TRUE == wms_nv_read_cdma_params(0, nv_data))
   {
      if (FALSE == wms_ts_cdma_decode_smsp(nv_data, sizeof(nv_data), template_ptr))
      {
         MSG_ERROR_0("_ts_cdma_decode_smsp failed");
      }
   }
   else
   {
      MSG_ERROR_0("_nv_read_cdma_param failed");
   }
}
#endif
