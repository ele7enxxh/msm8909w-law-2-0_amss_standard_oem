/*!
  @file
  lte_rlcdl_hdr.h

  @brief
  Definition file for RLC DL header related functions.

  @author
  axiao

*/

/*==============================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/rlc/src/lte_rlcdl_hdr.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
10/25/11   ax      eMBMS support
11/13/09   ax      support for SDU flush within in-sequence segments of VR(R)
08/27/09   ax      fixed strict_aliasing violation: eliminate unneccsary pointer
                   access
06/22/09   ax      moved away from bit field: packing is not standard on all
04/13/09   ax      added overlapping segment support
11/06/08   ax      added macros that helps with logging
08/19/08   ax      initial version
==============================================================================*/

#ifndef LTE_RLCDL_HDR_H
#define LTE_RLCDL_HDR_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <dsm_item.h>
#include <lte.h>
#include "lte_rlci.h"
#include "lte_rlcdli.h"
/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! @brief sequence number fully received
*/
#define LTE_RLCDL_HDR_SN_FULLY_RXED                                     1

/*! @brief sequence number not fully received
*/
#define LTE_RLCDL_HDR_SN_NOT_FULLY_RXED                                 0

/*! @brief defines SO mask as stored within the 32 bit hdr info
*/
#define LTE_RLCDL_HDR_SO_MASK                                  0x00007FFF

/*! @brief defines SO position as stored within the 32 bit hdr info
*/
#define LTE_RLCDL_HDR_SO_POS                                           0

/*! @brief defines LSF bit mask as stored within the 32 bit hdr info
*/
#define LTE_RLCDL_HDR_LSF_MASK                                  0x00008000

/*! @brief defines LSF bit position as stored within the 32 bit hdr info
*/
#define LTE_RLCDL_HDR_LSF_POS                                          15

/*! @brief defines SN mask as stored within the 32 bit hdr info
*/
#define LTE_RLCDL_HDR_SN_MASK                                  0x03FF0000

/*! @brief defines SN position as stored within the 32 bit hdr info
*/
#define LTE_RLCDL_HDR_SN_POS                                          16

/*! @brief defines E bit mask as stored within the 32 bit hdr info
*/
#define LTE_RLCDL_HDR_E_MASK                                  0x04000000

/*! @brief defines E bit position as stored within the 32 bit hdr info
*/
#define LTE_RLCDL_HDR_E_POS                                          26

/*! @brief defines FI mask as stored within the 32 bit hdr info
*/
#define LTE_RLCDL_HDR_FI_MASK                                  0x18000000

/*! @brief defines FI position as stored within the 32 bit hdr info
*/
#define LTE_RLCDL_HDR_FI_POS                                          27

/*! @brief defines FI last mask as stored within the 32 bit hdr info
*/
#define LTE_RLCDL_HDR_FI_LAST_MASK                             0x08000000

/*! @brief defines FI last position as stored within the 32 bit hdr info
*/
#define LTE_RLCDL_HDR_FI_LAST_POS                                     27

/*! @brief defines FI first mask as stored within the 32 bit hdr info
*/
#define LTE_RLCDL_HDR_FI_FIRST_MASK                            0x10000000

/*! @brief defines FI first position as stored within the 32 bit hdr info
*/
#define LTE_RLCDL_HDR_FI_FIRST_POS                                    28

/*! @brief defines Poll bit mask as stored within the 32 bit hdr info
*/
#define LTE_RLCDL_HDR_P_MASK                                   0x20000000

/*! @brief defines Poll bit position as stored within the 32 bit hdr info
*/
#define LTE_RLCDL_HDR_P_POS                                           29

/*! @brief defines RF bit mask as stored within the 32 bit hdr info
*/
#define LTE_RLCDL_HDR_RF_MASK                                   0x40000000

/*! @brief defines RF bit position as stored within the 32 bit hdr info
*/
#define LTE_RLCDL_HDR_RF_POS                                          30

/*! @brief defines D/C bit mask as stored within the 32 bit hdr info
*/
#define LTE_RLCDL_HDR_DC_MASK                                   0x80000000

/*! @brief defines D/C bit position as stored within the 32 bit hdr info
*/
#define LTE_RLCDL_HDR_DC_POS                                           31

/*! @brief defines data length mask as stored within the 32 bit ext hdr info
*/
#define LTE_RLCDL_HDR_EXT_DATA_LEN_MASK                        0x00007FFF

/*! @brief defines data length position as stored within the 32 bit ext hdr info
*/
#define LTE_RLCDL_HDR_EXT_DATA_LEN_POS                                  0

/*! @brief defines data offset mask as stored within the 32 bit ext hdr info
*/
#define LTE_RLCDL_HDR_EXT_DATA_OFFSET_MASK                     0x7FFF0000

/*! @brief defines data offset position as stored within the 32 bit ext hdr info
*/
#define LTE_RLCDL_HDR_EXT_DATA_OFFSET_POS                               16

/*! @brief defines fully rxed mask as stored within the 32 bit ext hdr info
*/
#define LTE_RLCDL_HDR_EXT_FULLY_RXED_MASK                        0x80000000

/*! @brief defines fully rxed position as stored within the 32 bit ext hdr info
*/
#define LTE_RLCDL_HDR_EXT_FULLY_RXED_POS                                 31

/*! @brief PDU hdr parsing result
*/
typedef enum
{
  LTE_RLCDL_HDR_DATA_PDU = 0,      /*!< valid data PDU */
  LTE_RLCDL_HDR_CONTROL_PDU,       /*!< detect of control PDU */
  LTE_RLCDL_HDR_INVALID_PDU,       /*!< detect of invalid data PDU */
  LTE_RLCDL_HDR_MAX                /*!< MAX of enum */
} lte_rlcdl_hdr_pdu_type_e;

/*! @brief structure describes the packed format of the meta info about a 
    RLC PDU. It is saved in app_ptr as an uint32 of the dsm item. The layout
    of the structure is dictated by the AM PDU Segment hdr format, even 
    though some fields are not necessarily needed down the line.

    NOTE: THIS BIT FIELD STRUCTURE IS NO LONGER USED, BECAUSE THE WAY FIELDS
    ARE PACKED IS NOT STANDARD ACROSS DIFFERENT COMPILERS. THE
    MASK DEFINED ABOVE SHALL BE USED INSTEAD. THE STRUCTURE IS STILL KEPT
    IN HERE FOR ILLUSTRATION ONLY. IT MAY ALSO BE USED BY SOME OFFLINE PARSER
    TOOL, WHICH DOES NOT HAVE THE REQUIREMENT TO BE ABLE TO RUN ON DIFFERENT
    PLATFROMS.

*/
typedef struct
{
  uint32  so:15;       /*!< Segment Offset (SO) field  */
  uint32  lsf:1;       /*!< Last Segment Flag (LSF) field, 1 if the last seg*/
  uint32  sn:10;       /*!< Sequence Number (SN) field  */
  uint32  e:1;         /*!< Extension bit (E) field  */
  uint32  fi:2;        /*!< framing info (FI) field */
  uint32  p:1;         /*!< Polling bit (P) field */
  uint32  rf:1;        /*!< Re-segmentation Flag (RF) field, 1 if segment*/
  uint32  d_c:1;       /*!< Data/Control (D/C) field, 1: data PDU; 0: control */
} lte_rlcdl_hdr_info_s;

/*! @brief structure describes the packed format of the meta info about a 
    RLC PDU. It is saved in app_field as an uint32 of the dsm item. 

    NOTE: THIS BIT FIELD STRUCTURE IS NO LONGER USED, BECAUSE THE WAY FIELDS
    ARE PACKED IS NOT STANDARD ACROSS DIFFERENT COMPILERS. THE
    MASK DEFINED ABOVE SHALL BE USED INSTEAD. THE STRUCTURE IS STILL KEPT
    IN HERE FOR ILLUSTRATION ONLY. IT MAY ALSO BE USED BY SOME OFFLINE PARSER
    TOOL, WHICH DOES NOT HAVE THE REQUIREMENT TO BE ABLE TO RUN ON DIFFERENT
    PLATFROMS.

*/
typedef struct
{
  uint32  data_length:15;        /*!< length of the RLC PDU */
  uint32  reserved:1;           /*!< reserved for now  */
  uint32  data_offset:15;       /*!< offset of the data field */
  uint32  rxed_in_full:1;       /*!< PDU of this SN fully received? */
} lte_rlcdl_hdr_info_ext_s;

/*! @brief field within dsm item used to store basic hdr information
*/
#define LTE_RLCDL_HDR_INFO_BAS(dsm_ptr)                              \
(                                                                    \
  (uint32) ((dsm_ptr)->app_ptr)                                      \
)

/*! @brief field within dsm item used to store basic hdr information: L value
*/
#define LTE_RLCDL_HDR_INFO_BAS_L(dsm_ptr)                            \
(                                                                    \
  ((dsm_ptr)->app_ptr)                                               \
)

/*! @brief initialize the HDR INFO to 0
*/
#define LTE_RLCDL_HDR_INIT_INFO(dsm_ptr)                         \
(                                                                \
  (dsm_ptr)->app_ptr = (void*) 0;                                \
)

/*! @brief field within dsm item used to store derived hdr information
*/
#define LTE_RLCDL_HDR_INFO_EXT(dsm_ptr)                          \
(                                                                \
  ((dsm_ptr)->app_field)                                         \
)

/*! @brief copy basic hdr meta info from source to target
*/
#define LTE_RLCDL_HDR_COPY_INFO(src_dsm_ptr, target_dsm_ptr)     \
(                                                                \
  (target_dsm_ptr)->app_ptr = (src_dsm_ptr)->app_ptr             \
)

/*==============================================================================

  FUNCTION:  LTE_RLCDL_HDR_GET_LSF

==============================================================================*/
/*!
    @brief
    extract the LSF bit.

    @return
    LSF bit.
*/
/*============================================================================*/
static inline uint8 LTE_RLCDL_HDR_GET_LSF
(
   const dsm_item_type* dsm_ptr  /*!< ptr to the PDU dsm item */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  return (uint8) ((LTE_RLCDL_HDR_INFO_BAS(dsm_ptr) & 
                   LTE_RLCI_FIELD_MASK(LTE_RLCDL_HDR_LSF)) >>
                   LTE_RLCI_FIELD_POS(LTE_RLCDL_HDR_LSF));
} /* LTE_RLCDL_HDR_GET_LSF() */

/*==============================================================================

  FUNCTION:  LTE_RLCDL_HDR_GET_RF

==============================================================================*/
/*!
    @brief
    extract the RF field.

    @return
    RF field.
*/
/*============================================================================*/
static inline uint8 LTE_RLCDL_HDR_GET_RF
(
   const dsm_item_type* dsm_ptr  /*!< ptr to the PDU dsm item */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  return (uint8) ((LTE_RLCDL_HDR_INFO_BAS(dsm_ptr) & 
                   LTE_RLCI_FIELD_MASK(LTE_RLCDL_HDR_RF)) >>
                   LTE_RLCI_FIELD_POS(LTE_RLCDL_HDR_RF));
} /* LTE_RLCDL_HDR_GET_RF() */

/*==============================================================================

  FUNCTION:  LTE_RLCDL_HDR_GET_POLL_BIT

==============================================================================*/
/*!
    @brief
    extract the poll field.

    @return
    poll field.
*/
/*============================================================================*/
static inline uint8 LTE_RLCDL_HDR_GET_POLL_BIT
(
   const dsm_item_type* dsm_ptr  /*!< ptr to the PDU dsm item */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  return (uint8) ((LTE_RLCDL_HDR_INFO_BAS(dsm_ptr) & 
                   LTE_RLCI_FIELD_MASK(LTE_RLCDL_HDR_P)) >>
                   LTE_RLCI_FIELD_POS(LTE_RLCDL_HDR_P));
} /* LTE_RLCDL_HDR_GET_POLL_BIT() */

/*==============================================================================

  FUNCTION:  LTE_RLCDL_HDR_GET_FI

==============================================================================*/
/*!
    @brief
    extract the FI (frame info) field.

    @return
    FI field
*/
/*============================================================================*/
static inline uint8 LTE_RLCDL_HDR_GET_FI
(
   const dsm_item_type* dsm_ptr  /*!< ptr to the PDU dsm item */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  return (uint8) ((LTE_RLCDL_HDR_INFO_BAS(dsm_ptr) & 
                   LTE_RLCI_FIELD_MASK(LTE_RLCDL_HDR_FI)) >>
                   LTE_RLCI_FIELD_POS(LTE_RLCDL_HDR_FI));
} /* LTE_RLCDL_HDR_GET_FI() */

/*==============================================================================

  FUNCTION:  LTE_RLCDL_HDR_INIT_SET_SN

==============================================================================*/
/*!
    @brief
    Set the sequence number in the header info and initialize the rest to 0.

    @return
    None
*/
/*============================================================================*/
static inline void LTE_RLCDL_HDR_INIT_SET_SN
(
  dsm_item_type* dsm_ptr,  /*!< ptr to the PDU dsm item */
  uint32         sn        /*!< sequence number */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  LTE_RLCDL_HDR_INFO_BAS_L(dsm_ptr) = 
    (void*)(sn << LTE_RLCI_FIELD_POS(LTE_RLCDL_HDR_SN));
} /* LTE_RLCDL_HDR_INIT_SET_SN() */

/*==============================================================================

  FUNCTION:  LTE_RLCDL_HDR_GET_SN

==============================================================================*/
/*!
    @brief
    Get the sequence number of this PDU.

    @return
    sequence number
*/
/*============================================================================*/
static inline uint16 LTE_RLCDL_HDR_GET_SN
(
  const dsm_item_type* dsm_ptr  /*!< ptr to the PDU dsm item */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  return (uint16) ((LTE_RLCDL_HDR_INFO_BAS(dsm_ptr)& 
                    LTE_RLCI_FIELD_MASK(LTE_RLCDL_HDR_SN)) >>
                    LTE_RLCI_FIELD_POS(LTE_RLCDL_HDR_SN));
} /* LTE_RLCDL_HDR_GET_SN() */

/*==============================================================================

  FUNCTION:  LTE_RLCDL_HDR_FULLY_RXED

==============================================================================*/
/*!
    @brief
    extract the fully received flag.

    @return
    TRUE if fully received
*/
/*============================================================================*/
static inline boolean LTE_RLCDL_HDR_FULLY_RXED
(
   const dsm_item_type* dsm_ptr  /*!< ptr to the PDU dsm item */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  return ((LTE_RLCDL_HDR_INFO_EXT(dsm_ptr) & 
           LTE_RLCI_FIELD_MASK(LTE_RLCDL_HDR_EXT_FULLY_RXED)) != 0);
} /* LTE_RLCDL_HDR_FULLY_RXED() */

/*==============================================================================

  FUNCTION:  LTE_RLCDL_HDR_SET_FULLY_RXED

==============================================================================*/
/*!
    @brief
    set the fully received field of meta data.

    @return
    None
*/
/*============================================================================*/
static inline void LTE_RLCDL_HDR_SET_FULLY_RXED
(
  dsm_item_type* dsm_ptr,    /*!< ptr to the PDU dsm item */
  uint32         fully_rxed  /*!< value of fully received field */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  LTE_RLCDL_HDR_INFO_EXT(dsm_ptr) = (LTE_RLCDL_HDR_INFO_EXT(dsm_ptr) & 
    ~LTE_RLCI_FIELD_MASK(LTE_RLCDL_HDR_EXT_FULLY_RXED)) |
    (fully_rxed << LTE_RLCI_FIELD_POS(LTE_RLCDL_HDR_EXT_FULLY_RXED));
} /* LTE_RLCDL_HDR_SET_FULLY_RXED() */

/*==============================================================================

  FUNCTION:  LTE_RLCDL_HDR_E_SET

==============================================================================*/
/*!
    @brief
    extract the E flag.

    @return
    TRUE if E flag is set
*/
/*============================================================================*/
static inline boolean LTE_RLCDL_HDR_E_SET
(
   const dsm_item_type* dsm_ptr  /*!< ptr to the PDU dsm item */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  return ((LTE_RLCDL_HDR_INFO_BAS(dsm_ptr) & 
           LTE_RLCI_FIELD_MASK(LTE_RLCDL_HDR_E)) != 0);
} /* LTE_RLCDL_HDR_E_SET() */

/*==============================================================================

  FUNCTION:  LTE_RLCDL_HDR_SET_E

==============================================================================*/
/*!
    @brief
    extract the E flag.

    @return
    TRUE if E flag is set
*/
/*============================================================================*/
static inline void LTE_RLCDL_HDR_SET_E
(
   dsm_item_type* dsm_ptr,  /*!< ptr to the PDU dsm item */
   uint32         e_bit     /*!< new e bit */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  LTE_RLCDL_HDR_INFO_BAS_L(dsm_ptr) = 
    (void*)((LTE_RLCDL_HDR_INFO_BAS(dsm_ptr) & 
             ~LTE_RLCI_FIELD_MASK(LTE_RLCDL_HDR_E)) |
            (e_bit << LTE_RLCI_FIELD_POS(LTE_RLCDL_HDR_E)));
} /* LTE_RLCDL_HDR_SET_E() */

/*==============================================================================

  FUNCTION:  LTE_RLCDL_HDR_GET_E

==============================================================================*/
/*!
    @brief
    extract the E field.

    @return
    E field
*/
/*============================================================================*/
static inline uint8 LTE_RLCDL_HDR_GET_E
(
   const dsm_item_type* dsm_ptr  /*!< ptr to the PDU dsm item */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  return (uint8)((LTE_RLCDL_HDR_INFO_BAS(dsm_ptr) & 
                  LTE_RLCI_FIELD_MASK(LTE_RLCDL_HDR_E)) >>
                  LTE_RLCI_FIELD_POS(LTE_RLCDL_HDR_E) );
} /* LTE_RLCDL_HDR_GET_E() */

/*==============================================================================

  FUNCTION:  LTE_RLCDL_HDR_FI_FIRST

==============================================================================*/
/*!
    @brief
    extract the "first" flag.

    @return
    TRUE if "first" flag is set
*/
/*============================================================================*/
static inline boolean LTE_RLCDL_HDR_FI_FIRST
(
   const dsm_item_type* dsm_ptr  /*!< ptr to the PDU dsm item */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  return ((LTE_RLCDL_HDR_INFO_BAS(dsm_ptr) & 
           LTE_RLCI_FIELD_MASK(LTE_RLCDL_HDR_FI_FIRST)) == 0); 
} /* LTE_RLCDL_HDR_FI_FIRST() */

/*==============================================================================

  FUNCTION:  LTE_RLCDL_HDR_SET_FI_FIRST

==============================================================================*/
/*!
    @brief
    set the "first" field.

    @return
    None
*/
/*============================================================================*/
static inline void LTE_RLCDL_HDR_SET_FI_FIRST
(
  dsm_item_type* dsm_ptr,  /*!< ptr to the PDU dsm item */
  uint32         first     /*!< value of the first field */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  LTE_RLCDL_HDR_INFO_BAS_L(dsm_ptr) = 
    (void*)((LTE_RLCDL_HDR_INFO_BAS(dsm_ptr) & 
             ~LTE_RLCI_FIELD_MASK(LTE_RLCDL_HDR_FI_FIRST)) |
             (first << LTE_RLCI_FIELD_POS(LTE_RLCDL_HDR_FI_FIRST)));
} /* LTE_RLCDL_HDR_SET_FI_FIRST() */

/*==============================================================================

  FUNCTION:  LTE_RLCDL_HDR_SET_FI

==============================================================================*/
/*!
    @brief
    set the fi field.

    @return
    None
*/
/*============================================================================*/
static inline void LTE_RLCDL_HDR_SET_FI
(
  dsm_item_type* dsm_ptr,  /*!< ptr to the PDU dsm item */
  uint32         fi        /*!< new fi */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  LTE_RLCDL_HDR_INFO_BAS_L(dsm_ptr) = 
    (void*)((LTE_RLCDL_HDR_INFO_BAS(dsm_ptr) & 
             ~LTE_RLCI_FIELD_MASK(LTE_RLCDL_HDR_FI)) |
            (fi << LTE_RLCI_FIELD_POS(LTE_RLCDL_HDR_FI)));
} /* LTE_RLCDL_HDR_SET_FI() */

/*==============================================================================

  FUNCTION:  LTE_RLCDL_HDR_FI_LAST

==============================================================================*/
/*!
    @brief
    extract the "last" flag.

    @return
    TRUE if "last" flag is set
*/
/*============================================================================*/
static inline boolean LTE_RLCDL_HDR_FI_LAST
(
   const dsm_item_type* dsm_ptr  /*!< ptr to the PDU dsm item */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  return ((LTE_RLCDL_HDR_INFO_BAS(dsm_ptr) & 
           LTE_RLCI_FIELD_MASK(LTE_RLCDL_HDR_FI_LAST)) == 0); 
} /* LTE_RLCDL_HDR_FI_LAST() */

/*==============================================================================

  FUNCTION:  LTE_RLCDL_HDR_SET_FI_LAST

==============================================================================*/
/*!
    @brief
    set the "last" field.

    @return
    None
*/
/*============================================================================*/
static inline void LTE_RLCDL_HDR_SET_FI_LAST
(
  dsm_item_type* dsm_ptr,  /*!< ptr to the PDU dsm item */
  uint32         last      /*!< value of the last field */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  LTE_RLCDL_HDR_INFO_BAS_L(dsm_ptr) = 
    (void*)((LTE_RLCDL_HDR_INFO_BAS(dsm_ptr) & 
             ~LTE_RLCI_FIELD_MASK(LTE_RLCDL_HDR_FI_LAST)) |
            (last << LTE_RLCI_FIELD_POS(LTE_RLCDL_HDR_FI_LAST)));
} /* LTE_RLCDL_HDR_SET_FI_LAST() */

/*==============================================================================

  FUNCTION:  LTE_RLCDL_HDR_IS_DATA_PDU

==============================================================================*/
/*!
    @brief
    extract the D/C field.

    @return
    TRUE if it is a data PDU. FALSE for control PDU
*/
/*============================================================================*/
static inline boolean LTE_RLCDL_HDR_IS_DATA_PDU
(
   const dsm_item_type* dsm_ptr  /*!< ptr to the PDU dsm item */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  return ((LTE_RLCDL_HDR_INFO_BAS(dsm_ptr)& 
           LTE_RLCI_FIELD_MASK(LTE_RLCDL_HDR_DC)) != 0) ;
} /* LTE_RLCDL_HDR_IS_DATA_PDU() */

/*==============================================================================

  FUNCTION:  LTE_RLCDL_HDR_POLL_SET

==============================================================================*/
/*!
    @brief
    extract the poll bit.

    @return
    TRUE if poll bit is set. FALSE otherwise.
*/
/*============================================================================*/
static inline boolean LTE_RLCDL_HDR_POLL_SET
(
   const dsm_item_type* dsm_ptr  /*!< ptr to the PDU dsm item */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  return ((LTE_RLCDL_HDR_INFO_BAS(dsm_ptr) & 
           LTE_RLCI_FIELD_MASK(LTE_RLCDL_HDR_P)) != 0);
} /* LTE_RLCDL_HDR_POLL_SET() */

/*==============================================================================

  FUNCTION:  LTE_RLCDL_HDR_SET_POLL_BIT

==============================================================================*/
/*!
    @brief
    set the poll field.

    @return
    None.
*/
/*============================================================================*/
static inline void LTE_RLCDL_HDR_SET_POLL_BIT
(
  dsm_item_type* dsm_ptr  /*!< ptr to the PDU dsm item */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  LTE_RLCDL_HDR_INFO_BAS_L(dsm_ptr) = 
    (void*)((LTE_RLCDL_HDR_INFO_BAS(dsm_ptr) & 
             ~LTE_RLCI_FIELD_MASK(LTE_RLCDL_HDR_P)) |
            (LTE_RLCI_HDR_P_SET << LTE_RLCI_FIELD_POS(LTE_RLCDL_HDR_P)));
} /* LTE_RLCDL_HDR_SET_POLL_BIT() */

/*==============================================================================

  FUNCTION:  LTE_RLCDL_HDR_PDU_SEGMENT

==============================================================================*/
/*!
    @brief
    extract the RF bit.

    @return
    TRUE if it is a pdu segment. FALSE otherwise.
*/
/*============================================================================*/
static inline boolean LTE_RLCDL_HDR_PDU_SEGMENT
(
   const dsm_item_type* dsm_ptr  /*!< ptr to the PDU dsm item */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  return ((LTE_RLCDL_HDR_INFO_BAS(dsm_ptr) & 
           LTE_RLCI_FIELD_MASK(LTE_RLCDL_HDR_RF)) != 0);
} /* LTE_RLCDL_HDR_PDU_SEGMENT() */


/*==============================================================================

  FUNCTION:  LTE_RLCDL_HDR_LSF

==============================================================================*/
/*!
    @brief
    extract the LSF bit.

    @return
    TRUE if the segment is the last of the PDU. FALSE otherwise.
*/
/*============================================================================*/
static inline boolean LTE_RLCDL_HDR_LSF
(
   const dsm_item_type* dsm_ptr  /*!< ptr to the PDU dsm item */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  return ((LTE_RLCDL_HDR_INFO_BAS(dsm_ptr) & 
           LTE_RLCI_FIELD_MASK(LTE_RLCDL_HDR_LSF)) != 0);
} /* LTE_RLCDL_HDR_LSF() */

/*==============================================================================

  FUNCTION:  LTE_RLCDL_HDR_SET_LSF

==============================================================================*/
/*!
    @brief
    set the LSF bit.

    @return
    None
*/
/*============================================================================*/
static inline void LTE_RLCDL_HDR_SET_LSF
(
  dsm_item_type* dsm_ptr,  /*!< ptr to the PDU dsm item */
  uint32         lsf       /*!< new lsf */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  LTE_RLCDL_HDR_INFO_BAS_L(dsm_ptr) = 
    (void*)((LTE_RLCDL_HDR_INFO_BAS(dsm_ptr) & 
             ~LTE_RLCI_FIELD_MASK(LTE_RLCDL_HDR_LSF)) |
            (lsf << LTE_RLCI_FIELD_POS(LTE_RLCDL_HDR_LSF)));
} /* LTE_RLCDL_HDR_GET_LSF() */

/*==============================================================================

  FUNCTION:  LTE_RLCDL_HDR_SEGMENT_SO

==============================================================================*/
/*!
    @brief
    Get the segment offset of this particular PDU segment.

    @return
    position within the data field of the original AMD PDU.
*/
/*============================================================================*/
static inline uint16 LTE_RLCDL_HDR_SEGMENT_SO
(
   const dsm_item_type* dsm_ptr  /*!< ptr to the PDU dsm item */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  return (uint16) ((LTE_RLCDL_HDR_INFO_BAS(dsm_ptr) & 
                    LTE_RLCI_FIELD_MASK(LTE_RLCDL_HDR_SO)) >>
                    LTE_RLCI_FIELD_POS(LTE_RLCDL_HDR_SO));
} /* LTE_RLCDL_HDR_SEGMENT_SO() */

/*==============================================================================

  FUNCTION:  LTE_RLCDL_HDR_SET_SEGMENT_SO

==============================================================================*/
/*!
    @brief
    Get the segment offset of this particular PDU segment.

    @return
    position within the data field of the original AMD PDU.
*/
/*============================================================================*/
static inline void LTE_RLCDL_HDR_SET_SEGMENT_SO
(
  dsm_item_type* dsm_ptr,  /*!< ptr to the PDU dsm item */
  uint16         new_so    /*!< new segment offset */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  LTE_RLCDL_HDR_INFO_BAS_L(dsm_ptr) = 
    (void*)((LTE_RLCDL_HDR_INFO_BAS(dsm_ptr) & 
             ~LTE_RLCI_FIELD_MASK(LTE_RLCDL_HDR_SO)) |
            (new_so << LTE_RLCI_FIELD_POS(LTE_RLCDL_HDR_SO)));
} /* LTE_RLCDL_HDR_SEGMENT_SO() */

/*==============================================================================

  FUNCTION:  LTE_RLCDL_HDR_GET_DATA_OFFSET

==============================================================================*/
/*!
    @brief
    extract the data segment offset.

    @return
    the data segment offset
*/
/*============================================================================*/
static inline uint16 LTE_RLCDL_HDR_GET_DATA_OFFSET
(
   const dsm_item_type* dsm_ptr  /*!< ptr to the PDU dsm item */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  return (uint16)((LTE_RLCDL_HDR_INFO_EXT(dsm_ptr) & 
    LTE_RLCI_FIELD_MASK(LTE_RLCDL_HDR_EXT_DATA_OFFSET)) >>
    LTE_RLCI_FIELD_POS(LTE_RLCDL_HDR_EXT_DATA_OFFSET));
} /* LTE_RLCDL_HDR_GET_DATA_OFFSET() */

/*==============================================================================

  FUNCTION:  LTE_RLCDL_HDR_SET_DATA_OFFSET

==============================================================================*/
/*!
    @brief
    set the data offset, which also happends to be the hdr length.

    @return
    None
*/
/*============================================================================*/
static inline void LTE_RLCDL_HDR_SET_DATA_OFFSET
(
  dsm_item_type* dsm_ptr,  /*!< ptr to the PDU dsm item */
  uint16         offset    /*!< data field offset */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  LTE_RLCDL_HDR_INFO_EXT(dsm_ptr) = (LTE_RLCDL_HDR_INFO_EXT(dsm_ptr) & 
    ~LTE_RLCI_FIELD_MASK(LTE_RLCDL_HDR_EXT_DATA_OFFSET)) |
    (offset << LTE_RLCI_FIELD_POS(LTE_RLCDL_HDR_EXT_DATA_OFFSET));
} /* LTE_RLCDL_HDR_SET_DATA_OFFSET() */

/*==============================================================================

  FUNCTION:  LTE_RLCDL_HDR_GET_DATA_LEN

==============================================================================*/
/*!
    @brief
    extract the data segment length.

    @return
    the data segment length
*/
/*============================================================================*/
static inline uint16 LTE_RLCDL_HDR_GET_DATA_LEN
(
   const dsm_item_type* dsm_ptr  /*!< ptr to the PDU dsm item */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  return (uint16)((LTE_RLCDL_HDR_INFO_EXT(dsm_ptr) & 
    LTE_RLCI_FIELD_MASK(LTE_RLCDL_HDR_EXT_DATA_LEN)) >>
    LTE_RLCI_FIELD_POS(LTE_RLCDL_HDR_EXT_DATA_LEN));
} /* LTE_RLCDL_HDR_GET_DATA_LEN() */

/*==============================================================================

  FUNCTION:  LTE_RLCDL_HDR_SET_DATA_LEN

==============================================================================*/
/*!
    @brief
    set the data segment length.

    @return
    None
*/
/*============================================================================*/
static inline void LTE_RLCDL_HDR_SET_DATA_LEN
(
  dsm_item_type* dsm_ptr,  /*!< ptr to the PDU dsm item */
  uint16         len       /*!< data len */
)
{
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  LTE_RLCDL_HDR_INFO_EXT(dsm_ptr) = (LTE_RLCDL_HDR_INFO_EXT(dsm_ptr) & 
    ~LTE_RLCI_FIELD_MASK(LTE_RLCDL_HDR_EXT_DATA_LEN)) |
    (len << LTE_RLCI_FIELD_POS(LTE_RLCDL_HDR_EXT_DATA_LEN));
} /* LTE_RLCDL_HDR_SET_DATA_LEN() */


/*! @brief extract even LI from the buffer, buf_ptr needs to be ptr to uint8
*/
#define LTE_RLCDL_HDR_GET_EVEN_LI(buf_ptr)                                 \
(                                                                          \
  (uint16) ( (*(buf_ptr) & 0x7f) << 4) | (*((buf_ptr)+1) >> 4)             \
)

/*! @brief extract odd LI from the buffer, buf_ptr needs to be ptr to uint8
*/
#define LTE_RLCDL_HDR_GET_ODD_LI(buf_ptr)                                  \
(                                                                          \
  (uint16) ( (*(buf_ptr) & 0x07) << 8) | *((buf_ptr)+1)                    \
)

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/
extern lte_rlcdl_hdr_pdu_type_e lte_rlcdl_hdr_parse_um
(
  const lte_rlcdli_ctrl_blk_s*    ctrl_blk_ptr,
  dsm_item_type*                  pdu_dsm_ptr
);

extern lte_rlcdl_hdr_pdu_type_e lte_rlcdl_hdr_parse_am
(
  const lte_rlcdli_ctrl_blk_s*    ctrl_blk_ptr,
  dsm_item_type*                  pdu_dsm_ptr
);

extern lte_rlcdl_hdr_pdu_type_e lte_rlcdl_hdr_update_um_pdu_info_sn5
(
  lte_rb_cfg_idx_t               rb_cfg_idx,
  dsm_item_type*                 dsm_ptr    
);

#endif /* LTE_RLCDL_HDR_H */
