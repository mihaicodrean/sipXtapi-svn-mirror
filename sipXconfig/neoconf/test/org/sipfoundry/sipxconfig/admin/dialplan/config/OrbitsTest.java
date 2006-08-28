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
package org.sipfoundry.sipxconfig.admin.dialplan.config;

import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.StringReader;
import java.util.ArrayList;
import java.util.List;

import org.custommonkey.xmlunit.XMLTestCase;
import org.custommonkey.xmlunit.XMLUnit;
import org.sipfoundry.sipxconfig.TestHelper;
import org.sipfoundry.sipxconfig.admin.parkorbit.BackgroundMusic;
import org.sipfoundry.sipxconfig.admin.parkorbit.ParkOrbit;
import org.sipfoundry.sipxconfig.common.TestUtil;

public class OrbitsTest extends XMLTestCase {
    private List<ParkOrbit> m_parkOrbits;

    private static String[][] DATA = {
        {
            "disable", "101", "x.wav", "this one should not be generated"
        }, {
            "sales", "400", "basia.wav", "test description"
        }, {
            "CEO orbit", "179", "marketing.wav", null
        }
    };

    protected void setUp() throws Exception {
        XMLUnit.setIgnoreWhitespace(true);

        m_parkOrbits = new ArrayList<ParkOrbit>(DATA.length);
        for (int i = 0; i < DATA.length; i++) {
            ParkOrbit orbit = new ParkOrbit();
            orbit.setModelFilesContext(TestHelper.getModelFilesContext());
            orbit.setName(DATA[i][0]);
            orbit.setExtension(DATA[i][1]);
            orbit.setMusic(DATA[i][2]);
            orbit.setDescription(DATA[i][3]);
            // the first one is disabled
            // the remaining orbits will be enabled
            orbit.setEnabled(i > 0);
            m_parkOrbits.add(orbit);
        }

        // make 3rd orbit more interesting

        ParkOrbit orbit = m_parkOrbits.get(2);
        orbit.setSettingValue("general/enableTimeout", "1");
        orbit.setSettingValue("general/parkTimeout", "150");
        orbit.setSettingValue("general/allowTransfer", "1");
        orbit.setSettingValue("general/transferKey", "9");
        orbit.setSettingValue("general/multipleCalls", "1");

    }

    public void testGenerate() throws Exception {
        Orbits orbits = new Orbits();
        orbits.setAudioDirectory("/var/sipxdata/parkserver/music");
        orbits.generate(new BackgroundMusic(), m_parkOrbits);
        String generatedXml = orbits.getFileContent();
        // orbits file on windows includes "c:/" etc
        generatedXml = generatedXml.replaceAll(TestUtil.currentDrive(), "");
        generatedXml = generatedXml.replace('\\', '/');
        InputStream referenceXml = OrbitsTest.class.getResourceAsStream("orbits.test.xml");
        assertXMLEqual(new InputStreamReader(referenceXml), new StringReader(generatedXml));
    }
}
