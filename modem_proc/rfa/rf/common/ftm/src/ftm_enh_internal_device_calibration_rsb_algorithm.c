/*!
  @file
  ftm_common_enh_internal_device_calibration_rsb_alg.c

  @brief

*/
/*==============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftm_enh_internal_device_calibration_rsb_algorithm.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 


02/01/14  xsh     Initial Revision

==============================================================================*/
#ifndef FEATURE_GNSS_ONLY_NO_WWAN

#include "rfa_variation.h"
#include "comdef.h"
#include "modem_mem.h"
#include "string.h"
#include "stringl.h" /* for memscpy */
#include "fs_estdio.h"
#include "ftm_msg.h"
#include "rfm_mode_types.h"
#include "ftm_common_msm.h"
#include "ftm_common_control.h"
#include "ftm_common_data.h"
#include "rfcommon_math.h"
#include "math.h"
#include "ftm_enh_internal_device_cal_rsb_alg.h"


#ifdef FEATURE_FACTORY_TESTMODE

const int16 rsb_sincos_lut[RSB_SINCOS_LUT_SIZE][RSB_SINCOS_NUM] = 
{ 
  {16384, 0},
  {16384, 3},
  {16384, 6},
  {16384, 9},
  {16384, 11},
  {16384, 14},
  {16384, 17},
  {16384, 20},
  {16384, 23},
  {16384, 26},
  {16384, 29},
  {16384, 31},
  {16384, 34},
  {16384, 37},
  {16384, 40},
  {16384, 43},
  {16384, 46},
  {16384, 49},
  {16384, 51},
  {16384, 54},
  {16384, 57},
  {16384, 60},
  {16384, 63},
  {16384, 66},
  {16384, 69},
  {16384, 71},
  {16384, 74},
  {16384, 77},
  {16384, 80},
  {16384, 83},
  {16384, 86},
  {16384, 89},
  {16384, 92},
  {16384, 94},
  {16384, 97},
  {16384, 100},
  {16384, 103},
  {16384, 106},
  {16384, 109},
  {16384, 112},
  {16384, 114},
  {16384, 117},
  {16384, 120},
  {16384, 123},
  {16384, 126},
  {16383, 129},
  {16383, 132},
  {16383, 134},
  {16383, 137},
  {16383, 140},
  {16383, 143},
  {16383, 146},
  {16383, 149},
  {16383, 152},
  {16383, 154},
  {16383, 157},
  {16383, 160},
  {16383, 163},
  {16383, 166},
  {16383, 169},
  {16383, 172},
  {16383, 174},
  {16383, 177},
  {16383, 180},
  {16383, 183},
  {16383, 186},
  {16383, 189},
  {16383, 192},
  {16383, 194},
  {16383, 197},
  {16383, 200},
  {16383, 203},
  {16383, 206},
  {16383, 209},
  {16383, 212},
  {16383, 214},
  {16383, 217},
  {16383, 220},
  {16382, 223},
  {16382, 226},
  {16382, 229},
  {16382, 232},
  {16382, 234},
  {16382, 237},
  {16382, 240},
  {16382, 243},
  {16382, 246},
  {16382, 249},
  {16382, 252},
  {16382, 254},
  {16382, 257},
  {16382, 260},
  {16382, 263},
  {16382, 266},
  {16382, 269},
  {16382, 272},
  {16382, 275},
  {16382, 277},
  {16382, 280},
  {16382, 283},
  {16382, 286},
  {16381, 289},
  {16381, 292},
  {16381, 295},
  {16381, 297},
  {16381, 300},
  {16381, 303},
  {16381, 306},
  {16381, 309},
  {16381, 312},
  {16381, 315},
  {16381, 317},
  {16381, 320},
  {16381, 323},
  {16381, 326},
  {16381, 329},
  {16381, 332},
  {16381, 335},
  {16381, 337},
  {16380, 340},
  {16380, 343},
  {16380, 346},
  {16380, 349},
  {16380, 352},
  {16380, 355},
  {16380, 357},
  {16380, 360},
  {16380, 363},
  {16380, 366},
  {16380, 369},
  {16380, 372},
  {16380, 375},
  {16380, 377},
  {16380, 380},
  {16380, 383},
  {16379, 386},
  {16379, 389},
  {16379, 392},
  {16379, 395},
  {16379, 397},
  {16379, 400},
  {16379, 403},
  {16379, 406},
  {16379, 409},
  {16379, 412},
  {16379, 415},
  {16379, 417},
  {16379, 420},
  {16379, 423},
  {16378, 426},
  {16378, 429},
  {16378, 432},
  {16378, 435},
  {16378, 437},
  {16378, 440},
  {16378, 443},
  {16378, 446},
  {16378, 449},
  {16378, 452},
  {16378, 455},
  {16378, 457},
  {16378, 460},
  {16377, 463},
  {16377, 466},
  {16377, 469},
  {16377, 472},
  {16377, 475},
  {16377, 477},
  {16377, 480},
  {16377, 483},
  {16377, 486},
  {16377, 489},
  {16377, 492},
  {16377, 495},
  {16376, 497},
  {16376, 500},
  {16376, 503},
  {16376, 506},
  {16376, 509},
  {16376, 512},
  {16376, 515},
  {16376, 517},
  {16376, 520},
  {16376, 523},
  {16376, 526},
  {16375, 529},
  {16375, 532},
  {16375, 535},
  {16375, 537},
  {16375, 540},
  {16375, 543},
  {16375, 546},
  {16375, 549},
  {16375, 552},
  {16375, 555},
  {16375, 558},
  {16374, 560},
  {16374, 563},
  {16374, 566},
  {16374, 569},
  {16374, 572},
};

const int16 rsb_gain_lut[RSB_GAIN_LUT_SIZE][RSB_GAIN_NUM] = 
{ 
  {16384, 16384},
  {16382, 16386},
  {16380, 16388},
  {16378, 16390},
  {16376, 16392},
  {16375, 16393},
  {16373, 16395},
  {16371, 16397},
  {16369, 16399},
  {16367, 16401},
  {16365, 16403},
  {16363, 16405},
  {16361, 16407},
  {16359, 16409},
  {16358, 16410},
  {16356, 16412},
  {16354, 16414},
  {16352, 16416},
  {16350, 16418},
  {16348, 16420},
  {16346, 16422},
  {16344, 16424},
  {16343, 16426},
  {16341, 16427},
  {16339, 16429},
  {16337, 16431},
  {16335, 16433},
  {16333, 16435},
  {16331, 16437},
  {16329, 16439},
  {16328, 16441},
  {16326, 16443},
  {16324, 16444},
  {16322, 16446},
  {16320, 16448},
  {16318, 16450},
  {16316, 16452},
  {16314, 16454},
  {16312, 16456},
  {16311, 16458},
  {16309, 16460},
  {16307, 16462},
  {16305, 16463},
  {16303, 16465},
  {16301, 16467},
  {16299, 16469},
  {16297, 16471},
  {16296, 16473},
  {16294, 16475},
  {16292, 16477},
  {16290, 16479},
  {16288, 16480},
  {16286, 16482},
  {16284, 16484},
  {16282, 16486},
  {16281, 16488},
  {16279, 16490},
  {16277, 16492},
  {16275, 16494},
  {16273, 16496},
  {16271, 16498},
  {16269, 16499},
  {16267, 16501},
  {16266, 16503},
  {16264, 16505},
  {16262, 16507},
  {16260, 16509},
  {16258, 16511},
  {16256, 16513},
  {16254, 16515},
  {16252, 16517},
  {16251, 16518},
  {16249, 16520},
  {16247, 16522},
  {16245, 16524},
  {16243, 16526},
  {16241, 16528},
  {16239, 16530},
  {16238, 16532},
  {16236, 16534},
  {16234, 16536},
  {16232, 16538},
  {16230, 16539},
  {16228, 16541},
  {16226, 16543},
  {16224, 16545},
  {16223, 16547},
  {16221, 16549},
  {16219, 16551},
  {16217, 16553},
  {16215, 16555},
  {16213, 16557},
  {16211, 16558},
  {16210, 16560},
  {16208, 16562},
  {16206, 16564},
  {16204, 16566},
  {16202, 16568},
  {16200, 16570},
  {16198, 16572},
  {16196, 16574},
  {16195, 16576},
  {16193, 16578},
  {16191, 16579},
  {16189, 16581},
  {16187, 16583},
  {16185, 16585},
  {16183, 16587},
  {16182, 16589},
  {16180, 16591},
  {16178, 16593},
  {16176, 16595},
  {16174, 16597},
  {16172, 16599},
  {16170, 16600},
  {16169, 16602},
  {16167, 16604},
  {16165, 16606},
  {16163, 16608},
  {16161, 16610},
  {16159, 16612},
  {16157, 16614},
  {16155, 16616},
  {16154, 16618},
  {16152, 16620},
  {16150, 16621},
  {16148, 16623},
  {16146, 16625},
  {16144, 16627},
  {16142, 16629},
  {16141, 16631},
  {16139, 16633},
  {16137, 16635},
  {16135, 16637},
  {16133, 16639},
  {16131, 16641},
  {16129, 16643},
  {16128, 16644},
  {16126, 16646},
  {16124, 16648},
  {16122, 16650},
  {16120, 16652},
  {16118, 16654},
  {16116, 16656},
  {16115, 16658},
  {16113, 16660},
  {16111, 16662},
  {16109, 16664},
  {16107, 16666},
  {16105, 16667},
  {16103, 16669},
  {16102, 16671},
  {16100, 16673},
  {16098, 16675},
  {16096, 16677},
  {16094, 16679},
  {16092, 16681},
  {16091, 16683},
  {16089, 16685},
  {16087, 16687},
  {16085, 16689},
  {16083, 16691},
  {16081, 16692},
  {16079, 16694},
  {16078, 16696},
  {16076, 16698},
  {16074, 16700},
  {16072, 16702},
  {16070, 16704},
  {16068, 16706},
  {16066, 16708},
  {16065, 16710},
  {16063, 16712},
  {16061, 16714},
  {16059, 16716},
  {16057, 16717},
  {16055, 16719},
  {16054, 16721},
  {16052, 16723},
  {16050, 16725},
  {16048, 16727},
  {16046, 16729},
  {16044, 16731},
  {16042, 16733},
  {16041, 16735},
  {16039, 16737},
  {16037, 16739},
  {16035, 16741},
  {16033, 16742},
  {16031, 16744},
  {16029, 16746},
  {16028, 16748},
  {16026, 16750},
  {16024, 16752},
  {16022, 16754},
  {16020, 16756},
  {16018, 16758},
  {16017, 16760},
  {16015, 16762},
  {16013, 16764},
  {16011, 16766},
  {16009, 16768},
  {16007, 16769},
  {16006, 16771},
  {16004, 16773},
  {16002, 16775},
  {16000, 16777},
  {15998, 16779},
  {15996, 16781},
  {15994, 16783},
  {15993, 16785},
  {15991, 16787},
  {15989, 16789},
  {15987, 16791},
  {15985, 16793},
  {15983, 16795},
  {15982, 16797},
  {15980, 16798},
  {15978, 16800},
  {15976, 16802},
  {15974, 16804},
  {15972, 16806},
  {15971, 16808},
  {15969, 16810},
  {15967, 16812},
  {15965, 16814},
  {15963, 16816},
  {15961, 16818},
  {15960, 16820},
  {15958, 16822},
  {15956, 16824},
  {15954, 16826},
  {15952, 16828},
  {15950, 16829},
  {15949, 16831},
  {15947, 16833},
  {15945, 16835},
  {15943, 16837},
  {15941, 16839},
  {15939, 16841},
  {15937, 16843},
  {15936, 16845},
  {15934, 16847},
  {15932, 16849},
  {15930, 16851},
  {15928, 16853},
  {15926, 16855},
  {15925, 16857},
  {15923, 16859},
  {15921, 16860},
  {15919, 16862},
  {15917, 16864},
  {15915, 16866},
  {15914, 16868},
  {15912, 16870},
  {15910, 16872},
  {15908, 16874},
  {15906, 16876},
  {15904, 16878},
  {15903, 16880},
  {15901, 16882},
  {15899, 16884},
  {15897, 16886},
  {15895, 16888},
  {15894, 16890},
  {15892, 16892},
  {15890, 16894},
  {15888, 16895},
  {15886, 16897},
  {15884, 16899},
  {15883, 16901},
  {15881, 16903},
  {15879, 16905},
  {15877, 16907},
  {15875, 16909},
  {15873, 16911},
  {15872, 16913},
  {15870, 16915},
  {15868, 16917},
  {15866, 16919},
  {15864, 16921},
  {15862, 16923},
  {15861, 16925},
  {15859, 16927},
  {15857, 16929},
  {15855, 16931},
  {15853, 16932},
  {15851, 16934},
  {15850, 16936},
  {15848, 16938},
  {15846, 16940},
  {15844, 16942},
  {15842, 16944},
  {15841, 16946},
  {15839, 16948},
  {15837, 16950},
  {15835, 16952},
  {15833, 16954},
  {15831, 16956},
  {15830, 16958},
  {15828, 16960},
  {15826, 16962},
  {15824, 16964},
  {15822, 16966},
  {15820, 16968},
  {15819, 16970},
  {15817, 16971},
  {15815, 16973},
  {15813, 16975},
  {15811, 16977},
  {15810, 16979},
  {15808, 16981},
  {15806, 16983},
  {15804, 16985},
  {15802, 16987},
  {15800, 16989},
  {15799, 16991},
  {15797, 16993},
  {15795, 16995},
  {15793, 16997},
  {15791, 16999},
  {15790, 17001},
  {15788, 17003},
  {15786, 17005},
  {15784, 17007},
  {15782, 17009},
  {15780, 17011},
  {15779, 17013},
  {15777, 17015},
  {15775, 17016},
  {15773, 17018},
  {15771, 17020},
  {15770, 17022},
  {15768, 17024},
  {15766, 17026},
  {15764, 17028},
  {15762, 17030},
  {15760, 17032},
  {15759, 17034},
  {15757, 17036},
  {15755, 17038},
  {15753, 17040},
  {15751, 17042},
  {15750, 17044},
  {15748, 17046},
  {15746, 17048},
  {15744, 17050},
  {15742, 17052},
  {15741, 17054},
  {15739, 17056},
  {15737, 17058},
  {15735, 17060},
  {15733, 17062},
  {15731, 17064},
  {15730, 17066},
  {15728, 17068},
  {15726, 17069},
  {15724, 17071},
  {15722, 17073},
  {15721, 17075},
  {15719, 17077},
  {15717, 17079},
  {15715, 17081},
  {15713, 17083},
  {15712, 17085},
  {15710, 17087},
  {15708, 17089},
  {15706, 17091},
  {15704, 17093},
  {15703, 17095},
  {15701, 17097},
  {15699, 17099},
  {15697, 17101},
  {15695, 17103},
  {15694, 17105},
  {15692, 17107},
  {15690, 17109},
  {15688, 17111},
  {15686, 17113},
  {15684, 17115},
  {15683, 17117},
  {15681, 17119},
  {15679, 17121},
  {15677, 17123},
  {15675, 17125},
  {15674, 17127},
  {15672, 17129},
  {15670, 17130},
  {15668, 17132},
  {15666, 17134},
  {15665, 17136},
  {15663, 17138},
  {15661, 17140},
  {15659, 17142},
  {15657, 17144},
  {15656, 17146},
  {15654, 17148},
  {15652, 17150},
  {15650, 17152},
  {15648, 17154},
  {15647, 17156},
  {15645, 17158},
  {15643, 17160},
  {15641, 17162},
  {15639, 17164},
  {15638, 17166},
  {15636, 17168},
  {15634, 17170},
  {15632, 17172},
  {15630, 17174},
  {15629, 17176},
  {15627, 17178},
  {15625, 17180},
  {15623, 17182},
  {15621, 17184},
  {15620, 17186},
  {15618, 17188},
  {15616, 17190},
  {15614, 17192},
  {15612, 17194},
  {15611, 17196},
  {15609, 17198},
  {15607, 17200},
  {15605, 17202},
  {15603, 17204},
  {15602, 17206},
  {15600, 17208},
  {15598, 17210},
  {15596, 17212},
  {15594, 17214},
  {15593, 17216},
  {15591, 17217},
  {15589, 17219},
  {15587, 17221},
  {15585, 17223},
  {15584, 17225},
  {15582, 17227},
  {15580, 17229},
  {15578, 17231},
  {15577, 17233},
  {15575, 17235},
  {15573, 17237},
  {15571, 17239},
  {15569, 17241},
  {15568, 17243},
  {15566, 17245},
  {15564, 17247},
  {15562, 17249},
  {15560, 17251},
  {15559, 17253},
  {15557, 17255},
  {15555, 17257},
  {15553, 17259},
  {15551, 17261},
  {15550, 17263},
  {15548, 17265},
  {15546, 17267},
  {15544, 17269},
  {15542, 17271},
  {15541, 17273},
  {15539, 17275},
  {15537, 17277},
  {15535, 17279},
  {15534, 17281},
  {15532, 17283},
  {15530, 17285},
  {15528, 17287},
  {15526, 17289},
  {15525, 17291},
  {15523, 17293},
  {15521, 17295},
  {15519, 17297},
  {15517, 17299},
  {15516, 17301},
  {15514, 17303},
  {15512, 17305},
  {15510, 17307},
  {15509, 17309},
  {15507, 17311},
  {15505, 17313},
  {15503, 17315},
  {15501, 17317},
  {15500, 17319},
  {15498, 17321},
  {15496, 17323},
  {15494, 17325},
  {15492, 17327},
  {15491, 17329},
  {15489, 17331},
  {15487, 17333},
  {15485, 17335},
  {15484, 17337},
  {15482, 17339},
  {15480, 17341},
  {15478, 17343},
  {15476, 17345},
  {15475, 17347},
  {15473, 17349},
  {15471, 17351},
  {15469, 17353},
  {15467, 17355},
  {15466, 17357},
  {15464, 17359},
  {15462, 17361},
  {15460, 17363},
  {15459, 17365},
  {15457, 17367},
  {15455, 17369},
  {15453, 17371},
  {15451, 17373},
  {15450, 17375},
  {15448, 17377},
  {15446, 17379},
  {15444, 17381},
  {15443, 17383},
  {15441, 17385},
  {15439, 17387},
  {15437, 17389},
  {15435, 17391},
  {15434, 17393},
  {15432, 17395},
  {15430, 17397},
  {15428, 17399},
  {15427, 17401},
  {15425, 17403},
  {15423, 17405},
  {15421, 17407},
  {15419, 17409},
  {15418, 17411},
  {15416, 17413},
  {15414, 17415},
  {15412, 17417},
  {15411, 17419},
  {15409, 17421},
  {15407, 17423},
  {15405, 17425},
  {15404, 17427},
  {15402, 17429},
  {15400, 17431},
  {15398, 17433},
  {15396, 17435},
  {15395, 17437},
  {15393, 17439},
  {15391, 17441},
  {15389, 17443},
  {15388, 17445},
  {15386, 17447},
  {15384, 17449},
  {15382, 17451},
  {15380, 17453},
  {15379, 17455},
  {15377, 17457},
  {15375, 17459},
  {15373, 17461},
  {15372, 17463},
  {15370, 17465},
  {15368, 17467},
  {15366, 17469},
  {15365, 17471},
  {15363, 17473},
  {15361, 17475},
  {15359, 17477},
  {15357, 17479},
  {15356, 17481},
  {15354, 17483},
  {15352, 17485},
  {15350, 17487},
  {15349, 17489},
  {15347, 17491},
  {15345, 17493},
  {15343, 17495},
  {15342, 17497},
  {15340, 17499},
  {15338, 17501},
  {15336, 17503},
  {15335, 17505},
  {15333, 17507},
  {15331, 17509},
  {15329, 17511},
  {15327, 17513},
  {15326, 17515},
  {15324, 17517},
  {15322, 17519},
  {15320, 17521},
  {15319, 17523},
  {15317, 17525},
  {15315, 17527},
  {15313, 17530},
  {15312, 17532},
  {15310, 17534},
  {15308, 17536},
  {15306, 17538},
  {15305, 17540},
  {15303, 17542},
  {15301, 17544},
  {15299, 17546},
  {15297, 17548},
  {15296, 17550},
  {15294, 17552},
  {15292, 17554},
  {15290, 17556},
  {15289, 17558},
  {15287, 17560},
  {15285, 17562},
  {15283, 17564},
  {15282, 17566},
  {15280, 17568},
  {15278, 17570},
  {15276, 17572},
  {15275, 17574},
  {15273, 17576},
  {15271, 17578},
  {15269, 17580},
  {15268, 17582},
  {15266, 17584},
  {15264, 17586},
  {15262, 17588},
  {15261, 17590},
  {15259, 17592},
  {15257, 17594},
  {15255, 17596},
  {15254, 17598},
  {15252, 17600},
  {15250, 17602},
  {15248, 17604},
  {15246, 17606},
  {15245, 17608},
  {15243, 17610},
  {15241, 17612},
  {15239, 17614},
  {15238, 17617},
  {15236, 17619},
  {15234, 17621},
  {15232, 17623},
  {15231, 17625},
  {15229, 17627},
  {15227, 17629},
  {15225, 17631},
  {15224, 17633},
  {15222, 17635},
  {15220, 17637},
  {15218, 17639},
  {15217, 17641},
  {15215, 17643},
  {15213, 17645},
  {15211, 17647},
  {15210, 17649},
  {15208, 17651},
  {15206, 17653},
  {15204, 17655},
  {15203, 17657},
  {15201, 17659},
  {15199, 17661},
  {15197, 17663},
  {15196, 17665},
  {15194, 17667},
  {15192, 17669},
  {15190, 17671},
  {15189, 17673},
  {15187, 17675},
  {15185, 17677},
  {15183, 17679},
  {15182, 17682},
  {15180, 17684},
  {15178, 17686},
  {15176, 17688},
  {15175, 17690},
  {15173, 17692},
  {15171, 17694},
  {15169, 17696},
  {15168, 17698},
  {15166, 17700},
  {15164, 17702},
  {15162, 17704},
  {15161, 17706},
  {15159, 17708},
  {15157, 17710},
  {15155, 17712},
  {15154, 17714},
  {15152, 17716},
  {15150, 17718},
  {15149, 17720},
  {15147, 17722},
  {15145, 17724},
  {15143, 17726},
  {15142, 17728},
  {15140, 17730},
  {15138, 17732},
  {15136, 17735},
  {15135, 17737},
  {15133, 17739},
  {15131, 17741},
  {15129, 17743},
  {15128, 17745},
  {15126, 17747},
  {15124, 17749},
  {15122, 17751},
  {15121, 17753},
  {15119, 17755},
  {15117, 17757},
  {15115, 17759},
  {15114, 17761},
  {15112, 17763},
  {15110, 17765},
  {15108, 17767},
  {15107, 17769},
  {15105, 17771},
  {15103, 17773},
  {15101, 17775},
  {15100, 17777},
  {15098, 17780},
  {15096, 17782},
  {15095, 17784},
  {15093, 17786},
  {15091, 17788},
  {15089, 17790},
  {15088, 17792},
  {15086, 17794},
  {15084, 17796},
  {15082, 17798},
  {15081, 17800},
  {15079, 17802},
  {15077, 17804},
  {15075, 17806},
  {15074, 17808},
  {15072, 17810},
  {15070, 17812},
  {15068, 17814},
  {15067, 17816},
  {15065, 17818},
  {15063, 17821},
  {15062, 17823},
  {15060, 17825},
  {15058, 17827},
  {15056, 17829},
  {15055, 17831},
  {15053, 17833},
  {15051, 17835},
  {15049, 17837},
  {15048, 17839},
  {15046, 17841},
  {15044, 17843},
  {15042, 17845},
  {15041, 17847},
  {15039, 17849},
  {15037, 17851},
  {15036, 17853},
  {15034, 17855},
  {15032, 17857},
  {15030, 17860},
  {15029, 17862},
  {15027, 17864},
  {15025, 17866},
  {15023, 17868},
  {15022, 17870},
  {15020, 17872},
  {15018, 17874},
  {15017, 17876},
  {15015, 17878},
  {15013, 17880},
  {15011, 17882},
  {15010, 17884},
  {15008, 17886},
  {15006, 17888},
  {15004, 17890},
  {15003, 17892},
  {15001, 17895},
  {14999, 17897},
  {14998, 17899},
  {14996, 17901},
  {14994, 17903},
  {14992, 17905},
  {14991, 17907},
  {14989, 17909},
  {14987, 17911},
  {14985, 17913},
  {14984, 17915},
  {14982, 17917},
  {14980, 17919},
  {14979, 17921},
  {14977, 17923},
  {14975, 17925},
  {14973, 17928},
  {14972, 17930},
  {14970, 17932},
  {14968, 17934},
  {14966, 17936},
  {14965, 17938},
  {14963, 17940},
  {14961, 17942},
  {14960, 17944},
  {14958, 17946},
  {14956, 17948},
  {14954, 17950},
  {14953, 17952},
  {14951, 17954},
  {14949, 17956},
  {14948, 17958},
  {14946, 17961},
  {14944, 17963},
  {14942, 17965},
  {14941, 17967},
  {14939, 17969},
  {14937, 17971},
  {14936, 17973},
  {14934, 17975},
  {14932, 17977},
  {14930, 17979},
  {14929, 17981},
  {14927, 17983},
  {14925, 17985},
  {14923, 17987},
  {14922, 17990},
  {14920, 17992},
  {14918, 17994},
  {14917, 17996},
  {14915, 17998},
  {14913, 18000},
  {14911, 18002},
  {14910, 18004},
  {14908, 18006},
  {14906, 18008},
  {14905, 18010},
  {14903, 18012},
  {14901, 18014},
  {14899, 18016},
  {14898, 18019},
  {14896, 18021},
  {14894, 18023},
  {14893, 18025},
  {14891, 18027},
  {14889, 18029},
  {14887, 18031},
  {14886, 18033},
  {14884, 18035},
  {14882, 18037},
  {14881, 18039},
  {14879, 18041},
  {14877, 18043},
  {14875, 18046},
  {14874, 18048},
  {14872, 18050},
  {14870, 18052},
  {14869, 18054},
  {14867, 18056},
  {14865, 18058},
  {14863, 18060},
  {14862, 18062},
  {14860, 18064},
  {14858, 18066},
  {14857, 18068},
  {14855, 18070},
  {14853, 18073},
  {14851, 18075},
  {14850, 18077},
  {14848, 18079},
  {14846, 18081},
  {14845, 18083},
  {14843, 18085},
  {14841, 18087},
  {14840, 18089},
  {14838, 18091},
  {14836, 18093},
  {14834, 18095},
  {14833, 18098},
  {14831, 18100},
  {14829, 18102},
  {14828, 18104},
  {14826, 18106},
  {14824, 18108},
  {14822, 18110},
  {14821, 18112},
  {14819, 18114},
  {14817, 18116},
  {14816, 18118},
  {14814, 18120},
  {14812, 18123},
  {14811, 18125},
  {14809, 18127},
  {14807, 18129},
  {14805, 18131},
  {14804, 18133},
  {14802, 18135},
  {14800, 18137},
  {14799, 18139},
  {14797, 18141},
  {14795, 18143},
  {14793, 18146},
  {14792, 18148},
  {14790, 18150},
  {14788, 18152},
  {14787, 18154},
  {14785, 18156},
  {14783, 18158},
  {14782, 18160},
  {14780, 18162},
  {14778, 18164},
  {14776, 18166},
  {14775, 18169},
  {14773, 18171},
  {14771, 18173},
  {14770, 18175},
  {14768, 18177},
  {14766, 18179},
  {14765, 18181},
  {14763, 18183},
  {14761, 18185},
  {14759, 18187},
  {14758, 18189},
  {14756, 18192},
  {14754, 18194},
  {14753, 18196},
  {14751, 18198},
  {14749, 18200},
  {14748, 18202},
  {14746, 18204},
  {14744, 18206},
  {14742, 18208},
  {14741, 18210},
  {14739, 18213},
  {14737, 18215},
  {14736, 18217},
  {14734, 18219},
  {14732, 18221},
  {14731, 18223},
  {14729, 18225},
  {14727, 18227},
  {14725, 18229},
  {14724, 18231},
  {14722, 18233},
  {14720, 18236},
  {14719, 18238},
  {14717, 18240},
  {14715, 18242},
  {14714, 18244},
  {14712, 18246},
  {14710, 18248},
  {14709, 18250},
  {14707, 18252},
  {14705, 18254},
  {14703, 18257},
  {14702, 18259},
  {14700, 18261},
  {14698, 18263},
  {14697, 18265},
  {14695, 18267},
  {14693, 18269},
  {14692, 18271},
  {14690, 18273},
  {14688, 18276},
  {14687, 18278},
  {14685, 18280},
  {14683, 18282},
  {14681, 18284},
  {14680, 18286},
  {14678, 18288},
  {14676, 18290},
  {14675, 18292},
  {14673, 18294},
  {14671, 18297},
  {14670, 18299},
  {14668, 18301},
  {14666, 18303},
  {14665, 18305},
  {14663, 18307},
  {14661, 18309},
  {14660, 18311},
  {14658, 18313},
  {14656, 18316},
  {14654, 18318},
  {14653, 18320},
  {14651, 18322},
  {14649, 18324},
  {14648, 18326},
  {14646, 18328},
  {14644, 18330},
  {14643, 18332},
  {14641, 18335},
  {14639, 18337},
  {14638, 18339},
  {14636, 18341},
  {14634, 18343},
  {14633, 18345},
  {14631, 18347},
  {14629, 18349},
  {14627, 18351},
  {14626, 18354},
  {14624, 18356},
  {14622, 18358},
  {14621, 18360},
  {14619, 18362},
  {14617, 18364},
  {14616, 18366},
  {14614, 18368},
  {14612, 18370},
  {14611, 18373},
  {14609, 18375},
  {14607, 18377},
  {14606, 18379},
  {14604, 18381},
  {14602, 18383},
};


/*!
  @name: compute_rsb

  @brief: rsb value based on rsb coef 
   this function is for returning to use so that they can judge if the calibrated rsb is good or not
 
  @details
  
  @param
  input: rsb_data: rsb coef for rsb computation

  
  @return
     rsb value :
*/

double 	compute_rsb
(
	rfdevice_rsb_cal_data_type rsb_data
)
{
  double gain_inv_raw = 0;
  double cos_theta_raw = 0;
  double rsb_raw = 0;

//========================= for debuging=======================

  double sin_theta_float = 0;
  double cos_theta_float = 0;
  double gain_inv_float = 0;
  double rsb_float = 0;
  

//===========================
  
  gain_inv_raw = rsb_data.gain_inv_f/pow((double) 2, (double) 14);
  cos_theta_raw = rsb_data.cos_theta_f/pow((double) 2, (double) 14);
  rsb_raw = 10*log10( (gain_inv_raw*gain_inv_raw+2*gain_inv_raw*cos_theta_raw+1)/(gain_inv_raw*gain_inv_raw-2*gain_inv_raw*cos_theta_raw+1) );

//  double sin_theta_val = rsb_data.i_q/rsb_data.iq
  sin_theta_float = ((double)rsb_data.iq)/((double) (sqrt(((double)rsb_data.i_sq)  * ((double)rsb_data.q_sq))));
  cos_theta_float = ((double) (sqrt( ((double)1.0) - (sin_theta_float*sin_theta_float)))) ;
  gain_inv_float = (double) (sqrt(((double)rsb_data.i_sq)/((double) rsb_data.q_sq)));
  
  rsb_float = 10*log10( (gain_inv_float*gain_inv_float+2*gain_inv_float*cos_theta_float+1.0)/(gain_inv_float*gain_inv_float-2*gain_inv_float*cos_theta_float+1.0) );

  FTM_MSG_2(FTM_MED,"rsb calibrated using cos_theta_f: %d, using raw_iq_double: %d", ((int) floor(rsb_raw)), ((int) floor(rsb_float)));
      
  return rsb_float;	
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  common_compute_rsb_cal_params_gain_phase_lut_index

  @details
  this function is used to calculate the nv values for sin(theta); cos(theta) and 1/g referenced to the temperature at which calibration is done. this function also returns the value of gain and phase lut indexes along with the polarity of gain and phase error. if calibration for a given band/port enum is not supported this function will return the values corresponding to 25 degree celsius from rfic characterized data and lut indexes are then returned as 0s. input:uint64 i_sq; uint64 q_sq; int64 iq; uint16 therm_read; uint8 stg_is_bad_fuse; wtr1625_rsb_data_type* g_ph_vec;  output:int16* sin_val; int16* cos_val; int16* gain_inv_val; uint16* gain_lut_index; uint16* phase_lut_index; uint8* gain_sign; int8* phase_sign
 
  @param 
  common_instance_type* instance, uint64 i_sq, uint64 q_sq, int64 iq, uint16 therm_read, uint8 stg_is_bad_fuse, const wtr1625_rsb_data_type* g_ph_vec, int16* sin_val, int16* cos_val, int16* gain_inv_val, uint16* gain_lut_index, uint16* phase_lut_index, uint8* gain_sign, int8* phase_sign
*/

void common_compute_rsb_cal_params_gain_phase_lut_index(uint64 i_sq, uint64 q_sq, int64 iq, uint16 therm_read,
		uint8 stg_is_bad_fuse, const rsb_three_thermal_range_data_type* g_ph_vec, 
		int16* sin_val, int16* cos_val, int16* gain_inv_val, 
		uint16* gain_lut_index, uint16* phase_lut_index, 
		int8* gain_sign, int8* phase_sign)
{
  int16 temp_slope = 0;
  uint16 therm_25_c = 0;
  int16 power14 = 0;
  int32 delta_temp = 0;
  uint8 stg_is_bad_fuse_mod = 0;
  int16 gain_delta = 0;
  int16 phase_delta = 0;
  uint32 delta_temp_abs = 0;
  uint32 max_min_temp_diff = 0;
  uint16 gain_delta_abs = 0;
  uint16 phase_delta_abs = 0;
  double sin_val_float = 0;

  /* slope of temp comp multipled by 10000 */
  temp_slope = ((int16)1025)*(-1);

  /* thermistor reading at 25 degrees */
  therm_25_c = 1196;

  /* 2^14 */
  power14 = 16384;

// hard code therm_read as currently it reads 0
  therm_read = therm_25_c; // hard code to room temp
  
  if ((therm_read == 0)) { 

  delta_temp = 0;

  }
  else { 

  /* delta temperature from room temp */
  delta_temp = ((int32)temp_slope)*((int32)(therm_read-therm_25_c));

  }


    stg_is_bad_fuse_mod =0;
  	/* only ic data and no cal data */
  	if (((g_ph_vec->rsb_calibration_required== FALSE)||(stg_is_bad_fuse_mod == 1))) { 

  		/* 1/g value from the ic characterized data */
  		*gain_inv_val = g_ph_vec->error_gain_room_x2pow14;

  		/* cos(theta) value from ic characterized data */
  		*cos_val = (g_ph_vec->cos_phase_room_x2pow14)*((int16)-1);

  		/* sin(theta) value from ic characterized data */
  		*sin_val = (g_ph_vec->sin_phase_room_x2pow14);

  		/* lut index defaults to 0 */
  		*gain_lut_index = 0;

  		/* lut index defaults to 0 */
  		*phase_lut_index = 0;

  		/* gain sign defaults to +ve */
  		*gain_sign = 0;

  		/* phase sign defaults to +ve */
  		*phase_sign = 1;

 		 /* if rsb params are to be computed through calibration */
  	}
  	else { 

  		if ((delta_temp < 0)) { 

 			 /* gain_delta in 1/1000 db between -35 and +25  degrees */
  			gain_delta = (g_ph_vec-> error_gain_cold_x1000)  -   ( g_ph_vec-> error_gain_room_x1000);

  			/* phase_delta in 1/1000 degree between -35 and +25  degrees */
  			phase_delta = (g_ph_vec-> error_phase_cold_x1000)  -   ( g_ph_vec-> error_phase_room_x1000);

  			/* absolue delta temp */
  			delta_temp_abs = (uint32)(delta_temp*((int32)(-1)));

  			/* difference between cold and room temp in spreadsheet data = 25-(-35) */
  			max_min_temp_diff = 60;

  		}
  		else { 
  	

  			/* gain_delta in 1/1000 db between +95 and +25  degrees */
  			gain_delta = (g_ph_vec -> error_gain_hot_x1000)  -   ( g_ph_vec-> error_gain_room_x1000);

  			/* phase_delta in 1/1000 degree between -95 and +25  degrees */
  			phase_delta = (g_ph_vec-> error_phase_hot_x1000)  -   ( g_ph_vec-> error_phase_room_x1000);

  			/* absolue delta temp */
  			delta_temp_abs = (uint32)delta_temp;

  			/* difference between hot and room temp in spreadsheet data = (95-25) */
  			max_min_temp_diff = 70;

  		}

  		if ((gain_delta < 0)) { 

  			/* sign is 1 if gain_delta is < 0 */
  			*gain_sign = 1;

  			/* absolute gain delta */
  			gain_delta_abs = (uint16)(gain_delta*((int16)(-1)));

  		}
  		else { 

  			/* sign is 0 if gain_delta is >= 0 */
  			*gain_sign = 0;

  			/* absolute gain delta */
  			gain_delta_abs = (uint16)gain_delta;

  		}

  		if ((phase_delta < 0)) { 

 			 /* sign is 1 if phase_delta is < 0 */
  			*phase_sign = -1;

  			/* absolute phase delta */
  			phase_delta_abs = (uint16)(phase_delta*((int16)(-1)));

  		}
  		else { 

  			/* sign is 0 if phase_delta is >= 0 */
  			*phase_sign = 1;

  			/* absolute phase delta */
  			phase_delta_abs = (uint16)phase_delta;

  		}

  		/* compute lut index for gain delta table */
  		*gain_lut_index = (uint16)(( ((uint32)(delta_temp_abs * gain_delta_abs)) + (((uint32)(max_min_temp_diff*10000)) >>1))/((uint32)(max_min_temp_diff*10000)));

  		/* compute lut index phase for sine and cosine delta table */
  		*phase_lut_index = (uint16)((  ((uint32)(delta_temp_abs * phase_delta_abs)) + (((uint32)(max_min_temp_diff*100000)) >>1))/((uint32)(max_min_temp_diff*100000)));

  		/* compute gain_inverse for calibration measurement */
  		*gain_inv_val = (int16)(((double)(sqrt(  ((double)i_sq)/((double)q_sq) ))) * ((double)power14));
 	//	*gain_inv_val = (int16)(((double)(sqrt(  ((double)q_sq)/((double)i_sq) ))) * ((double)power14)); //xh hard code to see any improvement

  		sin_val_float = ((double)iq)/((double) (sqrt(((double)i_sq)  * ((double)q_sq))));

  		/* compute cos(theta) for calibration measurement */
  		*cos_val = (int16)(((double) (sqrt( ((double)1.0) - (sin_val_float*sin_val_float) )))  * ((double)power14));

  		/* compute sin(theta) for calibration measurement */
  		*sin_val = (int16)(sin_val_float * ((double)power14));

  	}
  	FTM_MSG_7(FTM_MED," gain_inv_val:%d, cos_val:%d, sin_val:%d, gain_lut_index:%d, phase_lut_index:%d, gain_sign:%d, phase_sign:%d\n", 
  			*gain_inv_val, *cos_val, *sin_val,*gain_lut_index,*phase_lut_index, *gain_sign,*phase_sign);   
    FTM_MSG_3(FTM_MED,"gain_delta: %d, phase_delta:%d,delta_temp_abs:%d",gain_delta, phase_delta, delta_temp_abs);
}



/*----------------------------------------------------------------------------*/
/*!
  @brief
  common_compute_rsb_cal_nv_params

  @details
  this function is used to calculate the nv values for sin(theta); cos(theta) and 1/g referenced to 25 degrees celsius that are stored in nv. input:int16 sin_val; int16 cos_val; int16 gain_inv_val; int16 sin_delta; int16 cos_delta; int16 gain_inv_delta; output:int16* sin_theta_final; int16* cos_theta_final; int16* gain_inv_final
 
  @param 
  wtr1625_common_instance_type* instance, int16 sin_val, int16 cos_val, int16 gain_inv_val, int16 sin_delta, int16 cos_delta, int16 gain_inv_delta, int16* sin_theta_final, int16* cos_theta_final, int16* gain_inv_final
*/

void common_compute_rsb_cal_nv_params(int16 sin_val, int16 cos_val, int16 gain_inv_val, int16 sin_delta, int16 cos_delta, int16 gain_inv_delta, int16* sin_theta_final, int16* cos_theta_final, int16* gain_inv_final)
{
  int32 power14 = 0;
  int32 sin_theta = 0;
  int32 cos_theta = 0;
  int32 gain_inv = 0;

  power14 = 16384;

  /* compute sin(theta+delta_theta) */
  sin_theta = (int32)((((int32)sin_val) *((int32)cos_delta))  + (((int32)cos_val) *((int32)sin_delta)));

  /* compute cos(theta+delta_theta) */
  cos_theta = (int32)((((int32)cos_val) *((int32)cos_delta))  - (((int32)sin_val) *((int32)sin_delta)));

  /* compute 1/g * 1/delta_g */
  gain_inv = (int32)(((int32)gain_inv_val)*((int32)gain_inv_delta));

  if ((sin_theta >= 0)) { 

  /* rounding off sin theta */
  *sin_theta_final = (int16)(((int32)(sin_theta + (power14 >> 1)))>>14);

  }
  else { 

  /* rounding off sin theta */
  *sin_theta_final = (int16)(((int32)(sin_theta - (power14 >> 1)))>>14);

  }

  if ((cos_theta >= 0)) { 

  /* rounding off cos theta */
  *cos_theta_final = (int16)(((int32)(cos_theta + (power14 >> 1)))>>14);

  }
  else { 

  /* rounding off cos theta */
  *cos_theta_final = (int16)(((int32)(cos_theta - (power14 >> 1)))>>14);

  }

  if ((gain_inv >= 0)) { 

  /* rounding off gain */
  *gain_inv_final = (int16)(((int32)(gain_inv + (power14 >> 1)))>>14);

  }
  else { 

  /* rounding off gain */
  *gain_inv_final = (int16)(((int32)(gain_inv - (power14 >> 1)))>>14);

  }

}

boolean ftm_compute_rsb_nv_params
( 
  rfm_device_enum_type rfm_device,
  uint8  tech,
  uint16 rf_band, 
  rfcom_lte_bw_type rx_bw,
  uint64   i_sq,
  uint64   q_sq,
  int64    i_q,
  int16    therm_data, 
  int16*   sin_theta_final, 
  int16*   cos_theta_final, 
  int16*   gain_inv_final,
  uint8 stg_is_bad_fuse
)
{

  int16 sin_val, cos_val, gain_inv_val;
  uint16 gain_lut_index, phase_lut_index;
  int8 gain_sign, phase_sign;
  int16   sin_delta; 
  int16   cos_delta; 
  int16   gain_inv_delta; 
  //uint8   div_ratio = 0;
  
  rsb_three_thermal_range_data_type  gain_phase_vec = {TRUE,-369,-307,-229,-51,31,144,-9,-16384,15815,9750}; // hacked for now. 
  //rsb_three_thermal_range_data_type  gain_phase_vec = {TRUE,-472,-367,-256,-21,48,144,-14,-16384,15706,0}; // hacked for now. 

//{TRUE,-381,-284,-167,2,100,275,-29,-16384,15857}, {TRUE,-369,-307,-229,-51,31,144,-9,-16384,15815}, {TRUE,-342,-234,-108,81,207,404,-59,-16384,15949}

/*
  void common_compute_rsb_cal_params_gain_phase_lut_index(uint64 i_sq, uint64 q_sq, int64 iq, uint16 therm_read,
		  uint8 stg_is_bad_fuse, const rsb_three_thermal_range_data_type* g_ph_vec, 
		  int16* sin_val, int16* cos_val, int16* gain_inv_val, 
		  uint16* gain_lut_index, uint16* phase_lut_index, 
		  uint8* gain_sign, int8* phase_sign)

 */
  common_compute_rsb_cal_params_gain_phase_lut_index( 
			i_sq, 
			q_sq, 
			i_q, 
			therm_data, 
			stg_is_bad_fuse,
			&gain_phase_vec, 
			&sin_val,  //below are output values
			&cos_val, 
			&gain_inv_val, 
			&gain_lut_index, 
			&phase_lut_index, 
			&gain_sign, 
			&phase_sign);

  /* Calculating RSB NVs step #2 */
  if ( (phase_lut_index < RSB_SINCOS_LUT_SIZE) &&
	   (gain_lut_index < RSB_GAIN_LUT_SIZE)&&
	   (gain_sign < RSB_GAIN_NUM) )
  {
	sin_delta = phase_sign * rsb_sincos_lut[phase_lut_index][RSB_SINE];
	cos_delta = rsb_sincos_lut[phase_lut_index][RSB_COSINE];
	gain_inv_delta = rsb_gain_lut[gain_lut_index][gain_sign];
	FTM_MSG_5(FTM_MED, "phase_lut_index :%d, sin_delta:%d, cos_delta:%d, gain_lut_index:%d, gain_inv_delta:%d", phase_lut_index, sin_delta, 
		cos_delta, gain_lut_index, gain_inv_delta);  
  }
  else
  {  
	MSG_3(MSG_SSID_RF, MSG_LEGACY_LOW,"Invalid phase_lut_index=%d,gain_lut_index=%d,gain_sign=%d ", 
			phase_lut_index,gain_lut_index,gain_sign);
  
	return FALSE;
  }


  /* Calculating RSB NVs step #2, sin_theta +sin_delta_temp_theta*/
  common_compute_rsb_cal_nv_params( 
		   sin_val, 
		   cos_val, 
		   gain_inv_val, 
		   sin_delta, 
		   cos_delta, 
		   gain_inv_delta, 
		   sin_theta_final, //below are output values 
		   cos_theta_final, 
		   gain_inv_final);
  FTM_MSG_3(FTM_MED,"sin_theta_final:%d, cos_theta_final:%d, gain_inv_final:%d", *sin_theta_final, *cos_theta_final, *gain_inv_final);
  

   return TRUE;
}


 



/*----------------------------------------------------------------------------*/
/*!
  @brief
  ftm_common_compute_rsb_coeffs_a_b_params

  @details
  this function is used to calculate hw written values of coeff_a and coeff_b. input: int16 sin_val; int16 cos_val; int16 gain_inv_val; int16 sin_delta; int16 cos_delta; int16 gain_inv_delta; uint8 rsb_cal_is_on. output: int16* a_coeff; int16* b_coeff
 
  @param 
  wtr3925_common_instance_class* instance, int16 sin_val, int16 cos_val, int16 gain_inv_val, int16 sin_delta, int16 cos_delta, int16 gain_inv_delta, uint8 rsb_cal_is_on, int32* a_coeff, int32* b_coeff
*/

void ftm_common_compute_rsb_coeffs_a_b_params(int16 sin_val, int16 cos_val, int16 gain_inv_val, int16 sin_delta, int16 cos_delta, int16 gain_inv_delta, uint8 rsb_cal_is_on,
	int32* a_coeff, int32* b_coeff)
{
  int32 power15 = 0;
  int32 denom = 0;
  int32 a_coeff_raw = 0;
  int32 b_coeff_raw = 0;

  power15 = 32768;

  denom = (int32)((((int32)sin_val) *((int32)sin_delta)) - (((int32)cos_val) *((int32)cos_delta)));

  a_coeff_raw = (int32)((((int32)sin_val) *((int32)cos_delta)) + (((int32)cos_val) *((int32)sin_delta)));

  b_coeff_raw = (int32)(((int32)gain_inv_val)*((int32)gain_inv_delta) *(-1));

  if ((rsb_cal_is_on ==0)) { 

	  if ((denom > 0)) { 

  		if ((a_coeff_raw >= 0)) { 

  			/* rounding off for a_coeff = -tan(theta) */
  			*a_coeff = (int32)(((((int64)a_coeff_raw) * ((int64)power15)) + ((int64) (((int64)denom) >> 1)))/ ((int64)denom));

  		}
  		else { 

  		/* rounding off for a_coeff = -tan(theta) */
  			*a_coeff = (int32)(((((int64)a_coeff_raw) * ((int64)power15)) - ((int64) (((int64)denom) >> 1)))/ ((int64)denom));

  		}

  		if ((b_coeff_raw >= 0)) { 

  		/* rounding off for b_coeff =1/(gcos(theta)) */
  			*b_coeff = (int32)(((((int64)b_coeff_raw) * ((int64)power15)) + ((int64)(((int64)denom) >> 1)))/ ((int64)denom));

  		}
  		else { 

  		/* rounding off for b_coeff =1/(gcos(theta)) */
  			*b_coeff = (int32)(((((int64)b_coeff_raw) * ((int64)power15)) - ((int64)(((int64)denom) >> 1)))/ ((int64)denom));

  		}

  	}
  	else if ((denom < 0)) { 

  		if ((a_coeff_raw >= 0)) { 

  			/* rounding off for a_coeff = -tan(theta) */
  			*a_coeff = (int32)(((((int64)a_coeff_raw) * ((int64)power15)) + ((int64) (((int64)denom) >> 1)))/ ((int64)denom));

  		}
  		else { 

  			/* rounding off for a_coeff = -tan(theta) */
  			*a_coeff = (int32)(((((int64)a_coeff_raw) * ((int64)power15)) - ((int64) (((int64)denom) >> 1)))/ ((int64)denom));

  		}

  		if ((b_coeff_raw >= 0)) { 

  			/* rounding off for b_coeff =1/(gcos(theta)) */
  			*b_coeff = (int32)(((((int64)b_coeff_raw) * ((int64)power15)) + ((int64)(((int64)denom) >> 1)))/ ((int64)denom));

  		}
  		else { 

  			/* rounding off for b_coeff =1/(gcos(theta)) */
  		*b_coeff = (int32)(((((int64)b_coeff_raw) * ((int64)power15)) - ((int64)(((int64)denom) >> 1)))/ ((int64)denom));

  		}

  	}
  	else { 




		*a_coeff = 0;

  		*b_coeff = 32768;

  	}

  }
  else { 

  *a_coeff = 0;

  *b_coeff = 32768;

  }

}

#endif /* FEATURE_FACTORY_TESTMODE */

#endif /* FEATURE_GNSS_ONLY_NO_WWAN */

/*! @} */


