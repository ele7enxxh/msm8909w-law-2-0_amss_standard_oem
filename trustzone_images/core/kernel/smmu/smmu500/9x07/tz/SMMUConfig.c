/*=========================================================================

                       S M M U  L I B R A R Y

FILE:      SMMUConfig.c
   System MMU

GENERAL DESCRIPTION
  This file contains the one-time configuration data for configuring each of
  the context banks for the SMMU instances for multimedia hardware blocks.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
 
===============================================================================
Copyright (c) 2010 - 2014
Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/kernel/smmu/smmu500/9x07/tz/SMMUConfig.c#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $
$Change: 10727476 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/22/14   sc      Updated CB23 from S1-only to S2-only
03/03/14   sc      Updated SID mappings
02/02/14   sc      Swapped APPS CBs 19 and 20 SIDs
01/14/14   sc      Added S2 config
12/02/13   sc      Updated with SID mappings
11/08/13   sc      Updated for 8916
10/23/13   sc      Updated
03/01/12   kcr     Initial Version
=============================================================================*/
#include "tzbsp_target.h"
#include "SMMUConfig.h"

extern uint32 smmu_num_max_devices;
// table that contains TBU SS info such as TBU SS address, TBU ID, and
// max number of the SID2VMID_SMRn entries
SMMUTBUSSInfo_t smmuTBUSSInfo[] =
{
  // === APPS ===
  {
    // PERIPH (TBU ID = 0)
    0x1EF4000 + TZBSP_SANDBOX_RELOCATE_OFFSET,  // TBU SS base address
    47,                                         // max number of SID2VMID_SMRn entries
  },
  {
    // GPS (TBU ID = 1)
    0x1EF5000 + TZBSP_SANDBOX_RELOCATE_OFFSET,  // TBU SS base address
    2,                                          // max number of SID2VMID_SMRn entries
  },
  {
    // MCDMA (TBU ID = 2)
    0x1EF6000 + TZBSP_SANDBOX_RELOCATE_OFFSET,  // TBU SS base address
    2,                                         // max number of SID2VMID_SMRn entries
  },
  {
    // QDSP (TBU ID = 3)
    0x1EF7000 + TZBSP_SANDBOX_RELOCATE_OFFSET,  // TBU SS base address
    2,                                          // max number of SID2VMID_SMRn entries
  },
};

/*Static configuration data is for V1.0*/
SMMUDeviceConfigEntry_t smmuDeviceConfigData[] =
{
  /* ======================= Device 1 (SMMU_APPS) ========================= */
  {
    0x01E00000 + TZBSP_SANDBOX_RELOCATE_OFFSET, //base phys address
    31,                                         //numNonSecureCBs
    1,                                          //numSecureCBs
    0,                                          //secure interrupt number
    (32 - 4),                                 //SMR Override (for SMT)
    (16 - 1),                                 //CB Group Override
    31,                                         //No Interrupt Report Override
    {0, 1, 2, 3, 16, 17, 24, 25, 48},               //List of secure masters (MIDs) that drive NS_STATE=0
    // list of bypass attr control
    {
      { // MSS-GPS, MCDMA
        SMMU_MODEM_VMID,     // client vmid
        // SIDs of the bypass attr control
        {0x80000401, 				//mss-gps
	  0x80000801,	SMMU_INVALID_BPSID		//MCDMA			
		}
      },
      { // XM_DAP, TIC/ DCC/ DBG-UI, SMMU_INVALID_BPSID
        SMMU_HLOS_VMID,      // client vmid
        // SIDs of the bypass attr control
        {0x813F0080,	SMMU_INVALID_BPSID			}
      },
      { // XM_DAP, TIC/ DCC/ DBG-UI, SMMU_INVALID_BPSID
        SMMU_SROT_VMID,      // client vmid
        // SIDs of the bypass attr control
        {0x813F0080,	SMMU_INVALID_BPSID			}
      },
      { // MSS-Qdsp
        SMMU_MODEM_VMID,      // client vmid
        // SIDs of the bypass attr control
        { 0x80000C01, SMMU_INVALID_BPSID}
      }
    },
    //support for active context banks
    {
      // CB0
      {
        SMMU_HLOS_VMID,      //client vmid
        0,                   //interrupt index
        1,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS,      // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        19,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0x802300C4, 0x802700C8, 0x800300D4, 0x800300D8, 0x800300F4, 0x800700F8,		//CE
			0x807F0280,																	//BLSP, //QHM_USB_HS1
			0x807F0100,																	//SDC
			0x803F0000,																	//SGMI	
			0x800101C2, 0x800301C4, 0x800101C8, 0x800301CC, 0x800F01D0,					//audio
			0x803F0300,																	//QPIC	
			0x813F0240,																	//SPDM, XM_USB_HS1
			0x807F0380,																	//QHM_HSIC,XM_HSIC	
			0x80000800	   																//MSS-MCDMA	
		},
      },
      // CB1
      {
        SMMU_RPM_VMID,     //client vmid
        1,                   //interrupt index
        1,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS,      // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        1,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0x803F0040, 0 },		//RPM
      },
      // CB2
      {
        SMMU_MODEM_VMID,      //client vmid
        2,                   //interrupt index
        1,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        3,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0x800101C0, 0x800101CA, 				//audio
			0x80000400, 0 						//mss-gps
		},
      },
      // CB3
      {
        SMMU_HLOS_VMID,      //client vmid
        3,                   //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB4 - MDP
      {
        SMMU_HLOS_VMID,      //client vmid
        4,                   //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB5
      {
        SMMU_HLOS_VMID,      //client vmid
        5,                   //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB6
      {
        SMMU_HLOS_VMID,      //client vmid
        6,                   //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_NESTED, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        {  0 },
      },
      // CB7
      {
        SMMU_HLOS_VMID,     //client vmid
        7,                   //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB8
      {
        SMMU_MODEM_VMID,     //client vmid
        8,                   //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB9
      {
        SMMU_HLOS_VMID,      //client vmid
        9,                   //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB10
      {
        SMMU_HLOS_VMID,      //client vmid
        10,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB11
      {
        SMMU_HLOS_VMID,      //client vmid
        11,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB12
      {
        SMMU_HLOS_VMID,      //client vmid
        12,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB13
      {
        SMMU_HLOS_VMID,      //client vmid
        13,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB14
      {
        SMMU_HLOS_VMID,      //client vmid
        14,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB15
      {
        SMMU_SROT_VMID,      //client vmid
        15,                  //interrupt index
        1,                   //Valid Context bank
        1,                   //secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        3,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
		{ 0x802300C0, 0x800300D0, 0x800300F0 , 0			//CE
		},
      },
      // CB16
      {
        SMMU_HLOS_VMID,      //client vmid
        16,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB17
      {
        SMMU_WCNSS_VMID,     //client vmid
        17,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB18
      {
        SMMU_CP_VMID,        //client vmid
        18,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB19
      {
        SMMU_CP_VMID,        //client vmid
        19,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB20
      {
        SMMU_CP_VMID,        //client vmid
        20,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB21
      {
        SMMU_CP_VMID,        //client vmid
        21,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB22
      {
        SMMU_VIDEO_VMID,     //client vmid
        22,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB23
      {
        SMMU_RPM_VMID,       //client vmid
        23,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS,      // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB24
      {
        SMMU_HLOS_VMID,      //client vmid
        24,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB25
      {
        SMMU_HLOS_VMID,      //client vmid
        25,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB26
      {
        SMMU_HLOS_VMID,      //client vmid
        26,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB27
      {
        SMMU_HLOS_VMID,      //client vmid
        27,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB28
      {
        SMMU_HLOS_VMID,      //client vmid
        28,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB29
      {
        SMMU_HLOS_VMID,      //client vmid
        29,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB30
      {
        SMMU_SROT_VMID,      //client vmid
        30,                  //interrupt index
        0,                   //Valid Context bank
        0,                   // non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { // Master BLSP
          // TODO: need the BLSP NS=0 mapping info?
          0 },
      },
      // CB31
      {
        SMMU_SROT_VMID,      //client vmid
        31,                  //interrupt index
        0,                   //Valid Context bank
        1,                   // secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
    }
  },
  /* ======================= Device 2 (SMMU_GPU) ========================== */
  {
    0x01F00000 + TZBSP_SANDBOX_RELOCATE_OFFSET, //base phys address
    4,                                          //numNonSecureCBs
    0,                                          //numSecureCBs
    0,                                          //secure interrupt number
    0xFFFF,                                     //No SMR Override
    0xFF,                                       //No CB Group Override
    0xFF,                                       //No Interrupt Report Override
    //List of secure masters (MIDs) that drive NS_STATE=0
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
    // list of bypass attr control
    {
      {
        SMMU_INVALID_VMID,   // client vmid
        // SIDs of the bypass attr control
        {SMMU_INVALID_BPSID}
      },
      {
        SMMU_INVALID_VMID,   // client vmid
        // SIDs of the bypass attr control
        {SMMU_INVALID_BPSID}
      },
      {
        SMMU_INVALID_VMID,   // client vmid
        // SIDs of the bypass attr control
        {SMMU_INVALID_BPSID}
      },
    },
    //support for active context banks
    {
      // CB0
      {
        SMMU_HLOS_VMID,      //client vmid
        0,                   //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S2_CTX,      // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB1
      {
        SMMU_HLOS_VMID,      //client vmid
        1,                   //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB2
      {
        SMMU_HLOS_VMID,      //client vmid
        2,                   //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        {  0 },
      },
      // CB3
      {
        SMMU_HLOS_VMID,      //client vmid
        3,                   //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB4
      {
        SMMU_HLOS_VMID,      //client vmid
        4,                   //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB5
      {
        SMMU_HLOS_VMID,      //client vmid
        5,                   //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB6
      {
        SMMU_HLOS_VMID,      //client vmid
        6,                   //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB7
      {
        SMMU_HLOS_VMID,      //client vmid
        7,                   //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB8
      {
        SMMU_HLOS_VMID,      //client vmid
        8,                   //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB9
      {
        SMMU_HLOS_VMID,      //client vmid
        9,                   //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB10
      {
        SMMU_HLOS_VMID,      //client vmid
        10,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB11
      {
        SMMU_HLOS_VMID,      //client vmid
        11,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB12
      {
        SMMU_HLOS_VMID,      //client vmid
        12,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB13
      {
        SMMU_HLOS_VMID,      //client vmid
        13,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB14
      {
        SMMU_HLOS_VMID,      //client vmid
        14,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB15
      {
        SMMU_HLOS_VMID,      //client vmid
        15,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB16
      {
        SMMU_HLOS_VMID,      //client vmid
        16,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB17
      {
        SMMU_HLOS_VMID,      //client vmid
        17,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB18
      {
        SMMU_HLOS_VMID,      //client vmid
        18,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB19
      {
        SMMU_HLOS_VMID,      //client vmid
        19,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB20
      {
        SMMU_HLOS_VMID,      //client vmid
        20,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB21
      {
        SMMU_HLOS_VMID,      //client vmid
        21,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB22
      {
        SMMU_HLOS_VMID,      //client vmid
        22,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB23
      {
        SMMU_HLOS_VMID,      //client vmid
        23,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB24
      {
        SMMU_HLOS_VMID,      //client vmid
        24,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB25
      {
        SMMU_HLOS_VMID,      //client vmid
        25,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB26
      {
        SMMU_HLOS_VMID,      //client vmid
        26,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB27
      {
        SMMU_HLOS_VMID,      //client vmid
        27,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB28
      {
        SMMU_HLOS_VMID,      //client vmid
        28,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB29
      {
        SMMU_HLOS_VMID,      //client vmid
        29,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB30
      {
        SMMU_HLOS_VMID,      //client vmid
        30,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB31
      {
        SMMU_HLOS_VMID,      //client vmid
        31,                  //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
    }
  },
  /* ======================= End of Configuration ========================== */
};

SMMUConfig_t smmuConfigInfo[] =
{
  {0, smmuDeviceConfigData}
};
