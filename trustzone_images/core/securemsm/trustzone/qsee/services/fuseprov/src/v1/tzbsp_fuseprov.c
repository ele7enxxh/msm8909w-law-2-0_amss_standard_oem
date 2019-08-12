/**
@file tzbsp_fuseprov.c
@brief Implementation of Fuse blowing for secure boot devices
*/

/*=============================================================================
                              EDIT HISTORY

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/services/fuseprov/src/v1/tzbsp_fuseprov.c#1 $ 
  $DateTime: 2016/06/17 14:31:11 $ 
  $Author: pwbldsvc $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------- 

2013/10/18  mic   Initial Version of Fuse Provisioning Library.
=============================================================================*/
#include "tzbsp_fuseprov.h"
#include "tzbsp.h"
#include "tzbsp_mem.h"
#include "tzbsp_hash.h"
#include "stdlib.h"
#include "string.h"
#include "tzbsp_timer.h"
#include "tzbsp_hwio.h"
#include "HALhwio.h"
#include "qfprom.h"
#include "tzbsp_prng.h"
#include "tzbsp_chipset.h"
#include "tzbsp_target.h"
#include "tzbsp_memory_map.h"
#include "tzbsp_fuseprov_chipset.h"
#include <stringl/stringl.h>
#include "pm_pon.h"

extern void tzbsp_enable_ce1_clocks(boolean enable) __attribute__((weak));

#define QFPROM_READ_ROW(a,b,c,d) tzbsp_fuse_read((a),(b),(c),(d))
#define QFPROM_WRITE_ROW(a,b,c,d) tzbsp_fuse_write((a),(b),(c),(d));

#define CURRENT_SUPPORTED_SECDAT_REVISION    1    //Revision in secdat header that's currently supported
#define CURRENT_SUPPORTED_QFUSELIST_REVISION 1    //Revision in fuselist header that's currently supported


typedef enum
{
  FUSEPROV_CATEGORY_GENERAL             = 0x00000000,
  FUSEPROV_CATEGORY_SECBOOT             = 0x00000001,
  FUSEPROV_CATEGORY_SHK                 = 0x00000002,
  FUSEPROV_CATEGORY_OEM_CONFIG          = 0x00000003,
  FUSEPROV_CATEGORY_RW_PERM             = 0x00000004,
  FUSEPROV_CATEGORY_FEC_EN              = 0x00000005,
  FUSEPROV_CATEGORY_TYPE_MAX            = 0x7FFFFFFF
} fuseprov_category_etype;

static uint32 qfprom_fuse_data[2];   //Array we pass to QFPROM API
static boolean device_reset_needed = FALSE; //Fuse blow takes affect only on device reset

//Magic numbers to indicate a boot into download mode
//from tzbsp_dload_mode.c
#define TZBSP_DLOAD_MAGIC_NUM_1     0xE47B337D
#define TZBSP_DLOAD_MAGIC_NUM_2     0xCE14091A
typedef struct
{
  uint32 magic_1;
  uint32 magic_2;
} tzbsp_dload_id_type;

//Add below for dev testing purposes
//volatile int g_spin = TRUE; //For test/debug purposes
//while (g_spin); at beginning of tzbsp_blow_fuses_and_reset
//to test with hardcoded secdat file, add array like
//uint8 test_secdat[] = {0xca,0x51,0x72,0x3b,0x29,0x6f,0x12,0x2a...};
//and in beginning of tzbsp_blow_fuses_and_reset()
//memcpy((uint8*)TZBSP_SECDAT_BUFFER_LOCATION, test_secdat, sizeof(test_secdat));
//secdat_len = TZBSP_SECDAT_BUFFER_SIZE;
//QFPROM_READ_ROW and QFPROM_WRITE_ROW macro's can also be changed to read/write
//from memory instead of fuses

/**
 * Calculates the FEC value
 *
 * @param [in] lsb_data  LSB fuse value
 * @param [in] msb_data  MSB fuse value
 * 
 * returns the MSB fuse value with the FEC bits set in bits 30-24
 */
uint32 FEC_63_56_Bit(uint32 lsb_data, uint32 msb_data)
{
  uint8 lfsr[7] = {0};
  int i = 0;
  uint32 temp = 0;
  uint32 fec_val = 0;
  uint64 data_loc = 0;

  data_loc = (uint64)((uint64)msb_data << 32) | lsb_data;
  for (i=0; i < 56; i++)
  {
    temp = lfsr[0] ^ ((data_loc >> i) & 0x1);

    lfsr[0] = lfsr[1] ^ temp;
    lfsr[1] = lfsr[2];
    lfsr[2] = lfsr[3];
    lfsr[3] = lfsr[4];
    lfsr[4] = lfsr[5] ^ temp;
    lfsr[5] = lfsr[6];
    lfsr[6] = temp;
  }
  /* Put the FEC bits into one variable */
  for (i=6; i >= 0; i--)
  {
    temp = (lfsr[i] << i);
    fec_val = (fec_val | temp);
  }
  return ((fec_val << 24) | msb_data);
}

/**
 * Checks if the fuse region matches the fuse category. i.e is the region
 * an OEM PK HASH, SECBOOT etc. Fuse region's that are not in known list
 * are categorized as GENERAL fuses
 *
 * @param [in] fuse_category  Category we want to check if fuse region belongs to
 * @param [in] fuse_region    Fuse region type in SECDAT
 */
boolean tzbsp_isfuseregion_in_category(fuseprov_category_etype fuse_category,
                          fuseprov_region_etype   fuse_region) 
{
  boolean incategory = FALSE;

  switch (fuse_category)
  {
    case FUSEPROV_CATEGORY_SECBOOT:
      if (fuse_region == FUSEPROV_REGION_TYPE_OEM_SEC_BOOT)
      {
        incategory = TRUE;
      }
      break;
    case FUSEPROV_CATEGORY_SHK:
      if (fuse_region == FUSEPROV_REGION_TYPE_SEC_HW_KEY)
      {
        incategory = TRUE;
      }
      break;
    case FUSEPROV_CATEGORY_OEM_CONFIG:
      if (fuse_region == FUSEPROV_REGION_TYPE_OEM_CONFIG)
      {
        incategory = TRUE;
      }
      break;
    case FUSEPROV_CATEGORY_RW_PERM:
      if (fuse_region == FUSEPROV_REGION_TYPE_READ_WRITE_PERM)
      {
        incategory = TRUE;
      }
      break;
    case FUSEPROV_CATEGORY_FEC_EN:
      if (fuse_region == FUSEPROV_REGION_TYPE_FEC_EN)
      {
        incategory = TRUE;
      }
      break;
    case FUSEPROV_CATEGORY_GENERAL:
      if ((fuse_region != FUSEPROV_REGION_TYPE_OEM_SEC_BOOT) &&
         (fuse_region != FUSEPROV_REGION_TYPE_SEC_HW_KEY) && 
         (fuse_region != FUSEPROV_REGION_TYPE_OEM_CONFIG) && 
         (fuse_region != FUSEPROV_REGION_TYPE_FEC_EN) && 
         (fuse_region != FUSEPROV_REGION_TYPE_READ_WRITE_PERM)) 
      {
        //All other fuse region's are bucketed in general category
        incategory = TRUE;
      }
      break;

	 default:
	  break;
  }

  return incategory;
}

/**
 * Blows all the fuses in the fuse entry list which belongs to the category specified
 * HW register is read first to see if fuses are not already blown (sometime previously)
 * and blows the fuses
 *
 * @param [in] fuse_category    Category of fuses we want to blow
 * @param [in] fuse_entry_head  Start of list of fuses
 * @param [in] fuse_count       Number of fuses in the list
 */
fuseprov_error_etype tzbsp_blow_fuseregion(fuseprov_category_etype fuse_category,
                                           fuseprov_qfuse_entry   *fuse_entry_head,
                                           uint32                  fuse_count) 
{
  fuseprov_error_etype   status             = FUSEPROV_SUCCESS;
  uint32                 qfprom_status      = QFPROM_NO_ERR;
  uint32                 i;
  
  for (i=0; i < fuse_count; i++)
  { 
    if (tzbsp_isfuseregion_in_category(fuse_category, fuse_entry_head[i].region_type) &&
       (fuse_entry_head[i].operation == FUSEPROV_OPERATION_TYPE_BLOW) &&
       ((fuse_entry_head[i].lsb_val != 0) || (fuse_entry_head[i].msb_val != 0)))
    {
      TZBSP_LOG_DBG("FP:Info(0x%8X),(0x%8X),(0x%8X)\n", __LINE__, i,fuse_entry_head[i].region_type);

      //Read the fuse
      qfprom_fuse_data[0] = 0;
      qfprom_fuse_data[1] = 0;
      QFPROM_READ_ROW(fuse_entry_head[i].raw_row_address + TZBSP_SANDBOX_RELOCATE_OFFSET, QFPROM_ADDR_SPACE_RAW, qfprom_fuse_data, &qfprom_status);
      if (qfprom_status != QFPROM_NO_ERR)
      {
        TZBSP_LOG_DBG("FP:(0x%8X),(0x%8X),(0x%8X),(0x%8X),(0x%8X)\n",
                __LINE__,fuse_entry_head[i].raw_row_address, qfprom_fuse_data[0], qfprom_fuse_data[1],
                qfprom_status);
        status = FUSEPROV_QFPROM_READ_ERROR;
        break;
      }

      //Blow only if we need to - i.e the bits are not blown already
      if ( (((qfprom_fuse_data[0]) & (fuse_entry_head[i].lsb_val)) != fuse_entry_head[i].lsb_val ) ||
           (((qfprom_fuse_data[1]) & (fuse_entry_head[i].msb_val)) != fuse_entry_head[i].msb_val) )
      {
        qfprom_fuse_data[0] = fuse_entry_head[i].lsb_val;
        qfprom_fuse_data[1] = fuse_entry_head[i].msb_val;
        TZBSP_LOG_DBG("FP:Info(0x%8X),(0x%8X),(0x%8X)\n", __LINE__, qfprom_fuse_data[0], qfprom_fuse_data[1]);

        QFPROM_WRITE_ROW(fuse_entry_head[i].raw_row_address + TZBSP_SANDBOX_RELOCATE_OFFSET, qfprom_fuse_data, 0, &qfprom_status);
        if (qfprom_status != QFPROM_NO_ERR)
        {
          TZBSP_LOG_DBG("FP:(0x%8X),(0x%8X),(0x%8X),(0x%8X),(0x%8X)\n",
                __LINE__, fuse_entry_head[i].raw_row_address, qfprom_fuse_data[0], qfprom_fuse_data[1],
                qfprom_status);
          status = FUSEPROV_QFPROM_WRITE_ERROR;
          break;
        }
        device_reset_needed = TRUE; //Fuse takes affect only if device is reset
      }
    }
  }
  return status;
}

/**
 * Blows the SHK (secondary hw derivative key)
 *
 * @param [in] fuse_entry_head  Start of list of fuses
 * @param [in] fuse_count       Number of fuses in the list
 */
fuseprov_error_etype tzbsp_provision_shk(fuseprov_qfuse_entry   *fuse_entry_head,
                                         uint32                  fuse_count) 
{
  fuseprov_error_etype  status              = FUSEPROV_SUCCESS;
  uint32                qfprom_status       = QFPROM_NO_ERR;
  uint32                ntemp_cnt           = 0;
  uint32                nbytes_read         = 0;
  uint32                i                   = 0;
  boolean               write_disable_blown = FALSE;
  boolean               read_disable_blown  = FALSE;
  uint32                random_num_buf[10]  = {0}; //40 bytes to write 5 set of 8 byte long fuse register (FEC will be masked)

  do
  {
    /* If the SHK is already provisioned there's nothing to do. We expect SHK and read and write perm
       disable fuses for SHK to be blown as a unit */
    QFPROM_READ_ROW(TZBSP_FUSEPROV_WR_PERM_HWIO_ADDR, QFPROM_ADDR_SPACE_RAW, qfprom_fuse_data, &qfprom_status);
    if (qfprom_status == QFPROM_NO_ERR) 
    {
      if (TZBSP_FUSEPROV_IS_SHK_WR_PERM_BLOWN(qfprom_fuse_data))
      {
        write_disable_blown = TRUE;
      }
    }
    else
    {
      TZBSP_LOG_DBG("FP:(0x%8X),(0x%8X),(0x%8X),(0x%8X),(0x%8X)\n",
                __LINE__, TZBSP_FUSEPROV_WR_PERM_HWIO_ADDR, qfprom_fuse_data[0], qfprom_fuse_data[1],
                qfprom_status);

      status = FUSEPROV_QFPROM_READ_ERROR;
      break;
    }

    QFPROM_READ_ROW(TZBSP_FUSEPROV_RD_PERM_HWIO_ADDR, QFPROM_ADDR_SPACE_RAW, qfprom_fuse_data, &qfprom_status);
    if (qfprom_status == QFPROM_NO_ERR) 
    {
      if (TZBSP_FUSEPROV_IS_SHK_RD_PERM_BLOWN(qfprom_fuse_data))
      {
        read_disable_blown = TRUE;
      }
    }
    else
    {
      TZBSP_LOG_DBG("FP:(0x%8X),(0x%8X),(0x%8X),(0x%8X),(0x%8X)\n",
                __LINE__,TZBSP_FUSEPROV_RD_PERM_HWIO_ADDR, qfprom_fuse_data[0], qfprom_fuse_data[1],
                qfprom_status);
      status = FUSEPROV_QFPROM_READ_ERROR;
      break;
    }

    if (write_disable_blown && read_disable_blown)
    {
      /* Not necessarily an error - i.e SHK
         i.e when blowing secboot fuses etc due to voltage etc. So when we retry again, SHK would already have
         been blown */
      TZBSP_LOG_DBG("FP:Info(0x%8X),(0x%8X)\n", __LINE__, 0);
      status = FUSEPROV_SHK_RD_WR_DISABLE_BLOWN;
      break;
    }
    else if (write_disable_blown || read_disable_blown)
    {
      TZBSP_LOG_DBG("FP:(0x%8X),(0x%8X),(0x%8X)\n", __LINE__, write_disable_blown, read_disable_blown);
      status = FUSEPROV_SHK_RD_WR_MISMATCH;
      break;
    }

    /* Check if an SHK was already provisioned */
    for (i=0; i < fuse_count; i++)
    {
      TZBSP_LOG_DBG("FP:Info(0x%8X),(0x%8X)\n", __LINE__, i);

      if ((FUSEPROV_REGION_TYPE_SEC_HW_KEY == fuse_entry_head[i].region_type) &&
         (fuse_entry_head[i].operation == FUSEPROV_OPERATION_TYPE_BLOW))
      {
        TZBSP_LOG_DBG("FP:Info(0x%8X),(0x%8X)\n", __LINE__, i);
        QFPROM_READ_ROW(fuse_entry_head[i].raw_row_address + TZBSP_SANDBOX_RELOCATE_OFFSET, QFPROM_ADDR_SPACE_RAW, qfprom_fuse_data, &qfprom_status);
        if (qfprom_status != QFPROM_NO_ERR)
        {
          /* Note, we don't log the SHK fuse value i.e the qfprom_fuse_data value */
          TZBSP_LOG_DBG("FP:(0x%8X),(0x%8X),(0x%8X)\n",
                __LINE__,fuse_entry_head[i].raw_row_address, qfprom_status);
          status = FUSEPROV_QFPROM_READ_ERROR;
          break;
        }
        if ((qfprom_fuse_data[0] != 0) || (qfprom_fuse_data[1] != 0))
        {
          /* Not necessarily an error - i.e SHK was blown, then before R/W perm fuses blown, there was an error
             i.e when blowing secboot fuses etc due to voltage etc. So when we retry again, SHK would already have
             been blown */
          TZBSP_LOG_DBG("FP:Info(0x%8X),(0x%8X)\n", __LINE__, i);
          status = FUSEPROV_SHK_ALRDY_BLOWN;
          break;
        }
      }
    }

    if (status != FUSEPROV_SUCCESS)
    {
      break;
    }

    memset(&random_num_buf[0], 0, sizeof(random_num_buf));  
    
    //Casting for uint8 as the array is a unit32
    nbytes_read = tzbsp_prng_getdata((uint8*)(&random_num_buf[0]), sizeof(random_num_buf));
    if (nbytes_read != sizeof(random_num_buf))
    {
      TZBSP_LOG_DBG("FP:Info(0x%8X),(0x%8X)\n", __LINE__, nbytes_read);
      status = FUSEPROV_SHK_GENERATION_FAILED;
      break;
    }

    /* Now blow the SHK */
    ntemp_cnt = 0x0;
    for (i=0; i < fuse_count; i++)
    {
      if ((FUSEPROV_REGION_TYPE_SEC_HW_KEY == fuse_entry_head[i].region_type) &&
         (fuse_entry_head[i].operation == FUSEPROV_OPERATION_TYPE_BLOW))
      {
        TZBSP_LOG_DBG("FP:Info(0x%8X),(0x%8X)\n", __LINE__, i);

        /* Set the fuse value with the FEC bits masked */
        qfprom_fuse_data[0] = random_num_buf[2 * ntemp_cnt]  & TZBSP_FUSEPROV_SEC_KEY_LSB_MASK; //LSB
        qfprom_fuse_data[1] = random_num_buf[2 * ntemp_cnt + 1]  & TZBSP_FUSEPROV_SEC_KEY_MSB_MASK; //MSB

        /* Now calculate the FEC, 8916 do not have fec for sec hw key */
        //qfprom_fuse_data[1] = FEC_63_56_Bit(qfprom_fuse_data[0], qfprom_fuse_data[1]);
        ntemp_cnt++;

        QFPROM_WRITE_ROW(fuse_entry_head[i].raw_row_address + TZBSP_SANDBOX_RELOCATE_OFFSET, qfprom_fuse_data, 0, &qfprom_status);
        if (qfprom_status != QFPROM_NO_ERR)
        {
          /* Note, we don't log the SHK fuse value i.e the qfprom_fuse_data value */
          TZBSP_LOG_DBG("FP:(0x%8X),(0x%8X),(0x%8X)\n",
                __LINE__,fuse_entry_head[i].raw_row_address, qfprom_status);
          status = FUSEPROV_QFPROM_WRITE_ERROR;
          break;
        }        
        device_reset_needed = TRUE; //Fuses take affect only if device is reset
      }
    }
  }while (0);
  
  secure_memset(&random_num_buf[0], 0, sizeof(random_num_buf));
  return status;
}

/**
 * Parses the secdat buffer and verifies the hash
 *
 * @param [in]     secdat_buffer   Buffer to parse
 * @param [in]     secdat_len      Buffer to parse
 * @param [in/out] fuse_head_pptr  Start of list of fuses
 * @param [in/out] fuse_count_ptr  Number of fuses in the list
 */
fuseprov_error_etype
tzbsp_parse_secdat(uint8* secdat_buffer, uint32 secdat_len, fuseprov_qfuse_entry** qfuse_head_pptr, uint32* fuse_count_ptr)
{
  fuseprov_secdat_hdr_type*     secdat_hdr_ptr       = NULL;
  fuseprov_qfuse_list_hdr_type* qfuse_list_hdr_ptr   = NULL;
  uint32                        num_bytes_for_hash   = 0;  
  fuseprov_error_etype          status               = FUSEPROV_SUCCESS;
  uint8                         sha_digest[TZBSP_SHA256_HASH_SZ];

  do
  {
    secdat_hdr_ptr =  (fuseprov_secdat_hdr_type*)secdat_buffer;

    /* Magic numbers should match */
    if ((secdat_hdr_ptr->magic1 != FUSEPROV_SECDAT_MAGIC1) ||
        (secdat_hdr_ptr->magic2 != FUSEPROV_SECDAT_MAGIC2))
    {
      TZBSP_LOG_DBG("FP:(0x%8X),(0x%8X),(0x%8X)\n",
            __LINE__,secdat_hdr_ptr->magic1, secdat_hdr_ptr->magic2);
      status = FUSEPROV_SECDAT_MAGIC_MISMATCH;
      break;
    }

     /* Revision should be what we support */
    if (secdat_hdr_ptr->revision  != CURRENT_SUPPORTED_SECDAT_REVISION)
    {
      TZBSP_LOG_DBG("FP:(0x%8X),(0x%8X)\n",
            __LINE__,secdat_hdr_ptr->revision);
      status = FUSEPROV_SECDAT_REV_NOT_SUPPORTED;
      break;
    }

    /* Length of the buffer can't be less than the size denoted by the header */
    if (secdat_len < (secdat_hdr_ptr->size + sizeof(fuseprov_secdat_hdr_type)))
    {
      TZBSP_LOG_DBG("FP:(0x%8X),(0x%8X),(0x%8X)\n",
            __LINE__,secdat_len, secdat_hdr_ptr->size);
      status = FUSEPROV_SECDAT_SIZE_LEN_MISMATCH;
      break;
    }

    /* Check if there are qfuse entries */
    if (secdat_hdr_ptr->size == sizeof(fuseprov_secdat_footer_type))
    {
      /* It's valid to have no entries and just the secdat header + footer, i.e nothing to be done */
      TZBSP_LOG_DBG("FP:Info(0x%8X),(0x%8X)\n",__LINE__,secdat_hdr_ptr->size);
      status = FUSEPROV_SECDAT_DEFAULT_NOFUSES;
      /* Note we don't break from the while as we still need to check the hash */
    }
    else
    {
      /* Don't start parsing the qfuse list header until we know the size includes the header  */
      if (secdat_hdr_ptr->size < ((sizeof(fuseprov_qfuse_list_hdr_type)) + sizeof(fuseprov_secdat_footer_type)))
      {
        TZBSP_LOG_DBG("FP:(0x%8X),(0x%8X)\n",
                __LINE__, secdat_hdr_ptr->size);
        status = FUSEPROV_SECDAT_SIZE_LEN_MISMATCH;
        break;
      }
      /* We have a qfuse header */
      qfuse_list_hdr_ptr = (fuseprov_qfuse_list_hdr_type*)(secdat_buffer + sizeof(fuseprov_secdat_hdr_type));
      if (qfuse_list_hdr_ptr->revision  != CURRENT_SUPPORTED_QFUSELIST_REVISION)
      {
        TZBSP_LOG_DBG("FP:(0x%8X),(0x%8X)\n",
                __LINE__, qfuse_list_hdr_ptr->revision);
        status = FUSEPROV_QFUSE_REV_NOT_SUPPORTED;
        break;
      }

      /* Ensure fuse_count isn't some arbitary number which could cause an overflow */
      /* OEM's probably won't blow more than 30 or so fuse addresses, but keeping 1024 */
      /* for general purpose */
      if ((qfuse_list_hdr_ptr->fuse_count == 0) || (qfuse_list_hdr_ptr->fuse_count > 1024))
      {
        TZBSP_LOG_DBG("FP:(0x%8X),(0x%8X)\n",
                __LINE__, qfuse_list_hdr_ptr->fuse_count);
        status = FUSEPROV_SECDAT_SIZE_LEN_MISMATCH;
        break;
      }

      /* Size in header must match how many fuse entries there are */
      if ((qfuse_list_hdr_ptr->size != qfuse_list_hdr_ptr->fuse_count * sizeof(fuseprov_qfuse_entry)) ||
          (secdat_hdr_ptr->size != (sizeof(fuseprov_qfuse_list_hdr_type) + 
                                    qfuse_list_hdr_ptr->size +
                                    sizeof(fuseprov_secdat_footer_type)))
         )
      {
        TZBSP_LOG_DBG("FP:(0x%8X),(0x%8X),(0x%8X),(0x%8X)\n",
                  __LINE__,secdat_hdr_ptr->size, qfuse_list_hdr_ptr->size,
                  qfuse_list_hdr_ptr->fuse_count);
        status = FUSEPROV_SECDAT_SIZE_LEN_MISMATCH;
        break;
      }
    }

    /* Get to the footer so we can verify the hash
       The hash is the hash of everything except the footer  */
    num_bytes_for_hash = (sizeof(fuseprov_secdat_hdr_type) + secdat_hdr_ptr->size -
                                       sizeof(fuseprov_secdat_footer_type));
    if (E_SUCCESS != tzbsp_hash(TZBSP_HASH_SHA256, secdat_buffer, num_bytes_for_hash,
                               sha_digest, sizeof(sha_digest)))
    {
      TZBSP_LOG_DBG("FP:(0x%8X),(0x%8X),(0x%8X)\n",
                  __LINE__,secdat_buffer, num_bytes_for_hash);
      status = FUSEPROV_TZBSP_HASH_FAIL;
      break;
    }
    /* Now verify the hash  */
    if(memcmp(secdat_buffer + num_bytes_for_hash, sha_digest, sizeof(sha_digest)) != 0)
    {
      TZBSP_LOG_DBG("FP:(0x%8X),(0x%8X)\n",
                  __LINE__,num_bytes_for_hash);
      status = FUSEPROV_INVALID_HASH;
      break;
    }

    /* Now, if there was a qfuse list, set fuse information */
    if (status != FUSEPROV_SECDAT_DEFAULT_NOFUSES)
    {
      (*qfuse_head_pptr) = (fuseprov_qfuse_entry*)(secdat_buffer + sizeof(fuseprov_secdat_hdr_type) + sizeof(fuseprov_qfuse_list_hdr_type));
      (*fuse_count_ptr)  = qfuse_list_hdr_ptr->fuse_count;
    }
  } while (0);

  return status;
}

/**
 * Parses secdat buffer and blows the list of fuses as long as the
 * fuse lock (OEM SEC BOOT write perm disable fuse) in not blown
 *
 * @param [in] secdat_buffer  Pointer to the sec.dat in memory
 * @param [in] secdat_len     Size in bytes of the buffer
 */
fuseprov_error_etype
tzbsp_blow_fuses(uint8* secdat_buffer, uint32 secdat_len)
{
  fuseprov_error_etype       status                = FUSEPROV_FAILURE;
  fuseprov_qfuse_entry*      qfuse_head_entry_ptr  = NULL;
  uint32                     qfuse_count           = 0;
  uint32                     i                     = 0;
  uint32                     qfprom_status         = QFPROM_NO_ERR;

  do
  {
    /* If the OEM_SEC_BOOT Write Permission fuse is blown, it indicates
       fuse blowing has already taken place as the read and write permission fuses
       are the last fuses that need to be locked down */
    memset(qfprom_fuse_data, 0, sizeof(qfprom_fuse_data));    
    QFPROM_READ_ROW(TZBSP_FUSEPROV_WR_PERM_HWIO_ADDR, QFPROM_ADDR_SPACE_RAW, qfprom_fuse_data, &qfprom_status);
    if (qfprom_status == QFPROM_NO_ERR) 
    {
      if (TZBSP_FUSEPROV_IS_SECBOOT_WR_PERM_BLOWN(qfprom_fuse_data))
      {
        status = FUSEPROV_SECDAT_LOCK_BLOWN;
        break;
      }
    }
    else
    {
      TZBSP_LOG_DBG("FP:(0x%8X),(0x%8X),(0x%8X),(0x%8X),(0x%8X)\n",
                __LINE__,TZBSP_FUSEPROV_WR_PERM_HWIO_ADDR, qfprom_fuse_data[0], qfprom_fuse_data[1],
                qfprom_status);
      status = FUSEPROV_QFPROM_READ_ERROR;
      break;
    }

    if ((secdat_buffer == NULL) || (secdat_len == 0) ||
        (secdat_len < sizeof(fuseprov_secdat_hdr_type) ||
        (secdat_len > TZBSP_SECDAT_BUFFER_SIZE)))
    {
      TZBSP_LOG_DBG("FP:(0x%8X),(0x%8X),(0x%8X)\n",
                __LINE__,secdat_buffer, secdat_len);
      status = FUSEPROV_INVALID_ARG;
      break;
    }

    /* Check if there is data in the buffer */
    for (i=0; ((i < secdat_len) && (secdat_buffer[i] == 0)); i++);
    if (i == secdat_len)
    {
      /* This is not an error. It's valid to not have the sec partition, or an empty sec partition */
      status = FUSEPROV_SUCCESS;
      break;
    }

    tzbsp_enable_ce1_clocks(TRUE);
    status = tzbsp_parse_secdat(secdat_buffer, secdat_len, &qfuse_head_entry_ptr, &qfuse_count);
    tzbsp_enable_ce1_clocks(FALSE);
    if (status != FUSEPROV_SUCCESS)
    {
      /* mismatching magic number could be caused by garbage data in the sec.dat ddr region,
         which is not an error and should not fail the boot up.  */
      if (status == FUSEPROV_SECDAT_MAGIC_MISMATCH)
      {
        status = FUSEPROV_SUCCESS;
      }

      break;
    }

    TZBSP_LOG_DBG("FP:Info(0x%8X),(0x%8X)\n", __LINE__, qfuse_count);

    /*
    ** Blow Fuses in following sequence mainly to have potential to still debug chip
    ** or do ram dumps depending on which step the failure occured.
    ** 1. Blow all fuse regions except below.
    ** 2. Blow SHK (Secondary Hardware Key)
    ** 3. Blow OEM_CONFIG (Contains debug disable fuses etc)
    ** 4. Blow SECURE BOOT (Authentication enable fuses)
    ** 5. Blow FEC_EN (FEC enables)
    ** 5. Blow Read-Write disable permission fuses.
    */
    status = tzbsp_blow_fuseregion(FUSEPROV_CATEGORY_GENERAL, qfuse_head_entry_ptr, qfuse_count);
    if (status!= FUSEPROV_SUCCESS)
    {
      TZBSP_LOG_DBG("FP:(0x%8X),(0x%8X),(0x%8X)\n",
                __LINE__,FUSEPROV_CATEGORY_GENERAL, status);
      break;
    }
    status = tzbsp_provision_shk(qfuse_head_entry_ptr, qfuse_count);
    /* If SHK r/w permission disables are blown, SHK has already been provisioned earlier, so no need to do anything.
       If SHK already blown (but SHK r/w perm disables not blown), no need to do anything */
    if ((status != FUSEPROV_SUCCESS) && (status != FUSEPROV_SHK_RD_WR_DISABLE_BLOWN) && (status != FUSEPROV_SHK_ALRDY_BLOWN))
    {
      TZBSP_LOG_DBG("FP:(0x%8X),(0x%8X),(0x%8X)\n",
                __LINE__,FUSEPROV_CATEGORY_SHK, status);
      break;
    }
    status = tzbsp_blow_fuseregion(FUSEPROV_CATEGORY_OEM_CONFIG, qfuse_head_entry_ptr, qfuse_count);
    if (status!= FUSEPROV_SUCCESS)
    {
      TZBSP_LOG_DBG("FP:(0x%8X),(0x%8X),(0x%8X)\n",
                __LINE__,FUSEPROV_CATEGORY_OEM_CONFIG, status);
      break;
    }
    status = tzbsp_blow_fuseregion(FUSEPROV_CATEGORY_SECBOOT, qfuse_head_entry_ptr, qfuse_count);
    if (status!= FUSEPROV_SUCCESS)
    {
      TZBSP_LOG_DBG("FP:(0x%8X),(0x%8X),(0x%8X)\n",
                __LINE__,FUSEPROV_CATEGORY_SECBOOT, status);
      break;
    }
    status = tzbsp_blow_fuseregion(FUSEPROV_CATEGORY_FEC_EN, qfuse_head_entry_ptr, qfuse_count);
    if (status!= FUSEPROV_SUCCESS)
    {
      TZBSP_LOG_DBG("FP:(0x%8X),(0x%8X),(0x%8X)\n",
                __LINE__,FUSEPROV_CATEGORY_FEC_EN, status);
      break;
    }
    status = tzbsp_blow_fuseregion(FUSEPROV_CATEGORY_RW_PERM, qfuse_head_entry_ptr, qfuse_count);
    if (status!= FUSEPROV_SUCCESS)
    {
      TZBSP_LOG_DBG("FP:(0x%8X),(0x%8X),(0x%8X)\n",
                __LINE__,FUSEPROV_CATEGORY_RW_PERM, status);
      break;
    }
  }while (0);

  return status;
}

/**
 * Blows the fuses listed in the secdat buffer loaded in TZ DDR memory
 * secdat is only consumed if the OEM SEC BOOT write perm disable fuse
 * has not been blown
 *
 * Device will reset if any fuse is blown or on error
 *
 */
void
tzbsp_blow_fuses_and_reset(void)
{
  fuseprov_error_etype       status                = FUSEPROV_FAILURE;
  tzbsp_dload_id_type*       dload_magic_ptr       = (tzbsp_dload_id_type*) DLOAD_ID_ADDR; //from target.builds header
  uint8*                     secdat_buffer         = (uint8*)TZBSP_SECDAT_BUFFER_BASE+TZBSP_SECDAT_BUFFER_OFFSET;
  uint32                     secdat_len            = TZBSP_SECDAT_BUFFER_SIZE;

  device_reset_needed = FALSE;

  /* All validations of secdat buffer happen inside here */
  status = tzbsp_blow_fuses(secdat_buffer, secdat_len);

  /* The status could say there were no fuses to blow or that the fuse that says ignore secdat is blown (i.e OEM SECBOOT
     write perm fuse blown) in which case we don't consider it an error and just boot up as normal */
  if ((status != FUSEPROV_SUCCESS ) && (status != FUSEPROV_SECDAT_DEFAULT_NOFUSES) && (status != FUSEPROV_SECDAT_LOCK_BLOWN))
  {
    /* An error occured. Error handling for fuse blowing service:
       TZ logs to TZ diag region. Write magic number in IMEM for DLOAD and reset device, so it goes into
       SBL download mode. RAM dumps can be collected to know what error occured if the secure boot fuse is not blown */

    TZBSP_LOG_DBG("FP:(0x%8X),(0x%8X)\n",__LINE__,status);

    /* These SBL DLOAD mode magic numbers (see tzbsp_dload_mode.c) */
    dload_magic_ptr->magic_1 = TZBSP_DLOAD_MAGIC_NUM_1;
    dload_magic_ptr->magic_2 = TZBSP_DLOAD_MAGIC_NUM_2;
    //From tzbsp_memory_dump.c, tzbsp_wdt_reset(), this line does the actual reset
    //#define TZBSP_WDT_DELAY_MS          500
    tzbsp_wdt_start(500 * 2, 500);
    while(1);
  }
  else
  {
    if (device_reset_needed)
    {
      TZBSP_LOG_DBG("FP:Info(0x%8X),(0x%8X)\n",__LINE__,status);

      //enable this sectino once PMIC promotion for pon is in
      //Since fuses were blown successfully, cause a hard reset to happen so the fuses take effect on
      //next boot up - i.e the fuse sense registers are populated correct etc.  
      pm_pon_ps_hold_cfg(0, PM_PON_RESET_CFG_HARD_RESET);
  
      /* pull down pmic PS_HOLD (drop ps_hold) */
      /* Note: Device's PS_HOLD dip switch must be in OFF position - i.e won't force high */    
      HWIO_OUTF( MPM2_MPM_PS_HOLD, PSHOLD, 0x0 );    
      while(1);
    }
  }

  /* Clear the buffer as this is a normal boot up */
  memset(secdat_buffer, 0, secdat_len);  
}
