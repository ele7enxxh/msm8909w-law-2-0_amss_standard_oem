/**
 * @file boot_gpt_partition_id.c
 * @brief
 * Source file contains the GUID for sbl2,sbl3,tz,rpm and appsbl
 *
 */

/*==========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_gpt_partition_id.c#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
    
when       who     what, where, why
--------   ---     ---------------------------------------------------------
03/21/14   ck      Added GUID for SEC partition
10/17/13   ck      Split TZ into QSEE and QHEE and created GUID's for them.
03/19/13   dh      Added GUID for ram dump parition
12/05/12   jz      Added GUID for CDT partition
07/20/12   dh      Add GUID for ddr training parameter partition
03/22/12   dh      Add GUID for wdog debug image
04/05/11   plc     Add GUID info for SBL1 
03/24/11   dh      Initial creation 

============================================================================
                     Copyright 2014 Qualcomm Technologies Incorporated.
                            All Rights Reserved.
                    Qualcomm Confidential and Proprietary
===========================================================================*/

/*==========================================================================
                             INCLUDE FILES
===========================================================================*/
#include "boot_comdef.h"
#include "fs_hotplug.h"

/*define GUID for SBL1*/
/*This is the GUID that PBL expects to identify and load the first SBL*/
struct hotplug_guid sbl1_partition_id = 
      /*{DEA0BA2C-CBDD-4805-B4F9-F428251C3E98}*/
      { 0xDEA0BA2C, 0xCBDD, 0x4805, { 0xB4, 0xF9, 0xF4, 0x28, 0x25, 0x1C, 0x3E, 0x98 } };

/*define GUID for SBL2*/
struct hotplug_guid sbl2_partition_id = 
      /*{8C6B52AD-8A9E-4398-AD09-AE916E53AE2D}*/
      { 0x8C6B52AD, 0x8A9E, 0x4398, { 0xAD, 0x09, 0xAE, 0x91, 0x6E, 0x53, 0xAE, 0x2D } };

/*define GUID for SBL3*/       
struct hotplug_guid sbl3_partition_id = 
      /*{05E044DF-92F1-4325-B69E-374A82E97D6E}*/
      { 0x05E044DF, 0x92F1, 0x4325, { 0xB6, 0x9E, 0x37, 0x4A, 0x82, 0xE9, 0x7D, 0x6E } };
      
/*define GUID for APPSBL*/      
struct hotplug_guid appsbl_partition_id = 
      /*{400FFDCD-22E0-47E7-9A23-F16ED9382388}*/
      { 0x400FFDCD, 0x22E0, 0x47E7, { 0x9A, 0x23, 0xF1, 0x6E, 0xD9, 0x38, 0x23, 0x88 } };
      
/*define GUID for QSEE*/
struct hotplug_guid qsee_partition_id = 
      /*{A053AA7F-40B8-4B1C-BA08-2F68AC71A4F4}*/
      { 0xA053AA7F, 0x40B8, 0x4B1C, { 0xBA, 0x08, 0x2F, 0x68, 0xAC, 0x71, 0xA4, 0xF4 } };

/*define GUID for QHEE*/
struct hotplug_guid qhee_partition_id = 
      /*{E1A6A689-0C8D-4CC6-B4E8-55A4320FBD8A}*/
      { 0xE1A6A689, 0x0C8D, 0x4CC6, { 0xB4, 0xE8, 0x55, 0xA4, 0x32, 0x0F, 0xBD, 0x8A } };
 
/*define GUID for RPM*/      
struct hotplug_guid rpm_partition_id = 
      /*{098DF793-D712-413D-9D4E-89D711772228}*/
      { 0x098DF793, 0xD712, 0x413D, { 0x9D, 0x4E, 0x89, 0xD7, 0x11, 0x77, 0x22, 0x28 } };
      
/*define GUID for wdog debug image*/      
struct hotplug_guid wdt_partition_id = 
      /*{D4E0D938-B7FA-48C1-9D21-BC5ED5C4B203}*/
      { 0xD4E0D938, 0xB7FA, 0x48C1, { 0x9D, 0x21, 0xBC, 0x5E, 0xD5, 0xC4, 0xB2, 0x03 } };
      
/*define GUID for DDR params partiton*/      
struct hotplug_guid ddr_params_partition_id =
      /*{20A0C19C-286A-42FA-9CE7-F64C3226A794}*/
      { 0x20A0C19C, 0x286A, 0x42FA, { 0x9C, 0xE7, 0xF6, 0x4C, 0x32, 0x26, 0xA7, 0x94 } };
      
/*define GUID for CDT partition*/
struct hotplug_guid cdt_partition_id =
      /*{a19f205f-ccd8-4b6d-8f1e-2d9bc24cffb1}*/
      { 0xA19F205F, 0xCCD8, 0x4B6D, { 0x8F, 0x1E, 0x2D, 0x9B, 0xC2, 0x4C, 0xFF, 0xB1 } };

/*define GUID for Ram Dump partition*/
struct hotplug_guid ram_dump_partition_id =
      /*{66C9B323-F7FC-48B6-BF96-6F32E335A428}*/
      { 0x66C9B323, 0xF7FC, 0x48B6, { 0xBF, 0x96, 0x6F, 0x32, 0xE3, 0x35, 0xA4, 0x28 } };

/*define GUID for SEC partition*/
struct hotplug_guid sec_partition_id =
      /*{303E6AC3-AF15-4C54-9E9B-D9A8FBECF401}*/
      { 0x303E6AC3, 0xAF15, 0x4C54, { 0x9E, 0x9B, 0xD9, 0xA8, 0xFB, 0xEC, 0xF4, 0x01 } };
