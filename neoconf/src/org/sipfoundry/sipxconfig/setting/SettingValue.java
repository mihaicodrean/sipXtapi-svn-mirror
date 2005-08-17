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
package org.sipfoundry.sipxconfig.setting;

/**
 * Proxy setting class thru a decorator that manages values in the ValueStorage class.
 */
public class SettingValue extends SettingDecorator {

    private Storage m_storage;

    public SettingValue(Storage storage, Setting delegate) {
        super(delegate);
        m_storage = storage;
    }

    public String getDefaultValue() {
        return getDelegate().getValue();
    }

    public void setValue(String value) {
        String defValue = getDefaultValue();
        if (value == null) {
            if (defValue == null) {
                m_storage.remove(getDelegate());
            } else {
                m_storage.setValue(getDelegate(), Setting.NULL_VALUE);
            }
        } else {
            if (value.equals(defValue)) {
                m_storage.remove(getDelegate());
            } else {
                m_storage.setValue(getDelegate(), value);
            }
        }
    }

    public String getValue() {
        String value = (String) m_storage.getValue(getDelegate());
        return value == null ? getDefaultValue() : value;
    }    
}
