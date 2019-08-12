#ifndef WWAN_STUBS_H
#define WWAN_STUBS_H


#define SM_OTDOA_NEIGHBOUR_FREQ_INFO_ELEMENTS 3
#define SM_LOC_LTE_OTDOA_MAX_NGBR_CELLS        24

/**************************************************************/
/*                                                            */
/*  ECGI                                                      */
/*                                                            */
/**************************************************************/

typedef struct {
   uint8 mcc[3];
   uint8 mnc[3];
   uint32 cellidentity;
   uint8 num_mnc_digits;
   uint8 cell_id_numbits;
} sm_ECGI;


typedef struct sm_PRS_Info_prs_MutingInfo_r9 {
   uint8 t;
   union {
      /* t = 1 */
      uint8 po2_r9[4];
      /* t = 2 */
      uint8 po4_r9[4];
      /* t = 3 */
      uint8 po8_r9[4];
      /* t = 4 */
      uint8 po16_r9[4];
   } u;
} sm_PRS_Info_prs_MutingInfo_r9;

/**************************************************************/
/*                                                            */
/*  PRS_Info                                                  */
/*                                                            */
/**************************************************************/

typedef struct {
   struct {
      unsigned prs_MutingInfo_r9Present : 1;
   } m;
   uint32 prs_Bandwidth;
   uint16 prs_ConfigurationIndex;
   uint32 numDL_Frames;
   sm_PRS_Info_prs_MutingInfo_r9 prs_MutingInfo_r9;
} sm_PRS_Info;

/**************************************************************/
/*                                                            */
/*  sm_OTDOA_NeighbourCellInfoElement                            */
/*                                                            */
/**************************************************************/

typedef struct sm_OTDOA_NeighbourCellInfoElement {
   struct {
      unsigned cellGlobalIdPresent : 1;
      unsigned earfcnPresent : 1;
      unsigned cpLengthPresent : 1;
      unsigned prsInfoPresent : 1;
      unsigned antennaPortConfigPresent : 1;
      unsigned slotNumberOffsetPresent : 1;
      unsigned prs_SubframeOffsetPresent : 1;
   } m;
   uint16 physCellId;
   sm_ECGI cellGlobalId;
   uint16 earfcn;
   uint32 cpLength;
   sm_PRS_Info prsInfo;
   uint32 antennaPortConfig;
   uint8 slotNumberOffset;
   uint16 prs_SubframeOffset;
   uint16 expectedRSTD;
   uint16 expectedRSTD_Uncertainty;
} sm_OTDOA_NeighbourCellInfoElement;


/**************************************************************/
/*                                                            */
/*  sm_OTDOA_NeighbourFreqInfo                                   */
/*                                                            */
/**************************************************************/

typedef struct sm_OTDOA_NeighbourFreqInfo {
   uint32 n;
   sm_OTDOA_NeighbourCellInfoElement neighbor_cell_elem[SM_LOC_LTE_OTDOA_MAX_NGBR_CELLS];
} sm_OTDOA_NeighbourFreqInfo;

/**************************************************************/
/*                                                            */
/*  sm_OTDOA_NeighbourCellInfoList                               */
/*                                                            */
/**************************************************************/

typedef struct sm_OTDOA_NeighbourCellInfoList {
   uint32 n;
   sm_OTDOA_NeighbourFreqInfo neighbor_freq_elem[SM_OTDOA_NEIGHBOUR_FREQ_INFO_ELEMENTS];
} sm_OTDOA_NeighbourCellInfoList;
/**************************************************************/
/*                                                            */
/*  sm_OTDOA_ReferenceCellInfo                                   */
/*                                                            */
/**************************************************************/

typedef struct sm_OTDOA_ReferenceCellInfo {
   struct {
      unsigned cellGlobalIdPresent : 1;
      unsigned earfcnRefPresent : 1;
      unsigned antennaPortConfigPresent : 1;
      unsigned prsInfoPresent : 1;
   } m;
   uint16 physCellId;
   sm_ECGI cellGlobalId;
   uint16 earfcnRef;
   uint32 antennaPortConfig;
   uint32 cpLength;
   sm_PRS_Info prsInfo;
} sm_OTDOA_ReferenceCellInfo;
/* Assistance Data provided by the network */

typedef struct sm_OTDOA_AssistanceData {
   struct {
      unsigned otdoa_ReferenceCellInfoPresent : 1;
      unsigned otdoa_NeighbourCellInfoPresent : 1;
   } m;
   sm_OTDOA_ReferenceCellInfo otdoa_ReferenceCellInfo;
   sm_OTDOA_NeighbourCellInfoList otdoa_NeighbourCellInfo;
} sm_OTDOA_AssistanceData;
#endif /* WWAN_STUBS_H */
