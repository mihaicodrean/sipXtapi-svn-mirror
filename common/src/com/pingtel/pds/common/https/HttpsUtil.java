/*
 * $Id: //depot/OPENDEV/sipXconfig/common/src/com/pingtel/pds/common/https/HttpsUtil.java#4 $
 * 
 * Copyright (C) 2004 SIPfoundry Inc.
 * Licensed by SIPfoundry under the LGPL license.
 * 
 * Copyright (C) 2004 Pingtel Corp.
 * Licensed to SIPfoundry under a Contributor Agreement.
 * 
 * $$
 */


package com.pingtel.pds.common.https;

import javax.net.ssl.HttpsURLConnection;

/**
 * Utility class to open up SSL comms. to the other SIPxchange servers.
 *
 * @author ibutcher
 * 
 */
public class HttpsUtil {

//////////////////////////////////////////////////////////////////////////
// Constants
////


//////////////////////////////////////////////////////////////////////////
// Attributes
////
    static WideOpenHostnameVerifier mWideOpen = new WideOpenHostnameVerifier();


//////////////////////////////////////////////////////////////////////////
// Construction
////


//////////////////////////////////////////////////////////////////////////
// Public Methods
////
    public static void init(){
        HttpsURLConnection.setDefaultHostnameVerifier(mWideOpen);
    }


//////////////////////////////////////////////////////////////////////////
// Implementation Methods
////


//////////////////////////////////////////////////////////////////////////
// Nested / Inner classes
////    


//////////////////////////////////////////////////////////////////////////
// Native Method Declarations
////

}
