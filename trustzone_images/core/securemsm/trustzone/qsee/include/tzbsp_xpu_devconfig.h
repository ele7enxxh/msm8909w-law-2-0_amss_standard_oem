#ifndef XPU_DEVCFG_STRUCT_H
#define XPU_DEVCFG_STRUCT_H

#include "tzbsp_vmid_config.h"

#define TRUE     1
#define FALSE    0
#define false    0
#define X(n)    0

typedef struct {
  uint8 mpu:1; //mpu or apu/rpu
  uint8 enable:1;
  uint8 staticcfg:1;
  uint8 owner:1;
  uint8 has_override:1;
  uint8 has_milestone:1;
}flags_t;

typedef struct {
  uint8 xpuvmide:1;
  uint8 cfgere:1;
  uint8 clere:1;
  uint8 eie:1;
  uint8 dcdee:1;
  uint8 amte:1;
  uint8 cfgeie:1;
  uint8 cleie:1;
}xpu_cr_t;

typedef struct {
  uint16 xpunse:1;
  uint16 scfgere:1;
  uint16 clere:1;
  uint16 seie:1;
  uint16 sdcdee:1;
  uint16 nscfge:1;
  uint16 nsrgclee:1;
  uint16 amte:1;
  uint16 scfgeie:1;
  uint16 scleie:1;
}xpu_scr_t;

typedef struct {
  uint8 msae:1;
  uint8 cfgere:1;
  uint8 clere:1;
  uint8 eie:1;
  uint8 dcdee:1;
  uint8 cfgeie:1;
  uint8 cleie:1;
}xpu_mcr_t;

/* MPU with AMT support */
typedef struct {
  xpu_scr_t scr;
  xpu_cr_t  cr;
  xpu_mcr_t mcr;
  flags_t   flags;
  uint32    rpu_acr;
  uint32    ump_racr;
  uint32    ump_wacr;
  uint32    amt_racr;
  uint32    amt_wacr;
}mpu_amt_ctrl_reg_t;

/* MPU with AMT support */
typedef struct {
  xpu_scr_t  scr;
  xpu_cr_t   cr;
  xpu_mcr_t  mcr;
  flags_t   flags;
  uint32     rpu_acr ;
  uint32     ump_racr;
  uint32   ump_wacr;
}mpu_noamt_ctrl_reg_t;

/* RPU or APU  */
typedef struct {
  xpu_scr_t  scr;
  xpu_cr_t   cr;
  xpu_mcr_t  mcr;
  flags_t   flags;
  uint16     rpu_acr ;
}xpu_ctrl_reg_t;

typedef struct {
  uint8 ns:1;
  uint8 msaclrwe:1;
  uint8 vmidclrwe:1;
  uint8 msaclroe:1;
  uint8 vmidclroe:1;
}xpu_RGn_scr_t;

typedef struct {
  uint8 msae:1;
  uint8 sclrwe:1;
  uint8 vmidclrwe:1;
  uint8 sclroe:1;
  uint8 vmidclroe:1;
}xpu_RGn_mcr_t;

/* ............. PRTn .............. */

typedef struct {
  uint16 rwvmid:5;
  uint16 rwe:1;
  uint16 rwge:1;
  uint16 rovmid:5;
  uint16 roe:1;
  uint16 roge:1;
}acr_single_vmid_t;

typedef struct {
  uint32 racr;
  uint32 wacr;
}acr_multi_vmid_t;

typedef struct {
  uint32 PRTn_start;
  uint32 PRTn_end;
  uint8 index;
  xpu_RGn_scr_t scr;
  xpu_RGn_mcr_t mcr;
  flags_t   flags;
  union
  {
    acr_multi_vmid_t multi_acr;
    acr_single_vmid_t single_acr;
  }multi;
}PRTn_t;

/* ............. RGn .............. */
typedef struct {
  uint16 index;
  xpu_RGn_scr_t scr;
  xpu_RGn_mcr_t mcr;
  flags_t   flags;
  union
  {
    acr_multi_vmid_t multi_acr;
    acr_single_vmid_t single_acr;
  }multi;
}RGn_t;

extern const char * g_tzbsp_xpu_devcfg_map[HAL_XPU2_COUNT];
void tzbsp_xpu_devcfg_map_init(void);

#endif /* XPU_DEVCFG_STRUCT_H */
