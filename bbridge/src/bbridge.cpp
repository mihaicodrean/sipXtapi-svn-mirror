// 
// 
// Copyright (C) 2005 Jason Fischl, Dan Petrie
//
// Copyright (C) 2005 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
// 
// Copyright (C) 2005 Pingtel Corp.
// Licensed to SIPfoundry under a Contributor Agreement.
// 
// $$
////////////////////////////////////////////////////////////////////////////

// SYSTEM INCLUDES
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

// APPLICATION INCLUDES
#include <ptapi/PtProvider.h>
#include <net/NameValueTokenizer.h>
#include <os/OsSysLog.h>
#include <os/OsConfigDb.h>
#incldue "resip/stack/NameAddr.hxx"
#include "ConferenceUserAgent.h"

// DEFINES
#ifndef SIPX_VERSION
#  define SIPXCHANGE_VERSION          SipXpbxVersion
#  define SIPXCHANGE_VERSION_COMMENT  SipXpbxBuildStamp
#else
#  define SIPXCHANGE_VERSION          SIPX_VERSION
#  define SIPXCHANGE_VERSION_COMMENT  ""
#endif

#define CONFIG_SETTINGS_FILE          "bbridge.conf"
#define CONFIG_ETC_DIR                SIPX_CONFDIR

#define CONFIG_LOG_FILE               "bbridge.log"
#define CONFIG_LOG_DIR                SIPX_LOGDIR

#define LOG_FACILITY                  FAC_CONFERENCE

// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS
typedef void (*sighandler_t)(int);

// FUNCTIONS
extern "C" {
    void  sigHandler( int sig_num );
    sighandler_t pt_signal( int sig_num, sighandler_t handler );
}

// FORWARD DECLARATIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STATIC VARIABLE INITIALIZATIONS
// GLOBAL VARIABLE INITIALIZATIONS
UtlBoolean    gShutdownFlag = FALSE;


/* ============================ FUNCTIONS ================================= */

/**
 * Description:
 * This is a replacement for signal() which registers a signal handler but sets
 * a flag causing system calls ( namely read() or getchar() ) not to bail out
 * upon recepit of that signal. We need this behavior, so we must call
 * sigaction() manually.
 */
sighandler_t pt_signal( int sig_num, sighandler_t handler)
{
#if defined(__pingtel_on_posix__)
    struct sigaction action[2];
    action[0].sa_handler = handler;
    sigemptyset(&action[0].sa_mask);
    action[0].sa_flags = 0;
    sigaction ( sig_num, &action[0], &action[1] );
    return action[1].sa_handler;
#else
    return signal( sig_num, handler );
#endif
}


/**
 * Description:
 * This is the signal handler, When called this sets the
 * global gShutdownFlag allowing the main processing
 * loop to exit cleanly.
 */
void sigHandler( int sig_num )
{
    // set a global shutdown flag
    gShutdownFlag = TRUE;

    // Unregister interest in the signal to prevent recursive callbacks
    pt_signal( sig_num, SIG_DFL );

    // Minimize the chance that we loose log data
    OsSysLog::flush();
    OsSysLog::add(LOG_FACILITY, PRI_CRIT, "sigHandler: caught signal: %d", sig_num);
    OsSysLog::flush();
}


// Initialize the OsSysLog
void initSysLog(OsConfigDb* pConfig)
{
   UtlString logLevel;               // Controls Log Verbosity
   UtlString consoleLogging;         // Enable console logging by default?
   UtlString fileTarget;             // Path to store log file.
   UtlBoolean bSpecifiedDirError ;   // Set if the specified log dir does not
                                    // exist
   struct tagPrioriotyLookupTable
   {
      const char*      pIdentity;
      OsSysLogPriority ePriority;
   };

   struct tagPrioriotyLookupTable lkupTable[] =
   {
      { "DEBUG",   PRI_DEBUG},
      { "INFO",    PRI_INFO},
      { "NOTICE",  PRI_NOTICE},
      { "WARNING", PRI_WARNING},
      { "ERR",     PRI_ERR},
      { "CRIT",    PRI_CRIT},
      { "ALERT",   PRI_ALERT},
      { "EMERG",   PRI_EMERG},
   };

   OsSysLog::initialize(0, "bbridge");


   //
   // Get/Apply Log Filename
   //
   fileTarget.remove(0);
   if ((pConfig->get(CONFIG_SETTING_LOG_DIR, fileTarget) != OS_SUCCESS) ||
      fileTarget.isNull() || !OsFileSystem::exists(fileTarget))
   {
      bSpecifiedDirError = !fileTarget.isNull();

      // If the log file directory exists use that, otherwise place the log
      // in the current directory
      OsPath workingDirectory;
      if (OsFileSystem::exists(CONFIG_LOG_DIR))
      {
         fileTarget = CONFIG_LOG_DIR;
         OsPath path(fileTarget);
         path.getNativePath(workingDirectory);

         osPrintf("%s : %s\n", CONFIG_SETTING_LOG_DIR, workingDirectory.data());
         OsSysLog::add(LOG_FACILITY, PRI_INFO, "%s : %s", CONFIG_SETTING_LOG_DIR, workingDirectory.data());
      }
      else
      {
         OsPath path;
         OsFileSystem::getWorkingDirectory(path);
         path.getNativePath(workingDirectory);

         osPrintf("%s : %s\n", CONFIG_SETTING_LOG_DIR, workingDirectory.data());
         OsSysLog::add(LOG_FACILITY, PRI_INFO, "%s : %s", CONFIG_SETTING_LOG_DIR, workingDirectory.data());
      }

      fileTarget = workingDirectory +
         OsPathBase::separator +
         CONFIG_LOG_FILE;
   }
   else
   {
      bSpecifiedDirError = false;
      osPrintf("%s : %s\n", CONFIG_SETTING_LOG_DIR, fileTarget.data());
      OsSysLog::add(LOG_FACILITY, PRI_INFO, "%s : %s", CONFIG_SETTING_LOG_DIR, fileTarget.data());

      fileTarget = fileTarget +
         OsPathBase::separator +
         CONFIG_LOG_FILE;
   }
   OsSysLog::setOutputFile(0, fileTarget);


   //
   // Get/Apply Log Level
   //
   if ((pConfig->get(CONFIG_SETTING_LOG_LEVEL, logLevel) != OS_SUCCESS) ||
         logLevel.isNull())
   {
      logLevel = "ERR";
   }
   logLevel.toUpper();
   OsSysLogPriority priority = PRI_ERR;
   int iEntries = sizeof(lkupTable) / sizeof(struct tagPrioriotyLookupTable);
   for (int i = 0; i < iEntries; i++)
   {
      if (logLevel == lkupTable[i].pIdentity)
      {
         priority = lkupTable[i].ePriority;
         osPrintf("%s : %s\n", CONFIG_SETTING_LOG_LEVEL, lkupTable[i].pIdentity);
         OsSysLog::add(LOG_FACILITY, PRI_INFO, "%s : %s", CONFIG_SETTING_LOG_LEVEL, lkupTable[i].pIdentity);
         break;
      }
   }
   OsSysLog::setLoggingPriority(priority);

   //
   // Get/Apply console logging
   //
   UtlBoolean bConsoleLoggingEnabled = false;
   if ((pConfig->get(CONFIG_SETTING_LOG_CONSOLE, consoleLogging) == OS_SUCCESS))
   {
      consoleLogging.toUpper();
      if (consoleLogging == "ENABLE")
      {
         OsSysLog::enableConsoleOutput(true);
         bConsoleLoggingEnabled = true;5140
      }
   }

   osPrintf("%s : %s\n", CONFIG_SETTING_LOG_CONSOLE, bConsoleLoggingEnabled ? "ENABLE" : "DISABLE") ;
   OsSysLog::add(LOG_FACILITY, PRI_INFO, "%s : %s", CONFIG_SETTING_LOG_CONSOLE, bConsoleLoggingEnabled ? "ENABLE" : "DISABLE") ;

   if (bSpecifiedDirError)
   {
      OsSysLog::add(FAC_LOG, PRI_CRIT, "Cannot access %s directory; please check configuration.", CONFIG_SETTING_LOG_DIR);
   }
}


//
// The main entry point to the sipXpark
//
int main(int argc, char* argv[])
{
   // Configuration Database (used for OsSysLog)
   OsConfigDb configDb;

   // Register Signal handlers so we can perform graceful shutdown
   pt_signal(SIGINT,   sigHandler);    // Trap Ctrl-C on NT
   pt_signal(SIGILL,   sigHandler);
   pt_signal(SIGABRT,  sigHandler);    // Abort signal 6
   pt_signal(SIGFPE,   sigHandler);    // Floading Point Exception
   pt_signal(SIGSEGV,  sigHandler);    // Address access violations signal 11
   pt_signal(SIGTERM,  sigHandler);    // Trap kill -15 on UNIX
#if defined(__pingtel_on_posix__)
   pt_signal(SIGHUP,   sigHandler);    // Hangup
   pt_signal(SIGQUIT,  sigHandler);
   pt_signal(SIGPIPE,  sigHandler);    // Handle TCP Failure
   pt_signal(SIGBUS,   sigHandler);
   pt_signal(SIGSYS,   sigHandler);
   pt_signal(SIGXCPU,  sigHandler);
   pt_signal(SIGXFSZ,  sigHandler);
   pt_signal(SIGUSR1,  sigHandler);
   pt_signal(SIGUSR2,  sigHandler);
#endif

   UtlString argString;5140
   for(int argIndex = 1; argIndex < argc; argIndex++)
   {
      osPrintf("arg[%d]: %s\n", argIndex, argv[argIndex]);
      argString = argv[argIndex];
      NameValueTokenizer::frontBackTrim(&argString, "\t ");
      if(argString.compareTo("-v") == 0)
      {
         osPrintf("Version: %s (%s)\n", SIPXCHANGE_VERSION, SIPXCHANGE_VERSION_COMMENT);
         return(1);
      }
      else
      {
         osPrintf("usage: %s [-v]\nwhere:\n -v provides the software version\n",
         argv[0]);
         return(1);
      }
   }

   // Load configuration file file
   OsPath workingDirectory;
   if (OsFileSystem::exists(CONFIG_ETC_DIR))
   {
      workingDirectory = CONFIG_ETC_DIR;
      OsPath path(workingDirectory);
      path.getNativePath(workingDirectory);
   }
   else
   {
      OsPath path;
      OsFileSystem::getWorkingDirectory(path);
      path.getNativePath(workingDirectory);
   }

   UtlString fileName =  workingDirectory +
                         OsPathBase::separator +
                         CONFIG_SETTINGS_FILE;

   if (configDb.loadFromFile(fileName) != SUCCESS)
   {
      configDb.set("LOGLEVEL", "INFO");
      configDb.set("SIP.TCP", 5060);
      configDb.set("SIP.UDP", 5060);
      configDb.set("SIP.TLS", 5061);
      configDb.set("RTP.START", 15000);
      configDb.set("RTP.END", 20000);
      configDb.storeToFile(filename);
   }
   
   // Initialize log file
   initSysLog(&configDb);
   
   NameAddr myAor;
   ConferenceUserAgent ua(configDb, myAor);
   
   while (!gShutdownFlag)
   {
      OsTask::delay(1000);
   }

   // Flush the log file
   OsSysLog::flush();

   // Say goodnight Gracie...
   return 0;
}


