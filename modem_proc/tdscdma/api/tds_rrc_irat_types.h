#ifndef TDS_RRC_IRAT_TYPES_H
#define TDS_RRC_IRAT_TYPES_H


/*===========================================================================
                      RRC External APIs

DESCRIPTION

  This module contains protoypes of APIs that TDSCDMA RRC provides to external modules



Copyright (c) 2000-2009 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/api/tds_rrc_irat_types.h#1 $    $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

#include <comdef.h>      /* Definition of basic types and macros */
#include <tdsiratif.h>

#define TDSIRAT_MAX_TDS_HO_MEAS_FREQ 7

#if 0  //FDD only, not needed
#define TDSRRC_MAX_INTERRAT_MEASUREMENTS 96

typedef enum 
{ 
  TDSRRC_DIVERSITY_OFF, 
  TDSRRC_DIVERSITY_ON, 
  TDSRRC_DIVERSITY_UNKNOWN=255 
} tdsrrc_diversity_T;

typedef struct
{
  word                 uarfcn;
  word                 cpid; /* cell parameter id */
  tdsrrc_diversity_T   diversity; 
  uint16               PN_Offset; /* 0-38399 (valid), 65535 (not-known) */
} tdsrrc_cell_meas_type;
#endif

typedef struct 
{
  uint8  cpid;
  int16  rscp;
} tdsrrc_jds_cell_type;

typedef struct 
{
  uint16               uarfcn;
  uint8                num_of_jds_cells; /* number of JDS cells for interference cancellation */
  uint8                lna_state; /* current LNA state */
  uint16               ts0_position; /* ts0 start position from step0 result */
  tdsrrc_jds_cell_type cell_list[TDSIRAT_MAX_TDS_CELLS_PER_FREQ]; /* jds cell meas info */
} tdsrrc_freq_meas_type;

typedef struct 
{
  byte number_of_cells; /* this is not needed, but LTE still fills it, leave it for compile */
//PSHO TEMP End: for compilation only, need to be removed after both T and L code change complete
  uint8                  num_of_freq;
  tdsrrc_freq_meas_type  utran_tds_freq_measurements[TDSIRAT_MAX_TDS_HO_MEAS_FREQ];
} tdsrrc_interrat_measurement_data_type;

#endif /*TDS_RRC_IRAT_TYPES_H*/

