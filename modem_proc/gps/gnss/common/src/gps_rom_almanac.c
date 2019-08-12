/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             GPS Rom Almanac Module

GENERAL DESCRIPTION
  This module contains Default Rom Almanac helper function that is used 
  within the GPS sub-system.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/common/src/gps_rom_almanac.c#1 $ 
  $DateTime: 2016/12/13 07:59:45 $ 
  $Author: mplcsds1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*
 * Include files
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "customer.h"
#include "comdef.h"
#include "gnss_common.h"
#include "err.h"
#include "msg.h"

/*
 * Constant definitions
*/

/*
 * Local function prototypes
*/


/*
 ******************************************************************************
 * gps_GetGpsRomAlmanac
 *
 * Function description:
 *
 * 	gps_GetGpsRomAlmanac is a helper function used to retreive the default GPS
 *	ROM almanac.
 *
 * Parameters: 
 *
 *  pz_GpsRomAlm - Pointer to the GPS ROM almanac structure
 *
 * Return value: 
 *
 *  void
 *
 ******************************************************************************
*/

void gps_GetGpsRomAlmanac( gps_AlmStructType **pz_RomAlm )
{
  /* No almanac for PRN 1 */
  pz_RomAlm[0]->u_Sv = C_GPS_SV_ALM_INVALID ;
  
  pz_RomAlm[1]->u_Sv = 2 ;
  pz_RomAlm[1]->u_Health = 0 ;
  pz_RomAlm[1]->u_Toa = 99 ;
  pz_RomAlm[1]->w_E = 21047 ;
  pz_RomAlm[1]->w_DeltaI = 65016 ;
  pz_RomAlm[1]->w_OmegaDot = 64835 ;
  pz_RomAlm[1]->q_SqrtA = 10554689ul ;
  pz_RomAlm[1]->q_Omega0 = 1987168ul ;
  pz_RomAlm[1]->q_Omega = 4286760665ul ;
  pz_RomAlm[1]->q_M0 = 4294960815ul ;
  pz_RomAlm[1]->w_Af0 = 338 ;
  pz_RomAlm[1]->w_Af1 = 1 ;
  pz_RomAlm[1]->w_GpsWeek = 1624 ;
  
  pz_RomAlm[2]->u_Sv = 3 ;
  pz_RomAlm[2]->u_Health = 0 ;
  pz_RomAlm[2]->u_Toa = 99 ;
  pz_RomAlm[2]->w_E = 29316 ;
  pz_RomAlm[2]->w_DeltaI = 63224 ;
  pz_RomAlm[2]->w_OmegaDot = 64815 ;
  pz_RomAlm[2]->q_SqrtA = 10554532ul ;
  pz_RomAlm[2]->q_Omega0 = 4293867498ul ;
  pz_RomAlm[2]->q_Omega = 2869791ul ;
  pz_RomAlm[2]->q_M0 = 4294899594ul ;
  pz_RomAlm[2]->w_Af0 = 713 ;
  pz_RomAlm[2]->w_Af1 = 1 ;
  pz_RomAlm[2]->w_GpsWeek = 1624 ;
  
  pz_RomAlm[3]->u_Sv = 4 ;
  pz_RomAlm[3]->u_Health = 0 ;
  pz_RomAlm[3]->u_Toa = 99 ;
  pz_RomAlm[3]->w_E = 20154 ;
  pz_RomAlm[3]->w_DeltaI = 64863 ;
  pz_RomAlm[3]->w_OmegaDot = 64835 ;
  pz_RomAlm[3]->q_SqrtA = 10554456ul ;
  pz_RomAlm[3]->q_Omega0 = 2033902ul ;
  pz_RomAlm[3]->q_Omega = 1897641ul ;
  pz_RomAlm[3]->q_M0 = 8055996ul ;
  pz_RomAlm[3]->w_Af0 = 47 ;
  pz_RomAlm[3]->w_Af1 = 3 ;
  pz_RomAlm[3]->w_GpsWeek = 1624 ;
  
  pz_RomAlm[4]->u_Sv = 5 ;
  pz_RomAlm[4]->u_Health = 0 ;
  pz_RomAlm[4]->u_Toa = 99 ;
  pz_RomAlm[4]->w_E = 4765 ;
  pz_RomAlm[4]->w_DeltaI = 2057 ;
  pz_RomAlm[4]->w_OmegaDot = 64843 ;
  pz_RomAlm[4]->q_SqrtA = 10554513ul ;
  pz_RomAlm[4]->q_Omega0 = 4821821ul ;
  pz_RomAlm[4]->q_Omega = 743543ul ;
  pz_RomAlm[4]->q_M0 = 4580222ul ;
  pz_RomAlm[4]->w_Af0 = 65410 ;
  pz_RomAlm[4]->w_Af1 = 65534 ;
  pz_RomAlm[4]->w_GpsWeek = 1624 ;
  
  pz_RomAlm[5]->u_Sv = 6 ;
  pz_RomAlm[5]->u_Health = 0 ;
  pz_RomAlm[5]->u_Toa = 99 ;
  pz_RomAlm[5]->w_E = 13603 ;
  pz_RomAlm[5]->w_DeltaI = 64429 ;
  pz_RomAlm[5]->w_OmegaDot = 64823 ;
  pz_RomAlm[5]->q_SqrtA = 10554552ul ;
  pz_RomAlm[5]->q_Omega0 = 4294062070ul ;
  pz_RomAlm[5]->q_Omega = 4292678815ul ;
  pz_RomAlm[5]->q_M0 = 5515855ul ;
  pz_RomAlm[5]->w_Af0 = 65440 ;
  pz_RomAlm[5]->w_Af1 = 65532 ;
  pz_RomAlm[5]->w_GpsWeek = 1624 ;
  
  pz_RomAlm[6]->u_Sv = 7 ;
  pz_RomAlm[6]->u_Health = 0 ;
  pz_RomAlm[6]->u_Toa = 99 ;
  pz_RomAlm[6]->w_E = 8776 ;
  pz_RomAlm[6]->w_DeltaI = 5262 ;
  pz_RomAlm[6]->w_OmegaDot = 64846 ;
  pz_RomAlm[6]->q_SqrtA = 10554631ul ;
  pz_RomAlm[6]->q_Omega0 = 4288629151ul ;
  pz_RomAlm[6]->q_Omega = 4286679705ul ;
  pz_RomAlm[6]->q_M0 = 4290425235ul ;
  pz_RomAlm[6]->w_Af0 = 5 ;
  pz_RomAlm[6]->w_Af1 = 0 ;
  pz_RomAlm[6]->w_GpsWeek = 1624 ;
  
  pz_RomAlm[7]->u_Sv = 8 ;
  pz_RomAlm[7]->u_Health = 0 ;
  pz_RomAlm[7]->u_Toa = 99 ;
  pz_RomAlm[7]->w_E = 23989 ;
  pz_RomAlm[7]->w_DeltaI = 8863 ;
  pz_RomAlm[7]->w_OmegaDot = 64866 ;
  pz_RomAlm[7]->q_SqrtA = 10554523ul ;
  pz_RomAlm[7]->q_Omega0 = 4288825619ul ;
  pz_RomAlm[7]->q_Omega = 4286727189ul ;
  pz_RomAlm[7]->q_M0 = 4289011643ul ;
  pz_RomAlm[7]->w_Af0 = 8 ;
  pz_RomAlm[7]->w_Af1 = 0 ;
  pz_RomAlm[7]->w_GpsWeek = 1624 ;
  
  pz_RomAlm[8]->u_Sv = 9 ;
  pz_RomAlm[8]->u_Health = 0 ;
  pz_RomAlm[8]->u_Toa = 99 ;
  pz_RomAlm[8]->w_E = 35749 ;
  pz_RomAlm[8]->w_DeltaI = 6761 ;
  pz_RomAlm[8]->w_OmegaDot = 64854 ;
  pz_RomAlm[8]->q_SqrtA = 10554647ul ;
  pz_RomAlm[8]->q_Omega0 = 4288551383ul ;
  pz_RomAlm[8]->q_Omega = 4158522ul ;
  pz_RomAlm[8]->q_M0 = 4288654638ul ;
  pz_RomAlm[8]->w_Af0 = 67 ;
  pz_RomAlm[8]->w_Af1 = 1 ;
  pz_RomAlm[8]->w_GpsWeek = 1624 ;
  
  pz_RomAlm[9]->u_Sv = 10 ;
  pz_RomAlm[9]->u_Health = 0 ;
  pz_RomAlm[9]->u_Toa = 99 ;
  pz_RomAlm[9]->w_E = 21161 ;
  pz_RomAlm[9]->w_DeltaI = 1216 ;
  pz_RomAlm[9]->w_OmegaDot = 64837 ;
  pz_RomAlm[9]->q_SqrtA = 10554422ul ;
  pz_RomAlm[9]->q_Omega0 = 4886496ul ;
  pz_RomAlm[9]->q_Omega = 1861628ul ;
  pz_RomAlm[9]->q_M0 = 5289349ul ;
  pz_RomAlm[9]->w_Af0 = 65466 ;
  pz_RomAlm[9]->w_Af1 = 0 ;
  pz_RomAlm[9]->w_GpsWeek = 1624 ;
  
  pz_RomAlm[10]->u_Sv = 11 ;
  pz_RomAlm[10]->u_Health = 0 ;
  pz_RomAlm[10]->u_Toa = 99 ;
  pz_RomAlm[10]->w_E = 24413 ;
  pz_RomAlm[10]->w_DeltaI = 56500 ;
  pz_RomAlm[10]->w_OmegaDot = 64789 ;
  pz_RomAlm[10]->q_SqrtA = 10554214ul ;
  pz_RomAlm[10]->q_Omega0 = 1366316ul ;
  pz_RomAlm[10]->q_Omega = 2435646ul ;
  pz_RomAlm[10]->q_M0 = 4290676924ul ;
  pz_RomAlm[10]->w_Af0 = 65400 ;
  pz_RomAlm[10]->w_Af1 = 65535 ;
  pz_RomAlm[10]->w_GpsWeek = 1624 ;
  
  pz_RomAlm[11]->u_Sv = 12 ;
  pz_RomAlm[11]->u_Health = 0 ;
  pz_RomAlm[11]->u_Toa = 99 ;
  pz_RomAlm[11]->w_E = 7609 ;
  pz_RomAlm[11]->w_DeltaI = 5230 ;
  pz_RomAlm[11]->w_OmegaDot = 64858 ;
  pz_RomAlm[11]->q_SqrtA = 10554884ul ;
  pz_RomAlm[11]->q_Omega0 = 4291445893ul ;
  pz_RomAlm[11]->q_Omega = 4294212867ul ;
  pz_RomAlm[11]->q_M0 = 4290673118ul ;
  pz_RomAlm[11]->w_Af0 = 65511 ;
  pz_RomAlm[11]->w_Af1 = 1 ;
  pz_RomAlm[11]->w_GpsWeek = 1624 ;
  
  pz_RomAlm[12]->u_Sv = 13 ;
  pz_RomAlm[12]->u_Health = 0 ;
  pz_RomAlm[12]->u_Toa = 99 ;
  pz_RomAlm[12]->w_E = 9519 ;
  pz_RomAlm[12]->w_DeltaI = 8060 ;
  pz_RomAlm[12]->w_OmegaDot = 64853 ;
  pz_RomAlm[12]->q_SqrtA = 10554564ul ;
  pz_RomAlm[12]->q_Omega0 = 7833233ul ;
  pz_RomAlm[12]->q_Omega = 4773321ul ;
  pz_RomAlm[12]->q_M0 = 2197402ul ;
  pz_RomAlm[12]->w_Af0 = 295 ;
  pz_RomAlm[12]->w_Af1 = 0 ;
  pz_RomAlm[12]->w_GpsWeek = 1624 ;
  
  pz_RomAlm[13]->u_Sv = 14 ;
  pz_RomAlm[13]->u_Health = 0 ;
  pz_RomAlm[13]->u_Toa = 99 ;
  pz_RomAlm[13]->w_E = 11755 ;
  pz_RomAlm[13]->w_DeltaI = 6904 ;
  pz_RomAlm[13]->w_OmegaDot = 64847 ;
  pz_RomAlm[13]->q_SqrtA = 10554554ul ;
  pz_RomAlm[13]->q_Omega0 = 7770265ul ;
  pz_RomAlm[13]->q_Omega = 4289406011ul ;
  pz_RomAlm[13]->q_M0 = 2096814ul ;
  pz_RomAlm[13]->w_Af0 = 140 ;
  pz_RomAlm[13]->w_Af1 = 1 ;
  pz_RomAlm[13]->w_GpsWeek = 1624 ;
  
  pz_RomAlm[14]->u_Sv = 15 ;
  pz_RomAlm[14]->u_Health = 0 ;
  pz_RomAlm[14]->u_Toa = 99 ;
  pz_RomAlm[14]->w_E = 7060 ;
  pz_RomAlm[14]->w_DeltaI = 1435 ;
  pz_RomAlm[14]->w_OmegaDot = 64818 ;
  pz_RomAlm[14]->q_SqrtA = 10554414ul ;
  pz_RomAlm[14]->q_Omega0 = 7575922ul ;
  pz_RomAlm[14]->q_Omega = 4294693684ul ;
  pz_RomAlm[14]->q_M0 = 1108609ul ;
  pz_RomAlm[14]->w_Af0 = 65380 ;
  pz_RomAlm[14]->w_Af1 = 1 ;
  pz_RomAlm[14]->w_GpsWeek = 1624 ;
  
  pz_RomAlm[15]->u_Sv = 16 ;
  pz_RomAlm[15]->u_Health = 0 ;
  pz_RomAlm[15]->u_Toa = 99 ;
  pz_RomAlm[15]->w_E = 12995 ;
  pz_RomAlm[15]->w_DeltaI = 5408 ;
  pz_RomAlm[15]->w_OmegaDot = 64861 ;
  pz_RomAlm[15]->q_SqrtA = 10554797ul ;
  pz_RomAlm[15]->q_Omega0 = 4291493286ul ;
  pz_RomAlm[15]->q_Omega = 4294521957ul ;
  pz_RomAlm[15]->q_M0 = 6104196ul ;
  pz_RomAlm[15]->w_Af0 = 65375 ;
  pz_RomAlm[15]->w_Af1 = 65535 ;
  pz_RomAlm[15]->w_GpsWeek = 1624 ;
  
  pz_RomAlm[16]->u_Sv = 17 ;
  pz_RomAlm[16]->u_Health = 0 ;
  pz_RomAlm[16]->u_Toa = 99 ;
  pz_RomAlm[16]->w_E = 12836 ;
  pz_RomAlm[16]->w_DeltaI = 3138 ;
  pz_RomAlm[16]->w_OmegaDot = 64849 ;
  pz_RomAlm[16]->q_SqrtA = 10554703ul ;
  pz_RomAlm[16]->q_Omega0 = 4294278112ul ;
  pz_RomAlm[16]->q_Omega = 4288440984ul ;
  pz_RomAlm[16]->q_M0 = 3223954ul ;
  pz_RomAlm[16]->w_Af0 = 192 ;
  pz_RomAlm[16]->w_Af1 = 0 ;
  pz_RomAlm[16]->w_GpsWeek = 1624 ;
  
  pz_RomAlm[17]->u_Sv = 18 ;
  pz_RomAlm[17]->u_Health = 0 ;
  pz_RomAlm[17]->u_Toa = 99 ;
  pz_RomAlm[17]->w_E = 24997 ;
  pz_RomAlm[17]->w_DeltaI = 64070 ;
  pz_RomAlm[17]->w_OmegaDot = 64824 ;
  pz_RomAlm[17]->q_SqrtA = 10554798ul ;
  pz_RomAlm[17]->q_Omega0 = 4875352ul ;
  pz_RomAlm[17]->q_Omega = 4288899168ul ;
  pz_RomAlm[17]->q_M0 = 6810472ul ;
  pz_RomAlm[17]->w_Af0 = 129 ;
  pz_RomAlm[17]->w_Af1 = 1 ;
  pz_RomAlm[17]->w_GpsWeek = 1624 ;
  
  pz_RomAlm[18]->u_Sv = 19 ;
  pz_RomAlm[18]->u_Health = 0 ;
  pz_RomAlm[18]->u_Toa = 99 ;
  pz_RomAlm[18]->w_E = 14725 ;
  pz_RomAlm[18]->w_DeltaI = 2731 ;
  pz_RomAlm[18]->w_OmegaDot = 64844 ;
  pz_RomAlm[18]->q_SqrtA = 10554506ul ;
  pz_RomAlm[18]->q_Omega0 = 4294423037ul ;
  pz_RomAlm[18]->q_Omega = 4294935871ul ;
  pz_RomAlm[18]->q_M0 = 1282649ul ;
  pz_RomAlm[18]->w_Af0 = 65416 ;
  pz_RomAlm[18]->w_Af1 = 65535 ;
  pz_RomAlm[18]->w_GpsWeek = 1624 ;
  
  pz_RomAlm[19]->u_Sv = 20 ;
  pz_RomAlm[19]->u_Health = 0 ;
  pz_RomAlm[19]->u_Toa = 99 ;
  pz_RomAlm[19]->w_E = 9868 ;
  pz_RomAlm[19]->w_DeltaI = 64085 ;
  pz_RomAlm[19]->w_OmegaDot = 64825 ;
  pz_RomAlm[19]->q_SqrtA = 10554705ul ;
  pz_RomAlm[19]->q_Omega0 = 4731263ul ;
  pz_RomAlm[19]->q_Omega = 3392301ul ;
  pz_RomAlm[19]->q_M0 = 7821255ul ;
  pz_RomAlm[19]->w_Af0 = 51 ;
  pz_RomAlm[19]->w_Af1 = 0 ;
  pz_RomAlm[19]->w_GpsWeek = 1624 ;
  
  pz_RomAlm[20]->u_Sv = 21 ;
  pz_RomAlm[20]->u_Health = 0 ;
  pz_RomAlm[20]->u_Toa = 99 ;
  pz_RomAlm[20]->w_E = 36302 ;
  pz_RomAlm[20]->w_DeltaI = 63795 ;
  pz_RomAlm[20]->w_OmegaDot = 64832 ;
  pz_RomAlm[20]->q_SqrtA = 10554832ul ;
  pz_RomAlm[20]->q_Omega0 = 2057813ul ;
  pz_RomAlm[20]->q_Omega = 4288554464ul ;
  pz_RomAlm[20]->q_M0 = 4288452460ul ;
  pz_RomAlm[20]->w_Af0 = 65404 ;
  pz_RomAlm[20]->w_Af1 = 65535 ;
  pz_RomAlm[20]->w_GpsWeek = 1624 ;
  
  pz_RomAlm[21]->u_Sv = 22 ;
  pz_RomAlm[21]->u_Health = 0 ;
  pz_RomAlm[21]->u_Toa = 99 ;
  pz_RomAlm[21]->w_E = 11277 ;
  pz_RomAlm[21]->w_DeltaI = 63685 ;
  pz_RomAlm[21]->w_OmegaDot = 64821 ;
  pz_RomAlm[21]->q_SqrtA = 10554688ul ;
  pz_RomAlm[21]->q_Omega0 = 4886906ul ;
  pz_RomAlm[21]->q_Omega = 4289750912ul ;
  pz_RomAlm[21]->q_M0 = 4609501ul ;
  pz_RomAlm[21]->w_Af0 = 160 ;
  pz_RomAlm[21]->w_Af1 = 0 ;
  pz_RomAlm[21]->w_GpsWeek = 1624 ;
  
  pz_RomAlm[22]->u_Sv = 23 ;
  pz_RomAlm[22]->u_Health = 0 ;
  pz_RomAlm[22]->u_Toa = 99 ;
  pz_RomAlm[22]->w_E = 14647 ;
  pz_RomAlm[22]->w_DeltaI = 3847 ;
  pz_RomAlm[22]->w_OmegaDot = 64831 ;
  pz_RomAlm[22]->q_SqrtA = 10554445ul ;
  pz_RomAlm[22]->q_Omega0 = 7653309ul ;
  pz_RomAlm[22]->q_Omega = 8355654ul ;
  pz_RomAlm[22]->q_M0 = 101879ul ;
  pz_RomAlm[22]->w_Af0 = 340 ;
  pz_RomAlm[22]->w_Af1 = 65535 ;
  pz_RomAlm[22]->w_GpsWeek = 1624 ;
  
  pz_RomAlm[23]->u_Sv = 24 ;
  pz_RomAlm[23]->u_Health = 0 ;
  pz_RomAlm[23]->u_Toa = 99 ;
  pz_RomAlm[23]->w_E = 12339 ;
  pz_RomAlm[23]->w_DeltaI = 1023 ;
  pz_RomAlm[23]->w_OmegaDot = 64844 ;
  pz_RomAlm[23]->q_SqrtA = 10554715ul ;
  pz_RomAlm[23]->q_Omega0 = 2173306ul ;
  pz_RomAlm[23]->q_Omega = 4294144856ul ;
  pz_RomAlm[23]->q_M0 = 4293242941ul ;
  pz_RomAlm[23]->w_Af0 = 386 ;
  pz_RomAlm[23]->w_Af1 = 1 ;
  pz_RomAlm[23]->w_GpsWeek = 1624 ;
  
  pz_RomAlm[24]->u_Sv = 25 ;
  pz_RomAlm[24]->u_Health = 0 ;
  pz_RomAlm[24]->u_Toa = 99 ;
  pz_RomAlm[24]->w_E = 911 ;
  pz_RomAlm[24]->w_DeltaI = 3354 ;
  pz_RomAlm[24]->w_OmegaDot = 64847 ;
  pz_RomAlm[24]->q_SqrtA = 10554666ul ;
  pz_RomAlm[24]->q_Omega0 = 4291369092ul ;
  pz_RomAlm[24]->q_Omega = 2804515ul ;
  pz_RomAlm[24]->q_M0 = 7252894ul ;
  pz_RomAlm[24]->w_Af0 = 65463 ;
  pz_RomAlm[24]->w_Af1 = 0 ;
  pz_RomAlm[24]->w_GpsWeek = 1624 ;
  
  pz_RomAlm[25]->u_Sv = 26 ;
  pz_RomAlm[25]->u_Health = 0 ;
  pz_RomAlm[25]->u_Toa = 99 ;
  pz_RomAlm[25]->w_E = 42563 ;
  pz_RomAlm[25]->w_DeltaI = 7667 ;
  pz_RomAlm[25]->w_OmegaDot = 64851 ;
  pz_RomAlm[25]->q_SqrtA = 10554573ul ;
  pz_RomAlm[25]->q_Omega0 = 7821425ul ;
  pz_RomAlm[25]->q_Omega = 3001507ul ;
  pz_RomAlm[25]->q_M0 = 4294247723ul ;
  pz_RomAlm[25]->w_Af0 = 65400 ;
  pz_RomAlm[25]->w_Af1 = 65533 ;
  pz_RomAlm[25]->w_GpsWeek = 1624 ;
  
  pz_RomAlm[26]->u_Sv = 27 ;
  pz_RomAlm[26]->u_Health = 0 ;
  pz_RomAlm[26]->u_Toa = 99 ;
  pz_RomAlm[26]->w_E = 45696 ;
  pz_RomAlm[26]->w_DeltaI = 6362 ;
  pz_RomAlm[26]->w_OmegaDot = 64855 ;
  pz_RomAlm[26]->q_SqrtA = 10554711ul ;
  pz_RomAlm[26]->q_Omega0 = 4288482782ul ;
  pz_RomAlm[26]->q_Omega = 4291525727ul ;
  pz_RomAlm[26]->q_M0 = 2019082ul ;
  pz_RomAlm[26]->w_Af0 = 252 ;
  pz_RomAlm[26]->w_Af1 = 1 ;
  pz_RomAlm[26]->w_GpsWeek = 1624 ;
  
  pz_RomAlm[27]->u_Sv = 28 ;
  pz_RomAlm[27]->u_Health = 0 ;
  pz_RomAlm[27]->u_Toa = 99 ;
  pz_RomAlm[27]->w_E = 35545 ;
  pz_RomAlm[27]->w_DeltaI = 5121 ;
  pz_RomAlm[27]->w_OmegaDot = 64860 ;
  pz_RomAlm[27]->q_SqrtA = 10554667ul ;
  pz_RomAlm[27]->q_Omega0 = 4291512941ul ;
  pz_RomAlm[27]->q_Omega = 4289883189ul ;
  pz_RomAlm[27]->q_M0 = 4892569ul ;
  pz_RomAlm[27]->w_Af0 = 23 ;
  pz_RomAlm[27]->w_Af1 = 1 ;
  pz_RomAlm[27]->w_GpsWeek = 1624 ;
  
  pz_RomAlm[28]->u_Sv = 29 ;
  pz_RomAlm[28]->u_Health = 0 ;
  pz_RomAlm[28]->u_Toa = 99 ;
  pz_RomAlm[28]->w_E = 5522 ;
  pz_RomAlm[28]->w_DeltaI = 3208 ;
  pz_RomAlm[28]->w_OmegaDot = 64848 ;
  pz_RomAlm[28]->q_SqrtA = 10554394ul ;
  pz_RomAlm[28]->q_Omega0 = 4294300406ul ;
  pz_RomAlm[28]->q_Omega = 4291598575ul ;
  pz_RomAlm[28]->q_M0 = 4288912797ul ;
  pz_RomAlm[28]->w_Af0 = 197 ;
  pz_RomAlm[28]->w_Af1 = 1 ;
  pz_RomAlm[28]->w_GpsWeek = 1624 ;
  
  pz_RomAlm[29]->u_Sv = 30 ;
  pz_RomAlm[29]->u_Health = 0 ;
  pz_RomAlm[29]->u_Toa = 99 ;
  pz_RomAlm[29]->w_E = 26341 ;
  pz_RomAlm[29]->w_DeltaI = 2790 ;
  pz_RomAlm[29]->w_OmegaDot = 64846 ;
  pz_RomAlm[29]->q_SqrtA = 10554566ul ;
  pz_RomAlm[29]->q_Omega0 = 4291291878ul ;
  pz_RomAlm[29]->q_Omega = 4052542ul ;
  pz_RomAlm[29]->q_M0 = 2513611ul ;
  pz_RomAlm[29]->w_Af0 = 340 ;
  pz_RomAlm[29]->w_Af1 = 1 ;
  pz_RomAlm[29]->w_GpsWeek = 1624 ;
  
  pz_RomAlm[30]->u_Sv = 31 ;
  pz_RomAlm[30]->u_Health = 0 ;
  pz_RomAlm[30]->u_Toa = 99 ;
  pz_RomAlm[30]->w_E = 16503 ;
  pz_RomAlm[30]->w_DeltaI = 6127 ;
  pz_RomAlm[30]->w_OmegaDot = 64851 ;
  pz_RomAlm[30]->q_SqrtA = 10554496ul ;
  pz_RomAlm[30]->q_Omega0 = 4288631395ul ;
  pz_RomAlm[30]->q_Omega = 4292322088ul ;
  pz_RomAlm[30]->q_M0 = 4290514956ul ;
  pz_RomAlm[30]->w_Af0 = 48 ;
  pz_RomAlm[30]->w_Af1 = 1 ;
  pz_RomAlm[30]->w_GpsWeek = 1624 ;
  
  pz_RomAlm[31]->u_Sv = 32 ;
  pz_RomAlm[31]->u_Health = 0 ;
  pz_RomAlm[31]->u_Toa = 99 ;
  pz_RomAlm[31]->w_E = 25722 ;
  pz_RomAlm[31]->w_DeltaI = 2188 ;
  pz_RomAlm[31]->w_OmegaDot = 64844 ;
  pz_RomAlm[31]->q_SqrtA = 10554747ul ;
  pz_RomAlm[31]->q_Omega0 = 5059634ul ;
  pz_RomAlm[31]->q_Omega = 4292780070ul ;
  pz_RomAlm[31]->q_M0 = 4292488614ul ;
  pz_RomAlm[31]->w_Af0 = 65323 ;
  pz_RomAlm[31]->w_Af1 = 65534 ;
  pz_RomAlm[31]->w_GpsWeek = 1624 ;
  
}
