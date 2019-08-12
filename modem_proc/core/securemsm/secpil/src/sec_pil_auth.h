#ifndef SEC_PIL_AUTH_H
#define SEC_PIL_AUTH_H

/**
@file sec_pil_auth.h
@brief Secure PIL Auth Security implementation
*/

/*===========================================================================
   Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/secpil/src/sec_pil_auth.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when       who      what, where, why
--------   ---      ------------------------------------
04/05/12   vg      Added segmented authentication changes.
02/28/12   vg      Fixed review comments
02/20/12   vg      Ported from TZ PIL.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/
 
/**
 * @brief Verify the hash on all loadable program segments
 *
 * @param[in] proc  The peripheral image
 * @param[in] elf   ELF header of the loaded image
 *
 * @return 0 on success, negative on failure
 */
int sec_pil_auth_segments(sec_pil_proc_et proc,
                          const sec_pil_elf_info_t *elf); 

/**
 * @brief This function checks if the image associated with the code segment
 *        needs to be authenticated. If authentication is required, callers
 *        MUST authenticate the image successfully before allowing it to execute.          
 *
 * @param[in] Code segment in SECURE_BOOTn register containing authentication 
 *            information of the image.
 * @param[in,out] Pointer to a uint32 indicating whether authentication is required. 
 *                Will be populated to 0 if authentication is not required, 
 *                1 if authentication is required. 
 * @return 0 on success, negative on failure
 */
uint32 sec_pil_hw_is_auth_enabled(uint32 code_segment, uint32 *auth_enabled);

/**
 * @brief This function verifies hash table segment signature and validates
 *        ELF and program headers hash against thier entry in hash table.          
 *
 * @param[in] proc The peripheral image 
 * @param[in] elf  Pointer to the ELF info of the loaded image.
 *
 * @return 0 on success, negative on failure
 */
int sec_pil_verify_sig(sec_pil_proc_et proc,
                       const sec_pil_elf_info_t *elf,
                       sec_pil_verified_info_t *v_info);

/**
 * @brief This function updates the internal function table pointers for PBL 
 *        secboot routines.
 *
 * @return 0 on success, 1 on failure
 */
uint32 sec_pil_get_secboot_ftbl(void);

/**
 * @brief This funnction initialises segments auth internal data structure.          
 *
 * @param[in]     proc             The peripheral image
 * @param[in]     image_start_addr Start address of the image loaded on to RAM.
 * @param[in]     pil              Pointer to Pil internal data
 * @param[in,out] seg_info         Pointer to segments auth internal data.
 *
 * @return 0 on success, negative on failure 
 */
int sec_pil_seg_auth_init(sec_pil_proc_et proc,
                          uint32 image_start_addr,
                          const sec_pil_priv_t *pil,
                          sec_pil_seg_auth_info_t *seg_info);

/**
 * @brief This funnction updates the hash for the length loaded and compares 
 *        the hash against their entries in the hash table until the length loaded.
 *        Also tracks the elf segments loaded so far.
 *
 * @param[in]     proc         The peripheral image
 * @param[in]     len_tb_auth  Length of the image loaded 
 * @param[in]     pil          Pointer to Pil internal data
 * @param[in,out] seg_info     Pointer to segments auth internal data.
 *
 * @return 0 on success, negative on failure 
 */

int sec_pil_auth_partial_segments(sec_pil_proc_et proc,
                                  uint32 len_tb_auth,
                                  const sec_pil_priv_t *pil,
                                  sec_pil_seg_auth_info_t *seg_info);



#endif /* SEC_PIL_AUTH_H */
