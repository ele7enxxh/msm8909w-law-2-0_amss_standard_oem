/*===========================================================================

                         R F   1 X  N V   M a n a g e r

DESCRIPTION
  This header file contains definitions necessary inorder to interface with
  the RF 1X NV Manager.

Copyright (c) 2010 - 2011      by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/sv/common/inc/rfnv_sv.h#1 $
$DateTime: 2016/12/13 07:59:52 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/07/12   gvn     Added IM3 backoff for LTE B2/B25 and 1X B15 combination 
                   (Merge from Nikel CR399105)
05/16/12   php     Add IM3 backoff for LTE B2/1x BC1
05/24/12   ndb     Implementation of IM2 backoff for LTE B4 and 1xBC0
05/23/12   ndb     Implementation of IM3 backoff for GPS desense
03/08/12   aca     Updated fail safe power from -60 to 23db to avoid call drops
11/17/11   Saul    SV Limiting. Added LTE B25 support. New SVDO Chans Combo
11/14/11   Saul    SV Limiting. Added support for 1x-HDR Half-Duplex and 
                   DSI backoffs.
11/11/11   Saul    SV Limiting. LTE backoff support.
11/10/11   Saul    SV Limiting. Initial support.
04/20/11   kma     Updated DSI macro
04/12/11   kma     Added DSI based SAR NVs for SVDO/SVLTE
10/29/10   kma     Provided default setting for LTE BC13 IM3 back off
10/13/10   cri     Fixed compiler errors
10/11/10   kma     Cleaned up the code
10/04/10   kma     Implemented fail-safe NV values for SAR
07/13/10   kma     Fixed TX power resolution bug
06/16/10   kma     Initial Revision.

===========================================================================*/
#ifndef RFNV_SV_H
#define RFNV_SV_H
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "rfcom.h"
#include "rfcommon_nv.h"
//#include "rflte_nv.h"
#include <memory.h>

#define MAX_NUM_OF_ROWS 5
#define MAX_NUM_OF_DSI_STATE 8

/*Provide a default TX power limit 23dbm in event of SAR NVs are missing or mis-configurated*/
#define DEFAULT_FAIL_SAFE_TX_POWER_LIM 230

/*Default SAR back off table*/
#define RF_HDR_BC0_DEFAULT_SAR_LIMIT  {{150,230}, \
                                       {155,170}, \
                                       {230,170}, \
                                       {230,170}, \
                                       {230,170}}

#define RF_HDR_BC1_DEFAULT_SAR_LIMIT  {{150,230}, \
                                       {155,170}, \
                                       {230,170}, \
                                       {230,170}, \
                                       {230,170}}

#define RF_HDR_BC4_DEFAULT_SAR_LIMIT  {{150,230}, \
                                       {155,170}, \
                                       {230,170}, \
                                       {230,170}, \
                                       {230,170}}


#define RF_HDR_BC6_DEFAULT_SAR_LIMIT  {{150,230}, \
                                       {155,170}, \
                                       {230,170}, \
                                       {230,170}, \
                                       {230,170}}

#define RF_HDR_BC10_DEFAULT_SAR_LIMIT  {{150,230}, \
                                        {155,170}, \
                                        {230,170}, \
                                        {230,170}, \
                                        {230,170}}


#define RF_HDR_BC15_DEFAULT_SAR_LIMIT  {{150,230}, \
                                        {155,170}, \
                                        {230,170}, \
                                        {230,170}, \
                                        {230,170}}

#define RF_HDR_BC0_DEFAULT_HALF_DUPLEX_LIMIT  {{150,230}, \
                                               {155,170}, \
                                               {230,170}, \
                                               {230,170}, \
                                               {230,170}}

#define RF_HDR_BC1_DEFAULT_HALF_DUPLEX_LIMIT  {{150,230}, \
                                               {155,170}, \
                                               {230,170}, \
                                               {230,170}, \
                                               {230,170}}

#define RF_HDR_BC6_DEFAULT_HALF_DUPLEX_LIMIT  {{150,230}, \
                                               {155,170}, \
                                               {230,170}, \
                                               {230,170}, \
                                               {230,170}}

#define RF_LTE_BC13_DEFAULT_SAR_LIMIT  {{180,230}, \
                                        {185,190}, \
                                        {230,190}, \
                                        {230,190}, \
                                        {230,190}}

#define RF_LTE_BC17_DEFAULT_SAR_LIMIT  {{180,230}, \
                                        {185,190}, \
                                        {230,190}, \
                                        {230,190}, \
                                        {230,190}}

#define RF_LTE_BC4_DEFAULT_SAR_LIMIT  {{180,230}, \
                                       {185,190}, \
                                       {230,190}, \
                                       {230,190}, \
                                       {230,190}}

#define RF_LTE_BC2_DEFAULT_SAR_LIMIT  {{180,230}, \
                                       {185,190}, \
                                       {230,190}, \
                                       {230,190}, \
                                       {230,190}}

#define RF_LTE_BC5_DEFAULT_SAR_LIMIT  {{180,230}, \
                                       {185,190}, \
                                       {230,190}, \
                                       {230,190}, \
                                       {230,190}}

#define RF_LTE_BC12_DEFAULT_SAR_LIMIT  {{180,230}, \
                                       {185,190}, \
                                       {230,190}, \
                                       {230,190}, \
                                       {230,190}}

#define RF_LTE_BC13_DEFAULT_IM3_LIMIT  {{180,210}, \
                                        {190,190}, \
                                        {200,165}, \
                                        {210,140}, \
                                        {220,120}}

#define RF_LTE_B25_DEFAULT_IM3_LIMIT   {{180,210}, \
                                        {190,190}, \
                                        {200,165}, \
                                        {210,140}, \
                                        {220,120}}

#define RF_LTE_B2_DEFAULT_IM3_LIMIT   {{180,210}, \
                                        {190,190}, \
                                        {200,165}, \
                                        {210,140}, \
                                        {220,120}}
#define RF_AWS_PCS_GNSS_DEFAULT_IM3_LIMIT   {{180,210}, \
                                             {190,190}, \
                                             {200,165}, \
                                             {210,140}, \
                                             {220,120}}
#define RF_CDMA1X_BC15_LTE_B2_B25_LTE_IM3_LIMIT   {{180,210}, \
                                                   {190,190}, \
                                                   {200,165}, \
                                                   {210,140}, \
                                                   {220,120}}

#define RF_CDMA1X_BC15_LTE_B25_1X_IM3_LIMIT   {{180,210}, \
                                               {190,190}, \
                                               {200,165}, \
                                               {210,140}, \
                                               {220,120}}

#define RF_LTE_BC4_DEFAULT_IM2_LIMIT   {{180,210}, \
                                        {190,190}, \
                                        {200,165}, \
                                        {210,140}, \
                                        {220,120}}


#define RF_LTE_B25_DEFAULT_SAR_LIMIT  {{180,230}, \
                                       {185,190}, \
                                       {230,190}, \
                                       {230,190}, \
                                       {230,190}}

#define RF_LTE_B17_DEFAULT_SAR_LIMIT  {{180,230}, \
                                       {185,190}, \
                                       {230,190}, \
                                       {230,190}, \
                                       {230,190}}

/*Default SVDO DSI SAR bias table*/
#define RF_HDR_BC0_DEFAULT_DSI_SAR_BIAS  {{0,0}, \
                                          {0,0}, \
                                          {0,0}, \
                                          {0,0}, \
                                          {0,0}, \
                                          {0,0}, \
                                          {0,0}, \
                                          {0,0}}


#define RF_HDR_BC1_DEFAULT_DSI_SAR_BIAS  {{0,0}, \
                                          {0,0}, \
                                          {0,0}, \
                                          {0,0}, \
                                          {0,0}, \
                                          {0,0}, \
                                          {0,0}, \
                                          {0,0}}

#define RF_HDR_BC4_DEFAULT_DSI_SAR_BIAS  {{0,0}, \
                                          {0,0}, \
                                          {0,0}, \
                                          {0,0}, \
                                          {0,0}, \
                                          {0,0}, \
                                          {0,0}, \
                                          {0,0}}

#define RF_HDR_BC6_DEFAULT_DSI_SAR_BIAS  {{0,0}, \
                                          {0,0}, \
                                          {0,0}, \
                                          {0,0}, \
                                          {0,0}, \
                                          {0,0}, \
                                          {0,0}, \
                                          {0,0}}

#define RF_HDR_BC10_DEFAULT_DSI_SAR_BIAS  {{0,0}, \
                                           {0,0}, \
                                           {0,0}, \
                                           {0,0}, \
                                           {0,0}, \
                                           {0,0}, \
                                           {0,0}, \
                                           {0,0}}

#define RF_HDR_BC15_DEFAULT_DSI_SAR_BIAS  {{0,0}, \
                                           {0,0}, \
                                           {0,0}, \
                                           {0,0}, \
                                           {0,0}, \
                                           {0,0}, \
                                           {0,0}, \
                                           {0,0}}

/*Default SVLTE DSI SAR bias table*/
#define RF_LTE_BC13_DEFAULT_DSI_SAR_BIAS  {{0,0}, \
                                           {0,0}, \
                                           {0,0}, \
                                           {0,0}, \
                                           {0,0}, \
                                           {0,0}, \
                                           {0,0}, \
                                           {0,0}}

#define RF_LTE_BC2_DEFAULT_DSI_SAR_BIAS  {{0,0}, \
                                          {0,0}, \
                                          {0,0}, \
                                          {0,0}, \
                                          {0,0}, \
                                          {0,0}, \
                                          {0,0}, \
                                          {0,0}}

#define RF_LTE_BC4_DEFAULT_DSI_SAR_BIAS  {{0,0}, \
                                          {0,0}, \
                                          {0,0}, \
                                          {0,0}, \
                                          {0,0}, \
                                          {0,0}, \
                                          {0,0}, \
                                          {0,0}}

#define RF_LTE_BC5_DEFAULT_DSI_SAR_BIAS  {{0,0}, \
                                          {0,0}, \
                                          {0,0}, \
                                          {0,0}, \
                                          {0,0}, \
                                          {0,0}, \
                                          {0,0}, \
                                          {0,0}}

#define RF_LTE_BC25_DEFAULT_DSI_SAR_BIAS  {{0,0}, \
                                           {0,0}, \
                                           {0,0}, \
                                           {0,0}, \
                                           {0,0}, \
                                           {0,0}, \
                                           {0,0}, \
                                           {0,0}}

#define RF_LTE_BC12_DEFAULT_DSI_SAR_BIAS  {{0,0}, \
                                           {0,0}, \
                                           {0,0}, \
                                           {0,0}, \
                                           {0,0}, \
                                           {0,0}, \
                                           {0,0}, \
                                           {0,0}}

#define RF_LTE_BC17_DEFAULT_DSI_SAR_BIAS  {{0,0}, \
                                           {0,0}, \
                                           {0,0}, \
                                           {0,0}, \
                                           {0,0}, \
                                           {0,0}, \
                                           {0,0}, \
                                           {0,0}}

typedef struct   /* rfnv_sv_items_type */
{
/*************SAR POWER LIMIT*******************/

  /* SAR pwr limit for 1x BC0 and DO BC0 */
  int16 cdma1x_bc0_do_bc0_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC0 and DO BC1 */
  int16 cdma1x_bc0_do_bc1_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC0 and DO BC4 */
  int16 cdma1x_bc0_do_bc4_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC0 and DO BC6 */
  int16 cdma1x_bc0_do_bc6_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC0 and DO BC10 */
  int16 cdma1x_bc0_do_bc10_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC0 and DO BC15 */
  int16 cdma1x_bc0_do_bc15_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC1 and DO BC0 */
  int16 cdma1x_bc1_do_bc0_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC1 and DO BC1 */
  int16 cdma1x_bc1_do_bc1_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC1 and DO BC4 */
  int16 cdma1x_bc1_do_bc4_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC1 and DO BC6 */
  int16 cdma1x_bc1_do_bc6_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC1 and DO BC10 */
  int16 cdma1x_bc1_do_bc10_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC1 and DO BC15 */
  int16 cdma1x_bc1_do_bc15_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC4 and DO BC0 */
  int16 cdma1x_bc4_do_bc0_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC4 and DO BC1 */
  int16 cdma1x_bc4_do_bc1_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC4 and DO BC4 */
  int16 cdma1x_bc4_do_bc4_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC4 and DO BC6 */
  int16 cdma1x_bc4_do_bc6_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC4 and DO BC10 */
  int16 cdma1x_bc4_do_bc10_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC4 and DO BC15 */
  int16 cdma1x_bc4_do_bc15_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC6 and DO BC0 */
  int16 cdma1x_bc6_do_bc0_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC6 and DO BC1 */
  int16 cdma1x_bc6_do_bc1_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC6 and DO BC4 */
  int16 cdma1x_bc6_do_bc4_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC6 and DO BC6 */
  int16 cdma1x_bc6_do_bc6_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC6 and DO BC10 */
  int16 cdma1x_bc6_do_bc10_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC6 and DO BC15 */
  int16 cdma1x_bc6_do_bc15_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC10 and DO BC0 */
  int16 cdma1x_bc10_do_bc0_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC10 and DO BC1 */
  int16 cdma1x_bc10_do_bc1_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC10 and DO BC4 */
  int16 cdma1x_bc10_do_bc4_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC10 and DO BC6 */
  int16 cdma1x_bc10_do_bc6_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC10 and DO BC10 */
  int16 cdma1x_bc10_do_bc10_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC10 and DO BC15 */
  int16 cdma1x_bc10_do_bc15_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC15 and DO BC0 */
  int16 cdma1x_bc15_do_bc0_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC15 and DO BC1 */
  int16 cdma1x_bc15_do_bc1_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC15 and DO BC4 */
  int16 cdma1x_bc15_do_bc4_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC15 and DO BC6 */
  int16 cdma1x_bc15_do_bc6_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC15 and DO BC10 */
  int16 cdma1x_bc15_do_bc10_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC15 and DO BC15 */
  int16 cdma1x_bc15_do_bc15_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC0 and LTE BC4 */
  int16 cdma1x_bc0_lte_bc4_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC0 and LTE BC13 */
  int16 cdma1x_bc0_lte_bc13_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC1 and LTE BC4 */
  int16 cdma1x_bc1_lte_bc4_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC1 and LTE BC13 */
  int16 cdma1x_bc1_lte_bc13_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC6 and LTE BC4 */
  int16 cdma1x_bc6_lte_bc4_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC6 and LTE BC13 */
  int16 cdma1x_bc6_lte_bc13_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC0 and LTE BC2 */
  int16 cdma1x_bc0_lte_bc2_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC0 and LTE BC5 */
  int16 cdma1x_bc0_lte_bc5_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC0 and LTE BC12 */
  int16 cdma1x_bc0_lte_bc12_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC1 and LTE BC2 */
  int16 cdma1x_bc1_lte_bc2_sar[MAX_NUM_OF_ROWS][2];

   /* SAR pwr limit for 1x BC1 and LTE BC5 */
  int16 cdma1x_bc1_lte_bc5_sar[MAX_NUM_OF_ROWS][2];

   /* SAR pwr limit for 1x BC1 and LTE BC12 */
  int16 cdma1x_bc1_lte_bc12_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC15 and LTE BC2 */
  int16 cdma1x_bc15_lte_bc2_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC15 and LTE BC4 */
  int16 cdma1x_bc15_lte_bc4_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC15 and LTE BC5 */
  int16 cdma1x_bc15_lte_bc5_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC15 and LTE BC12 */
  int16 cdma1x_bc15_lte_bc12_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC4 and LTE BC5 */
  int16 cdma1x_bc4_lte_bc5_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC0 and LTE B25 */
  int16 cdma1x_bc0_lte_b25_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC1 and LTE B25 */
  int16 cdma1x_bc1_lte_b25_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC10 and LTE B25 */
  int16 cdma1x_bc10_lte_b25_sar[MAX_NUM_OF_ROWS][2];
    
  /* SAR pwr limit for 1x BC10 and LTE BC2 */
  int16 cdma1x_bc10_lte_bc2_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC0 and LTE BC17 */
  int16 cdma1x_bc0_lte_bc17_sar[MAX_NUM_OF_ROWS][2];
 
  /* SAR pwr limit for 1x BC1 and LTE BC17 */
  int16 cdma1x_bc1_lte_bc17_sar[MAX_NUM_OF_ROWS][2];

  /* SAR pwr limit for 1x BC6 and LTE BC17 */
  int16 cdma1x_bc6_lte_bc17_sar[MAX_NUM_OF_ROWS][2];

/*************IM3 de-sense POWER LIMIT**************************/

  /* IM3 de-sense pwr limit for 1x BC0 and LTE BC13 */
  int16 cdma1x_bc0_lte_bc13_im3[MAX_NUM_OF_ROWS][2];

  /* IM3 de-sense pwr limit for 1x BC1 and LTE BC25 */
  int16 cdma1x_bc1_lte_b25_im3[MAX_NUM_OF_ROWS][2];

  /* IM3 de-sense pwr limit for 1x BC1 and LTE BC25 */
  int16 cdma1x_bc1_lte_b2_im3[MAX_NUM_OF_ROWS][2];

  /* GNSS IM3 de-sense pwr limit for PCS & AWS bands  */
   int16 sv_aws_pcs_gnss_im3_desense[MAX_NUM_OF_ROWS][2];


  /*************IM2 de-sense POWER LIMIT**************************/

  /* IM2 de-sense pwr limit for 1x BC0 and LTE BC4 */
  int16 cdma1x_bc0_lte_b4_im2[MAX_NUM_OF_ROWS][2];

  /*LTE IM3 de-sense pwr limit for :LTE BC2 + 1x B15, LTE BC25 + 1xB15*/
  int16 cdma1x_bc15_lte_b2_b25_lte_im3_desense[MAX_NUM_OF_ROWS][2];

  /*CDMA 1x IM3 de-sense pwr limit for :LTE BC25 + 1x B15, LTE BC25 + 1x B8*/
  int16 cdma1x_bc15_lte_b25_1x_im3_desense[MAX_NUM_OF_ROWS][2]; 


/*************Half-duplex de-sense POWER LIMIT*******************/

  /* Half-duplex de-sense pwr limit for DO BC0*/
  int16 hdr_bc0_half_duplex[MAX_NUM_OF_ROWS][2];

  /* Half-duplex de-sense pwr limit for DO BC1*/
  int16 hdr_bc1_half_duplex[MAX_NUM_OF_ROWS][2];

  /* Half-duplex de-sense pwr limit for DO BC6*/
  int16 hdr_bc6_half_duplex[MAX_NUM_OF_ROWS][2];

/************DSI based SVDO SAR pwr bias************************/

  /* DSI SVDO SAR pwr limit bias for 1x BC0 and DO BC0*/
  int16 cdma1x_bc0_do_bc0_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVDO SAR pwr limit bias for 1x BC0 and DO BC1*/
  int16 cdma1x_bc0_do_bc1_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVDO SAR pwr limit bias for 1x BC0 and DO BC4*/
  int16 cdma1x_bc0_do_bc4_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVDO SAR pwr limit bias for 1x BC0 and DO BC6*/
  int16 cdma1x_bc0_do_bc6_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVDO SAR pwr limit bias for 1x BC0 and DO BC10*/
  int16 cdma1x_bc0_do_bc10_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVDO SAR pwr limit bias for 1x BC0 and DO BC15*/
  int16 cdma1x_bc0_do_bc15_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVDO SAR pwr limit bias for 1x BC1 and DO BC0*/
  int16 cdma1x_bc1_do_bc0_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVDO SAR pwr limit bias for 1x BC1 and DO BC1*/
  int16 cdma1x_bc1_do_bc1_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVDO SAR pwr limit bias for 1x BC1 and DO BC4*/
  int16 cdma1x_bc1_do_bc4_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVDO SAR pwr limit bias for 1x BC1 and DO BC6*/
  int16 cdma1x_bc1_do_bc6_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVDO SAR pwr limit bias for 1x BC1 and DO BC10*/
  int16 cdma1x_bc1_do_bc10_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVDO SAR pwr limit bias for 1x BC1 and DO BC15*/
  int16 cdma1x_bc1_do_bc15_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVDO SAR pwr limit bias for 1x BC4 and DO BC0*/
  int16 cdma1x_bc4_do_bc0_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVDO SAR pwr limit bias for 1x BC4 and DO BC1*/
  int16 cdma1x_bc4_do_bc1_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVDO SAR pwr limit bias for 1x BC4 and DO BC4*/
  int16 cdma1x_bc4_do_bc4_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVDO SAR pwr limit bias for 1x BC4 and DO BC6*/
  int16 cdma1x_bc4_do_bc6_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVDO SAR pwr limit bias for 1x BC4 and DO BC10*/
  int16 cdma1x_bc4_do_bc10_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVDO SAR pwr limit bias for 1x BC4 and DO BC15*/
  int16 cdma1x_bc4_do_bc15_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVDO SAR pwr limit bias for 1x BC6 and DO BC0*/
  int16 cdma1x_bc6_do_bc0_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVDO SAR pwr limit bias for 1x BC6 and DO BC1*/
  int16 cdma1x_bc6_do_bc1_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVDO SAR pwr limit bias for 1x BC6 and DO BC4*/
  int16 cdma1x_bc6_do_bc4_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVDO SAR pwr limit bias for 1x BC6 and DO BC6*/
  int16 cdma1x_bc6_do_bc6_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVDO SAR pwr limit bias for 1x BC6 and DO BC10*/
  int16 cdma1x_bc6_do_bc10_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVDO SAR pwr limit bias for 1x BC6 and DO BC15*/
  int16 cdma1x_bc6_do_bc15_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVDO SAR pwr limit bias for 1x BC10 and DO BC0*/
  int16 cdma1x_bc10_do_bc0_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVDO SAR pwr limit bias for 1x BC10 and DO BC1*/
  int16 cdma1x_bc10_do_bc1_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVDO SAR pwr limit bias for 1x BC10 and DO BC4*/
  int16 cdma1x_bc10_do_bc4_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVDO SAR pwr limit bias for 1x BC10 and DO BC6*/
  int16 cdma1x_bc10_do_bc6_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVDO SAR pwr limit bias for 1x BC10 and DO BC10*/
  int16 cdma1x_bc10_do_bc10_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVDO SAR pwr limit bias for 1x BC10 and DO BC15*/
  int16 cdma1x_bc10_do_bc15_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVDO SAR pwr limit bias for 1x BC15 and DO BC0*/
  int16 cdma1x_bc15_do_bc0_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVDO SAR pwr limit bias for 1x BC15 and DO BC1*/
  int16 cdma1x_bc15_do_bc1_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVDO SAR pwr limit bias for 1x BC15 and DO BC4*/
  int16 cdma1x_bc15_do_bc4_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVDO SAR pwr limit bias for 1x BC15 and DO BC6*/
  int16 cdma1x_bc15_do_bc6_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVDO SAR pwr limit bias for 1x BC15 and DO BC10*/
  int16 cdma1x_bc15_do_bc10_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVDO SAR pwr limit bias for 1x BC15 and DO BC15*/
  int16 cdma1x_bc15_do_bc15_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

/************DSI based SVLTE SAR pwr bias************************/

  /* DSI SVLTE SAR pwr limit bias for 1x BC0 and LTE BC4 */
  int16 cdma1x_bc0_lte_bc4_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVLTE SAR pwr limit bias for 1x BC1 and LTE BC4 */
  int16 cdma1x_bc1_lte_bc4_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVLTE SAR pwr limit bias for 1x BC6 and LTE BC4 */
  int16 cdma1x_bc6_lte_bc4_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVLTE SAR pwr limit bias for 1x BC0 and LTE BC13 */
  int16 cdma1x_bc0_lte_bc13_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVLTE SAR pwr limit bias for 1x BC1 and LTE BC13 */
  int16 cdma1x_bc1_lte_bc13_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

   /* DSI SVLTE SAR pwr limit bias for 1x BC6 and LTE BC13 */
  int16 cdma1x_bc6_lte_bc13_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVLTE SAR pwr limit bias for 1x BC0 and LTE BC2 */
  int16 cdma1x_bc0_lte_bc2_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVLTE SAR pwr limit bias for 1x BC1 and LTE BC2 */
  int16 cdma1x_bc1_lte_bc2_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVLTE SAR pwr limit bias for 1x BC15 and LTE BC2 */
  int16 cdma1x_bc15_lte_bc2_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVLTE SAR pwr limit bias for 1x BC15 and LTE BC4 */
  int16 cdma1x_bc15_lte_bc4_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVLTE SAR pwr limit bias for 1x BC4 and LTE BC5 */
  int16 cdma1x_bc4_lte_bc5_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVLTE SAR pwr limit bias for 1x BC0 and LTE BC25 */
  int16 cdma1x_bc0_lte_b25_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVLTE SAR pwr limit bias for 1x BC1 and LTE BC25 */
  int16 cdma1x_bc1_lte_b25_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVLTE SAR pwr limit bias for 1x BC10 and LTE BC25 */
  int16 cdma1x_bc10_lte_b25_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVLTE SAR pwr limit bias for 1x BC10 and LTE BC2 */
  int16 cdma1x_bc10_lte_bc2_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVLTE SAR pwr limit bias for 1x BC0 and LTE BC12 */
  int16 cdma1x_bc0_lte_bc12_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

   /* DSI SVLTE SAR pwr limit bias for 1x BC1 and LTE BC12 */
  int16 cdma1x_bc1_lte_bc12_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

   /* DSI SVLTE SAR pwr limit bias for 1x BC15 and LTE BC12 */
  int16 cdma1x_bc15_lte_bc12_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVLTE SAR pwr limit bias for 1x BC0 and LTE BC17 */
  int16 cdma1x_bc0_lte_bc17_dsi_sar[MAX_NUM_OF_DSI_STATE][2];
 
  /* DSI SVLTE SAR pwr limit bias for 1x BC1 and LTE BC17 */
  int16 cdma1x_bc1_lte_bc17_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

  /* DSI SVLTE SAR pwr limit bias for 1x BC6 and LTE BC17 */
  int16 cdma1x_bc6_lte_bc17_dsi_sar[MAX_NUM_OF_DSI_STATE][2];

}rfnv_sv_items_type;

extern rfnv_sv_items_type rfnv_cdma1x_sv_tbl;

/*----------------------------------------------------------------------------*/
boolean 
rfsv_retrieve_nv_items
(
  rex_tcb_type *task_ptr,
  rex_sigs_type task_wait_sig,
  void (*task_wait_handler)( rex_sigs_type )
);

#endif /* RFNV_SV_H */
