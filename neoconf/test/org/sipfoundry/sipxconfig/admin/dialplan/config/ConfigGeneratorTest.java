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

import java.io.File;
import java.io.FileInputStream;
import java.io.StringWriter;

import org.custommonkey.xmlunit.XMLTestCase;
import org.custommonkey.xmlunit.XMLUnit;
import org.dom4j.Document;
import org.sipfoundry.sipxconfig.admin.dialplan.FlexibleDialPlan;

/**
 * ConfigGeneratorTest
 */
public class ConfigGeneratorTest extends XMLTestCase {
    public ConfigGeneratorTest() {
        XMLUnit.setIgnoreWhitespace(true);
    }

    public void testGetFileContent() throws Exception {
        FlexibleDialPlan empty = new FlexibleDialPlan();
        ConfigGenerator generator = new ConfigGenerator();
        generator.generate(empty);

        checkConfigFileGeneration(generator, new AuthRules(), ConfigFileType.AUTH_RULES);
        checkConfigFileGeneration(generator, new MappingRules(), ConfigFileType.MAPPING_RULES);
        checkConfigFileGeneration(generator, new FallbackRules(), ConfigFileType.FALLBACK_RULES);
    }

    /**
     * Execute test for a single configuration type. Tries to generate it
     * directly and generate pretty formatted text through generator.
     */
    private void checkConfigFileGeneration(ConfigGenerator generator, ConfigFile configFile,
            ConfigFileType type) throws Exception {
        Document document = configFile.getDocument();
        StringWriter writer = new StringWriter();
        document.write(writer);
        String xml = generator.getFileContent(type);
        assertXMLEqual("Comparing: " + type, writer.getBuffer().toString(), xml);
    }
    
    public void testActivate() throws Exception {
        FlexibleDialPlan empty = new FlexibleDialPlan();
        ConfigGenerator generator = new ConfigGenerator();
        generator.generate(empty);
        File x = File.createTempFile("test", "in");
        String tmpDir = x.getParent();
        x.deleteOnExit();
        generator.activate(tmpDir);
        
        File[] files = new File[3];
        files[0] = new File(tmpDir, "mappingrules.xml.in.new");
        files[1] = new File(tmpDir, "fallbackrules.xml.in.new");
        files[2] = new File(tmpDir, "authrules.xml.in.new");
        
        for (int i = 0; i < files.length; i++) {
            File file = files[i];
            assertTrue( file.canRead() );
            FileInputStream stream = new FileInputStream(file);
            assertEquals( '<', stream.read() );
            stream.close();
            file.deleteOnExit();
        }
    }
}
