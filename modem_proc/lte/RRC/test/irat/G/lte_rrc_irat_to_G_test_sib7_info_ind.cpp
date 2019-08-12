


/*!
  @file lte_rrc_irat_to_g_sib7_info.cpp

  @brief
  Unit test code for the G RAT manager sib7 procedure.

*/

/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/test/irat/G/lte_rrc_irat_to_G_test_sib7_info_ind.cpp#1 $

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
#include "__lte_rrc_irat_to_g_mgr.h"
#include "lte_rrc_irat_to_g_mgri.h"
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
#include "lte_rrc_to_g_test_msg.h"
#include "lte_rrc_meas_test_msg.h"
#include <tf_stub.h>
/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/
/*
SIB7:value BCCH-DL-SCH-Message ::= 
{
  message c1 : systemInformation : 
      {
        criticalExtensions systemInformation-r8 : 
          {
            sib-TypeAndInfo 
            {
              sib7 : 
                {
                  t-ReselectionGERAN 4,
                  t-ReselectionGERAN-SF 
                  {
                    sf-Medium oDot5,
                    sf-High oDot75
                  },
                  carrierFreqsInfoList 
                  {
                    {
                      carrierFreqs 
                      {
                        startingARFCN 50,
                        bandIndicator dcs1800,
                        followingARFCNs explicitListOfARFCNs : 
                          {
                            55,
                            60,
                            65,
                            70,
                            75,
                            80,
                            85
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '00000101'B,
                        q-RxLevMin 35,
                        p-MaxGERAN 20,
                        threshX-High 30,
                        threshX-Low 15
                      }
                    },
                    {
                      carrierFreqs 
                      {
                        startingARFCN 150,
                        bandIndicator pcs1900,
                        followingARFCNs equallySpacedARFCNs : 
                          {
                            arfcn-Spacing 5,
                            numberOfFollowingARFCNs 5
                          }
                      },
                      commonInfo 
                      {
                        cellReselectionPriority 5,
                        ncc-Permitted '00000011'B,
                        q-RxLevMin 40,
                        p-MaxGERAN 28,
                        threshX-High 30,
                        threshX-Low 10
                      }
                    }
                  }
                }
            }
          }
      }
}
*/

static const uint8 lte_rrc_to_g_test_sib7_msg1[] = \
{ \
  0x00, 0x15, 0xC6, 0x10, 0x64, 0x0E, 0x1B, 0x87, 0x82, 0x08, 0x8C, 0x25, \
  0x8A, 0x02, 0xAF, 0x41, 0x63, 0x53, 0xCF, 0x12, 0xD6, 0x17, 0xA0, 0x74, \
  0x39, 0xE5, 0x00 \
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

void lte_rrc_irat_to_g_test_send_sib7 ( void )
{
  lte_rrc_sib_updated_indi_s    sib_updated_indi; 
#ifdef FEATURE_QTF_CRM_TODO
  STD_ZEROAT(&sib_updated_indi);
#endif//FEATURE_QTF_CRM_TODO

  TF_MSG ( "WRATMGR UT: Sending event LTE_RRC_SIB_UPDATED_INDI" );

  // Send sib updated indi to the G RAT MGR
  lte_rrc_init_default_hdr ( &sib_updated_indi.msg_hdr, 
                             LTE_RRC_SIB_UPDATED_INDI );

  // Send SIB3, SIB4 and SIB7 updated indication
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                              LTE_RRC_SIB7_BITMASK;

  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
                                            LTE_RRC_SIB7_BITMASK;

  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq        = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done ();
}

void lte_rrc_irat_to_g_test_send_sib7_for_new_cell ( void )
{
  lte_rrc_sib_updated_indi_s    sib_updated_indi; 
#ifdef FEATURE_QTF_CRM_TODO
  STD_ZEROAT(&sib_updated_indi);
#endif//FEATURE_QTF_CRM_TODO

  TF_MSG ( "gRATMGR UT: Sending event LTE_RRC_SIB_UPDATED_INDI" );

  // Send sib updated indi to the g RAT MGR
  lte_rrc_init_default_hdr ( &sib_updated_indi.msg_hdr, 
                             LTE_RRC_SIB_UPDATED_INDI );

  // Send SIB3, SIB4 and SIB7 updated indication
  sib_updated_indi.sibs_changed_bitmask = LTE_RRC_SIB3_BITMASK | 
                                          LTE_RRC_SIB4_BITMASK |
                                              LTE_RRC_SIB7_BITMASK;

  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK | 
                                            LTE_RRC_SIB4_BITMASK |
                                            LTE_RRC_SIB7_BITMASK;

  sib_updated_indi.cell_id.phy_cell_id = 0x20;
  sib_updated_indi.cell_id.freq        = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done ();
}

void lte_rrc_irat_to_g_test_send_sib7_not_available (void )
{
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ; 
#ifdef FEATURE_QTF_CRM_TODO
  STD_ZEROAT(&sib_updated_indi);
#endif//FEATURE_QTF_CRM_TODO

  TF_MSG ( "wRATMGR UT: Sending event LTE_RRC_SIB_UPDATED_INDI (SIB7 not available)" );

  // Send sib updated indi
  lte_rrc_init_default_hdr ( &sib_updated_indi.msg_hdr, 
                             LTE_RRC_SIB_UPDATED_INDI );

  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK;

  sib_updated_indi.cell_id.phy_cell_id = 0x10;
  sib_updated_indi.cell_id.freq        = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done ();
}

void lte_rrc_irat_to_g_test_send_sib7_not_available_for_new_cell (void )
{
  lte_rrc_sib_updated_indi_s    sib_updated_indi   ; 
#ifdef FEATURE_QTF_CRM_TODO
  STD_ZEROAT(&sib_updated_indi);
#endif//FEATURE_QTF_CRM_TODO

  TF_MSG ( "gRATMGR UT: Sending event LTE_RRC_SIB_UPDATED_INDI (SIB7 not available)" );

  // Send sib updated indi to the 1x RAT MGR
  lte_rrc_init_default_hdr ( &sib_updated_indi.msg_hdr, 
                             LTE_RRC_SIB_UPDATED_INDI );

  sib_updated_indi.sibs_scheduled_bitmask = LTE_RRC_SIB3_BITMASK;

  sib_updated_indi.cell_id.phy_cell_id = 0x20;
  sib_updated_indi.cell_id.freq        = 0x100;

  lte_rrc_utf_send_msg((byte*)&sib_updated_indi, sizeof(sib_updated_indi));

  lte_rrc_utf_wait_for_done ();
}

void lte_rrc_irat_to_g_test_verify_sib7( void )
{
  uint32                         buf_len               = 0;
  lte_rrc_sib7_info_ind_s*      ind_ptr      = NULL;

  TF_MSG ( "wRATMGR UT: Verify SIB-7" );

  lte_rrc_utf_get_next_ext_msg ( (byte**) &ind_ptr, &buf_len );

  TF_ASSERT ( buf_len                == sizeof ( lte_rrc_sib7_info_ind_s ) );
  TF_ASSERT ( ind_ptr->msg_hdr.id    == LTE_RRC_SIB7_INFO_IND );

  // SIB8 should be present
  TF_ASSERT ( ind_ptr->sib7_present == TRUE );

  // Check priority
  TF_ASSERT ( ind_ptr->priority == 5 );
  
}

void lte_rrc_irat_to_g_test_verify_sib7_not_available ( void )
{
  uint32                         buf_len      = 0;
  lte_rrc_sib7_info_ind_s*    ind_ptr      = NULL;

  TF_MSG ( "wRATMGR UT: Verify SIB-6" );

  lte_rrc_utf_get_next_ext_msg ( (byte**) &ind_ptr, &buf_len );

  TF_ASSERT ( buf_len                == sizeof ( lte_rrc_sib7_info_ind_s ) );
  TF_ASSERT ( ind_ptr->msg_hdr.id    == LTE_RRC_SIB7_INFO_IND );
  
  TF_ASSERT ( ind_ptr->sib7_present  == FALSE );

  // Check priority
  TF_ASSERT ( ind_ptr->priority == 0 );
}

/*===========================================================================

                        lte_rrc_irat_to_g_test_sib7_ind

===========================================================================*/
/**
    @brief

    @detail

    @return
    None
*/

TF_DEFINE_TEST_CASE ( COMPONENT, lte_rrc_irat_to_g_test_sib7_ind ) ;

void lte_rrc_irat_to_g_test_sib7_ind::Setup()
{
  /* Only add those external UMIDs that 1X RAT Mgr will send out */
  uint32 umid_list[] = 
    { 
      LTE_RRC_SIB7_INFO_IND
    };

  lte_rrc_utf_setup(LTE_RRC_IRAT_TO_G_MGR_SM, umid_list,
         sizeof(umid_list)/sizeof(umid_list[0]));

  lte_rrc_irat_to_g_test_sib7_ind_init ( );
}/*  lte_rrc_irat_to_g_test_sib7_ind */

void lte_rrc_irat_to_g_test_sib7_ind::Test()
{

  // Make sure that the state machine has been activated and that the 
  // initial state is 'Initial'
  TF_ASSERT(stm_get_state(LTE_RRC_IRAT_TO_G_MGR_SM) == LTE_RRC_IRAT_TO_G_MGR_SM__INITIAL);

  lte_rrc_to_g_test_decode_sib7((void*) lte_rrc_to_g_test_sib7_msg1,
                                sizeof(lte_rrc_to_g_test_sib7_msg1));
  
  // Send SIB-7
  lte_rrc_irat_to_g_test_send_sib7 ();

  // Receive and verifiy that IRAT Mgr sends SIB-7
  lte_rrc_irat_to_g_test_verify_sib7 ();

  // Make sure that the state machine has been activated and that the 
  // initial state is 'Initial'
  TF_ASSERT(stm_get_state(LTE_RRC_IRAT_TO_G_MGR_SM) == LTE_RRC_IRAT_TO_G_MGR_SM__INITIAL);

  // Send SIB-6 not available
  lte_rrc_irat_to_g_test_send_sib7_not_available ();

  // Receive and verifiy that IRAT Mgr sends SIB-7 not avail
  lte_rrc_irat_to_g_test_verify_sib7_not_available ();


   // Make sure that the state machine has been activated and that the 
  // initial state is 'Initial'
  TF_ASSERT(stm_get_state(LTE_RRC_IRAT_TO_G_MGR_SM) == LTE_RRC_IRAT_TO_G_MGR_SM__INITIAL);

  // Send SIB-6 not available
  lte_rrc_irat_to_g_test_send_sib7_not_available ();

  // Should not result in any outcome
  // because we already sent not available

  // Send SIB-6 available again now
  lte_rrc_irat_to_g_test_send_sib7 ();

  // Receive and verifiy that IRAT Mgr sends SIB-7
  lte_rrc_irat_to_g_test_verify_sib7 ();

   // Make sure that the state machine has been activated and that the 
  // initial state is 'Initial'
  TF_ASSERT(stm_get_state(LTE_RRC_IRAT_TO_G_MGR_SM) == LTE_RRC_IRAT_TO_G_MGR_SM__INITIAL);

  // Reset the UTF framework
  lte_rrc_utf_reset();
}/* lte_rrc_irat_to_g_test_sib7_ind */

void lte_rrc_irat_to_g_test_sib7_ind::Teardown()
{
  lte_rrc_utf_teardown();
}/* lte_rrc_irat_to_g_test_sib7_ind */

TF_DEFINE_TEST_CASE ( COMPONENT, lte_rrc_irat_to_g_test_sib7_ind_new_cells ) ;

void lte_rrc_irat_to_g_test_sib7_ind_new_cells::Setup()
{
  /* Only add those external UMIDs that 1X RAT Mgr will send out */
  uint32 umid_list[] = 
    { 
      LTE_RRC_SIB7_INFO_IND
    };

  lte_rrc_utf_setup(LTE_RRC_IRAT_TO_G_MGR_SM, umid_list,
         sizeof(umid_list)/sizeof(umid_list[0]));

  lte_rrc_irat_to_g_test_sib7_ind_init ( );
}/*  lte_rrc_irat_to_g_test_sib7_ind_new_cells */

void lte_rrc_irat_to_g_test_sib7_ind_new_cells::Test()
{

  // Make sure that the state machine has been activated and that the 
  // initial state is 'Initial'
  TF_ASSERT(stm_get_state(LTE_RRC_IRAT_TO_G_MGR_SM) == LTE_RRC_IRAT_TO_G_MGR_SM__INITIAL);

  lte_rrc_to_g_test_decode_sib7((void*) lte_rrc_to_g_test_sib7_msg1,
                                sizeof(lte_rrc_to_g_test_sib7_msg1));
  
  // Send SIB-7
  lte_rrc_irat_to_g_test_send_sib7 ();

  // Receive and verifiy that IRAT Mgr sends SIB-7
  lte_rrc_irat_to_g_test_verify_sib7 ();

  // Make sure that the state machine has been activated and that the 
  // initial state is 'Initial'
  TF_ASSERT(stm_get_state(LTE_RRC_IRAT_TO_G_MGR_SM) == LTE_RRC_IRAT_TO_G_MGR_SM__INITIAL);

  // move to a new cell
  lte_rrc_irat_to_g_test_sib7_ind_init_new_serv_cell();

  // sib7 not avail in new cell
  lte_rrc_irat_to_g_test_send_sib7_not_available_for_new_cell();

  // Receive and verifiy that IRAT Mgr sends SIB-6 not avail
  lte_rrc_irat_to_g_test_verify_sib7_not_available ();

  TF_ASSERT(stm_get_state(LTE_RRC_IRAT_TO_G_MGR_SM) == LTE_RRC_IRAT_TO_G_MGR_SM__INITIAL);

  // move back to prev cell
  lte_rrc_irat_to_g_test_sib7_ind_init();

  // Send SIB-6 available again now
  lte_rrc_irat_to_g_test_send_sib7 ();

  // Receive and verifiy that IRAT Mgr sends SIB-6
  lte_rrc_irat_to_g_test_verify_sib7 ();

   // Make sure that the state machine has been activated and that the 
  // initial state is 'Initial'
  TF_ASSERT(stm_get_state(LTE_RRC_IRAT_TO_G_MGR_SM) == LTE_RRC_IRAT_TO_G_MGR_SM__INITIAL);

  // Reset the UTF framework
  lte_rrc_utf_reset();
}/* lte_rrc_irat_to_g_test_sib7_ind_new_cells */

void lte_rrc_irat_to_g_test_sib7_ind_new_cells::Teardown()
{
  lte_rrc_utf_teardown();
}/* lte_rrc_irat_to_g_test_sib7_ind_new_cells */
