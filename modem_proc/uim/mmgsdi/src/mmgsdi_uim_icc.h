#ifndef MMGSDI_UIM_ICC_H
#define MMGSDI_UIM_ICC_H
/*===========================================================================


           M M G S D I   U I M   I C C   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2008, 2010 - 2014 QUALCOMM Technologies, Inc (QTI) and its 
licensors.  All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software is regulated by the U.S. Government. 
Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_uim_icc.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
01/21/14   yt      Remove duplicate code
10/02/13   yt      Remove duplicate code
08/07/13   tl      Add CDMA AKA Authenticate functionality
04/16/13   tl      Clean up of radio access features from MMGSDI code
09/03/12   bcho    Modifiy mmgsdi_uim_icc_search_record for sync operation
05/17/11   yt      Added support for INCREASE request
05/19/10   nb      Updated prototype for mmgsdi_uim_icc_write_transparent
08/20/08   jk      Support for Generate Key API
06/12/08   ssr     Support of MMGSDI Search functionality
03/06/08   nk      Updated mmgsdi_uim_icc_select and mmgsdi_uim_icc_rehabilitate
                   to support synchronous call
08/15/07   jk      Changes to Support DATA Authentication MMGSDI API 
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
05/23/06   tml     Rehabilitate and invalidate supports
12/14/05   tml     MMGSDI Cleanup
11/03/05   tml     fixed header
08/30/05   jar     Added support for ICC Status Commands
07/26/05   sst     Added MMGSDIBT support
06/16/05   tml     Prototype change for transparent and record access
02/24/05   jk      Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "uim_variation.h"
#include "mmgsdi.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/


/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/


/*=============================================================================

                                   MMGSDI MACROS
                        MACRO Definitions used through MMGSDI

=============================================================================*/



/*=============================================================================

                          FUNCTIONS

=============================================================================*/
/* =============================================================================
   FUNCTION:      MMGSDI_UIM_ICC_SELECT

   DESCRIPTION:
     This function will format the get file attribute request in the form
     required by the UIM.

     Behavior for the function for the following file types:

     TRANSPARENT FILE:  Will get information on the characteristics of
                        Transparent file
     CYCLIC FILE:       Will get information on the characteristics of
                        Cyclic file.
     LINEAR FIXED FILE: Will get information on the characteristics of
                        Linear Fixed file.

   DEPENDENCIES:
     The technology type (GSM/CDMA/UICC) has to be dtermined prior to using this
     function


   LIMITATIONS:
     This function is limited to the use of ICC ie. (technologies of type GSM
     and CDMA only). This function is also limited to the use of EFs.


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The funtion was successful in getting the file
                              attributes of a particular file.
     MMGSDI_ERROR:
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                              within appropriate ranges.

   SIDE EFFECTS:
     None

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_icc_select (
  const mmgsdi_get_file_attr_req_type  *req_p,
  boolean                               sync_process,
  mmgsdi_get_file_attr_cnf_type       **cnf_pptr
);


/* =============================================================================
   FUNCTION:      MMGSDI_UIM_ICC_WRITE_TRANSPARENT

   DESCRIPTION:
     This function package the Write Transparent command and send it to 
     UIM for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_write_transparent should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_icc_write_transparent (
  const mmgsdi_write_req_type *  req_ptr,
  boolean                        sync_process,
  mmgsdi_write_cnf_type       ** cnf_pptr
);


/* =============================================================================
   FUNCTION:      MMGSDI_UIM_ICC_WRITE_RECORD

   DESCRIPTION:
     This function package the Write Record command and send it to 
     UIM for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_write_record should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_icc_write_record (
  const mmgsdi_write_req_type *  req_ptr
);


/* =============================================================================
   FUNCTION:      MMGSDI_UIM_ICC_READ_TRANSPARENT

   DESCRIPTION:
     This function package the Read Transparent command and send it to UIM 
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_read_transparent should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_icc_read_transparent (
  const mmgsdi_read_req_type *  req_ptr,
  boolean                       sync_process,
  mmgsdi_read_cnf_type       ** cnf_pptr
);


/* =============================================================================
   FUNCTION:      MMGSDI_UIM_ICC_READ_RECORD

   DESCRIPTION:
     This function package the Read Record command and send it to UIM 
     for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_read_transparent should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_icc_read_record (
  const mmgsdi_read_req_type *  req_ptr,
  boolean                       sync_process,
  mmgsdi_read_cnf_type       ** cnf_pptr
);


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_INCREASE

   DESCRIPTION:
     This function package the Increase command and send it to
     UIM for further processing

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uim_uicc_increase should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_increase (
  const mmgsdi_increase_req_type *  req_ptr
);


/*===========================================================================
FUNCTION:  MMGSDI_UIM_ICC_SEND_APDU

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_icc_send_apdu(
  mmgsdi_send_apdu_req_type* req_ptr);


/* =============================================================================
   FUNCTION:      MMGSDI_UIM_ICC_STATUS

   DESCRIPTION
     This function is called to Queue the command to the UIM for
     a Status command.

   DEPENDENCIES:


   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_icc_status(
  const mmgsdi_status_req_type        *  req_ptr
);


/* =============================================================================
   FUNCTION:      MMGSDI_UIM_ICC_REHABILITATE

   DESCRIPTION:
     This function will rehabilitate a file


   DEPENDENCIES:
     The technology type (GSM/CDMA/UICC) has to be dtermined prior to using this
     function


   LIMITATIONS:
     This function is limited to the use of ICC ie. (technologies of type GSM
     and CDMA only). 

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The function was successful queue the rehabilitate
                              or invalidate the BDN file
     MMGSDI_ERROR:
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                              within appropriate ranges.

   SIDE EFFECTS:
  None

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_icc_rehabilitate (
  const mmgsdi_set_file_status_req_type *  req_ptr,
  boolean                                  sync_process,
  mmgsdi_rehabilitate_cnf_type          ** cnf_pptr
);


/* =============================================================================
   FUNCTION:      MMGSDI_UIM_ICC_INVALIDATE

   DESCRIPTION:
     This function will invalidate a file


   DEPENDENCIES:
     The technology type (GSM/CDMA/UICC) has to be dtermined prior to using this
     function


   LIMITATIONS:
     This function is limited to the use of ICC ie. (technologies of type GSM
     and CDMA only). 

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The function was successful queue the invalidate 
                              command
     MMGSDI_ERROR:
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                              within appropriate ranges.

   SIDE EFFECTS:
  None

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_icc_invalidate (
  const mmgsdi_set_file_status_req_type *  req_ptr
);

/* --------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_SEARCH_RECORD

   DESCRIPTION:
     This function package the Search Record command and send it to UIM 
     for further processing

   DEPENDENCIES:

   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_search_record should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS          : The command processing was successful.
     MMGSDI_ERROR            : The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                               within appropriate ranges.
     MMGSDI_NOT_SUPPORTED    : When Search Type is not valid for ICC Card.

   SIDE EFFECTS:

----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_search_record (
  const mmgsdi_search_req_type             * req_ptr,
  boolean                                    sync_process,
  mmgsdi_search_cnf_type                  ** cnf_pptr
);

/* --------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_RUN_GSM_ALGO

   DESCRIPTION:
     This function package the Run GSM algorithm command and send it to UIM 
     for further processing

   DEPENDENCIES:
     None

   LIMITATIONS:
     This function can be used only in case of ICC Card

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS          : The command processing was successful.
     MMGSDI_ERROR            : The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                               within appropriate ranges.

   SIDE EFFECTS:
     None
----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_run_gsm_algo (
  const mmgsdi_session_run_gsm_algo_req_type       * req_ptr
);

#endif /* MMGSDI_UIM_ICC_H */
