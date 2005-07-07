/*
 * 
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
import java.util.Collection;

import javax.ejb.CreateException;
import javax.ejb.EJBHome;
import javax.ejb.FinderException;


public interface RefConfigurationSetHome extends EJBHome {

    public RefConfigurationSet create ( String name, Integer organizationID )
        throws CreateException, RemoteException;

    public RefConfigurationSet findByPrimaryKey ( Integer pk )
        throws FinderException, RemoteException;

    public Collection findByNameAndOrganization (   String name,
                                                    Integer organizationID )
        throws FinderException, RemoteException;

    public Collection findByName ( String name )
        throws FinderException, RemoteException;

    public Collection findByOrganizationID ( Integer organizationID )
        throws FinderException, RemoteException;


}
