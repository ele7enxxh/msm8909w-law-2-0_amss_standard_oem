/**
 * @file:  SpmiRadixTree.c
 * 
 * Copyright (c) 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2016/06/17 14:31:11 $
 * $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/spmi/src/core/hal/badger/SpmiRadixTree.c#1 $
 * $Change: 10727476 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 10/1/13  Initial Version
 */

#include "SpmiCfg.h"
#include "SpmiHal.h"

//******************************************************************************
// Local Helper Functions
//******************************************************************************

static uint32 SpmiMaster_CreateMappingTableEntry(uint32 uBitIndex,
                                                 uint32 uBranchResult0Flag,
                                                 uint32 uBranchResult0,
                                                 uint32 uBranchResult1Flag,
                                                 uint32 uBranchResult1)
{
    return ((uBitIndex          << HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_BIT_INDEX_SHFT)            & HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_BIT_INDEX_BMSK) |
           ((uBranchResult0Flag << HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_BRANCH_RESULT_0_FLAG_SHFT) & HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_BRANCH_RESULT_0_FLAG_BMSK) |
           ((uBranchResult0     << HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_BRANCH_RESULT_0_SHFT)      & HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_BRANCH_RESULT_0_BMSK) |
           ((uBranchResult1Flag << HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_BRANCH_RESULT_1_FLAG_SHFT) & HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_BRANCH_RESULT_1_FLAG_BMSK) |
           ((uBranchResult1     << HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_BRANCH_RESULT_1_SHFT)      & HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_BRANCH_RESULT_1_BMSK);
}

static void SpmiMaster_UpdateMappingImageEntry(uint8 ucEntryIndex,
                                               uint8 ucBitIndex,
                                               uint8 ucBitValue,
                                               boolean bIsResult,
                                               uint8 ucTarget,
                                               uint32* radixTree)
{
    uint32 uFlag;
    uint32 uOldEntryValue;
    uint32 uNewEntryValue;

    enum {
        RESULT = 0, BRANCH = 1
    };

    uFlag = (TRUE == bIsResult) ? RESULT : BRANCH;
    uOldEntryValue = radixTree[ucEntryIndex];

    uNewEntryValue = uOldEntryValue;

    if (1 == ucBitValue) {
        uNewEntryValue |= SpmiMaster_CreateMappingTableEntry( ucBitIndex,
                                                              0,
                                                              0,
                                                              uFlag,
                                                              ucTarget );
    } else /* 0 == bitValue */ {
        uNewEntryValue |= SpmiMaster_CreateMappingTableEntry( ucBitIndex,
                                                              uFlag,
                                                              ucTarget,
                                                              0,
                                                              0 );
    }
    radixTree[ucEntryIndex] = uNewEntryValue;
}

static uint8 SpmiMaster_CreateMappingImage(SpmiCfg_ChannelCfg* entries,
                                           uint32 numEntries,
                                           uint8 ucEntryIndex,
                                           uint8 ucMsbIndex,
                                           uint32 *radixTree)
{
    uint8 ucOnes = 0;
    uint16 usMsbCompareMask = (1 << ucMsbIndex);
    uint8 ucNewMsbIndex = ucMsbIndex - 1;
    uint8 ucNextEntryIndex = 0;

    /* Count number of values with a '1' at ucMsbIndex  */
    while ((ucOnes < numEntries) && (((entries[ucOnes].slaveId << 8) | entries[ucOnes].periphId) & usMsbCompareMask)) {
        ++ucOnes;
    }

    /* All relevant values match at MSB, continue to next ucMsbIndex */
    if ((ucOnes == numEntries) || (0 == ucOnes)) {
        return SpmiMaster_CreateMappingImage( entries,
                                              numEntries,
                                              ucEntryIndex,
                                              ucNewMsbIndex,
                                              radixTree );
    }

    /* Split is possible, first handle all values with '1' at ucMsbIndex */
    if (1 == ucOnes) { /* RESULT, target APID must be at (sortedPairs[0].apid) */
        SpmiMaster_UpdateMappingImageEntry( ucEntryIndex,
                                            ucMsbIndex,
                                            1,
                                            TRUE,
                                            entries[0].channel,
                                            radixTree );

        /* When deploying the subtree for '0' continue from ucEntryIndex+1 */
        ucNextEntryIndex = ucEntryIndex + 1;
    } else { /* BRANCH, target table entry is (ucEntryIndex+1) */
        SpmiMaster_UpdateMappingImageEntry( ucEntryIndex,
                                            ucMsbIndex,
                                            1,
                                            FALSE,
                                            ucEntryIndex + 1,
                                            radixTree );

        /* Continue recursively with all values with '1' at ucMsbIndex */
        ucNextEntryIndex = SpmiMaster_CreateMappingImage( entries,
                                                          ucOnes,
                                                          ucEntryIndex + 1,
                                                          ucNewMsbIndex,
                                                          radixTree );
    }

    /* Now handle all values with '0' at ucMsbIndex */
    if (ucOnes == numEntries - 1) { /* RESULT, target APID must be at (sortedPairs[uEntries-1].apid) */
        SpmiMaster_UpdateMappingImageEntry( ucEntryIndex,
                                            ucMsbIndex,
                                            0,
                                            TRUE,
                                            entries[numEntries - 1].channel,
                                            radixTree );
    } else {
        /* BRANCH to the entry after the subtree for '1' */
        SpmiMaster_UpdateMappingImageEntry( ucEntryIndex,
                                            ucMsbIndex,
                                            0,
                                            FALSE,
                                            ucNextEntryIndex,
                                            radixTree );

        /* Continue recursively with all values with '0' at ucMsbIndex */
        ucNextEntryIndex = SpmiMaster_CreateMappingImage( (&entries[ucOnes]),
                                                          (numEntries - ucOnes),
                                                          ucNextEntryIndex,
                                                          ucNewMsbIndex,
                                                          radixTree );
    }

    /* Return the entry after the last one which was configured */
    return ucNextEntryIndex;
}

//******************************************************************************
// Public API Functions
//******************************************************************************

void SpmiMaster_LoadRadixTree(SpmiCfg_ChannelCfg* entries, uint32 numEntries)
{
    uint32 k;
    uint32 radixTree[HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_MAXk + 1] = {0};
    
    if (1 == numEntries)
    {
        /* Configure the first entry to the APID result */
        SpmiMaster_UpdateMappingImageEntry( 0,
                                            0,
                                            entries[0].periphId & 0x1,
                                            TRUE,
                                            entries[0].channel,
                                            radixTree );
    } 
    else if (2 <= numEntries) {
        /* Configure recursively for any real mapping (2 entries or more) */
        SpmiMaster_CreateMappingImage( entries, numEntries, 0, 11, radixTree );
    }
    
    for(k = 0; k <= HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_MAXk; k++) {
        SPMI_HWIO_OUT( HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_ADDR( k ), radixTree[k] );
    }
}
