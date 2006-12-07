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
package org.sipfoundry.sipxconfig.site.phone;

import junit.framework.Test;
import net.sourceforge.jwebunit.WebTestCase;

import org.sipfoundry.sipxconfig.site.SiteTestHelper;

public class NewPhoneTestUi extends WebTestCase {

    private PhoneTestHelper m_helper;

    public static Test suite() throws Exception {
        return SiteTestHelper.webTestSuite(NewPhoneTestUi.class);
    }

    protected void setUp() throws Exception {
        super.setUp();
        getTestContext().setBaseUrl(SiteTestHelper.getBaseUrl());
        SiteTestHelper.home(getTester());
        m_helper = new PhoneTestHelper(tester);
        m_helper.reset();
    }

    protected void tearDown() throws Exception {
        super.tearDown();
    }

    public void testAddPhone() {
        clickLink("NewPhone");
        setFormElement("serialNumber", "000000000000");
        clickButton("form:ok");
        String[][] table = new String[][] {
            {
                "000000000000", "", "Acme"
            },
        };
        assertTextInTable("phone:list", table[0]);
    }

    public void testSaveAndStay() {
        clickLink("NewPhone");
        setFormElement("serialNumber", "000000000000");
        checkCheckbox("stay");
        clickButton("form:ok");
        assertCheckboxSelected("stay");
        // should clear the form
        assertEquals("", getDialog().getFormParameterValue("serialNumber"));

        clickButton("form:cancel");
        String[][] table = new String[][] {
            {
                "000000000000", "", "Acme"
            },
        };
        assertTextInTable("phone:list", table[0]);
    }

    public void testInvalidSerialNumber() {
        clickLink("NewPhone");

        // no digits
        clickButton("form:ok");
        SiteTestHelper.assertUserError(tester);

        // wrong chars and wrong number
        setFormElement("serialNumber", "x");
        clickButton("form:ok");
        SiteTestHelper.assertUserError(tester);

        // 12 digits, but not valid chars
        setFormElement("serialNumber", "123456789abx");
        clickButton("form:ok");
        SiteTestHelper.assertUserError(tester);

        // 16 correct digits - is OK - we accept 12 or more now
        setFormElement("serialNumber", "123456789abcdef");
        clickButton("form:ok");
        SiteTestHelper.assertNoUserError(tester);

        SiteTestHelper.home(getTester());
        clickLink("NewPhone");
        // finally got it right
        setFormElement("serialNumber", "123456789abc");
        clickButton("form:ok");
        SiteTestHelper.assertNoUserError(tester);
    }
}
