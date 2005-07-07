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
package org.sipfoundry.sipxconfig.phone.polycom;

import org.apache.commons.lang.StringUtils;
import org.sipfoundry.sipxconfig.common.User;
import org.sipfoundry.sipxconfig.phone.AbstractLine;
import org.sipfoundry.sipxconfig.setting.RenderProperties;
import org.sipfoundry.sipxconfig.setting.Setting;
import org.sipfoundry.sipxconfig.setting.SettingRenderer;

/**
 * Polycom business functions for line meta setting
 */
public class PolycomLine extends AbstractLine implements SettingRenderer {

    public static final String FACTORY_ID = "polycomLine";

    public static final String REGISTRATION = "reg";

    public static final String SERVER = "server";

    public static final String ADDRESS = "address";

    public static final String PORT = "port";

    public static final String DISPLAY_NAME = "displayName";

    public static final String FIRST = "1";

    private static final int DEFAULT_SIP_PORT = 5060;
    
    /** while building model set root so getters/setting operation on this setting set*/ 
    private Setting m_root;

    public PolycomLine() {
        setModelFilename(PolycomPhone.FACTORY_ID + "/line.xml");
    }
    
    public void setDefaults(Setting settings) {

        // HACK : temporarily set root setting to trick utility methods to
        // operate on this setting set.
        m_root = settings;
        try {
            User u = getLineData().getUser();
            if (u != null) {
                setUserId(u.getDisplayId());
                setDisplayName(u.getDisplayName());
                getRegistration().getSetting("auth.userId").setValue(u.getDisplayId());
                            
                String password = getPhoneContext().getClearTextPassword(u);
                getRegistration().getSetting("auth.password").setValue(password);
    
                // only when there's a user to register do you set the registration server
                // although probably harmless            
                String domainName = getPhoneContext().getDnsDomain();
                setPrimaryRegistrationServerAddress(domainName);
            }
    
            // See pg. 125 Admin Guide/16 June 2004
            if (getLineData().getPosition() == 0) {
                settings.getSetting("msg.mwi").getSetting("callBackMode").setValue("registration");
            }
        } finally {        
            m_root = null;
        }
    }
    
    private Setting getRoot() {
        return m_root != null ? m_root : getSettings();
    }

    public String getPrimaryRegistrationServerAddress() {
        return getPrimaryRegistrationServer().getSetting(ADDRESS).getValue();
    }

    public void setPrimaryRegistrationServerAddress(String address) {
        getPrimaryRegistrationServer().getSetting(ADDRESS).setValue(address);
    }

    public String getPrimaryRegistrationServerPort() {
        return getPrimaryRegistrationServer().getSetting(PORT).getValue();
    }

    public void setPrimaryRegistrationServerPort(String port) {
        getPrimaryRegistrationServer().getSetting(PORT).setValue(port);
    }
    
    private Setting getRegistration() {
        return getRoot().getSetting(REGISTRATION);
    }
    
    private Setting getPrimaryRegistrationServer() {
        return getRegistration().getSetting(SERVER).getSetting(FIRST);
    }
    
    public String getUserId() {
        return getRegistration().getSetting(ADDRESS).getValue();
    }

    public void setUserId(String userId) {
        getRegistration().getSetting(ADDRESS).setValue(userId);
    }

    public String getDisplayName() {
        return getRegistration().getSetting(DISPLAY_NAME).getValue();
    }

    public void setDisplayName(String userId) {
        getRegistration().getSetting(DISPLAY_NAME).setValue(userId);
    }

    public static int getSipPort(String portString) {
        int port = DEFAULT_SIP_PORT;
        if (StringUtils.isNotBlank(portString)) {
            try {
                port = Integer.parseInt(portString);
            } catch (NumberFormatException badPortNonFatal) {
                new Exception("Bad port number " + portString, badPortNonFatal).printStackTrace();
            }
        }

        return port;
    }
        
    /**
     * Doesn't include Display name or angle bracket, 
     * e.g. sip:user@blah.com, not "User Name"&lt;sip:user@blah.com&gt; 
     * NOTE: Unlike request URIs for REGISTER, this apparently requires the user
     * portion.  NOTE: I found this out thru trial and error.
     */
    public String getNotifyRequestUri() {
        StringBuffer sb = new StringBuffer();        
        sb.append("sip:").append(getUserId());
        sb.append('@').append(getPrimaryRegistrationServerAddress());
        String port = getPrimaryRegistrationServerPort();
        if (StringUtils.isNotBlank(port)) {
            sb.append(':').append(port);
        }
        
        return sb.toString();
    }

    public RenderProperties getRenderProperties(Setting setting) {
        RenderProperties props = null;
        
        if (setting.getName().endsWith(".password")) {
            props = RenderProperties.createPasswordField();
        }

        return props;
    }
}