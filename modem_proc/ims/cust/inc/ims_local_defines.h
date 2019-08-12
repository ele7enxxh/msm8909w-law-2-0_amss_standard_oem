#ifndef IMS_LOCAL_DEFINES_H
#define IMS_LOCAL_DEFINES_H

/* includes for local cust files
 * Note - these files are only visible to ims.  Any new FEATURE used by multiple SUs must be 
 * provisioned in the FEATURE database, after which it will be included in the <su>_variation.h 
 * file generated for each SUs that uses the FEATURE. */
#include "custims.h"

#include "custpsmedia.h"
	
#ifdef FEATURE_IMS_RTP_JITTERBUFFER
	#include "custqdj.h"
#endif

#endif /* IMS_LOCAL_DEFINES_H */
