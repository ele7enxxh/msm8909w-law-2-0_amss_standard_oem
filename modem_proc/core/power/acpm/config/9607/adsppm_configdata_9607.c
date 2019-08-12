/*
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/


/*
 * adsspm_ConfigData.c - contains all 9607-specific data definitions for Config Manager
 *
 *  Created on: Jul 11, 2014
 *      Author: mkushal
 */
#include "adsppm.h"
#include "asic.h"
#include "asic_internal.h"
#include "adsppm_utils.h"
#include "icbid.h"
#include "DDIIPCInt.h"
#include "ultaudio_hwio_9607.h"
#include "msmhwiobase.h"

AsicHwioAddrRangeType lpassBaseAddr_9607 = { ULTAUDIO_CORE_BASE, ULTAUDIO_CORE_BASE_PHYS, ULTAUDIO_CORE_BASE_SIZE};

AsicCpuAggregatorMppsCppType cppFactors_9607[] = {
		{
				50, 	//MPPS
				1200    //CPP*1000
		},
		{
				100, 	//MPPS
				1500    //CPP*1000
		},
		{
				150, 	//MPPS
				1700    //CPP*1000
		},
		{
				200, 	//MPPS
				2200    //CPP*1000
		},
		{
				300, 	//MPPS
				2700    //CPP*1000
		},
		{
				32768, 	//MPPS
				3200    //CPP*1000
		}
};

AsicCpuAggregatorCppAdjustmentType cppFudgeFactors_9607[] = {
		{
					300000, 	//Footprint factor (BW_bps/(MPPS*1000) normalized to L2 size)
					1000    //CPP fudge factor (base 1000)
		}
};

AsicCpuAggregatorMppsCppArrayType cppFactorsArray_9607 = {
		ADSPPM_ARRAY(cppFactors_9607)
};

AsicCpuAggregatorCppAdjustmentArrayType cppFudgeFactorsArray_9607 = {
		ADSPPM_ARRAY(cppFudgeFactors_9607)
};


/**
 * Array of 9607 core descriptors. Arranged according to core ID enum
 */
AsicCoreDescType cores_array_9607[Adsppm_Core_Id_Max] = {
        {   //Adsppm_Core_Id_None
                Adsppm_Core_Id_None, // Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_None, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_None, //Sleep Latency
                        AsicRsc_None, //MIPS
                        AsicRsc_None, //BW
                        AsicRsc_None  //Thermal
                }, // HwResource IDs
                AsicPowerDomain_AON, // Power Domain
                AdsppmBusPort_None, // Master port
                AdsppmBusPort_None  // Slave port
        },
        {   //Adsppm_Core_Id_ADSP
                Adsppm_Core_Id_ADSP, // Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_Power_ADSP, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_Latency, //Sleep Latency
                        AsicRsc_MIPS_Clk | AsicRsc_MIPS_BW, //MIPS
                        AsicRsc_BW_Internal | AsicRsc_BW_External, //BW
                        AsicRsc_Thermal //Thermal
                }, // HwResource IDs
                AsicPowerDomain_Adsp, // Power Domain
                AdsppmBusPort_Adsp_Master, // Master port
                AdsppmBusPort_None  // Slave port
        },
        {   //Adsppm_Core_Id_LPM
                Adsppm_Core_Id_LPM, // Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_Power_Mem, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_Latency, //Sleep Latency
                        AsicRsc_MIPS_Clk | AsicRsc_MIPS_BW, //MIPS
                        AsicRsc_BW_Internal | AsicRsc_BW_External, //BW
                        AsicRsc_Thermal //Thermal
                }, // HwResource IDs
                AsicPowerDomain_LpassCore, // Power Domain
                AdsppmBusPort_None, // Master port
                AdsppmBusPort_Lpm_Slave  // Slave port
        },
        {   //Adsppm_Core_Id_DML
                Adsppm_Core_Id_None, // Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_None, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_None, //Sleep Latency
                        AsicRsc_None, //MIPS
                        AsicRsc_None, //BW
                        AsicRsc_None //Thermal
                }, // HwResource IDs
                AsicPowerDomain_AON, // Power Domain
                AdsppmBusPort_None, // Master port
                AdsppmBusPort_None  // Slave port
        },
        {   //Adsppm_Core_Id_AIF
                Adsppm_Core_Id_AIF, // Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_Power_Core, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_Latency, //Sleep Latency
                        AsicRsc_MIPS_Clk | AsicRsc_MIPS_BW, //MIPS
                        AsicRsc_BW_Internal | AsicRsc_BW_External, //BW
                        AsicRsc_Thermal //Thermal
                }, // HwResource IDs
                AsicPowerDomain_LpassCore, // Power Domain
                AdsppmBusPort_Aif_Master, // Master port
                AdsppmBusPort_Aif_Slave  // Slave port
        },
        {   //Adsppm_Core_Id_SlimBus
                Adsppm_Core_Id_None, // Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_None, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_None, //Sleep Latency
                        AsicRsc_None, //MIPS
                        AsicRsc_None, //BW
                        AsicRsc_None //Thermal
                }, // HwResource IDs
                AsicPowerDomain_LpassCore, // Power Domain
                AdsppmBusPort_None, // Master port
                AdsppmBusPort_None  // Slave port
        },
        {   //Adsppm_Core_Id_Midi
                Adsppm_Core_Id_None, // Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_None, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_None, //Sleep Latency
                        AsicRsc_None, //MIPS
                        AsicRsc_None, //BW
                        AsicRsc_None //Thermal
                }, // HwResource IDs
                AsicPowerDomain_AON, // Power Domain
                 AdsppmBusPort_None, // Master port
                 AdsppmBusPort_None  // Slave port
        },
        {   //Adsppm_Core_Id_AVsync
                Adsppm_Core_Id_AVsync, // Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_Power_Core, //Power
                        AsicRsc_Core_Clk, //Core Clock
                        AsicRsc_Latency, //Sleep Latency
                        AsicRsc_MIPS_Clk | AsicRsc_MIPS_BW, //MIPS
                        AsicRsc_BW_Internal | AsicRsc_BW_External, //BW
                        AsicRsc_Thermal //Thermal
                }, // HwResource IDs
                AsicPowerDomain_LpassCore, // Power Domain
                AdsppmBusPort_None, // Master port
                AdsppmBusPort_AvSync_Slave  // Slave port
        },
        {   //Adsppm_Core_Id_HWRSMP
                Adsppm_Core_Id_None, // Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_None, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_None, //Sleep Latency
                        AsicRsc_None, //MIPS
                        AsicRsc_None, //BW
                        AsicRsc_None //Thermal
                }, // HwResource IDs
                AsicPowerDomain_AON, // Power Domain
                AdsppmBusPort_None, // Master port
                AdsppmBusPort_None  // Slave port
        },
        {   //Adsppm_Core_Id_SRam
                Adsppm_Core_Id_None, // Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_None, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_None, //Sleep Latency
                        AsicRsc_None, //MIPS
                        AsicRsc_None, //BW
                        AsicRsc_None //Thermal
                }, // HwResource IDs
                AsicPowerDomain_AON, // Power Domain
                AdsppmBusPort_None, // Master port
                AdsppmBusPort_None  // Slave port
        },
        {   //Adsppm_Core_Id_DCodec
        		Adsppm_Core_Id_None, // Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_None, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_None, //Sleep Latency
                        AsicRsc_None, //MIPS
                        AsicRsc_None, //BW
                        AsicRsc_None //Thermal
                }, // HwResource IDs
                AsicPowerDomain_AON, // Power Domain
                AdsppmBusPort_None, // Master port
                AdsppmBusPort_None  // Slave port
        },
        {   //Adsppm_Core_Id_Spdif
                 Adsppm_Core_Id_None, // Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_None, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_None, //Sleep Latency
                        AsicRsc_None, //MIPS
                        AsicRsc_None, //BW
                        AsicRsc_None //Thermal
                }, // HwResource IDs
                AsicPowerDomain_AON, // Power Domain
                AdsppmBusPort_None, // Master port
                AdsppmBusPort_None  // Slave port
        },
		{   //Adsppm_Core_Id_Hdmirx
                Adsppm_Core_Id_None, // Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_None, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_None, //Sleep Latency
                        AsicRsc_None, //MIPS
                        AsicRsc_None, //BW
                        AsicRsc_None //Thermal
                }, // HwResource IDs
                AsicPowerDomain_AON, // Power Domain
                AdsppmBusPort_None, // Master port
                AdsppmBusPort_None  // Slave port
        },
        {   //Adsppm_Core_Id_Sif
                Adsppm_Core_Id_None, // Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_None, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_None, //Sleep Latency
                        AsicRsc_None, //MIPS
                        AsicRsc_None, //BW
                        AsicRsc_None //Thermal
                }, // HwResource IDs
                AsicPowerDomain_AON, // Power Domain
                AdsppmBusPort_None, // Master port
                AdsppmBusPort_None  // Slave port
        },
        {   //Adsppm_Core_Id_BSTC
                Adsppm_Core_Id_BSTC, // Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_Power_Core, //Power
                        AsicRsc_Core_Clk, //Core Clock
                        AsicRsc_Latency, //Sleep Latency
                        AsicRsc_MIPS_Clk | AsicRsc_MIPS_BW, //MIPS
                        AsicRsc_BW_Internal | AsicRsc_BW_External, //BW
                        AsicRsc_Thermal //Thermal
                }, // HwResource IDs
                AsicPowerDomain_LpassCore, // Power Domain
                AdsppmBusPort_None, // Master port
                AdsppmBusPort_Bstc_Slave  // Slave port
        }

};

AsicCoreDescriptorArrayType cores_9607 = {
		ADSPPM_ARRAY(cores_array_9607)
};


AsicMemDescType memories_array_9607[Adsppm_Mem_Max] = {
        {   //Adsppm_Mem_None
                Adsppm_Mem_None, // Mem ID
                AsicPowerDomain_AON, // Power Domain
        },
        {   //Adsppm_Mem_Lpass_LPM
                Adsppm_Mem_Lpass_LPM, // Core ID
                AsicPowerDomain_Lpm, // Power Domain
        }        
};

AsicMemDescriptorArrayType memories_9607 = {
		ADSPPM_ARRAY(memories_array_9607)
};

/**
 * Array of 9607 core clock descriptors. Arranged by core clock ID
 */
AsicClkDescriptorType clocks_array_9607[AdsppmClk_EnumMax] = {
        {
                AdsppmClk_None, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "",//Name
                AdsppmClk_None, //Source
                AsicPowerDomain_AON, //Power domain
				Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Adsp_Core, //Clk ID
                AsicClk_TypeNpa, //Clk Type
                AsicClk_CntlSW, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "/clk/cpu",//Name
                AdsppmClk_None, //Source
                AsicPowerDomain_AON, //Power domain
				Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Ahb_Root, //Clk ID
                AsicClk_TypeDalFreqSet, //Clk Type
                AsicClk_CntlSW_SHARED, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "gcc_ultaudio_ahbfabric_ixfabric_clk",//Name
                AdsppmClk_None, //Source
                AsicPowerDomain_AON, //Power domain
				Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Adsp_Hclk, //Clk ID - using for pcnoc sway clk
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlSW_SHARED,   // Cntl Type; 
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "gcc_ultaudio_pcnoc_sway_clk",//Name
                AdsppmClk_None, //Source
                AsicPowerDomain_AON, //Power domain
				Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_HwRsp_Hclk, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "",//Name
                AdsppmClk_None, //Source
                AsicPowerDomain_AON, //Power domain
				Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Dml_Hclk, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "",//Name
                AdsppmClk_None, //Source
                AsicPowerDomain_AON, //Power domain
				Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Aif_Hclk, //Clk ID
                AsicClk_TypeInternalCGC, //Clk Type
                AsicClk_CntlHW, //AsicClk_CntlHW, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.hwioInfo = {
                		HWIO_LPASS_AUDIO_CORE_LPAIF_CTL_CGCR_OFFS, //CGCR Offset
                		HWIO_LPASS_AUDIO_CORE_LPAIF_CTL_CGCR_CLK_ENABLE_BMSK, //Enable mask
                		HWIO_LPASS_AUDIO_CORE_LPAIF_CTL_CGCR_HW_CTL_BMSK, //HW control mask
                		0	//Status mask
                },
                AdsppmClk_None, //Source
                AsicPowerDomain_LpassCore, //Power domain
				Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Aif_Csr_Hclk, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlHW, //AsicClk_CntlHW, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.hwioInfo = {
                		HWIO_LPASS_AUDIO_CORE_LPAIF_CTL_CGCR_OFFS, //CGCR Offset
                		HWIO_LPASS_AUDIO_CORE_LPAIF_CTL_CGCR_CLK_ENABLE_BMSK, //Enable mask
                		HWIO_LPASS_AUDIO_CORE_LPAIF_CTL_CGCR_HW_CTL_BMSK, //HW control mask
                		0	//Status mask
                },
                AdsppmClk_None, //Source
                AsicPowerDomain_LpassCore, //Power domain
				Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Slimbus_Hclk, //Clk ID
                AsicClk_TypeNone, //Clk Type 
                AsicClk_CntlNone, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "",//Name
                AdsppmClk_None, //Source
                AsicPowerDomain_AON, //Power domain
				Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Midi_Hclk, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "",//Name
                AdsppmClk_None, //Source
                AsicPowerDomain_AON, //Power domain
				Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_AvSync_Hclk, //Clk ID
                AsicClk_TypeInternalCGC, //Clk Type
                AsicClk_CntlHW, //AsicClk_CntlHW, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.hwioInfo = {
                		HWIO_LPASS_AUDIO_CORE_AVSYNC_CGCR_OFFS, //CGCR Offset
                		HWIO_LPASS_AUDIO_CORE_AVSYNC_CGCR_CLK_ENABLE_BMSK, //Enable mask
                		HWIO_LPASS_AUDIO_CORE_AVSYNC_CGCR_HW_CTL_BMSK, //HW control mask
                		0	//Status mask
                },
                AdsppmClk_Bstc_Hclk, //Source. Relying on recursion to enable both AVSync and AudSync Hclks
                AsicPowerDomain_LpassCore, //Power domain
				Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Lpm_Hclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlSW, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "gcc_ultaudio_ahbfabric_ixfabric_lpm_clk",//Name
                AdsppmClk_None, //Source
                AsicPowerDomain_AON, //Power domain
				Adsppm_Mem_Lpass_LPM //Memory ID
        },
        {
                AdsppmClk_Sram_Hclk, //Clk ID. Using this for LPM CGC
                AsicClk_TypeInternalCGC, //Clk Type
                AsicClk_CntlHW,  // Cntl Type. TODO: Fall back to AsicClk_CntlAlwaysON if HW control doesn't work
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.hwioInfo = {
                		HWIO_LPASS_AUDIO_CORE_LPM_CGCR_OFFS, //CGCR Offset
                		HWIO_LPASS_AUDIO_CORE_LPM_CGCR_CLK_ENABLE_BMSK, //Enable mask
                		HWIO_LPASS_AUDIO_CORE_LPM_CGCR_HW_CTL_BMSK, //HW control mask
                		0	//Status mask
                },
                AdsppmClk_None, //Source. None is used to prevent LPM CBC to be ON all the time.
                AsicPowerDomain_LpassCore, //Power domain
				Adsppm_Mem_Lpass_LPM //Memory ID
        },
        {
                AdsppmClk_Lcc_Hclk, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "",//Name
                AdsppmClk_None, //Source
                AsicPowerDomain_AON, //Power domain
				Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_AhbE_Hclk, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "",//Name
                AdsppmClk_None, //Source
                AsicPowerDomain_AON, //Power domain
				Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_AhbI_Hclk, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "",//Name
                AdsppmClk_None, //Source
                AsicPowerDomain_AON, //Power domain
				Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_AhbX_Hclk, //Clk ID
                AsicClk_TypeInternalCGC, //Clk Type
                AsicClk_CntlAlwaysON, //AsicClk_CntlHW, // Cntl Type. TODO: Fall back to AsicClk_CntlAlwaysON if HW control doesn't work
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.hwioInfo = {
                		HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_MST_CGCR_OFFS, //CGCR Offset
                		HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_MST_CGCR_CLK_ENABLE_BMSK, //Enable mask
                		HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_MST_CGCR_HW_CTL_BMSK, //HW control mask
                		0	//Status mask
                },
                AdsppmClk_None, //Source
                AsicPowerDomain_LpassCore, //Power domain
				Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Csr_Hclk, //Clk ID Using this for VFR clock in Bear
                AsicClk_TypeInternalCGC, //Clk Type
                AsicClk_CntlAlwaysON, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.hwioInfo = {
                		HWIO_LPASS_AUDIO_CORE_VFR_CGCR_OFFS, //CGCR Offset
                		HWIO_LPASS_AUDIO_CORE_VFR_CGCR_CLK_ENABLE_BMSK, //Enable mask
                		HWIO_LPASS_AUDIO_CORE_VFR_CGCR_HW_CTL_BMSK, //HW control mask
                		0	//Status mask
                },
                AdsppmClk_None, //Source
                AsicPowerDomain_LpassCore, //Power domain
				Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Security_Hclk, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "",//Name
                AdsppmClk_None, //Source
                AsicPowerDomain_AON, //Power domain
				Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Wrapper_Security_Hclk, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "",//Name
                AdsppmClk_None, //Source
                AsicPowerDomain_AON, //Power domain
				Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Wrapper_Br_Hclk, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "",//Name
                AdsppmClk_None, //Source
                AsicPowerDomain_AON, //Power domain
				Adsppm_Mem_None //Memory ID
        },
		{
                AdsppmClk_Dcodec_Hclk, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "",//Name

                AdsppmClk_None, //Source
                AsicPowerDomain_AON, //Power domain
				Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Spdif_Hmclk, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "",//Name
                AdsppmClk_None, //Source
                AsicPowerDomain_AON, //Power domain
				Adsppm_Mem_None //Memory ID
        },
		{
                AdsppmClk_Spdif_Hsclk, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "",//Name
                AdsppmClk_None, //Source
                AsicPowerDomain_AON, //Power domain
				Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Smmu_Adsp_Hclk, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "",//Name
                AdsppmClk_None, //Source
                AsicPowerDomain_AON, //Power domain
				Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Smmu_Lpass_Hclk, //Clk ID
                AsicClk_TypeInternalCGC, //Clk Type
                AsicClk_CntlAlwaysON, //AsicClk_CntlHW, // Cntl Type. TODO: Fall back to AsicClk_CntlAlwaysON if HW control doesn't work
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.hwioInfo = {
                		HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_SLV_CGCR_OFFS, //CGCR Offset
                		HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_SLV_CGCR_CLK_ENABLE_BMSK, //Enable mask
                		HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_SLV_CGCR_HW_CTL_BMSK, //HW control mask
                		0	//Status mask
                },
                AdsppmClk_None, //Source
                AsicPowerDomain_LpassCore, //Power domain
				Adsppm_Mem_None //Memory ID
        },
		{
                AdsppmClk_Smmu_Sysnoc_Hclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlSW, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "gcc_ultaudio_pcnoc_mport_clk",//Name
                AdsppmClk_None, //Source
                AsicPowerDomain_AON, //Power domain
				Adsppm_Mem_None //Memory ID
        },
		{
                AdsppmClk_Hdmirx_Hclk, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "", //Name
                AdsppmClk_None, //Source
                AsicPowerDomain_AON, //Power domain
				Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Sif_Hclk, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "", //Name
                AdsppmClk_None, //Source
                AsicPowerDomain_AON, //Power domain
				Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Bstc_Hclk, //Clk ID
                AsicClk_TypeInternalCGC, //Clk Type
                AsicClk_CntlHW, //AsicClk_CntlHW, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.hwioInfo = {
                		HWIO_LPASS_AUDIO_CORE_AUDSYNC_CGCR_OFFS, //CGCR Offset
                		HWIO_LPASS_AUDIO_CORE_AUDSYNC_CGCR_CLK_ENABLE_BMSK, //Enable mask
                		HWIO_LPASS_AUDIO_CORE_AUDSYNC_CGCR_HW_CTL_BMSK, //HW control mask
                		0	//Status mask
                },
                AdsppmClk_None, //Source
                AsicPowerDomain_LpassCore, //Power domain
				Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_HwRsp_Core, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "",//Name
                AdsppmClk_None, //Source
                AsicPowerDomain_AON, //Power domain
				Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Midi_Core, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "",//Name
                AdsppmClk_None, //Source
                AsicPowerDomain_AON, //Power domain
				Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_AvSync_Xo, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlSW, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "gcc_ultaudio_avsync_xo_clk",//Name
                AdsppmClk_None, //Source
                AsicPowerDomain_AON, //Power domain
				Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_AvSync_Bt, //Clk ID
                AsicClk_TypeInternalCGC, //Clk Type
                AsicClk_CntlHW, //AsicClk_CntlHW, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.hwioInfo = {
                		HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_BT_CGCR_OFFS, //CGCR Offset
                		HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_BT_CGCR_CLK_ENABLE_BMSK, //Enable mask
                		HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_BT_CGCR_HW_CTL_BMSK, //HW control mask
                		0	//Status mask
                },
                AdsppmClk_AvSync_Xo, //Source
                AsicPowerDomain_LpassCore, //Power domain
				Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_AvSync_Fm, //Clk ID
                AsicClk_TypeInternalCGC, //Clk Type
                AsicClk_CntlHW, //AsicClk_CntlHW, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.hwioInfo = {
                		HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_FM_CGCR_OFFS, //CGCR Offset
                		HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_FM_CGCR_CLK_ENABLE_BMSK, //Enable mask
                		HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_FM_CGCR_HW_CTL_BMSK, //HW control mask
                		0	//Status mask
                },
                AdsppmClk_AvSync_Xo, //Source
                AsicPowerDomain_LpassCore, //Power domain
				Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Slimbus_Core, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "",//Name
                AdsppmClk_None, //Source
                AsicPowerDomain_AON, //Power domain
				Adsppm_Mem_None //Memory ID
        },
		{
                AdsppmClk_Avtimer_core, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlSW, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "gcc_ultaudio_stc_xo_clk",//Name
                AdsppmClk_None, //Source
                AsicPowerDomain_AON, //Power domain
				Adsppm_Mem_None //Memory ID
        },
		{
                AdsppmClk_Atime_core, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "",//Name
                AdsppmClk_None, //Source
                AsicPowerDomain_AON, //Power domain
				Adsppm_Mem_None //Memory ID
        },
		{
                AdsppmClk_Atime2_core, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "", //Name
                AdsppmClk_None, //Source
                AsicPowerDomain_AON, //Power domain
				Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_BSTC_core, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "",//
                AdsppmClk_Avtimer_core, //Source
                AsicPowerDomain_AON, //Power domain
				Adsppm_Mem_None //Memory ID
        },

};

AsicClockDescriptorArrayType clocks_9607 = {
		ADSPPM_ARRAY(clocks_array_9607)
};

/**
 * Array of 9607 Bus port descriptors arranged by Bus port ID
 */
AsicBusPortDescriptorType busPorts_array_9607[AdsppmBusPort_EnumMax] = {
        {
                AdsppmBusPort_None, //ID
                0, //Connection
                AdsppmClk_None, //Bus clock
                AdsppmClk_None, //Csr clock
                {.icbarbMaster = ICBID_MASTER_APPSS_PROC}, // icbarb ID
                AdsppmBusPort_None // Access port
        },
        {
                AdsppmBusPort_Adsp_Master, //ID
                AsicBusPort_Ext_M , //Connection
                AdsppmClk_AhbX_Hclk, //Bus clock
                AdsppmClk_AhbX_Hclk, //Csr clock
                {.icbarbMaster = ICBID_MASTER_MSS_PROC}, // icbarb ID
                AdsppmBusPort_Adsp_Master // Access port
        },
        {
                AdsppmBusPort_Dml_Master, //ID
                0, //Connection
                AdsppmClk_None, //Bus clock
                AdsppmClk_None, //Csr clock
                {.icbarbMaster = ICBID_MASTER_APPSS_PROC}, // icbarb ID
                AdsppmBusPort_None // Access port
        },
        {
                AdsppmBusPort_Aif_Master, //ID
                AsicBusPort_AhbX_M, //Connection
                AdsppmClk_Aif_Hclk, //Bus clock
                AdsppmClk_Aif_Hclk, //Csr clock
                {.icbarbMaster = ICBID_MASTER_AUDIO}, // icbarb ID
                AdsppmBusPort_Ext_Ahb_Master // Access port
        },
        {
                AdsppmBusPort_Slimbus_Master, //ID
                0, //Connection
                AdsppmClk_None, //Bus clock
                AdsppmClk_None, //Csr clock
                {.icbarbMaster = ICBID_MASTER_AUDIO}, // icbarb ID
                AdsppmBusPort_None // Access port
        },
        {
                AdsppmBusPort_Midi_Master, //ID
                0, //Connection
                AdsppmClk_None, //Bus clock
                AdsppmClk_None, //Csr clock
                {.icbarbMaster = ICBID_MASTER_APPSS_PROC}, // icbarb ID
                AdsppmBusPort_None // Access port
        },
        {
                AdsppmBusPort_HwRsmp_Master, //ID
                 0, //Connection
                AdsppmClk_None, //Bus clock
                AdsppmClk_None, //Csr clock
                {.icbarbMaster = ICBID_MASTER_APPSS_PROC}, // icbarb ID
                AdsppmBusPort_None // Access port
        },
        {
                AdsppmBusPort_Ext_Ahb_Master, //ID
                AsicBusPort_Ext_M, //Connection
                AdsppmClk_AhbX_Hclk, //Bus clock
                AdsppmClk_AhbX_Hclk, //Csr clock
                {.icbarbMaster = ICBID_MASTER_AUDIO}, // icbarb ID
                AdsppmBusPort_Ext_Ahb_Master // Access port
        },
        {
                AdsppmBusPort_Spdif_Master, //ID
                 0, //Connection
                AdsppmClk_None, //Bus clock
                AdsppmClk_None, //Csr clock
                {.icbarbMaster = ICBID_MASTER_APPSS_PROC}, // icbarb ID
                AdsppmBusPort_None // Access port
        },
		{
                AdsppmBusPort_Hdmirx_Master, //ID
                0, //Connection
                AdsppmClk_None, //Bus clock
                AdsppmClk_None, //Csr clock
                {.icbarbMaster = ICBID_MASTER_APPSS_PROC}, // icbarb ID
                AdsppmBusPort_None // Access port
        },
        {
                AdsppmBusPort_Sif_Master, //ID
                0, //Connection
                AdsppmClk_None, //Bus clock
                AdsppmClk_None, //Csr clock
                {.icbarbMaster = ICBID_MASTER_APPSS_PROC}, // icbarb ID
                AdsppmBusPort_None // Access port
        },
        {
                AdsppmBusPort_Dml_Slave, //ID
                0, //Connection
                AdsppmClk_None, //Bus clock
                AdsppmClk_None, //Csr clock
                {.icbarbSlave = ICBID_SLAVE_AUDIO}, // icbarb ID
                AdsppmBusPort_None // Access port
        },
        {
                AdsppmBusPort_Aif_Slave, //ID
                AsicBusPort_AhbX_S, //Connection
                AdsppmClk_Aif_Hclk, //Bus clock
                AdsppmClk_Aif_Hclk, //Csr clock
                {.icbarbSlave = ICBID_SLAVE_AUDIO}, // icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave // Access port
        },
        {
                AdsppmBusPort_Slimbus_Slave, //ID
                0, //Connection
                AdsppmClk_None, //Bus clock
                AdsppmClk_None, //Csr clock
                {.icbarbSlave = ICBID_SLAVE_AUDIO}, // icbarb ID
                AdsppmBusPort_None // Access port
        },
        {
                AdsppmBusPort_Midi_Slave, //ID
                0, //Connection
                AdsppmClk_None, //Bus clock
                AdsppmClk_None, //Csr clock
                {.icbarbSlave = ICBID_SLAVE_AUDIO}, // icbarb ID
                AdsppmBusPort_None // Access port
        },
        {
                AdsppmBusPort_HwRsmp_Slave, //ID
                0, //Connection
                AdsppmClk_None, //Bus clock
                AdsppmClk_None, //Csr clock
                {.icbarbSlave = ICBID_SLAVE_AUDIO}, // icbarb ID
                AdsppmBusPort_None // Access port
        },
        {
                AdsppmBusPort_AvSync_Slave, //ID
                AsicBusPort_AhbX_S, //Connection
                AdsppmClk_AvSync_Hclk, //Bus clock
                AdsppmClk_AvSync_Xo, //Csr clock
                {.icbarbSlave = ICBID_SLAVE_AUDIO}, // icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave // Access port
        },
        {
                AdsppmBusPort_Lpm_Slave, //ID
                AsicBusPort_AhbX_S, //Connection
                AdsppmClk_Lpm_Hclk, //Bus clock
                AdsppmClk_Lpm_Hclk, //Csr clock
                {.icbarbSlave = ICBID_SLAVE_AUDIO}, // icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave // Access port
        },
        {
                AdsppmBusPort_Sram_Slave, //ID
                0, //Connection
                AdsppmClk_None, //Bus clock
                AdsppmClk_None, //Csr clock
                {.icbarbSlave = ICBID_SLAVE_AUDIO}, // icbarb ID
                AdsppmBusPort_None // Access port
        },
        {
                AdsppmBusPort_Ext_Ahb_Slave, //ID
                AsicBusPort_Ext_S, //Connection
                AdsppmClk_Smmu_Lpass_Hclk, //Bus clock
                AdsppmClk_Smmu_Lpass_Hclk, //Csr clock
                {.icbarbSlave = ICBID_SLAVE_AUDIO}, // icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave // Access port
        },
        {
                AdsppmBusPort_Ddr_Slave, //ID
                AsicBusPort_Ext_S, //Connection
                AdsppmClk_Smmu_Sysnoc_Hclk, //Bus clock
                AdsppmClk_Smmu_Sysnoc_Hclk, //Csr clock
                {.icbarbSlave = ICBID_SLAVE_EBI1}, // icbarb ID
                AdsppmBusPort_Ext_Ahb_Master // Access port
        },
		{
                AdsppmBusPort_Ocmem_Slave, //ID
                0, //Connection
                AdsppmClk_None, //Bus clock
                AdsppmClk_None, //Csr clock
                {.icbarbSlave = ICBID_SLAVE_OCMEM}, // icbarb ID
                AdsppmBusPort_None // Access port
        },
        {
                AdsppmBusPort_PerifNoc_Slave, //ID
                0, //Connection
                AdsppmClk_None, //Bus clock
                AdsppmClk_None, //Csr clock
                {.icbarbSlave = ICBID_SLAVE_BLSP_1}, // icbarb ID
                AdsppmBusPort_None // Access port
        },
        {
        		AdsppmBusPort_DCodec_Slave, //ID
        		0, //Connection
        		AdsppmClk_None, //Bus clock
        		AdsppmClk_None, //Csr clock
        		{.icbarbSlave = ICBID_SLAVE_AUDIO}, // icbarb ID
        		AdsppmBusPort_None // Access port
        },
        {
                AdsppmBusPort_Spdif_Slave, //ID
                0, //Connection
                AdsppmClk_None, //Bus clock
                AdsppmClk_None, //Csr clock
                {.icbarbSlave = ICBID_SLAVE_AUDIO}, // icbarb ID
                AdsppmBusPort_None // Access port
        },
		{
                AdsppmBusPort_Hdmirx_Slave, //ID
                0, //Connection
                AdsppmClk_None, //Bus clock
                AdsppmClk_None, //Csr clock
                {.icbarbSlave = ICBID_SLAVE_AUDIO}, // icbarb ID
                AdsppmBusPort_None // Access port
        },
        {
                AdsppmBusPort_Sif_Slave, //ID
                0, //Connection
                AdsppmClk_None, //Bus clock
                AdsppmClk_None, //Csr clock
                {.icbarbSlave = ICBID_SLAVE_AUDIO}, // icbarb ID
                AdsppmBusPort_None // Access port
        },
        {
                AdsppmBusPort_Bstc_Slave, //ID
                AsicBusPort_AhbX_S, //Connection
                AdsppmClk_Bstc_Hclk, //Bus clock
                AdsppmClk_None, //Csr clock, Need to be added in clk structure
                {.icbarbSlave = ICBID_SLAVE_AUDIO}, // icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave // Access port
        }
};


AsicBusPortDescriptorArrayType busPorts_9607 = {
		ADSPPM_ARRAY(busPorts_array_9607)
};

/**
 * List of supported external bus routes
 * Note: Must sync with NUMBER_OF_EXTERNAL_BUS_ROUTES in adsppm.xml
 */
AdsppmBusRouteType extBusRoutes_array_9607[] = {
        {
                AdsppmBusPort_Adsp_Master, //master
                AdsppmBusPort_Ddr_Slave //slave
        },
        {
                AdsppmBusPort_Adsp_Master, //master
                AdsppmBusPort_Ext_Ahb_Slave //slave
        },
        {
                AdsppmBusPort_Ext_Ahb_Master, //master
                AdsppmBusPort_Ddr_Slave //slave
        }
};

AsicBusRouteArrayType extBusRoutes_9607 = {
		ADSPPM_ARRAY(extBusRoutes_array_9607)
};

AdsppmBusRouteType mipsBwRoutes_array_9607[] =
{
      {
              AdsppmBusPort_Adsp_Master,
              AdsppmBusPort_Ddr_Slave
      }
};

AsicBusRouteArrayType mipsBwRoutes_9607 = {
		ADSPPM_ARRAY(mipsBwRoutes_array_9607)
};

/**
 * Array of power domain descriptors
 */
AsicPowerDomainDescriptorType pwrDomains_array_9607[AsicPowerDomain_EnumMax] = {
        {
                AsicPowerDomain_AON, //Id
                "", //Name
                AsicRsc_None, //Type
                AdsppmClk_None, // Clock ID
                AdsppmBusPort_None //Bus Master
        },
        {
                AsicPowerDomain_Adsp, //Id
                "/core/cpu/vdd", //Name
                AsicRsc_Power_ADSP, //Type
                AdsppmClk_None, // Clock ID
                AdsppmBusPort_None //Bus Master
        },
        {
                AsicPowerDomain_LpassCore, //Id Should we change it to AON or make new ULT AUDIO Macro??
                "", //Name
                AsicRsc_Power_Core, //Type
                AdsppmClk_Ahb_Root, // Clock ID
                AdsppmBusPort_Ext_Ahb_Master // Bus Master, for virtio bringup
        },
        {
                AsicPowerDomain_Lpm, //Id
                "", //Name
                AsicRsc_Power_Mem, //Type
                AdsppmClk_Lpm_Hclk, // Clock ID
                AdsppmBusPort_None //Bus Master
        },
        {
                AsicPowerDomain_SRam, //Id
                "", //Name
                AsicRsc_None, //Type
                AdsppmClk_None, // Clock ID
                AdsppmBusPort_None //Bus Master
        }
};
   
AsicPwrDescriptorArrayType pwrDomains_9607 = {
		ADSPPM_ARRAY(pwrDomains_array_9607)
};


AsicFeatureDescType features_9607 [AsicFeatureId_enum_max] =
{
        //AsicFeatureId_Adsp_Clock_Scaling
        {
                AsicFeatureState_Enabled,
                80, //min MIPS
                492, //max MIPS
        },
        //AsicFeatureId_Adsp_LowTemp_Voltage_Restriction
        {
                AsicFeatureState_Disabled,
                0, //min
                0, //max
        },
        //AsicFeatureId_Adsp_PC
        {
                AsicFeatureState_Enabled,
                0, //min
                0, //max
        },
        //AsicFeatureId_Ahb_Scaling
        {
                AsicFeatureState_Enabled,
                32768, //min bytes per second
                561736000, //max bytes per second
        },
        //AsicFeatureId_Ahb_Sw_CG
        {
                AsicFeatureState_Enabled,
                0, //min
                0, //max
        },
        //AsicFeatureId_Ahb_DCG
        {
                AsicFeatureState_Disabled,
                0, //min
                0, //max
        },
        //AsicFeatureId_LpassCore_PC
        {
                AsicFeatureState_Disabled,
                0, //min
                0, //max
        },
        //AsicFeatureId_LpassCore_PC_TZ_Handshake
        {
                AsicFeatureState_Disabled,
                0, //min
                0, //max
        },
        //AsicFeatureId_Bus_Scaling
        {
                AsicFeatureState_Enabled,
                32768, //min bytes per second
                0x7FFFFFFF, //max bytes per second
        },
        //AsicFeatureId_CoreClk_Scaling
        {
                AsicFeatureState_Enabled,
                0, //min Hz
                140434000, //max Hz
        },
        //AsicFeatureId_Min_Adsp_BW_Vote
        {
                AsicFeatureState_Disabled,
                10000000, //min bytes per second
                0x7FFFFFFF, //max bytes per second
        },
        //AsicFeatureId_InitialState
        {
                AsicFeatureState_Enabled,
                0, //min
                0, //max
        },
        //AsicFeatureId_TimelineOptimisationMips
        {
                AsicFeatureState_Disabled,
                240, //min MIPS for periodic clients
                600  //max MIPS value
        },
        //AsicFeatureId_TimelineOptimisationBw
        {
                AsicFeatureState_Disabled,
                30000000, //min ext BW in bytes per second
                1000000000, //max ext BW in bytes per second
        },
        //AsicFeatureId_TimelineOptimisationAhb
        {
                AsicFeatureState_Enabled,
                19000000, //min AHB clock in Hz
                141000000, //max AHB clock in Hz
        },
        //AsicFeatureId_LpassClkSleepOptimization
        {
                AsicFeatureState_Disabled,
                0, //min
                0, //max
        }
};
