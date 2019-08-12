#ifndef NAS_UTILS_H
#define NAS_UTILS_H

/*=========================================================================
                 DEFINITIONS AND DECLARATIONS FOR THE MODULE
===========================================================================*/

#ifdef FEATURE_DUAL_SIM

#define MSG_HIGH_DS_0( sub , str)\
    MSG_1 (MSG_SSID_DFLT, MSG_LVL_HIGH,"DS: SUB %d " str , (sub+1))

#define MSG_HIGH_DS_1( sub , str, p1)\
    MSG_2 (MSG_SSID_DFLT, MSG_LVL_HIGH,"DS: SUB %d " str , (sub+1), (p1))

#define MSG_HIGH_DS_2( sub , str, p1, p2)\
    MSG_3 (MSG_SSID_DFLT, MSG_LVL_HIGH,"DS: SUB %d " str , (sub+1), (p1), (p2))

#define MSG_HIGH_DS_3( sub , str, p1, p2, p3)\
    MSG_4 (MSG_SSID_DFLT, MSG_LVL_HIGH,"DS: SUB %d " str , (sub+1), (p1), (p2), (p3))

#define MSG_HIGH_DS_4( sub , str,p1, p2, p3, p4)\
    MSG_5 (MSG_SSID_DFLT, MSG_LVL_HIGH,"DS: SUB %d " str , (sub+1),(p1), (p2), (p3), (p4))

#define MSG_HIGH_DS_5( sub , str, p1, p2, p3, p4, p5)\
    MSG_6 (MSG_SSID_DFLT, MSG_LVL_HIGH,"DS: SUB %d " str , (sub+1), (p1), (p2), (p3), (p4), (p5))

#define MSG_HIGH_DS_6( sub , str, p1, p2, p3, p4, p5, p6)\
    MSG_7 (MSG_SSID_DFLT, MSG_LVL_HIGH,"DS: SUB %d " str , (sub+1), (p1), (p2), (p3), (p4), (p5), (p6))

#define MSG_HIGH_DS_7( sub , str, p1, p2, p3, p4, p5, p6, p7)\
    MSG_8 (MSG_SSID_DFLT, MSG_LVL_HIGH,"DS: SUB %d " str , (sub+1), (p1), (p2), (p3), (p4), (p5), (p6), (p7))

#define MSG_HIGH_DS_8( sub , str, p1, p2, p3, p4, p5, p6, p7, p8)\
    MSG_9 (MSG_SSID_DFLT, MSG_LVL_HIGH,"DS: SUB %d " str , (sub+1), (p1), (p2), (p3), (p4), (p5), (p6), (p7), (p8))
	

#define MSG_MED_DS_0( sub , str)\
    MSG_1 (MSG_SSID_DFLT, MSG_LVL_MED,"DS: SUB %d " str , (sub+1))

#define MSG_MED_DS_1( sub , str, p1)\
    MSG_2 (MSG_SSID_DFLT, MSG_LVL_MED,"DS: SUB %d " str , (sub+1), (p1))

#define MSG_MED_DS_2( sub , str, p1, p2)\
    MSG_3 (MSG_SSID_DFLT, MSG_LVL_MED,"DS: SUB %d " str , (sub+1), (p1), (p2))

#define MSG_MED_DS_3( sub , str, p1, p2, p3)\
    MSG_4 (MSG_SSID_DFLT, MSG_LVL_MED,"DS: SUB %d " str , (sub+1), (p1), (p2), (p3))

#define MSG_MED_DS_4( sub , str,p1, p2, p3, p4)\
    MSG_5 (MSG_SSID_DFLT, MSG_LVL_MED,"DS: SUB %d " str , (sub+1),(p1), (p2), (p3), (p4))

#define MSG_MED_DS_5( sub , str, p1, p2, p3, p4, p5)\
    MSG_6 (MSG_SSID_DFLT, MSG_LVL_MED,"DS: SUB %d " str , (sub+1), (p1), (p2), (p3), (p4), (p5))

#define MSG_MED_DS_6( sub , str, p1, p2, p3, p4, p5, p6)\
    MSG_7 (MSG_SSID_DFLT, MSG_LVL_MED,"DS: SUB %d " str , (sub+1), (p1), (p2), (p3), (p4), (p5), (p6))

#define MSG_MED_DS_7( sub , str, p1, p2, p3, p4, p5, p6, p7)\
    MSG_8 (MSG_SSID_DFLT, MSG_LVL_MED,"DS: SUB %d " str , (sub+1), (p1), (p2), (p3), (p4), (p5), (p6), (p7))

#define MSG_MED_DS_8( sub , str, p1, p2, p3, p4, p5, p6, p7, p8)\
    MSG_9 (MSG_SSID_DFLT, MSG_LVL_MED,"DS: SUB %d " str , (sub+1), (p1), (p2), (p3), (p4), (p5), (p6), (p7), (p8))
	
    
#define MSG_LOW_DS_0( sub , str)\
    MSG_1 (MSG_SSID_DFLT, MSG_LVL_LOW,"DS: SUB %d " str , (sub+1))

#define MSG_LOW_DS_1( sub , str, p1)\
    MSG_2 (MSG_SSID_DFLT, MSG_LVL_LOW,"DS: SUB %d " str , (sub+1), (p1))

#define MSG_LOW_DS_2( sub , str, p1, p2)\
    MSG_3 (MSG_SSID_DFLT, MSG_LVL_LOW,"DS: SUB %d " str , (sub+1), (p1), (p2))

#define MSG_LOW_DS_3( sub , str, p1, p2, p3)\
    MSG_4 (MSG_SSID_DFLT, MSG_LVL_LOW,"DS: SUB %d " str , (sub+1), (p1), (p2), (p3))

#define MSG_LOW_DS_4( sub , str,p1, p2, p3, p4)\
    MSG_5 (MSG_SSID_DFLT, MSG_LVL_LOW,"DS: SUB %d " str , (sub+1),(p1), (p2), (p3), (p4))

#define MSG_LOW_DS_5( sub , str, p1, p2, p3, p4, p5)\
    MSG_6 (MSG_SSID_DFLT, MSG_LVL_LOW,"DS: SUB %d " str , (sub+1), (p1), (p2), (p3), (p4), (p5))

#define MSG_LOW_DS_6( sub , str, p1, p2, p3, p4, p5, p6)\
    MSG_7 (MSG_SSID_DFLT, MSG_LVL_LOW,"DS: SUB %d " str , (sub+1), (p1), (p2), (p3), (p4), (p5), (p6))

#define MSG_LOW_DS_7( sub , str, p1, p2, p3, p4, p5, p6, p7)\
    MSG_8 (MSG_SSID_DFLT, MSG_LVL_LOW,"DS: SUB %d " str , (sub+1), (p1), (p2), (p3), (p4), (p5), (p6), (p7))

#define MSG_LOW_DS_8( sub , str, p1, p2, p3, p4, p5, p6, p7, p8)\
    MSG_9 (MSG_SSID_DFLT, MSG_LVL_LOW,"DS: SUB %d " str , (sub+1), (p1), (p2), (p3), (p4), (p5), (p6), (p7), (p8))

#define MSG_ERROR_DS_0( sub , str)\
    MSG_1 (MSG_SSID_DFLT, MSG_LVL_ERROR,"DS: SUB %d " str , (sub+1))

#define MSG_ERROR_DS_1( sub , str, p1)\
    MSG_2 (MSG_SSID_DFLT, MSG_LVL_ERROR,"DS: SUB %d " str , (sub+1), (p1))

#define MSG_ERROR_DS_2( sub , str, p1, p2)\
    MSG_3 (MSG_SSID_DFLT, MSG_LVL_ERROR,"DS: SUB %d " str , (sub+1), (p1), (p2))

#define MSG_ERROR_DS_3( sub , str, p1, p2, p3)\
    MSG_4 (MSG_SSID_DFLT, MSG_LVL_ERROR,"DS: SUB %d " str , (sub+1), (p1), (p2), (p3))

#define MSG_ERROR_DS_4( sub , str,p1, p2, p3, p4)\
    MSG_5 (MSG_SSID_DFLT, MSG_LVL_ERROR,"DS: SUB %d " str , (sub+1),(p1), (p2), (p3), (p4))

#define MSG_ERROR_DS_5( sub , str, p1, p2, p3, p4, p5)\
    MSG_6 (MSG_SSID_DFLT, MSG_LVL_ERROR,"DS: SUB %d " str , (sub+1), (p1), (p2), (p3), (p4), (p5))

#define MSG_ERROR_DS_6( sub , str, p1, p2, p3, p4, p5, p6)\
    MSG_7 (MSG_SSID_DFLT, MSG_LVL_ERROR,"DS: SUB %d " str , (sub+1), (p1), (p2), (p3), (p4), (p5), (p6))

#define MSG_ERROR_DS_7( sub , str, p1, p2, p3, p4, p5, p6, p7)\
    MSG_8 (MSG_SSID_DFLT, MSG_LVL_ERROR,"DS: SUB %d " str , (sub+1), (p1), (p2), (p3), (p4), (p5), (p6), (p7))

#define MSG_ERROR_DS_8( sub , str, p1, p2, p3, p4, p5, p6, p7, p8)\
    MSG_9 (MSG_SSID_DFLT, MSG_LVL_ERROR,"DS: SUB %d " str , (sub+1), (p1), (p2), (p3), (p4), (p5), (p6), (p7), (p8))
/*
#define MSG_FATAL_DS( sub , str, p1, p2, p3)\
    ERR_FATAL ("DS: SUB %d " str , (sub+1), (p1), (p2))
*/
#else  /* FEATURE_DUAL_SIM */
	
#define MSG_HIGH_DS_0( sub , str)\
    MSG (MSG_SSID_DFLT, MSG_LVL_HIGH, str )

#define MSG_HIGH_DS_1( sub , str, p1)\
    MSG_1 (MSG_SSID_DFLT, MSG_LVL_HIGH, str, (p1))

#define MSG_HIGH_DS_2( sub , str, p1, p2)\
    MSG_2 (MSG_SSID_DFLT, MSG_LVL_HIGH, str, (p1), (p2))

#define MSG_HIGH_DS_3( sub , str, p1, p2, p3)\
    MSG_3 (MSG_SSID_DFLT, MSG_LVL_HIGH, str, (p1), (p2), (p3))

#define MSG_HIGH_DS_4( sub , str,p1, p2, p3, p4)\
    MSG_4 (MSG_SSID_DFLT, MSG_LVL_HIGH, str,(p1), (p2), (p3), (p4))

#define MSG_HIGH_DS_5( sub , str, p1, p2, p3, p4, p5)\
    MSG_5 (MSG_SSID_DFLT, MSG_LVL_HIGH, str, (p1), (p2), (p3), (p4), (p5))

#define MSG_HIGH_DS_6( sub , str, p1, p2, p3, p4, p5, p6)\
    MSG_6 (MSG_SSID_DFLT, MSG_LVL_HIGH, str, (p1), (p2), (p3), (p4), (p5), (p6))

#define MSG_HIGH_DS_7( sub , str, p1, p2, p3, p4, p5, p6, p7)\
    MSG_7 (MSG_SSID_DFLT, MSG_LVL_HIGH, str, (p1), (p2), (p3), (p4), (p5), (p6), (p7))

#define MSG_HIGH_DS_8( sub , str, p1, p2, p3, p4, p5, p6, p7, p8)\
    MSG_8 (MSG_SSID_DFLT, MSG_LVL_HIGH, str, (p1), (p2), (p3), (p4), (p5), (p6), (p7), (p8))
	
#define MSG_MED_DS_0( sub , str)\
    MSG (MSG_SSID_DFLT, MSG_LVL_MED, str )

#define MSG_MED_DS_1( sub , str, p1)\
    MSG_1 (MSG_SSID_DFLT, MSG_LVL_MED, str, (p1))

#define MSG_MED_DS_2( sub , str, p1, p2)\
    MSG_2 (MSG_SSID_DFLT, MSG_LVL_MED, str, (p1), (p2))

#define MSG_MED_DS_3( sub , str, p1, p2, p3)\
    MSG_3 (MSG_SSID_DFLT, MSG_LVL_MED, str, (p1), (p2), (p3))

#define MSG_MED_DS_4( sub , str,p1, p2, p3, p4)\
    MSG_4 (MSG_SSID_DFLT, MSG_LVL_MED, str,(p1), (p2), (p3), (p4))

#define MSG_MED_DS_5( sub , str, p1, p2, p3, p4, p5)\
    MSG_5 (MSG_SSID_DFLT, MSG_LVL_MED, str, (p1), (p2), (p3), (p4), (p5))

#define MSG_MED_DS_6( sub , str, p1, p2, p3, p4, p5, p6)\
    MSG_6 (MSG_SSID_DFLT, MSG_LVL_MED, str, (p1), (p2), (p3), (p4), (p5), (p6))

#define MSG_MED_DS_7( sub , str, p1, p2, p3, p4, p5, p6, p7)\
    MSG_7 (MSG_SSID_DFLT, MSG_LVL_MED, str, (p1), (p2), (p3), (p4), (p5), (p6), (p7))

#define MSG_MED_DS_8( sub , str, p1, p2, p3, p4, p5, p6, p7, p8)\
    MSG_8 (MSG_SSID_DFLT, MSG_LVL_MED, str, (p1), (p2), (p3), (p4), (p5), (p6), (p7), (p8))
    
#define MSG_LOW_DS_0( sub , str)\
    MSG (MSG_SSID_DFLT, MSG_LVL_LOW, str )

#define MSG_LOW_DS_1( sub , str, p1)\
    MSG_1 (MSG_SSID_DFLT, MSG_LVL_LOW, str, (p1))

#define MSG_LOW_DS_2( sub , str, p1, p2)\
    MSG_2 (MSG_SSID_DFLT, MSG_LVL_LOW, str, (p1), (p2))

#define MSG_LOW_DS_3( sub , str, p1, p2, p3)\
    MSG_3 (MSG_SSID_DFLT, MSG_LVL_LOW, str, (p1), (p2), (p3))

#define MSG_LOW_DS_4( sub , str,p1, p2, p3, p4)\
    MSG_4 (MSG_SSID_DFLT, MSG_LVL_LOW, str,(p1), (p2), (p3), (p4))

#define MSG_LOW_DS_5( sub , str, p1, p2, p3, p4, p5)\
    MSG_5 (MSG_SSID_DFLT, MSG_LVL_LOW, str, (p1), (p2), (p3), (p4), (p5))

#define MSG_LOW_DS_6( sub , str, p1, p2, p3, p4, p5, p6)\
    MSG_6 (MSG_SSID_DFLT, MSG_LVL_LOW, str, (p1), (p2), (p3), (p4), (p5), (p6))

#define MSG_LOW_DS_7( sub , str, p1, p2, p3, p4, p5, p6, p7)\
    MSG_7 (MSG_SSID_DFLT, MSG_LVL_LOW, str, (p1), (p2), (p3), (p4), (p5), (p6), (p7))

#define MSG_LOW_DS_8( sub , str, p1, p2, p3, p4, p5, p6, p7, p8)\
    MSG_8 (MSG_SSID_DFLT, MSG_LVL_LOW, str, (p1), (p2), (p3), (p4), (p5), (p6), (p7), (p8))
    
#define MSG_ERROR_DS_0( sub , str)\
    MSG (MSG_SSID_DFLT, MSG_LVL_ERROR, str )

#define MSG_ERROR_DS_1( sub , str, p1)\
    MSG_1 (MSG_SSID_DFLT, MSG_LVL_ERROR, str, (p1))

#define MSG_ERROR_DS_2( sub , str, p1, p2)\
    MSG_2 (MSG_SSID_DFLT, MSG_LVL_ERROR, str, (p1), (p2))

#define MSG_ERROR_DS_3( sub , str, p1, p2, p3)\
    MSG_3 (MSG_SSID_DFLT, MSG_LVL_ERROR, str, (p1), (p2), (p3))

#define MSG_ERROR_DS_4( sub , str,p1, p2, p3, p4)\
    MSG_4 (MSG_SSID_DFLT, MSG_LVL_ERROR, str,(p1), (p2), (p3), (p4))

#define MSG_ERROR_DS_5( sub , str, p1, p2, p3, p4, p5)\
    MSG_5 (MSG_SSID_DFLT, MSG_LVL_ERROR, str, (p1), (p2), (p3), (p4), (p5))

#define MSG_ERROR_DS_6( sub , str, p1, p2, p3, p4, p5, p6)\
    MSG_6 (MSG_SSID_DFLT, MSG_LVL_ERROR, str, (p1), (p2), (p3), (p4), (p5), (p6))

#define MSG_ERROR_DS_7( sub , str, p1, p2, p3, p4, p5, p6, p7)\
    MSG_7 (MSG_SSID_DFLT, MSG_LVL_ERROR, str, (p1), (p2), (p3), (p4), (p5), (p6), (p7))

#define MSG_ERROR_DS_8( sub , str, p1, p2, p3, p4, p5, p6, p7, p8)\
    MSG_8 (MSG_SSID_DFLT, MSG_LVL_ERROR, str, (p1), (p2), (p3), (p4), (p5), (p6), (p7), (p8))
/*
#define MSG_FATAL_DS( sub , str, p1, p2, p3)\
    ERR_FATAL (str , (p1), (p2), (p3))
*/

#endif /* FEATURE_DUAL_SIM */

/* Macro deficitions for teh modules which are Independent of Dual Subscription, like NAS services*/
#define MSG_HIGH_0( str)\
    MSG (MSG_SSID_DFLT, MSG_LVL_HIGH, str )

#define MSG_HIGH_1( str, p1)\
    MSG_1 (MSG_SSID_DFLT, MSG_LVL_HIGH, str, (p1))

#define MSG_HIGH_2( str, p1, p2)\
    MSG_2 (MSG_SSID_DFLT, MSG_LVL_HIGH, str, (p1), (p2))

#define MSG_HIGH_3( str, p1, p2, p3)\
    MSG_3 (MSG_SSID_DFLT, MSG_LVL_HIGH, str, (p1), (p2), (p3))

#define MSG_HIGH_4( str,p1, p2, p3, p4)\
    MSG_4 (MSG_SSID_DFLT, MSG_LVL_HIGH, str,(p1), (p2), (p3), (p4))

#define MSG_HIGH_5( str, p1, p2, p3, p4, p5)\
    MSG_5 (MSG_SSID_DFLT, MSG_LVL_HIGH, str, (p1), (p2), (p3), (p4), (p5))

#define MSG_HIGH_6( str, p1, p2, p3, p4, p5, p6)\
    MSG_6 (MSG_SSID_DFLT, MSG_LVL_HIGH, str, (p1), (p2), (p3), (p4), (p5), (p6))

#define MSG_HIGH_7( str, p1, p2, p3, p4, p5, p6, p7)\
    MSG_7 (MSG_SSID_DFLT, MSG_LVL_HIGH, str, (p1), (p2), (p3), (p4), (p5), (p6), (p7))

#define MSG_HIGH_8( str, p1, p2, p3, p4, p5, p6, p7, p8)\
    MSG_8 (MSG_SSID_DFLT, MSG_LVL_HIGH, str, (p1), (p2), (p3), (p4), (p5), (p6), (p7), (p8))
	
#define MSG_MED_0( str)\
    MSG (MSG_SSID_DFLT, MSG_LVL_MED, str )

#define MSG_MED_1( str, p1)\
    MSG_1 (MSG_SSID_DFLT, MSG_LVL_MED, str, (p1))

#define MSG_MED_2( str, p1, p2)\
    MSG_2 (MSG_SSID_DFLT, MSG_LVL_MED, str, (p1), (p2))

#define MSG_MED_3( str, p1, p2, p3)\
    MSG_3 (MSG_SSID_DFLT, MSG_LVL_MED, str, (p1), (p2), (p3))

#define MSG_MED_4( str,p1, p2, p3, p4)\
    MSG_4 (MSG_SSID_DFLT, MSG_LVL_MED, str,(p1), (p2), (p3), (p4))

#define MSG_MED_5( str, p1, p2, p3, p4, p5)\
    MSG_5 (MSG_SSID_DFLT, MSG_LVL_MED, str, (p1), (p2), (p3), (p4), (p5))

#define MSG_MED_6( str, p1, p2, p3, p4, p5, p6)\
    MSG_6 (MSG_SSID_DFLT, MSG_LVL_MED, str, (p1), (p2), (p3), (p4), (p5), (p6))

#define MSG_MED_7( str, p1, p2, p3, p4, p5, p6, p7)\
    MSG_7 (MSG_SSID_DFLT, MSG_LVL_MED, str, (p1), (p2), (p3), (p4), (p5), (p6), (p7))

#define MSG_MED_8( str, p1, p2, p3, p4, p5, p6, p7, p8)\
    MSG_8 (MSG_SSID_DFLT, MSG_LVL_MED, str, (p1), (p2), (p3), (p4), (p5), (p6), (p7), (p8))
    
#define MSG_LOW_0( str)\
    MSG (MSG_SSID_DFLT, MSG_LVL_LOW, str )

#define MSG_LOW_1( str, p1)\
    MSG_1 (MSG_SSID_DFLT, MSG_LVL_LOW, str, (p1))

#define MSG_LOW_2( str, p1, p2)\
    MSG_2 (MSG_SSID_DFLT, MSG_LVL_LOW, str, (p1), (p2))

#define MSG_LOW_3( str, p1, p2, p3)\
    MSG_3 (MSG_SSID_DFLT, MSG_LVL_LOW, str, (p1), (p2), (p3))

#define MSG_LOW_4( str,p1, p2, p3, p4)\
    MSG_4 (MSG_SSID_DFLT, MSG_LVL_LOW, str,(p1), (p2), (p3), (p4))

#define MSG_LOW_5( str, p1, p2, p3, p4, p5)\
    MSG_5 (MSG_SSID_DFLT, MSG_LVL_LOW, str, (p1), (p2), (p3), (p4), (p5))

#define MSG_LOW_6( str, p1, p2, p3, p4, p5, p6)\
    MSG_6 (MSG_SSID_DFLT, MSG_LVL_LOW, str, (p1), (p2), (p3), (p4), (p5), (p6))

#define MSG_LOW_7( str, p1, p2, p3, p4, p5, p6, p7)\
    MSG_7 (MSG_SSID_DFLT, MSG_LVL_LOW, str, (p1), (p2), (p3), (p4), (p5), (p6), (p7))

#define MSG_LOW_8( str, p1, p2, p3, p4, p5, p6, p7, p8)\
    MSG_8 (MSG_SSID_DFLT, MSG_LVL_LOW, str, (p1), (p2), (p3), (p4), (p5), (p6), (p7), (p8))
    
#define MSG_ERROR_0( str)\
    MSG (MSG_SSID_DFLT, MSG_LVL_ERROR, str )

#define MSG_ERROR_1( str, p1)\
    MSG_1 (MSG_SSID_DFLT, MSG_LVL_ERROR, str, (p1))

#define MSG_ERROR_2( str, p1, p2)\
    MSG_2 (MSG_SSID_DFLT, MSG_LVL_ERROR, str, (p1), (p2))

#define MSG_ERROR_3( str, p1, p2, p3)\
    MSG_3 (MSG_SSID_DFLT, MSG_LVL_ERROR, str, (p1), (p2), (p3))

#define MSG_ERROR_4( str,p1, p2, p3, p4)\
    MSG_4 (MSG_SSID_DFLT, MSG_LVL_ERROR, str,(p1), (p2), (p3), (p4))

#define MSG_ERROR_5( str, p1, p2, p3, p4, p5)\
    MSG_5 (MSG_SSID_DFLT, MSG_LVL_ERROR, str, (p1), (p2), (p3), (p4), (p5))

#define MSG_ERROR_6( str, p1, p2, p3, p4, p5, p6)\
    MSG_6 (MSG_SSID_DFLT, MSG_LVL_ERROR, str, (p1), (p2), (p3), (p4), (p5), (p6))

#define MSG_ERROR_7( str, p1, p2, p3, p4, p5, p6, p7)\
    MSG_7 (MSG_SSID_DFLT, MSG_LVL_ERROR, str, (p1), (p2), (p3), (p4), (p5), (p6), (p7))

#define MSG_ERROR_8( str, p1, p2, p3, p4, p5, p6, p7, p8)\
    MSG_8 (MSG_SSID_DFLT, MSG_LVL_ERROR, str, (p1), (p2), (p3), (p4), (p5), (p6), (p7), (p8))
	
#endif /* NAS_UTILS_H */