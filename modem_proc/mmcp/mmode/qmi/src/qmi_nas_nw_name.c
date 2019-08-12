/*=========================================================================== 
 
                         DS_QMI_NAS_MMGSDI.C 
 
DESCRIPTION 
 
 The Qualcomm Network Access Services MMGSDI Interface source file. 
 
   
Copyright (c) 2010 Qualcomm Technologies Incorporated. 
All Rights Reserved. 
Qualcomm Confidential and Proprietary. 
===========================================================================*/ 
/*=========================================================================== 
 
                      EDIT HISTORY FOR FILE 
 
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/qmi/src/qmi_nas_nw_name.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $ 
 
when        who    what, where, why 
--------    ---    ---------------------------------------------------------- 
01/10/11    sm     Initial version 
===========================================================================*/

#include "qmi_nas.h"
#include "qmi_nas_internal.h"
#include "qm_efs.h"
#include "mmgsdisessionlib.h"

#define QMI_NAS_MMGSDI_EONS_RECORDS    (2) // caching information from 2 most recent MMGSDI_EONS_EVT 
#define NEW_RECORD             (0)
#define OLD_RECORD             (1)
static mmgsdi_eons_evt_info_type eons[QMI_NAS_MMGSDI_SESSION_MAX][QMI_NAS_MMGSDI_EONS_RECORDS];

/*=========================================================================== 
  FUNCTION QMI_NAS_NW_NAME_INIT() 

  DESCRIPTION 
  Initialize cache to empty

  RETURN VALUE 
  None 

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
===========================================================================*/
void qmi_nas_nw_name_init()
{
  memset(&eons, 0, sizeof(eons));
}

/*=========================================================================== 
  FUNCTION QMI_NAS_NW_NAME_ADD() 

  DESCRIPTION 
  Store nw name information in the cache

  RETURN VALUE 
  None 

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None
===========================================================================*/
void qmi_nas_nw_name_add(
  mmgsdi_session_type_enum_type session,
  mmgsdi_eons_evt_info_type     evt_info
)
{
  uint32 i = 0;
  qmi_nas_mmgsdi_session_e_type qmi_session = qmi_nas_map_mmgsdi_session_to_qmi( session );

  if( qmi_nas_mmgsdi_is_valid_qmi_session(qmi_session) )
  {
    // Dealloc memory for name being removed, for with display condition information
    if( eons[qmi_session][OLD_RECORD].spn.eons_data.data_ptr != NULL )
    {
      modem_mem_free( eons[qmi_session][OLD_RECORD].spn.eons_data.data_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
      eons[qmi_session][OLD_RECORD].spn.eons_data.data_ptr = NULL;
    }
    if ( eons[qmi_session][OLD_RECORD].first_stack_info.is_valid)
    {
      if( eons[qmi_session][OLD_RECORD].first_stack_info.operator_info.plmn_long_name.plmn_name.eons_data.data_ptr != NULL )
      {
        modem_mem_free( eons[qmi_session][OLD_RECORD].first_stack_info.operator_info.plmn_long_name.plmn_name.eons_data.data_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
        eons[qmi_session][OLD_RECORD].first_stack_info.operator_info.plmn_long_name.plmn_name.eons_data.data_ptr = NULL;
      }
      if( eons[qmi_session][OLD_RECORD].first_stack_info.operator_info.plmn_short_name.plmn_name.eons_data.data_ptr != NULL )
      {
        modem_mem_free( eons[qmi_session][OLD_RECORD].first_stack_info.operator_info.plmn_short_name.plmn_name.eons_data.data_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
        eons[qmi_session][OLD_RECORD].first_stack_info.operator_info.plmn_short_name.plmn_name.eons_data.data_ptr = NULL;
      }
      if ( eons[qmi_session][OLD_RECORD].first_stack_info.operator_info.plmn_additional_info.info_data.data_ptr != NULL )
      {
        modem_mem_free( eons[qmi_session][OLD_RECORD].first_stack_info.operator_info.plmn_additional_info.info_data.data_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
        eons[qmi_session][OLD_RECORD].first_stack_info.operator_info.plmn_additional_info.info_data.data_ptr = NULL;
      }
      for ( i = 0; i < eons[qmi_session][OLD_RECORD].first_stack_info.operator_info.num_plmn_additional_names; i++ )
      {
        if( eons[qmi_session][OLD_RECORD].first_stack_info.operator_info.plmn_additional_names[i].plmn_long_name.plmn_name.eons_data.data_ptr != NULL )
        {
          modem_mem_free( eons[qmi_session][OLD_RECORD].first_stack_info.operator_info.plmn_additional_names[i].plmn_long_name.plmn_name.eons_data.data_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
          eons[qmi_session][OLD_RECORD].first_stack_info.operator_info.plmn_additional_names[i].plmn_long_name.plmn_name.eons_data.data_ptr = NULL;
        }
        if( eons[qmi_session][OLD_RECORD].first_stack_info.operator_info.plmn_additional_names[i].plmn_short_name.plmn_name.eons_data.data_ptr != NULL )
        {
          modem_mem_free( eons[qmi_session][OLD_RECORD].first_stack_info.operator_info.plmn_additional_names[i].plmn_short_name.plmn_name.eons_data.data_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
          eons[qmi_session][OLD_RECORD].first_stack_info.operator_info.plmn_additional_names[i].plmn_short_name.plmn_name.eons_data.data_ptr = NULL;
        }
      }
    }
    // for second stack
    if ( eons[qmi_session][OLD_RECORD].sec_stack_info.is_valid )
    {
      if( eons[qmi_session][OLD_RECORD].sec_stack_info.operator_info.plmn_long_name.plmn_name.eons_data.data_ptr != NULL )
      {
        modem_mem_free( eons[qmi_session][OLD_RECORD].sec_stack_info.operator_info.plmn_long_name.plmn_name.eons_data.data_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
        eons[qmi_session][OLD_RECORD].sec_stack_info.operator_info.plmn_long_name.plmn_name.eons_data.data_ptr = NULL;
      }
      if( eons[qmi_session][OLD_RECORD].sec_stack_info.operator_info.plmn_short_name.plmn_name.eons_data.data_ptr != NULL )
      {
        modem_mem_free( eons[qmi_session][OLD_RECORD].sec_stack_info.operator_info.plmn_short_name.plmn_name.eons_data.data_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
        eons[qmi_session][OLD_RECORD].sec_stack_info.operator_info.plmn_short_name.plmn_name.eons_data.data_ptr = NULL;
      }
      if ( eons[qmi_session][OLD_RECORD].sec_stack_info.operator_info.plmn_additional_info.info_data.data_ptr != NULL )
      {
        modem_mem_free( eons[qmi_session][OLD_RECORD].sec_stack_info.operator_info.plmn_additional_info.info_data.data_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
        eons[qmi_session][OLD_RECORD].sec_stack_info.operator_info.plmn_additional_info.info_data.data_ptr = NULL;
      }
      for( i = 0; i < eons[qmi_session][OLD_RECORD].sec_stack_info.operator_info.num_plmn_additional_names; i++ )
      {
        if( eons[qmi_session][OLD_RECORD].sec_stack_info.operator_info.plmn_additional_names[i].plmn_long_name.plmn_name.eons_data.data_ptr != NULL )
        {
          modem_mem_free( eons[qmi_session][OLD_RECORD].sec_stack_info.operator_info.plmn_additional_names[i].plmn_long_name.plmn_name.eons_data.data_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
          eons[qmi_session][OLD_RECORD].sec_stack_info.operator_info.plmn_additional_names[i].plmn_long_name.plmn_name.eons_data.data_ptr = NULL;
        }
        if( eons[qmi_session][OLD_RECORD].sec_stack_info.operator_info.plmn_additional_names[i].plmn_short_name.plmn_name.eons_data.data_ptr != NULL )
        {
          modem_mem_free( eons[qmi_session][OLD_RECORD].sec_stack_info.operator_info.plmn_additional_names[i].plmn_short_name.plmn_name.eons_data.data_ptr, MODEM_MEM_CLIENT_QMI_MMODE );
          eons[qmi_session][OLD_RECORD].sec_stack_info.operator_info.plmn_additional_names[i].plmn_short_name.plmn_name.eons_data.data_ptr = NULL;
        }
      }
    }

    // Move old information and store new
    memscpy(&eons[qmi_session][OLD_RECORD], sizeof( mmgsdi_eons_evt_info_type ),
            &eons[qmi_session][NEW_RECORD], sizeof( mmgsdi_eons_evt_info_type ) );
    memscpy(&eons[qmi_session][NEW_RECORD], sizeof( mmgsdi_eons_evt_info_type ),
            &evt_info, sizeof( mmgsdi_eons_evt_info_type ) );

    // Alloc memory for name ptrs and store them, for with display condition information
    if( eons[qmi_session][NEW_RECORD].spn.eons_data.data_len > 0 )
    {
      eons[qmi_session][NEW_RECORD].spn.eons_data.data_ptr = modem_mem_alloc(eons[qmi_session][NEW_RECORD].spn.eons_data.data_len, MODEM_MEM_CLIENT_QMI_MMODE);
      if( eons[qmi_session][NEW_RECORD].spn.eons_data.data_ptr != NULL )
      {
        memscpy(eons[qmi_session][NEW_RECORD].spn.eons_data.data_ptr, eons[qmi_session][NEW_RECORD].spn.eons_data.data_len,
                evt_info.spn.eons_data.data_ptr, eons[qmi_session][NEW_RECORD].spn.eons_data.data_len );
      }
    }
    if ( eons[qmi_session][NEW_RECORD].first_stack_info.is_valid )
    {
      if( eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.plmn_long_name.plmn_name.eons_data.data_len > 0 )
      {
        eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.plmn_long_name.plmn_name.eons_data.data_ptr = modem_mem_alloc(eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.plmn_long_name.plmn_name.eons_data.data_len, MODEM_MEM_CLIENT_QMI_MMODE);
        if( eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.plmn_long_name.plmn_name.eons_data.data_ptr != NULL )
        {
          memscpy(eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.plmn_long_name.plmn_name.eons_data.data_ptr, 
                  eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.plmn_long_name.plmn_name.eons_data.data_len,
                  evt_info.first_stack_info.operator_info.plmn_long_name.plmn_name.eons_data.data_ptr, 
                  eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.plmn_long_name.plmn_name.eons_data.data_len );
        }
      }
      if( eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.plmn_short_name.plmn_name.eons_data.data_len > 0 )
      {
        eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.plmn_short_name.plmn_name.eons_data.data_ptr = modem_mem_alloc(eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.plmn_short_name.plmn_name.eons_data.data_len, MODEM_MEM_CLIENT_QMI_MMODE);
        if( eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.plmn_short_name.plmn_name.eons_data.data_ptr != NULL )
        {
          memscpy(eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.plmn_short_name.plmn_name.eons_data.data_ptr, 
                  eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.plmn_short_name.plmn_name.eons_data.data_len,
                  evt_info.first_stack_info.operator_info.plmn_short_name.plmn_name.eons_data.data_ptr, 
                  eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.plmn_short_name.plmn_name.eons_data.data_len );
        }
      }
      if( eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.plmn_additional_info.info_data.data_len > 0 )
      {
        eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.plmn_additional_info.info_data.data_ptr = modem_mem_alloc(eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.plmn_additional_info.info_data.data_len, MODEM_MEM_CLIENT_QMI_MMODE);
        if( eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.plmn_additional_info.info_data.data_ptr != NULL )
        {
          memscpy(eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.plmn_additional_info.info_data.data_ptr, 
                  eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.plmn_additional_info.info_data.data_len,
                  evt_info.first_stack_info.operator_info.plmn_additional_info.info_data.data_ptr, 
                  eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.plmn_additional_info.info_data.data_len );
        }
      }
      QM_MSG_HIGH_1("Additional names first stack %d", eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.num_plmn_additional_names);
      for ( i = 0; i < eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.num_plmn_additional_names; i++ )
      {
        if( eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.plmn_additional_names[i].plmn_long_name.plmn_name.eons_data.data_len > 0 )
        {
          eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.plmn_additional_names[i].plmn_long_name.plmn_name.eons_data.data_ptr = modem_mem_alloc(eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.plmn_additional_names[i].plmn_long_name.plmn_name.eons_data.data_len, MODEM_MEM_CLIENT_QMI_MMODE);
          if( eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.plmn_additional_names[i].plmn_long_name.plmn_name.eons_data.data_ptr != NULL )
          {
            memscpy( eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.plmn_additional_names[i].plmn_long_name.plmn_name.eons_data.data_ptr,
                    eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.plmn_additional_names[i].plmn_long_name.plmn_name.eons_data.data_len,
                    evt_info.first_stack_info.operator_info.plmn_additional_names[i].plmn_long_name.plmn_name.eons_data.data_ptr, 
                    eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.plmn_additional_names[i].plmn_long_name.plmn_name.eons_data.data_len );
          }
        }
        if( eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.plmn_additional_names[i].plmn_short_name.plmn_name.eons_data.data_len > 0 )
        {
          eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.plmn_additional_names[i].plmn_short_name.plmn_name.eons_data.data_ptr = modem_mem_alloc(eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.plmn_additional_names[i].plmn_short_name.plmn_name.eons_data.data_len, MODEM_MEM_CLIENT_QMI_MMODE);
          if( eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.plmn_additional_names[i].plmn_short_name.plmn_name.eons_data.data_ptr != NULL )
          {
            memscpy( eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.plmn_additional_names[i].plmn_short_name.plmn_name.eons_data.data_ptr,
                     eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.plmn_additional_names[i].plmn_short_name.plmn_name.eons_data.data_len,
                     evt_info.first_stack_info.operator_info.plmn_additional_names[i].plmn_short_name.plmn_name.eons_data.data_ptr, 
                     eons[qmi_session][NEW_RECORD].first_stack_info.operator_info.plmn_additional_names[i].plmn_short_name.plmn_name.eons_data.data_len );
          }
        }
      }
    }
    // for second stack
    if ( eons[qmi_session][NEW_RECORD].sec_stack_info.is_valid )
    {
      if( eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.plmn_long_name.plmn_name.eons_data.data_len > 0 )
      {
        eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.plmn_long_name.plmn_name.eons_data.data_ptr = modem_mem_alloc(eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.plmn_long_name.plmn_name.eons_data.data_len, MODEM_MEM_CLIENT_QMI_MMODE);
        if( eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.plmn_long_name.plmn_name.eons_data.data_ptr != NULL )
        {
          memscpy(eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.plmn_long_name.plmn_name.eons_data.data_ptr, 
                  eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.plmn_long_name.plmn_name.eons_data.data_len,
                  evt_info.sec_stack_info.operator_info.plmn_long_name.plmn_name.eons_data.data_ptr, 
                  eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.plmn_long_name.plmn_name.eons_data.data_len );
        }
      }
      if( eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.plmn_short_name.plmn_name.eons_data.data_len > 0 )
      {
        eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.plmn_short_name.plmn_name.eons_data.data_ptr = modem_mem_alloc(eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.plmn_short_name.plmn_name.eons_data.data_len, MODEM_MEM_CLIENT_QMI_MMODE);
        if( eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.plmn_short_name.plmn_name.eons_data.data_ptr != NULL )
        {
          memscpy(eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.plmn_short_name.plmn_name.eons_data.data_ptr, 
                  eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.plmn_short_name.plmn_name.eons_data.data_len,
                  evt_info.sec_stack_info.operator_info.plmn_short_name.plmn_name.eons_data.data_ptr, 
                  eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.plmn_short_name.plmn_name.eons_data.data_len );
        }
      }
      if( eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.plmn_additional_info.info_data.data_len > 0 )
      {
        eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.plmn_additional_info.info_data.data_ptr = modem_mem_alloc(eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.plmn_additional_info.info_data.data_len, MODEM_MEM_CLIENT_QMI_MMODE);
        if( eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.plmn_additional_info.info_data.data_ptr != NULL )
        {
          memscpy(eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.plmn_additional_info.info_data.data_ptr, 
                  eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.plmn_additional_info.info_data.data_len,
                  evt_info.sec_stack_info.operator_info.plmn_additional_info.info_data.data_ptr, 
                  eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.plmn_additional_info.info_data.data_len );
        }
      }
      QM_MSG_HIGH_1("Additional names sec stack %d", eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.num_plmn_additional_names);
      for ( i = 0; i < eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.num_plmn_additional_names; i++ )
      {
        if( eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.plmn_additional_names[i].plmn_long_name.plmn_name.eons_data.data_len > 0 )
        {
          eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.plmn_additional_names[i].plmn_long_name.plmn_name.eons_data.data_ptr = modem_mem_alloc(eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.plmn_additional_names[i].plmn_long_name.plmn_name.eons_data.data_len, MODEM_MEM_CLIENT_QMI_MMODE);
          if( eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.plmn_additional_names[i].plmn_long_name.plmn_name.eons_data.data_ptr != NULL )
          {
            memscpy( eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.plmn_additional_names[i].plmn_long_name.plmn_name.eons_data.data_ptr,
                     eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.plmn_additional_names[i].plmn_long_name.plmn_name.eons_data.data_len,
                     evt_info.sec_stack_info.operator_info.plmn_additional_names[i].plmn_long_name.plmn_name.eons_data.data_ptr, 
                     eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.plmn_additional_names[i].plmn_long_name.plmn_name.eons_data.data_len );
          }
        }
        if( eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.plmn_additional_names[i].plmn_short_name.plmn_name.eons_data.data_len > 0 )
        {
          eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.plmn_additional_names[i].plmn_short_name.plmn_name.eons_data.data_ptr = modem_mem_alloc(eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.plmn_additional_names[i].plmn_short_name.plmn_name.eons_data.data_len, MODEM_MEM_CLIENT_QMI_MMODE);
          if( eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.plmn_additional_names[i].plmn_short_name.plmn_name.eons_data.data_ptr != NULL )
          {
            memscpy( eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.plmn_additional_names[i].plmn_short_name.plmn_name.eons_data.data_ptr,
                     eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.plmn_additional_names[i].plmn_short_name.plmn_name.eons_data.data_len, 
                     evt_info.sec_stack_info.operator_info.plmn_additional_names[i].plmn_short_name.plmn_name.eons_data.data_ptr, 
                     eons[qmi_session][NEW_RECORD].sec_stack_info.operator_info.plmn_additional_names[i].plmn_short_name.plmn_name.eons_data.data_len );
          }
        }
      }
    }
  }

  QM_MSG_HIGH_2("Received new eons op stack %d, %d ", eons[qmi_session][NEW_RECORD].first_stack_info.is_valid, eons[qmi_session][NEW_RECORD].sec_stack_info.is_valid);
}

/*=========================================================================== 
  FUNCTION QMI_NAS_NW_NAME_FINE() 

  DESCRIPTION 
  Get nw name information for a specific plmn_id, if it is cached 

  RETURN VALUE 
  A pointer to the cached name, or NULL if information is unavailable 

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
===========================================================================*/
mmgsdi_eons_evt_info_type * qmi_nas_nw_name_find(
  mmgsdi_session_type_enum_type session,
  sys_plmn_id_s_type plmn_id,
  sys_lac_type lac_tac,
  mmgsdi_rat_enum_type rat,
  sys_csg_id_type csg_id
)
{
  mmgsdi_eons_evt_info_type * eons_info = NULL;
  int i;
  qmi_nas_mmgsdi_session_e_type qmi_session = qmi_nas_map_mmgsdi_session_to_qmi( session );

  if( qmi_nas_mmgsdi_is_valid_qmi_session(qmi_session) )
  {
    for( i = 0; i < QMI_NAS_MMGSDI_EONS_RECORDS; i++ )
    {
      // check if main stack matches first
      if ( eons[qmi_session][i].first_stack_info.is_valid )
      {
#ifdef FEATURE_FEMTO_CSG
        // either CSG is not supported, or csg id has to match
        if ( !qm_efs_csg_supported() ||
             eons[qmi_session][i].first_stack_info.operator_info.plmn_id.csg_id == csg_id )
#endif
        {
          if( eons[qmi_session][i].first_stack_info.operator_info.lac == lac_tac &&
              !memcmp( eons[qmi_session][i].first_stack_info.operator_info.plmn_id.plmn_id_val, plmn_id.identity, sizeof(plmn_id) ) &&
              eons[qmi_session][i].first_stack_info.operator_info.plmn_id.rat == rat )
          {
            eons_info = &eons[qmi_session][i];
            break;
          }
        }
      }
      // check if second stack matches
      if ( eons[qmi_session][i].sec_stack_info.is_valid )
      {
#ifdef FEATURE_FEMTO_CSG
        // either CSG is not supported, or csg id has to match
        if ( !qm_efs_csg_supported() ||
             eons[qmi_session][i].sec_stack_info.operator_info.plmn_id.csg_id == csg_id )
#endif
        {
          if( eons[qmi_session][i].sec_stack_info.operator_info.lac == lac_tac &&
              !memcmp( eons[qmi_session][i].sec_stack_info.operator_info.plmn_id.plmn_id_val, plmn_id.identity, sizeof(plmn_id) ) &&
              eons[qmi_session][i].sec_stack_info.operator_info.plmn_id.rat == rat )
          {
            eons_info = &eons[qmi_session][i];
            break;
          }
        }
      }
    }
  }

  return eons_info;
}

/*=========================================================================== 
  FUNCTION QMI_NAS_NW_NAME_INFO_STACK() 

  DESCRIPTION 
  Determine which stack contains the nw name info, given a set of eons
  information and stack information.

  RETURN VALUE 
  The stack to use for operator info, or NONE if the information does not match

  DEPENDENCIES 
  None 

  SIDE EFFECTS 
  None 
===========================================================================*/
enum qmi_nas_stack_e qmi_nas_nw_name_info_stack(
   mmgsdi_eons_evt_info_type * eons_info,
   sys_plmn_id_s_type plmn_id,
   sys_lac_type lac_tac,
   mmgsdi_rat_enum_type rat,
   sys_csg_id_type csg_id
)
{
  // check if main stack matches first
  if ( eons_info->first_stack_info.is_valid )
  {
#ifdef FEATURE_FEMTO_CSG
    // either CSG is not supported, or csg id has to match
    if ( !qm_efs_csg_supported() ||
         eons_info->first_stack_info.operator_info.plmn_id.csg_id == csg_id )
#endif
    {
      if( eons_info->first_stack_info.operator_info.lac == lac_tac &&
          !memcmp( eons_info->first_stack_info.operator_info.plmn_id.plmn_id_val, plmn_id.identity, sizeof(plmn_id) ) &&
          eons_info->first_stack_info.operator_info.plmn_id.rat == rat )
      {
        return QMI_NAS_STACK_MAIN;
      }
    }
  }
  // check if second stack matches
  if ( eons_info->sec_stack_info.is_valid )
  {
#ifdef FEATURE_FEMTO_CSG
    // either CSG is not supported, or csg id has to match
    if ( !qm_efs_csg_supported() ||
         eons_info->sec_stack_info.operator_info.plmn_id.csg_id == csg_id )
#endif
    {
      if( eons_info->sec_stack_info.operator_info.lac == lac_tac &&
          !memcmp( eons_info->sec_stack_info.operator_info.plmn_id.plmn_id_val, plmn_id.identity, sizeof(plmn_id) ) &&
          eons_info->sec_stack_info.operator_info.plmn_id.rat == rat )
      {
        return QMI_NAS_STACK_GW_HYB;
      }
    }
  }

  return QMI_NAS_STACK_NONE;
}

