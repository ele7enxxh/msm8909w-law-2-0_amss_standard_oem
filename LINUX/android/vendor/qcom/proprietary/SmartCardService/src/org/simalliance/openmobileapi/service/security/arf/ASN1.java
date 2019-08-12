/*
 * Copyright (C) 2011 Deutsche Telekom, A.G.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.simalliance.openmobileapi.service.security.arf;

/**
 * Defines all tags for parsing PKCS#15 files
 ***************************************************/
public abstract class ASN1 {

    // ASN.1 tags
    public static final byte TAG_Sequence   = 0x30;
    public static final byte TAG_OctetString = 0x04;
    public static final byte TAG_OID            = 0x06;

    // EF_DIR tags
    public static final byte TAG_ApplTemplate = 0x61;
    public static final byte TAG_ApplIdentifier  = 0x4F;
    public static final byte TAG_ApplLabel       = 0x50;
    public static final byte TAG_ApplPath        = 0x51;
    public static final byte TAG_FCP               = 0x62;

    // Others tags
    public static final byte TAG_Padding         = (byte)0xFF;
}