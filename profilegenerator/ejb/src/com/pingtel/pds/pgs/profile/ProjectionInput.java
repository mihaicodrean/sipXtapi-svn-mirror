/*
 * $Id: //depot/OPENDEV/sipXconfig/profilegenerator/ejb/src/com/pingtel/pds/pgs/profile/ProjectionInput.java#4 $
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

import java.util.Collection;

import org.jdom.Document;

public class ProjectionInput {

    public ProjectionInput ( Document properties, Collection finalRules ) {
        this.m_properties = properties;
        this.m_finalRules = finalRules;
    }

    public Document getDocument () {
        return this.m_properties;
    }

    public Collection getFinalRules () {
        return this.m_finalRules;
    }

    private Document m_properties;
    private Collection m_finalRules;
}