/*
 * $Id: //depot/OPENDEV/sipXconfig/profilegenerator/ejb/src/com/pingtel/pds/pgs/phone/DeviceTypeAdvocate.java#4 $
 * 
 * Copyright (C) 2004 SIPfoundry Inc.
 * Licensed by SIPfoundry under the LGPL license.
 * 
 * Copyright (C) 2004 Pingtel Corp.
 * Licensed to SIPfoundry under a Contributor Agreement.
 * 
 * $$
 */


package com.pingtel.pds.pgs.phone;

import javax.ejb.EJBObject;

/**
 *  Session Bean Facade for PhoneTag entitiy bean. This bean will be used to
 *  assign and unassign phones. This is the Remote Interface
 *
 *@author     ibutcher
 *@created    December 13, 2001
 */
public interface DeviceTypeAdvocate extends EJBObject, DeviceTypeAdvocateBusiness {}