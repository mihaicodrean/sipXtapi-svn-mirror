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
package org.sipfoundry.sipxconfig.site;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.net.URL;
import java.security.CodeSource;
import java.util.Properties;

import org.springframework.context.ApplicationContext;
import org.springframework.context.support.ClassPathXmlApplicationContext;

public class SiteTestHelper {
    
    private static final String APPLICATION_CONTEXT_FILE = "org/sipfoundry/sipxconfig/applicationContext-sipxconfig.xml";
    
    private static Properties s_sysProps;
    
    private static ApplicationContext s_appContext;
    
    public static ApplicationContext getApplicationContext() {
        if (s_appContext == null) {
            getSysDirProperties();
            s_appContext = new ClassPathXmlApplicationContext(
                    SiteTestHelper.APPLICATION_CONTEXT_FILE);
        }
        
        return s_appContext;
    }        

    /**
     * Create a sysdir.properties file in the classpath.  Uses a trick that
     * will only work if unittests are unjar-ed.  This is infavor of doing
     * in ant because it avoids setup and works in IDE's like eclipse where
     * bin.eclipse is the classpath
     */
    public static Properties getSysDirProperties() {
        if (s_sysProps == null) {
	        // create file on classpath
	        CodeSource code = SiteTestHelper.class.getProtectionDomain().getCodeSource();
	        URL classpathUrl = code.getLocation();
	        File classpathDir = new File(classpathUrl.getFile());
	        File sysdirPropsFile = new File(classpathDir, "sysdir.properties");
	        FileOutputStream sysdirPropsStream;
	        try {
	            sysdirPropsStream = new FileOutputStream(sysdirPropsFile);
	        } catch (FileNotFoundException e) {
	            throw new RuntimeException("could not create system dir properties file", e);
	        }
	        s_sysProps = new Properties();
	        
	        String baseDir = System.getProperty("basedir");
	        String etcDir = System.getProperty("user.dir", baseDir) + "/../etc";
	        s_sysProps.setProperty("sysdir.etc", etcDir);
	        s_sysProps.setProperty("sysdir.var", classpathDir.getAbsolutePath());
	        s_sysProps.setProperty("sysdir.log", classpathDir.getAbsolutePath());
	        
	        try {
	            // store them so spring's application context file find it
	            // in classpath
	            s_sysProps.store(sysdirPropsStream, null);
	        } catch (IOException e) {
	            throw new RuntimeException("could not store system dir properties", e);
	        }
        }
        
        return s_sysProps;
    }
}
