
#ifndef LPASSHWIO_DEVCFG_H
#define LPASSHWIO_DEVCFG_H

#include "typedef_drivers.h"
#include "LPASS_ADDRESS_FILE.h"

#define MAX_AUDIF_TYPE_NUM   9
#define AFE_MMPM_AXI_BW_STEPS 8


#define NUM_MAX_VFR_SRC    4

#define AUDIOIF_DEVCFG_DIR_SINK         0x1
#define AUDIOIF_DEVCFG_DIR_SOURCE       0x2

#define   HW_VERSION__MAJOR___M        ULTAUDIO_HW_VERSION__MAJOR___M
#define   HW_VERSION__MINOR___M        ULTAUDIO_HW_VERSION__MINOR___M
#define   HW_VERSION__STEP___M         ULTAUDIO_HW_VERSION__STEP___M

#define LPASS_HW_VER_3_0_2    (0x30000002) /* BIT: 31:28, Major verion 
                                              BIT: 27:16, Minor verion
                                              BIT: 15:0, step */


#define LPASS_HW_VER_3_5_0    (0x30050000) /* BIT: 31:28, Major verion 
                                              BIT: 27:16, Minor verion
                                              BIT: 15:0, step */
                                           
/* This is audio if H/W version (ult_audio version), it is created to identify 9607 audio if config (clk table) and HAL function table*/
#define LPASS_HW_VER_1_6_0    (0x10060000)

/* This is audio if H/W version (ult_audio version), it is created to identify 8909 audio if config*/
#define LPASS_HW_VER_1_4_0    (0x10040000)

#define NUM_MAX_AVTIMER_HW_INSTANCE      3

enum
{
   TIMESTAMP_LATCH_TYPE_SOFTWARE = 0,
   TIMESTAMP_LATCH_TYPE_HARDWARE
};

typedef struct
{
   uint32 hw_ver_reg_base;   /* LPASS hw version reg base addr */
}HwdLpassPropertyType;

typedef struct
{
   uint32 audHwRegBase;   // lpaif reg base addr
   uint32 audHwRegSize;   // lpaif reg page size
   uint32 hdmiAudHwRegBase;   // hdmi audio reg base addr
   uint32 hdmiAudHwRegSize;   // hdmi audio reg page size
   uint32 dmaWordSize;   // DMA word size based on byte
   uint32 outgoingDmaChCnt;   // read DMA channel count
   uint32 incomingDmaChCnt;   // write DMA channel count
   uint32 infTypeCnt;   // # of lpaif interface type count
   uint32 infTypeList[MAX_AUDIF_TYPE_NUM];    // cast to AudioHWInterfaceType
   uint32 infTypeDataLineCaps[MAX_AUDIF_TYPE_NUM]; // maximum # of channel for audio if.
   uint32 infTypeDirection[MAX_AUDIF_TYPE_NUM];  // 0x1 spacker path only,  0x2 mic path only,   0x3: bidirectional 
   uint32 isrHookPinNum;   // lpaif intr irq #
   uint32 intrOutputIndex;   // interrupt line #
   uint32 hw_revision;   // lpaif hw revision
   uint32 avtimer_latch_type;  // Indicates if hardware latching support for AV-timer present
} HwdAudioIfPropertyType;

typedef struct
{
   uint32 baseRegAddr[NUM_MAX_AVTIMER_HW_INSTANCE];   // avtimer reg base addr
   uint32 regSize[NUM_MAX_AVTIMER_HW_INSTANCE];   // avtimer reg page size
   uint32 rootClk[NUM_MAX_AVTIMER_HW_INSTANCE];   // avtimer reg page size
   uint32 isrHookPinNum;   // avtimer intr irq #
   uint32 hw_revision;   // avtimer hw revision
} HwdAvtimerPropertyType;

typedef struct
{
   uint32   axi_bw[AFE_MMPM_AXI_BW_STEPS]; /* AXI bus bandwidth steps. Bus
                                              bandwidth is in terms of Bytes per
                                              second.*/
   uint32   mcps_scaling[AFE_MMPM_AXI_BW_STEPS]; /* CPP factors or MCPS scaling
                                                    factors which would be used
                                                    for scaling MPPS numbers.
                                                    These scaling factors will be
                                                    over all percentage of MPPS
                                                    numbers.*/
} HwdMmpmPropertyType;

typedef struct
{
   uint32 baseRegAddr;   // dmlite reg base addr
   uint32 regSize;   // dmlite reg page size
   uint32 intPinNo;   // dmlite intr irq #
   uint32 hw_revision;   // dmlite hw revision
} HwdDmlitePropertyType;

typedef struct
{
   uint32 baseRegAddr;   // hw resampler reg base addr
   uint32 regSize;   // hw resampler reg page size
   uint32 isrHookPinNum;   // hw resampler intr irq #
   uint32 hw_revision;   // resampler hw revision
} HwdResamplerPropertyType;

typedef struct vfr_src_prop
{
   uint32 l2vic_num;          // vfr intr irq #
   uint32 irq_mux_ctl_sel;    // Mux control for VFR source selection
   uint32 is_supported;       // If this VFR source is hw supported or not
} vfr_src_prop_t;

typedef struct
{
   uint32            reg_base_addr;       // VFR reg base addr
   uint32            reg_size;            // VFR reg page size
   uint32            hw_revision;         // VFR hw revision
   uint32            hw_latch_ver;        // VFR hw latching version
   vfr_src_prop_t    vfr_src_prop[NUM_MAX_VFR_SRC];
}lpasshwio_vfr_prop_t;

 typedef struct  {
    uint32 hw_revision; // hw revision, 0-not supported
   uint32 ccu_mb0_ctrl_reg_phy_addr; // malibox register physical address
   uint32 q6ss_ipc_reg_phy_addr; // LPASS IPC register physical address
   uint32 q6ss_ipc_reg_ack_bit_pos; // LPASS IPC interupt register bit position
    uint16 ipc_send_mail_box_offset; // mail box offset from mb0 ctrl register addr
    uint16 internal_bt_tx_int; // BT Mic path interrupt number
	uint16 internal_bt_rx_int; // BT Speaker path interrupt number
	uint16 internal_fm_tx_rx_int; // FM interrupt number
	uint16 internal_bt_fm_ipc_int; // IPC interrupt
}lpasshwio_prop_riva_struct_t;

 typedef struct  {
    uint32 hw_revision; // hw revision, 0-not supported
}lpasshwio_prop_slimbus_struct_t;

typedef struct
{
   uint32 baseRegAddr;   	// MIDI reg base addr
   uint32 baseRegVirtAddr;  // MIDI reg base virtual addr
   uint32 virtOffset;		// MIDI virtual address base offset
   uint32 regSize;   		// MIDI registers size
   uint32 intPinNo;   		// MIDI intr irq #
   uint32 hw_revision;   	// MIDI hw revision
} HwdMidiPropertyType;

typedef struct
{
   uint32 base_reg_phy_addr;  // spdiftx reg base addr
   uint32 reg_size;           // spdiftx reg page size
   uint32 reset_reg_phy_addr; // spdiftx reset reg base addr
   uint32 reset_reg_size;     // spdiftx rest reg page size;
   uint32 int_irq_no;         // spdiftx intr irq #
   uint32 num_hw_buf;         // num of hw buffer    
   uint32 hw_revision;        // spdiftx hw revision
} lpasshwio_prop_spdif_struct_t;

typedef struct
{   
   uint32 reg_base_addr;      // hdmi input reg base addr
   uint32 reg_size;           // hdmi input reg page size
   uint32 int_irq_no;         // l2vic id for hdmi interrupt
   uint32 hw_revision;        // hdmi input hw revision
} lpasshwio_prop_hdmi_input_struct_t;
#endif // #ifndef LPASSHWIODEVCFG_H
