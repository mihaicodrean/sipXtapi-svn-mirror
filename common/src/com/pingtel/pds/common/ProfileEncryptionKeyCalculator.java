/*
 * $Id: //depot/OPENDEV/sipXconfig/common/src/com/pingtel/pds/common/ProfileEncryptionKeyCalculator.java#4 $
 * 
 * Copyright (C) 2004 SIPfoundry Inc.
 * Licensed by SIPfoundry under the LGPL license.
 * 
 * Copyright (C) 2004 Pingtel Corp.
 * Licensed to SIPfoundry under a Contributor Agreement.
 * 
 * $$
 */

package com.pingtel.pds.common;


/**
 * ProfileEncryptionKeyCalculator is a helper class use to calculate the
 * digested value that is used as the key to encrypt Users profiles.
 *
 * @author butcheri
 */
public class ProfileEncryptionKeyCalculator {

//////////////////////////////////////////////////////////////////////////
// Constants
////


//////////////////////////////////////////////////////////////////////////
// Attributes
////
    private static MD5Encoder mMd5Encoder = new MD5Encoder();


//////////////////////////////////////////////////////////////////////////
// Construction
////


//////////////////////////////////////////////////////////////////////////
// Public Methods
////
    public static String calculateProfileEncryptionKey(    String displayID,
                                                    String password) {
        return mMd5Encoder.encode(displayID + ":" + password);
    }


//////////////////////////////////////////////////////////////////////////
// Nested / Inner classes
////


//////////////////////////////////////////////////////////////////////////
// Native Method Declarations
////

}
