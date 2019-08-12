
/*!
  @file lte_rrc_irat_to_w_sib6_info.cpp

  @brief
  Unit test code for the W RAT manager sib6 procedure.

*/

/*===========================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/test/irat/w/lte_rrc_irat_to_w_test_sib6_info.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/29/14   gf      Initial Revision
           
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

extern "C"
{
#include "lte_rrc_int_msg.h"
#include "lte_rrc_ext_msg.h"
#include "lte_rrc_utils.h"
#include "__lte_rrc_irat_to_w_mgr.h"
#include "lte_rrc_irat_to_w_mgri.h"
#include "lte_rrc_osys_asn1util.h"
#include "lte_rrc_controller.h"
#include "sys.h"
#include "lte_rrc_mem_util.h"
#ifdef FEATURE_QTF_CRM_TODO
#include "AEEstd.h"
#endif//FEATURE_QTF_CRM_TODO
}

#include "TestFramework.h"
#include "lte_rrc_utf.h"
#include "lte_rrc_irat_to_w_test_common.h"
#include "lte_rrc_meas_test_msg.h"
#include <tf_stub.h>
/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/

/*
SIB6:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib6 : 
                {
                  carrierFreqListUTRA-FDD 
                  {
                    {
                      carrierFreq 100,
                      cellReselectionPriority 5,
                      threshX-High 25,
                      threshX-Low 15,
                      q-RxLevMin -30,
                      p-MaxUTRA 5,
                      q-QualMin -10
                    },
                    {
                      carrierFreq 200,
                      cellReselectionPriority 3,
                      threshX-High 25,
                      threshX-Low 10,
                      q-RxLevMin -30,
                      p-MaxUTRA 1,
                      q-QualMin -5
                    },
                    {
                      carrierFreq 300,
                      cellReselectionPriority 3,
                      threshX-High 25,
                      threshX-Low 12,
                      q-RxLevMin -40,
                      p-MaxUTRA 5,
                      q-QualMin -10
                    }
                  },
                  t-ReselectionUTRA 3,
                  t-ReselectionUTRA-SF 
                  {
                    sf-Medium oDot5,
                    sf-High lDot0
                  }
                }
            }
          }
      }
}
*/

static const uint8 lte_rrc_to_w_test_sib6_msg1[] = \
{ \
  0x00, 0x11, 0x49, 0x01, 0x92, 0xE5, 0xEF, 0x37, 0x72, 0x06, 0x43, 0xCA, \
  0x9E, 0x67, 0x34, 0x12, 0xC7, 0x96, 0x28, 0xDD, 0xCD, 0xC0  \
};


/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

                                FUNCTIONS

===========================================================================*/


/*===========================================================================

                                UNIT TEST

===========================================================================*/

void lte_rrc_irat_to_w_test_send_sib6 ( void )
{
  lte_rrc_sib_updated_indi_s    sib_updated_indi; 
#ifdef FEATURE_QTF_CRM_TODO
  STD_ZEROAT(&sib_updated_indi);
#endif//FEATURE_QTF_CRM_TODO

  TF_MSG ( "WRATMGR UT: Sending event LTE_RRC_SIB_UPDATED_INDI" );

  // Send sib updated indi to the W RAT MGR
  lte_rrc_init_default_hdr ( &sib_updated_indi.msg_hdr, 
                             LTE_RRC_SIB_UPDATED_INDI );

  // Send SIB3, SIB4 and SIB6 updated indication
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                              LTE_RRC_SIB6_BITMASK;

  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
                                            LTE_RRC_SIB6_BITMASK;

  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq        = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done ();
}

void lte_rrc_irat_to_w_test_send_sib6_for_new_cell ( void )
{
  lte_rrc_sib_updated_indi_s    sib_updated_indi; 
#ifdef FEATURE_QTF_CRM_TODO
  STD_ZEROAT(&sib_updated_indi);
#endif//FEATURE_QTF_CRM_TODO

  TF_MSG ( "wRATMGR UT: Sending event LTE_RRC_SIB_UPDATED_INDI" );

  // Send sib updated indi to the w RAT MGR
  lte_rrc_init_default_hdr ( &sib_updated_indi.msg_hdr, 
                             LTE_RRC_SIB_UPDATED_INDI );

  // Send SIB3, SIB4 and SIB6 updated indication
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                              LTE_RRC_SIB6_BITMASK;

  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
                                            LTE_RRC_SIB6_BITMASK;

  sib_updated_indi.cell_id.phy_cell_id = 0x20;
  sib_updated_indi.cell_id.freq        = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done ();
}

void lte_rrc_irat_to_w_test_send_sib6_not_available (void )
{
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ; 
#ifdef FEATURE_QTF_CRM_TODO
  STD_ZEROAT(&sib_updated_indi);
#endif//FEATURE_QTF_CRM_TODO

  TF_MSG ( "wRATMGR UT: Sending event LTE_RRC_SIB_UPDATED_INDI (SIB6 not available)" );

  // Send sib updated indi to the 1x RAT MGR
  lte_rrc_init_default_hdr ( &sib_updated_indi.msg_hdr, 
                             LTE_RRC_SIB_UPDATED_INDI );

  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK;

  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq        = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done ();
}

void lte_rrc_irat_to_w_test_send_sib6_not_available_for_new_cell (void )
{
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ; 
#ifdef FEATURE_QTF_CRM_TODO
  STD_ZEROAT(&sib_updated_indi);
#endif//FEATURE_QTF_CRM_TODO

  TF_MSG ( "1xRATMGR UT: Sending event LTE_RRC_SIB_UPDATED_INDI (SIB8 not available)" );

  // Send sib updated indi to the 1x RAT MGR
  lte_rrc_init_default_hdr ( &sib_updated_indi.msg_hdr, 
                             LTE_RRC_SIB_UPDATED_INDI );

  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK;

  sib_updated_indi.cell_id.phy_cell_id = 0x20;
  sib_updated_indi.cell_id.freq        = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done ();
}

void lte_rrc_irat_to_w_test_verify_sib6( void )
{
  uint32                         buf_len               = 0;
  lte_rrc_sib6_info_ind_s*      ind_ptr      = NULL;

  TF_MSG ( "wRATMGR UT: Verify SIB-6" );

  lte_rrc_utf_get_next_ext_msg ( (byte**) &ind_ptr, &buf_len );

  TF_ASSERT ( buf_len                == sizeof ( lte_rrc_sib6_info_ind_s ) );
  TF_ASSERT ( ind_ptr->msg_hdr.id    == LTE_RRC_SIB6_INFO_IND );

  // SIB8 should be present
  TF_ASSERT ( ind_ptr->sib6_present == TRUE );

  // Check priority
  TF_ASSERT ( ind_ptr->priority_UTRAN_FDD == 5 );
  TF_ASSERT ( ind_ptr->priority_UTRAN_TDD == LTE_RRC_INVALID_SIB_PRIORITY);


  
}

void lte_rrc_irat_to_w_test_verify_sib6_not_available ( void )
{
  uint32                         buf_len      = 0;
  lte_rrc_sib6_info_ind_s*    ind_ptr      = NULL;

  TF_MSG ( "wRATMGR UT: Verify SIB-6" );

  lte_rrc_utf_get_next_ext_msg ( (byte**) &ind_ptr, &buf_len );

  TF_ASSERT ( buf_len                == sizeof ( lte_rrc_sib6_info_ind_s ) );
  TF_ASSERT ( ind_ptr->msg_hdr.id    == LTE_RRC_SIB6_INFO_IND );
  
  TF_ASSERT ( ind_ptr->sib6_present  == FALSE );

  // Check priority
  TF_ASSERT ( ind_ptr->priority_UTRAN_FDD == 0 );
  TF_ASSERT ( ind_ptr->priority_UTRAN_TDD == LTE_RRC_INVALID_SIB_PRIORITY);

}

/*===========================================================================

                        lte_rrc_irat_to_w_test_sib6_ind

===========================================================================*/
/**
    @brief

    @detail

    @return
    None
*/

TF_DEFINE_TEST_CASE ( COMPONENT, lte_rrc_irat_to_w_test_sib6_ind ) ;

void lte_rrc_irat_to_w_test_sib6_ind::Setup()
{
  /* Only add those external UMIDs that 1X RAT Mgr will send out */
  uint32 umid_list[] = 
    { 
      LTE_RRC_SIB6_INFO_IND
    };

  lte_rrc_utf_setup(LTE_RRC_IRAT_TO_W_MGR_SM, umid_list,
         sizeof(umid_list)/sizeof(umid_list[0]));

  lte_rrc_irat_to_w_test_sib6_ind_init ( );
}/*  lte_rrc_irat_to_w_test_sib6_ind */

void lte_rrc_irat_to_w_test_sib6_ind::Test()
{

  // Make sure that the state machine has been activated and that the 
  // initial state is 'Initial'
  TF_ASSERT(stm_get_state(LTE_RRC_IRAT_TO_W_MGR_SM) == LTE_RRC_IRAT_TO_W_MGR_SM__INITIAL);

  lte_rrc_to_w_mgr_test_decode_sib6((void*) lte_rrc_to_w_test_sib6_msg1,
                                sizeof(lte_rrc_to_w_test_sib6_msg1));
  
  // Send SIB-6
  lte_rrc_irat_to_w_test_send_sib6 ();

  // Receive and verifiy that IRAT Mgr sends SIB-6
  lte_rrc_irat_to_w_test_verify_sib6 ();

  // Make sure that the state machine has been activated and that the 
  // initial state is 'Initial'
  TF_ASSERT(stm_get_state(LTE_RRC_IRAT_TO_W_MGR_SM) == LTE_RRC_IRAT_TO_W_MGR_SM__INITIAL);

  // Send SIB-6 not available
  lte_rrc_irat_to_w_test_send_sib6_not_available ();

  // Receive and verifiy that IRAT Mgr sends SIB-6 not avail
  lte_rrc_irat_to_w_test_verify_sib6_not_available ();


   // Make sure that the state machine has been activated and that the 
  // initial state is 'Initial'
  TF_ASSERT(stm_get_state(LTE_RRC_IRAT_TO_W_MGR_SM) == LTE_RRC_IRAT_TO_W_MGR_SM__INITIAL);

  // Send SIB-6 not available
  lte_rrc_irat_to_w_test_send_sib6_not_available ();

  // Should not result in any outcome
  // because we already sent not available to 1xCP

  // Send SIB-6 available again now
  lte_rrc_irat_to_w_test_send_sib6 ();

  // Receive and verifiy that IRAT Mgr sends SIB-6
  lte_rrc_irat_to_w_test_verify_sib6 ();

   // Make sure that the state machine has been activated and that the 
  // initial state is 'Initial'
  TF_ASSERT(stm_get_state(LTE_RRC_IRAT_TO_W_MGR_SM) == LTE_RRC_IRAT_TO_W_MGR_SM__INITIAL);

  // Reset the UTF framework
  lte_rrc_utf_reset();
}/* lte_rrc_irat_to_w_test_sib6_ind */

void lte_rrc_irat_to_w_test_sib6_ind::Teardown()
{
  lte_rrc_utf_teardown();
}/* lte_rrc_irat_to_w_test_sib6_ind */

TF_DEFINE_TEST_CASE ( COMPONENT, lte_rrc_irat_to_w_test_sib6_ind_new_cells ) ;

void lte_rrc_irat_to_w_test_sib6_ind_new_cells::Setup()
{
  /* Only add those external UMIDs that 1X RAT Mgr will send out */
  uint32 umid_list[] = 
    { 
      LTE_RRC_SIB6_INFO_IND
    };

  lte_rrc_utf_setup(LTE_RRC_IRAT_TO_W_MGR_SM, umid_list,
         sizeof(umid_list)/sizeof(umid_list[0]));

  lte_rrc_irat_to_w_test_sib6_ind_init ( );
}/*  lte_rrc_irat_to_w_test_sib6_ind_new_cells */

void lte_rrc_irat_to_w_test_sib6_ind_new_cells::Test()
{

  // Make sure that the state machine has been activated and that the 
  // initial state is 'Initial'
  TF_ASSERT(stm_get_state(LTE_RRC_IRAT_TO_W_MGR_SM) == LTE_RRC_IRAT_TO_W_MGR_SM__INITIAL);

  lte_rrc_to_w_mgr_test_decode_sib6((void*) lte_rrc_to_w_test_sib6_msg1,
                                sizeof(lte_rrc_to_w_test_sib6_msg1));
  
  // Send SIB-6
  lte_rrc_irat_to_w_test_send_sib6 ();

  // Receive and verifiy that IRAT Mgr sends SIB-6
  lte_rrc_irat_to_w_test_verify_sib6 ();

  // Make sure that the state machine has been activated and that the 
  // initial state is 'Initial'
  TF_ASSERT(stm_get_state(LTE_RRC_IRAT_TO_W_MGR_SM) == LTE_RRC_IRAT_TO_W_MGR_SM__INITIAL);

  // move to a new cell
  lte_rrc_irat_to_w_test_sib6_ind_init_new_serv_cell();

  // sib6 not avail in new cell
  lte_rrc_irat_to_w_test_send_sib6_not_available_for_new_cell();

  // Receive and verifiy that IRAT Mgr sends SIB-6 not avail
  lte_rrc_irat_to_w_test_verify_sib6_not_available ();

  TF_ASSERT(stm_get_state(LTE_RRC_IRAT_TO_W_MGR_SM) == LTE_RRC_IRAT_TO_W_MGR_SM__INITIAL);

  // move back to prev cell
  lte_rrc_irat_to_w_test_sib6_ind_init();

  // Send SIB-6 available again now
  lte_rrc_irat_to_w_test_send_sib6 ();

  // Receive and verifiy that IRAT Mgr sends SIB-6
  lte_rrc_irat_to_w_test_verify_sib6 ();

   // Make sure that the state machine has been activated and that the 
  // initial state is 'Initial'
  TF_ASSERT(stm_get_state(LTE_RRC_IRAT_TO_W_MGR_SM) == LTE_RRC_IRAT_TO_W_MGR_SM__INITIAL);

  // Reset the UTF framework
  lte_rrc_utf_reset();
}/* lte_rrc_irat_to_w_test_sib6_ind_new_cells */

void lte_rrc_irat_to_w_test_sib6_ind_new_cells::Teardown()
{
  lte_rrc_utf_teardown();
}/* lte_rrc_irat_to_w_test_sib6_ind_new_cells */
