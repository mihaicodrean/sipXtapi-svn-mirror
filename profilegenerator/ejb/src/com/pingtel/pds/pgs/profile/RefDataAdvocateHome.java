/*
 * $Id: //depot/OPENDEV/sipXconfig/profilegenerator/ejb/src/com/pingtel/pds/pgs/profile/RefDataAdvocateHome.java#4 $
 * 
 * Copyright (C) 2004 SIPfoundry Inc.
 * Licensed by SIPfoundry under the LGPL license.
 * 
 * Copyright (C) 2004 Pingtel Corp.
 * Licensed to SIPfoundry under a Contributor Agreement.
 * 
 * $$
 */

package com.pingtel.pds.pgs.profile;

import java.rmi.RemoteException;

import javax.ejb.CreateException;
import javax.ejb.EJBHome;

/**
 *  Description of the Interface
 *
 *@author     ibutcher
 *@created    December 13, 2001
 */
public interface RefDataAdvocateHome extends EJBHome {

    /**
     *  Description of the Method
     *
     *@return                      Description of the Return Value
     *@exception  CreateException  Description of the Exception
     *@exception  RemoteException  Description of the Exception
     */
    public RefDataAdvocate create()
             throws CreateException, RemoteException;
}
