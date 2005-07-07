/*
 * 
 * 
 * Copyright (C) 2005 SIPfoundry Inc.
 * Licensed by SIPfoundry under the LGPL license.
 * 
 * Copyright (C) 2005 Pingtel Corp.
 * Licensed to SIPfoundry under a Contributor Agreement.
 * 
 * $
 */
package org.sipfoundry.sipxconfig.admin.dialplan;

import java.util.ArrayList;
import java.util.Collection;

/**
 * EmergencyRouting
 */
public class EmergencyRouting {
    private Gateway m_defaultGateway;
    private String m_externalNumber;

    private Collection m_exceptions = new ArrayList();

    public void addException(RoutingException exception) {
        m_exceptions.add(exception);
    }

    public void removeException(RoutingException exception) {
        m_exceptions.remove(exception);
    }

    public void removeException(Integer exceptionId) {
        m_exceptions.remove(new BeanWithId(exceptionId));
    }

    // getters and setters
    public Gateway getDefaultGateway() {
        return m_defaultGateway;
    }

    public void setDefaultGateway(Gateway defaultGateway) {
        m_defaultGateway = defaultGateway;
    }

    public Collection getExceptions() {
        return m_exceptions;
    }

    public void setExceptions(Collection exceptions) {
        m_exceptions = exceptions;
    }

    public String getExternalNumber() {
        return m_externalNumber;
    }

    public void setExternalNumber(String externalNumber) {
        m_externalNumber = externalNumber;
    }
}
