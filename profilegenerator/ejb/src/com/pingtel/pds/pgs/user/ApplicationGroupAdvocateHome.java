/*
 * $Id: //depot/OPENDEV/sipXconfig/profilegenerator/ejb/src/com/pingtel/pds/pgs/user/ApplicationGroupAdvocateHome.java#4 $
 * 
 * Copyright (C) 2004 SIPfoundry Inc.
 * Licensed by SIPfoundry under the LGPL license.
 * 
 * Copyright (C) 2004 Pingtel Corp.
 * Licensed to SIPfoundry under a Contributor Agreement.
 * 
 * $$
 */



package com.pingtel.pds.pgs.user;

import java.rmi.RemoteException;

import javax.ejb.CreateException;
import javax.ejb.EJBHome;

public interface ApplicationGroupAdvocateHome extends EJBHome {

    public ApplicationGroupAdvocate create ()
        throws CreateException, RemoteException;
}