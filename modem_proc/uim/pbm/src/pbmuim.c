/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                          PHONEBOOK MANAGER UIM ENTRIES

GENERAL DESCRIPTION
  This file contains the functions used to manage phonebook entries within the
  EFS.

  Copyright (c) 2002 - 2015 by QUALCOMM Technologies, Inc(QTI).
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/pbm/src/pbmuim.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/25/16   sp      F3 improvements
05/24/16   nr      Logging improvements
05/11/16   sp      Logging improvements
05/11/16   nr      Logging improvements
12/04/15   nr      Fix to ignore invalid tags present in EF-PBR record for non spec cards
06/16/15   nr      Validating EF-EXT data
06/09/15   stv     Fix to address multiple initialization in parallel on card session
05/26/15   stv     Need not include AAS/GAS in FDN enable/disable clearing
12/15/14   nr      Freeing Hidden cache entries as part of card error event
11/14/14   NR      Fixing medium and low level compiler warnings
09/22/14   NR      GRP-ID should be validated before reading and writing to SIM
09/10/14   NR      Fix to query for WMS ready in case of REFRESH while provisioning 
                   session is ongoing
07/29/14   am      Correctly direct/redirect atomic_ops APIs across MOBs 
07/16/14   stv     Ignore the PBR record if it doesnt have adn reference or incorrect adn reference 
07/09/14   NR      Use v2.0 diag macros
07/07/14   NR      Fixing medium and low level compiler warnings
05/19/14   NR      Fix for allowing PBM to read/write EF_ADN when it is 
                   invalidated but card allows read/write when invalidated
05/16/14   NR      Enabling or Disabling ADN based on NV configurations
05/01/14   NR      FR 19033: Reduce Code Duplication pbm
04/10/14   NR      Adding 2 bytes to GAS and AAS text length for UCS2 NULL 
                   character to support session_record_write for GAS or AAS 
                   with maxmum text length
02/20/14   NR      SIM_INIT_DONE being repeated with FDN enabled and Disabled
02/08/14   NR      1X session INIT stopped in ICC mode
01/10/14   NR      Operation Delete AAS Record not working
01/10/14   NR      Issue with PBM SIM INTI indication seen thrice
10/10/13  nalavala Don't extract the CNF incase of failure from MMGSDI
10/04/13   stv     F3 message reduction 
10/03/13  nalavala memcpy banned api replacement with memscpy
10/01/13   sv      Add checks to validate the data in EF IAP 
09/16/10  krishnac Refresh by path changes
04/30/10  krishnac Do not close the sessions during card errors
                     and reuse them for future valid sessions
05/10/10   krishna provided the clients Session based field info
11/05/09   krishnac CR 211973  MO live Call fails if FDN enabled RUIM is used
                     (Fixed issue populating file access method)
07/10/09  ashwanik Support for CSIM and Dual Sim Features
06/10/09   kcp     clearing the ADN cache when FDN enabled or disabled CR182391
05/04/09   krishna changes for CR 167785
12/26/08   kcp     adding changes for ASUS CR 165600
09/09/08   sg      Don't read IAP file(s) if there are no TYPE 2 files
09/03/08   sg      Don't SEEK/SEARCH on transparent files
07/07/08   sg      Use mmgsdi_seek() to skip over unused records to improve
                   SIM initialization time
06/12/08   sg      Fix deallocation of cache records in pbm_fileinfo_complete()
03/14/08   clm     Add PBM_EVENT_SIM_INIT_DONE event.
03/13/08   clm     Add restart of SIM read init for enable/disable FDN
01/17/08   clm     Add FEATURE_PBM_USIM_SUPPORT to featurize USIM features.
11/02/07   clm     Add refresh okay and init complete support to pbm for mmgsdi.
08/31/07   cvs     Fix issue with fcn during init
05/18/07   cvs     Fix abort when receiving SIM_INIT_COMPLETED while initializing.
03/12/07   cvs     Featurize 4 PB set support
02/23/07   cvs     Clear stale extension record indicator before writing USIM fields
02/22/07   cvs     Keep track of filetype for read + write ops
02/09/07   cvs     Support for 4 phonebook sets
01/16/07   cvs     Add event for phonebooks becoming ready
11/06/06   cvs     Fixes for cards with PB sets with different text sizes.
10/24/06   cvs     Register for refresh on all files, to fix FDN refresh issue
11/01/06   cvs     multi pb set fixes
10/10/06   cvs     Move messages to PBM's diag msg ID
10/03/06   cvs     Add support for MBN
08/15/06   cvs     fix crash w/enable+disable FDN with type 2 USIM fields
08/04/06   cvs     fix extension record overwrite/delete issues
07/27/06   slw     Fix menu items displayed when adding FDNs
07/25/06   slw     Added error reporting for pbm_create_sim_record call in pbm_sim_write
07/24/06   cvs     Fixes for unequal size multi pb set support
06/14/06   aps     support for ANRC and ANRC1 type records
06/13/06   aps     Fix for mem leak when FDN is enabled and disabled.
05/31/06   cvs     Fix support of multiple ANR files
05/03/06   cvs     convert to MMGSDI APIs.  Lint cleanup.
03/24/06   cvs     Fix GSDI error handling code in fileinfo_cb.
03/23/06   cvs     Fix MBDN UST entry.  Lint fixes.
03/22/06   cvs     better logging
03/13/06   cvs     add error handling in pbm_fileinfo_cb() if GSDI returns error
02/28/06   cvs     Add support for MBDN
02/23/06   cvs     rework memory allocation macros
02/22/06   cvs     Better error reporting on SIM write failures
02/09/06   cvs     When can't encode all characters on SIM, return errors
01/09/06   cvs     Lint Fixes
12/20/05   cvs     Fix reboot (infinite loop) when adding to a full USIM file
12/14/05   cvs     Fix data abort when IAP is programmed incorrectly.
12/13/05   cvs     Implement partial PB refresh
11/16/05   cvs     Lint Fixes
09/22/05   cvs     Free caches_in_use earlier.
08/22/05   AT      Moved contents inside of a FEATURE_PBM_TASK.
08/12/05   AT      Added UCS2 support for strings.
08/12/05   cvs     When extension records go unused, write out blank record
08/04/05   cvs     Recover from committed list for SIM errors
07/27/05   cvs     Cleanup more coverity errors
07/19/05   AT      Fix issue where PIN becomes blocked and unblocked
                   after SIM_INIT_COMPLETED.  Also, add range checking
                   on sim_id and pb_set before using them as an array index.
07/13/05   cvs     Init unused group file entries to 0 (no group), not ff
07/11/05   cvs     Better error messages
06/14/05   cvs     Fix for not being able to add ADN if ADN file is enumerated last.
05/26/05   cvs     support for API changes to make APIs more multiprocessor friendly
05/05/05   cvs     Clear out caches_in_use after deleting old record
04/29/05   cvs     caches_in_use is one based, so allocate at num_caches+1
04/12/05   AT      Only update a USIM sync value for ADN files.
03/22/05   PA      Do not update unique id for deletion/update.
                   Changed PBM_MAX_PBR_RECS to PBM_MAX_NUM_PB_SETS.
                   Fixed issue in IAP update for Type 2 ANR.
03/08/05   PA      Support for PBM notifications. Double check #records in ADN
                   cache before initializing USIM files.
02/23/05   PA      Added back pbmuim.c#8 changes and usage of special
                   service enums.
02/23/05   PA      Reverted to pbmuim.c#7 and using special GSDI service enums
                   for USIM.
02/08/05   PA      Support for MSISDN, check if IAP file is present.
01/18/05   PA      Conversion between 7-bit and 8-bit encoding
01/14/05   PA      Make PBM GSTK unaware
01/12/05   PA      Remove stub code for GSDI
01/12/05   PA      PBM USIM Support
11/19/04   PA      Support non-GSDI builds, code review comments.
11/02/04   PA      Support for SDN
09/29/04   PA      Initial Revision
===========================================================================*/
#include "customer.h"
#include "pbm.h"
#include "pbmtask_i.h"
#include "pbmtask.h"
#include "uim_msg.h"
#include "pbmuim.h"
#include "pbmgsdi.h"
#include "pbmcache.h"
#include "pbmutils.h"
#include "pbm_conversion.h"
#include "mmgsdilib.h"
#include "atomic_ops.h"
#include "stringl/stringl.h"

#define PBM_NUM_FILES_IN_PB_FILE_ENUM_LOOKUP_TABLE 0x0B



#define PBM_PATH_LOOKUP_TABLE_MAX_FILE_LEN         0x04
#define PBM_LKUP_TABLE_USIM_PB_LEN                 3
#define PBM_LKUP_TABLE_TELECOM_LEN                 4
#define PBM_LKUP_TABLE_LEN                         4
#define PBM_NOT_IN_LKUP_TABLE                      255



#define PBM_PB_FILE_TYPE1               0x01
#define PBM_PB_FILE_TYPE2               0x02
#define PBM_PB_FILE_TYPE3               0x03

#define PBM_ATLEAST_TWO_CNT_ADN_IN_PBR  0x02
#define PBM_MAX_ADN_CNT_SUPPORTED       0x04

/*===========================================================================
                              STATIC DATA
===========================================================================*/
/* The following variables are used when we are initializing
 * the files from the SESSION.  We will
 * attempt to read all entries from each file in the sequence.
 * The initialize_index points to the index we are currently
 * working with.
 */
int pbm_initialize_index[MAX_UIM_APPS];

pbm_file_type pbm_initialize_list[PBM_SIZE_INITIALIZE_LIST] =
{
PBM_FILE_FDN,        /* Fixed Dialing Number list */
PBM_FILE_LND,        /* SIM/RUIM Last Number Dialed list */
PBM_FILE_SDN,        /* Service Dialling Numbers */
PBM_FILE_MSISDN,     /* MSISDN */
PBM_FILE_MBN,        /* CPHS MBN */
/* 5 */
PBM_FILE_GAS,        /* USIM Group Names */
PBM_FILE_AAS,        /* USIM Additional Alpha names */
PBM_FILE_MBDN,       /* MBDN */
PBM_FILE_OCI,        /* USIM Outgoing Call Information file.  */
PBM_FILE_ICI,        /* USIM Incoming Call Information file.  */
PBM_FILE_PSC,        /* USIM Synchronization counter */
/* 10 */
PBM_FILE_PUID,       /* USIM Previous unique id */

PBM_FILE_CC,         /* USIM Change counter */
PBM_FILE_PBC,        /* USIM phonebook control */
PBM_FILE_UID,        /* USIM Unique id */

PBM_FILE_EMAIL,      /* USIM email in phonebook one */
/* 15 */
PBM_FILE_SNE,        /* USIM second number in phonebook one */

PBM_FILE_GRP,        /* USIM group in phonebook one */
PBM_FILE_ANR,        /* USIM additional number in phonebook one */
PBM_FILE_ANRA,       /* USIM additional number in phonebook one */
PBM_FILE_ANRB,       /* USIM additional number in phonebook one */
/* 20 */
PBM_FILE_ANRC,       /* USIM additional number in phonebook one */

PBM_FILE_IAP,        /* USIM Index file */

PBM_FILE_EMAIL1,     /* USIM email in phonebook two */
PBM_FILE_SNE1,       /* USIM second number in phonebook two */
PBM_FILE_GRP1,       /* USIM group in phonebook two */
/* 25 */
PBM_FILE_ANR1,       /* USIM additional number in phonebook two */

PBM_FILE_ANRA1,      /* uSIM additional number in phonebook two */
PBM_FILE_ANRB1,      /* uSIM additional number in phonebook two */
PBM_FILE_ANRC1,      /* uSIM additional number in phonebook two */
PBM_FILE_IAP1,       /* USIM Index file in phonebook two */

/* 30 */
PBM_FILE_EMAIL2,     /* USIM email in phonebook three */
PBM_FILE_SNE2,       /* USIM second number in phonebook three */
PBM_FILE_GRP2,       /* USIM group in phonebook three */
PBM_FILE_ANR2,       /* USIM additional number in phonebook three */
PBM_FILE_ANRA2,      /* uSIM additional number in phonebook three */
/* 35 */
PBM_FILE_ANRB2,      /* uSIM additional number in phonebook three */
PBM_FILE_ANRC2,      /* uSIM additional number in phonebook three */
PBM_FILE_IAP2,       /* USIM Index file in phonebook three */

PBM_FILE_EMAIL3,     /* USIM email in phonebook four */
PBM_FILE_SNE3,       /* USIM second number in phonebook four */
/* 40 */
PBM_FILE_GRP3,       /* USIM group in phonebook four */

PBM_FILE_ANR3,       /* USIM additional number in phonebook four */
PBM_FILE_ANRA3,      /* uSIM additional number in phonebook four */
PBM_FILE_ANRB3,      /* uSIM additional number in phonebook four */
PBM_FILE_ANRC3,      /* uSIM additional number in phonebook four */
/* 45 */
PBM_FILE_IAP3,       /* USIM Index file in phonebook four */
PBM_FILE_ADN,        /* Abbreviated Dialing Number list (SIM/USIM Phonebook) */
};

/* SIM phonebook files (that are accessible to the user directly) */
pbm_file_type pbm_sim_pb_files[PBM_NUM_SIM_PB] = {
PBM_FILE_ADN, PBM_FILE_SDN,
PBM_FILE_FDN, PBM_FILE_LND,
PBM_FILE_OCI, PBM_FILE_ICI,
PBM_FILE_MSISDN,
PBM_FILE_GAS,
PBM_FILE_AAS,
PBM_FILE_MBDN,
PBM_FILE_MBN};

pbm_file_type pbm_sim_adn_fdn_files[PBM_NUM_SIM_ADN_FDN_FILES] = {
PBM_FILE_ADN, 
PBM_FILE_FDN
};


/* Has a one-to-one mapping with pbm_file_type */
pbm_uim_file_entry pbm_file_to_uim_file[PBM_FILE_MAX_REQUESTS] = {
{PBM_FILE_ADN,    {PBM_FILE_ADN,    PBM_FILE_ADN,    PBM_FILE_ADN,        PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ADN,          PBM_FILE_ADN,       PBM_FILE_ADN,          PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_FDN,    {PBM_FILE_FDN,    PBM_FILE_FDN,    PBM_FILE_FDN,          PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_FDN,          PBM_FILE_FDN,       PBM_FILE_FDN,           PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_SDN,    {PBM_FILE_SDN,    PBM_FILE_SDN,    PBM_FILE_SDN,        PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_SDN,          PBM_FILE_SDN,       PBM_FILE_SDN,          PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_LND,    {PBM_FILE_LND,    PBM_FILE_NONE,    PBM_FILE_NONE,      PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ADN,          PBM_FILE_NONE,       PBM_FILE_NONE,      PBM_FILE_NONE, PBM_FILE_NONE}},  /* USES EXT1 */
{PBM_FILE_MSISDN, {PBM_FILE_MSISDN, PBM_FILE_MSISDN,    PBM_FILE_MSISDN,      PBM_FILE_NONE, PBM_FILE_MSISDN},
                  {PBM_FILE_ADN,          PBM_FILE_OCI,       PBM_FILE_OCI,          PBM_FILE_NONE, PBM_FILE_ADN}}, /* USIM EXT5, SIM EXT1 */
{PBM_FILE_MBN,    {PBM_FILE_NONE,         PBM_FILE_MBN,    PBM_FILE_NONE,       PBM_FILE_MBN, PBM_FILE_NONE},
                  {PBM_FILE_NONE,          PBM_FILE_ADN,       PBM_FILE_ADN,             PBM_FILE_ADN, PBM_FILE_NONE}},
{PBM_FILE_EXT1,   {PBM_FILE_EXT1,   PBM_FILE_EXT1,    PBM_FILE_EXT2,           PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ADN,          PBM_FILE_ADN,       PBM_FILE_ADN,               PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_EXT2,   {PBM_FILE_EXT2,   PBM_FILE_EXT2,    PBM_FILE_NONE,           PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_FDN,          PBM_FILE_FDN,       PBM_FILE_FDN,                 PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_EXT3,   {PBM_FILE_EXT3,   PBM_FILE_EXT3,    PBM_FILE_EXT3,             PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_SDN,          PBM_FILE_SDN,       PBM_FILE_SDN,               PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_ECC,    {PBM_FILE_NONE,        PBM_FILE_ECC,    PBM_FILE_ECC,        PBM_FILE_ECC, PBM_FILE_ECC},
                  {PBM_FILE_NONE,          PBM_FILE_ECC,       PBM_FILE_ECC,              PBM_FILE_ECC, PBM_FILE_ECC}},
{PBM_FILE_GAS,    {PBM_FILE_GAS,    PBM_FILE_GAS,    PBM_FILE_GAS,                PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_GAS,          PBM_FILE_GAS,       PBM_FILE_GAS,                 PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_AAS,    {PBM_FILE_AAS,    PBM_FILE_AAS,    PBM_FILE_AAS,                PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_AAS,          PBM_FILE_AAS,       PBM_FILE_AAS,                 PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_MBDN,   {PBM_FILE_NONE,       PBM_FILE_MBDN,    PBM_FILE_MBDN,     PBM_FILE_MBDN, PBM_FILE_NONE},
                  {PBM_FILE_NONE,         PBM_FILE_MBDN,       PBM_FILE_MBDN,            PBM_FILE_MBDN, PBM_FILE_NONE}},
{PBM_FILE_OCI,    {PBM_FILE_NONE,   PBM_FILE_OCI,    PBM_FILE_OCI,                 PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_NONE,         PBM_FILE_OCI,       PBM_FILE_OCI,                 PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_ICI,    {PBM_FILE_NONE,   PBM_FILE_ICI,    PBM_FILE_ICI,                PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_NONE,         PBM_FILE_OCI,       PBM_FILE_OCI,            PBM_FILE_NONE, PBM_FILE_NONE}}, /* uses EXT5 with OCI */
{PBM_FILE_EXT5,   {PBM_FILE_NONE,   PBM_FILE_EXT5,    PBM_FILE_EXT5,           PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_NONE,         PBM_FILE_OCI,       PBM_FILE_OCI,             PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_EXT6,   {PBM_FILE_NONE,       PBM_FILE_EXT6,    PBM_FILE_NONE,  PBM_FILE_EXT6, PBM_FILE_NONE},
                  {PBM_FILE_NONE,         PBM_FILE_MBDN,       PBM_FILE_MBDN,        PBM_FILE_MBDN, PBM_FILE_NONE}},
{PBM_FILE_PSC,    {PBM_FILE_PSC,    PBM_FILE_PSC,    PBM_FILE_PSC,       PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ALL,          PBM_FILE_ALL,       PBM_FILE_ALL,           PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_PUID,   {PBM_FILE_PUID,   PBM_FILE_PUID,    PBM_FILE_PUID,   PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ALL,          PBM_FILE_ALL,       PBM_FILE_ALL,         PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_CC,     {PBM_FILE_CC,     PBM_FILE_CC,    PBM_FILE_CC,         PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ALL,          PBM_FILE_ALL,       PBM_FILE_ALL,          PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_EMAIL,  {PBM_FILE_EMAIL,  PBM_FILE_EMAIL,    PBM_FILE_EMAIL,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ADN,          PBM_FILE_ADN,       PBM_FILE_ADN,         PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_SNE,    {PBM_FILE_SNE,    PBM_FILE_SNE,    PBM_FILE_SNE,          PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ADN,          PBM_FILE_ADN,       PBM_FILE_ADN,         PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_GRP,    {PBM_FILE_GRP,    PBM_FILE_GRP,    PBM_FILE_GRP,        PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ADN,          PBM_FILE_ADN,       PBM_FILE_ADN,        PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_ANR,    {PBM_FILE_ANR,    PBM_FILE_ANR,    PBM_FILE_ANR,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ADN,          PBM_FILE_ADN,       PBM_FILE_ADN,   PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_ANRA,   {PBM_FILE_ANRA,   PBM_FILE_ANRA,    PBM_FILE_ANRA,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ADN,          PBM_FILE_ADN,       PBM_FILE_ADN,   PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_ANRB,   {PBM_FILE_ANRB,   PBM_FILE_ANRB,    PBM_FILE_ANRB, PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ADN,          PBM_FILE_ADN,       PBM_FILE_ADN,   PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_ANRC,   {PBM_FILE_ANRC,   PBM_FILE_ANRC,    PBM_FILE_ANRC, PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ADN,          PBM_FILE_ADN,       PBM_FILE_ADN,   PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_IAP,    {PBM_FILE_IAP,    PBM_FILE_IAP,    PBM_FILE_IAP, PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ALL,          PBM_FILE_ALL,       PBM_FILE_ALL, PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_UID,    {PBM_FILE_UID,    PBM_FILE_UID,    PBM_FILE_UID, PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ALL,          PBM_FILE_ALL,       PBM_FILE_ALL, PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_PBC,    {PBM_FILE_PBC,    PBM_FILE_PBC,    PBM_FILE_PBC,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ALL,          PBM_FILE_ALL,       PBM_FILE_ALL, PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_ADN1,   {PBM_FILE_ADN1,   PBM_FILE_ADN1,    PBM_FILE_ADN1,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ADN,          PBM_FILE_ADN,       PBM_FILE_ADN,     PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_EMAIL1, {PBM_FILE_EMAIL1, PBM_FILE_EMAIL1,    PBM_FILE_EMAIL1,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ADN,          PBM_FILE_ADN,       PBM_FILE_ADN,     PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_SNE1,   {PBM_FILE_SNE1,   PBM_FILE_SNE1,    PBM_FILE_SNE1,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ADN,          PBM_FILE_ADN,       PBM_FILE_ADN,     PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_GRP1,   {PBM_FILE_GRP1,   PBM_FILE_GRP1,    PBM_FILE_GRP1,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ADN,          PBM_FILE_ADN,       PBM_FILE_ADN,     PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_ANR1,   {PBM_FILE_ANR1,   PBM_FILE_ANR1,    PBM_FILE_ANR1,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ADN,          PBM_FILE_ADN,       PBM_FILE_ADN,     PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_ANRA1,  {PBM_FILE_ANRA1,  PBM_FILE_ANRA1,    PBM_FILE_ANRA1,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ADN,          PBM_FILE_ADN,       PBM_FILE_ADN,     PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_ANRB1,  {PBM_FILE_ANRB1,  PBM_FILE_ANRB1,    PBM_FILE_ANRB1,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ADN,          PBM_FILE_ADN,       PBM_FILE_ADN,     PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_ANRC1,  {PBM_FILE_ANRC1,  PBM_FILE_ANRC1,    PBM_FILE_ANRC1,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ADN,          PBM_FILE_ADN,       PBM_FILE_ADN,     PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_IAP1,   {PBM_FILE_IAP1,   PBM_FILE_IAP1,    PBM_FILE_IAP1,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ALL,          PBM_FILE_ALL,       PBM_FILE_ALL,     PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_UID1,   {PBM_FILE_UID1,   PBM_FILE_UID1,    PBM_FILE_UID1,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ALL,          PBM_FILE_ALL,       PBM_FILE_ALL,     PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_PBC1,   {PBM_FILE_PBC1,   PBM_FILE_PBC1,    PBM_FILE_PBC1,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ALL,          PBM_FILE_ALL,       PBM_FILE_ALL,     PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_ADN2,   {PBM_FILE_ADN2,   PBM_FILE_ADN2,    PBM_FILE_ADN2,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ADN,          PBM_FILE_ADN,       PBM_FILE_ADN,     PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_EMAIL2, {PBM_FILE_EMAIL2, PBM_FILE_EMAIL2,    PBM_FILE_EMAIL2,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ADN,          PBM_FILE_ADN,       PBM_FILE_ADN,     PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_SNE2,   {PBM_FILE_SNE2,   PBM_FILE_SNE2,    PBM_FILE_SNE2,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ADN,          PBM_FILE_ADN,       PBM_FILE_ADN,     PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_GRP2,   {PBM_FILE_GRP2,   PBM_FILE_GRP2,    PBM_FILE_GRP2,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ADN,          PBM_FILE_ADN,       PBM_FILE_ADN,     PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_ANR2,   {PBM_FILE_ANR2,   PBM_FILE_ANR2,    PBM_FILE_ANR2,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ADN,          PBM_FILE_ADN,       PBM_FILE_ADN,     PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_ANRA2,  {PBM_FILE_ANRA2,  PBM_FILE_ANRA2,    PBM_FILE_ANRA2,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ADN,          PBM_FILE_ADN,       PBM_FILE_ADN,     PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_ANRB2,  {PBM_FILE_ANRB2,  PBM_FILE_ANRB2,    PBM_FILE_ANRB2,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ADN,          PBM_FILE_ADN,       PBM_FILE_ADN,     PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_ANRC2,  {PBM_FILE_ANRC2,  PBM_FILE_ANRC2,    PBM_FILE_ANRC2,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ADN,          PBM_FILE_ADN,       PBM_FILE_ADN,     PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_IAP2,   {PBM_FILE_IAP2,   PBM_FILE_IAP2,    PBM_FILE_IAP2,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ALL,          PBM_FILE_ALL,       PBM_FILE_ALL,     PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_UID2,   {PBM_FILE_UID2,   PBM_FILE_UID2,    PBM_FILE_UID2,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ALL,          PBM_FILE_ALL,       PBM_FILE_ALL,     PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_PBC2,   {PBM_FILE_PBC2,   PBM_FILE_PBC2,    PBM_FILE_PBC2,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ALL,          PBM_FILE_ALL,       PBM_FILE_ALL,     PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_ADN3,   {PBM_FILE_ADN3,   PBM_FILE_ADN3,    PBM_FILE_ADN3,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ADN,          PBM_FILE_ADN,       PBM_FILE_ADN,     PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_EMAIL3, {PBM_FILE_EMAIL3, PBM_FILE_EMAIL3,    PBM_FILE_EMAIL3,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ADN,          PBM_FILE_ADN,       PBM_FILE_ADN,     PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_SNE3,   {PBM_FILE_SNE3,   PBM_FILE_SNE3,    PBM_FILE_SNE3,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ADN,          PBM_FILE_ADN,       PBM_FILE_ADN,     PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_GRP3,   {PBM_FILE_GRP3,   PBM_FILE_GRP3,    PBM_FILE_GRP3,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ADN,          PBM_FILE_ADN,       PBM_FILE_ADN,     PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_ANR3,   {PBM_FILE_ANR3,   PBM_FILE_ANR3,    PBM_FILE_ANR3,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ADN,          PBM_FILE_ADN,       PBM_FILE_ADN,     PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_ANRA3,  {PBM_FILE_ANRA3,  PBM_FILE_ANRA3,    PBM_FILE_ANRA3,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ADN,          PBM_FILE_ADN,       PBM_FILE_ADN,     PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_ANRB3,  {PBM_FILE_ANRB3,  PBM_FILE_ANRB3,    PBM_FILE_ANRB3,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ADN,          PBM_FILE_ADN,       PBM_FILE_ADN,     PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_ANRC3,  {PBM_FILE_ANRC3,  PBM_FILE_ANRC3,    PBM_FILE_ANRC3,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ADN,          PBM_FILE_ADN,       PBM_FILE_ADN,     PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_IAP3,   {PBM_FILE_IAP3,   PBM_FILE_IAP3,    PBM_FILE_IAP3,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ALL,          PBM_FILE_ALL,       PBM_FILE_ALL,     PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_UID3,   {PBM_FILE_UID3,   PBM_FILE_UID3,    PBM_FILE_UID3,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ALL,          PBM_FILE_ALL,       PBM_FILE_ALL,     PBM_FILE_NONE, PBM_FILE_NONE}},
{PBM_FILE_PBC3,   {PBM_FILE_PBC3,   PBM_FILE_PBC3,    PBM_FILE_PBC3,  PBM_FILE_NONE, PBM_FILE_NONE},
                  {PBM_FILE_ALL,          PBM_FILE_ALL,       PBM_FILE_ALL,     PBM_FILE_NONE, PBM_FILE_NONE}},
};


extern uint32 ecc_has_entries;

/*
 * The initialization chain for each SESSION is independent of each other and
 * Four of them could be going on at the same time.
 *
 * An initialization chain starts from pbm_*_fileinfo_internal() and ends with
 * pbm_app_init_completed().
 *
 * It includes getting file attribute for each SESSION file
 * (pbm_make_fileinfo_request()/pbm_fileinfo_cb()), allocating memory
 * depending on the number of records read from file attribute
 * (pbm_fileinfo_complete()) and for each file (pbm_init_entries()),
 * reading all data in the file (pbm_sim_read()/pbm_sim_read_cb()).
 *
 * pbm_session_initializing is set to TRUE when the above chain is in progress.
 *
 * pbm_init_fileinfo_internal() is used to initialize all SIM files.
 * pbm_start_fileinfo_internal() is used to start initialization based on
 * the files specified in pbm_fileinfo_requests.
 *
 * When either of them is called, if another initialization is going on,
 * the current initialization is deferred until the next callback where
 * pbm_restart_fileinfo_internal() picks it up.
 *
 * pbm_session_restart is used to notify the callback to start again.
 *
 * The callbacks also check whether an initialization should be stopped
 * (say SIM removed) by checking pbm_session_initializing.
 */

/* Initialization variables */
uint32 pbm_session_activated = 0;      /* True if cache building started for
                                          session */
uint32 pbm_session_subs_ready = 0;   /* Initialization is going on */
uint32 pbm_session_initializing = 0;   /* Initialization is going on */
uint32 pbm_session_restart = 0;        /* Somebody wants to restart */
uint32 pbm_session_lpm_status = 0;      /* True if card error has been recieved after the LPM mode was turned On */

/* SESSION status */
uint32 pbm_session_fdn_enabled = 0;     /* FDN enabled */

pbm_static_path_lookup_table_element_T pbm_static_path_lookup_table[PBM_MAX_STATIC_FILES] =
{
  {PBM_FILE_ADN,{{0x3F00,0x7F10,0x6F3A,0xFFFF},{0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000}},{3,3,3,3,3}},
  {PBM_FILE_FDN,{{0x3F00,0x7F10, 0x6F3B,0xFFFF},{0x3F00,0x7FFF, 0x6F3B,0xFFFF},{0x3F00,0x7FFF, 0x6F3B,0xFFFF},{0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000}},{3,3,3,3,3}},
  {PBM_FILE_LND,{{0x3F00,0x7F10, 0x6F44,0xFFFF},{0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000}},{3,3,3,3,3}},
  {PBM_FILE_SDN,{{0x3F00,0x7F10, 0x6F49,0xFFFF},{0x3F00,0x7FFF, 0x6F49,0xFFFF},{0x3F00,0x7FFF, 0x6F79,0xFFFF},{0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000}},{3,3,3,3,3}},
  {PBM_FILE_MSISDN,{{0x3F00,0x7F10, 0x6F40,0xFFFF},{0x3F00,0x7FFF, 0x6F40,0xFFFF},{0x3F00,0x7FFF, 0x6F44,0xFFFF},{0x0000,0x0000,0x0000,0x0000},{0x3F00,0x7F25, 0x6F44,0xFFFF}},{3,3,3,3,3}},
  {PBM_FILE_MBN,{{0x0000,0x0000,0x0000,0x0000},{0x3F00,0x7FFF, 0x6F17, 0xFFFF},{0x0000,0x0000,0x0000,0x0000},{0x3F00,0x7F20, 0x6F17, 0xFFFF},{0x0000,0x0000,0x0000,0x0000}},{3,3,3,3,3}},
  {PBM_FILE_ICI,{{0x0000,0x0000,0x0000,0x0000},{0x3F00,0x7FFF, 0x6F80, 0xFFFF},{0x3F00,0x7FFF, 0x6F80, 0xFFFF},{0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000}},{3,3,3,3,3}},
  {PBM_FILE_OCI,{{0x0000,0x0000,0x0000,0x0000},{0x3F00,0x7FFF, 0x6F81, 0xFFFF},{0x3F00,0x7FFF, 0x6F81, 0xFFFF},{0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000}},{3,3,3,3,3}},
  {PBM_FILE_MBDN,{{0x0000,0x0000,0x0000,0x0000},{0x3F00,0x7FFF, 0x6FC7, 0xFFFF},{0x0000,0x0000,0x0000,0x0000},{0x3F00,0x7F20, 0x6FC7, 0xFFFF},{0x0000,0x0000,0x0000,0x0000}},{3,3,3,3,3}},
  {PBM_FILE_EXT1,{{0x3F00,0x7F10, 0x6F4A,0xFFFF},{0x3F00,0x7FFF, 0x6F4A, 0xFFFF},{0x3F00,0x7FFF, 0x6F4A, 0xFFFF},{0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000}},{3,3,3,3,3}},
  {PBM_FILE_EXT2,{{0x3F00,0x7F10, 0x6F4B,0xFFFF},{0x3F00,0x7FFF, 0x6F4B, 0xFFFF},{0x3F00,0x7FFF, 0x6F4B, 0xFFFF},{0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000}},{3,3,3,3,3}},
  {PBM_FILE_EXT3,{{0x3F00,0x7F10, 0x6F4C,0xFFFF},{0x3F00,0x7FFF, 0x6F4C, 0xFFFF},{0x3F00,0x7FFF, 0x6F4C, 0xFFFF},{0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000}},{3,3,3,3,3}},
  {PBM_FILE_EXT5,{{0x0000,0x0000,0x0000,0x0000},{0x3F00,0x7FFF, 0x6F4E, 0xFFFF},{0x3F00,0x7FFF, 0x6F7E, 0xFFFF},{0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000}},{3,3,3,3,3}},
  {PBM_FILE_EXT6,{{0x0000,0x0000,0x0000,0x0000},{0x3F00,0x7FFF, 0x6FC8, 0xFFFF},{0x0000,0x0000,0x0000,0x0000},{0x3F00,0x7F20, 0x6FC8, 0xFFFF},{0x0000,0x0000,0x0000,0x0000}},{3,3,3,3,3}},
  {PBM_FILE_PSC,{{0x3F00,0x7F10, 0x5f3A, 0x4f22},{0x3F00,0x7FFF, 0x5f3A, 0x4f22},{0x3F00,0x7FFF, 0x5f3A, 0x4f22},{0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000}},{4,4,4,4,4}},
  {PBM_FILE_CC,{{0x3F00,0x7F10, 0x5f3A, 0x4f23},{0x3F00,0x7FFF, 0x5f3A, 0x4f23},{0x3F00,0x7FFF, 0x5f3A, 0x4f23},{0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000}},{4,4,4,4,4}},
  {PBM_FILE_PUID,{{0x3F00,0x7F10, 0x5f3A, 0x4f24},{0x3F00,0x7FFF, 0x5f3A, 0x4f24},{0x3F00,0x7FFF, 0x5f3A, 0x4f24},{0x0000,0x0000,0x0000,0x0000},{0x0000,0x0000,0x0000,0x0000}},{4,4,4,4,4}},
};



pbm_usim_path_lookup_table_element_T pbm_usim_path_lookup_table[PBM_SESSION_MAX][PBM_NUM_FILES_IN_LOOKUP_TABLE] =
{
{
    /* 0 */
    { PBM_FILE_ADN,  0,  0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_ADN1, 0,  0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_ANR,  0,  0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_ANRA, 0,  0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_ANRB, 0,  0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5F3A,0xFFFF,0xFFFF}},
    /* 5 */
    { PBM_FILE_ANR1, 0,  0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5F3A,0xFFFF,0xFFFF}},
    { PBM_FILE_ANRA1, 0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5F3A,0xFFFF,0xFFFF}},
    { PBM_FILE_ANRB1, 0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5F3A,0xFFFF,0xFFFF}},
    { PBM_FILE_ANRC,  0, 0, 0, FALSE, PBM_LKUP_TABLE_TELECOM_LEN,  {0x7F10,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_ANRC1, 0, 0, 0, FALSE, PBM_LKUP_TABLE_TELECOM_LEN,  {0x7F10,0x5f3A,0xFFFF,0xFFFF}},
    /* 10 */
    { PBM_FILE_EMAIL, 0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5F3A,0xFFFF,0xFFFF}},
    { PBM_FILE_EMAIL1,0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5F3A,0xFFFF,0xFFFF}},
    { PBM_FILE_EXT1,  0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_GAS,   0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_GRP,   0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    /* 15 */
    { PBM_FILE_GRP1,  0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_IAP,   0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
     { PBM_FILE_IAP1, 0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0x7F10,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_PBC,   0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_PBC1,  0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    /* 20 */
    { PBM_FILE_SNE,   0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_SNE1,  0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_UID,   0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_UID1,  0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_EXT2, 0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0x7F10,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_AAS, 0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0x7F10,0x5f3A,0xFFFF,0xFFFF}},
    /* 25 */
    { PBM_FILE_ADN2,     0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_IAP2,     0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_PBC2,     0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_GRP2,     0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_SNE2,     0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    /* 30 */
    { PBM_FILE_UID2,     0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_EMAIL2,   0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_ANR2,     0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_ANRA2,    0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_ANRB2,    0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    /* 35 */
    { PBM_FILE_ANRC2,    0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_ADN3,     0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_IAP3,     0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_PBC3,     0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_GRP3,     0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    /* 40 */
    { PBM_FILE_SNE3,     0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_UID3,     0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_EMAIL3,   0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_ANR3,     0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_ANRA3,    0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    /* 45 */
    { PBM_FILE_ANRB3,    0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_ANRC3,    0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}}
}
};

pbm_usim_path_lookup_table_element_T pbm_usim_path_lookup_table_default[1][PBM_NUM_FILES_IN_LOOKUP_TABLE] =
{
{
    /* 0 */
    { PBM_FILE_ADN,  0,  0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_ADN1, 0,  0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_ANR,  0,  0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_ANRA, 0,  0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_ANRB, 0,  0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5F3A,0xFFFF,0xFFFF}},
    /* 5 */
    { PBM_FILE_ANR1, 0,  0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5F3A,0xFFFF,0xFFFF}},
    { PBM_FILE_ANRA1, 0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5F3A,0xFFFF,0xFFFF}},
    { PBM_FILE_ANRB1, 0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5F3A,0xFFFF,0xFFFF}},
    { PBM_FILE_ANRC,  0, 0, 0, FALSE, PBM_LKUP_TABLE_TELECOM_LEN,  {0x7F10,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_ANRC1, 0, 0, 0, FALSE, PBM_LKUP_TABLE_TELECOM_LEN,  {0x7F10,0x5f3A,0xFFFF,0xFFFF}},
    /* 10 */
    { PBM_FILE_EMAIL, 0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5F3A,0xFFFF,0xFFFF}},
    { PBM_FILE_EMAIL1,0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5F3A,0xFFFF,0xFFFF}},
    { PBM_FILE_EXT1,  0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_GAS,   0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_GRP,   0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    /* 15 */
    { PBM_FILE_GRP1,  0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_IAP,   0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
     { PBM_FILE_IAP1, 0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0x7F10,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_PBC,   0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_PBC1,  0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    /* 20 */
    { PBM_FILE_SNE,   0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_SNE1,  0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_UID,   0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_UID1,  0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_EXT2,  0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0x7F10,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_AAS,   0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,    {0x7F10,0x5f3A,0xFFFF,0xFFFF}},
    /* 25 */
    { PBM_FILE_ADN2,     0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_IAP2,     0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_PBC2,     0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_GRP2,     0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_SNE2,     0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    /* 30 */
    { PBM_FILE_UID2,     0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_EMAIL2,   0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_ANR2,     0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_ANRA2,    0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_ANRB2,    0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    /* 35 */
    { PBM_FILE_ANRC2,    0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_ADN3,     0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_IAP3,     0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_PBC3,     0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_GRP3,     0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    /* 40 */
    { PBM_FILE_SNE3,     0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_UID3,     0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_EMAIL3,   0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_ANR3,     0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_ANRA3,    0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    /* 45 */
    { PBM_FILE_ANRB3,    0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}},
    { PBM_FILE_ANRC3,    0, 0, 0, FALSE,  PBM_LKUP_TABLE_TELECOM_LEN,  {0xFFFF,0x5f3A,0xFFFF,0xFFFF}}
}
};

typedef struct {
  pbm_file_type pbm_base_pb_filename;
  pbm_file_type pbm_pb_filename[PBM_MAX_NUMBER_OF_ADN_SUPPORTED-1];
}pbm_pb_file_enum_lookup_table_element_T;

pbm_pb_file_enum_lookup_table_element_T
  pbm_pb_file_enum_lookup_table[PBM_NUM_FILES_IN_PB_FILE_ENUM_LOOKUP_TABLE] =
{
    /* -- */
    { PBM_FILE_ADN,   {PBM_FILE_ADN1,    PBM_FILE_ADN2,    PBM_FILE_ADN3}},
    { PBM_FILE_IAP,   {PBM_FILE_IAP1,    PBM_FILE_IAP2,    PBM_FILE_IAP3}},
    { PBM_FILE_SNE,   {PBM_FILE_SNE1,    PBM_FILE_SNE2,    PBM_FILE_SNE3}},
    { PBM_FILE_ANR,   {PBM_FILE_ANR1,   PBM_FILE_ANR2,    PBM_FILE_ANR3}},
    { PBM_FILE_ANRA,  {PBM_FILE_ANRA1,   PBM_FILE_ANRA2,  PBM_FILE_ANRA3}},
    /* 5 */
    { PBM_FILE_ANRB,  {PBM_FILE_ANRB1,   PBM_FILE_ANRB2,   PBM_FILE_ANRB3}},
    { PBM_FILE_ANRC,  {PBM_FILE_ANRC1,   PBM_FILE_ANRC2,   PBM_FILE_ANRC3}},
    { PBM_FILE_PBC,   {PBM_FILE_PBC1,    PBM_FILE_PBC2,    PBM_FILE_PBC3}},
    { PBM_FILE_GRP,   {PBM_FILE_GRP1,    PBM_FILE_GRP2,    PBM_FILE_GRP3}},
    { PBM_FILE_UID,   {PBM_FILE_UID1,    PBM_FILE_UID2,    PBM_FILE_UID3}},
    /* 10 */
    { PBM_FILE_EMAIL,    {PBM_FILE_EMAIL1,     PBM_FILE_EMAIL2,     PBM_FILE_EMAIL3}}
};

pbm_fileinfo_request_t pbm_fileinfo_requests[ (int)PBM_FILE_MAX_REQUESTS *
                                              (int)MAX_UIM_APPS];

static mmgsdi_get_file_attr_cnf_type              pbr_file_attr[MAX_UIM_APPS];

static pbm_return_type pbm_util_map_usim_pbr_table_to_response(
  pbm_get_pbr_table_cnf_T * rsp_p,
  pbm_session_enum_type pbm_sess_type);

static void pbm_process_pbr(pbm_get_pbr_table_cnf_T * rsp_p,
  pbm_session_enum_type pbm_sess_type);

static pbm_file_type pbm_util_get_phonebook_filename_from_tag (
  byte pb_global_flag,
  byte ef_tag,
  int  *anr_count_ptr);

static pbm_file_type pbm_util_assign_anr_to_adn (
  int *anr_count_ptr,
  int pb_global_flag);

static void pbm_umts_populate_fid_lookup_table(
  byte             * data_ptr,
  byte               pb_global_flag,
  mmgsdi_len_type    rec_len,
  byte             * ad8_cnt_ptr,
  pbm_session_enum_type pbm_sess_type);



static pbm_return_type pbm_umts_find_pb_file_enum(pbm_file_type root_file_name,
											 int				 level,
											 pbm_file_type *new_file_name_ptr);

static pbm_return_type pbm_usim_read_pbr(pbm_session_enum_type session_type);

static void pbm_util_dump_path_lookup_table ( pbm_session_enum_type pbm_sess_type );

static pbm_return_type pbm_util_search_path_lookup_table (
  byte                   *out_filenum,
  pbm_file_type     file_id,
  pbm_session_enum_type pbm_sess_type);

static boolean pbm_util_write_phonebook_path_lookup_table (
    pbm_file_type 	file_id,
    byte		  file_type,
    unsigned int  ef_id,
    uint32		  ef_sid,
    byte		  global,
    uint32		  iap_index_order,
    byte		  tag,
    pbm_session_enum_type pbm_sess_type);

static pbm_return_type pbm_util_copy_path_lookup_table (
    pbm_file_type file_id,
    mmgsdi_path_type		*path_ptr,
    pbm_session_enum_type pbm_sess_type);

static pbm_return_type pbm_util_is_ef_init_path_lookup_table (
      boolean *out_is_init,
      pbm_file_type	  file_id,
      pbm_session_enum_type pbm_sess_type);

static pbm_return_type pbm_seek_unused_records(pbm_session_enum_type session_type,
                                               pbm_file_type file_id);

/*===========================================================================
FUNCTION PBM_FDN_EXT_READ_CB

DESCRIPTION
  Read the PBR structure from GSDI.

PARAMETERS
   None.

SIDE EFFECTS
===========================================================================*/
void pbm_fdn_ext_read_cb(
  mmgsdi_return_enum_type        status,
  mmgsdi_cnf_enum_type           data_type,
  const mmgsdi_cnf_type         *data_ptr)
{
  byte                              *bcd_number       = NULL;
  uint32                             client_ref       = PBM_ASYNC_BUFFERS;
  boolean                            ret              = FALSE;
  pbm_session_enum_type              pbm_sess_type    = PBM_SESSION_MAX;
  pbm_addr_cache_s_type             *prev_rec         = NULL;
  pbm_addr_cache_s_type             *curr_rec         = NULL;
  pbm_notify_event_e_type            event            = PBM_EVENT_REC_ADD;
  pbm_session_event_data_u_type      evt_data         = {0};
  PB_CACHE                          *cache            = NULL;
  pbm_phonebook_type                 pb_id            = {PBM_GPB};

  PBM_CHECK_PTR_RET(data_ptr, VOID);
  pbm_sess_type = pbm_mmgsdi_session_to_pbm_session(data_ptr->response_header.session_id);
  client_ref    = data_ptr->response_header.client_data;
  if (status == MMGSDI_SUCCESS)
  {
    bcd_number = pbm_buffers[client_ref].record.data.num_text.bcd_number;

    pbm_mmgsdi_process_ext_data(data_ptr->read_cnf.read_data.data_ptr, bcd_number);
    pb_id = pbm_file_id_to_pb_id(pbm_sess_type, PBM_FILE_FDN);

    rex_enter_crit_sect(&pbm_crit_sect);
    cache    = pbm_file_id_to_cache(pbm_sess_type, PBM_FILE_FDN);
    prev_rec = cache->pb_cache_array[pbm_buffers[client_ref].record.index];
    ret      = pbm_cache_record_add(pb_id,
                                    &pbm_buffers[client_ref].record.data.num_text,
                                    pbm_buffers[client_ref].record.index);
    if (ret)
    {
      curr_rec = cache->pb_cache_array[pbm_buffers[client_ref].record.index];
    }
    rex_leave_crit_sect(&pbm_crit_sect);

    if(ret)
    {
      evt_data.rec_id = pbm_session_location_to_record_id(pb_id,pbm_buffers[client_ref].record.index);
      if(!prev_rec && curr_rec)
      {
        event = PBM_EVENT_REC_ADD;
      }
      else if(prev_rec && curr_rec)
      {
        event = PBM_EVENT_REC_UPDATE;
      }
      else if(prev_rec && !curr_rec)
      {
        event = PBM_EVENT_REC_DELETE;
      }
      evt_data.rec_id = pbm_session_location_to_record_id(pb_id,pbm_buffers[client_ref].record.index);
      pbm_add_notify(event, evt_data);
    }
    else
    {
      UIM_MSG_ERR_2("Unable to add FDN rec to cache, index 0x%x session 0x%x",
                     pbm_buffers[client_ref].record.index, pbm_sess_type);
    }
  }
  pbm_free_record((int) client_ref);
  return;
}/* pbm_fdn_ext_read_cb */


/*===========================================================================
FUNCTION pbm_usim_read_pbr

DESCRIPTION
  Read the PBR structure from GSDI.

PARAMETERS
   None.

SIDE EFFECTS
===========================================================================*/
static pbm_return_type pbm_usim_read_pbr(pbm_session_enum_type session_type)
{
   mmgsdi_file_enum_type pbr_file = MMGSDI_NO_FILE_ENUM;
   pbm_return_type l_ret = PBM_ERROR;

   /* initialize the pbm usim path look up table by default for the session_type */
   memscpy(pbm_usim_path_lookup_table[session_type],
           sizeof(pbm_usim_path_lookup_table[0][0])*PBM_NUM_FILES_IN_LOOKUP_TABLE,
           pbm_usim_path_lookup_table_default[0],
           sizeof(pbm_usim_path_lookup_table[0][0])*PBM_NUM_FILES_IN_LOOKUP_TABLE);
   do
   {
      if(USE_USIM_LOCAL_SUPPORT(session_type))
      {
         pbr_file = MMGSDI_USIM_PBR;
      }
      else if(USE_CSIM_LOCAL_SUPPORT(session_type))
      {
         pbr_file = MMGSDI_CSIM_PBR;
      }
      else if(pbm_session_type_to_pb_category(session_type) == PBM_GPB)
      //global PBR case
      {
         //no need to check for the UST availability since this is a GlobalPB that belongs to telecom
         pbr_file = MMGSDI_TELECOM_PBR;
      }

      if(pbr_file != MMGSDI_NO_FILE_ENUM)
      {
         l_ret = pbm_mmgsdi_get_file_attributes_pbr(session_type, /* Session id */
                                                pbr_file, /* mmgsdi file id */
                                                (uint32)pbr_file, /* Client Ref */
                                                pbm_mmgsdi_read_pbr_attr_cb); /* Async CB. */
      }
   }while(0);
   return l_ret;
}

/*===========================================================================
FUNCTION pbm_usim_read_pbr_cb

DESCRIPTION
  Initiliaze linkage type and add the available USIM files to init list.

PARAMETERS
  None.

SIDE EFFECTS
===========================================================================*/
void pbm_usim_read_pbr_attr_cb(mmgsdi_return_enum_type status,
                                     mmgsdi_cnf_enum_type	 data_type,
                                     const mmgsdi_cnf_type	*data_ptr)

{

    pbm_session_enum_type pbm_sess_type;
    mmgsdi_access_type file_struct;
    mmgsdi_return_enum_type ret = MMGSDI_SUCCESS;

    pbm_sess_type = pbm_mmgsdi_session_to_pbm_session(data_ptr->response_header.session_id);

    // Restart initialization if required */
    if (pbm_session_restart & PBM_SESSIONID_TO_SESSION_MASK(pbm_sess_type))
    {
       UIM_MSG_HIGH_1("PBM restarting init of session_type %d", pbm_sess_type);
       pbm_restart_fileinfo_internal(pbm_sess_type);
       return;
    }

    // Stop initialization if required
    if (!(pbm_session_initializing & PBM_SESSIONID_TO_SESSION_MASK(pbm_sess_type)))
    {
       UIM_MSG_HIGH_1("PBM stopping init of SIM %d", pbm_sess_type);
       return;
    }


    if (status == MMGSDI_SUCCESS)
    {

         memscpy(&pbr_file_attr[pbm_sess_type], sizeof(pbr_file_attr[pbm_sess_type]),
                 &(data_ptr->get_file_attr_cnf), sizeof(pbr_file_attr[pbm_sess_type]));
         if(pbr_file_attr[pbm_sess_type].file_attrib.file_info.linear_fixed_file.num_of_rec > PBM_MAX_ADN_CNT_SUPPORTED)
         {
           pbr_file_attr[pbm_sess_type].file_attrib.file_info.linear_fixed_file.num_of_rec = PBM_MAX_ADN_CNT_SUPPORTED;
         }
         file_struct.access_method = MMGSDI_EF_ENUM_ACCESS;
         file_struct.file.file_enum = (mmgsdi_file_enum_type)data_ptr->response_header.client_data;
         ret = mmgsdi_session_read_record (data_ptr->response_header.session_id,
                                    file_struct,
                                    1,
                                    data_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.rec_len,
                                    pbm_mmgsdi_read_pbr_rec_cb,
                                    data_ptr->response_header.client_data); //this arguement is for client_ref
         if ( ret != MMGSDI_SUCCESS )
         {
            UIM_MSG_HIGH_1(" PBM: mmgsdi_session_read_record returned error  %d",ret);
         }
    }
    else
    {
         if( pbm_session_type_to_pb_category(pbm_sess_type) == PBM_GPB )
         {
            pbm_session_info[pbm_sess_type].in_use_adn_pb = PBM_PB_GLOBAL_PBR_NOT_PRESENT;
         }
         else
         {
            pbm_session_info[pbm_sess_type].in_use_adn_pb = PBM_PB_LOCAL_PBR_NOT_PRESENT;
            //global phonebook not present. Start reading the default ADN , SDN , MSISDN etc ..
         }
         pbm_make_fileinfo_request(pbm_sess_type, 0);
    }
}

/*===========================================================================
FUNCTION pbm_usim_read_pbr_rec_cb

DESCRIPTION
  Initiliaze linkage type and add the available USIM files to init list.

PARAMETERS
  None.

SIDE EFFECTS
===========================================================================*/
void pbm_usim_read_pbr_rec_cb(mmgsdi_return_enum_type status,
                                        mmgsdi_cnf_enum_type data_type,
                                        const mmgsdi_cnf_type *data_ptr)

{

    pbm_session_enum_type pbm_sess_type;
    mmgsdi_access_type file_struct;
    uint8 ad8_cnt = 0;
    mmgsdi_return_enum_type mmgsdi_ret = MMGSDI_SUCCESS;
    pbm_return_type pbm_ret = PBM_SUCCESS;
    int32 tbl_index = 0;
    int32 rsp_index = 0;

    pbm_sess_type = pbm_mmgsdi_session_to_pbm_session(data_ptr->response_header.session_id);

     // Restart initialization if required */
    if (pbm_session_restart & PBM_SESSIONID_TO_SESSION_MASK(pbm_sess_type))
    {
         UIM_MSG_HIGH_1("PBM restarting init of session_type %d", pbm_sess_type);
         pbm_restart_fileinfo_internal(pbm_sess_type);
         return;
    }
	// Stop initialization if required
    if (!(pbm_session_initializing & PBM_SESSIONID_TO_SESSION_MASK(pbm_sess_type)))
    {
	 UIM_MSG_HIGH_1("PBM stopping init of SIM %d", pbm_sess_type);
	 return;
    }

    do
    {

        if (status != MMGSDI_SUCCESS)
        {
            switch (status)
            {
                case MMGSDI_NOT_FOUND:
                    UIM_MSG_HIGH_2("GSDI error MMGSDI_NOT_FOUND sim 0x%x MMGSDI file %d sim read",
                         pbm_sess_type, data_ptr->response_header.client_data);
                break;

                case MMGSDI_ACCESS_DENIED:
                    UIM_MSG_HIGH_2("GSDI error MMGSDI_ACCESS_DENIED sim 0x%x MMGSDI file %d sim read",
                         pbm_sess_type, data_ptr->response_header.client_data);
                break;

                default:
                UIM_MSG_HIGH_3("GSDI error code 0x%x sim 0x%x MMGSDI file %d sim read",
                             status, pbm_sess_type, data_ptr->response_header.client_data);
                break;
            }

            /* PBM shouldn't parse the CNF incase of failure from MMGSDI as  
             * MMGSDI won't fill accessed_rec_num,etc.. details and 
             * hence removeing the check 
             */
            if(pbm_session_type_to_pb_category(pbm_sess_type) == PBM_GPB)
            {
              pbm_session_info[pbm_sess_type].in_use_adn_pb = PBM_PB_GLOBAL_PBR_NOT_PRESENT;
            }
            else
            {
              pbm_session_info[pbm_sess_type].in_use_adn_pb = PBM_PB_LOCAL_PBR_NOT_PRESENT;
            }

            /* continue with reading other files */
            pbm_make_fileinfo_request(pbm_sess_type, 0);
            break; /* break the dowhile */
        }



        pbm_umts_populate_fid_lookup_table(data_ptr->read_cnf.read_data.data_ptr,
                                           (data_ptr->response_header.client_data == (uint32) MMGSDI_TELECOM_PBR)?1:0,
                                           //above arg indicates whether the file being processed is local or global
                                           data_ptr->read_cnf.read_data.data_len,
                                           &ad8_cnt,
                                           pbm_sess_type);
        if(data_ptr->read_cnf.accessed_rec_num <
                       pbr_file_attr[pbm_sess_type].file_attrib.file_info.linear_fixed_file.num_of_rec)
        {

            file_struct.file.file_enum = (mmgsdi_file_enum_type) data_ptr->response_header.client_data;
            file_struct.access_method = MMGSDI_EF_ENUM_ACCESS;
            mmgsdi_ret = mmgsdi_session_read_record (data_ptr->response_header.session_id,
                                file_struct,
                                data_ptr->read_cnf.accessed_rec_num + 1, //next record
                                pbr_file_attr[pbm_sess_type].file_attrib.file_info.linear_fixed_file.rec_len,
                                pbm_mmgsdi_read_pbr_rec_cb,
                                data_ptr->response_header.client_data); //this arguement is for client_ref
            if  ( mmgsdi_ret != MMGSDI_SUCCESS )
            {
               UIM_MSG_HIGH_1(" PBM: mmgsdi_session_read_record returned error %d ",mmgsdi_ret);
            }

        }
        else
        {
            pbm_get_pbr_table_cnf_T pbr_table = {0};
            //we have constructed the path lookup tables for both local and global now we have to extract the PBR info
            int32 index = 0;

            /* -----------------------------------------------------------------------
                         Iterrate through the entire USIM Path Lookup Table.  If a File Type is
                         not unknown, then it is a valid entry, and needs to be accounted for
             ---------------------------------------------------------------------*/
            while ( index < PBM_NUM_FILES_IN_LOOKUP_TABLE )
            {
                if ( pbm_usim_path_lookup_table[pbm_sess_type][index].file_type != (int)PBM_MAPPING_UNKNOWN )
                {
                    pbr_table.num_rows++;
                }
                index++;
            }
            pbm_ret = pbm_util_map_usim_pbr_table_to_response(&pbr_table,pbm_sess_type);
            if ( pbm_ret != PBM_SUCCESS )
            {
                UIM_MSG_HIGH_1(" PBM: pbm_util_map_usim_pbr_table_to_response returned error  %d",pbm_ret);
            }

            pbm_process_pbr(&pbr_table,pbm_sess_type);
            // Register for FCN changes
            //Though we have already registered in pbm_session_phonebook_init
            // we need to register again since we dont know %=5F3A paths at that time
            //However MMGSDI will take care of removing the duplicates in files list
            pbm_mmgsdi_register_fcn(pbm_sess_type);

            while ( tbl_index < PBM_NUM_FILES_IN_LOOKUP_TABLE &&
                    rsp_index <  pbr_table.num_rows)
            {
                if (pbm_usim_path_lookup_table[pbm_sess_type][tbl_index].file_type != (int)PBM_MAPPING_UNKNOWN &&
                    pbm_usim_path_lookup_table[pbm_sess_type][tbl_index].init      == TRUE )
                {
                    PBM_MEM_FREEIF(pbr_table.pbr_tbl_p[rsp_index].path_p);
                    rsp_index++;
                }
                tbl_index++;
            }
            PBM_MEM_FREEIF(pbr_table.pbr_tbl_p);
        }

    }while(0);
}

/*===========================================================================
FUNCTION PBM_UTIL_MAP_USIM_PBR_TABLE_TO_RESPONSE

DESCRIPTION
  This function is used to pack the GET PBR Table Response with the
  information from the GSDI Global Data that contains the USIM Phonebook
  information extracted from EF-PBR.

DEPENDENCIES
  The resulting packing is dependent on the contents of the Global
  Table: pbm_usim_path_lookup_table[pbm_sess_type]

RETURN VALUE
  gsdi_returns_T.
===========================================================================*/
static pbm_return_type pbm_util_map_usim_pbr_table_to_response(
  pbm_get_pbr_table_cnf_T * rsp_p,
  pbm_session_enum_type pbm_sess_type)
{
  int32 tbl_index = 0;
  int32 rsp_index = 0;
  /* --------------------------------------------------------------------------
     Perform NULL Pointer Checks
     ------------------------------------------------------------------------*/
  PBM_CHECK_PTR_RET(rsp_p, PBM_ERROR);

  /* --------------------------------------------------------------------------
     Get memory for the Table
     ------------------------------------------------------------------------*/
  PBM_MEM_ALLOC(rsp_p->pbr_tbl_p,sizeof(pbm_pbr_table_row_type) * (rsp_p->num_rows));

  PBM_CHECK_PTR_RET(rsp_p->pbr_tbl_p, PBM_ERROR);

  memset(rsp_p->pbr_tbl_p,0x00,sizeof(pbm_pbr_table_row_type)*(rsp_p->num_rows));

  /* -------------------------------------------------------------------------
     Iterrate throught pbm_usim_path_lookup_table[pbm_sess_type] and populate the response
     appropriately
     -----------------------------------------------------------------------*/
 /* Search for the first USIM PATH LOOK UP TABLE Entry */
  while ( tbl_index < PBM_NUM_FILES_IN_LOOKUP_TABLE &&
          rsp_index <  rsp_p->num_rows)
  {
    if (pbm_usim_path_lookup_table[pbm_sess_type][tbl_index].file_type != (int)PBM_MAPPING_UNKNOWN &&
        pbm_usim_path_lookup_table[pbm_sess_type][tbl_index].init      == TRUE )
    {
      rsp_p->pbr_tbl_p[rsp_index].file_name    =
        (int)pbm_usim_path_lookup_table[pbm_sess_type][tbl_index].file_id;
      rsp_p->pbr_tbl_p[rsp_index].file_mapping =
        (pbm_pbr_mapping_enum_type)pbm_usim_path_lookup_table[pbm_sess_type][tbl_index].file_type;
      /* Short File ID is not yet saved in the MMGSDI USIM Table */
      rsp_p->pbr_tbl_p[rsp_index].file_sfid    =
        pbm_usim_path_lookup_table[pbm_sess_type][tbl_index].ef_sid;

      /* Set the Order after the IAP File */
      rsp_p->pbr_tbl_p[rsp_index].iap_order =
        pbm_usim_path_lookup_table[pbm_sess_type][tbl_index].index_after_iap;

      /* Now allocate memory for the Path to be made available to the client */
      /* GSDI keeps the path as word array.  Returned to the client
      ** as a byte array.
      */

      PBM_MEM_ALLOC(rsp_p->pbr_tbl_p[rsp_index].path_p,(sizeof(word) * PBM_PATH_LOOKUP_TABLE_MAX_FILE_LEN));

      /* The Path Pointer is being sent back as a byte type.  Thus,
      ** need to send the path len in bytes instead of words
      */

      rsp_p->pbr_tbl_p[rsp_index].path_len =
        ((sizeof(word) / sizeof(byte)) * PBM_PATH_LOOKUP_TABLE_MAX_FILE_LEN);

      if ( rsp_p->pbr_tbl_p[rsp_index].path_p == NULL )
      {
        UIM_MSG_ERR_0("NO Memory for Table Paths");

        /* Need to clean up and free memory allocated for all paths*/
        while ( rsp_index >= 0 )
        {
          if ( rsp_p->pbr_tbl_p[rsp_index].path_p != NULL )
          {
            PBM_MEM_FREEIF(rsp_p->pbr_tbl_p[rsp_index].path_p);
          }
          rsp_index--;
        }
        /* Now free up the block of memory allocated for the table */
        PBM_MEM_FREEIF(rsp_p->pbr_tbl_p);

        /* Now return out of this function with an Error */
        return PBM_ERROR;
      }

      /* Got Memory for individual path, memcpy from the global path */
      memscpy(rsp_p->pbr_tbl_p[rsp_index].path_p, (sizeof(word) * PBM_PATH_LOOKUP_TABLE_MAX_FILE_LEN),
             pbm_usim_path_lookup_table[pbm_sess_type][tbl_index].paths,
             (sizeof(word) * PBM_PATH_LOOKUP_TABLE_MAX_FILE_LEN));

      /* Move to the next free row in the Response Table */
      rsp_index++;
    }
    tbl_index++;
  }
  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION pbm_process_pbr

DESCRIPTION
  Initiliaze linkage type and add the available USIM files to init list.

PARAMETERS
  None.

SIDE EFFECTS
===========================================================================*/
static void pbm_process_pbr(pbm_get_pbr_table_cnf_T * rsp_p,pbm_session_enum_type pbm_sess_type)
{
   pbm_file_type file_id;
   int i;
   int j;
   int index = 0;
   PB_CACHE *pb_cache;
   PB_USIM_CACHE *usim_cache;
   PB_MASTER_USIM_CACHE *m_usim_cache;
   pbm_pb_set_type pb_set;
   int num_type2_files = 0;

   PBM_VALIDATE_SESSION_RET(pbm_sess_type, VOID);
   index = pbm_util_return_fileset_for_session(pbm_sess_type);
   for (i = 0; i < rsp_p->num_rows; i++)
   {
      file_id = PBM_FILE_MAX_REQUESTS;
      for (j = 0; j < (int)PBM_FILE_MAX_REQUESTS; j++)
      {
         if (pbm_file_to_uim_file[j].pbm_session_filename[index] == (pbm_file_type)rsp_p->pbr_tbl_p[i].file_name)
         {
            file_id = pbm_file_to_uim_file[j].pbm_filename;
            break;
         }
      }

      if (file_id != PBM_FILE_MAX_REQUESTS)
      {
         switch(pbm_file_id_to_cache_type(file_id))
         {
            case PBM_CACHE_TYPE_PB:
               pb_cache = (PB_CACHE *)pbm_file_id_to_cache(pbm_sess_type, file_id); //lint !e826 area too small
               if (pb_cache && (file_id == PBM_FILE_ADN || file_id == PBM_FILE_ADN1
               || file_id == PBM_FILE_ADN2 || file_id == PBM_FILE_ADN3
               ))
               {
                  pb_set = pbm_file_id_to_pb_set(file_id);
                  if ( ! pbm_validate_pb_set(pb_set) )
                  {
                      return ;
                  }
                  m_usim_cache = &adn_m_usim_cache[pbm_sess_type][pb_set];
                  m_usim_cache->adn_sfi = (uint8)rsp_p->pbr_tbl_p[i].file_sfid;
               }
               break;

            case PBM_CACHE_TYPE_USIM:
               usim_cache = (PB_USIM_CACHE *)pbm_file_id_to_cache(pbm_sess_type, file_id); //lint !e826 area too small
               if (usim_cache)
               {
                  usim_cache->mapping_type = rsp_p->pbr_tbl_p[i].file_mapping;
                  usim_cache->order = (uint8) rsp_p->pbr_tbl_p[i].iap_order;
                  if (usim_cache->mapping_type == PBM_MAPPING_TYPE_2)
                  {
                     ++num_type2_files;
                  }
                  pbm_init_fileinfo_requests(pbm_sess_type, file_id);
               }
               break;

            case PBM_CACHE_TYPE_INDEX:
            case PBM_CACHE_TYPE_SYNC1:
            case PBM_CACHE_TYPE_SYNC2:
               pbm_init_fileinfo_requests(pbm_sess_type, file_id);
               break;

            case PBM_CACHE_TYPE_EXT:
               /*nothing to do for extension files here */
               break;

            default:
               UIM_MSG_ERR_1("Not handled file %d in read pbr", file_id);
               break;
            }
         }
    }

   pbm_init_fileinfo_requests(pbm_sess_type, PBM_FILE_PSC);
   pbm_init_fileinfo_requests(pbm_sess_type, PBM_FILE_CC);
   pbm_init_fileinfo_requests(pbm_sess_type, PBM_FILE_PUID);

   /* If there are no TYPE 2 files then don't read the IAP files */
   if (num_type2_files == 0)
   {
      pbm_fileinfo_requests[PBM_SESSION_FILENAME_TO_INDEX(pbm_sess_type, PBM_FILE_IAP)].do_request = FALSE;
      pbm_fileinfo_requests[PBM_SESSION_FILENAME_TO_INDEX(pbm_sess_type, PBM_FILE_IAP1)].do_request = FALSE;
      pbm_fileinfo_requests[PBM_SESSION_FILENAME_TO_INDEX(pbm_sess_type, PBM_FILE_IAP2)].do_request = FALSE;
      pbm_fileinfo_requests[PBM_SESSION_FILENAME_TO_INDEX(pbm_sess_type, PBM_FILE_IAP3)].do_request = FALSE;
   }

  if ( pbm_session_type_to_pb_category(pbm_sess_type) == PBM_GPB )
  {
     pbm_session_info[pbm_sess_type].in_use_adn_pb = PBM_PB_GLOBAL_PBR_READ ;
  }
  else
  {
     pbm_session_info[pbm_sess_type].in_use_adn_pb = PBM_PB_LOCAL_PBR_READ ;
  }

   pbm_make_fileinfo_request(pbm_sess_type, 0);

}

/*===========================================================================
FUNCTION pbm_umts_find_pb_file_enum

DESCRIPTION
  This function will map the phone book file enums.This function has support
  for 1000 phonebook entries

DEPENDENCIES
  None

RETURN VALUE
  PBM_ERROR    - if error was encountered
  PBM_SUCCESS  - in case of success

COMMENTS
  This function is used when FEATURE_1000_PBM_SUPPORT is  defined
===========================================================================*/
static pbm_return_type pbm_umts_find_pb_file_enum(pbm_file_type root_file_name,
                                           int                 level,
                                           pbm_file_type *new_file_name_ptr)

{
  int i = 0;
  if (new_file_name_ptr == NULL)
  {
    UIM_MSG_ERR_0("new_file_name_ptr NULL ptr");
    return PBM_ERROR;
  }

  switch(root_file_name)
  {
  case PBM_FILE_GAS:
  case PBM_FILE_EXT1:
    UIM_MSG_ERR_1("No level supported for 0x%x Type 3 File",root_file_name);
    return PBM_ERROR;
  default:
    break;
  }
  for(i=0; i< PBM_NUM_FILES_IN_PB_FILE_ENUM_LOOKUP_TABLE; i++)
  {
    if(root_file_name == pbm_pb_file_enum_lookup_table[i].pbm_base_pb_filename)
    {
      *new_file_name_ptr = pbm_pb_file_enum_lookup_table[i].pbm_pb_filename[level-1];
      break;
    }
  }
  return PBM_SUCCESS;
} /* pbm_umts_find_pb_file_enum */

/*===========================================================================

FUNCTION PBM_UTIL_ASSIGN_ANR_TO_ADN

INPUT
    int *  anr_count_ptr       : count of ANR assigned to ADN
    int    pb_global_flag   : flag to indicate local or global phonebook

DESCRIPTION
  This function helps to assign a base ANR file for an ADN in a single
  phonebook record in the PBR file.


DEPENDENCIES
  None


RETURN VALUE
  pbm_file_type - returns the corresponding ANR file name
  In case of error, returns PBM_FILE_NONE
===========================================================================*/
pbm_file_type pbm_util_assign_anr_to_adn (
  int *anr_count_ptr,
  int pb_global_flag)
{
  pbm_file_type file_id = PBM_FILE_NONE;

  if(anr_count_ptr == NULL)
  {
    UIM_MSG_ERR_0("anr_count_ptr NULL ptr");
    return (file_id);
  }

    switch(*anr_count_ptr)
    {
    case 1:
    file_id = PBM_FILE_ANR;
      break;
    case 2:
    file_id = PBM_FILE_ANRA;
      break;
    case 3:
    file_id = PBM_FILE_ANRB;
      break;
    case 4:
    file_id = PBM_FILE_ANRC;
      break;
    default:
      UIM_MSG_HIGH_1("No ANR file for Record 0x%x",*anr_count_ptr);
      break;
    }
  if(file_id != PBM_FILE_NONE)
  {
    *anr_count_ptr += 1;
  }
  return (file_id);
  }



/*===========================================================================

FUNCTION PBM_UTIL_GET_PHONEBOOK_FILENAME_FROM_TAG

INPUT
    int    pb_global_flag   : flag to indicate local or global phonebook
    byte   ef_tag              : EF Tag from PBR record.
    int *  anr_count_ptr       : count of ANR assigned to ADN

DESCRIPTION
  This function helps to assign a base Phonebook file_id from the tag value
  specified in the record of EF PBR


DEPENDENCIES
  None


RETURN VALUE
  pbm_file_type - returns the corresponding Phonebook file name
  In case of error, returns PBM_FILE_NONE
===========================================================================*/
pbm_file_type pbm_util_get_phonebook_filename_from_tag (
  byte pb_global_flag,
  byte ef_tag,
  int  *anr_count_ptr)
{
    pbm_file_type file_id = PBM_FILE_NONE;
      switch(ef_tag)
      {
      case 0xC0:
        file_id = PBM_FILE_ADN;
        break;
      case 0xC1:
        file_id = PBM_FILE_IAP;
        break;
      case 0xC2:
        file_id = PBM_FILE_EXT1;
        break;
      case 0xC3:
        file_id = PBM_FILE_SNE;
        break;
      case 0xC4:
        file_id = pbm_util_assign_anr_to_adn(anr_count_ptr, pb_global_flag);
        break;
      case 0xC5:
        file_id = PBM_FILE_PBC;
        break;
      case 0xC6:
        file_id = PBM_FILE_GRP;
        break;
      case 0xC7:
        /* Type 3 Files:  Can be shared between Entries */
        file_id = PBM_FILE_AAS;
        break;
      case 0xC8:
        file_id = PBM_FILE_GAS;
        break;
      case 0xC9:
        file_id = PBM_FILE_UID;
        break;
      case 0xCA:
        file_id = PBM_FILE_EMAIL;
        break;
      default:
        file_id = PBM_FILE_NONE;
        UIM_MSG_ERR_0(" -- ERROR:  file_id can't be determined");
        break;
      }

    return (file_id);
}


/*===========================================================================
FUNCTION  PBM_CHECK_FILE_SUPPORTED_OR_NOT

DESCRIPTION
  This function is to check file supported or not.

DEPENDENCIES
  None

RETURN VALUE
  None.

COMMENTS
  None
===========================================================================*/
static void pbm_check_file_supported_or_not(
  pbm_file_type         *base_file_id_ptr,
  uint32                *usim_field_parsed_ptr
)
{
  uint32    check_mask = 0;

  #define EMAIL_PARSED_MASK 1<<1
  #define SNE_PARSED_MASK   1<<2
  #define GRP_PARSED_MASK   1<<3
  #define ADN_PARSED_MASK   1<<4
  #define IAP_PARSED_MASK   1<<5
  #define EXT1_PARSED_MASK  1<<6
  #define PBC_PARSED_MASK   1<<7
  #define AAS_PARSED_MASK   1<<8
  #define GAS_PARSED_MASK   1<<9
  #define UID_PARSED_MASK   1<<10

  if(NULL == base_file_id_ptr || NULL == usim_field_parsed_ptr)
  {
    return;
  }

  /* Check if email/sne/grp/etc. tag has already been parsed in the PBR record */
  switch(*base_file_id_ptr)
  {
    case PBM_FILE_EMAIL:
      check_mask = EMAIL_PARSED_MASK;
      break;

    case PBM_FILE_SNE:
      check_mask = SNE_PARSED_MASK;
      break;

    case PBM_FILE_GRP:
      check_mask = GRP_PARSED_MASK;
      break;

    case PBM_FILE_ADN:
      check_mask = ADN_PARSED_MASK;
      break;

    case PBM_FILE_IAP:
      check_mask = IAP_PARSED_MASK;
      break;

    case PBM_FILE_EXT1:
      check_mask = EXT1_PARSED_MASK;
      break;

    case PBM_FILE_PBC:
      check_mask = PBC_PARSED_MASK;
      break;

    case PBM_FILE_AAS:
      check_mask = AAS_PARSED_MASK;
      break;

    case PBM_FILE_GAS:
      check_mask = GAS_PARSED_MASK;
      break;

    case PBM_FILE_UID:
      check_mask = UID_PARSED_MASK;
      break;

    default:
      break;
  }

  /* Only one EMAIL/SNE/GRP/ADN/IAP/PBC/UID/AAS/GAS file is supported per phonebook set */
  if(*usim_field_parsed_ptr & check_mask)
  {
    UIM_MSG_HIGH_1("Only one 0x%x file supported per phonebook", *base_file_id_ptr);
    *base_file_id_ptr = PBM_FILE_NONE;
  }

  *usim_field_parsed_ptr |= check_mask;
}/* pbm_check_file_supported_or_not */


/*===========================================================================
FUNCTION pbm_umts_populate_fid_lookup_table

DESCRIPTION
  This is a function designed for analyzing the contents of the PBR File.
  It will take a pointer to an array of TLV Objects.  It will then
  go through this array to retrieve the FILE IDs.
  This function has support for 1000 phonebook entries.

DEPENDENCIES
  None

RETURN VALUE
  None.

COMMENTS
  This function is used when FEATURE_1000_PBM_SUPPORT is not defined
===========================================================================*/
static void pbm_umts_populate_fid_lookup_table(
  byte                  *data_ptr,
  byte                   pb_global_flag,
  mmgsdi_len_type        rec_len,
  byte                  *ad8_cnt_ptr,
  pbm_session_enum_type  pbm_sess_type
)
{
  pbm_return_type       pbm_status          = PBM_SUCCESS;
  byte                  file_type           = 0x00;
  byte                  tag                 = 0x00;
  byte                  file_type_len       = 0x00;
  byte                  ef_tag              = 0x00;
  word                  ef_fid              = 0xffff;
  byte                  ef_sid              = 0xff;
  byte                  ef_fid_len          = 0x00;
  word                  index               = 0x00;
  word                  index_offset        = 0x00;
  pbm_file_type         file_id             = PBM_FILE_NONE;
  pbm_file_type         base_file_id        = PBM_FILE_NONE;
  int32                 tlv_len             = 0x00;
  uint32                iap_index_order     = 0x00;
  boolean               is_ef_init_in_table = FALSE;
  int                   anr_count           = 0x01;
  boolean               adn_type1           = FALSE;
  int                   file_type_len_val   = 0;
  uint32                usim_field_parsed   = 0;

  if(NULL == data_ptr || NULL == ad8_cnt_ptr)
  {
    UIM_MSG_ERR_0("data_ptr or ad8_cnt_ptr is NULL");
    return;
  }

  tlv_len        = data_ptr[1]+2;
  /* error checking */
  if (tlv_len > rec_len) {
    /* the record contains bad tlv length value */
    UIM_MSG_ERR_0("Bad TLV Length");
    return;
  }

  while( index < rec_len && data_ptr[index] != 0xFF )
  {
    if( data_ptr[index] == 0xD8  || data_ptr[index] == 0xA8 )
    {
      index++;
      file_type_len_val = data_ptr[index];
      index_offset = index++;
      while( index < index_offset+file_type_len_val )
      {
        if( data_ptr[index++] == 0xC0 )
        {
          adn_type1 = TRUE ;
          index = index_offset+file_type_len_val;
          break;
        }
      }
    }
    else if( data_ptr[index] == 0xD9  || data_ptr[index] == 0xA9 ||
             data_ptr[index] == 0xDA  || data_ptr[index] == 0xAA )
    {
      index++;
      file_type_len_val = data_ptr[index];
      index_offset = index++;
      while( index < index_offset+file_type_len_val )
      {
        if( data_ptr[index++] == 0xC0 )
        {
          UIM_MSG_ERR_0("ADN with type 2 or type 3 - Invalid PBR ");
          return;
        }
      }
    }

    index++;
  }

  index         = 0;
  index_offset  = 0;

  if( !adn_type1  )
  {
    UIM_MSG_ERR_1("No Type 1 ADN in PBR record - Invalid PBR 0x%x ", adn_type1);
    return ;
  }

  while ( (index+1) < tlv_len ) {
    /* while loop check on the index+1 (length field)
       to make sure that we are staying within bound of the tlv len */

    /* Check to see if we need to exit the while loop */
    /* Look at the next File Type TLV, and determine whether we need
       to continue in the while loop or exit */

    /* Analyzing 1st Byte of TLV to Determine File Type*/
    /* A8 or D8 -->  File Type is Type 1 */
    /* A9 or D9 -->  File Type is Type 2 */
    /* AA or DA -->  File Type is Type 3 */
    tag = data_ptr[index];

    switch (tag) {
    case 0xD8:
    case 0xA8:
      file_type = PBM_PB_FILE_TYPE1;

      /* Note the following:  For every PBR Entry, there  */
      /* needs to be at least a Tag Type of D8/A8 because */
      /* the ADN File, which is necessary for a phone     */
      /* book, can only be of tag type D8/A8.  Therefore, */
      /* when a second Tag type D8/A8 is encountered,     */
      /* that means the second PBR Entry is starting.     */
      /* Hence the type_AD8_cnt counter to keep track of  */
      /* how many PBR Entries exist                       */
      (*ad8_cnt_ptr)++;
      if (*ad8_cnt_ptr == PBM_ATLEAST_TWO_CNT_ADN_IN_PBR)
      {
        if (pb_global_flag)
        {
          UIM_MSG_MED_0(" -- PPUL8:  Set G Mult PBR Entry TO TRUE");
        }
        else
        {
          UIM_MSG_MED_0(" -- PPUL8:  Set L Mult PBR Entry TO TURE");
        }
      }
      break;
    case 0xD9:
    case 0xA9:
      file_type = PBM_PB_FILE_TYPE2;
      tag = data_ptr[index];
      break;
    case 0xDA:
    case 0xAA:
      file_type = PBM_PB_FILE_TYPE3;
      tag = data_ptr[index];
      break;
    default:
      return;
    }

    /* Read the next value.  It should be the length of bytes */
    /* for the specified tag                  */
    index++;
    file_type_len = data_ptr[index];

    /* At this point, we know the length of the TLV Data        */
    /* specifically geared to the actual EFs and EF FIDs        */
    /* This loop will grab all EFs listed under the file        */
    /* type determined from above.                    */
    /* Next Elements should be tags, lengths, and values (FID,SID) for  */
    /* C0 EF-ADN  Type 1                        */
    /* C1 EF-IAP  Type 1                        */
    /* C2 EF-EXT1 Type 3                        */
    /* C3 EF-SNE  Type 1 or Type 2                  */
    /* C4 EF-ANR  Type 1 or Type 2                  */
    /* C5 EF-PBC  Type 1                        */
    /* C6 EF-GRP  Type 1                        */
    /* C7 EF-AAS  Type 3                        */
    /* C8 EF-GAS  Type 3                        */
    /* C9 EF-UID  Type 1                        */
    /* CA EF-EMAIL  Type 1 or Type 2                  */
    /* CB EF-CCP1 Type 3                        */
    index_offset = index;

    iap_index_order = 0;
    index++;
    while (index < (file_type_len + index_offset-1)) {
      /* the while loop check is to ensure the tlv has at least a length field */

      iap_index_order++;

      ef_tag = data_ptr[index];
      if (ef_tag == 0xFF)
      {
        /* done, end of the search for the record */
        UIM_MSG_MED_0(" PBR Parsing data = 0xFF, parsing completed");
        return;
      }

      /* Need to get the Length for this EF_TAG */
      index++;
      ef_fid_len = data_ptr[index];

      /* FID Len can be upto 2 or 3 bytes as per spec.*/
      if (ef_fid_len == 0xFF)
      {
        UIM_MSG_MED_0(" PBR Parsing data = 0xFF, parsing completed");
        return;
      }
      /* This is to ignore wrong file ID length TAGs */
      else if ((ef_fid_len != 2 ) && (ef_fid_len != 3))
      {
        /* loop to the next tlv */
        index += ef_fid_len + 1;
        continue;
      }

      /* continue with the parsing */

      /* FID len is 2. Need to get the first part (MSB) of the EF ID */
      ef_fid = data_ptr[index+1];

      /* Shift first part of EF ID 8 bits to make it the High BYTE */
      ef_fid <<= 8;

      /* Get the Second part of the EF ID and add it to the Low Byte*/
      ef_fid += data_ptr[index+2];

      /* Check to see if an SID Exists.  If it does, assign it to EF FID */
      if (ef_fid_len > 2) {
        ef_sid = data_ptr[index+3];
      }

      /* Need to assign a pb fileid to the tag so that it can be
         properly placed in the PATH_LOOKUP_TABLE
      */

      file_id = PBM_FILE_NONE;

      /* Try and get a base file name */
      base_file_id = pbm_util_get_phonebook_filename_from_tag(pb_global_flag, ef_tag, &anr_count);

      /* Check if the file is supported or not as per PBR Rules mentioned in spec */
      pbm_check_file_supported_or_not(&base_file_id, &usim_field_parsed);

      if (base_file_id != PBM_FILE_NONE)
      {
        /* Temporary status variable to hold the return status for calls to
         * pbm_util_is_ef_init_path_lookup_table function
        */
        int level                       = 1;
        pbm_return_type temp_pbm_status = PBM_SUCCESS;
        pbm_status                      = PBM_SUCCESS;

        temp_pbm_status = pbm_util_is_ef_init_path_lookup_table(
                             &is_ef_init_in_table, base_file_id,pbm_sess_type);
        while ( (temp_pbm_status == PBM_SUCCESS) && is_ef_init_in_table &&
                (level < PBM_MAX_ADN_CNT_SUPPORTED) )
        {
          pbm_status = pbm_umts_find_pb_file_enum(base_file_id, level, &file_id);
          if (pbm_status == PBM_SUCCESS)
          {
            temp_pbm_status = pbm_util_is_ef_init_path_lookup_table(
                                 &is_ef_init_in_table, file_id,pbm_sess_type);
          }
          else
          {
            break;
          }
          level += 1;
        }/* while ( (temp_pbm_status == PBM_SUCCESS) && is_ef_init_in_table &&
                (level < PBM_MAX_ADN_CNT_SUPPORTED) )*/
        if(temp_pbm_status != PBM_SUCCESS)
        {
          pbm_status = PBM_ERROR;
        }
        if((level == 1) && (!is_ef_init_in_table) && (temp_pbm_status == PBM_SUCCESS))
        {
          file_id = base_file_id;
        }
        if(level > PBM_MAX_ADN_CNT_SUPPORTED)
        {
          UIM_MSG_ERR_1("Could not assign PB file id for 0x%x",file_id);
          pbm_status = PBM_ERROR;
        }/* if((level >= PBM_MAX_ADN_CNT_SUPPORTED) && (pbm_status != PBM_SUCCESS))*/
      }
      else
      {
        /* file_name was 0 */
        UIM_MSG_ERR_1(" No such file 0x%x",base_file_id);
        pbm_status = PBM_ERROR;
      }/* if (base_file_id != PBM_FILE_NONE) */

      if (pbm_status == PBM_SUCCESS)
      {
        if (!pbm_util_write_phonebook_path_lookup_table(file_id, file_type, ef_fid, ef_sid, pb_global_flag,
                                                                           iap_index_order, tag,pbm_sess_type))
        {
          UIM_MSG_ERR_3(" PPUL8: Could not find file id %d file_type %d ef_fid %d",
                          file_id, file_type, ef_fid);
          pbm_util_dump_path_lookup_table(pbm_sess_type);
          pbm_status = PBM_ERROR;
        }
      }
      else
      {
        /* reset pbm_status = SUCCESS */
        pbm_status = PBM_SUCCESS;
      } /* if (pbm_status == PBM_SUCCESS) */

      /* loop to the next tlv */
      index += ef_fid_len + 1;
    }/* while (index < (file_type_len + index_offset-1)) */

    if(index < (rec_len-1))
    {
      /* Two checks are required to be satisfied before continuing on with the next loop */
      /* 1. the total len has to be smaller than record length */
      /* 2. the file type of the next tlv has to be a valid file type (done at the beginning of the while loop */

      /* Check 1 */
      tlv_len = tlv_len + data_ptr[index+1] + 2;
      if (tlv_len > rec_len) 
      { /* finish with the parsing */
        UIM_MSG_MED_0(" -- PPUL8:  Parsing Finished");
        break;
      }/* if (tlv_len > rec_len) */
    }
    else
    {
      break;
    } /* if(index < (rec_len-1)) */
  } /* End of While Loop while ( (index+1) < tlv_len ) */
}/* pbm_umts_populate_fid_lookup_table */


/*===========================================================================
FUNCTION PBM_UTIL_DUMP_PATH_LOOKUP_TABLE

DESCRIPTION
   This function simply dump some of the contents of the PATH Lookup Table.

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
static void pbm_util_dump_path_lookup_table ( pbm_session_enum_type pbm_sess_type )
{
  byte  file_num = 0;
  for ( file_num = 0; file_num < PBM_NUM_FILES_IN_LOOKUP_TABLE; file_num++ ) {
    UIM_MSG_HIGH_3("  %x,%x,%x",pbm_usim_path_lookup_table[pbm_sess_type][file_num].file_id,
                pbm_usim_path_lookup_table[pbm_sess_type][file_num].init,
                pbm_usim_path_lookup_table[pbm_sess_type][file_num].paths[2]);
  }
}

/*===========================================================================
FUNCTION PBM_UTIL_SEARCH_PATH_LOOKUP_TABLE

DESCRIPTION
   This function will be used to check the usim_path_lookup_table
   maintained with FIDs.

PARAMETERS
  out_filenum : OUT variable which would be filled up with the file number
                on a successful search in the lookup table.
  file_id :    file_id to be searched for in the lookup table

DEPENDENCIES
  None

RETURN VALUE
  pbm_return_type:  Represents whether or not the search was successful
                   PBM_SUCCESS - Successful find and filling the output param
                   PBM_INCORRECT_PARAMS - Invalid parameters supplied
                   PBM_NOT_FOUND - Item not found in the lookup table

===========================================================================*/
static pbm_return_type pbm_util_search_path_lookup_table (
  byte                   *out_filenum,
  pbm_file_type     file_id,
  pbm_session_enum_type pbm_sess_type
)
{
  byte           file_num    = 0;
  pbm_return_type pbm_status = PBM_ERROR_NOT_FOUND;

  if (out_filenum == NULL)
  {
    UIM_MSG_ERR_0("pbm_util_search_path_lookup_table out_filenum is NULL");
    return PBM_ERROR;
  }

  for ( file_num = 0; file_num < PBM_NUM_FILES_IN_LOOKUP_TABLE; file_num++ )
  {
    if ( pbm_usim_path_lookup_table[pbm_sess_type][file_num].file_id == file_id )
    {
      *out_filenum = file_num;
      pbm_status =  PBM_SUCCESS;
      break;
    }
  }
  return pbm_status;
}

/*===========================================================================
FUNCTION PBM_UTIL_WRITE_PHONEBOOK_PATH_LOOKUP_TABLE

DESCRIPTION
   This function will be used to change the contents of the
   pbm_usim_path_lookup_table[pbm_sess_type] used for keeping track of the
   FIDs not explicitly defined by the TS 31.102 Section 4.4.2

DEPENDENCIES
  None

RETURN VALUE
  pbm_return_type.
===========================================================================*/
static boolean pbm_util_write_phonebook_path_lookup_table (
  pbm_file_type     file_id,
  byte          file_type,
  unsigned int  ef_id,
  uint32        ef_sid,
  byte          global,
  uint32        iap_index_order,
  byte          tag,
  pbm_session_enum_type pbm_sess_type)
{
  pbm_return_type  pbm_status = PBM_SUCCESS;
  byte            file_num    = 0;

  pbm_status = pbm_util_search_path_lookup_table(&file_num , file_id,pbm_sess_type);

  /* Showing the Contents of the Table before they are changed */
  /* OK to remove this comment and following code at a later   */
  /* time.                           */
  if (pbm_status == PBM_SUCCESS)
  {
    UIM_MSG_MED_3("  %x, %x, %x",file_num,
                               pbm_usim_path_lookup_table[pbm_sess_type][file_num].init,
                               pbm_usim_path_lookup_table[pbm_sess_type][file_num].paths[2]);

    /* Make changes to the PATH LOOKUP TABLE to do the following */
    /*     1)  Properly set the PATH                             */
    /*     2)  Properly set the FILE TYPE                        */
    /*     3)  Properly set the INIT flag to TRUE                */
    /*                               */
    pbm_usim_path_lookup_table[pbm_sess_type][file_num].paths[2]  = (word)ef_id;
    pbm_usim_path_lookup_table[pbm_sess_type][file_num].ef_sid    = ef_sid;
    pbm_usim_path_lookup_table[pbm_sess_type][file_num].file_type = file_type;
    pbm_usim_path_lookup_table[pbm_sess_type][file_num].init      = TRUE;


    if(USE_USIM_LOCAL_SUPPORT(pbm_sess_type) || USE_CSIM_LOCAL_SUPPORT(pbm_sess_type))
    {
       pbm_usim_path_lookup_table[pbm_sess_type][file_num].paths[0] = 0x7FFF;
    }
    else
    {
       pbm_usim_path_lookup_table[pbm_sess_type][file_num].paths[0] = 0x7F10;
    }

    if (tag == 0xA9 || tag == 0xD9 )
    {
      pbm_usim_path_lookup_table[pbm_sess_type][file_num].index_after_iap = iap_index_order;
    }

    UIM_MSG_MED_3("  %x, %x, %x",file_num,
                               pbm_usim_path_lookup_table[pbm_sess_type][file_num].init,
                               pbm_usim_path_lookup_table[pbm_sess_type][file_num].paths[2]);
  }
  else
  {
    UIM_MSG_ERR_3(" -- Could not Find EF in PATH LOOKUP TABLE Looking for %x, %x, %d",
                   file_id, ef_id, file_type);
    pbm_util_dump_path_lookup_table(pbm_sess_type);
  }
  return pbm_status == PBM_SUCCESS;
} /*lint !e715 */

/*===========================================================================
FUNCTION PBM_UTIL_COPY_PATH_LOOKUP_TABLE

DESCRIPTION
   This function will be used to check the usim_path_lookup_table
   maintained with FIDs.

DEPENDENCIES
  None

RETURN VALUE
  pbm_return_type.
===========================================================================*/
static pbm_return_type pbm_util_copy_path_lookup_table (
  pbm_file_type     file_id,
    mmgsdi_path_type      *path_ptr,
  pbm_session_enum_type pbm_sess_type)
{
  pbm_return_type  pbm_status = PBM_SUCCESS;
  byte      file_num;

  pbm_status = pbm_util_search_path_lookup_table(&file_num,file_id,pbm_sess_type);

  if (pbm_status == PBM_SUCCESS) {

    /* Now execute the memcopy using the calculated length */
    /* Note:  memcpy does a byte copy, so you length needs */
    /* to be multiplied by 2 (handled above)         */
    memscpy( path_ptr->path_buf,sizeof(path_ptr->path_buf),
        pbm_usim_path_lookup_table[pbm_sess_type][file_num].paths,
        PBM_LKUP_TABLE_LEN * 2);

    path_ptr->path_len = PBM_LKUP_TABLE_LEN;

    /* Dump out some contents for review */
    UIM_MSG_MED_3("cop_path_lookup_table  %x\t%x\t%x",path_ptr->path_buf[0],
                                                    path_ptr->path_buf[1],
                                                    path_ptr->path_buf[2]);
  }

  return pbm_status;

}

/*===========================================================================
FUNCTION PBM_UTIL_IS_EF_INIT_PATH_LOOKUP_TABLE

DESCRIPTION
   This function will be used to check the usim_path_lookup_table
   and determine whether the EF has been initialized or not.

PARAMETERS
  out_is_init : OUT variable which would be filled up with the information
                about whether or not the file was init in the path lookup table
  file_id    : file name uim item to be searched for in the lookup table

DEPENDENCIES
  None

RETURN VALUE
  pbm_return_type:  Represents whether or not the search was successful
                   PBM_SUCCESS - Successful find and filling the output param
                   PBM_INCORRECT_PARAMS - Invalid parameters supplied
                   PBM_NOT_FOUND - Item not found in the lookup table

===========================================================================*/
static pbm_return_type pbm_util_is_ef_init_path_lookup_table (
  boolean                *out_is_init,
  pbm_file_type     file_id,
  pbm_session_enum_type pbm_sess_type
)
{
  pbm_return_type   pbm_status = PBM_ERROR;
  byte             file_num = 0;

  if (out_is_init == NULL)
  {
    UIM_MSG_ERR_0("pbm_util_is_ef_init_path_lookup_table out_is_init is NULL");
    return PBM_ERROR;
  }

  /* Get the index (file number) in the path lookup table */
  pbm_status = pbm_util_search_path_lookup_table(&file_num, file_id,pbm_sess_type);

  /* If the file is found in the lookup table return the init variable */
  if ( pbm_status == PBM_SUCCESS)
  {
    *out_is_init = pbm_usim_path_lookup_table[pbm_sess_type][file_num].init;
  }

  return pbm_status;
}


pbm_return_type pbm_util_access_path_lookup_table (
  pbm_file_type     file_id,
  mmgsdi_access_type       *file_struct,
  boolean                  *is_ef_init_in_table_ptr,
  pbm_session_enum_type pbm_sess_type)
{
  pbm_return_type	pbm_status = PBM_SUCCESS;

  /* Check to see if ef is init in Path Lookup Table */
  pbm_status = pbm_util_is_ef_init_path_lookup_table(is_ef_init_in_table_ptr,
                                                           file_id,pbm_sess_type);

  if (pbm_status == PBM_SUCCESS)
  {
    if (*is_ef_init_in_table_ptr == TRUE)
    {
      /* Once determined to be init, i.e. a path exists, copy it to a global */
      pbm_status = pbm_util_copy_path_lookup_table(file_id,&file_struct->file.path_type,pbm_sess_type);
    }
  }
  return (pbm_status);
} /* mmgsdi_util_access_path_lookup_table */


/* ==========================================================================
FUNCTION PBM_CONVERT_PATH_TO_FILE_ENUM

DESCRIPTION
  This function, on providing the complete path, returns the mmgsdi file
  enum

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS
  MMGSDI_ERROR

SIDE EFFECTS
  NONE
----------------------------------------------------------------------------*/
pbm_return_type pbm_convert_path_to_file_enum(
  pbm_session_enum_type session_type,
  mmgsdi_path_type       path_type,
  pbm_file_type *file_enum_ptr)
{
  pbm_return_type l_pbm_status = PBM_ERROR;
  uint8                   file_num      = 0;
  uint8                   static_file_num      = 0;
  int len_to_be_used = 0;
  uint32 fileset;

  if(path_type.path_len > MMGSDI_MAX_PATH_LEN ) /* for Klocwork errors */
  {
    len_to_be_used = MMGSDI_MAX_PATH_LEN ;
  }
  else
  {
    len_to_be_used = path_type.path_len;
  }
  PBM_CHECK_PTR_RET(file_enum_ptr,PBM_ERROR);

  *file_enum_ptr = PBM_FILE_MAX_REQUESTS;

  if (len_to_be_used > 2)
  {
    for(file_num = 0; file_num < PBM_NUM_FILES_IN_LOOKUP_TABLE; file_num++)
    {
      if(pbm_usim_path_lookup_table[session_type][file_num].init == TRUE)
      {
        /*
         * pbm_usim_path_lookup_table holds file path excluding the MF file id
         * without any holes like in uim path table.
         * so compare the rest of the path items except MFID.
         */
        if(!memcmp(pbm_usim_path_lookup_table[session_type][file_num].paths,
                  (path_type.path_buf + 1),
                  (len_to_be_used - 1)*sizeof(uint16)))
        {
          *file_enum_ptr = pbm_usim_path_lookup_table[session_type][file_num].file_id;
          break;
        }
      }
    }/* for(;;) */
    if (*file_enum_ptr != PBM_FILE_MAX_REQUESTS)
      {
        l_pbm_status = PBM_SUCCESS;
      }
      else
      {
      //try in static path table
      fileset = pbm_util_return_fileset_for_session(session_type);
      for (static_file_num=0; static_file_num<(int)ARR_SIZE(pbm_static_path_lookup_table); static_file_num++)
      {
        len_to_be_used=pbm_static_path_lookup_table[static_file_num].path_length_to_be_used[fileset];
	 if(!memcmp(pbm_static_path_lookup_table[static_file_num].paths[fileset],
                  path_type.path_buf,
                  len_to_be_used*sizeof(uint16)))
        {
          *file_enum_ptr = pbm_static_path_lookup_table[static_file_num].pbm_filename;
          l_pbm_status = PBM_SUCCESS;
          break;
        }
      }
    }/* if (pbm_file_enum != PBM_FILE_NONE) */
  }

  return l_pbm_status;
}/* mmgsdi_convert_path_to_file_enum */

/*===========================================================================
FUNCTION PBM_FILEINFO_COMPLETE

DESCRIPTION
  After we have all the file attributes we need, this function will
  allocate the appropriate arrays based upon those answers.

PARAMETERS
   None.

SIDE EFFECTS
  Globals describing the world according to this SESSION are set.
===========================================================================*/
static void pbm_fileinfo_complete(pbm_session_enum_type session_type)
{
   int i;
   uint16 k;
   int j;
   int m_index[PBM_MAX_NUM_PB_SETS] = { 1, 1, 1, 1};

   pbm_file_type adn_file_id[PBM_MAX_NUM_PB_SETS] = {
      PBM_FILE_ADN,
      PBM_FILE_ADN1,
      PBM_FILE_ADN2,
      PBM_FILE_ADN3,
   };

   boolean bAllocMasterUSIMCache = FALSE;

#define ALLOC_MEM_W_ERRCODE(dest, num, size, code)  \
   PBM_MEM_FREEIF((dest));                           \
   if ((num)) {                                      \
      PBM_MEM_ALLOC((dest), (size));                  \
   if ((dest) == NULL) {                           \
      {code};                                       \
      (num) = 0;                                    \
      }                                               \
   }

#define ALLOC_MEM(dest, num, size)                  \
   ALLOC_MEM_W_ERRCODE((dest), (num), (size), NO_ACTION)
   PBM_VALIDATE_SESSION_RET(session_type, VOID);
   if (pbm_session_info[session_type].pbm_refresh_init == PBM_REFRESH_ONLY)
   /* for PBM refresh dring init state, pbm_session_activated will be set and else if code leg will be followed */
   {
      for (i = 0; i < pbm_session_info[session_type].pbm_refresh_num_file_devices; i++)
      {
         if (PBM_FILE_ADN == pbm_session_info[session_type].pbm_refresh_pbm_file_devices[i])
         {
            /* we're refreshing ADN.  We want to alloc USIM caches */
            bAllocMasterUSIMCache = TRUE;
            break;
         }
      }
   }
   else if(pbm_session_activated & PBM_SESSIONID_TO_SESSION_MASK(session_type))
   {
	 /* we're neither refreshing nor reading the ECC, so this is powerup. Want to alloc USIM caches */
	 bAllocMasterUSIMCache = TRUE;
   }


   /* Allocate master USIM cache for ADN. */
   if (bAllocMasterUSIMCache)
   {
      for (j = 0; j < (int)PBM_MAX_NUM_PB_SETS; j++)
      {
         i = PBM_SESSION_FILENAME_TO_INDEX(session_type, adn_file_id[j]);
         if( i < ARR_SIZE(pbm_fileinfo_requests) )
         {
          if (j < adn_cache[session_type].num_files)
          {
            if (pbm_fileinfo_requests[i].do_request)
            {
               PB_MASTER_USIM_CACHE *m_usim_cache =
               &adn_m_usim_cache[session_type][j];

               ALLOC_MEM(m_usim_cache->usim_cache_array,
                         m_usim_cache->num_caches,
                         (uint32) (m_usim_cache->num_caches+1) *
                         sizeof(PB_USIM_CACHE *));

               ALLOC_MEM_W_ERRCODE(m_usim_cache->index_array,
                                   m_usim_cache->num_caches,
                                   (uint32) (m_usim_cache->num_caches+1) * sizeof(byte *),
                                   PBM_MEM_FREEIF(m_usim_cache->usim_cache_array));

               if (m_usim_cache->num_caches)
               {
                  memset(m_usim_cache->usim_cache_array, 0,
                         (uint32) (m_usim_cache->num_caches+1) *
                         sizeof(PB_USIM_CACHE *));
                  memset(m_usim_cache->index_array, 0,
                         (uint32) (m_usim_cache->num_caches+1) * sizeof(byte *));
               }
            }
         } else
         {
            pbm_fileinfo_requests[i].do_request = FALSE;
         }
      }
    }
   }

   /* For every file within this UIM, allocate the corresponding cache
   * entries */
   for (i = (int)session_type * (int) PBM_FILE_MAX_REQUESTS;
       i < ((int)session_type + 1) * (int) PBM_FILE_MAX_REQUESTS;
       i++)
   {
      pbm_file_type file_id = (pbm_file_type)
                              (i % (int) PBM_FILE_MAX_REQUESTS);
      void *cache = pbm_file_id_to_cache(session_type, file_id);
      if (file_id == PBM_FILE_NONE || file_id == PBM_FILE_ALL || file_id == PBM_FILE_MAX_REQUESTS) /* KW error check */
         continue;

      if (!pbm_fileinfo_requests[i].do_request)
         continue;

      if (!cache)
      {
         UIM_MSG_ERR_2("Invalid cache for file id %d sim %d", file_id,
                        session_type);
         pbm_fileinfo_requests[i].do_request = FALSE;
         continue;
      }

      switch (pbm_file_id_to_cache_type(file_id))
      {
         /*------------------------------------------------------------------*/
         /* EXT_CACHE */
         case PBM_CACHE_TYPE_EXT:
            {
               pbm_ext_cache_s_type *ext_cache = (pbm_ext_cache_s_type *)cache;

               ALLOC_MEM (ext_cache->in_use_array, ext_cache->num_of_rec,
                          ((uint32)ext_cache->num_of_rec + 1) *
                          sizeof(pbm_ext_rec_in_use_s_type));

               if (ext_cache->num_of_rec)
               {
                  memset(ext_cache->in_use_array, 0,
                         ((uint32)ext_cache->num_of_rec + 1) *
                         sizeof(pbm_ext_rec_in_use_s_type));
               }
               break;
            } /* EXT_CACHE */
            /*-------------------------------------------------------------------*/
            /* PB_CACHE */
         case PBM_CACHE_TYPE_PB:
            {
               PB_CACHE *pb_cache = (PB_CACHE *) cache; //lint !e826 area too small
               /* if pb_cache_array is already filled with pointers to
               * records, we need to free those records first */

               if (pb_cache->records_in_use > 0)
               {
                  for ( k = 1; k <= pb_cache->num_of_records; k++ )
                  {
                     if (pb_cache->pb_cache_array[k])
                     {
                        (void)pbm_cache_record_free(pb_cache->pb_cache_array[k]);
                     }
                  }
               }

               ALLOC_MEM(pb_cache->pb_cache_array, pb_cache->num_of_records,
                         ((uint32) pb_cache->num_of_records + 1) *
                         sizeof(pbm_addr_cache_s_type *));

               pb_cache->start = 1;
               if (pb_cache->num_of_records)
               {
                  memset(pb_cache->pb_cache_array, 0,
                         ((uint32) pb_cache->num_of_records + 1) *
                         sizeof(pbm_addr_cache_s_type *));
                  pb_cache->circular = pbm_file_circular(file_id);
                  pb_cache->start = pb_cache->num_of_records;
               }
               pb_cache->records_in_use = 0;
            /* adding mem allocation for hiden cache if the file id is ADN, ADN1 or ADN2 or ADN3*/
            /* pb_cache->num_of_records is to be used as number of records can be fetched from ADN cache only */
               if ( (file_id == PBM_FILE_ADN) || (file_id == PBM_FILE_ADN1)
               || (file_id == PBM_FILE_ADN2) || (file_id == PBM_FILE_ADN3)
               )
               {
                  PB_CACHE *pb_cache_hidden = &hidden_cache[session_type];

                  if (pb_cache_hidden->records_in_use > 0)
                  {
                     for ( k = 1; k <= pb_cache_hidden->num_of_records; k++ )
                     {
                        if (pb_cache_hidden->pb_cache_array[k])
                        {
                           (void)pbm_cache_record_free(pb_cache_hidden->pb_cache_array[k]);
                        }
                     }
                  }

                  /* Updating to a new value after freeing to avoid running 
                     for loop on hidden cache with an incorrect value */
                  pb_cache_hidden->num_of_records = pb_cache->num_of_records;
                  ALLOC_MEM(pb_cache_hidden->pb_cache_array, pb_cache_hidden->num_of_records,
                          ((uint32) pb_cache_hidden->num_of_records + 1) *
                          sizeof(pbm_addr_cache_s_type *));

                  pb_cache_hidden->start = 1;
                  if (pb_cache_hidden->num_of_records)
                  {
                     memset(pb_cache_hidden->pb_cache_array, 0,
                           ((uint32) pb_cache_hidden->num_of_records + 1) *
                     sizeof(pbm_addr_cache_s_type *));
                     pb_cache_hidden->circular = pbm_file_circular(file_id);
                     pb_cache_hidden->start = pb_cache_hidden->num_of_records;
                  }
                  pb_cache_hidden->records_in_use = 0;
               }
               break;
            } /* PB_CACHE */

            /*------------------------------------------------------------------*/
            /* USIM_CACHE */
         case PBM_CACHE_TYPE_USIM:
            {
               PB_USIM_CACHE *usim_cache = (PB_USIM_CACHE *) cache;
               PB_MASTER_USIM_CACHE *m_usim_cache;
               pbm_pb_set_type pb_set = pbm_file_id_to_pb_set(file_id);
               int adn_records_this_file;
               int index;
               if (!usim_cache->num_of_records ||
                   !adn_cache[session_type].num_of_records)
                  break;

               if ( ! pbm_validate_pb_set(pb_set) )
               {
                   return ; 
               }
               m_usim_cache = &adn_m_usim_cache[session_type][pb_set];

               if (usim_cache->mapping_type == PBM_MAPPING_TYPE_2)
                  index = usim_cache->order;
               else
                  index = m_usim_cache->iap_rec_len + m_index[pb_set]++;

               if (index > m_usim_cache->num_caches) /* 1-based array */
               {
                  UIM_MSG_ERR_2("ADN cache has wrong number of usim caches %d, %d",
                              m_usim_cache->num_caches, index);
                  break;
               }

               /* if pb_cache_array is already filled with pointers to
               * records, we need to free those records first */
               if (usim_cache->records_in_use > 0)
               {
                  for ( k = 1; k <= usim_cache->num_of_records; k++ )
                  {
                     if (usim_cache->pb_cache_array[k])
                     {
                        (void)pbm_usim_cache_delete(session_type, usim_cache,
                                                    k, FALSE);
                     }
                  }
               }

               /* Allocate memory for USIM data */
               ALLOC_MEM(usim_cache->pb_cache_array, usim_cache->num_of_records,
                         ((uint32) usim_cache->num_of_records + 1) *
                         sizeof(pbm_usim_cache_s_type));

               /* Allocate memory for master USIM data */
               adn_records_this_file = adn_cache[session_type].records_in_pb_set[pb_set];

               PBM_CHECK_PTR_RET(m_usim_cache->index_array, VOID);
               ALLOC_MEM_W_ERRCODE(m_usim_cache->index_array[index],
                                   usim_cache->num_of_records,
                                   (uint32) (adn_records_this_file+1) * sizeof(byte),
                                   PBM_MEM_FREEIF(usim_cache->pb_cache_array));

               if (usim_cache->num_of_records)
               {
                  memset(usim_cache->pb_cache_array, 0,
                         ((uint32) usim_cache->num_of_records + 1) *
                         sizeof(pbm_usim_cache_s_type));

                  memset(m_usim_cache->index_array[index], 0,
                         (uint32) (adn_records_this_file+1) * sizeof(byte));

                  /* Add this cache to the master usim cache array */
                  m_usim_cache->usim_cache_array[index] = usim_cache;
                  if (usim_cache->mapping_type != PBM_MAPPING_TYPE_2)
                     usim_cache->order = (uint8)index;
               }
               break;
            } /* USIM_CACHE */

            /*------------------------------------------------------------------*/
         case PBM_CACHE_TYPE_INDEX:
         case PBM_CACHE_TYPE_SYNC1:
            break;

            /*------------------------------------------------------------------*/
         case PBM_CACHE_TYPE_SYNC2:
            {
               PB_SYNC2_CACHE *sync2_cache = (PB_SYNC2_CACHE *) cache;
               if (!sync2_cache->num_of_records ||
                   !adn_cache[session_type].num_of_records)
                  break;

               /* Allocate memory for USIM data */
               ALLOC_MEM(sync2_cache->data_array, sync2_cache->num_of_records,
                         ((uint32) sync2_cache->num_of_records + 1) *
                         sizeof(uint16));

               if (sync2_cache->num_of_records)
               {
                  memset(sync2_cache->data_array, 0,
                         ((uint32) sync2_cache->num_of_records + 1) *
                         sizeof(uint16));
               }
               break;
            }
            /*------------------------------------------------------------------*/
         default:
            UIM_MSG_ERR_1("Unrecognized file id %d", file_id);
            break;
      }
      pbm_fileinfo_requests[i].do_request = FALSE;
   }

   /* Initialize the PBM SEEK data before starting to read */
   pbm_init_seek_data(session_type);

   pbm_initialize_index[session_type] = 0;

   pbm_init_entries (session_type);
}

/*===========================================================================
FUNCTION PBM_MAKE_FILEINFO_REQUEST

DESCRIPTION
  This function looks for the pbm_fileinfo_requests array looking for
  the next item to request.  If it finds one, it launches the request,
  otherwise it says we have completed getting the entries.

PARAMETERS
   start [in] The file_id with which to start. The interval of file_id is
              [PBM_FILE_ADN, PBM_FILE_MAX_REQUESTS).
              start is NOT the index into pbm_fileinfo_requests array but the
              index can be calculated from it.

SIDE EFFECTS
  Globals describing the world according to this SESSION are set.
===========================================================================*/
void pbm_make_fileinfo_request (pbm_session_enum_type session_type,
                                       uint32 start)
{
   pbm_return_type pbm_status = PBM_ERROR;
   mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
   uint32 i;                /* Index into pbm_fileinfo_requests array */
   uint32 file_id;
   int wms_for_prov_done=0;

   PBM_VALIDATE_SESSION_RET(session_type, VOID);

   if ( pbm_session_type_to_pb_category(session_type) == PBM_GPB )
   {
	   /* if card session check if we got wms event for the provisioning sessions on the card */
	   for ( i=0 ; i < PBM_SESSION_MAX ; i ++ )
	   {
		   if ( (pbm_session_info[session_type].slot_id == pbm_session_info[i].slot_id) && (pbm_session_type_to_pb_category(i) == PBM_LPB ) )
		   {
			   if (  pbm_wms_init_finished & PBM_SESSIONID_TO_SESSION_MASK(i)  )
			   {
				   wms_for_prov_done++;
			   }
		   }
	   }
	   if ( !wms_for_prov_done )
	   {
		   UIM_MSG_HIGH_3("Did not receive wms finish events for all provisoing slots sess %d wms_init 0x%x wms_for_prov_done %d",
                                   session_type,pbm_wms_init_finished,wms_for_prov_done);
		   return ;
	   }
   }
   
   if ( (pbm_session_type_to_pb_category(session_type) == PBM_LPB ) &&  !(pbm_wms_init_finished & PBM_SESSIONID_TO_SESSION_MASK(session_type)) )
   {
	     UIM_MSG_HIGH_2("Waiting for WMS event session_type %d pbm_wms_init_finished %d ",session_type,pbm_wms_init_finished);
	     return ;
   }

   /* Starting at the current location, find the next item for which we
   * want to make a request. */
   for (file_id = start; file_id < (uint32)PBM_FILE_MAX_REQUESTS; file_id++)
   {
      i = file_id + ((uint32)session_type * (uint32)PBM_FILE_MAX_REQUESTS);

      /* If this offset needs the request, send it off. */
      if (pbm_fileinfo_requests[i].do_request)
      {
          //check if the availablity of service
          if(pbm_fileinfo_requests[i].service_available != MMGSDI_NONE)
          {
                mmgsdi_status =  mmgsdi_session_is_service_available(pbm_session_info[session_type].session_id,
                                                                    (mmgsdi_service_enum_type)pbm_fileinfo_requests[i].service_available,
                                                                    pbm_mmgsdi_service_available_cb,
                                                                    i);
          }
          else
          {
              //no need to check for service availablity,directly read the attributes
                pbm_status = pbm_mmgsdi_get_file_attributes(session_type,/* Session id */
                                                            pbm_fileinfo_requests[i].file_id,  /* file_id */
                                                            i,/* Client Ref */
                                                            pbm_mmgsdi_fileinfo_cb);/* Async CB. */

          }
          if ( (PBM_SUCCESS == pbm_status) || (MMGSDI_SUCCESS == mmgsdi_status) )
            return;
          else
            UIM_MSG_ERR_1("Unexpected return value from pbm_mmgsdi_get_file_attributes i = %d",i);
      }
      else
      {
        PB_CACHE *cache;
        if(pbm_file_id_to_cache_type(file_id) == PBM_CACHE_TYPE_PB)          
        {
          cache = (PB_CACHE *) pbm_file_id_to_cache(session_type, file_id);
          rex_enter_crit_sect(&pbm_crit_sect);
          if (cache != NULL && cache->num_of_records == 0)
          {
             cache->status = PBM_STATUS_NOT_AVAILABLE;
          }
          rex_leave_crit_sect(&pbm_crit_sect);        
   }
      }
        
   }

   /* If we reach this line, that means we have made all the requests that
   * will be pending for this session_type.  Call pbm_fileinfo_complete to process
   * this information and continue the initialization process.
   */
   pbm_fileinfo_complete(session_type);
}

/*===========================================================================
FUNCTION PBM_FILL_FILEINFO_REQUEST

DESCRIPTION
  This function is used to fill a fileinfo request for a given PBM file.

DEPENDENCIES
  card_apps must be previously set.
  Not reentrant. This function should always be called from PBM task.

SIDE EFFECTS
  Globals describing the world according to this SIM are set.
===========================================================================*/
static void pbm_fill_fileinfo_request (
                                      pbm_session_enum_type session_type,
                                      mmgsdi_service_enum_type service,
                                      pbm_file_type session_file)
{
   PBM_VALIDATE_SESSION_RET(session_type, VOID);  /* KW error, session type should be valid else out of bound error */
   if (session_file == PBM_FILE_NONE || session_file == PBM_FILE_ALL || session_file == PBM_FILE_MAX_REQUESTS) /* KW error check */
    return;
   pbm_fileinfo_requests[PBM_SESSION_FILENAME_TO_INDEX(session_type,
                                                       session_file)].service_available = service;

   pbm_fileinfo_requests[PBM_SESSION_FILENAME_TO_INDEX(session_type,
                                                       session_file)].file_id = session_file;

   pbm_fileinfo_requests[PBM_SESSION_FILENAME_TO_INDEX(session_type,
                                                       session_file)].do_request = TRUE;
}

/*=============================================================================
FUNCTION pbm_adn_caching_required_or_not

DESCRIPTION
  This function is used to find out the given filed id is needs to be cached or 
  not when ADN caching is diabled or not using NV 71535

DEPENDENCIES
  None

SIDE EFFECTS
  None.
===============================================================================*/
boolean pbm_adn_caching_required_or_not(pbm_file_type file_id)
{
  switch(file_id)
  {
    case PBM_FILE_ADN:
    case PBM_FILE_GAS:
    case PBM_FILE_AAS:
    case PBM_FILE_EXT1:
    case PBM_FILE_PSC:
    case PBM_FILE_PUID:
    case PBM_FILE_CC:
    case PBM_FILE_EMAIL:
    case PBM_FILE_SNE:
    case PBM_FILE_GRP:
    case PBM_FILE_ANR:
    case PBM_FILE_ANRA:
    case PBM_FILE_ANRB:
    case PBM_FILE_ANRC:
    case PBM_FILE_IAP:
    case PBM_FILE_UID:
    case PBM_FILE_PBC:
    case PBM_FILE_ADN1:
    case PBM_FILE_EMAIL1:
    case PBM_FILE_SNE1:
    case PBM_FILE_GRP1:
    case PBM_FILE_ANR1:
    case PBM_FILE_ANRA1:
    case PBM_FILE_ANRB1:
    case PBM_FILE_ANRC1:
    case PBM_FILE_IAP1:
    case PBM_FILE_UID1:
    case PBM_FILE_PBC1:
    case PBM_FILE_ADN2:
    case PBM_FILE_EMAIL2:
    case PBM_FILE_SNE2:
    case PBM_FILE_GRP2:
    case PBM_FILE_ANR2:
    case PBM_FILE_ANRA2:
    case PBM_FILE_ANRB2:
    case PBM_FILE_ANRC2:
    case PBM_FILE_IAP2:
    case PBM_FILE_UID2:
    case PBM_FILE_PBC2:
    case PBM_FILE_ADN3:
    case PBM_FILE_EMAIL3:
    case PBM_FILE_SNE3:
    case PBM_FILE_GRP3:
    case PBM_FILE_ANR3:
    case PBM_FILE_ANRA3:
    case PBM_FILE_ANRB3:
    case PBM_FILE_ANRC3:
    case PBM_FILE_IAP3:
    case PBM_FILE_UID3:
    case PBM_FILE_PBC3:
      return TRUE;
    default:
      return FALSE;
  }
}

/*=============================================================================
FUNCTION pbm_adn_caching_status

DESCRIPTION
  This function is used to find out ADN caching is diabled or not using NV 71535

DEPENDENCIES
  None

SIDE EFFECTS
  None.
===============================================================================*/
boolean pbm_adn_caching_status(pbm_session_enum_type session_type)
{
  uint8   slot_id;
  boolean ret = FALSE;

  slot_id = pbm_session_info[session_type].slot_id -1;
  if(slot_id < ((0)) && slot_id >= ((PBM_MAX_SLOTS)))
  {
    return ret;
  }

  do
  {
    if((pbm_features_status_list.pbm_adn_phonebook_caching_status[slot_id].gpb_caching_status == PBM_FEATURE_DISABLED && 
        pbm_session_type_to_pb_category(session_type) == PBM_GPB))
    {
      pbm_session_info[session_type].in_use_adn_pb = PBM_PB_GLOBAL_PBR_NOT_PRESENT;
      ret = TRUE;
      break;
    }

    if((pbm_features_status_list.pbm_adn_phonebook_caching_status[slot_id].lpb_gw_caching_status == PBM_FEATURE_DISABLED && 
       pbm_session_type_to_pb_category(session_type) == PBM_LPB && 
      (session_type == PBM_SESSION_LPB_GW || session_type == PBM_SESSION_LPB_GW_SECONDARY || 
       session_type == PBM_SESSION_LPB_GW_TERTIARY)))
    {
      pbm_session_info[session_type].in_use_adn_pb = PBM_PB_LOCAL_PBR_NOT_PRESENT;
      ret = TRUE;
      break;
    }

    if((pbm_features_status_list.pbm_adn_phonebook_caching_status[slot_id].lpb_1x_caching_status == PBM_FEATURE_DISABLED && 
       pbm_session_type_to_pb_category(session_type) == PBM_LPB && 
      (session_type == PBM_SESSION_LPB_1X || session_type == PBM_SESSION_LPB_1X_SECONDARY || 
       session_type == PBM_SESSION_LPB_1X_TERTIARY)))
    {
      pbm_session_info[session_type].in_use_adn_pb = PBM_PB_LOCAL_PBR_NOT_PRESENT;
      ret = TRUE;
      break;
    }
  }while(0);

  UIM_MSG_HIGH_2("pbm_adn_caching_status session_type 0x%x ret 0x%x", session_type, ret);
  return ret;
}

/*===========================================================================
FUNCTION PBM_SESSION_INIT_FILEINFO_REQUESTS

DESCRIPTION
  This function is used to initialize pbm_session_fileinfo_requests for
  the specified SIM filenames.

  Note that extension filenames are initialized if required and need not
  be specified separately.

DEPENDENCIES
  card_apps must be previously set.
  Not reentrant. This function should always be called from PBM task.

SIDE EFFECTS
  Globals describing the world according to this SIM are set.
===========================================================================*/
void pbm_init_fileinfo_requests (pbm_session_enum_type session_type,
                                        pbm_file_type file_id)
{
   /* Don't query file attributes if ADN and associated files caching
      is disabled using NV 71535 */
   if(pbm_adn_caching_status(session_type) == TRUE)
   {
     if(pbm_adn_caching_required_or_not(file_id))
     {
       return;
     }
   }
   
   switch (file_id)
   {
      case PBM_FILE_ADN:
         /* check for USIM support */
         if (USE_USIM_LOCAL_SUPPORT(session_type) || USE_CSIM_LOCAL_SUPPORT(session_type))
         {
            if(USE_USIM_LOCAL_SUPPORT(session_type))
            {
              pbm_fill_fileinfo_request(session_type,
                                         MMGSDI_USIM_SRV_LOCAL_PHONEBOOK, PBM_FILE_ADN);
              pbm_fill_fileinfo_request(session_type,
                                                            MMGSDI_NONE, PBM_FILE_EXT1);

              // Local USIM ADN1 (Phonebook set 2)
              pbm_fill_fileinfo_request(session_type,
                                            MMGSDI_USIM_SRV_LOCAL_PHONEBOOK, PBM_FILE_ADN1);

              /* Local USIM ADN2 (Phonebook set 3) */
              pbm_fill_fileinfo_request(session_type,
                                            MMGSDI_USIM_SRV_LOCAL_PHONEBOOK, PBM_FILE_ADN2);

              /* Local USIM ADN3 (Phonebook set 4) */
              pbm_fill_fileinfo_request(session_type,
                                            MMGSDI_USIM_SRV_LOCAL_PHONEBOOK, PBM_FILE_ADN3);

            }
            else if(USE_CSIM_LOCAL_SUPPORT(session_type))
            {
              pbm_fill_fileinfo_request(session_type,
                                         MMGSDI_CSIM_SRV_LOCAL_PHONEBOOK, PBM_FILE_ADN);
              pbm_fill_fileinfo_request(session_type,
                                        MMGSDI_NONE, PBM_FILE_EXT1);

              // Local USIM ADN1 (Phonebook set 2)
              pbm_fill_fileinfo_request(session_type,
                                        MMGSDI_CSIM_SRV_LOCAL_PHONEBOOK, PBM_FILE_ADN1);

              /* Local USIM ADN2 (Phonebook set 3) */
              pbm_fill_fileinfo_request(session_type,
                                        MMGSDI_CSIM_SRV_LOCAL_PHONEBOOK, PBM_FILE_ADN2);

              /* Local USIM ADN3 (Phonebook set 4) */
              pbm_fill_fileinfo_request(session_type,
                                        MMGSDI_CSIM_SRV_LOCAL_PHONEBOOK, PBM_FILE_ADN3);

            }

         }
         else if (USE_USIM_SUPPORT(session_type) || USE_CSIM_SUPPORT(session_type))
         {
            /* Global USIM Phonebook */
            pbm_fill_fileinfo_request(session_type, MMGSDI_NONE, PBM_FILE_ADN);
            pbm_fill_fileinfo_request(session_type,  MMGSDI_NONE, PBM_FILE_EXT1);

            /* Global USIM ADN1 (Phonebook set 2) */
            pbm_fill_fileinfo_request(session_type,  MMGSDI_NONE, PBM_FILE_ADN1);
            /* Global USIM ADN2 (Phonebook set 3) */
            pbm_fill_fileinfo_request(session_type, MMGSDI_NONE, PBM_FILE_ADN2);
            /* Global USIM ADN3 (Phonebook set 4) */
            pbm_fill_fileinfo_request(session_type, MMGSDI_NONE, PBM_FILE_ADN3);
         }
         else

         {
               if(!USE_GSM_SUPPORT(session_type) && !USE_CDMA_SUPPORT(session_type))
	       {
             pbm_fill_fileinfo_request(session_type,
               MMGSDI_NONE, PBM_FILE_ADN);
             pbm_fill_fileinfo_request(session_type,
               MMGSDI_NONE, PBM_FILE_EXT1);
         }
         }
         break;

         /* PBM_FILE_SDN */
      case PBM_FILE_SDN:
         if (USE_USIM_LOCAL_SUPPORT(session_type))
         {
            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_USIM_SRV_SDN,  PBM_FILE_SDN);

            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_USIM_SRV_EXT3, PBM_FILE_EXT3);
         }
         else if (USE_CSIM_LOCAL_SUPPORT(session_type))
         {
            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_CSIM_SRV_SDN,  PBM_FILE_SDN);

            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_CSIM_SRV_EXT3, PBM_FILE_EXT3);
         }
         else  if(!USE_GSM_SUPPORT(session_type) && !USE_CDMA_SUPPORT(session_type)
           && !USE_USIM_SUPPORT(session_type) &&  !USE_CSIM_SUPPORT(session_type)  )
         {
            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_NONE,  PBM_FILE_SDN);

            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_NONE, PBM_FILE_EXT3);
         }
         break;

      case PBM_FILE_MSISDN:
         if (USE_USIM_LOCAL_SUPPORT(session_type))
         {
            /* USIM uses EXT5 */
            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_USIM_SRV_MSISDN, PBM_FILE_MSISDN);

            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_USIM_SRV_EXT5, PBM_FILE_EXT5);
         }
         else if (USE_CSIM_LOCAL_SUPPORT(session_type))
         {
            /* CSIM uses EXT3 */
            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_NONE, PBM_FILE_MSISDN);

            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_CSIM_SRV_EXT3, PBM_FILE_EXT3);
         }
         else  if(USE_CDMA_SUPPORT(session_type))
         {
             pbm_fill_fileinfo_request(session_type, MMGSDI_NONE, PBM_FILE_MSISDN);
         }
         else  if(!USE_GSM_SUPPORT(session_type) && !USE_CDMA_SUPPORT(session_type)
            && !USE_USIM_SUPPORT(session_type) &&  !USE_CSIM_SUPPORT(session_type)  )
         {
            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_NONE, PBM_FILE_MSISDN);

            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_NONE, PBM_FILE_EXT3);
         }
         break;

      case PBM_FILE_GAS:
         //MBDN supported only in USIM & SIM
         if (USE_USIM_LOCAL_SUPPORT(session_type) || USE_CSIM_LOCAL_SUPPORT(session_type))
         {
            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_NONE, PBM_FILE_GAS);
         }
         else
         {
            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_NONE, PBM_FILE_GAS);
         }
         break;
      case PBM_FILE_AAS:         //MBDN supported only in USIM & SIM
         if (USE_USIM_LOCAL_SUPPORT(session_type) || USE_CSIM_LOCAL_SUPPORT(session_type))
         {
            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_NONE, PBM_FILE_AAS);
         }
         else
         {
            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_NONE, PBM_FILE_AAS);
         }
         break;
      case PBM_FILE_MBDN:
         //MBDN supported only in USIM & SIM
         if (USE_USIM_LOCAL_SUPPORT(session_type))
         {
            /* uses EXT6 */
            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_USIM_SRV_MDN, PBM_FILE_MBDN);

            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_USIM_SRV_MDN, PBM_FILE_EXT6);
         }
         else if (USE_GSM_SUPPORT(session_type))
         {  /* uses EXT6 */
            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_GSM_SRV_MDN, PBM_FILE_MBDN);

            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_GSM_SRV_MDN, PBM_FILE_EXT6);
         }
         break;

      case PBM_FILE_MBN:

         if (USE_USIM_LOCAL_SUPPORT(session_type))
         {
            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_NONE, PBM_FILE_MBN);
         } else if (USE_GSM_SUPPORT(session_type))
         {
            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_NONE, PBM_FILE_MBN);
         }
		 break;


      case PBM_FILE_FDN:
         if (USE_USIM_LOCAL_SUPPORT(session_type))
         {
            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_USIM_SRV_FDN, PBM_FILE_FDN);

            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_USIM_SRV_EXT2, PBM_FILE_EXT2);
         }
         else if (USE_CSIM_LOCAL_SUPPORT(session_type))
         {
            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_CSIM_SRV_FDN, PBM_FILE_FDN);

            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_CSIM_SRV_EXT2, PBM_FILE_EXT2);
         }
         else if(!USE_GSM_SUPPORT(session_type) && !USE_CDMA_SUPPORT(session_type)
             && !USE_USIM_SUPPORT(session_type) &&  !USE_CSIM_SUPPORT(session_type) )
         {           
           pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_NONE, PBM_FILE_FDN);

           pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_NONE, PBM_FILE_EXT2);
         }
         break;

      case PBM_FILE_LND:
         /* check for GPB only */
         if (pbm_session_type_to_pb_category(session_type) == PBM_GPB
            && !USE_USIM_SUPPORT(session_type) &&  !USE_CSIM_SUPPORT(session_type) )
         {
            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_NONE, PBM_FILE_EXT1);

            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_NONE, PBM_FILE_LND);
         }
         break;

      case PBM_FILE_OCI:
         if (USE_USIM_LOCAL_SUPPORT(session_type))
         {
            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_USIM_SRV_OCI_OCT, PBM_FILE_OCI);

            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_USIM_SRV_EXT5, PBM_FILE_EXT5);
         }
         else if (USE_CSIM_LOCAL_SUPPORT(session_type))
         {
            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_CSIM_SRV_OCI, PBM_FILE_OCI);

            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_CSIM_SRV_EXT5, PBM_FILE_EXT5);
         }
         break;

      case PBM_FILE_ICI:
         if (USE_USIM_LOCAL_SUPPORT(session_type))
         {
            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_USIM_SRV_ICI_ICT, PBM_FILE_ICI);

            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_USIM_SRV_EXT5, PBM_FILE_EXT5);
         }
         else if (USE_CSIM_LOCAL_SUPPORT(session_type))
         {
            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_CSIM_SRV_ICI, PBM_FILE_ICI);

            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_CSIM_SRV_EXT5, PBM_FILE_EXT5);
         }
         break;

      case PBM_FILE_EXT1:
         if (USE_USIM_LOCAL_SUPPORT(session_type))
         {
            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_NONE, PBM_FILE_EXT1);
         }
         else if(USE_CSIM_LOCAL_SUPPORT(session_type))
         {
            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_NONE, PBM_FILE_EXT1);
         }
         else  if(!USE_GSM_SUPPORT(session_type) && !USE_CDMA_SUPPORT(session_type))
         {
            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_NONE, PBM_FILE_EXT1);
         }

         break;

      case PBM_FILE_EXT2:
         if (USE_USIM_LOCAL_SUPPORT(session_type))
         {
            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_USIM_SRV_EXT2, PBM_FILE_EXT2);
         }
         else if (USE_CSIM_LOCAL_SUPPORT(session_type))
         {
            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_CSIM_SRV_EXT2, PBM_FILE_EXT2);
         } else  if(!USE_GSM_SUPPORT(session_type) && !USE_CDMA_SUPPORT(session_type)
            && !USE_USIM_SUPPORT(session_type) &&  !USE_CSIM_SUPPORT(session_type) )
         {
            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_NONE, PBM_FILE_EXT2);
         }
         break;
      case PBM_FILE_EXT5:
         if (USE_USIM_LOCAL_SUPPORT(session_type))
         {
            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_USIM_SRV_MDN, PBM_FILE_EXT5);
         }
         else if (USE_CSIM_LOCAL_SUPPORT(session_type))
         {
            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_NONE, PBM_FILE_EXT5);
         }
         break;

      case PBM_FILE_EXT6:
         if (USE_USIM_LOCAL_SUPPORT(session_type))
         {
            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_USIM_SRV_MDN, PBM_FILE_EXT6);
         } else if (USE_GSM_SUPPORT(session_type))
         {
            pbm_fill_fileinfo_request(session_type,
                                      MMGSDI_GSM_SRV_MDN, PBM_FILE_EXT6);
         }
         break;

      case PBM_FILE_PSC:
      case PBM_FILE_PUID:
      case PBM_FILE_CC:
      case PBM_FILE_EMAIL:
      case PBM_FILE_SNE:
      case PBM_FILE_GRP:
      case PBM_FILE_ANR:
      case PBM_FILE_ANRA:
      case PBM_FILE_ANRB:
      case PBM_FILE_ANRC:
      case PBM_FILE_IAP:
      case PBM_FILE_UID:
      case PBM_FILE_PBC:
      case PBM_FILE_ADN1:
      case PBM_FILE_EMAIL1:
      case PBM_FILE_SNE1:
      case PBM_FILE_GRP1:
      case PBM_FILE_ANR1:
      case PBM_FILE_ANRA1:
      case PBM_FILE_ANRB1:
      case PBM_FILE_ANRC1:
      case PBM_FILE_IAP1:
      case PBM_FILE_UID1:
      case PBM_FILE_PBC1:
      case PBM_FILE_ADN2:
      case PBM_FILE_EMAIL2:
      case PBM_FILE_SNE2:
      case PBM_FILE_GRP2:
      case PBM_FILE_ANR2:
      case PBM_FILE_ANRA2:
      case PBM_FILE_ANRB2:
      case PBM_FILE_ANRC2:
      case PBM_FILE_IAP2:
      case PBM_FILE_UID2:
      case PBM_FILE_PBC2:
      case PBM_FILE_ADN3:
      case PBM_FILE_EMAIL3:
      case PBM_FILE_SNE3:
      case PBM_FILE_GRP3:
      case PBM_FILE_ANR3:
      case PBM_FILE_ANRA3:
      case PBM_FILE_ANRB3:
      case PBM_FILE_ANRC3:
      case PBM_FILE_IAP3:
      case PBM_FILE_UID3:
      case PBM_FILE_PBC3:
      {
         pbm_file_type l_pbm_filename = PBM_FILE_NONE;
         uint8 j;

         for (j = 0; j < (int)PBM_FILE_MAX_REQUESTS; j++)
         {
            if (pbm_file_to_uim_file[j].pbm_filename == file_id)
            {
                if (USE_USIM_LOCAL_SUPPORT(session_type))
                {
                    l_pbm_filename = pbm_file_to_uim_file[j].pbm_session_filename[1];
                }
                else if (USE_CSIM_LOCAL_SUPPORT(session_type))
                {
                    l_pbm_filename = pbm_file_to_uim_file[j].pbm_session_filename[2];
                }
                else if (USE_USIM_SUPPORT(session_type) || USE_CSIM_SUPPORT(session_type))
                {
                    l_pbm_filename = pbm_file_to_uim_file[j].pbm_session_filename[0];
                }
                else
                {

                    UIM_MSG_ERR_2("File %d available only for USIM (sim %d)", file_id, session_type);
                }
                break;
            }

         }
         pbm_fill_fileinfo_request(session_type, MMGSDI_NONE,l_pbm_filename);
      }
      break;
      default:
         UIM_MSG_ERR_1("Unsupported init file_name %d", file_id);
         break;
   }
}

/*===========================================================================
FUNCTION PBM_START_FILEINFO_REQUEST

DESCRIPTION
   This function starts the initialization chain.
   It sends the first make file info request or reads USIM PBR if we are using
   a USIM.

DEPENDENCIES
  card_apps must be previously set.
  Assumes that pbm_fileinfo_requests have already been filled in.
  Not reentrant. This function should always be called from PBM task.

SIDE EFFECTS
  Globals describing the world according to this SIM are set.
===========================================================================*/
static void pbm_start_fileinfo_request(pbm_session_enum_type session_type)
{
   boolean bReadPBR;
   int i;
   boolean l_pbr_read_in_progress = FALSE;
   PBM_VALIDATE_SESSION_RET(session_type, VOID);

   //unless UST is off ..rest all cases we would need the PBR info
   if(pbm_session_info[session_type].in_use_adn_pb != PBM_PB_LOCAL_PBR_NOT_PRESENT)
   {
      /* Read USIM phonebook structure and then start initialization */
      bReadPBR = FALSE;

      if(pbm_session_info[session_type].pbm_refresh_init == PBM_REFRESH_ONLY)
      {
         for (i=0; i < pbm_session_info[session_type].pbm_refresh_num_file_devices; i++)
         {
            if (pbm_session_info[session_type].pbm_refresh_pbm_file_devices[i] == PBM_FILE_ADN)
            {
               bReadPBR = TRUE;
               break;
            }
         }
      }
      else if(pbm_session_activated & PBM_SESSIONID_TO_SESSION_MASK(session_type))
      {
        //this means we are neither refreshing nor reading the ECC
        bReadPBR = TRUE;
      }


      if(bReadPBR)
      {
        /* Don't query file attributes for PBR incase ADN cahing is disabled
            using NV 71535 item */
        if(pbm_adn_caching_status(session_type) == FALSE)
        {
          pbm_return_type ret;

          ret = pbm_usim_read_pbr(session_type);
          if (PBM_SUCCESS == ret)
          {
            l_pbr_read_in_progress = TRUE;
          }
        }

        UIM_MSG_HIGH_3("pbm_usim_read_pbr is not called for session_type %d in_use_adn_pb %d l_pbr_read_in_progress %d",
                        session_type, pbm_session_info[session_type].in_use_adn_pb,
                        l_pbr_read_in_progress);
      }
    }
   
   if(l_pbr_read_in_progress == FALSE)
   {
      /* Start initialization chain right away */
      pbm_make_fileinfo_request(session_type, 0);
   }
}


/*===========================================================================
FUNCTION PBM_INIT_FILEINFO_INTERNAL

DESCRIPTION
  This function will be called when MMGSDI gives us the app init completed
  event. If there is an initialization in progress for this app,
  that initialization is stopped and restarted from the beginning.

  Only one sequence of initializations can happen for each SESSION.

DEPENDENCIES
  Not reentrant. This function should always be called from PBM task.

SIDE EFFECTS
  Globals describing the world according to this SESSION are set.
===========================================================================*/
void pbm_init_fileinfo_internal (pbm_session_enum_type session_type)
{
   int i;
   PBM_VALIDATE_SESSION_RET(session_type, VOID);

   pbm_init_info.pbm_session_init_done[session_type] = FALSE;

   /* find the files that need refreshing */
   if (pbm_session_info[session_type].pbm_refresh_init == PBM_REFRESH_ONLY)
   {
      for (i=0; i<pbm_session_info[session_type].pbm_refresh_num_file_devices; i++)
      {
         pbm_init_fileinfo_requests(session_type, pbm_session_info[session_type].pbm_refresh_pbm_file_devices[i]);
      }
   }
   else
   {
      //in USIM/CSIM if it is an LPB that case we would need to read the ADN only UST bit is set.
      //GSM/CDMA there would be no ADN present in prov app
      if(pbm_session_info[session_type].in_use_adn_pb != PBM_PB_LOCAL_PBR_NOT_PRESENT
          || pbm_session_type_to_pb_category(session_type) != PBM_LPB)
      {
        pbm_init_fileinfo_requests(session_type, PBM_FILE_ADN);
      }
      else
      {
        adn_cache[session_type].status = PBM_STATUS_NOT_AVAILABLE;
      }
      pbm_init_fileinfo_requests(session_type, PBM_FILE_SDN);
      pbm_init_fileinfo_requests(session_type, PBM_FILE_MSISDN);
      pbm_init_fileinfo_requests(session_type, PBM_FILE_MBN);
      pbm_init_fileinfo_requests(session_type, PBM_FILE_FDN);
      pbm_init_fileinfo_requests(session_type, PBM_FILE_LND);
      pbm_init_fileinfo_requests(session_type, PBM_FILE_GAS);
      pbm_init_fileinfo_requests(session_type, PBM_FILE_AAS);
      pbm_init_fileinfo_requests(session_type, PBM_FILE_ICI);
      pbm_init_fileinfo_requests(session_type, PBM_FILE_OCI);
      pbm_init_fileinfo_requests(session_type, PBM_FILE_MBDN);

      }

   pbm_start_fileinfo_request(session_type);
}

/*===========================================================================
FUNCTION PBM_START_FILEINFO_INTERNAL

DESCRIPTION
  This function starts the initialization chain without re-initializing the
  pbm_fileinfo_requests array. The caller should set the appropriate
  element in pbm_fileinfo_requests before calling this function using
  pbm_init_fileinfo_requests().

  This is useful for reading only some of the files in the SESSION.

DEPENDENCIES
  Assumes that pbm_fileinfo_requests have already been filled in.
  Not reentrant. This function should always be called from PBM task.

SIDE EFFECTS
  Globals describing the world according to this SESSION are set.
===========================================================================*/
void pbm_start_fileinfo_internal (pbm_session_enum_type session_type)
{
   PBM_VALIDATE_SESSION_RET(session_type, VOID);

   /* If there is an initialization going on, continue this later */
   if (pbm_session_initializing & PBM_SESSIONID_TO_SESSION_MASK(session_type))
   {
      PBM_SESSION_SET(pbm_session_restart, session_type);
      return;
   }

   PBM_SESSION_SET(pbm_session_initializing, session_type);
   /* Start initialization chain */
   pbm_start_fileinfo_request(session_type);
}

/*===========================================================================
FUNCTION PBM_RESTART_FILEINFO_INTERNAL

DESCRIPTION
  This function is called from a GSDI callback to restart an initialization
  chain that is in limbo. An initialization chain is suspended when
  pbm_init_fileinfo_internal or pbm_start_fileinfo_internal is called
  while another initialization is in progress.

DEPENDENCIES
  Not reentrant. This function should always be called from PBM task.

SIDE EFFECTS
  Globals describing the world according to this SIM are set.
===========================================================================*/
void pbm_restart_fileinfo_internal (pbm_session_enum_type session_type)
{
   PBM_VALIDATE_SESSION_RET(session_type, VOID);

   PBM_SESSION_SET(pbm_session_initializing, session_type);
   PBM_SESSION_RESET(pbm_session_restart, session_type);
   pbm_session_info[session_type].isActive = TRUE;

   pbm_session_phonebook_init(session_type);
}

/*===========================================================================
FUNCTION PBM_STOP_FILEINFO_INTERNAL

DESCRIPTION
  This function is called to stop an initialization chain when SIM is
  removed.

DEPENDENCIES
  Not reentrant. This function should always be called from PBM task.

SIDE EFFECTS
  Globals describing the world according to this Session are set.
===========================================================================*/
void pbm_stop_fileinfo_internal(pbm_session_enum_type session_type)
{

   PBM_VALIDATE_SESSION_RET(session_type, VOID);

   PBM_SESSION_RESET(pbm_session_activated, session_type);

   PBM_SESSION_RESET(pbm_session_restart, session_type);
   PBM_SESSION_RESET(pbm_session_initializing, session_type);
   PBM_SESSION_RESET(pbm_session_subs_ready,session_type);
   PBM_SESSION_RESET(pbm_wms_init_finished,session_type);
   PBM_SESSION_ATOMIC_RESET(pbm_pin1_verified, session_type);
   PBM_SESSION_ATOMIC_RESET(pbm_pin2_verified, session_type);
   /* resetting the session_init_done */
   pbm_init_info.pbm_session_init_done[session_type] = FALSE;
   /* Clearing the tx_id */
   UIM_MSG_HIGH_3("PBM Clearing the tx_id for the session %d Tx-Id %d slot Id %d",
                      session_type,pbm_session_info[session_type].last_txid,
                      pbm_session_info[session_type].slot_id);
   pbm_session_info[session_type].last_txid = 0;

   //reset only refresh related data
   //since the remaining data will be applicable for this session
   pbm_session_info[session_type].in_use_adn_pb = PBM_PB_NOT_KNOWN;
   pbm_session_info[session_type].pbm_refresh_init = PBM_REFRESH_NONE;
   pbm_session_info[session_type].pbm_refresh_num_files = 0;
   pbm_session_info[session_type].pbm_refresh_num_file_devices= 0;
   memset(pbm_session_info[session_type].pbm_refresh_pbm_file_devices,
                        0x00,sizeof(pbm_session_info[session_type].pbm_refresh_pbm_file_devices));
   memset(pbm_session_info[session_type].pbm_refresh_file_list,
                        0x00,sizeof(pbm_session_info[session_type].pbm_refresh_file_list));

}

/*===========================================================================
FUNCTION PBM_FDN_ENABLE

DESCRIPTION
  This function is called is response to a MMGSDI_FDN_ENABLED event.
  It re-initializes FDN. If ADN is restricted when FDN is enabled for this
  Session, then ADN cache is also updated.

DEPENDENCIES
  Not reentrant. This function should always be called from PBM task.

SIDE EFFECTS
  Globals describing the world according to this Session are set.
===========================================================================*/
void pbm_fdn_enable (pbm_session_enum_type session_type)
{
   pbm_session_enum_type card_session=PBM_SESSION_GPB_1;
   PBM_VALIDATE_SESSION_RET(session_type, VOID);

   PBM_SESSION_SET(pbm_session_fdn_enabled, session_type);

   //   if (!(pbm_session_pin_waiting & PBM_SESSIONID_TO_SESSION_MASK(session_type))) {
   if(pbm_session_info[session_type].slot_id == (uint16)PBM_SLOT_1)
   {
       card_session = PBM_SESSION_GPB_1;
   }
   else if(pbm_session_info[session_type].slot_id == (uint16)PBM_SLOT_2)
   {
       card_session = PBM_SESSION_GPB_2;
   }
#ifdef FEATURE_TRIPLE_SIM
   else if(pbm_session_info[session_type].slot_id == (uint16)PBM_SLOT_3)
   {
       card_session = PBM_SESSION_GPB_3;
   }
#endif
   pbm_init_fileinfo_requests(session_type, PBM_FILE_ADN);
   pbm_init_fileinfo_requests(card_session, PBM_FILE_ADN);
   pbm_init_fileinfo_requests(session_type, PBM_FILE_FDN);
   pbm_init_fileinfo_requests(card_session, PBM_FILE_FDN);
   pbm_clear_and_set_session_caches(session_type, PBM_STATUS_NOT_READY, TRUE);
   pbm_clear_and_set_session_caches(card_session, PBM_STATUS_NOT_READY, TRUE);
   pbm_start_fileinfo_internal(session_type);
   pbm_start_fileinfo_internal(card_session);
}

/*===========================================================================
FUNCTION PBM_FDN_DISABLE

DESCRIPTION
  This function is called is response to a MMGSDI_FDN_DISABLED event.
  It re-initializes FDN and if ADN is readable now, it reinitalizes
  ADN too.

DEPENDENCIES
  Not reentrant. This function should always be called from PBM task.

SIDE EFFECTS
  Globals describing the world according to this Session are set.
===========================================================================*/
void pbm_fdn_disable(pbm_session_enum_type session_type)
{
   pbm_session_enum_type card_session = PBM_SESSION_GPB_1;
   PBM_VALIDATE_SESSION_RET(session_type, VOID);
   PBM_SESSION_RESET(pbm_session_fdn_enabled, session_type);

   //   if (!(pbm_session_pin_waiting & PBM_SESSIONID_TO_SESSION_MASK(session_type))) {
   if(pbm_session_info[session_type].slot_id == (uint16)PBM_SLOT_1)
   {
       card_session = PBM_SESSION_GPB_1;
   }
   else if(pbm_session_info[session_type].slot_id == (uint16)PBM_SLOT_2)
   {
       card_session = PBM_SESSION_GPB_2;
   }
#ifdef FEATURE_TRIPLE_SIM
   else if(pbm_session_info[session_type].slot_id == (uint16)PBM_SLOT_3)
   {
       card_session = PBM_SESSION_GPB_3;
   }
#endif
   pbm_init_fileinfo_requests(card_session, PBM_FILE_ADN);
   pbm_init_fileinfo_requests(session_type, PBM_FILE_ADN);
   pbm_init_fileinfo_requests(card_session, PBM_FILE_FDN);
   pbm_init_fileinfo_requests(session_type, PBM_FILE_FDN);
   pbm_clear_and_set_session_caches(card_session, PBM_STATUS_NOT_READY, TRUE);
   pbm_clear_and_set_session_caches(session_type, PBM_STATUS_NOT_READY, TRUE);
   pbm_start_fileinfo_internal(card_session);
   pbm_start_fileinfo_internal(session_type);
}

/*===========================================================================
FUNCTION PBM_FILL_PB_CACHE_INFO

DESCRIPTION
  This function is called to fill in the cache info for a PB cache
  when info comes back from MMGSDI

PARAMETERS

SIDE EFFECTS
  Globals describing the world according to this SESSION are set.
===========================================================================*/
void pbm_fill_pb_cache_info(PB_CACHE *cache,
                                   pbm_session_enum_type session_type,
                                   pbm_pb_set_type pb_set,
                                   int record_length,
                                   int min_record_length,
                                   pbm_file_type file_id,
                                   mmgsdi_file_structure_enum_type file_type,
                                   boolean increment_num_files,
                                   int num_records,
                                   const mmgsdi_file_security_type        *write)
{
   int i = 0;
   mmgsdi_pin_enum_type pin_type;
   PBM_VALIDATE_SESSION_RET(session_type,VOID);
   if ( ! pbm_validate_pb_set(pb_set) )
   {
      return ;
   }
   if (record_length >= min_record_length)
   {
      cache[session_type].file_ids[pb_set]  = file_id;
      cache[session_type].file_type         = file_type;

      if (increment_num_files)
      {
         cache[session_type].num_files++;
         cache[session_type].num_of_records += num_records;
      } else
      {
         cache[session_type].num_files = 1;
         cache[session_type].num_of_records = num_records;
      }

      if(pb_set != PBM_USIM_PB_INVALID)
      {
        cache[session_type].record_length[pb_set] = record_length;
        cache[session_type].records_in_pb_set[pb_set] = num_records;
        if (cache[session_type].pb_id.device_type != PBM_GRPNAME && cache[session_type].pb_id.device_type != PBM_AAS)
        {
          cache[session_type].text_length[pb_set] = record_length - min_record_length;
        } 
        else
        {
          cache[session_type].text_length[pb_set] = record_length;
        }
      }
      
      if (cache[session_type].num_of_records > 0)
      {
         cache[session_type].status = PBM_STATUS_NOT_READY;
      }
      else
         PBM_SET_PB_CACHE_READY(&cache[session_type]);

	  /* add write protection related status here */
	  if(write != NULL)
	  {
	     cache[session_type].write.protection_method = write->protection_method;
		 cache[session_type].write.num_protection_pin = write->num_protection_pin;
		 for(i=0; i < write->num_protection_pin; i++)
		 {
		    pin_type = write->protection_pin_ptr[i];
			if(pin_type == MMGSDI_PIN1)
			{
			   cache[session_type].write.pin_protection_mask |= (1 << PBM_PIN_PIN1);
			}
			else if(pin_type == MMGSDI_PIN2)
			{
			   cache[session_type].write.pin_protection_mask |= (1 << PBM_PIN_PIN2);
			}
			else if(pin_type == MMGSDI_ADM1)
			{
			   cache[session_type].write.pin_protection_mask |= (1 << PBM_PIN_ADM);
			} /* need to check what adm pin id to be used, currently using ADM1*/		
		 }
	  }
   } else
   {
      UIM_MSG_ERR_3("Bad data received from SIM 0x%x 0x%x 0x%x",
                  session_type,
                  file_id,
                  record_length);
      cache[session_type].status = PBM_STATUS_NOT_AVAILABLE;
   }
}

/*===========================================================================
FUNCTION PBM_FILL_PB_EXTCACHE_INFO

DESCRIPTION
  This function is called to fill in the cache info for a PB ext cache
  when info comes back from GSDI

PARAMETERS

SIDE EFFECTS
  Globals describing the world according to this SIM are set.
===========================================================================*/
static boolean pbm_fill_pb_extcache_info (
                                         pbm_ext_cache_s_type *pExtCache,
                                         pbm_file_type file_id,
                                         mmgsdi_file_structure_enum_type file_type,
                                         int num_records,
                                         int record_length,
                                         pbm_session_enum_type session_type
                                         )
{
   if (record_length == EXT_KNOWN_REC_LEN)
   {
      pExtCache->file_id        = file_id;
      pExtCache->file_type      = file_type;
      pExtCache->num_of_rec     = num_records;
      pExtCache->ext_rec_length = record_length;
      return TRUE;
   } else
   {
      UIM_MSG_ERR_3("Bad EXT data received from SESSION 0x%x 0x%x 0x%x",
                  session_type,
                  file_id,
                  record_length);
      return FALSE;
   }
}

/*===========================================================================
FUNCTION PBM_FILL_PB_SYNC1CACHE_INFO

DESCRIPTION
  This function is called to fill in the cache info for a PB sync1 cache
  when info comes back from GSDI

PARAMETERS

SIDE EFFECTS
  Globals describing the world according to this SIM are set.
===========================================================================*/
static void pbm_fill_pb_sync1cache_info(PB_SYNC1_CACHE *pSync1Cache,
                                        pbm_file_type file_id,
                                        mmgsdi_file_structure_enum_type file_type,
                                        int num_records,
                                        int record_length,
                                        pbm_session_enum_type session_type)
{
   if (file_type == MMGSDI_TRANSPARENT_FILE)
   {
      pSync1Cache->file_id   = file_id;
      pSync1Cache->file_type = file_type;
      /* GSDI does not provide number of records and record length for
       * transparent files */
      pSync1Cache->num_of_records = num_records;
      pSync1Cache->record_length  = record_length;
   } else
   {
      UIM_MSG_ERR_3("Bad sync1 cache data received from SIM 0x%x 0x%x 0x%x", session_type,
                  file_id, file_type);
   }
}

/*===========================================================================
FUNCTION PBM_FILL_PB_SYNC2CACHE_INFO

DESCRIPTION
  This function is called to fill in the cache info for a PB sync2 cache
  when info comes back from GSDI

PARAMETERS

SIDE EFFECTS
  Globals describing the world according to this SIM are set.
===========================================================================*/
static void pbm_fill_pb_sync2cache_info(PB_SYNC2_CACHE *pSync2Cache,
                                        pbm_file_type file_id,
                                        mmgsdi_file_structure_enum_type file_type,
                                        int num_records,
                                        int record_length,
                                        pbm_session_enum_type session_type)
{
   pbm_pb_set_type pb_set;
   pb_set = pbm_file_id_to_pb_set(file_id);
   if ( ! pbm_validate_pb_set(pb_set) )
   {
      return ;
   }

   if (record_length)
   {
      if (pSync2Cache->record_length != record_length)
      {
         UIM_MSG_ERR_3("Record length of Sync2 cache & PB %d doesnot match %d %d",
                     pb_set,
                     pSync2Cache->record_length,
                     record_length);
         return;
      }
      pSync2Cache->file_ids[pb_set] = file_id;
      pSync2Cache->file_type = file_type;
      pSync2Cache->num_of_records += num_records;
      pSync2Cache->records_in_pb_set[pb_set] = num_records;
      pSync2Cache->num_files++;
   } else
   {
      UIM_MSG_ERR_3("Bad Sync2 data received from SIM 0x%x 0x%x 0x%x", session_type,
                  file_id, record_length);
   }
}

/*===========================================================================
FUNCTION pbm_notify_pb_cache_status_update

DESCRIPTION
  This function is called to notify the qmi pbm to clear the valid_pb_mask
  when particular PB cahce status is restricted or not available.

PARAMETERS
  PB_CACHE cache
SIDE EFFECTS
  None.
===========================================================================*/
void pbm_notify_pb_cache_status_update(PB_CACHE *cache)
{
  if (((cache)->pb_id).device_type != PBM_DEFAULT)
  {
    pbm_session_event_data_u_type evt_data = {0};

    evt_data.pb_id  = (cache)->pb_id;
    UIM_MSG_ERR_1("PBM cache status update for %d",((cache)->pb_id).device_type);
    pbm_add_notify(PBM_EVENT_CACHE_STATUS_UPDATE, evt_data);
  }
}

/*===========================================================================
FUNCTION PBM_FILEINFO_CB

DESCRIPTION
  This function is called by GSDI when it has the results of our last
  get_file_attributes request.   It takes the request, looks at the
  client_ref to determine for which type of file the request was made,
  and stores the results.  It then calls the make_request function to
  get the next set of file attributes.  This way, this are never more
  than one PBM request to GSDI outstanding.

PARAMETERS

SIDE EFFECTS
  Globals describing the world according to this SIM are set.
===========================================================================*/
void pbm_fileinfo_cb(mmgsdi_return_enum_type status,
                     mmgsdi_cnf_enum_type    data_type,
                     const mmgsdi_cnf_type  *data_ptr)
{
   pbm_session_enum_type session_type;
   pbm_file_type file_id,file_id_in_same_pbset;
   uint32 client_ref,i; 
   int known_rec_len = 0;

   PB_USIM_CACHE *usim_cache;
   PB_MASTER_USIM_CACHE *m_usim_cache;

   pbm_pb_set_type pb_set;
   mmgsdi_file_structure_enum_type file_type;
   const mmgsdi_file_security_type        *write = NULL; /* for write security */
   int number_of_records = 0;
   int record_length = 0;
   PBM_CHECK_PTR_RET(data_ptr, VOID);

   if (MMGSDI_GET_FILE_ATTR_CNF != data_type)
   {
      UIM_MSG_ERR_1("invalid confirmation type %d in pbm_fileinfo_cb",
                  data_type);
      return;
   }

   client_ref = data_ptr->response_header.client_data;
   session_type = pbm_mmgsdi_session_to_pbm_session(
                                                   data_ptr->response_header.session_id
                                                   );
   PBM_VALIDATE_SESSION_RET(session_type, VOID);

   if (client_ref >= ARR_SIZE(pbm_fileinfo_requests))
   {
      UIM_MSG_ERR_1("Invalid client_ref found: %d\n", client_ref);
      return;
   }

   file_id = (pbm_file_type) (client_ref % (int)PBM_FILE_MAX_REQUESTS);

   // Restart initialization if required */
   if (pbm_session_restart & PBM_SESSIONID_TO_SESSION_MASK(session_type))
   {
      UIM_MSG_HIGH_1("PBM restarting init of session_type %d", session_type);
      pbm_restart_fileinfo_internal(session_type);
      return;
   }

   // Stop initialization if required
   if (!(pbm_session_initializing & PBM_SESSIONID_TO_SESSION_MASK(session_type)))
   {
      UIM_MSG_HIGH_1("PBM stopping init of SIM %d", session_type);
      return;
   }
   if (MMGSDI_SUCCESS == status)
   {
      file_type = data_ptr->get_file_attr_cnf.file_attrib.file_type;
      switch (file_type)
      {
         case MMGSDI_LINEAR_FIXED_FILE:
            number_of_records = data_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.num_of_rec;
            record_length     = data_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.rec_len;
			/* prepare data for write status here */
			write = &data_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.file_security.write;
            break;

         case MMGSDI_CYCLIC_FILE:
            number_of_records = data_ptr->get_file_attr_cnf.file_attrib.file_info.cyclic_file.num_of_rec;
            record_length     = data_ptr->get_file_attr_cnf.file_attrib.file_info.cyclic_file.rec_len;
			/* prepare data for write status here */
			write = &data_ptr->get_file_attr_cnf.file_attrib.file_info.cyclic_file.file_security.write;
            break;
         default:
            number_of_records = 0;
            record_length = 0;
            break;
      }

      //If we're a transparent file, or if we have records
      if ((MMGSDI_TRANSPARENT_FILE == file_type) || (number_of_records > 0))
      {
         switch (file_id)
         {
            case PBM_FILE_ADN:
            case PBM_FILE_ADN1:
            case PBM_FILE_ADN2:
            case PBM_FILE_ADN3:
               pb_set = pbm_file_id_to_pb_set(file_id);
               if ( ! pbm_validate_pb_set(pb_set) )
               {
                    adn_cache[session_type].status = PBM_STATUS_NOT_AVAILABLE ;
                    break;
               } 

               pbm_fill_pb_cache_info(adn_cache,
                                      session_type,
                                      pb_set,
                                      record_length,
                                      ADN_KNOWN_REC_LEN,
                                      file_id,
                                      file_type,
                                      (file_id == PBM_FILE_ADN) ? FALSE : TRUE,
                                      number_of_records,
                                      write);

               adn_cache[session_type].m_usim_cache[pb_set] =
               &adn_m_usim_cache[session_type][pb_set];
               UIM_MSG_HIGH_4(" PBM:found 0x%x records_in_pb_set 0x%x file_invalidated 0x%x, read_write_when_invalidated 0x%x",
                                adn_cache[session_type].records_in_pb_set[pb_set],
                                pb_set,
                                data_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.file_status.file_invalidated,
                                data_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.file_status.read_write_when_invalidated);
               if ( (data_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.file_status.file_invalidated) &&
                   (!data_ptr->get_file_attr_cnf.file_attrib.file_info.linear_fixed_file.file_status.read_write_when_invalidated) )
                        adn_cache[session_type].status = PBM_STATUS_RESTRICTED;

               if(adn_cache[session_type].status == PBM_STATUS_RESTRICTED)
               {
                 /* Notifying ADN Restricted status to QMI PBM to clear the ADN/FDN bitmask */
                 pbm_notify_pb_cache_status_update(&adn_cache[session_type]);
               }

               break;
            case PBM_FILE_SDN:
               pbm_fill_pb_cache_info(sdn_cache,
                                      session_type,
                                      PBM_USIM_PB_ONE,
                                      record_length,
                                      ADN_KNOWN_REC_LEN,
                                      file_id,
                                      file_type,
                                      FALSE,
                                      number_of_records,
                                      write);
               break;

            case PBM_FILE_MSISDN:
               if(USE_CDMA_SUPPORT(session_type) || USE_CSIM_LOCAL_SUPPORT(session_type))
               {
                 known_rec_len = MDN_1X_KNOWN_REC_LEN;                 
               }
               else
               {
                 known_rec_len = ADN_KNOWN_REC_LEN;                 
               }
               
               pbm_fill_pb_cache_info(msisdn_cache,
                                      session_type,
                                      PBM_USIM_PB_ONE,
                                      record_length,
                                      known_rec_len,
                                      file_id,
                                      file_type,
                                      FALSE,
                                      number_of_records,
                                      write);
               break;

            case PBM_FILE_MBN:
               pbm_fill_pb_cache_info(mbn_cache,
                                      session_type,
                                      PBM_USIM_PB_ONE,
                                      record_length,
                                      ADN_KNOWN_REC_LEN,
                                      file_id,
                                      file_type,
                                      FALSE,
                                      number_of_records,
                                      write);
               break;

            case PBM_FILE_FDN:
               pbm_fill_pb_cache_info(fdn_cache,
                                      session_type,
                                      PBM_USIM_PB_ONE,
                                      record_length,
                                      FDN_KNOWN_REC_LEN,
                                      file_id,
                                      file_type,
                                      FALSE,
                                      number_of_records,
                                      write);
               break;

            case PBM_FILE_LND:
               pbm_fill_pb_cache_info(lnd_cache,
                                      session_type,
                                      PBM_USIM_PB_ONE,
                                      record_length,
                                      LND_KNOWN_REC_LEN,
                                      file_id,
                                      file_type,
                                      FALSE,
                                      number_of_records,
                                      write);
               break;


            case PBM_FILE_GAS:
               {
                 pbm_g_other_field_info_s_type *field_info = &g_sim_field_info[session_type][2];
                 pbm_fill_pb_cache_info(grpname_cache,
                                      session_type,
                                      PBM_USIM_PB_ONE,
                                      record_length,
                                        1,
                                      file_id,
                                      file_type,
                                      FALSE,
                                        number_of_records,
                                        write);
                 field_info->info[0].num_items = 1;
                 field_info->info[0].max_length = record_length;
               }
               break;
            case PBM_FILE_AAS:
               {
                 pbm_g_other_field_info_s_type *field_info = &g_sim_field_info[session_type][3];
                 pbm_fill_pb_cache_info(aas_cache,
                                        session_type,
                                        PBM_USIM_PB_ONE,
                                        record_length,
                                        1,
                                        file_id,
                                        file_type,
                                        FALSE,
                                        number_of_records,
                                        write);
                 field_info->info[0].num_items = 1;
                 field_info->info[0].max_length = record_length;
               }
               break;
            case PBM_FILE_OCI:
               pbm_fill_pb_cache_info(oci_cache,
                                      session_type,
                                      PBM_USIM_PB_ONE,
                                      record_length,
                                      OCI_KNOWN_REC_LEN,
                                      file_id,
                                      file_type,
                                      FALSE,
                                      number_of_records,
                                      write);
               break;

            case PBM_FILE_ICI:
               pbm_fill_pb_cache_info(ici_cache,
                                      session_type,
                                      PBM_USIM_PB_ONE,
                                      record_length,
                                      ICI_KNOWN_REC_LEN,
                                      file_id,
                                      file_type,
                                      FALSE,
                                      number_of_records,
                                      write);
               break;

            case PBM_FILE_MBDN:
               pbm_fill_pb_cache_info(mbdn_cache,
                                      session_type,
                                      PBM_USIM_PB_ONE,
                                      record_length,
                                      ADN_KNOWN_REC_LEN,
                                      file_id,
                                      file_type,
                                      FALSE,
                                      number_of_records,
                                      write);
               break;

            case PBM_FILE_EXT1:
               if (pbm_fill_pb_extcache_info(
                                            &ext1_cache[session_type], file_id, file_type,
                                            number_of_records, record_length, session_type))
               {
                  adn_cache[session_type].extension_cache = &ext1_cache[session_type];
                  lnd_cache[session_type].extension_cache = &ext1_cache[session_type];
                  mbn_cache[session_type].extension_cache = &ext1_cache[session_type];
                  if (!USE_USIM_SUPPORT(session_type))
                     msisdn_cache[session_type].extension_cache =
                     &ext1_cache[session_type];
               }
               break;

            case PBM_FILE_EXT2:
               if (pbm_fill_pb_extcache_info(&ext2_cache[session_type],
                                             file_id, file_type,
                                             number_of_records, record_length, session_type))
               {
                  fdn_cache[session_type].extension_cache = &ext2_cache[session_type];
               }
               break;

            case PBM_FILE_EXT3:
               if (pbm_fill_pb_extcache_info(&ext3_cache[session_type],
                                             file_id, file_type,
                                             number_of_records, record_length, session_type))
               {
                  sdn_cache[session_type].extension_cache = &ext3_cache[session_type];
               }
               break;

            case PBM_FILE_EXT5:
               if (pbm_fill_pb_extcache_info(&ext5_cache[session_type], file_id,
                                             file_type,
                                             number_of_records, record_length, session_type))
               {
                  ici_cache[session_type].extension_cache = &ext5_cache[session_type];
                  oci_cache[session_type].extension_cache = &ext5_cache[session_type];
                  if (USE_USIM_SUPPORT(session_type)) /* For USIM */
                     msisdn_cache[session_type].extension_cache =
                     &ext5_cache[session_type];
               }
               break;

            case PBM_FILE_EXT6:
               if (pbm_fill_pb_extcache_info(&ext6_cache[session_type], file_id,
                                             file_type,
                                             number_of_records, record_length, session_type))
               {
                  if (USE_USIM_SUPPORT(session_type)) /* For USIM */
                     mbdn_cache[session_type].extension_cache =
                     &ext6_cache[session_type];
               }
               break;

            case PBM_FILE_EMAIL:
            case PBM_FILE_EMAIL1:
            case PBM_FILE_EMAIL2:
            case PBM_FILE_EMAIL3:
            case PBM_FILE_SNE:
            case PBM_FILE_SNE1:
            case PBM_FILE_SNE2:
            case PBM_FILE_SNE3:
            case PBM_FILE_GRP:
            case PBM_FILE_GRP1:
            case PBM_FILE_GRP2:
            case PBM_FILE_GRP3:
            case PBM_FILE_ANR:
            case PBM_FILE_ANRA:
            case PBM_FILE_ANRB:
            case PBM_FILE_ANRC:
            case PBM_FILE_ANR1:
            case PBM_FILE_ANRA1:
            case PBM_FILE_ANRB1:
            case PBM_FILE_ANRC1:
            case PBM_FILE_ANR2:
            case PBM_FILE_ANRA2:
            case PBM_FILE_ANRB2:
            case PBM_FILE_ANRC2:
            case PBM_FILE_ANR3:
            case PBM_FILE_ANRA3:
            case PBM_FILE_ANRB3:
            case PBM_FILE_ANRC3:
               /* TODO: record length check */
               if (record_length)
               {
                  pb_set = pbm_file_id_to_pb_set(file_id);
                  if ( ! pbm_validate_pb_set(pb_set) )
                  {
                     break;
                  }
                  usim_cache = pbm_file_id_to_cache(session_type, file_id);

                  if (NULL == usim_cache)
                  {
                     UIM_MSG_ERR_0("NULL usim_cache");
                     break;
                  }

                  usim_cache->file_id = file_id;
                  usim_cache->file_type = file_type;
                  usim_cache->record_length = record_length;
                  usim_cache->num_of_records = number_of_records;

                  if (usim_cache->mapping_type != PBM_MAPPING_TYPE_2)
                     adn_m_usim_cache[session_type][pb_set].num_caches++;
               } else
               {
                  UIM_MSG_ERR_3("Bad data received from SESSION 0x%x 0x%x 0x%x",
                              session_type,
                              pbm_fileinfo_requests[client_ref].file_id,
                              record_length);
               }
               break;

            case PBM_FILE_IAP:
               pb_set = pbm_file_id_to_pb_set(file_id);
               if ( ! pbm_validate_pb_set(pb_set) )
               {
                  break;
               }

               if (record_length)
               {
                  m_usim_cache = &adn_m_usim_cache[session_type][pb_set];
                  m_usim_cache->file_id = file_id;
                  m_usim_cache->file_type = file_type;
                  m_usim_cache->iap_rec_len = record_length;
                  m_usim_cache->iap_num_records = number_of_records;
                  m_usim_cache->num_caches += record_length;
               } else
               {
                  UIM_MSG_ERR_3("Bad IAP data received from SIM 0x%x 0x%x 0x%x",
                              session_type, pbm_fileinfo_requests[client_ref].file_id,
                              record_length);
               }
               break;

            case PBM_FILE_IAP1:
            case PBM_FILE_IAP2:
            case PBM_FILE_IAP3:
               pb_set = pbm_file_id_to_pb_set(file_id);
               if ( ! pbm_validate_pb_set(pb_set) )
               {
                  break;
               }

               if (record_length)
               {
                  m_usim_cache = &adn_m_usim_cache[session_type][pb_set];
                  if (adn_m_usim_cache[session_type][PBM_USIM_PB_ONE].iap_rec_len !=
                      record_length)
                  {
                     UIM_MSG_ERR_3("Rec len of IAP and IAP %d does not match %d %d",
                                 pb_set,
                                 adn_m_usim_cache[session_type][PBM_USIM_PB_ONE].iap_rec_len,
                                 record_length);
                     break;
                  }
                  m_usim_cache->file_id = file_id;
                  m_usim_cache->file_type = file_type;
                  m_usim_cache->iap_rec_len = record_length;
                  m_usim_cache->iap_num_records = number_of_records;
                  m_usim_cache->num_caches += record_length;
               } else
               {
                  UIM_MSG_ERR_3("Bad IAP-n data received from 0x%x 0x%x 0x%x",
                              session_type,
                              pbm_fileinfo_requests[client_ref].file_id,
                              record_length);
               }
               break;

            case PBM_FILE_PSC:
               pbm_fill_pb_sync1cache_info(&psc_cache[session_type], file_id,
                                           file_type,
                                           1, 4, session_type);
               break;

            case PBM_FILE_PUID:
               pbm_fill_pb_sync1cache_info(&puid_cache[session_type], file_id,
                                           file_type, 1, 2, session_type);
               break;

            case PBM_FILE_CC:
               pbm_fill_pb_sync1cache_info(&cc_cache[session_type], file_id,
                                           file_type, 1, 2, session_type);
               break;

            case PBM_FILE_PBC:
               if (record_length)
               {
                  pbc_cache[session_type].file_ids[PBM_USIM_PB_ONE] = file_id;
                  pbc_cache[session_type].file_type = file_type;
                  pbc_cache[session_type].num_of_records = number_of_records;
                  pbc_cache[session_type].records_in_pb_set[PBM_USIM_PB_ONE] =
                  number_of_records;
                  pbc_cache[session_type].num_files = 1;
                  pbc_cache[session_type].record_length = record_length;
               } else
               {
                  UIM_MSG_ERR_3("Bad PBC data received from SIM 0x%x 0x%x 0x%x",
                              session_type, pbm_fileinfo_requests[client_ref].file_id,
                              record_length);
               }
               break;

            case PBM_FILE_PBC1:
            case PBM_FILE_PBC2:
            case PBM_FILE_PBC3:
               pbm_fill_pb_sync2cache_info(&pbc_cache[session_type], file_id,
                                           file_type, number_of_records, record_length, session_type);
               break;

            case PBM_FILE_UID:
               pb_set = pbm_file_id_to_pb_set(file_id);
               if ( ! pbm_validate_pb_set(pb_set) )
               {
                  break; 
               }

               if (record_length)
               {
                  uid_cache[session_type].file_ids[pb_set] = file_id;
                  uid_cache[session_type].file_type = file_type;
                  uid_cache[session_type].num_of_records = number_of_records;
                  uid_cache[session_type].record_length = record_length;
                  uid_cache[session_type].records_in_pb_set[pb_set] =
                  number_of_records;
                  uid_cache[session_type].num_files = 1;
               } else
               {
                  UIM_MSG_ERR_3("Bad UID data received from SIM 0x%x 0x%x 0x%x",
                              session_type,
                              pbm_fileinfo_requests[client_ref].file_id,
                              record_length);
               }
               break;

            case PBM_FILE_UID1:
            case PBM_FILE_UID2:
            case PBM_FILE_UID3:
               pbm_fill_pb_sync2cache_info(&uid_cache[session_type], file_id,
                                           file_type, number_of_records, record_length, session_type);
               break;

            default:
               UIM_MSG_ERR_2("Unexpected file attr received SESSION %d REQUEST %d!",
                           session_type, file_id);
               break;
         }
      }
   } else
   { /* (status != MMGSDI_SUCCESS) */
      void* cache;

      pbm_cache_type_e_type cache_type;

      switch (status)
      {
         case MMGSDI_NOT_FOUND:
            UIM_MSG_HIGH_2("GSDI error MMGSDI_NOT_FOUND session 0x%x pbm file %d",
                         session_type, file_id);
            break;

         case MMGSDI_ACCESS_DENIED:
            UIM_MSG_HIGH_2("Error MMGSDI_ACCESS_DENIED session 0x%x pbm file %d",
                         session_type, file_id);
            break;

         default:
            UIM_MSG_HIGH_3("GSDI error code 0x%x session 0x%x pbm file %d", status,
                         session_type, file_id);
            break;
      }

	  /* If the IAP entry is present in PBR and file is not found on card */
	  if(file_id == PBM_FILE_IAP || file_id == PBM_FILE_IAP1 || file_id == PBM_FILE_IAP2 || file_id == PBM_FILE_IAP3)
	  {
	      PB_USIM_CACHE   *usim_cache = NULL;
		  pbm_file_type    file_id_loop;
		  uint32           index;      /* Index into pbm_fileinfo_requests array */
	      pb_set = pbm_file_id_to_pb_set(file_id);
		  /* Setting do_request flag flase for the type-2 files of the same PB set,
		  * so that we won't prepare cache for type-2 files if IAP is not found on the card */
		  for (file_id_loop = 0; file_id_loop < (uint32)PBM_FILE_MAX_REQUESTS; file_id_loop++)
		  {
		      usim_cache = pbm_file_id_to_cache(session_type, file_id_loop);
			  if(usim_cache != NULL && (pb_set == pbm_file_id_to_pb_set(file_id_loop))
			  	  && usim_cache->mapping_type == PBM_MAPPING_TYPE_2)
			  {
		          index = file_id_loop + ((uint32)session_type * (uint32)PBM_FILE_MAX_REQUESTS);
				  pbm_fileinfo_requests[index].do_request = FALSE;
				  usim_cache->num_of_records = 0;
			  }
		  }
	  }

      cache = pbm_file_id_to_cache(session_type, file_id);
      cache_type = pbm_file_id_to_cache_type(file_id);

      /* PB caches are the only ones with a status field */
      if (PBM_CACHE_TYPE_PB == cache_type)
      {
         if (NULL != cache)
         {
            /* beware that multiple GSDI files map to the same cache in some
            * cases like ADN */
            if (0 == ((PB_CACHE*)cache)->num_of_records)
            {
               ((PB_CACHE*)cache)->status = PBM_STATUS_NOT_AVAILABLE;
            }
         } else
         {
            UIM_MSG_ERR_2("NULL cache ptr, sim id 0x%x file_id 0x%x", session_type,
                        file_id);
         }
      }
      if(file_id == PBM_FILE_ADN || file_id == PBM_FILE_ADN1
          || file_id == PBM_FILE_ADN2 || file_id == PBM_FILE_ADN3)
      {
        for (file_id_in_same_pbset = 0; 
             file_id_in_same_pbset < (uint32)PBM_FILE_MAX_REQUESTS; 
                      file_id_in_same_pbset++)
        {
          
          if (pbm_file_id_to_pb_set(file_id) == 
                      pbm_file_id_to_pb_set(file_id_in_same_pbset))
          {
              cache_type = pbm_file_id_to_cache_type(file_id_in_same_pbset);
              if(cache_type == PBM_CACHE_TYPE_USIM || 
                  cache_type == PBM_CACHE_TYPE_INDEX || 
                  cache_type == PBM_CACHE_TYPE_SYNC2 )
            
              {
                i = file_id_in_same_pbset + ((uint32)session_type * (uint32)PBM_FILE_MAX_REQUESTS);                             
                pbm_fileinfo_requests[i].do_request = FALSE;            
              }               
            }
          
   }

      }      
   }

   /* After we process the results, put the next request in. */
   pbm_make_fileinfo_request(session_type, (1 + (uint32) file_id));

} /*void pbm_fileinfo_cb(gsdi_cnf... */

/*===========================================================================
FUNCTION PBM_SESSION_INIT_COMPLETED

DESCRIPTION
  This function is called during boot after all the records have been read.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
static void pbm_session_init_completed(pbm_session_enum_type session_type)
{
   pbm_phonebook_type pb_id = {PBM_GPB};
   pbm_session_event_data_u_type evt_data = {0};
   pbm_extended_fileinfo_s_type info;
   pbm_g_other_field_info_s_type *field_info;
   uint8 i = 0;
   pbm_slot_type slot_id = PBM_SLOT_1;

   PBM_VALIDATE_SESSION_RET(session_type, VOID);

   PBM_MEM_FREEIF(uid_cache[session_type].data_array);


   if(pbm_session_extended_file_info (
           pbm_device_type_to_phonebook_type(session_type, PBM_FDN),&info)==PBM_SUCCESS)
   {
     field_info = &g_sim_field_info[session_type][1];
     for ( i = 0; i < field_info->num_fields; i++)
     {
       if (field_info->info[i].field_id == PBM_FIELD_NAME)
       {
         //add 2 for UCS2 NULL char
         field_info->info[i].max_length =
             2 + MIN(info.text_len,field_info->info[i].max_length);
       }
       //for telephone numbers need to consider if EXT1 record is free
       else if (field_info->info[i].field_type == PBM_FT_PHONE)
       {
         //add 2 for NULL and '+' sign
         field_info->info[i].max_length = 2 + info.number_len;
       }
     }
   }
   if(pbm_session_extended_file_info (
           pbm_device_type_to_phonebook_type(session_type, PBM_ADN),&info)==PBM_SUCCESS)
   {
     field_info = &g_sim_field_info[session_type][0];
     for ( i = 0; i < field_info->num_fields; i++)
     {
       if (field_info->info[i].field_id == PBM_FIELD_NAME)
       {
         field_info->info[i].max_length =
             2 + MIN(info.text_len,field_info->info[i].max_length);
       }
       else if (field_info->info[i].field_type == PBM_FT_PHONE)
       {
         //increment by 2 to accomodate '+' sign & NULL char
         field_info->info[i].max_length = 2 + info.number_len;
       }

     }
   }


   pb_id = pbm_device_type_to_phonebook_type(session_type, PBM_ADN);

   if (ds_ready_fn != NULL)
      (*ds_ready_fn)((check_pb_ready(pb_id) == PBM_SUCCESS)?TRUE:FALSE);

   pbm_mmgsdi_refresh_complete(session_type);

   pbm_init_info.pbm_session_init_done[session_type] = TRUE;

   /*check if all sessions are initialized*/
   evt_data.pb_id = pb_id;
   pbm_add_notify(PBM_EVENT_SESSION_INIT_DONE, evt_data);

   /* check if the sessions blonging to current slot id are initialised or not */
   /* if yes then we need to send the SIM_INIT_DONE event to clients */
   slot_id = (pbm_slot_type) pb_id.slot_id;
   if(TRUE == pbm_is_init_completed_for_slot(slot_id))
   {
      evt_data.sim_id = slot_id;
      pbm_add_notify(PBM_EVENT_SIM_INIT_DONE, evt_data);
   }

    UIM_MSG_HIGH_2("PBM Finished init for session %d on slot %d", session_type, pb_id.slot_id);
}

/*===========================================================================
FUNCTION PBM_SIM_READ_CB

DESCRIPTION
  This function is called during init by GSDI when a record is read in.
  It places the record into the linked list and if there is more to read
  it requests the next record to be read. Once we read the last record, it calls
  pbm_session_init_completed to say init is done.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
static void pbm_sim_read_cb(pbm_return_type pbm_returned,
                            pbm_sim_record_s_type *record)
{
   PB_CACHE                 *cache;
   pbm_phonebook_type       pb_id = {PBM_GPB};
   pbm_session_enum_type    session_type;
   int                      record_num = 0;
   pbm_pb_set_type          pb_set = PBM_USIM_PB_INVALID;

   PBM_CHECK_PTR_RET(record, VOID);
   session_type = record->session_type;
   PBM_VALIDATE_SESSION_RET(session_type, VOID);
   pb_set = pbm_file_id_to_pb_set(record->file_id);
   pb_id = pbm_file_id_to_pb_id(session_type, record->file_id);
   cache = pbm_file_id_to_cache(session_type, record->file_id);
   UIM_MSG_HIGH_3("pbm_sim_read_cb  sess %d  file %d index %d",session_type,record->file_id,record->index);
   if (NULL != cache)
   {
      if (PBM_SUCCESS == pbm_returned) /* If we can trust the data. */
      {
         if (pbm_cache_record_add(pb_id, &record->data.num_text, record->index))
         {
            /* we will keep on getting the next used record until we get the one eligible to be added in cache */
            record_num = pbm_get_next_used_record(session_type,
                                                  record->index);

            if ((record_num <= cache->num_of_records) &&
                ((pbm_returned = pbm_sim_read(record->session_type,
                                              record->file_id,
                                              record_num,
                                              pbm_sim_read_cb)) == PBM_SUCCESS))
               return;
         }

         if (record_num > cache->num_of_records)
         {
            /* check for the hidden key verified for this session type, if it is verified , merge the hidden cache in adn cache */
            //No need to do anything related to hidden key,when EF-Hiddenkey is not present
            //for that we will check for valid dir_index
            if (record->file_id == PBM_FILE_ADN && pbm_session_info[session_type].dir_index != 0)
            {
              pbm_g_other_field_info_s_type *field_info;
              uint8 field_index = 0;
              field_info = &g_sim_field_info[session_type][0];
              for ( field_index = 0; field_index < field_info->num_fields; field_index++)
              {
                if (field_info->info[field_index].field_id == PBM_FIELD_HIDDEN)
                {
                  field_info->info[field_index].num_items = 1;
                }
              }
              if(pbm_hidden_key_verified & PBM_SESSIONID_TO_SESSION_MASK(session_type))/* only ot be done when FDN is being read */
              {
                if (pbm_merge_hidden_contacts(session_type) == PBM_SUCCESS)	/* this will merge the hidden cache in the adn cache */
                {
                  pbm_hidden_key_notify(session_type,TRUE);  /* to send the event as contacts have been merged */
                }
              }
              else
              {
                //no need to unmerge hidden contacts to hidden-cache,
                //since they are already present in hidden cache
                pbm_hidden_key_notify(session_type,FALSE);  /* to send the event as contacts have been merged */
              }
            }
            else if(record->file_id == PBM_FILE_GAS)
            {
              g_sim_field_info[session_type][2].info[0].num_items = 1;
              /* add 2 for UCS2 NULL char */
              g_sim_field_info[session_type][2].info[0].max_length = cache->text_length[pb_set] + 2;
            }
            else if(record->file_id == PBM_FILE_AAS)
            {
    	      g_sim_field_info[session_type][3].info[0].num_items = 1;
              /* add 2 for UCS2 NULL char */
              g_sim_field_info[session_type][3].info[0].max_length = cache->text_length[pb_set] + 2;
            }
            if (cache->m_sync_cache &&
                cache->m_sync_cache->pbc_cache)
            {
               cache->status = PBM_STATUS_INIT_SYNC;
               pbm_reset_pbc(session_type, 1);
               return;
            } else
            {
               PBM_SET_PB_CACHE_READY(cache);
            }
         }
         else
         {
            UIM_MSG_ERR_3("Unable to read next record from SIM card, Error %d  \
               after %d of max %d", pbm_returned, record->index,
                        cache->num_of_records);

            /* This cache will be unusable, make sure its clear. */
            rex_enter_crit_sect(&pbm_crit_sect);
            pbm_clear_cache(cache);
            rex_leave_crit_sect(&pbm_crit_sect);
            cache->status = PBM_STATUS_NOT_AVAILABLE;
         }
      }
      else
      {
         UIM_MSG_ERR_3("Unable to read from SIM, Error %d after %d of max %d",
                     pbm_returned, cache->records_in_use, cache->num_of_records);

         /* This cache will be unusable, make sure its clear. */
         rex_enter_crit_sect(&pbm_crit_sect);
         pbm_clear_cache(cache);
         rex_leave_crit_sect(&pbm_crit_sect);
         cache->status = PBM_STATUS_NOT_AVAILABLE;
      }
   } else
   {
      UIM_MSG_ERR_1("Could not find cache for phonebook %d", pb_id.device_type);
   }

   /* Move to next file regardless of why we got here. */
   pbm_initialize_index[session_type]++;

   pbm_init_entries(session_type);
}

/*===========================================================================
FUNCTION PBM_INIT_ENTRIES

DESCRIPTION
  This function is called after PIN is verified and GSDI is ready for a flood
  of UI requests.

DEPENDENCIES
   pbm_init must be called first.

SIDE EFFECTS
===========================================================================*/
void pbm_init_entries(pbm_session_enum_type session_type)
{
   pbm_cache_type_e_type     cache_type;
   pbm_file_type             file_id;
   int                       i,q_id = 0;

   /*
   ** OK, Start reading the entries... asynchronously.
   */
   PBM_VALIDATE_SESSION_RET(session_type, VOID);
   while (pbm_initialize_index[session_type] < (int)ARR_SIZE(pbm_initialize_list))
   {
	  file_id = pbm_initialize_list[pbm_initialize_index[session_type]];
      cache_type = pbm_file_id_to_cache_type(file_id);

      /* don't do anything if this is a refresh, and this is not an affected
      * file (group) */
      if (pbm_session_info[session_type].pbm_refresh_init == PBM_REFRESH_ONLY)
      {
         for (i = 0; i < pbm_session_info[session_type].pbm_refresh_num_file_devices; i++)
         {
            if (pbm_session_info[session_type].pbm_refresh_pbm_file_devices[i] == file_id)
            {
               /* this is one we want to refresh.  Carry on. */
               break;
            }
         }
         if (i == pbm_session_info[session_type].pbm_refresh_num_file_devices)
         {
            /* this is not one we want to refresh. Move to next file. */
            UIM_MSG_HIGH_2("Skipping init of PBM file %d session %d",
                            pbm_initialize_index[session_type],session_type);
            pbm_initialize_index[session_type]++;
            continue;
         }
      }

      switch (cache_type)
      {
      case PBM_CACHE_TYPE_PB:
      {
        /* Read file stored in PB_CACHE */
        PB_CACHE *cache = (PB_CACHE *)pbm_file_id_to_cache(session_type,
                                                           file_id);
        if (NULL != cache)
        {
          if ((cache->status == PBM_STATUS_NOT_READY) &&
              (cache->num_of_records > 0))
          {
            if (PBM_SUCCESS == pbm_seek_unused_records(session_type, file_id))
                        return;
                  }
               }
               break;
            }
         case PBM_CACHE_TYPE_USIM:
            {
               /* Read file stored in USIM_CACHE */
               PB_USIM_CACHE *usim_cache =
               (PB_USIM_CACHE *)pbm_file_id_to_cache(session_type, file_id);

               if (usim_cache != NULL)
               {
                  if (usim_cache->num_of_records &&
                      adn_cache[session_type].num_of_records)
                  {
                     if (PBM_SUCCESS == pbm_seek_unused_records(session_type, file_id))
                        return;
                  }
               }
               break;
            }

         case PBM_CACHE_TYPE_INDEX:
            {
               PB_MASTER_USIM_CACHE *m_usim_cache =
               (PB_MASTER_USIM_CACHE *)pbm_file_id_to_cache(session_type,
                                                            file_id);

               if (m_usim_cache != NULL)
               {
                  if (m_usim_cache->iap_num_records &&
                      adn_cache[session_type].num_of_records)
                  {
                     if (PBM_SUCCESS == pbm_seek_unused_records(session_type, file_id))
                        return;
                  }
               }
               break;
            }

         case PBM_CACHE_TYPE_SYNC1:
            {
               PB_SYNC1_CACHE *sync1_cache =
               (PB_SYNC1_CACHE *) pbm_file_id_to_cache(session_type,
                                                       file_id);

               if (sync1_cache != NULL)
               {
                  if (sync1_cache->num_of_records &&
                      adn_cache[session_type].num_of_records)
                  {
                     /* Don't SEEK for unused records on transparent files */
                     if (PBM_SUCCESS == pbm_sync_read(session_type, file_id,
                                                      1, pbm_sync1_read_cb))
                        return;
                  }
               }
               break;
            }

         case PBM_CACHE_TYPE_SYNC2:
            {
               PB_SYNC2_CACHE *sync2_cache =
               (PB_SYNC2_CACHE *) pbm_file_id_to_cache(session_type,
                                                       file_id);

               if (sync2_cache != NULL)
               {
                  if (sync2_cache->num_of_records &&
                      adn_cache[session_type].num_of_records)
                  {
                     if (PBM_SUCCESS == pbm_seek_unused_records(session_type, file_id))
                        return;
                  }
               }
               break;
            }
         default:
            UIM_MSG_ERR_2("Unknown file id %d in sim %d", file_id, session_type);
            break;
      }

    /* Didn't initialize this file, move onto next. */
      pbm_initialize_index[session_type]++;
   }

   /* Memory allocated for the last file's SEEK data hasn't been freed */
   pbm_free_seek_data(session_type);
   /* reset the session init var..since init is completed */
   PBM_SESSION_RESET(pbm_session_initializing, session_type);

   /* check that if it is not in ECCreading ..then only session init is completed */
   if(pbm_session_activated & PBM_SESSIONID_TO_SESSION_MASK(session_type))
   {
     pbm_session_init_completed(session_type);
   }
   
   q_id = pbm_session_info[session_type].slot_id - 1 ;
   UIM_MSG_HIGH_2("PBM pbm_init_entries q_id %d pbm_session_type %d ",q_id,session_type);
   if( q_id >= 0 && q_id < PBM_MAX_SLOTS )
   {
     curr_q[q_id] = pbm_q_dequeue(q_id);
     if( curr_q[q_id] != PBM_SESSION_MAX )
     {
       UIM_MSG_HIGH_2("PBM starting next session for q_id %d sess %d ",q_id, curr_q[q_id]);
       if(pbm_session_type_to_pb_category(curr_q[q_id]) == PBM_GPB && 
          !pbm_session_info[curr_q[q_id]].isActive)
       {
         /* Setting back the isActive flag to TRUE If the dequeued session is a card session and
          * if it is not set already in case of multiapp card in single slot
          */
         pbm_session_info[curr_q[q_id]].isActive = TRUE;
       }

       pbm_session_phonebook_init( curr_q[q_id] );
     }
   }   
}

/*===========================================================================
FUNCTION PBM_RECORD_WRITE_TO_SIM

DESCRIPTION
  Adds a record to UIM. This is an internal function and should be called only
  from within PBM task.

DEPENDENCIES
  The caller should free write cmd structure in case of an error.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_record_write_to_sim (pbm_write_record_s_type *write)
{
   int buffer_index = -1;
   PB_CACHE *pb_cache;
   pbm_return_type ret;
   pbm_pb_set_type pb_set;

   PBM_CHECK_PTR_RET(write, PBM_ERROR);

   pb_cache = pbm_pb_id_to_cache(pbm_session_record_id_to_phonebook_type(write->rec_id));
   PBM_CHECK_PTR_RET(pb_cache, PBM_ERROR);

   pb_set = pbm_record_id_to_pb_set(write->rec_id);
   if ( ! pbm_validate_pb_set(pb_set) )
   {
  	  return PBM_ERROR; 
   }

   buffer_index = pbm_alloc_record();
   if (buffer_index < 0)
   {
      UIM_MSG_ERR_1("No pbm buffers %d", buffer_index);
      pbm_list_recover(write, FALSE);
      return PBM_ERROR;
   }

   pbm_buffers[buffer_index].write = write;
   pbm_buffers[buffer_index].state = PBM_WRITE_NOT_INIT;

   if (PBM_SUCCESS != (ret = pbm_uim_write((uint32)buffer_index)))
   {
      UIM_MSG_ERR_3("Unable to write record %d to UIM %d %d", write->rec_id, ret, buffer_index);
      pbm_list_recover(write, FALSE);
      pbm_free_record(buffer_index);
      return ret;
   }
   return PBM_SUCCESS;
}

void pbm_del_links_write_cb( mmgsdi_return_enum_type status,
                         mmgsdi_cnf_enum_type    data_type,
                         const mmgsdi_cnf_type  *data_ptr)                         
{
   uint32 client_ref;
   pbm_session_enum_type session_type;
   uint16  l_delete_link_index;
   PB_CACHE *cache;
   pbm_async_gsdi_buffer *buffer;
   pbm_return_type ret;
   pbm_phonebook_type pb_id;
   PBM_CHECK_PTR_RET(data_ptr, VOID);

   client_ref = data_ptr->response_header.client_data;

   if (data_type != MMGSDI_WRITE_CNF)
   {
      UIM_MSG_ERR_1("Invalid response type to a GSDI read. %d",
                  data_type);
      return;
   }

   if ((client_ref >= PBM_ASYNC_BUFFERS) ||
       (pbm_buffers[client_ref].in_use == FALSE))
   {
      UIM_MSG_ERR_1("Invalid client_ref found: %d\n", client_ref);
      return;
   }
   buffer = &pbm_buffers[client_ref];   
   pb_id = pbm_session_record_id_to_phonebook_type(buffer->write->rec_id);
   pb_id.device_type = PBM_ADN;
   cache = pbm_pb_id_to_cache(pb_id);
   PBM_CHECK_PTR_RET(cache ,VOID );

   session_type = pbm_mmgsdi_session_to_pbm_session(
                                                   data_ptr->response_header.session_id);

   if (pbm_buffers[client_ref].record.session_type != session_type)
   {
      UIM_MSG_ERR_3("Wrong callback called for file %d and session %d, %d",
                  pbm_buffers[client_ref].record.file_id,
                  pbm_buffers[client_ref].record.session_type, session_type);
      return;
   }

   // Restart initialization if required
   if (pbm_session_restart & PBM_SESSIONID_TO_SESSION_MASK(session_type))
   {
      UIM_MSG_HIGH_1("PBM restarting init of SIM %d", session_type);
      pbm_restart_fileinfo_internal(session_type);
      pbm_free_record((int) client_ref);
      return;
   }
   l_delete_link_index =  buffer->record.delete_link_index;
   /* protection for cache array */
   rex_enter_crit_sect(&pbm_crit_sect);
   if(cache->pb_cache_array[l_delete_link_index] != NULL)   
   {
     uint16 cnt;
     for(cnt = 0;cnt < cache->pb_cache_array[l_delete_link_index]->num_fields;cnt++)
     {
       pbm_field_s_type *fields = cache->pb_cache_array[l_delete_link_index]->fields;       
       if(fields[cnt].field_id == PBM_FIELD_ADDITIONAL_NUMBER_EXT)
       {
         if(fields[cnt].field_type == PBM_FT_BCD)
         {
           byte* anr_data_ptr;
           anr_data_ptr = (byte*)pbm_get_field_data_ptr(fields,cnt);
           //reset the additional number type in cache
           if( anr_data_ptr != NULL )
           {
             anr_data_ptr[0] = 0;
           }
         }
         else
         {
           UIM_MSG_ERR_3("invalid field type %d %d, %d",
                       fields[cnt].field_type,
                       l_delete_link_index, cnt);           
         }
       }
     }     
   }
   rex_leave_crit_sect(&pbm_crit_sect);
   if (pbm_uim_delete_links(client_ref,&ret,l_delete_link_index) || ret != PBM_SUCCESS)
      return;
   
   buffer->findex = 0;
   buffer->state = PBM_WRITE_LINKS_DELETED;
   pbm_uim_write(client_ref);
 
} /* pbm_mmgsdi_read_cb...*/

/*===========================================================================
FUNCTION PBM_DEL_LINKS_READ_CB

DESCRIPTION
  After MMGSDI has read any SIM/CSIM/USIM record this function is called.
  This function makes sure that the data sent by GSDI is sane.

  The appropriate pbm read callback is called based on the file type.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_del_links_read_cb( mmgsdi_return_enum_type status,
                         mmgsdi_cnf_enum_type    data_type,
                         const mmgsdi_cnf_type  *data_ptr)
{
  
  uint32 client_ref;
  pbm_session_enum_type session_type;
  mmgsdi_return_enum_type mmgsdi_retval = MMGSDI_ERROR;
  mmgsdi_write_data_type             write_data;
  memset(&write_data,0,sizeof(write_data));
  
  PBM_CHECK_PTR_RET(data_ptr, VOID);
  
  client_ref = data_ptr->response_header.client_data;
  if (data_type != MMGSDI_READ_CNF)
  {
     UIM_MSG_ERR_1("Invalid response type to a GSDI read. %d",
                 data_type);
     return;
  }
  
  if ((client_ref >= PBM_ASYNC_BUFFERS) ||
      (pbm_buffers[client_ref].in_use == FALSE))
{
     UIM_MSG_ERR_1("Invalid client_ref found: %d\n", client_ref);
   return;
}
  
  session_type = pbm_mmgsdi_session_to_pbm_session(
                                                  data_ptr->response_header.session_id);
  
  if (pbm_buffers[client_ref].record.session_type != session_type)
  {
     UIM_MSG_ERR_3("Wrong callback called for file %d and session %d, %d",
                 pbm_buffers[client_ref].record.file_id,
                 pbm_buffers[client_ref].record.session_type, session_type);
     return;
  }
  
  // Restart initialization if required
  if (pbm_session_restart & PBM_SESSIONID_TO_SESSION_MASK(session_type))
  {
     UIM_MSG_HIGH_1("PBM restarting init of SIM %d", session_type);
     pbm_restart_fileinfo_internal(session_type);
     pbm_free_record((int) client_ref);
     return;
  }
  write_data.data_len =   data_ptr->read_cnf.read_data.data_len;
  PBM_MEM_ALLOC(write_data.data_ptr,write_data.data_len); 
  PBM_CHECK_PTR_RET(write_data.data_ptr,VOID);
  if(write_data.data_ptr != NULL)
  {
    memscpy(write_data.data_ptr, write_data.data_len, data_ptr->read_cnf.read_data.data_ptr,write_data.data_len);
    //reset the anr-id
    write_data.data_ptr[0] = 0;
    pbm_buffers[client_ref].gsdi_resp_type = PBM_DEL_LINKS_WRITE_CB;
    
    mmgsdi_retval = mmgsdi_session_write_record (data_ptr->response_header.session_id,
                                                 data_ptr->read_cnf.access,
                                                 MMGSDI_LINEAR_FIXED_FILE,
                                                 data_ptr->read_cnf.accessed_rec_num,
                                                 write_data,
                                                 pbm_mmgsdi_async_cb,
                                                 client_ref);
  
  }
  PBM_MEM_FREEIF(write_data.data_ptr);
  if(mmgsdi_retval != MMGSDI_SUCCESS)
  {
    pbm_uim_write_cb(client_ref,PBM_ERROR);    
  }
      
  
}

/*===========================================================================
FUNCTION pbm_uim_delete_links

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_uim_delete_links(uint32 buffer_index, pbm_return_type *ret,uint16 adn_index)
{
   pbm_async_gsdi_buffer     *buffer;
   pbm_phonebook_type        pb_id;   
   PB_CACHE                  *cache;
   boolean                   pbm_ret = FALSE;

   PBM_CHECK_PTR_RET(ret, FALSE);
   *ret   = PBM_ERROR;
   buffer = &pbm_buffers[buffer_index];
   
   pb_id = pbm_session_record_id_to_phonebook_type(buffer->write->rec_id);
   pb_id.device_type = PBM_ADN;
   cache = pbm_pb_id_to_cache(pb_id);
   PBM_CHECK_PTR_RET(cache , FALSE);
   /* for cache arrays */
   rex_enter_crit_sect(&pbm_crit_sect);
   for(;adn_index <= cache->num_of_records;adn_index++)
   {
     if(cache->pb_cache_array[adn_index] != NULL)
     {     
       uint16 cnt;
       for(cnt = 0;cnt < cache->pb_cache_array[adn_index]->num_fields;cnt++)
       {
         pbm_field_s_type *fields = cache->pb_cache_array[adn_index]->fields;
         if(fields[cnt].field_id == PBM_FIELD_ADDITIONAL_NUMBER_EXT)
         {
           if(fields[cnt].field_type == PBM_FT_BCD)
           {                  
             byte* anr_data_ptr;
             anr_data_ptr = (byte*)pbm_get_field_data_ptr(fields,cnt);
                  
             if ( anr_data_ptr != NULL  && anr_data_ptr[0] == buffer->record.index)
             {                    
                if(PBM_SUCCESS == (*ret = pbm_uim_del_anrid(buffer_index,adn_index,&fields[cnt])))
                {                    
                   buffer->record.delete_link_index = adn_index;
                   pbm_ret = TRUE;
                   goto END_PBM_UIM_DEL_LINKS;
                 }         
                 else                      
                 {                    
                   pbm_ret = FALSE;
                   goto END_PBM_UIM_DEL_LINKS;
                 }         
             }         
           }
           else
           {
             UIM_MSG_ERR_3("invalid field type %d %d, %d",
                         fields[cnt].field_type,
                         adn_index, cnt);           
           }
         }
       }
     }
   }

   *ret = PBM_SUCCESS;
   END_PBM_UIM_DEL_LINKS:
   rex_leave_crit_sect(&pbm_crit_sect);
   return pbm_ret;

}


pbm_return_type pbm_uim_del_anrid(uint32 buffer_index,uint16 delete_link_index,
                                       pbm_field_s_type *anr_field)
{
  pbm_async_gsdi_buffer         *buffer;
  pbm_phonebook_type            pb_id;   
  PB_CACHE                      *cache;
  PB_MASTER_USIM_CACHE          *m_usim_cache = NULL;  
  pbm_pb_set_type               pb_set;
  int                           usim_index;  
  PB_USIM_CACHE                 *usim_cache = NULL;  
  uint8                         i;

  buffer = &pbm_buffers[buffer_index];
  //rec-id in buffer will be pointing to aas rec-id
  //get the pb_id from aas rec-id
  pb_id = pbm_session_record_id_to_phonebook_type(buffer->write->rec_id);  
  pb_id.device_type = PBM_ADN;
  cache = pbm_pb_id_to_cache(pb_id);
  PBM_CHECK_PTR_RET(cache, PBM_ERROR );
  pb_set = pbm_pb_record_index_to_pb_set(cache, delete_link_index); 
  if ( ! pbm_validate_pb_set(pb_set) )
  {
	 return PBM_ERROR; 
  }
  m_usim_cache = cache->m_usim_cache[pb_set];  
  for (i = 1; i <= m_usim_cache->num_caches; i++)
  {
     usim_cache = m_usim_cache->usim_cache_array[i];
     if (!usim_cache)
        continue;
  
     if(pbm_file_id_to_field_id(usim_cache->file_id) == PBM_FIELD_ADDITIONAL_NUMBER_EXT)
     {
       uint16 rec_num_in_file = pbm_pb_record_index_to_fileloc(cache,delete_link_index);
       usim_index = m_usim_cache->index_array[i][rec_num_in_file];
       if (usim_index)
       {
           byte                        *gsdi_buffer=NULL;
           pbm_sim_record_s_type       record;
           pbm_fixed_sim_rec_s_type    *sim_rec;
           pbm_anr_record_s_type       *anr_record = &record.data.anr;
           pbm_session_enum_type       session_type;
           mmgsdi_access_type          file_struct;
           pbm_return_type             pbm_ret = PBM_SUCCESS;
           mmgsdi_write_data_type      write_data;
           mmgsdi_return_enum_type     mmgsdi_retval = MMGSDI_ERROR;
           
           session_type = pbm_mapping_to_session(pb_id.pb_category,(pbm_slot_type) pb_id.slot_id, pb_id.prov_type);
           PBM_MEM_ALLOC(gsdi_buffer, (uint32) usim_cache->record_length);
           PBM_CHECK_PTR_RET(gsdi_buffer, PBM_ERROR_MEM_FULL);            
           pbm_build_record_from_usim_fields(delete_link_index,
                                             anr_field,
                                             1, &record);
           gsdi_buffer[0] = 0;
           sim_rec = (pbm_fixed_sim_rec_s_type *) &gsdi_buffer[1];
           
           if ( anr_record->anr_data[1] != 0)
           {
              if (anr_record->anr_data[1] > (PBM_SIM_DIALING_NUMBER_LEN + 1))
              {
                 memscpy((byte *)&sim_rec->ton_npi,1,(byte *)&anr_record->anr_data[2],1);
                 memscpy((byte *)sim_rec->number,PBM_SIM_DIALING_NUMBER_LEN, 
                        (byte *)&anr_record->anr_data[3],PBM_SIM_DIALING_NUMBER_LEN);
                 sim_rec->bcd_len = (PBM_SIM_DIALING_NUMBER_LEN + 1);
              } 
              else
              {
                 memscpy((byte *)sim_rec,anr_record->anr_data[1]+2, 
                         anr_record->anr_data,anr_record->anr_data[1]+2);
              }
           }
           
           sim_rec->ext_id = pbm_buffers[buffer_index].extension;
           
           if (usim_cache->mapping_type == PBM_MAPPING_TYPE_2 )
           {
              pbm_ret = pbm_fill_adn_sfi(anr_record->adn_record_id,
                                   &gsdi_buffer[usim_cache->record_length-2],session_type);
              if ( PBM_SUCCESS != pbm_ret )
              {
                 UIM_MSG_ERR_1("Failed to fill ADN SFI with error %d", pbm_ret);
                 pbm_free_record(buffer_index);                               
                 PBM_MEM_FREEIF(gsdi_buffer);
                 return PBM_ERROR;
              }
           }
           
           sim_rec->ext_id = pbm_check_extension_record(cache->extension_cache,
                                                      rec_num_in_file,
                                                      usim_cache->file_id);
           
           pbm_buffers[buffer_index].gsdi_resp_type = PBM_DEL_LINKS_WRITE_CB;
           pbm_ret = pbm_populate_file_access(session_type,&file_struct,usim_cache->file_id);
           if(pbm_ret != PBM_SUCCESS)
           {
              UIM_MSG_HIGH_1("PBM: in pbm_mmgsdi_sim_read pbm_populate_file_access returned error %d",pbm_ret);
           }
           
           write_data.data_len = usim_cache->record_length;
           write_data.data_ptr = gsdi_buffer;
           
           mmgsdi_retval = mmgsdi_session_write_record (pbm_session_info[session_type].session_id,
                                                        file_struct,
                                                        MMGSDI_LINEAR_FIXED_FILE,
                                                        usim_index,
                                                        write_data,
                                                        pbm_mmgsdi_async_cb,
                                                        buffer_index);
           if(mmgsdi_retval == MMGSDI_SUCCESS)
           {
              buffer->record.delete_link_index = delete_link_index;
              PBM_MEM_FREEIF(gsdi_buffer);
              return PBM_SUCCESS;
           }
         }
      }
  }

  UIM_MSG_ERR_0("Could not issue read request");
  pbm_free_record(buffer_index);
  return PBM_ERROR;
}

/*===========================================================================
FUNCTION PBM_UIM_WRITE

DESCRIPTION

PARAMETERS:

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_uim_write (uint32 buffer_index)
{
   pbm_write_record_s_type *write;
   pbm_async_gsdi_buffer   *buffer;
   pbm_return_type          ret = PBM_SUCCESS;
   pbm_session_enum_type            session_type;
   pbm_file_type   file_id;
   pbm_pb_set_type          pb_set;

   if (buffer_index >= PBM_ASYNC_BUFFERS || !pbm_buffers[buffer_index].in_use)
   {
      UIM_MSG_ERR_1("Invalid buffer index %d", buffer_index);
      return PBM_ERROR;
   }

   buffer = &pbm_buffers[buffer_index];
   write = buffer->write;

   session_type = pbm_record_id_to_session_type(write->rec_id);

   file_id = pbm_record_id_to_file_id(write->rec_id);
   pb_set = pbm_record_id_to_pb_set(write->rec_id);
   if ( ! pbm_validate_pb_set(pb_set) )
   {
       return PBM_ERROR; 
   }

   buffer->m_file_id = file_id;

   while (buffer->state != PBM_WRITE_DONE)
   {
      switch (buffer->state)
      {
         case PBM_WRITE_NOT_INIT:
         case PBM_WRITE_LINKS_DELETED:
            buffer->record.session_type = session_type;
            buffer->record.file_id = file_id;
            buffer->record.index = pbm_record_id_to_location(write->rec_id);
            buffer->findex = 0;

            /* Allocate memory for caches in use */
            PBM_MEM_FREEIF(buffer->caches_in_use);
            PBM_MEM_ALLOC(buffer->caches_in_use,
                          ((uint32) sizeof(boolean)) *
                          ((uint32) adn_m_usim_cache[session_type][pb_set].num_caches + 1));
            PBM_CHECK_PTR_RET(buffer->caches_in_use, PBM_ERROR_MEM_FULL);

            /* Initialize to FALSE */
            memset(buffer->caches_in_use, 0,
                   ((uint32) sizeof(boolean)) *
                   ((uint32) adn_m_usim_cache[session_type][pb_set].num_caches + 1));

            pbm_group_fields(write->prev_rec);
            pbm_group_fields(write->curr_rec);
            if (buffer->state == PBM_WRITE_LINKS_DELETED ||
                (file_id == PBM_FILE_AAS && buffer->write->curr_rec->num_fields != 0) ||              
                (!USE_USIM_SUPPORT(session_type) && !USE_CSIM_SUPPORT(session_type))||
                ((file_id != PBM_FILE_AAS) 
                 && (file_id != PBM_FILE_ADN) && (file_id != PBM_FILE_ADN1)
                 && (file_id != PBM_FILE_ADN2) && (file_id != PBM_FILE_ADN3)
                ))
            {
               /* non-USIM record / non-ADN record */
               if (pbm_uim_write_master(buffer_index, &ret) || ret != PBM_SUCCESS)
               {
                  PBM_MEM_FREEIF(buffer->caches_in_use);
                  buffer->state = PBM_WRITE_DONE;
                  return ret;
               }
            }
            if (file_id == PBM_FILE_AAS)
            {
              buffer->record.delete_link_index = 1;
              buffer->state = PBM_WRITE_DELETE_LINKS;                        
            }
            else
            {
            buffer->state = PBM_WRITE_SYNC;
            }
            break;

         case PBM_WRITE_DELETE_LINKS:
           if (pbm_uim_delete_links(buffer_index,&ret,1) || ret != PBM_SUCCESS)
              return ret;
        
           buffer->findex = 0;
           buffer->state = PBM_WRITE_LINKS_DELETED;
            break;

         case PBM_WRITE_SYNC:
            if (pbm_uim_write_sync(buffer_index, &ret) || ret != PBM_SUCCESS)
               return ret;

            buffer->findex = 0;
            buffer->state = PBM_WRITE_PREV_USIM;
            break;

         case PBM_WRITE_PREV_USIM:
            if (pbm_uim_write_usim(buffer_index, &ret) || ret != PBM_SUCCESS)
               return ret;

            buffer->findex = 0;
            buffer->state = PBM_WRITE_MASTER;

            /* we're done getting rid of the old record.  Mark all of the caches
            * in use to false.  The new record can use any of them. */

            memset(buffer->caches_in_use, 0,
                   ((uint32) sizeof(boolean)) *
                   ((uint32) adn_m_usim_cache[session_type][pb_set].num_caches + 1));
            break;

         case PBM_WRITE_MASTER:
            if (pbm_uim_write_master(buffer_index, &ret) || ret != PBM_SUCCESS)
               return ret;

            buffer->findex = 0;
            buffer->state = PBM_WRITE_CURR_USIM;
            break;

         case PBM_WRITE_CURR_USIM:
            if (pbm_uim_write_usim(buffer_index, &ret) || ret != PBM_SUCCESS)
               return ret;

            PBM_MEM_FREEIF(buffer->caches_in_use);

            buffer->findex = 0;
            buffer->state = PBM_WRITE_PBC;
            break;

         case PBM_WRITE_PBC:
            if (pbm_uim_write_pbc(buffer_index, &ret) || ret != PBM_SUCCESS)
               return ret;

            buffer->findex = 0;
            buffer->state = PBM_WRITE_DONE;
            break;

         default:
            break;
      }
   }

   pbm_uim_write_cb(buffer_index, PBM_SUCCESS);

   return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_WRITE_MASTER

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_uim_write_master(uint32 buffer_index, pbm_return_type *ret)
{
   pbm_async_gsdi_buffer *buffer;
   boolean bret = TRUE;

   PBM_CHECK_PTR_RET(ret, FALSE);
   *ret = PBM_ERROR;

   buffer = &pbm_buffers[buffer_index];
   PBM_CHECK_PTR_RET(buffer->write, FALSE);

   PBM_VALIDATE_SESSION_RET(buffer->record.session_type, FALSE);

   if (adn_m_sync_cache[buffer->record.session_type].reset_pbc || buffer->findex)
   {
      bret = FALSE;
      *ret = PBM_SUCCESS;
   } else
   {
      buffer->record.file_id = buffer->m_file_id;
      buffer->record.index = pbm_record_id_to_location(buffer->write->rec_id);
      buffer->findex_delta = 1;

      if ((*ret = pbm_sim_write(buffer_index)) != PBM_SUCCESS)
      {
         UIM_MSG_ERR_3("Unable to add record %d to SIM %d %d",
                     buffer->write->rec_id, *ret, buffer_index);
         bret = FALSE;
      } else
      {
         bret = TRUE;
      }
   }
   return bret;
}


/*===========================================================================
FUNCTION PBM_UIM_WRITE_SYNC

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_uim_write_sync(uint32 buffer_index, pbm_return_type *ret)
{
   pbm_async_gsdi_buffer *buffer = &pbm_buffers[buffer_index];
   pbm_write_record_s_type *write;
   pbm_session_enum_type session_type;
   pbm_file_type file_id[] = {PBM_FILE_PSC, PBM_FILE_CC, PBM_FILE_PUID,
      PBM_FILE_UID};

   PBM_CHECK_PTR_RET(ret, FALSE);
   *ret = PBM_ERROR;

   write = buffer->write;
   session_type = buffer->record.session_type;
   PBM_VALIDATE_SESSION_RET(session_type, FALSE);

   if (!adn_m_sync_cache[session_type].sync_present)
   {
      *ret = PBM_SUCCESS;
      return FALSE;
   }

   while (buffer->findex < ARR_SIZE(file_id))
   {
      switch (file_id[buffer->findex])
      {
         case PBM_FILE_PUID:
         case PBM_FILE_CC:
            buffer->record.file_id = file_id[buffer->findex];
            buffer->record.index = 1;
            buffer->findex_delta = 1;
            *ret = pbm_sync_write_sync1(buffer_index);
            return(*ret == PBM_SUCCESS ? TRUE : FALSE);

         case PBM_FILE_PSC:
            if (!write->sync.update_psc)
            {
               buffer->findex++;
            } else
            {
               buffer->record.file_id = PBM_FILE_PSC;
               buffer->record.index = 1;
               buffer->findex_delta = 1;
               *ret = pbm_sync_write_sync1(buffer_index);
               return(*ret == PBM_SUCCESS ? TRUE : FALSE);
            }
            break;

         case PBM_FILE_UID:
            if (!write->sync.update_psc)
            {
               buffer->record.file_id = PBM_FILE_UID;
               buffer->record.index = pbm_record_id_to_location(write->rec_id);
               buffer->findex_delta = 1;
               *ret = pbm_sync_write_sync2(buffer->write->curr_rec->unique_id,
                                           buffer_index);
               return(*ret == PBM_SUCCESS ? TRUE : FALSE);
            } else
            {
               uint16 data;
               uint16 index;
               PB_CACHE *pb_cache = &adn_cache[session_type];

               if (buffer->record.file_id != PBM_FILE_UID)
                  index = 1;
               else
                  index = (uint16) buffer->record.index+1;

               /* Find first non-NULL record */
               while (index <= pb_cache->num_of_records)
               {
                  if (pb_cache->pb_cache_array[index])
                     break;
                  else
                     index++;
               }

               /* Check if we are done */
               if (index > pb_cache->num_of_records)
               {
                  buffer->findex++;
                  break;
               }

               buffer->findex_delta = 0;
               buffer->record.index = index;
               buffer->record.file_id = PBM_FILE_UID;

               data = pb_cache->pb_cache_array[buffer->record.index]->unique_id;
               *ret =  pbm_sync_write_sync2(data, buffer_index);
               return(*ret == PBM_SUCCESS ? TRUE : FALSE);
            }

         default:
            UIM_MSG_ERR_1("Unexpected file id %d", file_id[buffer->findex]);
            break;
      }
   }
   if (adn_cache[session_type].status == PBM_STATUS_WRITE_SYNC)
      PBM_SET_PB_CACHE_READY(&adn_cache[session_type]);

   *ret = PBM_SUCCESS;
   return FALSE;
}

/*===========================================================================
FUNCTION PBM_UIM_WRITE_PBC

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_uim_write_pbc (uint32 buffer_index, pbm_return_type *ret)
{
   pbm_async_gsdi_buffer *buffer = &pbm_buffers[buffer_index];
   pbm_write_record_s_type *write;
   pbm_addr_cache_s_type *record = NULL;
   uint8 l_j;
   uint16 pbc_data = 0;
   boolean bret = FALSE;
   pbm_session_enum_type session_type;
   PBM_CHECK_PTR_RET(ret, FALSE);
   *ret = PBM_ERROR;


   do
   {
      if (buffer->findex)
      {
         *ret = PBM_SUCCESS;
         return FALSE;
      }
      record = buffer->write->curr_rec;
      session_type = pbm_record_id_to_session_type (record->record_id);
      for(l_j = 0;l_j < record->num_fields; l_j++)
      {
         if((record->fields) &&
           (record->fields[l_j].field_id == PBM_FIELD_HIDDEN) &&
         (*((byte *) pbm_get_field_data_ptr(record->fields,l_j)) == TRUE) )
         /* record_fields should not be NULL, before passing it to pbm_get_field_data_ptr */
         /* If field is hidden and content of field is true then update the pbc data with hidden dir_index value */
         {
             pbc_data |= pbm_session_info[session_type].dir_index;
         }
      }

      //no need to write anything in the below cases
      if((pbm_session_info[session_type].dir_index == 0
      && adn_m_sync_cache[session_type].reset_pbc == FALSE)
      || ( (pbc_data == 0) && (adn_m_sync_cache[session_type].reset_pbc == FALSE) )
      || pbc_cache[session_type].num_of_records < pbm_record_id_to_location(buffer->write->rec_id))
      {
         //bret FALSE means no write operation issued
         bret = FALSE;
         *ret = PBM_SUCCESS;
         break;
      }

      write = buffer->write;

      buffer->record.file_id = PBM_FILE_PBC;
      buffer->record.index = pbm_record_id_to_location(write->rec_id);
      buffer->findex_delta = 1;
      *ret = pbm_sync_write_sync2(pbc_data, buffer_index);
      if(*ret ==  PBM_SUCCESS)
         bret = TRUE;
   }while(0);

   return(bret);
}

/*===========================================================================
FUNCTION pbm_uim_write_usim

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_uim_write_usim(uint32 buffer_index, pbm_return_type *ret)
{
   int i;
   pbm_addr_cache_s_type *record = NULL;
   pbm_async_gsdi_buffer *buffer;
   uint8 *index_array = NULL;

   PBM_CHECK_PTR_RET(ret, FALSE);
   *ret = PBM_ERROR;

   buffer = &pbm_buffers[buffer_index];
   PBM_CHECK_PTR_RET(buffer->write, FALSE);

   if (buffer->state == PBM_WRITE_PREV_USIM)
   {
      record = buffer->write->prev_rec;
      index_array = buffer->write->prev_usim_index;
   } else if (buffer->state == PBM_WRITE_CURR_USIM)
   {
      record = buffer->write->curr_rec;
      index_array = buffer->write->curr_usim_index;
   }

   if (record)
   {
      for (i = buffer->findex; i < record->num_fields; i++)
      {
        if (pbm_is_usim_field(record->fields[i].field_id))
         {
            buffer->findex = (uint16)i;
            return pbm_usim_write(buffer_index, ret);
         }
      }

      buffer->findex = record->num_fields;
      /* Add IAP entry */
      if (index_array)
      {
         pbm_pb_set_type pb_set =
         pbm_record_id_to_pb_set(buffer->write->rec_id);

         if ( ! pbm_validate_pb_set(pb_set) )
         {
             return FALSE; 
         }
         PBM_VALIDATE_SESSION_RET(buffer->record.session_type, FALSE);

         /* Add iap entry if IAP file is present & we've not already done it */
         if (adn_m_usim_cache[buffer->record.session_type][pb_set].iap_num_records
             && (pbm_file_id_to_cache_type(buffer->record.file_id) !=
                 PBM_CACHE_TYPE_INDEX))
         {
            if (pbm_iap_write(buffer_index) != PBM_SUCCESS)
            {
               UIM_MSG_ERR_1("Unable to update iap file %d", *ret);
               return FALSE;
            } else
            {
               *ret = PBM_SUCCESS;
               return TRUE;
            }
         }
      }
   }
   *ret = PBM_SUCCESS;
   return FALSE;
}

/*===========================================================================
FUNCTION PBM_SIM_WRITE

DESCRIPTION

PARAMETERS:
  record.index
  record.session_type
  record.file_id
  write

DEPENDENCIES
  Cache recovery should be done outside this function if write fails.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_sim_write (uint32 buffer_index)
{
   PB_CACHE                *cache;
   pbm_phonebook_type       pb_id = {PBM_GPB};
   pbm_file_type        file_id;
   pbm_session_enum_type       session_type;
   pbm_write_record_s_type *write;
   pbm_num_text_record_s_type *record;
   boolean                  delete_flag;
   pbm_return_type          ret_val;
   int                      rec_num_in_file;

   write = pbm_buffers[buffer_index].write;
   PBM_CHECK_PTR_RET(write, PBM_ERROR);
   record = &pbm_buffers[buffer_index].record.data.num_text;
   /* Fill SIM data */
   delete_flag = (write->curr_rec && write->curr_rec->num_fields) ? FALSE : TRUE;
   if (delete_flag)
   {
      memset(record, 0, sizeof(pbm_num_text_record_s_type));
      memset(record->sim_text,0xFF,sizeof(record->sim_text));
      record->index = pbm_buffers[buffer_index].record.index;
   } else
   {
      pbm_build_num_text_record_from_fields(write->curr_rec->fields,
                                            write->curr_rec->num_fields,
                                            pbm_buffers[buffer_index].record.index,
                                            record);
   }
   session_type = pbm_buffers[buffer_index].record.session_type;
   PBM_VALIDATE_SESSION_RET(session_type, PBM_ERROR);
   file_id = pbm_buffers[buffer_index].record.file_id;
   pb_id = pbm_session_record_id_to_phonebook_type(write->rec_id);
   cache = pbm_pb_id_to_cache(pb_id);
   if (!cache)
   {
      UIM_MSG_ERR_1("No cache pb_id = %d", pb_id.device_type);
      return PBM_ERROR;
   }

   /* buffer contains the bcd in the format    */
   /* byte  0      1      2 thru buffer[0]     */
   /*     <len><ton/npi><bcd packed number>    */
   /* Note that length is the length including */
   /* the TON/NPI field.                       */
   if(file_id != PBM_FILE_GAS
        && file_id != PBM_FILE_AAS)
   {
   /* Do we need an extension record?  */
      rec_num_in_file = pbm_pb_record_index_to_fileloc(cache, record->index);
      if ((record->bcd_number[0] - 1) > PBM_SIM_DIALING_NUMBER_LEN)
      {
         return pbm_create_ext_record(buffer_index, cache->extension_cache,
                                      record->bcd_number, 
                                      (uint16)rec_num_in_file, file_id);
      }
      /* see if we used to need one, but no longer */
      else if (pbm_check_extension_record(cache->extension_cache, rec_num_in_file,
                                          file_id))
      {
         return pbm_clear_ext_record(buffer_index, cache->extension_cache,
                                     (uint16)rec_num_in_file, file_id);
      }
   }

   pbm_buffers[buffer_index].extension = 0xff;
   ret_val = pbm_create_sim_record(session_type, file_id, record,
                                   (int)buffer_index);
   if (ret_val != PBM_SUCCESS)
   {
      UIM_MSG_ERR_1("Error %d from pbm_create_sim_record", ret_val);
      return ret_val;
   }
   return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_SIM_READ

DESCRIPTION
  Reads a specified record to phone book. This function is used for any file
  that is directly available to PBM clients for read/write. This means the
  record will be read to a PB_CACHE. Ex: ADN, FDN, SDN etc.

  To read a USIM record, pbm_usim_read() should be used.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_sim_read (pbm_session_enum_type session_type,
                              pbm_file_type file_id,
                              int record_num,
                              void (*proc_func) (pbm_return_type,
                                                 pbm_sim_record_s_type *))
{
   int buffer_index;
   PB_CACHE *cache;
   int index;

   if (proc_func == NULL)
   {
     UIM_MSG_ERR_3("Proc function is NULL sess %d file %d rec %d",session_type,file_id,record_num);
     return PBM_ERROR;
   }

   UIM_MSG_HIGH_3("pbm_sim_read for sess %d file %d rec %d",session_type,file_id,record_num);

  if (pbm_file_id_to_cache_type(file_id) != PBM_CACHE_TYPE_PB)
  {
    UIM_MSG_ERR_1("Wrong function (pbm_sim_read) used to read file %d",
                                                               file_id);
    return PBM_ERROR;
  }
  PBM_VALIDATE_SESSION_RET(session_type, PBM_ERROR);

   cache = (PB_CACHE *) pbm_file_id_to_cache(session_type, file_id);
   PBM_CHECK_PTR_RET(cache, PBM_ERROR);

   index = pbm_pb_record_index_to_fileloc(cache, record_num);
   file_id = pbm_pb_record_index_to_filename(cache, record_num);
   if (file_id == PBM_FILE_NONE || index <= 0)
   {
      UIM_MSG_ERR_3("Invalid cache/index %d %d %d",
                  file_id, record_num, cache->num_files);
      return PBM_ERROR;
   }

   buffer_index = pbm_alloc_record();

   if (buffer_index < 0)
      return PBM_ERROR;

   pbm_buffers[buffer_index].proc_func = proc_func;
   pbm_buffers[buffer_index].record.index = record_num;
   pbm_buffers[buffer_index].record.file_id = file_id;
   pbm_buffers[buffer_index].record.session_type = session_type;
   pbm_buffers[buffer_index].gsdi_resp_type = PBM_MMGSDI_READ_CB;
   if (pbm_send_read_request(session_type, file_id,
                             index, buffer_index, pbm_mmgsdi_async_cb))
   {
      return PBM_SUCCESS;
   }

   UIM_MSG_ERR_0("Could not issue read request");
   pbm_free_record(buffer_index);
   return PBM_ERROR;
}

/*===========================================================================
FUNCTION PBM_UIM_WRITE_CB

DESCRIPTION
  This function is called in response to a pbm_uim_write function after
  all processing is done. The input parameter 'ret' specifes whether the
  operation succeeded or not.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_uim_write_cb (uint32 buffer_index, pbm_return_type ret)
{
   pbm_write_record_s_type *write;
   boolean usim_fields_present = FALSE;

   if (buffer_index >= PBM_ASYNC_BUFFERS || !pbm_buffers[buffer_index].in_use)
   {
      UIM_MSG_ERR_1("Invalid buffer_index %d", buffer_index);
      (void) rex_set_sigs(PBM_TCB, PBM_CMD_COMPLETE_SIG);
      return;
   }

   write = pbm_buffers[buffer_index].write;
   PBM_CHECK_PTR_RET(write, VOID);

   if (ret == PBM_SUCCESS)
   {
      pbm_list_update(write);
   } else
   {
      if ((write->curr_rec && pbm_is_usim_required(write->rec_id,
                                                   write->curr_rec->fields,
                                                   write->curr_rec->num_fields)) ||
          (write->prev_rec && pbm_is_usim_required(write->rec_id,
                                                   write->prev_rec->fields,
                                                   write->prev_rec->num_fields)))
      {
         usim_fields_present = TRUE;
      }
      /* For now, if there is an error in USIM write, the cache entry is deleted.
      * TODO: A record that reflects the contents of USIM at the point
      * of error can be built from the values in USIM cache. */
      pbm_list_recover(write, usim_fields_present);
   }

   pbm_write_notify(write, ret);
   pbm_free_record((int)buffer_index);
   pbm_write_cmd_free(write);
}

/*===========================================================================
FUNCTION PBM_SYNC1_READ_CB

DESCRIPTION
  This function is called during init by GSDI when a USIM record is read in.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_sync1_read_cb (pbm_return_type pbm_returned,
                        pbm_sim_record_s_type *record)
{
   PB_SYNC1_CACHE *sync1_cache;
   pbm_session_enum_type session_type;
   pbm_file_type file_id;

   PBM_CHECK_PTR_RET(record, VOID);
   session_type = record->session_type;
   PBM_VALIDATE_SESSION_RET(session_type, VOID);

   file_id = record->file_id;

   sync1_cache = pbm_file_id_to_cache(session_type, file_id);
   if (sync1_cache != NULL)
   {
      if (pbm_returned == PBM_SUCCESS) /* If we can trust the data. */
      {
         switch (file_id)
         {
            case PBM_FILE_PSC:
               psc_cache[session_type].data.psc = record->data.dword;
               adn_m_sync_cache[session_type].psc_cache = &psc_cache[session_type];
               break;

            case PBM_FILE_PUID:
               puid_cache[session_type].data.puid = record->data.word;
               adn_m_sync_cache[session_type].puid_cache = &puid_cache[session_type];
               break;

            case PBM_FILE_CC:
               cc_cache[session_type].data.cc = record->data.word;
               adn_m_sync_cache[session_type].cc_cache = &cc_cache[session_type];
               break;

            default:
               break;
         }
      } else
      {
         UIM_MSG_ERR_1("Unable to read from session, Error %d ",
                     pbm_returned);
      }
   } else
   {
      UIM_MSG_ERR_1("Could not find cache for file %d", file_id);
   }

   /* Move to next file regardless of why we got here. */
   pbm_initialize_index[session_type]++;
   pbm_init_entries(session_type);
}

/*===========================================================================
FUNCTION PBM_SYNC2_READ_CB

DESCRIPTION
  This function is called during init by GSDI when a USIM record is read in.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_sync2_read_cb (pbm_return_type pbm_returned,
                        pbm_sim_record_s_type *record)
{
   PB_SYNC2_CACHE *sync2_cache;
   pbm_session_enum_type session_type;
   int index;
   pbm_file_type file_id;
   int record_num = 0;

   PBM_CHECK_PTR_RET(record, VOID);

   session_type = record->session_type;
   PBM_VALIDATE_SESSION_RET(session_type, VOID);

   file_id = record->file_id;
   index = record->index;

   sync2_cache = pbm_file_id_to_cache(session_type, file_id);
   if (sync2_cache != NULL)
   {
      if (pbm_returned == PBM_SUCCESS) /* If we can trust the data. */
      {
         switch (file_id)
         {
            case PBM_FILE_UID:
			case PBM_FILE_UID1:
			case PBM_FILE_UID2:
			case PBM_FILE_UID3:
               PBM_CHECK_PTR_RET(uid_cache[session_type].data_array, VOID);
               uid_cache[session_type].data_array[index] = record->data.word;
               break;

            case PBM_FILE_PBC:
			case PBM_FILE_PBC1:
			case PBM_FILE_PBC2:
			case PBM_FILE_PBC3:
               PBM_CHECK_PTR_RET(pbc_cache[session_type].data_array, VOID);
               pbc_cache[session_type].data_array[index] = record->data.word;
               break;

            default:
               break;
         }

         record_num = pbm_get_next_used_record(session_type,
                                               record->index);

         if ((record_num <= sync2_cache->num_of_records) &&
             ((pbm_returned = pbm_sync_read(record->session_type,
                                            record->file_id,
                                            record_num,
                                            pbm_sync2_read_cb)) == PBM_SUCCESS))
            return;

         if (record_num > sync2_cache->num_of_records)
         {
            switch (file_id)
            {
               case PBM_FILE_PBC:
			   case PBM_FILE_PBC1:
			   case PBM_FILE_PBC2:
			   case PBM_FILE_PBC3:
                  adn_m_sync_cache[session_type].pbc_cache = sync2_cache;
                  adn_cache[session_type].m_sync_cache =
                  &adn_m_sync_cache[session_type];
                  break;

               case PBM_FILE_UID:
			   case PBM_FILE_UID1:
			   case PBM_FILE_UID2:
			   case PBM_FILE_UID3:
                  adn_m_sync_cache[session_type].uid_cache = sync2_cache;
                  if (adn_m_sync_cache[session_type].psc_cache &&
                      adn_m_sync_cache[session_type].cc_cache &&
                      adn_m_sync_cache[session_type].puid_cache &&
                      adn_m_sync_cache[session_type].uid_cache)
                  {
                     adn_cache[session_type].m_sync_cache =
                     &adn_m_sync_cache[session_type];
                     adn_cache[session_type].m_sync_cache->sync_present = TRUE;
                  }
                  break;

               default:
                  break;
            }
         } else
         {
            UIM_MSG_ERR_3("Fail to read next rec in USIM, Err %d after %d max %d",
                        pbm_returned, record->index, sync2_cache->num_of_records);

            /* This cache will be unusable, make sure its clear. */
            rex_enter_crit_sect(&pbm_crit_sect);
            pbm_clear_sync2_cache(sync2_cache);
            rex_leave_crit_sect(&pbm_crit_sect);
         }
      } else
      {
         UIM_MSG_ERR_3("Fail to read from SIM card, Error %d after %d of max %d",
                     pbm_returned, record->index, sync2_cache->num_of_records);

         /* This cache will be unusable, make sure its clear. */
         rex_enter_crit_sect(&pbm_crit_sect);
         pbm_clear_sync2_cache(sync2_cache);
         rex_leave_crit_sect(&pbm_crit_sect);
      }
   } else
   {
      UIM_MSG_ERR_1("Could not find cache for file %d", file_id);
   }

   /* Move to next file regardless of why we got here. */
   pbm_initialize_index[session_type]++;

   pbm_init_entries(session_type);
}

/*===========================================================================
FUNCTION PBM_INDEX_READ_CB

DESCRIPTION
  This function is called during init by GSDI when a USIM record is read in.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_index_read_cb (
  pbm_return_type        pbm_returned,
  pbm_sim_record_s_type *record
)
{
  PB_MASTER_USIM_CACHE   *m_usim_cache = NULL;
  PB_USIM_CACHE          *usim_cache   = NULL;
  pbm_session_enum_type   session_type = PBM_SESSION_DEFAULT;
  pbm_pb_set_type         pb_set       = PBM_USIM_PB_INVALID;
  pbm_file_type           file_id      = PBM_FILE_MAX_REQUESTS;
  uint16                  usim_loc     = 0;
  int                     i            = 0;
  int                     k            = 0;
  int                     record_num   = 0;

  PBM_CHECK_PTR_RET(record, VOID);

  session_type = record->session_type;

  PBM_VALIDATE_SESSION_RET(session_type, VOID);
              
  file_id = record->file_id;
  m_usim_cache = pbm_file_id_to_cache(session_type, file_id);
  pb_set= pbm_file_id_to_pb_set(file_id);
  if (! pbm_validate_pb_set(pb_set))
  {
    return ;
  }
			  
  UIM_MSG_HIGH_3("pbm_index_read_cb session %d file_id %d index %d", session_type, file_id, record->index);

  if (m_usim_cache != NULL)
  {
    if (pbm_returned == PBM_SUCCESS) /* If we can trust the data. */
    {
      /* Check if record matches master index,
       * otherwise do not use that USIM field */
      for (i = 1; i <= m_usim_cache->num_caches && m_usim_cache->usim_cache_array; i++)
      {
        usim_cache = m_usim_cache->usim_cache_array[i];
        usim_loc = record->data.iap.index[i-1];

        if (usim_cache &&
            usim_cache->mapping_type == PBM_MAPPING_TYPE_2 &&
            usim_loc <= usim_cache->num_of_records)
        {
          for(k = 1; k < record->index; k++)
          {
            if( m_usim_cache->index_array  && m_usim_cache->index_array[i] )
            {
              if( m_usim_cache->index_array[i][k] ==  usim_loc )
              {
                break;
              }
            }
          }

          if( m_usim_cache->index_array  && m_usim_cache->index_array[i] && k == record->index)
          {
            UIM_MSG_HIGH_3("Updating index array for ADN loc %d usim-%d loc %d",
                            record->index, usim_cache->file_id, usim_loc);
            m_usim_cache->index_array[i][record->index] =  usim_loc;
          }

          if ( usim_cache->pb_cache_array && usim_cache->pb_cache_array[usim_loc] && k == record->index)
          {
            usim_cache->pb_cache_array[usim_loc]->m_recid= pbm_location_to_adn_rec_id(session_type,pb_set,record->index);
            UIM_MSG_HIGH_3("Updating usim cache with adn rec id 0x%x sess %d pbset %d",
                            usim_cache->pb_cache_array[usim_loc]->m_recid,session_type,pb_set);
          }
        }
      }

      record_num = pbm_get_next_used_record(session_type,record->index);

      if ((record_num <= m_usim_cache->iap_num_records) &&
          ((pbm_returned = pbm_index_read(record->session_type,
                                          record->file_id,
                                          record_num,
                                          pbm_index_read_cb)) == PBM_SUCCESS))
      {
        return;
      }

      if (record_num <= m_usim_cache->iap_num_records)
      {
        UIM_MSG_ERR_3("Fail to read next rec in USIM, Err %d after %d of max %d",
                       pbm_returned, record->index, m_usim_cache->iap_num_records);
        /* This cache will be unusable, make sure its clear. */

        rex_enter_crit_sect(&pbm_crit_sect);
        pbm_clear_index_cache(m_usim_cache);
        rex_leave_crit_sect(&pbm_crit_sect);
      }
    }
    else
    {
      UIM_MSG_ERR_3("Fail to read next rec in USIM, Err %d after %d of max %d",
                     pbm_returned, record->index, m_usim_cache->iap_num_records);

      /* This cache will be unusable, make sure its clear. */
      rex_enter_crit_sect(&pbm_crit_sect);
      pbm_clear_index_cache(m_usim_cache);
      rex_leave_crit_sect(&pbm_crit_sect);
    }
  }
  else
  {
    UIM_MSG_ERR_1("Could not find cache for file %d", file_id);
  }

  if( m_usim_cache != NULL )
  {
    for(i = 1; i <= m_usim_cache->num_caches && m_usim_cache->usim_cache_array; i++)
    {
      usim_cache = m_usim_cache->usim_cache_array[i];
      if (usim_cache && usim_cache->mapping_type == PBM_MAPPING_TYPE_2 )
      {
        uint8 iap_rec = 1;

        for(k = 1 ; k <= usim_cache->num_of_records; k++)
        {
          if(usim_cache->pb_cache_array && usim_cache->pb_cache_array[k] != NULL)
          {
            if(m_usim_cache->index_array  && m_usim_cache->index_array[i])
            {
              iap_rec = 1;
              while(iap_rec <= m_usim_cache->iap_num_records)
              {
                if(m_usim_cache->index_array[i][iap_rec] == k)
                {
                  break;
                }

                iap_rec++;
              }

              if(iap_rec > m_usim_cache->iap_num_records)
              {
                (void) pbm_usim_cache_delete(session_type, usim_cache, (uint16)k, FALSE);
                 usim_cache->records_in_use--;
              }
            }
          }
        }/*for k */
      }
    } /*for i */
  }

  /* Move to next file regardless of why we got here. */
  pbm_initialize_index[session_type]++;

  pbm_init_entries(session_type);
}

/*===========================================================================
FUNCTION PBM_USIM_READ

DESCRIPTION
  Reads a specified record to USIM phone book. This function should be used
  to read only USIM files.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_usim_read( pbm_session_enum_type session_type,
                               pbm_file_type file_id,
                               int record_num,
                               void (*proc_func) ( pbm_return_type,
                                                   pbm_sim_record_s_type *))
{
   int buffer_index;
   PB_USIM_CACHE *usim_cache;
   pbm_cache_type_e_type cache_type;
   int index;

   if (proc_func == NULL)
   {
      UIM_MSG_ERR_3("Proc function is NULL sess %d file %d rec %d",session_type,file_id,record_num);
      return PBM_ERROR;
   }

   UIM_MSG_HIGH_3("pbm_usim_read for sess %d file %d rec %d",session_type,file_id,record_num);

   cache_type = pbm_file_id_to_cache_type(file_id);
   if (cache_type != PBM_CACHE_TYPE_USIM)
   {
      UIM_MSG_ERR_1("Wrong function (pbm_usim_read) used to read file %d",
                  file_id);
      return PBM_ERROR;
   }

   PBM_VALIDATE_SESSION_RET(session_type, PBM_ERROR);
   usim_cache = pbm_file_id_to_cache(session_type, file_id);
   PBM_CHECK_PTR_RET(usim_cache, PBM_ERROR);

   index = record_num;

   if (file_id == PBM_FILE_NONE || index <= 0)
   {
      UIM_MSG_ERR_2("Invalid cache/index %d %d", file_id, record_num);
      return PBM_ERROR;
   }

   buffer_index = pbm_alloc_record();

   if (buffer_index < 0)
      return PBM_ERROR;

   pbm_buffers[buffer_index].proc_func = proc_func;
   pbm_buffers[buffer_index].record.index = record_num;
   pbm_buffers[buffer_index].record.file_id = file_id;
   pbm_buffers[buffer_index].record.session_type = session_type;
   pbm_buffers[buffer_index].gsdi_resp_type = PBM_MMGSDI_READ_CB;
   if (pbm_send_read_request(session_type, file_id,
                             index, buffer_index, pbm_mmgsdi_async_cb))
   {
      return PBM_SUCCESS;
   }

   UIM_MSG_ERR_0("Could not issue read request");
   pbm_free_record(buffer_index);
   return PBM_ERROR;
}

/*===========================================================================
FUNCTION PBM_INDEX_READ

DESCRIPTION

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_index_read(pbm_session_enum_type session_type,
                               pbm_file_type file_id,
                               int record_num,
                               void (*proc_func) ( pbm_return_type,
                                                   pbm_sim_record_s_type *))
{
   int buffer_index;
   PB_MASTER_USIM_CACHE *m_usim_cache;
   pbm_cache_type_e_type cache_type;
   int index;

   if (proc_func == NULL)
   {
      UIM_MSG_ERR_3("Proc function is NULL sess %d file %d rec %d",session_type,file_id,record_num);
      return PBM_ERROR;
   }

   UIM_MSG_HIGH_3("pbm_index_read for sess %d file %d rec %d",session_type,file_id,record_num);

   cache_type = pbm_file_id_to_cache_type(file_id);
   if (cache_type != PBM_CACHE_TYPE_INDEX)
   {
      UIM_MSG_ERR_1("Wrong function (pbm_index_read) used to read file %d",
                  file_id);
      return PBM_ERROR;
   }

   PBM_VALIDATE_SESSION_RET(session_type, PBM_ERROR);
   m_usim_cache = pbm_file_id_to_cache(session_type, file_id);
   PBM_CHECK_PTR_RET(m_usim_cache, PBM_ERROR);
   if (!m_usim_cache->iap_num_records)
   {
      UIM_MSG_ERR_0("IAP file not present");
      return PBM_ERROR;
   }
   index = record_num;
   if (file_id == PBM_FILE_NONE || index <= 0)
   {
      UIM_MSG_ERR_2("Invalid cache/index %d %d", file_id, record_num);
      return PBM_ERROR;
   }
   buffer_index = pbm_alloc_record();
   if (buffer_index < 0)
      return PBM_ERROR;
   pbm_buffers[buffer_index].proc_func = proc_func;
   pbm_buffers[buffer_index].record.index = record_num;
   pbm_buffers[buffer_index].record.file_id = file_id;
   pbm_buffers[buffer_index].record.session_type = session_type;
   pbm_buffers[buffer_index].gsdi_resp_type = PBM_MMGSDI_READ_CB;

   if (pbm_send_read_request(session_type, file_id,
                             index, buffer_index, pbm_mmgsdi_async_cb))
   {
      return PBM_SUCCESS;
   }

   UIM_MSG_ERR_0("Could not issue read request");
   pbm_free_record(buffer_index);
   return PBM_ERROR;
}

/*===========================================================================
FUNCTION pbm_sync_read

DESCRIPTION
  Reads a specified record to USIM phone book. This function should be used
  to read only synchronization files.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_sync_read(pbm_session_enum_type session_type,
                              pbm_file_type file_id,
                              int record_num,
                              void (*proc_func) ( pbm_return_type,
                                                  pbm_sim_record_s_type *))
{
   int buffer_index;
   int index;
   pbm_cache_type_e_type cache_type;

   if (proc_func == NULL)
      return PBM_ERROR;

   cache_type = pbm_file_id_to_cache_type(file_id);
   if (cache_type != PBM_CACHE_TYPE_SYNC1 &&
       cache_type != PBM_CACHE_TYPE_SYNC2)
   {
      UIM_MSG_ERR_1("Invalid function pbm_sync_read called for file_id %d",
                  file_id);
      return PBM_ERROR;
   }

   PBM_VALIDATE_SESSION_RET(session_type, PBM_ERROR);

   if (file_id >= PBM_FILE_MAX_REQUESTS)
   {
      UIM_MSG_ERR_1("Invalid file id %d", file_id);
      return PBM_ERROR;
   }

   if (cache_type == PBM_CACHE_TYPE_SYNC1)
   {
      PB_SYNC1_CACHE *sync1_cache;
      sync1_cache = pbm_file_id_to_cache(session_type, file_id);
      PBM_CHECK_PTR_RET(sync1_cache, PBM_ERROR);
      index = 0; /* Record number should be 0 for transparent file */
   } else
   {
      PB_SYNC2_CACHE *sync2_cache;
      sync2_cache = pbm_file_id_to_cache(session_type, file_id);
      PBM_CHECK_PTR_RET(sync2_cache, PBM_ERROR);
      index = pbm_sync2_record_index_to_fileloc(sync2_cache, record_num);
	  file_id = pbm_sync2_record_index_to_filename(sync2_cache,record_num);
   }

   if (file_id == PBM_FILE_NONE || index < 0)
   {
      UIM_MSG_ERR_2("Invalid file or index %d %d", file_id, record_num);
      return PBM_ERROR;
   }
   buffer_index = pbm_alloc_record();
   if (buffer_index < 0)
      return PBM_ERROR;

   pbm_buffers[buffer_index].proc_func = proc_func;
   pbm_buffers[buffer_index].record.index = record_num;
   pbm_buffers[buffer_index].record.file_id = file_id;
   pbm_buffers[buffer_index].record.session_type = session_type;
   pbm_buffers[buffer_index].gsdi_resp_type = PBM_MMGSDI_READ_CB;

   if (pbm_send_read_request(session_type, file_id,
                             index, buffer_index, pbm_mmgsdi_async_cb))
   {
      return PBM_SUCCESS;
   }

   UIM_MSG_ERR_0("Could not issue read request");
   pbm_free_record(buffer_index);
   return PBM_ERROR;
}

/*===========================================================================
FUNCTION PBM_SIM_WRITE_CB

DESCRIPTION
  After GSDI has written the extension record to EXT1, PBM_WRITE_ADN_CB
  is called to copy the data in to a pbm_buffer for later processing.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_sim_write_cb( mmgsdi_return_enum_type status,
                       const mmgsdi_cnf_type  *data_ptr)
{
   uint32 client_ref;
   PB_CACHE *cache;      /* The cache has info on extension records. */
   pbm_file_type file_id;
   pbm_g_other_field_info_s_type *field_info;
   int i = 0, j=0;
   boolean empty_ext = FALSE;
   pbm_session_enum_type session_type;

   if (!data_ptr)
   {
      UIM_MSG_ERR_1("PBM Bad pointer passed to function 0x%x",data_ptr);
      (void) rex_set_sigs(PBM_TCB, PBM_CMD_COMPLETE_SIG);
      return;
   }

   client_ref = data_ptr->response_header.client_data;
   file_id = pbm_buffers[client_ref].record.file_id;
   session_type = pbm_buffers[client_ref].record.session_type;
   cache = pbm_file_id_to_cache(
                               pbm_buffers[client_ref].record.session_type,
                               pbm_buffers[client_ref].m_file_id);
   if (cache == NULL)
   {
      UIM_MSG_ERR_1("cache was NULL for phonebook file 0x%x", file_id);
      pbm_uim_write_cb(client_ref, PBM_ERROR);
      return;
   }
   if (status != MMGSDI_SUCCESS)
   {
      pbm_return_type error_code = PBM_ERROR;
      /* give better error codes for access denied errors */
      if (status == MMGSDI_ACCESS_DENIED)
      {
         switch (file_id)
         {
            /* these require ADM access, which we don't have special error for */
            case PBM_FILE_SDN:
            case PBM_FILE_EXT3:
            case PBM_FILE_MSISDN:
            case PBM_FILE_MBN:
            case PBM_FILE_GAS:
            case PBM_FILE_AAS:
            case PBM_FILE_MBDN:
               error_code = PBM_ERROR;
               break;

               /* these require PIN2 */
            case PBM_FILE_FDN:
            case PBM_FILE_EXT2:
               error_code = PBM_ERROR_PIN2_REQ;
               break;

               /* the rest of the PB requires PIN1 */
            default:
               error_code = PBM_ERROR_SIM_PIN_REQ;
               break;
         }
      }
      else if (status == MMGSDI_MEMORY_ERROR)
      {
        error_code = PBM_ERROR_SIM_MEMORY_PROBLEM;
      }
      pbm_uim_write_cb(client_ref, error_code);
      return;
   }
   if (pbm_buffers[client_ref].extension != 0xFF)
   {
      if (cache->extension_cache)
      {
         rex_enter_crit_sect(&pbm_crit_sect); /* for extension cache in_use_array */
         cache->extension_cache->in_use_array[pbm_buffers[client_ref].extension].uim_device =
         pbm_pb_record_index_to_filename(cache,
                                         pbm_buffers[client_ref].record.index);
         cache->extension_cache->in_use_array[pbm_buffers[client_ref].extension].index =
         (byte)pbm_pb_record_index_to_fileloc(cache,
                                              pbm_buffers[client_ref].record.index);
         /* since the ext record and adn record has been written succesfully updating
         g_sim_field_info number lengths */
         /* check if the ext cache has any empty record or not */
         for (j = cache->extension_cache->num_of_rec; j > 0; j--)
         {
           if (cache->extension_cache->in_use_array[j].index == 0)
           {
             empty_ext = TRUE;
           }
         }
		 rex_leave_crit_sect(&pbm_crit_sect); /* for ext_cache->in_use_array */
         if(!empty_ext)
         {
           field_info = &g_sim_field_info[session_type][0]; /* ADN info */
           for ( i = 0; i < field_info->num_fields; i++)
           {
             if (field_info->info[i].field_type == PBM_FT_PHONE)
             {
               //increment by 2 to accomodate '+' sign & NULL char
               field_info->info[i].max_length = 2 + PBM_MAX_NUM_LENGTH_NO_EXT_RECORD;
             }
           }
           field_info = &g_sim_field_info[session_type][1]; /* FDN info*/
           for ( i = 0; i < field_info->num_fields; i++)
           {
             if (field_info->info[i].field_type == PBM_FT_PHONE)
             {
               //increment by 2 to accomodate '+' sign & NULL char
               field_info->info[i].max_length = 2 + PBM_MAX_NUM_LENGTH_NO_EXT_RECORD;
             }
           }
         }
      } else
      {
         UIM_MSG_ERR_0("We need an extension record but no cache.");
      }
   }

   if (pbm_buffers[client_ref].state != PBM_WRITE_DONE)
   {
      pbm_buffers[client_ref].findex += pbm_buffers[client_ref].findex_delta;
      if (pbm_uim_write(client_ref) != PBM_SUCCESS)
         pbm_uim_write_cb(client_ref, PBM_ERROR);
   } else
   {
      pbm_uim_write_cb(client_ref, PBM_SUCCESS);
   }
   return;
}

/*===========================================================================
FUNCTION PBM_USIM_WRITE_CB

DESCRIPTION TODO: Template, needs actual code
  After GSDI has written the extension record to EXT1, PBM_WRITE_ADN_CB
  is called to copy the data in to a pbm_buffer for later processing.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_usim_write_cb( mmgsdi_return_enum_type status,
                        const mmgsdi_cnf_type  *data_ptr)
{
   uint32 client_ref;
   PB_USIM_CACHE *usim_cache;
   pbm_return_type ret = PBM_SUCCESS;
   pbm_addr_cache_s_type *curr_rec;
   pbm_session_enum_type session_type;
   pbm_return_type error_code = PBM_ERROR;

   if (!data_ptr )
   {
      UIM_MSG_ERR_1("PBM Bad pointer passed to function 0x%x",data_ptr);
      (void) rex_set_sigs(PBM_TCB, PBM_CMD_COMPLETE_SIG);
      return;
   }

   client_ref = data_ptr->response_header.client_data;

   if (status != MMGSDI_SUCCESS)
   {
      if (status == MMGSDI_MEMORY_ERROR)
      {
        error_code = PBM_ERROR_SIM_MEMORY_PROBLEM;
      }   
      pbm_uim_write_cb(client_ref, error_code);
      return;
   }
   session_type = pbm_buffers[client_ref].record.session_type;
   usim_cache = pbm_file_id_to_cache(
                                    pbm_buffers[client_ref].record.session_type,
                                    pbm_buffers[client_ref].record.file_id);

   if (!usim_cache)
   {
      pbm_uim_write_cb(client_ref, PBM_ERROR);
      return;
   }

   /* Add to USIM cache */
   if (pbm_buffers[client_ref].state == PBM_WRITE_PREV_USIM)
   {
      rex_enter_crit_sect(&pbm_crit_sect);
      ret = pbm_usim_cache_delete(session_type, usim_cache,
                                  (uint16)pbm_buffers[client_ref].record.index,
                                  FALSE);
      rex_leave_crit_sect(&pbm_crit_sect);
   } else if (pbm_buffers[client_ref].state == PBM_WRITE_CURR_USIM)
   {
      rex_enter_crit_sect(&pbm_crit_sect);
      curr_rec = pbm_buffers[client_ref].write->curr_rec;
      ret = pbm_usim_cache_fields_add(session_type, usim_cache,
                                      (uint16) pbm_buffers[client_ref].record.index,
                                      curr_rec->record_id,
                                      &curr_rec->fields[pbm_buffers[client_ref].findex],
                                      pbm_buffers[client_ref].findex_delta);
      rex_leave_crit_sect(&pbm_crit_sect);
   }

   if (ret != PBM_SUCCESS)
   {
      UIM_MSG_ERR_2("Unable to write to location %d in cache of USIM file %d",
                  pbm_buffers[client_ref].record.index, usim_cache->file_id);
   }

   if (pbm_buffers[client_ref].extension != 0xFF)
   {
      if (adn_cache[pbm_buffers[client_ref].record.session_type].extension_cache)
      {
         rex_enter_crit_sect(&pbm_crit_sect);
         adn_cache[pbm_buffers[client_ref].record.session_type].extension_cache->in_use_array[pbm_buffers[client_ref].extension].uim_device =
         usim_cache->file_id;

         adn_cache[pbm_buffers[client_ref].record.session_type].extension_cache->in_use_array[pbm_buffers[client_ref].extension].index =
         (byte) pbm_buffers[client_ref].record.index;
	 rex_leave_crit_sect(&pbm_crit_sect);
      } else
      {
         UIM_MSG_ERR_0("We need an extension record but no cache.");
      }
   }
   if (pbm_buffers[client_ref].state != PBM_WRITE_DONE)
   {
      pbm_buffers[client_ref].findex += pbm_buffers[client_ref].findex_delta;
      if (pbm_uim_write(client_ref) != PBM_SUCCESS)
         pbm_uim_write_cb(client_ref, PBM_ERROR);
   } else
   {
      pbm_uim_write_cb(client_ref, PBM_SUCCESS);
   }
   return;
}

/*===========================================================================
FUNCTION PBM_SYNC_WRITE_CB

DESCRIPTION

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_sync_write_cb( mmgsdi_return_enum_type status,
                        const mmgsdi_cnf_type  *data_ptr)
{
   uint32 client_ref;
   if (!data_ptr )
   {
      UIM_MSG_ERR_1("PBM Bad pointer passed to function 0x%x ",data_ptr);
      (void) rex_set_sigs(PBM_TCB, PBM_CMD_COMPLETE_SIG);
      return;
   }

   client_ref = data_ptr->response_header.client_data;
   if (status != MMGSDI_SUCCESS)
   {
     pbm_return_type error_code = PBM_ERROR;
     if (status == MMGSDI_MEMORY_ERROR)
     {
       error_code = PBM_ERROR_SIM_MEMORY_PROBLEM;
     }   
     pbm_uim_write_cb(client_ref, error_code);
      return;
   }

   if (pbm_buffers[client_ref].state != PBM_WRITE_DONE)
   {
      pbm_buffers[client_ref].findex += pbm_buffers[client_ref].findex_delta;
      if (pbm_uim_write(client_ref) != PBM_SUCCESS)
         pbm_uim_write_cb(client_ref, PBM_ERROR);
   } else
   {
      pbm_uim_write_cb(client_ref, PBM_SUCCESS);
   }
   return;
}

/*===========================================================================
FUNCTION PBM_EXT_WRITE_CB

DESCRIPTION
  Called after GSDI has written the extension record.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_ext_write_cb( mmgsdi_return_enum_type status,
                       const mmgsdi_cnf_type  *data_ptr)
{
   uint32 client_ref;
   pbm_return_type   ret_val;
   pbm_return_type error_code = PBM_ERROR;

   if (!data_ptr )
   {
      UIM_MSG_ERR_1("PBM Bad pointer passed to function 0x%x",data_ptr);
      (void) rex_set_sigs(PBM_TCB, PBM_CMD_COMPLETE_SIG);
      return;
   }

   client_ref = data_ptr->response_header.client_data;
   if (status == MMGSDI_SUCCESS)
   {
      pbm_buffers[client_ref].record.file_id =
      pbm_buffers[client_ref].m_file_id;
      if (pbm_file_id_to_field_id(pbm_buffers[client_ref].m_file_id) ==
          PBM_FIELD_ADDITIONAL_NUMBER_EXT)
      {
         if (pbm_create_anr_record(pbm_buffers[client_ref].record.session_type,
                                   pbm_buffers[client_ref].m_file_id,
                                   &pbm_buffers[client_ref].record.data.anr,
                                   client_ref) != PBM_SUCCESS)
         {
            pbm_uim_write_cb(client_ref, PBM_ERROR);
         }
      } else
      {
         ret_val =
         pbm_create_sim_record(pbm_buffers[client_ref].record.session_type,
                               pbm_buffers[client_ref].record.file_id,
                               &pbm_buffers[client_ref].record.data.num_text,
                               (int) client_ref);
         if ( ret_val != PBM_SUCCESS )
         {
            pbm_uim_write_cb(client_ref, ret_val);
         }
      }
   } else
   {
     if (status == MMGSDI_MEMORY_ERROR)
     {
       error_code = PBM_ERROR_SIM_MEMORY_PROBLEM;
   }
     pbm_uim_write_cb(client_ref, error_code); 
   }
   return;
}

/*===========================================================================
FUNCTION PBM_INDEX_WRITE_CB

DESCRIPTION
  Called after GSDI has written the extension record.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_index_write_cb( mmgsdi_return_enum_type status,
                         const mmgsdi_cnf_type  *data_ptr)
{
   uint32 client_ref;
   pbm_return_type error_code = PBM_ERROR;
   if (!data_ptr )
   {
      UIM_MSG_ERR_1("PBM Bad pointer passed to function 0x%x",data_ptr);
      (void) rex_set_sigs(PBM_TCB, PBM_CMD_COMPLETE_SIG);
      return;
   }
   client_ref = data_ptr->response_header.client_data;
   if (status != MMGSDI_SUCCESS)
   {
     if (status == MMGSDI_MEMORY_ERROR)
     {
       error_code = PBM_ERROR_SIM_MEMORY_PROBLEM;
     }   
     pbm_uim_write_cb(client_ref, error_code);
      return;
   }

   if (pbm_buffers[client_ref].state != PBM_WRITE_DONE)
   {
      if (pbm_uim_write(client_ref) != PBM_SUCCESS)
         pbm_uim_write_cb(client_ref, PBM_ERROR);
   } else
   {
      pbm_uim_write_cb(client_ref, PBM_SUCCESS);
   }
   return;
}

/*===========================================================================
FUNCTION PBM_USIM_WRITE

DESCRIPTION

PARAMETERS
  field_index
  record.session_type
  write

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
boolean pbm_usim_write (uint32 buffer_index, pbm_return_type *ret)
{
   pbm_async_gsdi_buffer *buffer;
   pbm_addr_cache_s_type *record = NULL;
   pbm_write_record_s_type *write;
   PB_USIM_CACHE *usim_cache;
   uint16 field_index;
   pbm_pb_set_type pb_set;
   pbm_session_enum_type session_type;
   uint8 *index_array = NULL;
   uint16 index = 0;
   int i;

   PBM_CHECK_PTR_RET(ret, FALSE);
   buffer = &pbm_buffers[buffer_index];

   do
   {
      write = buffer->write;
      PBM_CHECK_PTR_BREAK(write, *ret, PBM_ERROR);
      if (buffer->state == PBM_WRITE_PREV_USIM)
      {
         record = write->prev_rec;
         index_array = write->prev_usim_index;
      } else if (buffer->state == PBM_WRITE_CURR_USIM)
      {
         record = write->curr_rec;
         index_array = write->curr_usim_index;
      }

      PBM_CHECK_PTR_BREAK(record, *ret, PBM_ERROR);
      PBM_CHECK_PTR_BREAK(index_array, *ret, PBM_ERROR);

      pb_set = pbm_record_id_to_pb_set(write->rec_id);
      if ( ! pbm_validate_pb_set(pb_set) )
      {
         break; 
      }

      memset(&buffer->record.data, 0, sizeof(pbm_generic_sim_data_s_type));
      buffer->extension = 0xff;

      session_type = buffer->record.session_type;
      field_index = buffer->findex;

      switch (record->fields[field_index].field_id)
      {
         case PBM_FIELD_EMAIL:
            usim_cache = &email_cache[session_type][pb_set];
            buffer->record.file_id = usim_cache->file_id;
            buffer->record.index = index_array[usim_cache->order];
            buffer->findex_delta = 1;
            memset(&buffer->record.data.text.sim_text, 
                   0xFF, sizeof(buffer->record.data.text.sim_text));

            if (buffer->state == PBM_WRITE_PREV_USIM)
               buffer->record.data.text.adn_record_id = buffer->write->rec_id;
            else
               pbm_build_record_from_usim_fields(buffer->write->rec_id,
                                                 &record->fields[field_index],
                                                 buffer->findex_delta, &buffer->record);

            *ret = pbm_usim_write_text(buffer_index, TRUE);
            break;

         case PBM_FIELD_ADDITIONAL_NUMBER_EXT:
            i = 0;
            index = 0;
            //go through the caches, and find the first one that is not
            //in use, and is valid for this type of data
            while ((usim_cache = pbm_get_next_usim_cache(write->rec_id,
                                                         PBM_FIELD_ADDITIONAL_NUMBER_EXT, &i)) != NULL )
	    {
	        if( !buffer->caches_in_use[usim_cache->order])
	        {
		   if( index_array[usim_cache->order] )
		   {
     		      break;
		   }
		}
            }

            if (usim_cache != NULL)
            {
               index = index_array[usim_cache->order];
               if (index)
               {
                  buffer->record.file_id = usim_cache->file_id;
                  buffer->record.index = index;
                  buffer->caches_in_use[usim_cache->order] = TRUE;
               }
            }
            if (!usim_cache || !index)
            {
               *ret = PBM_ERROR;
               break;
            }
            buffer->findex_delta = 1;

            if (buffer->state == PBM_WRITE_PREV_USIM)
               buffer->record.data.text.adn_record_id = buffer->write->rec_id;
            else
               pbm_build_record_from_usim_fields(buffer->write->rec_id,
                                                 &record->fields[field_index],
                                                 buffer->findex_delta, &buffer->record);
            *ret = pbm_usim_write_anr(buffer_index);
            break;

         case PBM_FIELD_OTHERNAME:
            usim_cache = &sne_cache[session_type][pb_set];
            buffer->record.file_id = usim_cache->file_id;
            buffer->record.index = index_array[usim_cache->order];
            buffer->findex_delta = 1;
            memset(&buffer->record.data.text.sim_text, 
                   0xFF, sizeof(buffer->record.data.text.sim_text));

            if (buffer->state == PBM_WRITE_PREV_USIM)
               buffer->record.data.text.adn_record_id = buffer->write->rec_id;
            else
               pbm_build_record_from_usim_fields(buffer->write->rec_id,
                                                 &record->fields[field_index],
                                                 buffer->findex_delta, &buffer->record);

            *ret = pbm_usim_write_text(buffer_index, FALSE);
            break;

         case PBM_FIELD_GRPNAME_ID:
            usim_cache = &grp_cache[session_type][pb_set];
            buffer->record.file_id = usim_cache->file_id;
            buffer->record.index = index_array[usim_cache->order];
            i = 0;
            while (record->fields[i].field_id == PBM_FIELD_GRPNAME_ID &&
                   i < record->num_fields) i++;
            buffer->findex_delta = (uint16) MIN(i, usim_cache->record_length);

            if (buffer->state != PBM_WRITE_PREV_USIM)
               pbm_build_record_from_usim_fields(buffer->write->rec_id,
                                                 &record->fields[field_index],
                                                 buffer->findex_delta, &buffer->record);

            *ret = pbm_usim_write_grp_id(buffer_index);
            break;

         default:
            *ret = PBM_ERROR;
            break;
      }

      if (*ret == PBM_SUCCESS)
         return TRUE;

   } while (FALSE); /*lint !e717 */
   return FALSE;
}

/*===========================================================================
FUNCTION PBM_SYNC_WRITE_SYNC1

DESCRIPTION

PARAMETERS
  record.index
  record.file_id
  record.session_type
  write

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_sync_write_sync1 (uint32 buffer_index)
{
   byte                         *gsdi_buffer=NULL;
   pbm_async_gsdi_buffer        *buffer;
   pbm_session_enum_type           session_type;
   pbm_file_type        file_id;
   pbm_return_type               pbm_retval;
   PB_SYNC1_CACHE               *sync_cache;

   buffer = &pbm_buffers[buffer_index];

   session_type = buffer->record.session_type;
   file_id = buffer->record.file_id;

   sync_cache = pbm_file_id_to_cache(session_type, file_id);
   PBM_CHECK_PTR_RET(sync_cache, PBM_ERROR);

   PBM_MEM_ALLOC(gsdi_buffer, (uint32) sync_cache->record_length);
   PBM_CHECK_PTR_RET(gsdi_buffer, PBM_ERROR_MEM_FULL);
   memset(gsdi_buffer, 0, (uint32)sync_cache->record_length);


   switch (file_id)
   {
      case PBM_FILE_PUID:
         {
            uint16 data = buffer->write->curr_rec->unique_id;
            gsdi_buffer[0] = (data & 0xFF00) >> 8;
            gsdi_buffer[1] = (data & 0x00FF);
            break;
         }

      case PBM_FILE_CC:
         {
            uint16 data = buffer->write->sync.cc;
            gsdi_buffer[0] = (data & 0xFF00) >> 8;
            gsdi_buffer[1] = (data & 0x00FF);
            break;
         }

      case PBM_FILE_PSC:
         {
            uint32 data = buffer->write->sync.psc;
            gsdi_buffer[0] = (data & 0xFF000000) >> 24;
            gsdi_buffer[1] = (data & 0x00FF0000) >> 16;
            gsdi_buffer[2] = (data & 0x0000FF00) >> 8;
            gsdi_buffer[3] = (data & 0x000000FF);
            break;
         }

      default:
         UIM_MSG_ERR_1("Invalid file id %d", file_id);
         break;
   }

   pbm_buffers[buffer_index].gsdi_resp_type = PBM_MMGSDI_WRITE_CB;
   pbm_retval = pbm_mmgsdi_uim_write(session_type,
                                     sync_cache->file_id,
                                     sync_cache->file_type,
                                     0, /* transparent file */
                                     gsdi_buffer,
                                     sync_cache->record_length,
                                     buffer_index,
                                     pbm_mmgsdi_async_cb);
   if (pbm_retval != PBM_SUCCESS)
   {
      PBM_MEM_FREEIF(gsdi_buffer);
      return PBM_ERROR;
   }

   PBM_MEM_FREEIF(gsdi_buffer);
   return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_SYNC_WRITE_SYNC2

DESCRIPTION

PARAMETERS
  record.index
  record.file_id
  record.session_type
  write

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_sync_write_sync2 (uint16 data, uint32 buffer_index)
{
   byte                         *gsdi_buffer=NULL;
   pbm_async_gsdi_buffer        *buffer;
   pbm_session_enum_type           session_type;
   pbm_file_type        base_file_id; /* this would always be the base file id like PBM_FILE_PBC etc*/
   pbm_file_type         file_id; /* this would point to the actual file id PBM_FILE_PBC1 etc*/
   pbm_return_type               pbm_retval;
   PB_SYNC2_CACHE               *sync_cache;
   pbm_pb_set_type               pb_set;
   int                           index;
   mmgsdi_file_structure_enum_type filetype;

   buffer = &pbm_buffers[buffer_index];

   session_type = buffer->record.session_type;
   base_file_id = buffer->record.file_id;

   sync_cache = pbm_file_id_to_cache(session_type, base_file_id);
   PBM_CHECK_PTR_RET(sync_cache, PBM_ERROR);

   PBM_MEM_ALLOC(gsdi_buffer, (uint32) sync_cache->record_length);
   PBM_CHECK_PTR_RET(gsdi_buffer, PBM_ERROR_MEM_FULL);
   memset(gsdi_buffer, 0, (uint32)sync_cache->record_length);

   pb_set = pbm_record_id_to_pb_set(buffer->write->rec_id);
   if ( ! pbm_validate_pb_set(pb_set) )
   {
     PBM_MEM_FREEIF(gsdi_buffer);
     return PBM_ERROR; 
   }

   file_id = sync_cache->file_ids[pb_set];
   filetype = sync_cache->file_type;
   index = pbm_sync2_record_index_to_fileloc(sync_cache, buffer->record.index);

   switch (base_file_id)
   {
      case PBM_FILE_UID:
         {
            gsdi_buffer[0] = (data & 0xFF00) >> 8;
            gsdi_buffer[1] = (data & 0x00FF);
            break;
         }

      case PBM_FILE_PBC:
         {
           //only need to write hidden data, always reset the Entry Control Information
           gsdi_buffer[1] = (data & PBM_PBC_HIDDEN_MASK);
         break;
         }

      default:
         UIM_MSG_ERR_1("Invalid file id %d", base_file_id);
         break;
   }

   pbm_buffers[buffer_index].gsdi_resp_type = PBM_MMGSDI_WRITE_CB;
   pbm_retval = pbm_mmgsdi_uim_write(session_type,
                                     file_id,
                                     filetype,
                                     index,
                                     gsdi_buffer,
                                     sync_cache->record_length,
                                     buffer_index,
                                     pbm_mmgsdi_async_cb);
   if (pbm_retval != PBM_SUCCESS)
   {
      PBM_MEM_FREEIF(gsdi_buffer);
      return PBM_ERROR;
   }

   PBM_MEM_FREEIF(gsdi_buffer);
   return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_IAP_WRITE

DESCRIPTION

PARAMETERS
  record.session_type
  write

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_iap_write (uint32 buffer_index)
{
   pbm_async_gsdi_buffer        *buffer;
   pbm_addr_cache_s_type        *record = NULL;
   pbm_write_record_s_type      *write;
   PB_USIM_CACHE                *usim_cache;
   pbm_pb_set_type               pb_set;
   uint16                        index = 0;
   PB_MASTER_USIM_CACHE         *m_usim_cache;
   int                           i;
   pbm_iap_record_s_type        *iap_record;
   uint8                        *index_array = NULL;

   buffer = &pbm_buffers[buffer_index];

   write = buffer->write;
   PBM_CHECK_PTR_RET(write, PBM_ERROR);

   if (buffer->state == PBM_WRITE_PREV_USIM)
   {
      record = write->prev_rec;
      index_array = write->prev_usim_index;
   } else if (buffer->state == PBM_WRITE_CURR_USIM)
   {
      record = write->curr_rec;
      index_array = write->curr_usim_index;
   }
   PBM_CHECK_PTR_RET(record, PBM_ERROR);
   pb_set = pbm_record_id_to_pb_set(write->rec_id);
   if ( ! pbm_validate_pb_set(pb_set) )
   {
       return PBM_ERROR; 
   }

   m_usim_cache = &adn_m_usim_cache[buffer->record.session_type][pb_set];
   if (!m_usim_cache->iap_num_records)
   {
      UIM_MSG_ERR_0("IAP file not present");
      return PBM_ERROR;
   }

   iap_record = &buffer->record.data.iap;
   memset(iap_record, 0xFF, sizeof(pbm_iap_record_s_type));

   index = (uint16) pbm_record_id_to_fileloc(write->rec_id);
   if (buffer->state != PBM_WRITE_PREV_USIM && index_array)
   {
      for (i = 1; i <= m_usim_cache->iap_rec_len; i++)
      {
         usim_cache = m_usim_cache->usim_cache_array[i];
         if (usim_cache && usim_cache->mapping_type == PBM_MAPPING_TYPE_2)
         {
            iap_record->index[i-1] = index_array[i] ? index_array[i] : 0xFF;
         }
      }
   }

   buffer->record.file_id = m_usim_cache->file_id;
   buffer->record.index = index;

   return pbm_usim_write_iap(buffer_index);
}

/*===========================================================================
FUNCTION pbm_usim_write_text

DESCRIPTION
  pbm_return_type pbm_usim_write_text(uint32 buffer_index, boolean gsm7_only)

  This function will write a text record to a USIM file.  The encoding
  of the text is either GSM7 or, if gsm7_only is false, one of the UCS2
  encoding schemes allowed.

PARAMETERS
  buffer_index: [in] Contains the index to where we are writing the info.
  gsm7_only: [in] TRUE if we must encode in GSM7 alphabet only.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_usim_write_text (uint32 buffer_index, boolean gsm7_only)
{
   byte                   *gsdi_buffer=NULL;
   PB_USIM_CACHE          *usim_cache;
   pbm_async_gsdi_buffer  *buffer;
   mmgsdi_file_structure_enum_type filetype;
   pbm_session_enum_type        session_type;
   pbm_return_type         pbm_retval;
   uint16                  text_size;
   pbm_text_record_s_type *text_record;

   buffer = &pbm_buffers[buffer_index];
   usim_cache = pbm_file_id_to_cache(buffer->record.session_type,
                                     buffer->record.file_id);
   PBM_CHECK_PTR_RET(usim_cache, PBM_ERROR);

   PBM_MEM_ALLOC(gsdi_buffer, (uint32) usim_cache->record_length);
   PBM_CHECK_PTR_RET(gsdi_buffer, PBM_ERROR_MEM_FULL);
   memset(gsdi_buffer, 0xFF, (uint32) usim_cache->record_length);

   text_record = &buffer->record.data.text;
   filetype = usim_cache->file_type;
   session_type = buffer->record.session_type;

   text_size = (uint16) usim_cache->record_length;
   if ((text_size >= 2) && (usim_cache->mapping_type == PBM_MAPPING_TYPE_2))
      text_size -= 2;

   if (((uint16*)text_record->sim_text)[0] != 0xFFFF)   
   {
     memscpy(gsdi_buffer,text_size,text_record->sim_text,text_size);
   }

   if (usim_cache->mapping_type == PBM_MAPPING_TYPE_2 &&
       buffer->state == PBM_WRITE_CURR_USIM)
   {
      if (pbm_fill_adn_sfi(text_record->adn_record_id, &gsdi_buffer[text_size],
                                                                  session_type)
          != PBM_SUCCESS)
      {
         UIM_MSG_ERR_0("Unable to fill ADN SFI");
         PBM_MEM_FREEIF(gsdi_buffer);
         return PBM_ERROR;
      }
   }

   pbm_buffers[buffer_index].gsdi_resp_type = PBM_MMGSDI_WRITE_CB;
   pbm_retval = pbm_mmgsdi_uim_write(session_type,
                                     usim_cache->file_id,
                                     filetype,
                                     buffer->record.index,
                                     gsdi_buffer,
                                     usim_cache->record_length,
                                     buffer_index,
                                     pbm_mmgsdi_async_cb);
   if (pbm_retval != PBM_SUCCESS)
   {
      PBM_MEM_FREEIF(gsdi_buffer);
      return PBM_ERROR;
   }
   PBM_MEM_FREEIF(gsdi_buffer);
   return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_USIM_WRITE_ANR

DESCRIPTION

PARAMETERS
  field_index
  record.sim_id
  write

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_usim_write_anr (uint32 buffer_index)
{
   PB_USIM_CACHE         *usim_cache;
   pbm_async_gsdi_buffer *buffer;
   pbm_session_enum_type      session_type;
   pbm_anr_record_s_type *anr_record;
   pbm_ext_cache_s_type  *ext_cache;

   buffer = &pbm_buffers[buffer_index];
   usim_cache = pbm_file_id_to_cache(buffer->record.session_type,
                                     buffer->record.file_id);
   PBM_CHECK_PTR_RET(usim_cache, PBM_ERROR);

   session_type = buffer->record.session_type;
   anr_record = &buffer->record.data.anr;

   /* Do we need an extension record?  */
   ext_cache = &ext1_cache[session_type];

   if ((anr_record->anr_data[1] - 1) > PBM_SIM_DIALING_NUMBER_LEN)
   {
      pbm_buffers[buffer_index].m_file_id = buffer->record.file_id;
      return pbm_create_ext_record(buffer_index, ext_cache, 
                                   &anr_record->anr_data[1],
                                   (uint16)buffer->record.index,
                                   usim_cache->file_id);
   }

   /* see if we used to need one, but no longer */
   else if (pbm_check_extension_record(ext_cache, (uint16)buffer->record.index,
                                       usim_cache->file_id))
   {
      pbm_buffers[buffer_index].m_file_id = buffer->record.file_id;
      return pbm_clear_ext_record(buffer_index,
                                  ext_cache,
                                  (uint16)buffer->record.index,
                                  usim_cache->file_id);
   } else
   {
      pbm_buffers[buffer_index].extension = 0xff;
      return pbm_create_anr_record(session_type, buffer->record.file_id,
                                   anr_record, buffer_index);
   }
}

/*===========================================================================
FUNCTION PBM_USIM_WRITE_GRP_ID

DESCRIPTION

PARAMETERS
  field_index
  record.session_type
  write

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_usim_write_grp_id (uint32 buffer_index)
{
   byte                  *gsdi_buffer=NULL;
   PB_USIM_CACHE         *usim_cache;
   pbm_async_gsdi_buffer *buffer;
   mmgsdi_file_structure_enum_type filetype;
   pbm_session_enum_type       session_type;
   pbm_return_type        pbm_retval;
   int                    i;

   buffer = &pbm_buffers[buffer_index];
   usim_cache = pbm_file_id_to_cache(buffer->record.session_type,
                                     buffer->record.file_id);

   PBM_CHECK_PTR_RET(usim_cache, PBM_ERROR);
   PBM_MEM_ALLOC(gsdi_buffer, (uint32) usim_cache->record_length);
   PBM_CHECK_PTR_RET(gsdi_buffer, PBM_ERROR_MEM_FULL);

   //unused group entries should be 0
   memset(gsdi_buffer, 0, (uint32) usim_cache->record_length);
   filetype = usim_cache->file_type;
   session_type = buffer->record.session_type;

   for (i = 0; i < usim_cache->record_length; i++)
   {
     /* Coding:TS 31.1.02 4.4.2.6 EFGRP (Grouping file)
      * - '00' – no group indicated;
      *   'XX' – record number in EFGAS containing the alpha string naming the
      *          group of which the phone book entry is a member.
      */
      if (buffer->record.data.grp.group_id[i] > 0 &&
          buffer->record.data.grp.group_id[i] < grpname_cache[session_type].num_of_records)
      {
        gsdi_buffer[i] = (uint8) buffer->record.data.grp.group_id[i];
      }
   }

   pbm_buffers[buffer_index].gsdi_resp_type = PBM_MMGSDI_WRITE_CB;
   pbm_retval = pbm_mmgsdi_uim_write(session_type,
                                     usim_cache->file_id,
                                     filetype,
                                     buffer->record.index,
                                     gsdi_buffer,
                                     usim_cache->record_length,
                                     buffer_index,
                                     pbm_mmgsdi_async_cb);
   if (pbm_retval != PBM_SUCCESS)
   {
      PBM_MEM_FREEIF(gsdi_buffer);
      return PBM_ERROR;
   }

   PBM_MEM_FREEIF(gsdi_buffer);
   return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_USIM_WRITE_IAP

DESCRIPTION

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_usim_write_iap(uint32 buffer_index)
{
   byte                         *gsdi_buffer=NULL;
   PB_MASTER_USIM_CACHE         *index_cache;
   pbm_async_gsdi_buffer        *buffer;
   mmgsdi_file_structure_enum_type filetype;
   pbm_session_enum_type              session_type;
   pbm_return_type               pbm_retval;

   buffer = &pbm_buffers[buffer_index];
   index_cache = pbm_file_id_to_cache(buffer->record.session_type,
                                      buffer->record.file_id);
   PBM_CHECK_PTR_RET(index_cache, PBM_ERROR);

   PBM_MEM_ALLOC(gsdi_buffer, (uint32) index_cache->iap_rec_len);
   PBM_CHECK_PTR_RET(gsdi_buffer, PBM_ERROR_MEM_FULL);
   memset(gsdi_buffer, 0, (uint32)index_cache->iap_rec_len);
   filetype = index_cache->file_type;
   session_type = buffer->record.session_type;

   memscpy(gsdi_buffer, (uint32) index_cache->iap_rec_len, buffer->record.data.iap.index,
          (uint32) index_cache->iap_rec_len);

   pbm_buffers[buffer_index].gsdi_resp_type = PBM_MMGSDI_WRITE_CB;
   pbm_retval = pbm_mmgsdi_uim_write(session_type,
                                     index_cache->file_id,
                                     filetype,
                                     buffer->record.index,
                                     gsdi_buffer,
                                     index_cache->iap_rec_len,
                                     buffer_index,
                                     pbm_mmgsdi_async_cb);
   if (pbm_retval != PBM_SUCCESS)
   {
      PBM_MEM_FREEIF(gsdi_buffer);
      return PBM_ERROR;
   }

   PBM_MEM_FREEIF(gsdi_buffer);
   return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_RESET_PBC

DESCRIPTION

DEPENDENCIES
  Should be called only from PBM task and only during initialization
  PBC file is present (pb_cache->m_sync_cache->pbc_cache is not NULL).

SIDE EFFECTS
===========================================================================*/
void pbm_reset_pbc (pbm_session_enum_type session_type, int index)
{
   int i;
   pbm_phonebook_type pb_id = {PBM_GPB};
   pbm_record_id_type rec_id;
   PB_CACHE *pb_cache = &adn_cache[session_type];

   PBM_CHECK_PTR2_RET(pb_cache->m_sync_cache, pbc_cache[session_type].data_array, VOID);
   pb_cache->m_sync_cache->reset_pbc = TRUE;

   for (i = index; i <= pbc_cache[session_type].num_of_records; i++)
   {
      if (pbc_cache[session_type].data_array[i] & PBM_PBC_MODIFIED_MASK)
      {
         pb_id = pbm_device_type_to_phonebook_type(session_type, PBM_ADN);
         rec_id = pbm_session_location_to_record_id(pb_id, i);
         if (pbm_delete_usim_fields(rec_id, pbm_reset_pbc_cb) == PBM_SUCCESS)
            return;
         else
            UIM_MSG_ERR_1("Unable to reset pbc record %d", rec_id);
      }
   }

   pb_cache->m_sync_cache->reset_pbc = FALSE;
   PBM_SET_PB_CACHE_READY(pb_cache);

   pbm_initialize_index[session_type]++;
   pbm_init_entries(session_type);
}

/*===========================================================================
FUNCTION PBM_RESET_PBC_CB

DESCRIPTION

DEPENDENCIES
  Should be called only from PBM task and only during initialization

SIDE EFFECTS
===========================================================================*/
void pbm_reset_pbc_cb (pbm_writecb_data_s_type *cb_data)
{
   int index;
   pbm_session_enum_type session_type;

   PBM_CHECK_PTR_RET(cb_data, VOID);

   index = pbm_record_id_to_location(cb_data->sess_rec_id);
   session_type = pbm_record_id_to_session_type(cb_data->sess_rec_id);

   if (cb_data->ret != PBM_SUCCESS)
   {
      UIM_MSG_ERR_2("Failed to reset PBC entry for record %d session %d",
                  index, session_type);
   }

   pbm_reset_pbc(session_type, index+1);
}

/*===========================================================================
FUNCTION PBM_DELETE_USIM_FIELDS

DESCRIPTION

DEPENDENCIES
  Should be called only from PBM task and only during initialization.
  Cache status should not be ready at this point ie clients should not be
  trying to write.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_delete_usim_fields (pbm_record_id_type rec_id,
                                        PBM_WRITE_CB_FUNC proc_func)
{
   pbm_phonebook_type       pb_id = pbm_session_record_id_to_phonebook_type(rec_id);
   PB_CACHE                *pb_cache = pbm_pb_id_to_cache(pb_id);
   pbm_pb_set_type          pb_set = pbm_record_id_to_pb_set(rec_id);
   int                      index = pbm_record_id_to_fileloc(rec_id);
   PB_MASTER_USIM_CACHE    *m_usim_cache;
   PB_USIM_CACHE           *usim_cache;
   int                      usim_index;
   uint8                   *prev_index = NULL;
   pbm_write_record_s_type *write_cmd = NULL;
   pbm_addr_cache_s_type   *prev_record = NULL;
   pbm_addr_cache_s_type   *curr_record = NULL;
   int                      num_usim_fields = 0;
   pbm_field_s_type       **usim_fields = NULL;
   pbm_usim_cache_s_type   *usim_rec = NULL;
   uint32                  *field_lengths = NULL;
   int i, j;

   if ( ! pbm_validate_pb_set(pb_set) )
   {
       return PBM_ERROR; 
   }
   PBM_CHECK_PTR_RET(pb_cache, PBM_ERROR);

   m_usim_cache = pb_cache->m_usim_cache[pb_set];

   if (!m_usim_cache)
      return PBM_SUCCESS;

   PBM_MEM_ALLOC(usim_fields, PBM_MAX_USIM_FIELDS_PER_REC *
                 sizeof(pbm_field_s_type *));
   PBM_CHECK_PTR_RET(usim_fields, PBM_ERROR);

   /* Find number of usim fields and usim field pointers */
   for ( i = 1; i <= m_usim_cache->num_caches; i++)
   {
      usim_cache = m_usim_cache->usim_cache_array[i];
      if (!usim_cache)
         continue;

      usim_index = m_usim_cache->index_array[i][index];
      if (usim_index)
      {
         usim_rec = usim_cache->pb_cache_array[usim_index];
         if (usim_rec)
         {
            for (j = 0; j < usim_rec->num_fields; j++)
               usim_fields[num_usim_fields++] = &usim_rec->fields[j];
         } else
         {
            UIM_MSG_ERR_2("Invalid record found in USIM %d index %d",
                        usim_cache->file_id, usim_index);
         }
      }
   }

   /* Allocate all required memory */
   PBM_MEM_ALLOC(prev_index, (uint32) (m_usim_cache->num_caches+1) *
                 sizeof(uint8));
   PBM_MEM_ALLOC(field_lengths, (uint32)num_usim_fields*sizeof(uint32));

   write_cmd = (pbm_write_record_s_type *)
               pbm_cmd_alloc(sizeof(pbm_write_record_s_type));
   prev_record = pbm_cache_record_new();
   curr_record = pbm_cache_record_new();

   if (!prev_index || (num_usim_fields && !field_lengths) 
       || !write_cmd ||!prev_record || !curr_record)
   {
      UIM_MSG_ERR_0("Unable to allocate required memory");
      PBM_MEM_FREEIF(prev_index);
      PBM_MEM_FREEIF(field_lengths);
      PBM_MEM_FREEIF(write_cmd);
      PBM_MEM_FREEIF(prev_record);
      PBM_MEM_FREEIF(curr_record);
      PBM_MEM_FREEIF(usim_fields);
      return PBM_ERROR_MEM_FULL;
   }

   for (i = 0; i < num_usim_fields; i++)
      field_lengths[i] = usim_fields[i]->data_len;

   if(num_usim_fields)
   {
   if (!pbm_allocate_fields_internal(&prev_record->fields,
                                     (uint32)num_usim_fields, field_lengths))
   {
      PBM_MEM_FREEIF(prev_index);
      PBM_MEM_FREEIF(field_lengths);
      PBM_MEM_FREEIF(write_cmd);
      PBM_MEM_FREEIF(prev_record);
      PBM_MEM_FREEIF(curr_record);
      PBM_MEM_FREEIF(usim_fields);
      return PBM_ERROR_MEM_FULL;
   }
   }

   for (i = 0; i < num_usim_fields; i++)
      pbm_fill_field(&prev_record->fields[i], usim_fields[i]->field_id,
                     usim_fields[i]->field_type, pbm_get_field_data_ptr(usim_fields[i],0),
                     (uint16) field_lengths[i]);

   prev_record->num_fields = (uint16) num_usim_fields;
   prev_record->record_id = rec_id;
   prev_record->unique_id = 0;

   curr_record->record_id = rec_id;
   curr_record->unique_id =
   pbm_uim_generate_sync_values(pb_cache, &write_cmd->sync, TRUE);

   PBM_MEM_FREEIF(field_lengths);
   PBM_MEM_FREEIF(usim_fields);

   if (!pbm_copy_usim_index(rec_id, prev_index))
   {
      UIM_MSG_ERR_0("Unable to copy previous index array");
      PBM_MEM_FREEIF(prev_index);
      PBM_MEM_FREEIF(write_cmd);
      (void) pbm_cache_record_free(prev_record);
      /* curr_record is not having fields_ptr and hence PBM_MEM_FREEIF is fine */
      PBM_MEM_FREEIF(curr_record);
      return PBM_ERROR;
   }

   if (pbm_master_usim_cache_update(rec_id, prev_index, NULL)
       != PBM_SUCCESS)
   {
      UIM_MSG_ERR_0("Unable to update master index");
   }

   write_cmd->cmd_type   = PBM_CMD_WRITE;
   write_cmd->rec_id     = rec_id;
   write_cmd->prev_rec   = prev_record;
   write_cmd->proc_func  = proc_func;

   write_cmd->curr_rec        = curr_record;
   write_cmd->curr_usim_index = NULL;
   write_cmd->prev_usim_index = prev_index;

   pbm_client_cmd_q_put(PBM_CMD_PTR(write_cmd));
   return PBM_SUCCESS;
}



/*===========================================================================
FUNCTION PBM_FILENAME_TO_REFRESH_GROUP

DESCRIPTION
  This function will return the refresh file group corresponding to the
  pbm file.

SIDE EFFECTS
===========================================================================*/
pbm_file_type pbm_filename_to_refresh_group(
                                               pbm_file_type pbm_file,
                                               pbm_session_enum_type session_type
                                               )
{
   int i;
   uint32 j = 0;

   j =pbm_util_return_fileset_for_session(session_type);

   /* find this file */
   for ( i = 0; i < (int) ARR_SIZE(pbm_file_to_uim_file); i++)
   {
     if (pbm_file == pbm_file_to_uim_file[i].pbm_session_filename[j])
     {
        return pbm_file_to_uim_file[i].pbm_file_group[j];
     }
   }

   UIM_MSG_ERR_2("Could not find group for file %d, session %d, USIM_LOCAL",
               pbm_file, session_type); /*, (int)USE_USIM_LOCAL(session_type)*/
   return PBM_FILE_NONE;
}



/*===========================================================================
FUNCTION PBM_ECC_BCD_TO_ASCII

DESCRIPTION
  This function converts an ECC number into an ASCII string.  Note that
  the input string must be at least 7 characters in length.  This function
  will always set the first 7 characters.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int pbm_ecc_bcd_to_ascii (const byte *ecc_number, char *ascii_out )
{
   int bcd_index;
   byte asc_1 = 0;
   byte asc_2 = 0;
   int returned_length = -1;
   int ascii_index = 0;

   PBM_CHECK_PTR2_RET(ecc_number, ascii_out, 0);

   /*******************************/
   /* Decode ECC's BCD into ascii */
   /*******************************/
   for ( bcd_index = 0; bcd_index < 3; bcd_index++)
   {
      asc_1 = (ecc_number[bcd_index] & 0x0F);
      asc_2 = (ecc_number[bcd_index] & 0xF0) >> 4;

      if ((returned_length == -1) && ((ascii_out[ascii_index++] = pbm_bcd_to_char_mapping[asc_1]) == '\0'))
         returned_length = ascii_index - 1;
      if ((returned_length == -1) && ((ascii_out[ascii_index++] = pbm_bcd_to_char_mapping[asc_2]) == '\0'))
         returned_length = ascii_index - 1;
   }

   /* Null terminate the ascii string */
   ascii_out[ascii_index] = '\0';
   if (returned_length == -1) returned_length = ascii_index;

   return returned_length;
}

/*===========================================================================
FUNCTION PBM_SEEK_UNUSED_RECORDS

DESCRIPTION
   Get the list of unused records via mmgsdi_seek(). These are returned via
   the pbm_mmgsdi_seek_cb().

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
static pbm_return_type pbm_seek_unused_records(pbm_session_enum_type session_type,
                                               pbm_file_type file_id)
{
   pbm_cache_type_e_type cache_type;
   uint32 client_ref;

   UIM_MSG_HIGH_2("PBM starts SEEK of PBM file: session %d, File id %d",
                session_type, file_id);

   /* Free data allocated for the previous file_id */
   pbm_free_seek_data(session_type);
   pbm_init_seek_data(session_type);

   cache_type = pbm_file_id_to_cache_type(file_id);

   switch (cache_type)
   {
      case PBM_CACHE_TYPE_PB:
         {
            PB_CACHE          *pb_cache = (PB_CACHE *)pbm_file_id_to_cache(
                                                                 session_type, file_id);
            pbm_pb_set_type    pb_set = pbm_file_id_to_pb_set(file_id);

            PBM_CHECK_PTR_RET(pb_cache, PBM_ERROR);  /* KW error */
            pbm_seek_data[session_type].num_files      = pb_cache->num_files;
            pbm_seek_data[session_type].num_of_records = pb_cache->num_of_records;
            if(pb_set != PBM_USIM_PB_INVALID)
            {
              pbm_seek_data[session_type].record_length = pb_cache->record_length[pb_set];
            }
            else
            { 
              pbm_seek_data[session_type].record_length = pb_cache->record_length[0];
            }

            memscpy(pbm_seek_data[session_type].file_ids, sizeof(pbm_seek_data[session_type].file_ids),
                    pb_cache->file_ids, sizeof(pbm_seek_data[session_type].file_ids));

            memscpy(pbm_seek_data[session_type].records_in_pb_set,
                    sizeof(pbm_seek_data[session_type].records_in_pb_set),
                   pb_cache->records_in_pb_set,
                   sizeof(pbm_seek_data[session_type].records_in_pb_set));
            break;
         }

      case PBM_CACHE_TYPE_USIM:
         {
            PB_USIM_CACHE *usim_cache =
            (PB_USIM_CACHE *)pbm_file_id_to_cache(session_type, file_id);
            PBM_CHECK_PTR_RET(usim_cache, PBM_ERROR);  /* KW error */
            pbm_seek_data[session_type].num_files      = 1;
            pbm_seek_data[session_type].num_of_records = usim_cache->num_of_records;
            pbm_seek_data[session_type].record_length  = usim_cache->record_length;
            pbm_seek_data[session_type].file_ids[PBM_USIM_PB_ONE] =
            usim_cache->file_id;
            pbm_seek_data[session_type].records_in_pb_set[PBM_USIM_PB_ONE] =
            usim_cache->num_of_records;
            break;
         }

      case PBM_CACHE_TYPE_INDEX:
         {
            PB_MASTER_USIM_CACHE *m_usim_cache =
            (PB_MASTER_USIM_CACHE *)pbm_file_id_to_cache(session_type,
                                                         file_id);
            PBM_CHECK_PTR_RET(m_usim_cache, PBM_ERROR);  /* KW error */
            pbm_seek_data[session_type].num_files      = 1;
            pbm_seek_data[session_type].num_of_records =
            m_usim_cache->iap_num_records;

            pbm_seek_data[session_type].record_length  = m_usim_cache->iap_rec_len;
            pbm_seek_data[session_type].file_ids[PBM_USIM_PB_ONE] =
            m_usim_cache->file_id;

            pbm_seek_data[session_type].records_in_pb_set[PBM_USIM_PB_ONE] =
            m_usim_cache->iap_num_records;
            break;
         }

      case PBM_CACHE_TYPE_SYNC2:
         {
            PB_SYNC2_CACHE *sync2_cache =
            (PB_SYNC2_CACHE *) pbm_file_id_to_cache(session_type,
                                                    file_id);
            PBM_CHECK_PTR_RET(sync2_cache, PBM_ERROR);  /* KW error */
            pbm_seek_data[session_type].num_files      = sync2_cache->num_files;
            pbm_seek_data[session_type].num_of_records = sync2_cache->num_of_records;
            pbm_seek_data[session_type].record_length  = sync2_cache->record_length;
            memscpy(pbm_seek_data[session_type].file_ids,
                    sizeof(pbm_seek_data[session_type].file_ids),
                    sync2_cache->file_ids,
                   sizeof(pbm_seek_data[session_type].file_ids));
            memscpy(pbm_seek_data[session_type].records_in_pb_set,
                    sizeof(pbm_seek_data[session_type].records_in_pb_set),
                   sync2_cache->records_in_pb_set,
                   sizeof(pbm_seek_data[session_type].records_in_pb_set));
            break;
         }

      default:
         return PBM_ERROR;
   }


   pbm_alloc_seek_data(session_type,
                       pbm_seek_data[session_type].num_of_records);

   client_ref =(uint32)( session_type * PBM_FILE_MAX_REQUESTS) + (uint32) file_id;
   if (PBM_SUCCESS == pbm_send_seek_request(session_type, client_ref))
   {
      return PBM_SUCCESS;
   }

      UIM_MSG_ERR_2("PBM SEEK failed of PBM file: sim %d, File id %d",
                  session_type, file_id);


      /* SEEK failed, so try reading records sequentially */
      if (PBM_SUCCESS == pbm_start_record_read(session_type, file_id, 1))
      {
         return PBM_SUCCESS;
      }


   /* Something went wrong, start reading next file */
   return PBM_ERROR;
}



/*===========================================================================
FUNCTION pbm_start_record_read

DESCRIPTION
   This function is called after the SEEK operation is complete to start
   reading records

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_start_record_read(pbm_session_enum_type session_type,
                                      pbm_file_type file_id,
                                      int record_num)
{
   pbm_cache_type_e_type cache_type = pbm_file_id_to_cache_type(file_id);

   switch (cache_type)
   {
      case PBM_CACHE_TYPE_PB:
         return pbm_sim_read(session_type, file_id, record_num, pbm_sim_read_cb);

      case PBM_CACHE_TYPE_USIM:
         return pbm_usim_read(session_type, file_id, record_num,
                              pbm_usim_read_cb);

      case PBM_CACHE_TYPE_INDEX:
         return pbm_index_read(session_type, file_id, record_num,
                               pbm_index_read_cb);

      case PBM_CACHE_TYPE_SYNC2:
         return pbm_sync_read(session_type, file_id, record_num,
                              pbm_sync2_read_cb);

      default:
         UIM_MSG_ERR_2("Unknown file id %d in sim %d", file_id, session_type);
         break;
   }

   return PBM_ERROR;
}


/*===========================================================================
FUNCTION PBM_SERVICE_INFO_CB

DESCRIPTION
  This function is called by MMGSDI for sending info for service available on
  card or not

PARAMETERS

SIDE EFFECTS
===========================================================================*/
void pbm_service_info_cb(mmgsdi_return_enum_type status,
                         mmgsdi_cnf_enum_type    data_type,
                         const mmgsdi_cnf_type  *data_ptr)
{
   boolean service;
   pbm_return_type pbm_status = PBM_SUCCESS;
   pbm_file_type file_id;
   uint32 client_ref;
   PB_CACHE *cache;
   pbm_session_enum_type session_type;
   service = data_ptr->srv_available_cnf.srv_available;
   session_type = pbm_mmgsdi_session_to_pbm_session(
                                                   data_ptr->srv_available_cnf.response_header.session_id);

   if ( MMGSDI_SRV_AVAILABLE_CNF != data_type )
   {
      UIM_MSG_ERR_1("invalid confirmation type %d in pbm_service_info_cb",
                  data_type);
      return;
   }
   /* retrieve it from Client Ref passed */
   client_ref = data_ptr->response_header.client_data;
   if (client_ref >= ARR_SIZE(pbm_fileinfo_requests))
   {
      UIM_MSG_ERR_1("Invalid client_ref found: %d\n", client_ref);
      return;
   }


   // * Restart initialization if required */
   if (pbm_session_restart & PBM_SESSIONID_TO_SESSION_MASK(session_type))
   {
      UIM_MSG_HIGH_1("PBM restarting init of session_type %d", session_type);
      pbm_restart_fileinfo_internal(session_type);
      return;
   }

   // * Stop initialization if required */
   if (!(pbm_session_initializing & PBM_SESSIONID_TO_SESSION_MASK(session_type)))
   {
      UIM_MSG_HIGH_1("PBM stopping init of SIM %d", session_type);
      return;
   }

   file_id = (pbm_file_type) (client_ref % (int)PBM_FILE_MAX_REQUESTS);

   /* if service available */
   if ( TRUE == service )
   {
      pbm_status = pbm_mmgsdi_get_file_attributes(
                                                 session_type,                                /* Session id */
                                                 pbm_fileinfo_requests[client_ref].file_id,          /* file_id */
                                                 client_ref,                                    /* Client Ref */
                                                 pbm_mmgsdi_fileinfo_cb);               /* Async CB. */

   }
   //if we are waiting for the file attributes then dont do anything..
   //otherwise we should process the next file
   if (pbm_status != PBM_SUCCESS || TRUE != service)
   {

      /* Could not get file attributes, so this file will not be read */
      cache = pbm_file_id_to_cache(session_type, (pbm_file_type)file_id);
      if (cache != NULL && cache->num_of_records==0 
          && pbm_file_id_to_cache_type(file_id) == PBM_CACHE_TYPE_PB)
         cache->status = PBM_STATUS_NOT_AVAILABLE;

      /* After we process the results, put the next request in. */
      pbm_make_fileinfo_request(session_type, (1 + (uint32) file_id));
   }

}

/*===========================================================================
FUNCTION pbm_is_init_completed_for_slot

DESCRIPTION
will let the client know whether the init process of PBM is comlpeted or not for sessions belonging to one slot.


DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_is_init_completed_for_slot(pbm_slot_type slot_id)
{
   boolean init_complete = FALSE;
   uint8 i;

   for (i = 0; i < (uint8) MAX_UIM_APPS; i++)
   {
      //check for the initialisation of card inserted sessions only
      if(pbm_session_info[i].slot_id == (uint16) slot_id)
      {
         if ((pbm_session_activated & PBM_SESSIONID_TO_SESSION_MASK(i)) != 0)
         {
            /* Also check if session is active or not */
            if ((TRUE == pbm_init_info.pbm_session_init_done[i]) ||
                (FALSE == pbm_session_info[i].isActive))
            {
               init_complete = TRUE;
            }
            else
            {
              /* return as soon as you find one SESSION that is not initialised */
              init_complete = FALSE;
              break;
            }
         }
         else
         {
           /* return as soon as you find one SESSION that is not initialised */
           init_complete = FALSE;
           break;
         }
      }
   }

   return init_complete;
}

/*===========================================================================
FUNCTION PBM_USIM_READ_CB

DESCRIPTION
  This function is called during init by GSDI when a USIM record is read in.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_usim_read_cb (pbm_return_type pbm_returned,
                       pbm_sim_record_s_type *record)
{
   PB_USIM_CACHE *usim_cache;
   pbm_session_enum_type session_type;
   pbm_file_type file_id;
   int record_num = 0;
   pbm_pb_set_type pb_set;

   PBM_CHECK_PTR_RET(record, VOID);

   session_type = record->session_type;
   PBM_VALIDATE_SESSION_RET(session_type, VOID);
   file_id = record->file_id;
   pb_set = pbm_file_id_to_pb_set(file_id);

   UIM_MSG_HIGH_3("pbm_usim_read_cb sess %d file %d index %d",
                   session_type,file_id,record->index);

   usim_cache = pbm_file_id_to_cache(session_type, file_id);
   if (usim_cache != NULL)
   {
      if (pbm_returned == PBM_SUCCESS) /* If we can trust the data. */
      {
         if (pbm_usim_cache_record_add(usim_cache, record))
         {
            record_num = pbm_get_next_used_record(session_type,
                                                  record->index);
            if ((record_num <= usim_cache->num_of_records) &&
                ((pbm_returned = pbm_usim_read(record->session_type,
                                               record->file_id,
                                               record_num,
                                               pbm_usim_read_cb)) == PBM_SUCCESS))
               return;
         }

         if (record_num <= usim_cache->num_of_records)
         {
            UIM_MSG_ERR_3("Fail to read next rec in USIM,Err %d after %d of max %d",
                        pbm_returned, record->index, usim_cache->num_of_records);

            /* This cache will be unusable, make sure its clear. */
            rex_enter_crit_sect(&pbm_crit_sect);
            pbm_clear_usim_cache(session_type, usim_cache);
            rex_leave_crit_sect(&pbm_crit_sect);
         }
      } else
      {
         UIM_MSG_ERR_3("Fail to read from SIM card, Error %d after %d of max %d",
                     pbm_returned, usim_cache->records_in_use, usim_cache->num_of_records);

         /* This cache will be unusable, make sure its clear. */
         rex_enter_crit_sect(&pbm_crit_sect);
         pbm_clear_usim_cache(session_type, usim_cache);
         rex_leave_crit_sect(&pbm_crit_sect);
      }

     if ( pb_set == PBM_USIM_PB_ONE )
     {
      /* Update the field info structure dynamically */
      if (usim_cache->num_of_records)
      {
        int n, i;
        pbm_field_id_e_type field_id =
        pbm_file_id_to_field_id(usim_cache->file_id);
        pbm_g_other_field_info_s_type *field_info = &g_sim_field_info[session_type][0];

	/* Converting the field type EXT to Simple ANR field to 
         * maintain the backward compatability */
        if(field_id == PBM_FIELD_ADDITIONAL_NUMBER_EXT)
        {
          field_id = PBM_FIELD_ADDITIONAL_NUMBER;
        }

        n = ((field_id == PBM_FIELD_GRPNAME_ID)? usim_cache->record_length : 1);

        for ( i = 0; i < field_info->num_fields; i++)
        {
          if (field_info->info[i].field_id == field_id)
          {
            field_info->info[i].num_items += n;
            if(field_id != PBM_FIELD_GRPNAME_ID)
            {
            field_info->info[i].max_length =
                MIN(usim_cache->record_length,field_info->info[i].max_length);
            }
            //if it is Type-2 file , then we don't need to add 
            //2 more bytes for NULL since it already have 2bytes more length            
            if(field_info->info[i].field_type == PBM_FT_UCS2 
                    && usim_cache->mapping_type != PBM_MAPPING_TYPE_2)
            {
              //increment 2 to accomodate NULL char
              field_info->info[i].max_length += 2;
            }
          }
        }
      }
     }
   } else
   {
      UIM_MSG_ERR_1("Could not find cache for file %d", file_id);
   }

   /* Move to next file regardless of why we got here. */
   pbm_initialize_index[session_type]++;
   pbm_init_entries(session_type);
}

