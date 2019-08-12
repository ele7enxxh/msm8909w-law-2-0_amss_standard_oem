#ifndef GSM_AG_H 
#define GSM_AG_H 

#define RFQTF_END_OF_TABLE 0xDEADDEAD

extern "C"
{    
    #include "rfcom.h"
    #include "rfm.h"
    #include "GetOutputFile.h"
    #include "rfcommon_nv_mm.h"
    #include "rfgsm_nv_cmn.h"
    #include "rfdevice_gsm_intf.h"
    #include "pm.h"
    #include "rfgsm_core.h"
    #include "rfgsm_core_util.h"
    #include "ftm_gsm_dispatch.h"
}

enum operation_type {
  ENTER_GSM_MODE,
  GSM_INIT_RX,
  GSM_INIT_TX,
  GSM900_TUNE_CHAN_RX,
  GSM900_TUNE_CHAN_TX,
  EGSM900_TUNE_CHAN_RX,
  EGSM900_TUNE_CHAN_TX,
  GSM850_TUNE_CHAN_RX,
  GSM850_TUNE_CHAN_TX,
  GSM1900_TUNE_CHAN_RX,
  GSM1900_TUNE_CHAN_TX,
  GSM1800_TUNE_CHAN_RX,
  GSM1800_TUNE_CHAN_TX,
  GSM_VCO_COARSE_TUNE,
  GSM_GET_KV_CAL_CODE,
  UNSUPPORTED_OP_TYPE
};

enum device_data_id_type {
    VCO_COARSE_TUNE_DATA,
    RTR_GSM_GAIN,
    VCO_COARSE_TUNE_LOCK_CODE,
    VCO_COARSE_TUNE_DONE_FLAG,    
    RTR_UNSUPPORTED_DATA
};

enum mdsp_offt_data_id_type
{
    MDSP_NONSBI_AFTER_RX_BURST,
    MDSP_NONSBI_AFTER_RX_GAIN_SET,
    MDSP_NONSBI_RX_SBI_BUF,
    MDSP_RXF_DIGITAL_GAIN,
    MDSP_SBI_AFTER_RX_GAIN_SET,
    MDSP_UNSUPPORTED_DATA
};

enum rtr_device_id
{
    RTR_8600
};

enum rtr_sbi_register_id
{
    RTR_VCO_LOCK_BIT_REG,
    RTR_GSM_KV_CAL_REG_CODE0,
    RTR_GSM_KV_CAL_REG_CODE1,
    RTR_GSM_SET_KV_CAL_REG_CODE0,
    RTR_GSM_SET_KV_CAL_REG_CODE1,
    UNSUPPORTED_REG
};

struct offtarget_rtr_device
{
    /* SBI, Non-SBI device specific functions */
    boolean (*verify_sbi_settings)(operation_type op_id, void *data);
    boolean (*verify_ssbi_register)(int bus,int page,int addr,int datas);    

    void (*save_regs)( );
    void (*rtr_device_restore_regs)( );
    void (*rtr_get_regs)( );
    void (*rtr_set_regs)(void *regs);    
    int (*rtr_set_sbi_reg)(rtr_sbi_register_id reg_id, void *reg_val);
    int (*rtr_get_sbi_reg)(rtr_sbi_register_id reg_id, void *reg_val);
    
    boolean (*verify_mdsp_sbi_settings)(operation_type op_id, void *data); 
    void *(*get_device_specific_data)(device_data_id_type data_id);    
    boolean (*rtr_init_test_data)(device_data_id_type data_id, void *data);  
    boolean (*rtr_gsm_get_kv_cal_code)(uint16 *kv_code, uint16 *kv_code_r1); 
    void (*rtr_gsm_set_kv_cal_code)(uint16 code);
    void (*rtr_vco_coarse_tune)( );
    byte (*rtr_interpolate_coarse_tune_val)(rfcom_mode_enum_type mode,uint8 rf_band,uint32 channel);
};

typedef struct
{
  uint32 addr;
  uint32 val;
} rfqtf_mdsp_expected_mdsp_values_type;

boolean rfqtf_verify_nv_items(int32 *exp_nv_items, int32 size);
boolean rfqtf_verify_mdsp_vars(rfqtf_mdsp_expected_mdsp_values_type *mdsp_vars);

uint32 getMinChannelForBand(rfgsm_band_type band);
uint32 getMaxChannelForBand(rfgsm_band_type band);
uint32 getMinRFCOMChannelForBand(rfcom_gsm_band_type band);
uint32 getMaxRFCOMChannelForBand(rfcom_gsm_band_type band);

typedef struct
{
   uint32 rxf_gain;  /* RXF digital gain */
   int16  gain_dB;   /* Gain in dB associated with the rxf_gain */
} qtf_rfgsm_mdsp_rxf_gain_type;


#endif



