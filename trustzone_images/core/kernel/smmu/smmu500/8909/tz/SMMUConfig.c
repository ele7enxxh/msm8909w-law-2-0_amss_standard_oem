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

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/kernel/smmu/smmu500/8909/tz/SMMUConfig.c#1 $
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

// table that contains TBU SS info such as TBU SS address, TBU ID, and
// max number of the SID2VMID_SMRn entries
SMMUTBUSSInfo_t smmuTBUSSInfo[] =
{
  // === APPS ===
  {
    // PERIPH (TBU ID = 0)
    0x1EF8000 + TZBSP_SANDBOX_RELOCATE_OFFSET,  // TBU SS base address
    47,                                         // max number of SID2VMID_SMRn entries
  },
  {
    // VFE (TBU ID = 1)
    0x1EF4000 + TZBSP_SANDBOX_RELOCATE_OFFSET,  // TBU SS base address
    4,                                          // max number of SID2VMID_SMRn entries
  },
  {
    // VENUS (TBU ID = 2)
    0x1EF6000 + TZBSP_SANDBOX_RELOCATE_OFFSET,  // TBU SS base address
    18,                                         // max number of SID2VMID_SMRn entries
  },
  {
    // MDP (TBU ID = 3)
    0x1EF5000 + TZBSP_SANDBOX_RELOCATE_OFFSET,  // TBU SS base address
    2,                                          // max number of SID2VMID_SMRn entries
  },
  {
    // QDSP (TBU ID = 4)
    0x1EFB000 + TZBSP_SANDBOX_RELOCATE_OFFSET,  // TBU SS base address
    4,                                          // max number of SID2VMID_SMRn entries
  },
  {
    // GPS (TBU ID = 5)
    0x1EFA000 + TZBSP_SANDBOX_RELOCATE_OFFSET,  // TBU SS base address
    2,                                          // max number of SID2VMID_SMRn entries
  },
  {
    // DIME/A2 (TBU ID = 6)
    0x1EF9000 + TZBSP_SANDBOX_RELOCATE_OFFSET,  // TBU SS base address
    2,                                          // max number of SID2VMID_SMRn entries
  },
  {
    // PRONTO (TBU ID = 7)
    0x1EF7000 + TZBSP_SANDBOX_RELOCATE_OFFSET,  // TBU SS base address
    13,                                         // max number of SID2VMID_SMRn entries
  },
  // === GFX ===
  {
    // GFX (TBU ID = 0 + SMMU_NUM_APPS_TBUS)
    // The actual GFX TBU ID is 0.
    0x1EFC000 + TZBSP_SANDBOX_RELOCATE_OFFSET,  // TBU SS base address
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
    (0x80 - 6),                                 //SMR Override (for SMT)
    (0x20 - 2),                                 //CB Group Override
    31,                                         //No Interrupt Report Override
    {0, 1, 2, 3, 16, 17, 24, 25},               //List of secure masters (MIDs) that drive NS_STATE=0
    // list of bypass attr control
    {
      { // MDSP, GPS, DIME MSA=1
        SMMU_MODEM_VMID,     // client vmid
        // SIDs of the bypass attr control
        {0x80061001, 0x80001401, 0x80001801}
      },
      { // TIC
        SMMU_HLOS_VMID,      // client vmid
        // SIDs of the bypass attr control
        {0x803F0080, SMMU_INVALID_BPSID}
      },
      { // DAP
        SMMU_HLOS_VMID,      // client vmid
        // SIDs of the bypass attr control
        {0x803F0180, SMMU_INVALID_BPSID}
      },
	  { // MODEM
        SMMU_HLOS_VMID,      // client vmid
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
        1,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S2_CTX,      // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
      },
      // CB1
      {
        SMMU_MODEM_VMID,     //client vmid
        1,                   //interrupt index
        0,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S2_CTX,      // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        0,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0 },
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
        { 0x800101C0, 0x800101CA, 0x80021004, 0 },
      },
      // CB3
      {
        SMMU_HLOS_VMID,      //client vmid
        3,                   //interrupt index
        1,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        1,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0x80000400, 0 },
      },
      // CB4 - MDP
      {
        SMMU_HLOS_VMID,      //client vmid
        4,                   //interrupt index
        1,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        1,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0x80000C00, 0 },
      },
      // CB5
      {
        SMMU_HLOS_VMID,      //client vmid
        5,                   //interrupt index
        1,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        5,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0x80000800, 0x80000807, 0x80270808, 0x80000811, 0x80000831, 0 },             
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
        1,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        1,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0x80021000, 0 },
      },
      // CB8
      {
        SMMU_MODEM_VMID,     //client vmid
        8,                   //interrupt index
        1,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        1,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0x80001400, 0 },
      },
      // CB9
      {
        SMMU_HLOS_VMID,      //client vmid
        9,                   //interrupt index
        1,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        1,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0x80001800, 0 },
      },
      // CB10
      {
        SMMU_HLOS_VMID,      //client vmid
        10,                  //interrupt index
        1,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        1,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0x803F0300, 0 },
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
        SMMU_WCNSS_VMID,     //client vmid
        17,                  //interrupt index
        1,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        12,                  // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0x80001C01, 0x80001C02, 0x80001C03, 0x80001C04, 0x80001C06,
          0x80001C07, 0x80001C08, 0x80001C09, 0x80001C0A, 0x80001C0B,
          0x80001C0C, 0x80001C0D, 0 },
      },
      // CB18
      {
        SMMU_CP_VMID,        //client vmid
        18,                  //interrupt index
        1,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        1,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0x80000C01, 0 },
      },
      // CB19
      {
        SMMU_CP_VMID,        //client vmid
        19,                  //interrupt index
        1,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        5,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0x80000900, 0x8000090A, 0x8000090E, 0x80200909, 0x8020090B ,0 },
      },
      // CB20
      {
        SMMU_CP_VMID,        //client vmid
        20,                  //interrupt index
        1,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        1,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0x8020090C, 0 },
      },
      // CB21
      {
        SMMU_CP_VMID,        //client vmid
        21,                  //interrupt index
        1,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        5,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0x800009C0, 0x80000907, 0x8000090F, 0x80200908, 0x8020090D, 0 },
      },
      // CB22
      {
        SMMU_VIDEO_VMID,     //client vmid
        22,                  //interrupt index
        1,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        2,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0x80000980, 0x80000986, 0 },
      },
      // CB23
      {
        SMMU_RPM_VMID,       //client vmid
        23,                  //interrupt index
        1,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS,      // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        1,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank -- PERIPH RPM
        { 0x803F0040, 0 },
      },
      // CB24
      {
        SMMU_HLOS_VMID,      //client vmid
        24,                  //interrupt index
        1,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        9,                  // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { // Master CE
          0x802300C4, 0x802300C8, 0x802300CC, 0x802000D4, 0x802000D7,
          0x802000D8, 0x802000DB, 0x802000DC, 0x802000DF, 0 },
      },
      // CB25
      {
        SMMU_HLOS_VMID,      //client vmid
        25,                  //interrupt index
        1,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        1,                  // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { // Master BLSP
		  0x803F0280 , 0 },
      },
      // CB26
      {
        SMMU_HLOS_VMID,      //client vmid
        26,                  //interrupt index
        1,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        1,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { // Master SDC1
          0x803F0100, 0 },
      },
      // CB27
      {
        SMMU_HLOS_VMID,      //client vmid
        27,                  //interrupt index
        1,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        1,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { // Master SDC2
          0x803F0140, 0 },
      },
      // CB28
      {
        SMMU_HLOS_VMID,      //client vmid
        28,                  //interrupt index
        1,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        7,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { // Master AUDIO
          0x800101C2, 0x800101C4, 0x800101C6, 0x800101C8, 0x800101CC, 0x800101CE, 0x800F01D0,  0 },
      },
      // CB29
      {
        SMMU_HLOS_VMID,      //client vmid
        29,                  //interrupt index
        1,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        1,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { // Master USB_HS1
          0x803F02C0, 0 },
      },
      // CB30
      {
        SMMU_SROT_VMID,      //client vmid
        30,                  //interrupt index
        1,                   //Valid Context bank
        1,                   // secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        1,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { // Master BLSP
          // TODO: need the BLSP NS=0 mapping info?
          0x803F0280, 0 },
      },
      // CB31
      {
        SMMU_SROT_VMID,      //client vmid
        31,                  //interrupt index
        1,                   //Valid Context bank
        1,                   // secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        5,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { // Master CE (SSD: 0 - 3, 16, 17, 24, 25)
          0x802300C0, 0x800000D0, 0x800000D3, 0x800000F0, 0x800000F3, 0 },
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
        1,                   //Valid Context bank
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
        1,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        1,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0x80000000, 0 },
      },
      // CB2
      {
        SMMU_HLOS_VMID,      //client vmid
        2,                   //interrupt index
        1,                   //Valid Context bank
        0,                   //non-secure CB
        SMMU_CB_S1_CTX_S2_BYPASS, // CB type
        0,                   // S2 nested CB index (ignored if CB type != S2 nested)
        1,                   // number of SIDs to be configured
        // masked SIDs attached to this context bank
        { 0x80000001, 0 },
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
  {SMMU_NUM_MAX_DEVICES, smmuDeviceConfigData}
};
