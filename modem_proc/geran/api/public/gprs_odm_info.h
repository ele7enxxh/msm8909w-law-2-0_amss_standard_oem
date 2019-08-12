#ifndef GPRS_ODM_INFO_H
#define GPRS_ODM_INFO_H
/*****************************************************************************
***
*** TITLE
***
***  GPRS ODM INFORMATION INTERFACE
***
***  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/public/gprs_odm_info.h#1 $
***
*** DESCRIPTION
***
***  Provides APIs to extract information from the GSM/GPRS Access Stratum
***
*** FUNCTION SUMMARY
***
***  gprs_odm_info_get_rach_counter()
***
***    Returns the RACH attempts counter. This counter is incremented by 5 each time a
***    RACH attempt fails (i.e. no IA or IARej received), and decremented by 1 each time
***    a RACH attempt is successful
***
***  gprs_odm_info_get_num_of_neighbour_cells()
***
***    Returns the number of neighbour cells (i.e. number of cells in the BA(GPRS) list)
***
***  gprs_odm_info_get_paging_multiframe()
***
***    Returns a value in the range 2 to 9, which is BS_PA_MFRMS from the Control Channel
***    Desription IE
***
***  gprs_odm_info_get_rx_lev_dBm()
***
***    Returns the current Rx level in dBm. Range -48 to -111.
***
***  gprs_odm_info_get_rx_lev_status()
***
***    Returns the current Rx level as 0=good, 1=bad
***
***  gprs_odm_info_get_pbcch_status()
***
***    Returns: 0 - PBCCH not present
***             1 - PBCCH present, but MS camped on BCCH
***             2 - MS camped on PBCCH
***
***  gprs_odm_info_is_assigned_tbf_egprs()
***
***    Returns: TRUE - EGPRS TBF currently assigned
***             FALSE - No TBF or GPRS TBF currently assigned
***
***  gprs_odm_info_get_cell_type()
***
***    Returns the cell capability of the cell currently camped onto
***             ODM_CELL_TYPE_UNKNOWN - out of service
***             ODM_CELL_TYPE_GSM     - camped on a GSM-only cell
***             ODM_CELL_TYPE_GPRS    - camped on a cell that supports GPRS
***             ODM_CELL_TYPE_EGPRS   - camped on a cell that supports EGPRS
***
***  gprs_odm_register_cell_type_cb()
***
***    Register a call-back function (of type gprs_odm_cell_type_cb_fn_t) which will
***    be called each time the MS reselects to a cell of a different cell capability
***    (or the advertised cell capability of the current cell changes)
***    Note: Only one call-back function can be registered. A subsequent registration
***    will over-write any previous registration
***
***  gprs_odm_deregister_cell_type_cb()
***
***    Deregister the call-back function.
***
***
*** Copyright (c) 2005-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or
*** software is regulated by the U.S. Government. Diversion contrary to U.S.
*** law prohibited.
***
*****************************************************************************/

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#ifndef COMDEF_H
  #include "comdef.h"
#endif


/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

extern uint8 gprs_odm_info_get_rach_counter( void );
extern uint8 gprs_odm_info_get_num_of_neighbour_cells( void );
extern uint8 gprs_odm_info_get_paging_multiframe( void );
extern  int8 gprs_odm_info_get_rx_lev_dBm( void );
extern uint8 gprs_odm_info_get_rx_lev_status( void );
extern uint8 gprs_odm_info_get_pbcch_status( void );
extern boolean gprs_odm_info_is_assigned_tbf_egprs( void );

/* Cell Type */
typedef enum
{
  ODM_CELL_TYPE_UNKNOWN,
  ODM_CELL_TYPE_GSM,
  ODM_CELL_TYPE_GPRS,
  ODM_CELL_TYPE_EGPRS

} gprs_odm_cell_type_t;

extern gprs_odm_cell_type_t gprs_odm_info_get_cell_type( void );

/* Cell type change callback function pointer type (1 parameter) */
typedef void (*gprs_odm_cell_type_cb_fn_t)(gprs_odm_cell_type_t cell_type);

extern void gprs_odm_register_cell_type_cb( gprs_odm_cell_type_cb_fn_t );
extern void gprs_odm_deregister_cell_type_cb( void );

/*
** A PLMN ID is stored in 3 octets as specified in 3G TS 24.008 Section 10.5.1.3
**                                        Bit
**                 |  8  |  7  |  6  |  5  |  4  |  3  |  2  |  1  |
**                 |===============================================|
**         octet 1 |      MCC Digit 2      |      MCC Digit 1      |
**                 |-----------------------------------------------|
**         octet 2 |      MNC Digit 3      |      MCC Digit 3      |
**                 |-----------------------------------------------|
**         octet 3 |      MNC Digit 2      |      MNC Digit 1      |
**                 |===============================================|
**/
typedef struct
{
  uint8                  identity[3];

}gprs_odm_plmn_id_t;

typedef uint16 gprs_odm_lac_t;

/* The MAX number of LACs could be detected belongs to one PLMN. */
/* customer can change this number */
#define GPRS_ODM_MAX_NUM_OF_PLMN_LACS   20

typedef struct
{
  gprs_odm_plmn_id_t     plmn_id;
  uint8                  num_of_lacs;
  gprs_odm_lac_t         lacs[GPRS_ODM_MAX_NUM_OF_PLMN_LACS];
}gprs_odm_plmn_lacs_t;


#define GPRS_ODM_MAX_NUM_OF_PLMN_LAC_RANGES   20

typedef struct
{
  uint16 start;
  uint16 end;
}gprs_odm_lac_range_t;

typedef enum
{
  LAC_DIRECT,
  LAC_INVERT
}gprs_odm_lac_cat_t;

typedef struct
{
  gprs_odm_plmn_id_t     plmn_id;
  gprs_odm_lac_cat_t     lac_range_cat;
  uint8                  num_of_lac_ranges;
  gprs_odm_lac_range_t   lac_ranges[GPRS_ODM_MAX_NUM_OF_PLMN_LAC_RANGES];
}gprs_odm_plmn_lac_ranges_t;


/*
** Populates the given structure with the list of location area codes
** for the indicated PLMN (i.e. caller sets 'plmn_id' and then calls
** function)
**
** Input:
**    plmn_lac_list.PLMN_id
**
** Output:
**    plmn_lac_list.num_of_lacs  - number of LACs available and belong to that PLMN
**    plmn_lac_list.lacs[]       - LACs available and belong to that PLMN
*/
extern void gprs_odm_get_plmn_lac_list( gprs_odm_plmn_lacs_t *plmn_lac_list );


 /*
 ** Set the range of LACs belongs to user selected network
 **
 ** Input:
 **    odm_lac_range_list  -  LAC ranges for the user selected PLMN, read from SIM
 **    gprs_odm_lac_cat_t  -  is LAC range allowed (LAC_DIRECT) or not (LAC_INVERT)
 **
 ** Output:
 **    rr_plmn_lac_range   -  RR's local static copy of odm_lac_range_list
 */
extern void gprs_odm_set_plmn_lac_list(
  const gprs_odm_plmn_lac_ranges_t *odm_lac_range_list,
  gprs_odm_lac_cat_t               range_cat
);

#endif /* GPRS_ODM_INFO_H */

/* EOF */
