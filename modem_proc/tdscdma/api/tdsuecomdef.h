#ifndef TDSUECOMDEF_H
#define TDSUECOMDEF_H

#include "uecomdef.h"
#ifndef REX_H
#include "rex.h"
#endif 

/*copied from newly added tds items to uecomdef.h
May need to revert back later*/


typedef enum
{
/*Duplicate TDS items --hx*/
  UE_UMTS_CIPHER_ALGO_NONE = 0x0,
  /*Ciphering Algo for No encryption, Value 1 for Bit 0 ?indicate UEA0*/
  UE_UMTS_CIPHER_ALGO_UEA0 = 0x1,
  
  /*KASUMI Ciphering Algo, Value 1 for Bit 1 ?indicate UEA1 */
  UE_UMTS_CIPHER_ALGO_UEA1 = 0x2,
  
  /*SNOW-3G Ciphering Algo,Value 1 for Bit 2 ?indicate UEA2 */
  UE_UMTS_CIPHER_ALGO_UEA2 = 0x4
} uecomdef_umts_cipher_algo_e_type;

/*typedef uecomdef_wcdma_cipher_algo_e_type uecomdef_umts_cipher_algo_e_type;*/

/*-------------------------------------------------------------------
TYPE: uecomdef_integrity_protection_algorithm_e_type

The following enumeration is for all supported Integrity Protection
Algorithmtypes.
--------------------------------------------------------------------*/   
typedef enum
{
  /*Invalid Integrity Algo */
  /*Duplicate TDS items --hx*/
  UE_UMTS_INTEGRITY_PROT_ALGO_NONE = 0x0,

  /*KASUMI Integrity Algo */
  UE_UMTS_INTEGRITY_PROT_ALGO_UIA1 = 0x1,
  
  /*SNOW-3G Integrity Algo */
  UE_UMTS_INTEGRITY_PROT_ALGO_UIA2 = 0x2
} uecomdef_umts_integrity_protection_algorithm_e_type;

/*typedef uecomdef_wcdma_integrity_protection_algorithm_e_type uecomdef_umts_integrity_protection_algorithm_e_type;*/





/* TDS defines for interrupt lock and task lock.                                           */
/********************************************************************/

/* Mutex for DOB handle */

extern rex_crit_sect_type tds_crit_sect;

#define TDS_INTLOCK() REX_ISR_LOCK(&tds_crit_sect)
#define TDS_INTFREE() REX_ISR_UNLOCK(&tds_crit_sect)

#define TDS_TASKLOCK() TASKLOCK()
#define TDS_TASKFREE() TASKFREE()


/* RLC Capabilities                                                 */
/********************************************************************/

#define TDSUE_MAX_USER_PLAN_AM_ENTITIES   ( SYS_MAX_PRIMARY_PDP_CONTEXTS +   \
                                         ( SYS_MAX_PRIMARY_PDP_CONTEXTS * \
                                          SYS_MAX_SEC_PDP_CONTEXT_PER_PRIMARY ) )

/* RF capabilities                                                  */
/********************************************************************/
/* Frequency separation in MHZ, as defined in 25.101 */

/* Physical Channel capabilities                                    */
/********************************************************************/

/*Add TDS only channel capabilities --hx*/


/*===================================================================
                        CONSTANTS
=====================================================================*/
/*********************************************************************
* If there is a change between the constants defined in the 25.331
* standard (RRC) and implementation it is noted. For instance, MAX_TRCH
* is 32 as defined in the standard, and TDSUE_MAX_TRCH is 8 as defined
* in the QCT implementation. In other words as UE_ in front of the
* constant defined in the standard indicates that the QCT implementation
* differs from that specified in the RRC spec, 25.331. Those values are
* listed prior to all of the RRC constants as defined in 25.331.
**********************************************************************/
/*===================================================================
                 Implemenation Specific UE Capabilities
=====================================================================*/

/*-------------------------------------------------------------------
* This defines the Maximum number of Uplink Unacknowleged Mode Channels
* that are supported by the QCT implementation.
---------------------------------------------------------------------*/
/*Duplicate TDS items --hx*/
#define TDSUE_MAX_UL_UM_CHANNEL             16

/*-------------------------------------------------------------------
* This defines the Maximum number of Downlink Unacknowleged Mode Channels
* that are supported by the QCT implementation.
---------------------------------------------------------------------*/
/*Duplicate TDS items --hx*/
#define TDSUE_MAX_DL_UM_CHANNEL             16

/*-------------------------------------------------------------------
* This defines the Maximum number of Acknowleged Mode Entities that
* are supported by the QCT implementation.
---------------------------------------------------------------------*/
/*Duplicate TDS items --hx*/
#define TDSUE_MAX_AM_ENTITY                ( UE_MAX_SRB_AM_EMTITIES_3 + \
                                         TDSUE_MAX_USER_PLAN_AM_ENTITIES )


/*-------------------------------------------------------------------
* This defines the Maximum number of Uplink Data Plane Acknowleged
* Mode Channels that are supported by the QCT implementation.
---------------------------------------------------------------------*/
/*Duplicate TDS items --hx*/
#define TDSUE_MAX_UL_AM_DATA_CHANNEL        TDSUE_MAX_AM_ENTITY


/*-------------------------------------------------------------------
* This defines the Maximum number of Downlink Data Plane Acknowleged
* Mode Channels that are supported by the QCT implementation.
---------------------------------------------------------------------*/

/*Duplicate TDS items --hx*/
#define TDSUE_MAX_DL_AM_DATA_CHANNEL        TDSUE_MAX_AM_ENTITY

/*-------------------------------------------------------------------
* This defines the Maximum number of all Uplink Logical Channel types
* that are supported by the QCT implementation.
---------------------------------------------------------------------*/
/*Duplicate TDS items --hx*/
#define TDSUE_MAX_UL_LOGICAL_CHANNEL        ( UE_MAX_UL_TM_CHANNEL + \
                                           TDSUE_MAX_UL_UM_CHANNEL + \
                                           UE_MAX_UL_AM_CHANNEL   \
                                         )

/*-------------------------------------------------------------------
* This defines the Maximum number of all Downlink Logical Channel types
* that are supported by the QCT implementation.
---------------------------------------------------------------------*/
/*Duplicate TDS items --hx*/
#define TDSUE_MAX_DL_LOGICAL_CHANNEL        ( UE_MAX_DL_TM_CHANNEL + \
                                           TDSUE_MAX_DL_UM_CHANNEL + \
                                           UE_MAX_DL_AM_CHANNEL   \
                                         )

/*-------------------------------------------------------------------
* This defines the Maximum number of Uplink Packet Data Units (PDUs)
* allowed per frame that are supported by the QCT implementation.
---------------------------------------------------------------------*/
/*Duplicate TDS items --hx*/
#define TDSUE_MAX_UL_RLC_PDUS              24

/*-------------------------------------------------------------------
* This defines the Maximum number of Downlink Packet Data Units (PDUs)
* allowed per frame that are supported by the QCT implementation.
---------------------------------------------------------------------*/
/*Duplicate TDS items --hx*/
#define TDSUE_MAX_DL_RLC_PDUS              70

/*-------------------------------------------------------------------
* This defines the Maximum number of Transport Channels that can be
* supported by the QCT UE Implementation.
* This is the same for both the uplink and downlink.
---------------------------------------------------------------------*/
/*Duplicate TDS items --hx*/
#define TDSUE_MAX_TRCH                      8

/*-------------------------------------------------------------------
* This defines the Maximum number of Coded Composite Transport Channels
* (CCTrCH) supported by the QCT UE Implementation. Note that only
* TDSUE_MAX_CCTRCH / 2 CCTrCHs can be active at a time. Only 1 CCTrCH 
* is allowed in the Uplink.
* Note that this is uplink + downlink. The number of allowed CCTrCHs
* to be setup by RRC is 5 since L1 reserves one for BCHs while in
* CELL_DCH state.
---------------------------------------------------------------------*/
/*Duplicate TDS items --hx*/
#define TDSUE_MAX_CCTRCH                    11

/*-------------------------------------------------------------------
* This defines the Maximum number of different transport formats that
* can be included in the Transport format set for all transport
* channels as supported by the UE according to 25.331 in the Downlink.
* Note that this value as calculated by using values defined the 
* standard (25.331) would be 32x32 = 1024.
---------------------------------------------------------------------*/
/*Duplicate TDS items --hx*/
#define TDSUE_MAX_TF                        (UE_MAX_NUM_TF_64)


/*-------------------------------------------------------------------
* This defines the Maximum number of transport format combinations
* as supported by the QCT UE Implemenation.
---------------------------------------------------------------------*/
/*Duplicate TDS items --hx*/
#define TDSUE_MAX_TFC                       (UE_MAX_NUM_TFC_128)

/*-------------------------------------------------------------------
* This defines the Maximum number of SCCPCHs per cell
* as supported by the QCT UE Implementation.
---------------------------------------------------------------------*/
/*Duplicate TDS items --hx*/
#define TDSUE_MAX_SCCPCH                    16

/*Duplicate TDS items --hx*/
#define TDSUE_MAX_RL                        6


/*===================================================================
                     User Equipment (UE) Information
=====================================================================*/
/*===================================================================
                      Radio Bearer (RB) Information
=====================================================================*/

/*-------------------------------------------------------------------
* This defines the Maximum number of E-DCH MAC-D flows 
* according to 25.331 (REL6)
---------------------------------------------------------------------*/
/*Duplicate TDS items --hx*/
#define TDSMAX_E_DCH_MAC_D_FLOW            8

/*===================================================================
                   Physical Channel (PhyCH) Information
=====================================================================*/
/*===================================================================
                         Frequency Information
=====================================================================*/
/*following are types*/

#define TDS_MAX_RB                33

#ifdef FEATURE_DUAL_SIM
#define TDSCDMA_NUM_SUBS 2
#else
#define TDSCDMA_NUM_SUBS 1
#endif

#endif
