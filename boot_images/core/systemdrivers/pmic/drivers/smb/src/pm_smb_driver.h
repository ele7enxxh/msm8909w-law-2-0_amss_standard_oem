#ifndef PM_SMB_DRIVER_H
#define PM_SMB_DRIVER_H

#define BIT(n) (1 << (n))
#define _SMB_MASK(BITS, POS) \
    ((unsigned char)(((1 << (BITS)) - 1) << (POS)))

#define SMB_MASK(HIGH_BIT_POS, LOW_BIT_POS) \
        _SMB_MASK((HIGH_BIT_POS) - (LOW_BIT_POS) + 1, \
                (LOW_BIT_POS))
#define SMB_BIT_OP_DECLARE(feature) \
    uint8 offset_##feature; \
    uint8 bit_##feature

typedef enum
{
    SMB_INDEX_1360,
    SMB_INDEX_358,
    SMB_INDEX_1357,
    SMB_INDEX_INVALID
} pm_smb_index_type;

typedef struct
{
    uint8 slave_address; // here refer to 7bit address
    uint32 device_id; // refer to which QUP
    uint32 bus_frequency;
    uint32 transfer_timeout_us;
    pm_smb_index_type index;
} pm_smb_hw_info_type;

typedef struct
{
    SMB_BIT_OP_DECLARE(nv_access);
} pm_smb_nv_access_type;

typedef struct
{
    SMB_BIT_OP_DECLARE(aicl_enable);
} pm_smb_aicl_enable_type;

typedef struct
{
    SMB_BIT_OP_DECLARE(enable_by_pin);
    SMB_BIT_OP_DECLARE(active_low);
    SMB_BIT_OP_DECLARE(charger_inhit);
    SMB_BIT_OP_DECLARE(charger_enable);
} pm_smb_enable_type;

typedef struct
{
    SMB_BIT_OP_DECLARE(usb_ac_by_pin);
    SMB_BIT_OP_DECLARE(usb51_reset_to_500);
    SMB_BIT_OP_DECLARE(usb_mode_ac);
    SMB_BIT_OP_DECLARE(usb_mode_500_100);
} pm_smb_mode_control_type;

typedef struct
{
    uint8 offset_icl;
    uint8 bit_high;
    uint8 bit_low;
} pm_smb_icl_type;

typedef struct
{
    uint8 offset_ibat;
    uint8 bit_high;
    uint8 bit_low;
} pm_smb_ibat_type;

typedef struct
{
    pm_smb_nv_access_type nv_access;
    pm_smb_aicl_enable_type aicl;
    pm_smb_enable_type charger_enable;
    pm_smb_mode_control_type mode_control;
    pm_smb_icl_type icl;
    pm_smb_ibat_type ibat;

    int* icl_list;
    uint8 icl_list_size;

    int* ibat_list;
    uint8 ibat_list_size;
} pm_smb_feature_list_type;

typedef struct
{
    pm_smb_hw_info_type* hw_info;
    pm_smb_feature_list_type* feature_list;
} pm_smb_info_type;

pm_smb_feature_list_type* pm_smb_get_feature_list(pm_smb_index_type index);

void pm_smb_driver_init(void);

pm_smb_info_type* pm_smb_get_info(void);

#endif /* end of PM_SMB_DRIVER_H */

