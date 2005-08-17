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
package org.sipfoundry.sipxconfig.login;

import junit.framework.TestCase;

import org.easymock.MockControl;
import org.sipfoundry.sipxconfig.common.CoreContext;
import org.sipfoundry.sipxconfig.common.User;

public class LoginContextImplTest extends TestCase {
    private LoginContextImpl m_impl;
    private User m_user;

    protected void setUp() throws Exception {
        m_impl = new LoginContextImpl();

        m_user = new User();

        MockControl control = MockControl.createNiceControl(CoreContext.class);
        CoreContext coreContext = (CoreContext) control.getMock();
        coreContext.loadUserByUserName("superadmin");
        control.setReturnValue(m_user, 3);

        coreContext.getAuthorizationRealm();
        control.setReturnValue("pingtel.com", 3);
        control.replay();

        m_impl.setCoreContext(coreContext);
    }

    public void testCheckCredentials() {
        m_user.setUserName("superadmin");
        m_user.setPintoken("e3e367205de83ab477cdf3449f152791");

        // password OK
        assertSame(m_user, m_impl.checkCredentials("superadmin", "1234"));

        // MD5 hash passed - should fail
        assertNull(m_impl.checkCredentials("superadmin", "e3e367205de83ab477cdf3449f152791"));

        // invalid password
        assertNull(m_impl.checkCredentials("superadmin", "123"));
    }

    public void testCheckCredentialsInvalid() {
        m_user.setUserName("superadmin");
        m_user.setPintoken("kkk");

        assertNull(m_impl.checkCredentials("superadmin", "zzz"));

        // invalid user - good password
        assertNull(m_impl.checkCredentials("xxx", "kkk"));

    }

    public void testIsAdmin() {
        m_user.setUserName("superadmin");
        assertTrue(m_impl.isAdmin(m_user));

        m_user.setUserName("xyz");
        assertFalse(m_impl.isAdmin(m_user));
    }
}
