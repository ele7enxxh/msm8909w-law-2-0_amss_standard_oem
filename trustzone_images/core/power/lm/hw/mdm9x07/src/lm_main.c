/*==============================================================================

  FILE:         lm_main.c

  OVERVIEW:     This file maintain S1 LM characterisation data which is used
                for S1 limit mitigation

  DEPENDENCIES: None

                Copyright (c) 2014 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: 
$DateTime: 
==============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "lm_internal.h"

/* Global flag indicates LM initialisation status */
boolean  lm_init_done                          = FALSE;
lm_mitigation_lookup_data_type *lm_lookup_table = NULL;

/* Hash table, it maps to LM look up table*/
uint16   lm_hash_table[LM_HASH_TABLE_SIZE_MAX];
uint32   lm_lookup_table_size         = 0;

//Global variables 
uint32  g_s1_lm_return_1 = 0;
uint32  g_s1_lm_return_2 = 0;
uint32  g_s1_lm_return_3 = 0;
boolean g_lm_clk_div_vote = 0;
uint32  g_iddq_value = 0;

/**
 * <!-- lm_init -->
 * 
 * @brief lm initialization code
 */

 void lm_init(void)
{
  uint16 nCnt = 0;
  
  /* read IDDQ APC ative value and check with limit */
  g_iddq_value =  LM_IDDQ_APC_ACTIVE_VALUE(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_IDDQ_APC_ACTIVE_BMSK);
  
  /* choose appropriate look-up table */
  if(g_iddq_value < LM_IDDQ_APC_ACTIVE_VALUE_LIMIT)
  {
    lm_lookup_table = lookup_table_normal;
    lm_lookup_table_size = sizeof(lookup_table_normal)/sizeof(lm_mitigation_lookup_data_type);
  }
  else
  {
    lm_lookup_table = lookup_table_high;
    lm_lookup_table_size = sizeof(lookup_table_high)/sizeof(lm_mitigation_lookup_data_type);
  }
  
  /* initialize hash_table to zero */
  memset( (void *)lm_hash_table, 0, sizeof(lm_hash_table) );
  
  /* Fill Hash table with corresponding LM table  indeces */
  for( nCnt = 0; nCnt < lm_lookup_table_size; nCnt++)
  {
    uint16 hash_value = lm_lookup_table[nCnt].hash_value;
    if(hash_value < LM_HASH_TABLE_SIZE_MAX)
    {
      /* fill in hash table with mitigation table index */
      lm_hash_table[hash_value] = nCnt;
    }
  }
  
  //initialize S1LM clock div value with 0x3
  HWIO_APCS_S1LM_SEC_DIVIDER_DIV_VALUE_OUTM(HWIO_APCS_S1LM_SEC_DIVIDER_DIV_VALUE_CDIV_DIV_VALUE_BMSK, LM_INITIAL_CLK_DIV_VALUE);
  
  lm_init_done = TRUE;
}

/**
 * <!-- lm_mitigatio_state_from_look_up_table -->
 * 
 * @brief provide mitigation state for given lm input
 */
int lm_get_mitigation_state_from_look_up_table( uint32 arg1, uint32 arg2,
                                                uint32 arg3, uint32 arg4,
												uint32 arg5)
{
  int status = LM_FAILED;
  uint32 hash_value = 0;
  
  /* Check whether LM driver is initialized or not */
  if( !lm_init_done )
  {
    g_s1_lm_return_1 = 0;
    g_s1_lm_return_2 = 0;
    g_s1_lm_return_3 = 0;
    return LM_FAILED;
  }
	
  /* Input validation */
  if( (arg1 >= LM_THERMAL_LEVEL_MAX) || ((arg2 -1) >= LM_ACPU_CNT_MAX) ||
      (arg3 >= LM_CORNER_STATE_MAX) || (arg4 >= LM_CORNER_STATE_MAX) ||
	  (arg5 >= LM_CORNER_STATE_MAX) )
  {
    return LM_INVALID_ARG;
  }

  /* compute hash value from input parameters */
  hash_value = (arg1 * LM_LKP_INDEX_1)  + ((arg2 - 1) * LM_LKP_INDEX_2)  +  
               (arg3 * LM_LKP_INDEX_3)  + (arg4 * LM_LKP_INDEX_4) + (arg5 * LM_LKP_INDEX_5);

  if(hash_value < LM_HASH_TABLE_SIZE_MAX)
  {
    uint32 index = 0;
    
    /* get lm mitigation table index */
    index = lm_hash_table[hash_value];
	
    if(index < lm_lookup_table_size)
    {
      //apply step1 mitigation
      if(lm_lookup_table[index].pre_clk_div_vote != g_lm_clk_div_vote)
      {
        HWIO_GCC_S1LM_APPS_DIV_VOTE_OUTM(HWIO_GCC_S1LM_APPS_DIV_VOTE_S1LM_APPS_DIV_VOTE_BMSK, lm_lookup_table[index].pre_clk_div_vote);
        g_lm_clk_div_vote = lm_lookup_table[index].pre_clk_div_vote;
      }
	  
      /* return mitigation state from the table */
      g_s1_lm_return_1 = lm_lookup_table[index].core_online_req;
      g_s1_lm_return_2 = lm_lookup_table[index].freq_req;
      g_s1_lm_return_3 = (lm_lookup_table[index].pre_clk_div_vote & LM_CLK_DIV_VOTE_BMASK) |
                          ((lm_lookup_table[index].post_clk_div_vote & LM_CLK_DIV_VOTE_BMASK) << LM_POST_CLK_DIV_VOTE_LSHIFT);
      status = LM_SUCCESS;
    }
    else
    {
      //Debug info
      status = LM_FAILED;
      g_s1_lm_return_1 = hash_value;
      g_s1_lm_return_2 = index;
      g_s1_lm_return_3 = 0;
    }
  }
  else
  {
    //Debug info
    status= LM_FAILED;
    g_s1_lm_return_1 = hash_value;
    g_s1_lm_return_2 = 0;
    g_s1_lm_return_3 = 0;
  }

  return status;
}

int lm_apply_clk_div_vote(uint32 arg1)
{
  int status = LM_SUCCESS;
  if(arg1)
  {
    HWIO_GCC_S1LM_APPS_DIV_VOTE_OUTM(HWIO_GCC_S1LM_APPS_DIV_VOTE_S1LM_APPS_DIV_VOTE_BMSK, 0x1);
    g_lm_clk_div_vote = 0x1;
  }
  else
  {
    HWIO_GCC_S1LM_APPS_DIV_VOTE_OUTM(HWIO_GCC_S1LM_APPS_DIV_VOTE_S1LM_APPS_DIV_VOTE_BMSK, 0x0);
    g_lm_clk_div_vote = 0x0;
  }
  return status;
}
