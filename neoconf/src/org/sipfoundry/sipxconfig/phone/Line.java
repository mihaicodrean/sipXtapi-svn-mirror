/*
 * 
 * 
 * Copyright (C) 2004 SIPfoundry Inc.
 * Licensed by SIPfoundry under the LGPL license.
 * 
 * Copyright (C) 2004 Pingtel Corp.
 * Licensed to SIPfoundry under a Contributor Agreement.
 * 
 * $
 */
package org.sipfoundry.sipxconfig.phone;

import java.io.Serializable;

import org.sipfoundry.sipxconfig.setting.ValueStorage;

/**
 * Association between Users and their assigned phones.
 */
public class Line implements Serializable {

    private static final long serialVersionUID = 1L;

    private int m_id = PhoneContext.UNSAVED_ID;

    /** null ok */
    private User m_user;

    private ValueStorage m_valueStorage;

    private Credential m_credential;

    public Credential getCredential() {
        return m_credential;
    }

    public void setCredential(Credential credential) {
        m_credential = credential;
    }

    public int getId() {
        return m_id;
    }

    public void setId(int id) {
        m_id = id;
    }

    public User getUser() {
        return m_user;
    }

    public void setUser(User user) {
        m_user = user;
    }

    public ValueStorage getValueStorage() {
        return m_valueStorage;
    }
    
    public void setValueStorage(ValueStorage valueStorage) {
        m_valueStorage = valueStorage;
    }
    
    public String getDisplayLabel() {
        return m_user.getDisplayId();
    }
}
