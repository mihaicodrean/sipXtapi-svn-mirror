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
import java.util.Collections;
import java.util.List;

import junit.framework.TestCase;

import org.sipfoundry.sipxconfig.admin.dialplan.config.Transform;
import org.sipfoundry.sipxconfig.admin.dialplan.config.UrlTransform;

/**
 * EmergencyRuleTest
 */
public class EmergencyRuleTest extends TestCase {
    private EmergencyRule m_rule;

    protected void setUp() throws Exception {
        m_rule = new EmergencyRule();
        m_rule.setEmergencyNumber("911");
        m_rule.setOptionalPrefix("9");

        Gateway g = new Gateway();
        g.setAddress("sosgateway.com");
        m_rule.setGateways(Collections.singletonList(g));
    }

    public void testGetPatterns() {
        String[] patterns = m_rule.getPatterns();
        assertEquals(3, patterns.length);
        assertEquals("sos", patterns[0]);
        assertEquals("911", patterns[1]);
        assertEquals("9911", patterns[2]);
    }

    public void testGetTransforms() {
        Transform[] transforms = m_rule.getTransforms();
        assertEquals(1, transforms.length);
        UrlTransform emergencyTransform = (UrlTransform) transforms[0];
        assertEquals("sip:911@sosgateway.com", emergencyTransform.getUrl());
    }

    public void testCallerSensitiveForwarding() {
        m_rule.setUseMediaServer(true);
        Transform[] transforms = m_rule.getTransforms();
        assertEquals(1, transforms.length);
        UrlTransform emergencyTransform = (UrlTransform) transforms[0];
        assertEquals(
                "<sip:{digits}@{mediaserver};play={voicemail}%2Fcgi-bin%2Fvoicemail%2Fmediaserver.cgi%3Faction%3Dsos>",
                emergencyTransform.getUrl());
    }

    public void testPermissions() {
        List permissions = m_rule.getPermissions();
        assertEquals(0, permissions.size());
    }

    public void testAppendToGenerationRulesEnabled() {
        m_rule.setEnabled(true);
        List gateways = m_rule.getGateways();
        assertEquals(1, gateways.size());
        ArrayList rules = new ArrayList();
        m_rule.appendToGenerationRules(rules);
        assertEquals(1, rules.size());
        DialingRule rule = (DialingRule) rules.get(0);
        assertEquals(1, rule.getGateways().size());

        // if we are using media server return empty gateways list
        m_rule.setUseMediaServer(true);
        gateways = m_rule.getGateways();
        assertEquals(1, gateways.size());
        rules = new ArrayList();
        m_rule.appendToGenerationRules(rules);
        assertEquals(1, rules.size());
        rule = (DialingRule) rules.get(0);
        assertTrue(rule.getGateways().isEmpty());
    }

    public void testAppendToGenerationRulesDisabled() {
        m_rule.setEnabled(false);
        m_rule.setUseMediaServer(true);
        ArrayList rules = new ArrayList();
        m_rule.appendToGenerationRules(rules);
        assertTrue(rules.isEmpty());
    }
}
