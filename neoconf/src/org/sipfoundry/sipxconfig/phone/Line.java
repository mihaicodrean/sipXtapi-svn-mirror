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

import org.sipfoundry.sipxconfig.setting.SettingGroup;
import org.sipfoundry.sipxconfig.setting.ValueStorage;

/**
 * Association between Users and their assigned phones.
 */
public class Line implements Serializable {

    private static final long serialVersionUID = 1L;

    private int m_id = PhoneContext.UNSAVED_ID;

    private User m_user;

    private ValueStorage m_valueStorage;
    
    private Endpoint m_endpoint;

    private int m_position;

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

    public Endpoint getEndpoint() {
        return m_endpoint;
    }

    public void setEndpoint(Endpoint endpoint) {
        m_endpoint = endpoint;
    }

    public int getPosition() {
        return m_position;
    }

    public void setPosition(int position) {
        m_position = position;
    }

    public SettingGroup getSettings(Phone phone) {
        SettingGroup model = phone.getSettingModel(this);
        if (m_valueStorage == null) {
            m_valueStorage = new ValueStorage();
        }
        
        return (SettingGroup) model.getCopy(getValueStorage());
    }
}