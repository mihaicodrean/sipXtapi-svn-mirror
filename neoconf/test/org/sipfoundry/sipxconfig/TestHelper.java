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
package org.sipfoundry.sipxconfig;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.security.CodeSource;
import java.sql.Connection;
import java.sql.DriverManager;
import java.util.Properties;

import org.apache.velocity.app.VelocityEngine;
import org.dbunit.database.DatabaseConfig;
import org.dbunit.database.DatabaseConnection;
import org.dbunit.database.IDatabaseConnection;
import org.dbunit.dataset.IDataSet;
import org.dbunit.dataset.xml.FlatDtdDataSet;
import org.dbunit.dataset.xml.FlatXmlDataSet;
import org.dbunit.dataset.xml.XmlDataSet;
import org.dbunit.operation.DatabaseOperation;
import org.springframework.context.ApplicationContext;
import org.springframework.context.support.ClassPathXmlApplicationContext;

/**
 * For unittests that need spring instantiated
 */
public final class TestHelper {
    
    private static final String APPLICATION_CONTEXT_FILE = "org/sipfoundry/sipxconfig/applicationContext-sipxconfig.xml";
    
    private static Properties s_sysProps;
    
    private static ApplicationContext s_appContext;
    
    private static String P6SPY = "com.p6spy.engine.spy.P6SpyDriver";
    
    private static String POSTGRES = "org.postgresql.Driver";
    
    static {
        // default XML parser (crimson) cannot resolve relative DTDs, google for bug
        System.setProperty("org.xml.sax.driver", "org.apache.xerces.parsers.SAXParser");
    }

    public static ApplicationContext getApplicationContext() {
        if (s_appContext == null) {
            getSysDirProperties();
            s_appContext = new ClassPathXmlApplicationContext(
                TestHelper.APPLICATION_CONTEXT_FILE);
        }
        
        return s_appContext;
    }
    
    public static String getTestDirectory() {
        return getSysDirProperties().getProperty("sysdir.var");
    }

    public static VelocityEngine getVelocityEngine() throws Exception {
        Properties sysdir = getSysDirProperties();

        String etcDir = sysdir.getProperty("sysdir.etc");

        VelocityEngine engine = new VelocityEngine();
        engine.setProperty("resource.loader", "file");
        engine.setProperty("file.resource.loader.path", etcDir);
        engine.init();
        
        return engine;
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
	        CodeSource code = TestHelper.class.getProtectionDomain().getCodeSource();
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
	        String userDir = System.getProperty("user.dir");
	        String etcDir = System.getProperty("basedir", userDir) + "/etc";
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
    
    public static IDatabaseConnection getConnection() throws Exception {
        // Could optionally get this from Spring
        //    DataSource.getConnection()
        // may pool connections and be faster.
        // Class driverClass = Class.forName(POSTGRES);
        
        //  dumps sql commands to log file spy.log in working directory
        Class driverClass = Class.forName(P6SPY);  
        
        Connection jdbcConnection = DriverManager.getConnection(
                "jdbc:postgresql://localhost/PDS", "postgres", "");        
        DatabaseConnection dbunitConnection = new DatabaseConnection(jdbcConnection);
        DatabaseConfig config = dbunitConnection.getConfig();
        config.setFeature("http://www.dbunit.org/features/batchedStatements", true);
        
        return dbunitConnection;
    }
    
    public static void main(String[] args) {
        try {
            //generateDbDtd();
            generateDbXml();
        } catch (Exception e) {
            e.printStackTrace();
            System.exit(1);
        }
    }
    
    private static void generateDbDtd() throws Exception {
        IDatabaseConnection c = getConnection();
        
        FlatDtdDataSet.write(c.createDataSet(),
            new FileOutputStream("test/org/sipfoundry/sipxconfig/sipxconfig-dataset.dtd"));
        
    }
    
    private static void generateDbXml() throws Exception {
        IDatabaseConnection c = getConnection();
        
        XmlDataSet.write(c.createDataSet(),
            new FileOutputStream("test/org/sipfoundry/sipxconfig/sipxconfig-dataset.xml"));
    }
    
    public static IDataSet loadDataSet(Class c, String fileResource) throws Exception {
        InputStream datasetStream = c.getResourceAsStream(fileResource);
        return new XmlDataSet(datasetStream);
    }
    
    public static IDataSet loadDataSetFlat(Class c, String resource) throws Exception {
        InputStream datasetStream = c.getResourceAsStream(resource);
        return new FlatXmlDataSet(datasetStream);
    }
    
    public static void cleanInsert(Class c, String resource) throws Exception {
        DatabaseOperation.CLEAN_INSERT.execute(getConnection(), loadDataSet(c, resource));
    }

    public static void cleanInsertFlat(Class c, String resource) throws Exception {
        DatabaseOperation.CLEAN_INSERT.execute(getConnection(), loadDataSetFlat(c, resource));
    }

    public static void insertFlat(Class c, String resource) throws Exception {
        DatabaseOperation.INSERT.execute(getConnection(), loadDataSetFlat(c, resource));
    }
}
