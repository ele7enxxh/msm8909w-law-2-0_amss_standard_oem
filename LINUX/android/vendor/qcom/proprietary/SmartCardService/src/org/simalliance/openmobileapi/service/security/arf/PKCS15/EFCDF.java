/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */
/*
    Copyright (c) 2014, The Linux Foundation. All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:
        * Redistributions of source code must retain the above copyright
          notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above
          copyright notice, this list of conditions and the following
          disclaimer in the documentation and/or other materials provided
          with the distribution.
        * Neither the name of The Linux Foundation nor the names of its
          contributors may be used to endorse or promote products derived
          from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
   ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

package org.simalliance.openmobileapi.service.security.arf.PKCS15;

import org.simalliance.openmobileapi.service.security.arf.DERParser;
import org.simalliance.openmobileapi.service.security.arf.SecureElement;
import org.simalliance.openmobileapi.service.security.arf.SecureElementException;
import org.simalliance.openmobileapi.internal.Util;
import org.simalliance.openmobileapi.service.security.arf.PKCS15.PKCS15Exception;

import android.util.Log;
import java.util.ArrayList;
import java.util.Arrays;

/**
 * EF_CDF related features
 ***************************************************/
public class EFCDF extends EF {

    public static final String TAG = "SmartcardService ACE ARF";
    // Standardized ID for EF_CDF file
    public static final byte[] EFCDF_PATH = { 0x50,0x03 };
    public ArrayList<byte[]> x509Bytes = null;
    private short DerIndex, DerSize  = 0;

    /**
     * Selects and Analyses EF_CDF file
     * stores the Path to "EF_TCF" from "TCF Tag" entry (T=04 L=02)
     * stores the start byte of the certificates (coded on 1 or 2 bytes)
     * stores the length of the certificate (coded on two bytes)
     */

    public  ArrayList<byte[]> isx509(byte[] buffer) throws PKCS15Exception {

        DerSize = (short)buffer.length;
        x509Bytes = new ArrayList<byte[]>();
        byte[] start =new byte[2];
        byte[] size = new byte[2];
        byte[] mTCFpath = new byte[2];
        DERParser DER=new DERParser(buffer);

        if (DerIndex==DerSize) return null;

        while(++DerIndex<DerSize) {
            // Parsing TLVs to find the TCF Path (4002)
            if ( (buffer[DerIndex] == (byte)0x04) && (buffer[DerIndex+1] == (byte)0x02) ) {
                mTCFpath = new byte[]{buffer[DerIndex+2],  buffer[DerIndex+3]};
                // buffer[DerIndex+5] represents the length of start byte (1 or 2 bytes)
                // the start byte is coded on buffer[DerIndex+6] or on buffer[DerIndex+6] & buffer[DerIndex+7]
                // the length is coded on buffer[DerIndex+9] & buffer[DerIndex+10]
                // or on buffer[DerIndex+10] & buffer[DerIndex+11] depending on the start byte length
                if ( buffer[DerIndex+5] == 0x01) {
                    start = new byte[]{ 0x00, buffer[DerIndex+6] };
                    size =  new byte[]{ buffer[DerIndex+9],  buffer[DerIndex+10] };
                } else if (buffer[DerIndex+5] == 0x02) {
                    start = new byte[]{ buffer[DerIndex+6], buffer[DerIndex+7] };
                    size =  new byte[]{ buffer[DerIndex+10],  buffer[DerIndex+11] };
                }
                byte[] Certbuff = new byte[6];
                System.arraycopy(start,0,Certbuff,0,start.length);
                System.arraycopy(size,0,Certbuff,start.length,size.length);
                System.arraycopy(mTCFpath,0,Certbuff,start.length + size.length,mTCFpath.length);
                x509Bytes.add(Certbuff);
            }
        }
        return x509Bytes;
    }

    public ArrayList<byte[]> returnCouples (){
        return x509Bytes;
    }

    /**
     * Constructor
     * @param secureElement SE on which ISO7816 commands are applied
     */
    public EFCDF(SecureElement handle) {
        super(handle);
    }

    public ArrayList<byte[]> checkCDF(byte[] cdfPath)  throws PKCS15Exception,SecureElementException {
        Log.i(TAG,"Analysing EF_CDF...");
        byte[] path = null;
        if (cdfPath != null) {
            path = cdfPath;
        } else {
             path = EFCDF_PATH;
        }

        if ( selectFile(path)!= APDU_SUCCESS) {
            Log.i(TAG,"EF_CDF not found!!");
            return null;
        } else {
            return isx509(readBinary(0,Util.END));
        }
    }

}
